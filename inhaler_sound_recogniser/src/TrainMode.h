#pragma once

#include <string>

#include "RunMode.h"

class TTrainMode : public IRunMode
{
public:
	TTrainMode();
	~TTrainMode();

	virtual void Run();

private:
	short int nCurMix;
    long nAllFrames;
    float ProbOld;
    float ProbNew;

	void Initialise();

	void Reestimation(int nIterations);

	void SplitMasterLabelFile(const std::string& labelsDir);

	// Probability messages, return likelihood difference between iterations
    float PMessage();
};

