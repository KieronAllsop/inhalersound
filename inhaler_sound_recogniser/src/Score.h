#pragma once

#include "SIG.h"

class TScore
{
public:
	TScore();
	~TScore();

	// Verify individual keywords or utterance
    bool Verify(std::vector<PUnit>* TranscribedUnits, std::vector<float>* ProbPerFrame, bool VerifyUtterance);

	// Given a reference and test transcription, determines which test units are correct
	void TestForHit(std::vector<PUnit>* t, std::vector<PUnit>* r);

private:

	// Compute confidence score - geometric
	double GeometricConfidenceScore(std::vector<PUnit>* TranscribedUnits, int nTranscribedUnits, 
		std::vector<float>* ProbPerFrame);

	// Compute confidence score - arithmetic
	double ArithmeticConfidenceScore(std::vector<PUnit>* TranscribedUnits, int nTranscribedUnits, 
		std::vector<float>* ProbPerFrame);
};

