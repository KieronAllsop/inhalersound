#include "WAVData.h"

#include "Global.h"
#include "Logger.h"
#include "SpecAnalysis.h"

WAVData::WAVData()
{
}

WAVData::~WAVData()
{
}

void 
WAVData::ExtractToBuffer(const std::string& fileName, BufferedASignal* buffer)
{
	std::ifstream infile(fileName, std::ios::binary);

	if (!infile)
	{
		std::string err = "WAVData: Specified WAV file: \"" + fileName + "\" does not exist or is inaccessible";
		Logger(LOG_ERROR) << err;
		throw std::runtime_error(err);
	}

	TSpecAnalysis SpecAnalysis = TSpecAnalysis();

	WAV wav = ReadyToReadWAVData(infile);

	if (wav.SamplingRate != GlobalSettings.DecodingSamplingRate)
	{
		std::stringstream ss;
		ss << "The sampling rate of file: " << fileName << ": " << wav.SamplingRate << 
			" does not match specified rate of " << GlobalSettings.DecodingSamplingRate << std::endl;
		Logger(LOG_WARN) << ss.str();
	}

	int nFrames = NumFramesFromNumSamples(wav.nSamples);

	buffer->SumFrames = nFrames;

	// Need to read extra samples at start and end to allow delta and delta-delta values to be calculated from previous and 
	// next frames at the start and end of current chunk
	int deltaExtra = (GlobalSettings.SpeechAnalysisParamaters.Delta > GlobalSettings.SpeechAnalysisParamaters.DeltaDelta) ? 
		(GlobalSettings.SpeechAnalysisParamaters.Delta) : (GlobalSettings.SpeechAnalysisParamaters.DeltaDelta);
	deltaExtra *= 2;

	if (GlobalSettings.DecodingElementSize < (deltaExtra * 2))
	{
		// If it's too small it'll cause issues trying to read extra samples at beginning and end to ensure 
		// that delta and delta-delta values are still calculated correctly
		std::string err = "WAVData: Specified \"Decoding.ElementSize\" setting value too small, must be at least: " + (deltaExtra * 2);
		Logger(LOG_ERROR) << err;
		throw std::runtime_error(err);
	}

	ASegment chunk = ASegment();
	unsigned int aimChunkSize = GlobalSettings.DecodingElementSize;
	unsigned int remaining = nFrames;
	unsigned int numChunks = 
		(nFrames % aimChunkSize == 0) ? (nFrames / aimChunkSize) : ((nFrames / aimChunkSize) + 1);
	unsigned int count = 0;
	unsigned int remainingSamples = wav.nSamples;

	while (count < numChunks)
	{
		try
		{
			buffer->BeginProducingChunk();

			unsigned int currentChunkSize = (remaining >= aimChunkSize) ? (aimChunkSize) : (remaining % aimChunkSize);
			unsigned int currentChunkNumSamples = NumSamplesFromNumFrames(currentChunkSize);
			chunk.nFrames = currentChunkSize;

			// Read WAV segment
			WAV wavSegment = WAV();
			ReadWAVSegment(infile, wavSegment, (wav.nSamples - remainingSamples), wav.nSamples, currentChunkNumSamples, deltaExtra);
			wavSegment.SamplingRate = wav.SamplingRate;

			// Spectral analysis on segment
			chunk.Frames = SpecAnalysis.Execute(wavSegment);

			int numExtra = chunk.Frames.size() - currentChunkSize;

			// Erase extra frames at end and start
			if (count == 0)
			{
				// Only extra frames at end
				if (numExtra > 0)
				{
					chunk.Frames.erase(chunk.Frames.begin() + currentChunkSize, chunk.Frames.begin() + chunk.Frames.size());
				}
			}
			else if (count == numChunks - 1)
			{
				// Only at start
				if (numExtra > 0)
				{
					chunk.Frames.erase(chunk.Frames.begin(), chunk.Frames.begin() + numExtra);
				}
			}
			else
			{
				// Extra frames at start and end
				if (numExtra > 0)
				{
					int numExtraPerEnd = numExtra / 2;
					int numExtraStart = numExtraPerEnd;
					int numExtraEnd = numExtraPerEnd;
					if (deltaExtra < (numExtraPerEnd))
					{
						numExtraStart = deltaExtra;
						numExtraEnd = ((numExtra - deltaExtra) > 0) ? (numExtra - deltaExtra) : 0;
					}
					// End
					chunk.Frames.erase(chunk.Frames.begin() + (chunk.Frames.size() - numExtraEnd), 
						chunk.Frames.begin() + chunk.Frames.size());
					// Start
					chunk.Frames.erase(chunk.Frames.begin(), chunk.Frames.begin() + numExtraStart);
				}
			}

			std::stringstream ss;
			ss << "Read chunk " << (count + 1) << " of " << numChunks << std::endl;
			Logger(LOG_DEBUG) << ss.str();

			buffer->ProduceChunk(chunk);

			count++;
			remaining -= chunk.nFrames;
			remainingSamples -= currentChunkNumSamples;
		}
		catch(boost::thread_interrupted& e)
		{
			infile.close();
			Logger(LOG_ERROR) << "WAVData: Feature loading halted." << std::endl;
			return;
		}
	}

	infile.close();
	Logger(LOG_INFO) << "Finished WAV feature extraction" << std::endl;
}

WAV 
WAVData::ReadFullWAV(const std::string& fileName)
{
	std::ifstream infile(fileName, std::ios::binary);

	if (!infile)
        {
                throw std::runtime_error("WAVData: Specified WAV file: \"" + fileName + "\" does not exist or is inaccessible");
        }

	WAV wav = ReadyToReadWAVData(infile);
	ReadWAVSegment(infile, wav, 0, wav.nSamples, wav.nSamples, 0);

	infile.close();

	return wav;
}

WAV 
WAVData::ReadyToReadWAVData(std::ifstream& infile)
{
	ChunkHeader chunkHeader = ChunkHeader();
	WaveFormatChunk waveFormatChunk = WaveFormatChunk();

	if (!ReadHeader(infile, chunkHeader, waveFormatChunk))
        {
                throw std::runtime_error("WAVData: Problem reading WAV, check that format is PCM");
        }

	// Make room
	WAV wav = WAV();
	wav.nSamples = chunkHeader.Len / waveFormatChunk.BytesPerSample;
        wav.Sample = std::vector<short int>();
	wav.Sample.resize(wav.nSamples);

	wav.SamplingRate = waveFormatChunk.Rate;

	return wav;
}

void 
WAVData::ReadWAVSegment(std::ifstream& infile, WAV& buffer, int realPos, int realEnd, int nSamples, short int extraFrames)
{
	int extraSamples = (extraFrames * GlobalSettings.SpeechAnalysisParamaters.Shift);

	int extraToReadStart = (realPos >= extraSamples) ? (extraSamples) : 0;
	int extraToReadEnd = (realPos + nSamples + extraSamples <= realEnd) ? (extraSamples) : 0;

	// Move stream back if need to read extra data at start
	if (extraToReadStart > 0)
	{
		int currentPos = (int)infile.tellg();
		Logger(LOG_TRACE) << "ReadWAVSegment: Start: tellg(): " << currentPos << std::endl;

		int backPos = currentPos - (extraToReadStart * sizeof(short int));
		infile.seekg(backPos);

		Logger(LOG_TRACE) << "ReadWAVSegment: Moved back: tellg(): " << backPos << std::endl;
	}

	buffer.nSamples = nSamples + extraToReadStart + extraToReadEnd;
	buffer.Sample.resize(buffer.nSamples);

	// Read wav data
	infile.read((char*)&buffer.Sample[0], buffer.nSamples * sizeof(short int));

	buffer.Start = 0;
	buffer.End = buffer.nSamples;

	// Move stream position back to before extra data at end so that next segment may read it, taking into 
	// account that it needs to be at the starting position of the next frame to then move back to get 
	// preceding frames
	if (extraToReadEnd > 0)
	{
		int currentPos = (int)infile.tellg();
		Logger(LOG_TRACE) << "ReadWAVSegment: After read: tellg(): " << (int)infile.tellg() << std::endl;

		int backPos = currentPos - (extraToReadEnd * sizeof(short int)) - 
			((GlobalSettings.SpeechAnalysisParamaters.FrmLength - GlobalSettings.SpeechAnalysisParamaters.Shift) * sizeof(short int));
		infile.seekg(backPos);

		Logger(LOG_TRACE) << "ReadWAVSegment: Move back: tellg(): " << backPos << std::endl;
	}
}

bool 
WAVData::ReadHeader(std::ifstream& infile, 
	ChunkHeader& chunkHeader, WaveFormatChunk& waveFormatChunk)
{
	char c;

	// Read RIFF header
	RiffHeader riffHeader = RiffHeader();
	infile.read((char*)&riffHeader, sizeof(RiffHeader));

	if (strncmp(riffHeader.ID, "RIFF", 4) != 0) return false;

	// Read Wave ID
	char waveID[4];
 	infile.read((char*)waveID, 4);

        // Read format chunk header
	infile.read((char*)&chunkHeader, sizeof(ChunkHeader));

        // Read format chunk
        infile.read((char*)&waveFormatChunk, sizeof(WaveFormatChunk));
        chunkHeader.Len -= sizeof(WaveFormatChunk);
        for (unsigned int i = 0; i < chunkHeader.Len; i++)
		infile.get(c);

        // PCM only
        if(waveFormatChunk.FormatTag != 1) return false;

        // Search for data chunk header
	while (true)
	{
 		infile.read((char*)&chunkHeader, sizeof(ChunkHeader));

        if (strncmp(chunkHeader.ID, "data", 4) == 0) 
			break;

     	for (unsigned int i = 0; i < chunkHeader.Len; i++) 
			infile.get(c);
        }

        return true;
}

int 
WAVData::NumFramesFromNumSamples(int nSamples)
{
	return ((nSamples - GlobalSettings.SpeechAnalysisParamaters.FrmLength) / 
		GlobalSettings.SpeechAnalysisParamaters.Shift + 1);
}

int 
WAVData::NumSamplesFromNumFrames(int nFrames)
{
	return (((nFrames - 1) * GlobalSettings.SpeechAnalysisParamaters.Shift) + 
		GlobalSettings.SpeechAnalysisParamaters.FrmLength);
}
