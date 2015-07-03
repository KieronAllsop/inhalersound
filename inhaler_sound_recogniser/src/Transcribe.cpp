#include "Transcribe.h"

#include <iomanip>
#include <iostream>

#include "Global.h"
#include "Helper.h"

TTranscribe::TTranscribe()
{
}


TTranscribe::~TTranscribe()
{
}

void 
TTranscribe::State2Unit(std::vector<unsigned int>* StateSeq, std::vector<PUnit>* U)
{
	int nFrames = StateSeq->size();

	if (nFrames == 0) return;

	bool reachedEnd = false;
	int currentEndState;
	int currentUnit;
	int currentStartFrame;

	for (int t = 0; t < nFrames; t++)
	{ 
		// The state the signal was in at frame t
		unsigned int i = (*StateSeq)[t];

		// The unit to which the current state, i belongs
		int n = Vocabulary->GetUnitPositionByState(i);

		// For the first frame, the current unit is whichever is in StateSeq[0]
		if (t == 0)
		{
			currentEndState = Vocabulary->Units[n].EndLogicalPosition - 1;
			currentUnit = n;
			currentStartFrame = t;
		}
		
		// Reached the end state of the current unit
		if (i == currentEndState)
		{
			reachedEnd = true;
		}

		// On to a new unit once we're no longer in the previous end state
		if (reachedEnd && i != currentEndState)
		{
			PUnit pUnit = PUnit();
			pUnit.Label = Vocabulary->Units[currentUnit].Name;
			pUnit.BeginBoundary = currentStartFrame;
			pUnit.EndBoundary = t;
			(*U).push_back(pUnit);

			reachedEnd = false;
			currentEndState = Vocabulary->Units[n].EndLogicalPosition - 1;
			currentUnit = n;
			currentStartFrame = t;

			if (i == currentEndState)
			{
				// Unit just one frame long
				reachedEnd = true;
			}
		}

		// Last unit
		if (t == nFrames - 1)
		{
			PUnit pUnit = PUnit();
			pUnit.Label = Vocabulary->Units[currentUnit].Name;
			pUnit.BeginBoundary = currentStartFrame;
			pUnit.EndBoundary = t;
			(*U).push_back(pUnit);
		}
	}
}

void 
TTranscribe::MergeUnitsByGroup(const std::string& label, std::vector<PUnit>& U)
{
	int ignoredEndState = -1;
	int ignoredEndUnit = -1;
	int ignoredStartState = -1;
	int ignoredStartUnit = -1;

	for (int i = U.size() - 1; i >= 0; i--)
	{
		bool match = (U[i].Label == label);
		if (match)
		{
			if (ignoredEndState < 0)
			{
				// If we've marked the end, subsequent words should only push the start boundary
				ignoredEndState = U[i].EndBoundary;
				ignoredEndUnit = i;
			}
			ignoredStartState = U[i].BeginBoundary;
			ignoredStartUnit = i;
		}

		if (!match || i == 0)
		{
			if (ignoredEndState < 0) continue;

			// Calculate the score as:
			// For each word, add the score * num. frames
			// Divide sum by total number of frames
			double sumScore = 0.0;
			for (int j = ignoredStartUnit; j <= ignoredEndUnit; j++)
			{
				sumScore += (U[j].Score * (U[j].EndBoundary - U[j].BeginBoundary));
			}
			double avgScore = sumScore / (ignoredEndState - ignoredStartState);

			// Merge the group of ignored units before this unit into one
			if (ignoredEndUnit != ignoredStartUnit)
			{
				// inclusive, exclusive indices
				U.erase(U.begin() + ignoredStartUnit + 1, U.begin() + ignoredEndUnit + 1);
			}
			U[ignoredStartUnit].EndBoundary = ignoredEndState;
			U[ignoredStartUnit].Score = avgScore;

			ignoredEndState = ignoredEndUnit = ignoredStartState = ignoredStartUnit = -1;
		}
	}
}

void 
TTranscribe::RemoveIgnoredUnits(std::vector<PUnit>& U)
{
	for (int i = U.size() - 1; i >= 0; i--)
	{
		bool ignore = Vocabulary->IgnoreUnitByName(U[i].Label);

		if (ignore)
		{
			U.erase(U.begin() + i);
		}
	}
}

void 
TTranscribe::RenameUnitsByGroup(std::vector<int>& group, std::vector<PUnit>& U, const std::string& name)
{
	for (int i = 0; i < U.size(); i++)
	{
		if (Vocabulary->InUnitListByName(group, U[i].Label))
		{
			U[i].Label = name;
		}
	}
}

void 
TTranscribe::AddFillerToGaps(std::vector<PUnit>& U)
{
	if (U.size() < 1) return;

	PUnit filler = PUnit();
	filler.Label = "Filler";

	if (U[0].BeginBoundary != 0 && U.size() > 1)
	{
		filler.BeginBoundary = 0;
		filler.EndBoundary = U[1].BeginBoundary;
		U.insert(U.begin(), filler);
	}

	for (int j = 0; j < U.size() - 1; j++)
	{
		if (U[j].EndBoundary != U[j + 1].BeginBoundary)
		{
			// insert filler unit
			filler.BeginBoundary = U[j].EndBoundary;
			filler.EndBoundary = U[j + 1].BeginBoundary;
			U.insert(U.begin() + j, filler);
			j++;
		}
	}
}

void 
TTranscribe::OffsetBoundaries(std::vector<PUnit>& U, unsigned long offsetMilliseconds)
{
	int offsetFrames = ((double)offsetMilliseconds / 1000.0) / 
		((double)GlobalSettings.SpeechAnalysisParamaters.Shift / (double)GlobalSettings.DecodingSamplingRate);

	for (int i = 0; i < U.size(); i++)
	{
		U[i].BeginBoundary += offsetFrames;
		U[i].EndBoundary += offsetFrames;
	}
}

void 
TTranscribe::OutputLabel(const std::string& fileName, std::vector<PUnit>& U)
{
	std::ofstream outfile(fileName);

	if (!outfile)
		throw std::runtime_error("Transcribe: Cannot write label file: " + fileName);

	if (outfile.is_open())
	{
		try
		{
			if (GlobalSettings.TranscriptionLabelFormat == LBLFMT_AUDACITY)
			{
				//outfile.precision(16);
				outfile.setf(std::ios::showpoint);
			}

			for (int i = 0; i < U.size(); i++)
			{
				// If the current unit is an ignored state, don't output to label
				/*if (Vocabulary->IgnoreUnitByName(U[i].Label))
				{
					continue;
				}*/

				double start = Helper::FramesToMilliseconds(U[i].BeginBoundary);
				double end = Helper::FramesToMilliseconds(U[i].EndBoundary) + 
					Helper::SamplesToMilliseconds(GlobalSettings.SpeechAnalysisParamaters.FrmLength - 
					GlobalSettings.SpeechAnalysisParamaters.Shift);
				// end calculated as so because otherwise it gives the time of the start of the frame, no the end
				if (GlobalSettings.TranscriptionLabelFormat == LBLFMT_AUDACITY)
				{
					// HTK uses 100ns units, 
					// Audacity uses 1s units
					start /= 1000;
					end /= 1000;

					outfile << std::setprecision(8) << start << " " << 
						end << " \t" << U[i].Label << "\t\t" << 
						std::setprecision(16) << U[i].Score << std::endl;
				}			
			}

			outfile.close();
		}
		catch (std::exception e)
		{
			outfile.close();
			throw;
		}
	}
}
