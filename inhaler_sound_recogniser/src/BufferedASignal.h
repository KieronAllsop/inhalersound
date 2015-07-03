#pragma once

#include <vector>

#include "Semaphore.h"
#include "SIG.h"

class BufferedASignal
{
public:
	BufferedASignal(int bufferSize);
	BufferedASignal();
	~BufferedASignal();

	void BeginProducingChunk();
	void ProduceChunk(ASegment segment);
	ASegment* RequestChunk();
	void FinishConsumingChunk();

	unsigned int SumFrames;

private:
	void Init(int bufferSize);

	std::vector<ASegment> Buffer;
	unsigned int BufferSize;
	unsigned int Head, Tail;
	Semaphore Elements;
	Semaphore Spaces;
};

