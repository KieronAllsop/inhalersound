#pragma once

#include <boost/atomic.hpp>

#include "RunMode.h"

class TSpecAnalysisMode : public IRunMode
{
public:
	TSpecAnalysisMode();
	~TSpecAnalysisMode();

	virtual void Run();

private:
	mutable boost::atomic<int> nFinished;

	void ConvertRange(int startIndex, int endIndex);

	void DisplayProgress();
};

