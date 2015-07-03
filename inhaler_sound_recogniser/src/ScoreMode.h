#pragma once

#include "RunMode.h"
#include "Score.h"

class TScoreMode : public IRunMode
{
public:
	TScoreMode();
	~TScoreMode();

	virtual void Run();

private:
	void OutputHitResults(const std::string& fileName, std::vector<PUnit>& U);
	void OutputForROCCurve(const std::string& fileName, std::vector<PUnit>& U, int nCough, int nFiller);
};

