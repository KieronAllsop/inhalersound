#include <iostream>
#include <sstream>

#include "Helper.h"
#include "HTKData.h"


HTKData::HTKData()
{
}

HTKData::~HTKData(void)
{
}

void HTKData::ExtractToBuffer(const std::string& fileName, BufferedASignal* buffer)
{
	std::ifstream infile;
	infile.open(fileName, std::ios::binary | std::ios::in);
	
	if (!infile)
		throw std::runtime_error("HTKData: Specified HTK file: \"" + fileName + "\" does not exist or is inaccessible");

	HTKHeader Header = HTKHeader();
	infile.read((char*)&Header.nSamples, sizeof(int));
	infile.read((char*)&Header.sampPeriod, sizeof(int));
	infile.read((char*)&Header.sampSize, sizeof(short int));
	infile.read((char*)&Header.parmKind, sizeof(short int));
	Header.sampSize /= sizeof(float);

	buffer->SumFrames = Header.nSamples;

	ASegment chunk = ASegment();
	unsigned int aimChunkSize = GlobalSettings.DecodingElementSize;
	unsigned int numChunks = (Header.nSamples % aimChunkSize == 0) ? (Header.nSamples / aimChunkSize) : ((Header.nSamples / aimChunkSize) + 1);
	unsigned int count = 0;
	unsigned int remaining = Header.nSamples;

	while (count < numChunks)
	{
		try
		{
			buffer->BeginProducingChunk();

			unsigned int currentChunkSize = (remaining >= aimChunkSize) ? (aimChunkSize) : (remaining % aimChunkSize);
			chunk.nFrames = currentChunkSize;
			chunk.Frames.resize(currentChunkSize);

			for(unsigned int t = 0; t < currentChunkSize; t++)
			{
				chunk.Frames[t].Vector.resize(Header.sampSize);
				infile.read((char*) &chunk.Frames[t].Vector[0], Header.sampSize * sizeof(float));
			}

			std::stringstream ss;
			ss << "Read chunk " << (count + 1) << " of " << numChunks << std::endl;
			Logger(LOG_DEBUG) << ss.str();

			buffer->ProduceChunk(chunk);

			count++;
			remaining -= currentChunkSize;
		}
		catch(boost::thread_interrupted& e)
		{
			infile.close();
			Logger(LOG_ERROR) << "HTKData: Feature loading halted." << std::endl;
			return;
		}
	}

	infile.close();
	Logger(LOG_DEBUG) << "Finished reading HTK data" << std::endl;
}

ASegment 
HTKData::LoadCompleteAcousticAndLabelHTKData(const std::string& acousticFileName, const std::string& labelFileName)
{
	ASegment aSegment = ASegment();

	LoadCompleteAcousticHTKData(acousticFileName, aSegment);
	LoadCompleteLabelHTKData(labelFileName, aSegment);

	return aSegment;
}

void
HTKData::LoadCompleteAcousticHTKData(const std::string& fileName, ASegment& aSegment)
{
	std::ifstream infile;
	infile.open(fileName, std::ios::binary | std::ios::in);

	if (!infile)
		throw std::runtime_error("HTK Acoustic File: specified file: \"" + fileName + "\" does not exist or is inaccessible");

	if (infile)
	{
		HTKHeader Header = HTKHeader();
		infile.read((char*)&Header.nSamples, sizeof(int));
		infile.read((char*)&Header.sampPeriod, sizeof(int));
		infile.read((char*)&Header.sampSize, sizeof(short int));
		infile.read((char*)&Header.parmKind, sizeof(short int));
		Header.sampSize /= sizeof(float);

		aSegment.Frames.resize(Header.nSamples);
		aSegment.nFrames = Header.nSamples;

		for(int t = 0; t < Header.nSamples; t++)
		{
			aSegment.Frames[t].Vector.resize(Header.sampSize);
			infile.read((char*) &aSegment.Frames[t].Vector[0], Header.sampSize * sizeof(float));
		}

		infile.close();
	}
}

void 
HTKData::LoadCompleteLabelHTKData(const std::string& fileName, ASegment& aSegment)
{
	// [start [end] ] name

	std::ifstream infile(fileName);

	if (!infile)
		throw std::runtime_error("Label File: specified file: \"" + fileName + "\" does not exist or is inaccessible");

	std::string line;

	if (infile)
	{
		// Current unit
		int n = 0;

		// Whether the start and end times are included
		bool bound;

		while (std::getline(infile, line))
		{
			if (line.empty()) continue;

			std::string buffer;
			std::stringstream ss(line);
			std::vector<std::string> elements;
				
			// Split current line by whitespace
			while (ss >> buffer)
			{
				elements.push_back(buffer);
			}

			bound = (elements.size() > 1);

			aSegment.Units.push_back(PUnit());

			if (bound)
			{
				if (GlobalSettings.TranscriptionLabelFormat == LBLFMT_AUDACITY)
				{
					double start = Helper::MillisecondsToFrames(std::stod(elements[0].c_str()) * 1000);
					double end = Helper::MillisecondsToFrames(std::stod(elements[1].c_str()) * 1000);

					aSegment.Units[n].BeginBoundary = start;
					aSegment.Units[n].EndBoundary = end;

					if (elements.size() >= 4)
					{
						aSegment.Units[n].Score = std::stod(elements[3].c_str());
					}
				}
				aSegment.Units[n].Label = elements[2];
			}
			else
			{
				aSegment.Units[n].Label = elements[0];
			}

			aSegment.Units[n].Hit = false;

			n++;
		}

		infile.close();

		aSegment.nUnits = aSegment.Units.size();

		if (bound && GlobalSettings.TranscriptionLabelFormat == LBLFMT_HTK)
		{
			ScaleLabelBoundaries(aSegment);
		}
	}
}

void 
HTKData::ScaleLabelBoundaries(ASegment& aSegment)
{
	if (GlobalSettings.SpeechAnalysisParamaters.Shift <= 0) return;

	for (int n = 0; n < aSegment.nUnits; n++) 
	{
		aSegment.Units[n].BeginBoundary /= GlobalSettings.SpeechAnalysisParamaters.Shift;
		aSegment.Units[n].EndBoundary = (unsigned int)(
			(float)aSegment.Units[n].EndBoundary / (float)GlobalSettings.SpeechAnalysisParamaters.Shift + 0.5);
    }
}

void HTKData::WriteAcousticHTKData(const std::string& fileName, ASegment& aSegment, int sampPeriod)
{
	std::ofstream outfile;
	outfile.open(fileName, std::ios::binary | std::ios::out);

	if (outfile.is_open())
	{
		HTKHeader Header = HTKHeader();
		Header.nSamples = aSegment.Frames.size();
		Header.sampPeriod = sampPeriod;
		Header.sampSize = aSegment.Frames[0].Vector.size();// All same size
		Header.parmKind = 6;

		short int oSampSize = Header.sampSize * sizeof(float);
		outfile.write((char*)&Header.nSamples, sizeof(int));
		outfile.write((char*)&Header.sampPeriod, sizeof(int));
		outfile.write((char*)&oSampSize, sizeof(short int));
		outfile.write((char*)&Header.parmKind, sizeof(short int));

		for(int t = 0; t < Header.nSamples; t++)
		{
			outfile.write((char*) &aSegment.Frames[t].Vector[0], Header.sampSize * sizeof(float));
		}

		outfile.close();
	}
	else 
	{
		Logger(LOG_ERROR) << "HTK Acoustic File: " << fileName << " cannot be written" << std::endl;
	}
}

HTKHeader 
HTKData::ReadHeaderOnly(const std::string& fileName)
{
	std::ifstream infile(fileName);

	if (!infile)
		throw std::runtime_error("HTK File: specified file: \"" + fileName + "\" does not exist or is inaccessible");

	HTKHeader Header = HTKHeader();

	if (infile.is_open())
	{
		try
		{
			infile.read((char*)&Header.nSamples, sizeof(int));
			infile.read((char*)&Header.sampPeriod, sizeof(int));
			infile.read((char*)&Header.sampSize, sizeof(short int));
			infile.read((char*)&Header.parmKind, sizeof(short int));
			Header.sampSize /= sizeof(float);

			infile.close();
		}
		catch (std::exception e)
		{
			infile.close();
			throw;
		}
	}

	return Header;
}
