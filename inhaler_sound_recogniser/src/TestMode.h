#pragma once

#include <boost/atomic.hpp>

#include <vector>

#include "RunMode.h"

class TTestMode : public IRunMode
{
public:
	TTestMode();
	~TTestMode();

	virtual void Run();

private:
	void DecodeRange(int threadIndex, int startIndex, int endIndex);
	void DisplayProgress();

	int nThreads;
	std::vector<int> nFilesPerThread;

	mutable boost::atomic<int> nFinished;
	std::vector<int> Progress;
};

