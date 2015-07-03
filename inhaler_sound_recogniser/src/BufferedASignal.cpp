#include "BufferedASignal.h"

const int DEFAULT_BUFFER_SIZE = 5000;

BufferedASignal::BufferedASignal(int bufferSize) 
	: Elements(0), Spaces(bufferSize)
{
	Init(bufferSize);
}

BufferedASignal::BufferedASignal() 
	: Elements(0), Spaces(DEFAULT_BUFFER_SIZE)
{
	Init(DEFAULT_BUFFER_SIZE);
}

BufferedASignal::~BufferedASignal()
{
}

void 
BufferedASignal::Init(int bufferSize)
{
	BufferSize = bufferSize;

	Buffer = std::vector<ASegment>();
	Buffer.resize(BufferSize);

	Head = 0;
	Tail = 0;
}

void BufferedASignal::BeginProducingChunk()
{
	Spaces.Wait();
}

void BufferedASignal::ProduceChunk(ASegment segment)
{
	Buffer[Tail] = segment;
	Tail = (Tail + 1) % BufferSize;
	Elements.Signal();
}

ASegment* BufferedASignal::RequestChunk()
{
	Elements.Wait();
	return &(Buffer[Head]);
}

void BufferedASignal::FinishConsumingChunk()
{
	Head = (Head + 1) % BufferSize;
	Spaces.Signal();
}
