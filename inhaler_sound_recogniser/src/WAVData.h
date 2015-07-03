#pragma once

#include <fstream>
#include <iostream>

#include "FeatureVectorLoader.h"

typedef struct 
{
	char ID[4];			// "RIFF"
    int Len;			// Remaining length after this header
} RiffHeader;

typedef struct 
{
	char ID[4];			// "fmt", "fact", "data", ...
    unsigned int Len;	// Remaining chunk length after this header
} ChunkHeader;

typedef struct 
{
	short int FormatTag;		// 0x0001->PCM, ...
    short int Channels;			// Number of channels
    int Rate;					// Sampling rate
    int BytesPerSec;			// Bytes per second
    short int BytesPerSample;	// Bytes per sample
	short int BitsPerSample;	// Bits per sample
} WaveFormatChunk;

struct WAV
{
	std::vector<short int> Sample;
    unsigned int nSamples;

    unsigned int Start, End; // Endpoints in frames

	int SamplingRate;

	WAV()
	{
		Sample = std::vector<short int>();
		nSamples = 0;
		Start = 0;
		End = 0;
		SamplingRate = 0;
	}
};

class WAVData : public IFeatureVectorLoader
{
public:
	WAVData();
	~WAVData();

	virtual void ExtractToBuffer(const std::string& fileName, BufferedASignal* buffer);

	WAV ReadFullWAV(const std::string& fileName);

	WAV ReadyToReadWAVData(std::ifstream& infile);

	void ReadWAVSegment(std::ifstream& infile, WAV& buffer, int realPos, int realEnd, int nSamples, short int extra);

private:
	bool ReadHeader(std::ifstream& infile, ChunkHeader& chunkHeader, WaveFormatChunk& waveFormatChunk);

	static int NumFramesFromNumSamples(int nSamples);

	static int NumSamplesFromNumFrames(int nFrames);
};

