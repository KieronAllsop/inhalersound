#include "Score.h"

#include <algorithm>

#include "Helper.h"
#include "Logger.h"
#include "Vocabulary.h"

TScore::TScore()
{
}

TScore::~TScore()
{
}

// Verify individual keywords or utterance
bool 
TScore::Verify(std::vector<PUnit>* TranscribedUnits, std::vector<float>* ProbPerFrame, bool VerifyUtterance)
{
	int nTranscribedUnits = TranscribedUnits->size();

	// No recognised keywords
	if (nTranscribedUnits < 1) return true;

	// Calculate confidence scores
	double utteranceConfidenceScore = 0.0;
	if (GlobalSettings.ScoreUseGeometricConfidenceScore)
	{
		utteranceConfidenceScore = GeometricConfidenceScore(TranscribedUnits, nTranscribedUnits, ProbPerFrame);
	}
	else
	{
		utteranceConfidenceScore = ArithmeticConfidenceScore(TranscribedUnits, nTranscribedUnits, ProbPerFrame);
	}

	// Verify whole utterance
	if (VerifyUtterance)
	{
		return (utteranceConfidenceScore >= GlobalSettings.ScoreAcceptanceThreshold);
	}

	// Verify each individual keyword
	for (int u = TranscribedUnits->size() - 1; u >= 0; u--)
	{
		// Delete (Reject) the current unit of its score is below threshold
		if ((*TranscribedUnits)[u].Score < GlobalSettings.ScoreAcceptanceThreshold)
		{
			Logger(LOG_DEBUG) << "Reject unit: " << u << ": " << (*TranscribedUnits)[u].Label << " with score: " 
				<< (*TranscribedUnits)[u].Score << std::endl;
			TranscribedUnits->erase(TranscribedUnits->begin() + u);
		}
		// Else keep (accept) it
	}

	return (TranscribedUnits->size() >= 1);
}

double
TScore::GeometricConfidenceScore(std::vector<PUnit>* TranscribedUnits, int nTranscribedUnits, 
	std::vector<float>* ProbPerFrame)
{
	double utteranceConfidenceScore = 0.0;
	int nObservations = 0;
	
	for (int n = nTranscribedUnits - 1; n >= 0; n--)
	{
		unsigned int beginFrame = (*TranscribedUnits)[n].BeginBoundary;
		unsigned int endFrame = (*TranscribedUnits)[n].EndBoundary;

		int minFrames = Helper::MillisecondsToFrames(Vocabulary->GetMinimumDurationByName((*TranscribedUnits)[n].Label));
		if (minFrames > 0)
		{
			if ((endFrame - beginFrame) < (unsigned int)minFrames)
			{
				TranscribedUnits->erase(TranscribedUnits->begin() + n);
				continue;
			}
		}

		// Confidence score for each recognised unit
		float p = 0.0;
		for (unsigned int t = beginFrame; t < endFrame; t++)
		{
			p += (*ProbPerFrame)[t];
		}
		(*TranscribedUnits)[n].Score = exp(p / (endFrame - beginFrame));

		// Accumulate for utterance 
		utteranceConfidenceScore += p;
		nObservations += (endFrame - beginFrame);
	}

	// Final utterance confidence score
	return (utteranceConfidenceScore = exp(utteranceConfidenceScore / nObservations));
}

double 
TScore::ArithmeticConfidenceScore(std::vector<PUnit>* TranscribedUnits, int nTranscribedUnits, 
	std::vector<float>* ProbPerFrame)
{
	double utteranceConfidenceScore = 0.0;
	int nObservations = 0;

	for (int n = nTranscribedUnits - 1; n >= 0; n--)
	{
		unsigned int beginFrame = (*TranscribedUnits)[n].BeginBoundary;
		unsigned int endFrame = (*TranscribedUnits)[n].EndBoundary;

		int minFrames = Helper::MillisecondsToFrames(Vocabulary->GetMinimumDurationByName((*TranscribedUnits)[n].Label));
		if (minFrames > 0)
		{
			if ((endFrame - beginFrame) < (unsigned int)minFrames)
			{
				TranscribedUnits->erase(TranscribedUnits->begin() + n);
				continue;
			}
		}

		// Confidence score for each recognised unit
		float p = 0.0;
		for (unsigned int t = beginFrame; t < endFrame; t++)
		{
			p += exp((*ProbPerFrame)[t]);
		}
		(*TranscribedUnits)[n].Score = p / (endFrame - beginFrame);

		// Accumulate for utterance 
		utteranceConfidenceScore += p;
		nObservations += (endFrame - beginFrame);
	}

	// Final utterance confidence score
	return (utteranceConfidenceScore /= nObservations);
}

void 
TScore::TestForHit(std::vector<PUnit>* t, std::vector<PUnit>* r)
{
	std::vector<std::vector<int>> matchedToRefLabel = std::vector<std::vector<int>>();
	for (int i = 0; i < r->size(); i++)
	{
		matchedToRefLabel.push_back(std::vector<int>());
	}

	for (int ru = 0; ru < r->size(); ru++)
	{
		std::string rLabel = (*r)[ru].Label;
		int rStart = (*r)[ru].BeginBoundary;
		int rEnd = (*r)[ru].EndBoundary;

		for (int tu = 0; tu < t->size(); tu++)
		{
			std::string tLabel = (*t)[tu].Label;
			int tStart = (*t)[tu].BeginBoundary;
			int tEnd = (*t)[tu].EndBoundary;

			if (tLabel != rLabel) continue;
			if (tStart >= rEnd) continue;
			if (tEnd <= rStart) continue;

			// Else, there's some overlap
			bool hit = false;
			int midpoint = tStart + ((tEnd - tStart) / 2);

			if (midpoint > rStart && midpoint < rEnd)
			{
				hit = true;
			}

			if (hit)
			{
				matchedToRefLabel[ru].push_back(tu);
			}
		}
	}

	// Need to ensure that if two or more events are detected for one reference event that it 
	// only counts once

	std::vector<int> toErase = std::vector<int>();
	for (int ru = 0; ru < r->size(); ru++)
	{
		if (matchedToRefLabel[ru].size() > 0)
		{
			double sumScore = 0.0;
			int sumFrames = 0;
			for (int i = 0; i < matchedToRefLabel[ru].size(); i++)
			{
				int nFrames = ((*t)[matchedToRefLabel[ru][i]].EndBoundary - (*t)[matchedToRefLabel[ru][i]].BeginBoundary);
				sumScore += (*t)[matchedToRefLabel[ru][i]].Score * (double)nFrames;
				sumFrames += nFrames;
			}
			double avgScore = (double)(sumScore / (double)sumFrames);

			// Merge units into one
			if (matchedToRefLabel[ru].size() > 1)
			{
				Logger(LOG_DEBUG) << "Merging test unit: " << matchedToRefLabel[ru][0] << " with: " 
					<< matchedToRefLabel[ru].size() << " others" << std::endl;
			}
			(*t)[matchedToRefLabel[ru][0]].Hit = true;
			(*t)[matchedToRefLabel[ru][0]].Score = avgScore;
			(*t)[matchedToRefLabel[ru][0]].EndBoundary = (*t)[matchedToRefLabel[ru][matchedToRefLabel[ru].size() - 1]].EndBoundary;
			for (int i = 1; i < matchedToRefLabel[ru].size(); i++)
			{
				if (std::find(toErase.begin(), toErase.end(), matchedToRefLabel[ru][i]) == toErase.end())
				{
					toErase.push_back(matchedToRefLabel[ru][i]);
				}
			}
		}
	}

	std::sort(toErase.begin(), toErase.end());
	for (int i = toErase.size() - 1; i >= 0; i--)
	{
		t->erase(t->begin() + toErase[i]);
	}
}
