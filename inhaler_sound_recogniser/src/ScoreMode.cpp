#include "ScoreMode.h"

#include <iomanip>

#include <boost/filesystem.hpp>

#include "Helper.h"
#include "HTKData.h"
#include "Global.h"
#include "Score.h"
#include "Transcribe.h"

TScore* Score = 0;
TTranscribe* Transcribe = 0;

TScoreMode::TScoreMode()
{
	Score = new TScore();
	Transcribe = new TTranscribe();

	Vocabulary = new TVocabulary(
		GlobalSettings.VocabularyVocabularyFileName, 
		GlobalSettings.VocabularyStateFileName, 
		GlobalSettings.VocabularyMixtureFileName);
}

TScoreMode::~TScoreMode()
{
}

void
TScoreMode::Run()
{
	HTKData htkReader = HTKData();

	std::vector<std::string> readRefLabels = std::vector<std::string>();

	std::vector<PUnit> mergedSorted = std::vector<PUnit>();
	int nCough = 0;
	int nFiller = 0;

	for (int i = 0; i < GlobalSettings.ScoreLabels.size(); i++)
	{
		ASegment testLabels = ASegment();
		htkReader.LoadCompleteLabelHTKData(GlobalSettings.ScoreLabels[i].Test, testLabels);
		Transcribe->OffsetBoundaries(testLabels.Units, GlobalSettings.ScoreLabels[i].StartOffsetMilliseconds);
		Transcribe->RenameUnitsByGroup(Vocabulary->IUnits, testLabels.Units, FILLER_LABEL);
		Transcribe->MergeUnitsByGroup(FILLER_LABEL, testLabels.Units);
		Transcribe->RenameUnitsByGroup(Vocabulary->CUnits, testLabels.Units, COUGH_LABEL);
		Transcribe->MergeUnitsByGroup(COUGH_LABEL, testLabels.Units);
		testLabels.nUnits = testLabels.Units.size();

		ASegment refLabels = ASegment();
		htkReader.LoadCompleteLabelHTKData(GlobalSettings.ScoreLabels[i].Reference, refLabels);
		Transcribe->RenameUnitsByGroup(Vocabulary->IUnits, refLabels.Units, FILLER_LABEL);
		Transcribe->RenameUnitsByGroup(Vocabulary->CUnits, refLabels.Units, COUGH_LABEL);
		Transcribe->AddFillerToGaps(refLabels.Units);
		refLabels.nUnits = refLabels.Units.size();

		std::stringstream ss;
		ss << GlobalSettings.ScoreLabels[i].Reference << "," << GlobalSettings.ScoreLabels[i].StartOffsetMilliseconds << 
				"," << GlobalSettings.ScoreLabels[i].EndOffsetMilliseconds;
		if (std::find(readRefLabels.begin(), readRefLabels.end(), ss.str()) == readRefLabels.end())
		{
			readRefLabels.push_back(ss.str());

			int startOffsetFrame = Helper::MillisecondsToFrames(GlobalSettings.ScoreLabels[i].StartOffsetMilliseconds);
			int endOffsetFrame = (GlobalSettings.ScoreLabels[i].EndOffsetMilliseconds != 0) ? 
				Helper::MillisecondsToFrames(GlobalSettings.ScoreLabels[i].EndOffsetMilliseconds) : -1;

			for (int ru = 0; ru < refLabels.nUnits; ru++)
			{
				// Ensure we take the offsets into account
				if (refLabels.Units[ru].BeginBoundary < startOffsetFrame) continue;
				if (endOffsetFrame > 0 && refLabels.Units[ru].BeginBoundary >= endOffsetFrame) continue;

				if (refLabels.Units[ru].Label == COUGH_LABEL) nCough++;
				else if (refLabels.Units[ru].Label == FILLER_LABEL) nFiller++;
			}
		}

		Logger(LOG_INFO) << "Testing hits in: " << 
			GlobalSettings.ScoreLabels[i].Test.substr(
			GlobalSettings.ScoreLabels[i].Test.find_last_of("\\/") + 1, std::string::npos) << std::endl;
		
		Score->TestForHit(&testLabels.Units, &refLabels.Units);
		testLabels.nUnits = testLabels.Units.size();
		
		std::string hitResOutName = boost::filesystem::basename(GlobalSettings.ScoreLabels[i].Test);
		int periodIndex = hitResOutName.rfind('.');
		if (std::string::npos != periodIndex)
		{
			hitResOutName.erase(periodIndex);
		}
		hitResOutName = GlobalSettings.ScoreOutputDirectory + "/" + hitResOutName + ".csv";

		OutputHitResults(hitResOutName, testLabels.Units);

		mergedSorted.insert(mergedSorted.end(), testLabels.Units.begin(), testLabels.Units.end());
	}

	std::sort(mergedSorted.begin(), mergedSorted.end(), PUnitGreaterThanScore());

	std::string sortedResOutName = GlobalSettings.ScoreOutputDirectory + "/MergedSortedHits.csv";

	Logger(LOG_INFO) << std::endl << "Merged and sorted hit results: " << std::endl;

	OutputHitResults(sortedResOutName, mergedSorted);

	Logger(LOG_INFO) << std::endl << "ROC data: " << std::endl;

	std::string detOutName = sortedResOutName;
	int periodIndex = detOutName.rfind('.');
	if (std::string::npos != periodIndex)
	{
		detOutName.erase(periodIndex);
	}
	detOutName += "_ROC.csv";
	OutputForROCCurve(detOutName, mergedSorted, nCough, nFiller);

	delete Score;
	delete Transcribe;
}

void 
TScoreMode::OutputHitResults(const std::string& fileName, std::vector<PUnit>& U)
{
	std::ofstream outfile(fileName);

	if (!outfile)
		throw std::runtime_error("ScoreMode: Cannot write hit results file: " + fileName);

	if (outfile.is_open())
	{
		double sampPeriod = (double)GlobalSettings.SpeechAnalysisParamaters.Shift / (double)GlobalSettings.DecodingSamplingRate;

		try
		{
			outfile << "start,end,event,score,hit" << std::endl;

			for (int i = 0; i < U.size(); i++)
			{
				double start = 0.0;
				double end = 0.0;
				if (GlobalSettings.TranscriptionLabelFormat == LBLFMT_AUDACITY)
				{
					start = U[i].BeginBoundary * sampPeriod;
					end = U[i].EndBoundary * sampPeriod;
				}
				else
				{
					start = U[i].BeginBoundary;
					end = U[i].EndBoundary;
				}

				outfile << std::setprecision(8) << start << "," << end << "," << 
					U[i].Label << "," << std::setprecision(16) << U[i].Score << 
					"," << U[i].Hit << std::endl;
			}

			Logger(LOG_INFO) << "Wrote hit results: " << fileName << std::endl;

			outfile.close();
		}
		catch (std::exception e)
		{
			outfile.close();
			throw;
		}
	}
}

void 
TScoreMode::OutputForROCCurve(const std::string& fileName, std::vector<PUnit>& U, int nCough, int nFiller)
{
	// Keyword Detection Rate (Y): no. of correctly marked coughs / no. of coughs in reference
	// False Alarm Rate (X): no. of incorrectly marked coughs / no. of fillers in reference

	std::vector<int> nCoughHits = std::vector<int>();
	std::vector<int> nCoughFalse = std::vector<int>();

	std::ofstream outfile(fileName);

	if (!outfile)
		throw std::runtime_error("ScoreMode: Cannot write ROC results file: " + fileName);

	if (outfile.is_open())
	{
		try
		{
			int unit = 0;
			for (int i = 0; i < U.size(); i++)
			{
				if (U[i].Label == COUGH_LABEL)
				{
					nCoughHits.push_back(0);
					nCoughFalse.push_back(0);

					nCoughHits[unit] = nCoughHits[(unit > 0) ? (unit - 1) : (0)] + ((U[i].Hit) ? 1 : 0);
					nCoughFalse[unit] = nCoughFalse[(unit > 0) ? (unit - 1) : (0)] + ((U[i].Hit) ? 0 : 1);

					unit++;
				}
			}

			outfile << "nCoughsRef=" << nCough << ",nFillersRef=" << nFiller << std::endl;
			outfile << "threshold,nCoughHits,nCoughFalse,keywordDetectionRate,falseAlarmRate" << std::endl;

			unit = 0;
			for (int i = 0; i < U.size(); i++)
			{
				if (U[i].Label == COUGH_LABEL)
				{
					double keywordDetectionRate = (nCough > 0) ? (double)nCoughHits[unit] / (double)nCough : 0.0;
					double falseAlarmRate = (nFiller > 0) ? (double)nCoughFalse[unit] / (double)nFiller : 0.0;

					outfile << std::setprecision(16) << U[i].Score << "," << nCoughHits[unit] << "," << nCoughFalse[unit] << "," << 
						keywordDetectionRate << "," << falseAlarmRate << std::endl;
					
					unit++;
				}
			}

			Logger(LOG_INFO) << "Wrote ROC data: " << fileName << std::endl;
		}
		catch (std::exception e)
		{
			outfile.close();
			throw;
		}
	}
}
