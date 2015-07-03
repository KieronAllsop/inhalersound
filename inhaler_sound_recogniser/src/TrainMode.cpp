#include "TrainMode.h"

#include <boost/filesystem.hpp>

#include "Code.h"
#include "Estimate.h"
#include "Global.h"
#include "Helper.h"
#include "HTKData.h"
#include "Model.h"
#include "RunMode.h"
#include "SpecAnalysisMode.h"
#include "Vocabulary.h"

TEstimate* Estimate;

TTrainMode::TTrainMode()
{
	// Load Vocabulary first 
	Vocabulary = new TVocabulary(
		GlobalSettings.VocabularyVocabularyFileName, 
		GlobalSettings.VocabularyStateFileName, 
		GlobalSettings.VocabularyMixtureFileName);
}

void
TTrainMode::Run()
{
	// First, convert any training data in WAV format to HTK features format
	std::string labelsDir = GlobalSettings.TrainingOptions.OutputDirectory + "/labels";
	int index = 0;
	while (GlobalSettings.TrainingAcousticFileNames.size() < GlobalSettings.DecodingNumFiles)
	{
		std::string acName = GlobalSettings.DecodingFileNames[index];
		std::string baseName = boost::filesystem::basename(acName);

		// Remove extension if present
		int periodIndex = baseName.rfind('.');
		if (std::string::npos != periodIndex)
		{
			baseName.erase(periodIndex);
		}

		std::string lblName = labelsDir + "/" + baseName + ".lbl";

		if (GlobalSettings.DecodingSourceTypes[index] == AUDSRC_WAV)
		{
			int periodIndex = acName.rfind('.');
			if (std::string::npos != periodIndex)
			{
				acName.erase(periodIndex);
			}
			acName += "/" + baseName + "." + GlobalSettings.SpeechAnalysisParamaters.OutputFileExtension;

			index++;
		}
		else if (GlobalSettings.DecodingSourceTypes[index] == AUDSRC_HTK)
		{
			GlobalSettings.DecodingFileNames.erase(GlobalSettings.DecodingFileNames.begin() + index);
			GlobalSettings.DecodingSourceTypes.erase(GlobalSettings.DecodingSourceTypes.begin() + index);
		}

		GlobalSettings.TrainingAcousticFileNames.push_back(acName);
		GlobalSettings.TrainingLabelFileNames.push_back(lblName);
	}
	GlobalSettings.TrainingNumFiles = GlobalSettings.TrainingAcousticFileNames.size();

	// GlobalSettings.DecodingFileNames now contains all training data in WAV format that needs converted

	GlobalSettings.DecodingNumFiles = GlobalSettings.DecodingFileNames.size();
	if (GlobalSettings.DecodingNumFiles > 0)
	{
		Logger(LOG_INFO) << "Converting training data in WAV format to feature vectors" << std::endl;

		IRunMode* specAnalMode = new TSpecAnalysisMode();
		specAnalMode->Run();

		delete specAnalMode;
	}

	// Convert Master Label File into individual labels
	SplitMasterLabelFile(labelsDir);

	Logger(LOG_INFO) << "Finished splitting Master Label File" << std::endl;

	HTKData htkReader = HTKData();

	int VectorSize = htkReader.ReadHeaderOnly(GlobalSettings.TrainingAcousticFileNames[0]).sampSize;
	int nStreams = GlobalSettings.TrainingOptions.TrStreams;
	int StreamSize = VectorSize / nStreams;

	Code = new TCode();
	Code->CreateEmpty(Vocabulary->nPhysicalStates, nStreams, StreamSize);

	if (boost::filesystem::exists(GlobalSettings.CodeFileName))
	{
		Code->Load(GlobalSettings.CodeFileName);
		nCurMix = Code->GetMaxNumGaussians();
	}
	else
	{
		nCurMix = -1;
	}
	if (nCurMix < 0) nCurMix = 0;

	Model = new TModel();

	Estimate = new TEstimate();

	nAllFrames = 0;
    ProbOld = ProbNew = 0.0;
	
	// Train mixture model
	// Initialise - single mixture by uniform segmentation
	if (nCurMix == 0)
	{ 
		Initialise();
	}
	Logger(LOG_INFO) << "Initialisation complete" << std::endl;
    // Reestimation
	Reestimation(GlobalSettings.TrainingOptions.ModelIterations[0]);
	// Increase mixtures
	if (!GlobalSettings.TrainingOptions.DoubleMixtures)
	{
		while (Estimate->Split()) 
		{
			nCurMix++; 
			Reestimation(GlobalSettings.TrainingOptions.ModelIterations[1]);
		}
	}
    else
	{
		while (Estimate->Split2()) 
		{
			nCurMix *= 2; 
			Reestimation(GlobalSettings.TrainingOptions.ModelIterations[1]);
		}
	}
   	// Finalise full mixture model
    Reestimation(GlobalSettings.TrainingOptions.ModelIterations[2]);

	delete Estimate;
}

void 
TTrainMode::Initialise()
{
	// Set all Code->State[].nMix to 1
    nCurMix = 1;
	for (unsigned int i = 0; i < Code->nStates; i++)
	{
		Code->States[i].nGaussians = 1;
	}

	int maxFrames = Helper::MillisecondsToFrames(GlobalSettings.TrainingMaxLengthMillisecondsPerFile);

	HTKData *htkLoader = new HTKData();

	Logger(LOG_DEBUG) << "Checking for files whose duration exceeds the permitted maximum" << std::endl;
	for (int i = GlobalSettings.TrainingNumFiles - 1; i >= 0; i--)
	{
		HTKHeader header = htkLoader->ReadHeaderOnly(GlobalSettings.TrainingAcousticFileNames[i]);

		if (header.nSamples > maxFrames)
		{
			GlobalSettings.TrainingAcousticFileNames.erase(GlobalSettings.TrainingAcousticFileNames.begin() + i);
			GlobalSettings.TrainingLabelFileNames.erase(GlobalSettings.TrainingLabelFileNames.begin() + i);
			GlobalSettings.TrainingNumFiles--;

			Logger(LOG_DEBUG) << "Removed long training file: " << GlobalSettings.TrainingAcousticFileNames[i] << std::endl;
		}
	}

	Logger(LOG_INFO) << "Begin accumulation by uniform segmentation" << std::endl;

	for (int i = 0; i < GlobalSettings.TrainingNumFiles; i++)
	{
		ASegment aSegment = htkLoader->LoadCompleteAcousticAndLabelHTKData(
			GlobalSettings.TrainingAcousticFileNames[i], 
			GlobalSettings.TrainingLabelFileNames[i]);

		Model->MatchLabel(aSegment);

		Estimate->AccByUniSeg(aSegment);
	}

	Estimate->UniSegModel();

	Code->Save(GlobalSettings.TrainingOptions.OutputDirectory + "/code.sb");
}

void
TTrainMode::Reestimation(int nIterations)
{
	HTKData htkRead = HTKData();
	TViterbiDecoder viterbi = TViterbiDecoder();

	Logger(LOG_INFO) << "Re-estimation with " << nIterations << " iterations" << std::endl;

	for (int i = 0; i < nIterations; i++)
	{
		Code->Zero(Code->AStates);

		for (int j = 0; j < GlobalSettings.TrainingNumFiles; j++)
		{
			ASegment aSegment = htkRead.LoadCompleteAcousticAndLabelHTKData(
				GlobalSettings.TrainingAcousticFileNames[j], 
				GlobalSettings.TrainingLabelFileNames[j]);

			// Create Model->State and ->AState matching Signal->Unit
			Model->MatchLabel(aSegment);
			// Forward-Backward algorithm
			Estimate->AccByTrueFB(aSegment);

			ProbNew += Estimate->Pr;
			nAllFrames += aSegment.nFrames;
		}

		// Finish accumulation
		Estimate->FBModel();
    	
		// Save
    	Code->Save(GlobalSettings.TrainingOptions.OutputDirectory + "/code.sb");

        // Probability
        if(PMessage() < 0.001) break;
	}
}

void 
TTrainMode::SplitMasterLabelFile(const std::string& labelsDir)
{
	boost::filesystem::path labelsOutDir = boost::filesystem::path(labelsDir);
	if (!boost::filesystem::exists(labelsOutDir))
	{
		if (!boost::filesystem::create_directory(boost::filesystem::path(labelsOutDir)))
		{
			throw std::runtime_error("Error creating directory: " + labelsDir);
		}
	}

	std::ifstream infile(GlobalSettings.TrainingOptions.MasterLabelFile);

	if (!infile)
		throw std::runtime_error("Train: specified Master Label File: " + GlobalSettings.TrainingOptions.MasterLabelFile + 
			" does not exist or is inaccessible");

	if (infile.is_open())
	{
		try
		{
			std::string line;

			while (std::getline(infile, line))
			{
				if (line.empty()) continue;
				if (line.substr(0, 2).compare("//") == 0) continue;
				if (line.substr(0, 7).compare("#!MLF!#") == 0) continue;

				std::string currentLabel = line;
				currentLabel.erase(std::remove(currentLabel.begin(), currentLabel.end(), '\"'), currentLabel.end());
				int lastSlashIndex = currentLabel.find_last_of("\\/");
				if (std::string::npos != lastSlashIndex)
				{
					currentLabel.erase(0, lastSlashIndex + 1);
				}
				currentLabel = labelsDir + "/" + currentLabel + ".lbl";

				std::ofstream outfile(currentLabel);
				if (!outfile)
					throw std::runtime_error("Train: Error: Cannot write to file: " + currentLabel);

				std::string data;
				bool end = false;
				while (!end && std::getline(infile, data))
				{
					if (data.empty()) continue;
					if (data.substr(0, 1).compare(".") == 0) end = true;

					outfile << data << std::endl;
				}

				outfile.close();
			}

			infile.close();
		}
		catch (std::exception e)
		{
			infile.close();
			throw;
		}
	}
}

float 
TTrainMode::PMessage()
{
	// Update probabilities
    ProbNew /= nAllFrames;
    float probDiff = ProbNew - ProbOld;

	Logger(LOG_INFO) << "ProbNew: " << ProbNew << ", probDiff: " << probDiff << ", \t\t" << Helper::CurrentDateTime() << std::endl;

	ProbOld = ProbNew;
   	ProbNew = 0;
   	nAllFrames = 0;

	if (probDiff < 0) probDiff = 10;
    return probDiff;
}

TTrainMode::~TTrainMode()
{
}
