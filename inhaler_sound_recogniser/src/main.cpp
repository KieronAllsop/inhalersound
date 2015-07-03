// CoughRecogniser.cpp : Defines the entry point for the console application.
//

#include <algorithm>
#include <iostream>
#include <signal.h>
#include <stdexcept>
#include <string>

#include "version.hpp"

#include "Bigram.h"
#include "Code.h"
#include "Global.h"
#include "Logger.h"
#include "Model.h"
#include "RunMode.h"
#include "ScoreMode.h"
#include "SpecAnalysis.h"
#include "SpecAnalysisMode.h"
#include "TestMode.h"
#include "TrainMode.h"
#include "Vocabulary.h"

Settings GlobalSettings = Settings();

TCode*				Code;
TVocabulary*		Vocabulary;
TModel*				Model;
TBigram*			Bigram;
IRunMode*			RunMode;

volatile sig_atomic_t flag = 0;

void ReadAndInitialiseSettings(int argc, char* argv[]);
void Cleanup();
void SigHandler(int sig);
void Identity();
void Usage();
void ProcessSettingsFile(const std::string& fileName);
void LoadBatchFile(const std::string& fileName);
void ReadAndInitialiseSpecAnalysisSettings(const std::string& fileName);
void ReadAndInitialiseTrainingSettings(const std::string& fileName);
void LoadComparisonFile(const std::string& fileName);

int main(int argc, char* argv[])
{
    Identity();

	signal(SIGINT, SigHandler);

	try
	{
		ReadAndInitialiseSettings(argc, argv);

		if (GlobalSettings.ProgramMode == MODE_TEST)
		{
			Logger(LOG_INFO) << std::endl << "TEST Mode" << std::endl;

			if (GlobalSettings.DecodingNumFiles < 1)
			{
				std::cout << "No files specified for recognition. Exiting." << std::endl;
				return 0;
			}

			RunMode = new TTestMode();
		}
		if (GlobalSettings.ProgramMode == MODE_TRAIN)
		{
			Logger(LOG_INFO) << std::endl << "TRAIN Mode" << std::endl;

			if (GlobalSettings.DecodingNumFiles < 1)
			{
				std::cout << "No files specified for training. Exiting." << std::endl;
				return 0;
			}

			RunMode = new TTrainMode();
		}
		else if (GlobalSettings.ProgramMode == MODE_SPECTRA)
		{
			Logger(LOG_INFO) << std::endl << "SPECTRAL ANALYSIS Mode" << std::endl;

			if (GlobalSettings.DecodingNumFiles < 1)
			{
				std::cout << "No files specified for feature calculation. Exiting." << std::endl;
				return 0;
			}

			RunMode = new TSpecAnalysisMode();
		}
		else if (GlobalSettings.ProgramMode == MODE_SCORE)
		{
			Logger(LOG_INFO) << std::endl << "SCORE ANALYSIS Mode" << std::endl;

			if (GlobalSettings.ScoreLabels.size() < 1)
			{
				std::cout << "No files specified for scoring. Exiting." << std::endl;
				return 0;
			}

			RunMode = new TScoreMode();
		}

		RunMode->Run();

	}
	catch (std::exception& e)
	{
		Logger(LOG_ERROR) << "ERROR: " << e.what() << std::endl;
		Cleanup();
		exit(-1);
	}

	#ifdef _DEBUG
	std::cout << std::endl << "Press return to exit..." << std::endl;
	std::getchar();
	#endif

	Cleanup();

	return 0;
}

void ReadAndInitialiseSettings(int argc, char* argv[])
{
	// Mode
	GlobalSettings.ProgramMode = MODE_TEST;
	// Code
	GlobalSettings.CodeFileName = "c:/tmp/c01/code.sb";
	// Vocabulary
	GlobalSettings.VocabularyVocabularyFileName = "c:/tmp/c01/States2.voc";
	GlobalSettings.VocabularyStateFileName = "c:/tmp/c01/state.sb";
	GlobalSettings.VocabularyMixtureFileName = "c:/tmp/c01/mix.sb";
	// Bigram
	GlobalSettings.BigramFileName = "c:/tmp/c01/one-word.bgm";
	GlobalSettings.BigramFixedPenalty = 0.0;
	GlobalSettings.BigramCrossUnitPenalty = 1.0;
	// Speech input parameters
	GlobalSettings.SpeechAnalysisInputParameterFile = "";
	GlobalSettings.SpeechAnalysisParamaters = SpeechAnalysisParamaters_t();
	GlobalSettings.SpeechAnalysisNumThreads = 4;
	// Decoding
	GlobalSettings.DecodingSourceTypes = std::vector<AudioSource>();
	GlobalSettings.DecodingFileNames = std::vector<std::string>();
	GlobalSettings.DecodingNumFiles = 0;
	GlobalSettings.DecodingBufferSize = 50;
	GlobalSettings.DecodingElementSize = 800;
	GlobalSettings.DecodingPruningThreshold = 500.0;
	GlobalSettings.DecodingSamplingRate = 16000;
	GlobalSettings.DecodingNumThreads = 1;
	//Transcription
	GlobalSettings.TranscriptionLabelFormat = LBLFMT_AUDACITY;
	// Training
	GlobalSettings.TrainingAcousticFileNames = std::vector<std::string>();
	GlobalSettings.TrainingLabelFileNames = std::vector<std::string>();
	GlobalSettings.TrainingNumFiles = 0;
	GlobalSettings.TrainingParameterFile = "";
	GlobalSettings.TrainingOptions = TrainingParameters();
	GlobalSettings.TrainingMaxLengthMillisecondsPerFile = 10000;
	// Viterbi
	GlobalSettings.ViterbiSkipState = 0;
	// Confidence Scoring
	GlobalSettings.ScoreAcceptanceThreshold = 0.0; // 0 - accept all
	GlobalSettings.ScoreUseGeometricConfidenceScore = false;
	GlobalSettings.ScoreLabels = std::vector<ScoreLabelPair>();
	GlobalSettings.ScoreOutputDirectory = "c:/cough/score";
	// Logging
	GlobalSettings.LogLogLevel = LOG_DEBUG;

	for (int i = 1; i < argc; i++)
	{
		Logger(LOG_DEBUG) << std::string(argv[i]) << " " << std::endl;
		if (strncmp(argv[i], "-h", 2) == 0 || strncmp(argv[i], "--help", 6) == 0)
		{
			Usage();
			exit(0);
		}
		else if (i + 1 != argc)
		{
			if (strncmp(argv[i], "-s", 2) == 0 || strncmp(argv[i], "--settings", 10) == 0)
			{
				std::string settingsFile = std::string(argv[i + 1]);
				settingsFile.erase(std::remove(settingsFile.begin(), settingsFile.end(), '\"'), settingsFile.end());

				ProcessSettingsFile(settingsFile);

				Logger(LOG_DEBUG) << settingsFile << std::endl;

				i++;
			}
			else if (strncmp(argv[i], "-m", 2) == 0 || strncmp(argv[i], "--mode", 6) == 0)
			{
				std::string mode = std::string(argv[i + 1]);
				std::transform(mode.begin(), mode.end(), mode.begin(), ::tolower);

				if (mode == "train")
				{
					GlobalSettings.ProgramMode = MODE_TRAIN;
				}
				else if (mode == "spectra")
				{
					GlobalSettings.ProgramMode = MODE_SPECTRA;
				}
				else if (mode == "test")
				{
					GlobalSettings.ProgramMode = MODE_TEST;
				}
				else if (mode == "score")
				{
					GlobalSettings.ProgramMode = MODE_SCORE;
				}

				Logger(LOG_DEBUG) << mode << std::endl;

				i++;
			}
			else if (strncmp(argv[i], "-i", 2) == 0 || strncmp(argv[i], "--infile", 7) == 0)
			{
				std::string infile = std::string(argv[i + 1]);
				infile.erase(std::remove(infile.begin(), infile.end(), '\"'), infile.end());

				GlobalSettings.DecodingFileNames = std::vector<std::string>();
				GlobalSettings.DecodingFileNames.push_back(infile);

				GlobalSettings.DecodingNumFiles = 1;

				Logger(LOG_DEBUG) << infile << std::endl;

				i++;
			}
			else if (strncmp(argv[i], "-f", 2) == 0 || strncmp(argv[i], "--format", 7) == 0)
			{
				std::string format = std::string(argv[i + 1]);
				std::transform(format.begin(), format.end(), format.begin(), ::tolower);

				GlobalSettings.DecodingSourceTypes = std::vector<AudioSource>();

				if (strncmp(argv[i + 1], "wav", 3) == 0)
				{
					GlobalSettings.DecodingSourceTypes.push_back(AUDSRC_WAV);
				}
				else if (strncmp(argv[i + 1], "htk", 3) == 0)
				{
					GlobalSettings.DecodingSourceTypes.push_back(AUDSRC_HTK);
				}

				Logger(LOG_DEBUG) << format << std::endl;

				i++;
			}
			else if (strncmp(argv[i], "-b", 2) == 0 || strncmp(argv[i], "--batch", 7) == 0)
			{
				std::string batchFile = std::string(argv[i + 1]);

				LoadBatchFile(batchFile);

				Logger(LOG_DEBUG) << batchFile << std::endl;

				i++;
			}
			else if (strncmp(argv[i], "-t", 2) == 0 || strncmp(argv[i], "--trainparams", 10) == 0)
			{
				std::string trainingSettings = std::string(argv[i + 1]);
				GlobalSettings.TrainingParameterFile = trainingSettings;

				ReadAndInitialiseTrainingSettings(trainingSettings);

				Logger(LOG_DEBUG) << trainingSettings << std::endl;

				i++;
			}
			else if (strncmp(argv[i], "-a", 2) == 0 || strncmp(argv[i], "--analysis", 10) == 0)
			{
				std::string specAnalysisSettings = std::string(argv[i + 1]);
				GlobalSettings.SpeechAnalysisInputParameterFile = specAnalysisSettings;

				ReadAndInitialiseSpecAnalysisSettings(specAnalysisSettings);

				Logger(LOG_DEBUG) << specAnalysisSettings << std::endl;

				i++;
			}
			else if (strncmp(argv[i], "-c", 2) == 0 || strncmp(argv[i], "--compare", 9) == 0)
			{
				std::string comparisonFile = std::string(argv[i + 1]);

				LoadComparisonFile(comparisonFile);

				Logger(LOG_DEBUG) << comparisonFile << std::endl;

				i++;
			}
			else
			{
				Logger(LOG_ERROR) << "Not enough or invalid arguments, please try again.\t" << std::string(argv[i]) << std::endl;
				Usage();
				exit(0);
			}
		}
    }
}

void Cleanup()
{
	delete Code;
	delete Vocabulary;
	delete Model;
	delete Bigram;
	if (RunMode) delete RunMode;
}

void SigHandler(int sig)
{
	Cleanup();
	Logger(LOG_ERROR) << "Signalled, exiting" << std::endl;
}


void Identity()
{
	std::cout
        << recogniser::build::identity::product_version()
        << " rev [" << recogniser::build::identity::product_revision()
        << "] - built in mode [" << recogniser::build::identity::build_variant()
        << "] at [" << recogniser::build::identity::build_time() << "]\n";
}


void Usage()
{
	std::cout <<
		"Argument(s)\tDescription" << std::endl <<
		"-h" << std::endl <<
		"--help\t\t> When supplied, outputs a list of the available arguments " << std::endl <<
		"\t\t  and their descriptions" << std::endl <<
		"-m" << std::endl <<
		"--mode\t\t> The mode in which the program is to be run" << std::endl <<
		"\t\t\t- \"test\" for performing recognition" << std::endl <<
		"\t\t\t- \"train\" for training" << std::endl <<
		"\t\t\t- \"spectra\" for calculating and outputting " << std::endl <<
		"\t\t\t  feature vectors" << std::endl <<
		"\t\t\t- \"score\" for measuring recognition performance" << std::endl <<
		"-s" << std::endl <<
		"--settings\t> When supplied should be followed by the name and location " << std::endl <<
		"\t\t  of the System Settings File (SSF)." << std::endl <<
		"-i" << std::endl <<
		"--infile\t> Used only in \"TEST\" and \"SPECTRA\" modes " << std::endl <<
		"\t\t> When supplied should be followed by the name and location " << std::endl <<
		"\t\t  of a single audio/features file. " << std::endl <<
		"\t\t> Should not be used with \"-b\" argument" << std::endl <<
		"\t\t> Used with \"-f\" option" << std::endl <<
		"-f" << std::endl <<
		"--format\t> Used only in \"TEST\" and \"SPECTRA\" modes " << std::endl <<
		"\t\t> To be supplied in conjunction with \"-i\" argument" << std::endl <<
		"\t\t> Specifies the format of the input audio file. " << std::endl <<
		"\t\t  Valid values are:" << std::endl <<
		"\t\t\t- \"htk\"" << std::endl <<
		"\t\t\t- \"wav\"" << std::endl <<
		"-b" << std::endl <<
		"--batch\t\t> Used in \"TEST\", \"SPECTRA\" and \"TRAIN\" modes" << std::endl <<
		"\t\t> Provides name and location of Input Batch File (IBF)" << std::endl <<
		"\t\t> Should not be used with \"-i\" argument. " << std::endl <<
		"-t" << std::endl <<
		"--trainparams\t> \tUsed only in \"TRAIN\" mode, required" << std::endl <<
		"\t\t> Provides the name and location of Training Parameters " << std::endl <<
		"\t\t  (TP) file" << std::endl <<
		"-a" << std::endl <<
		"--analysis\t> Required in all modes" << std::endl <<
		"\t\t> Provides name and location of Feature Vectors " << std::endl <<
		"\t\t  Parameters (FVP)" << std::endl <<
		"-c" << std::endl <<
		"--compare\t> Used only in \"SCORE\" mode" << std::endl <<
		"\t\t> Provides the name and location of Label Comparison " << std::endl <<
		"\t\t  File (LCF)" << std::endl <<
		std::endl;
}

void ProcessSettingsFile(const std::string& fileName)
{
	std::ifstream infile (fileName);

	if (!infile)
		throw std::runtime_error("Settings File: specified file: \"" + fileName + "\" does not exist or is inaccessible");

	std::string line;

	if (infile.is_open())
	{
		Logger(LOG_TRACE) << "Settings File Contents: " << std::endl;

		while (std::getline(infile, line))
		{
			if (line.empty()) continue;
			if (line.substr(0, 2).compare("//") == 0) continue;

			std::string buffer;
			std::stringstream ss(line);
			std::vector<std::string> elements;
			while( ss.good() )
			{
				std::getline(ss, buffer, '=');
				elements.push_back(buffer);
			}

			if (elements.size() >= 2)
			{
				std::string settingName = elements[0];
				std::transform(settingName.begin(), settingName.end(), settingName.begin(), ::tolower);
				std::string settingValue = elements[1];
				std::transform(settingValue.begin(), settingValue.end(), settingValue.begin(), ::tolower);

				if (settingValue.length() == 0) continue;

				Logger(LOG_TRACE) << settingName << ":\t" << settingValue << std::endl;

				if (settingName == "code.filename")
				{
					settingValue.erase(std::remove(settingValue.begin(), settingValue.end(), '\"'), settingValue.end());

					GlobalSettings.CodeFileName = settingValue;
				}
				else if (settingName == "vocabulary.vocabularyfilename")
				{
					settingValue.erase(std::remove(settingValue.begin(), settingValue.end(), '\"'), settingValue.end());

					GlobalSettings.VocabularyVocabularyFileName = settingValue;
				}
				else if (settingName == "vocabulary.statefilename")
				{
					settingValue.erase(std::remove(settingValue.begin(), settingValue.end(), '\"'), settingValue.end());

					GlobalSettings.VocabularyStateFileName = settingValue;
				}
				else if (settingName == "vocabulary.mixturefilename")
				{
					settingValue.erase(std::remove(settingValue.begin(), settingValue.end(), '\"'), settingValue.end());

					GlobalSettings.VocabularyMixtureFileName = settingValue;
				}
				else if (settingName == "bigram.filename")
				{
					settingValue.erase(std::remove(settingValue.begin(), settingValue.end(), '\"'), settingValue.end());

					GlobalSettings.BigramFileName = settingValue;
				}
				else if (settingName == "bigram.fixedpenalty")
				{
					std::replace(settingValue.begin(), settingValue.end(), ',', '.');

					GlobalSettings.BigramFixedPenalty = ::atof(settingValue.c_str());
				}
				else if (settingName == "bigram.crossunitpenalty")
				{
					std::replace(settingValue.begin(), settingValue.end(), ',', '.');

					GlobalSettings.BigramCrossUnitPenalty = ::atof(settingValue.c_str());
				}
				else if (settingName == "decoding.buffersize")
				{
					GlobalSettings.DecodingBufferSize = atoi(settingValue.c_str());
				}
				else if (settingName == "decoding.elementsize")
				{
					GlobalSettings.DecodingElementSize = atoi(settingValue.c_str());
				}
				else if (settingName == "decoding.pruningthreshold")
				{
					std::replace(settingValue.begin(), settingValue.end(), ',', '.');

					GlobalSettings.DecodingPruningThreshold = ::atof(settingValue.c_str());
				}
				else if (settingName == "decoding.samplingrate")
				{
					GlobalSettings.DecodingSamplingRate = atoi(settingValue.c_str());
				}
				else if (settingName == "decoding.numthreads")
				{
					GlobalSettings.DecodingNumThreads = atoi(settingValue.c_str());
				}
				else if (settingName == "transcription.labelformat")
				{
					if (settingValue == "audacity")
					{
						GlobalSettings.TranscriptionLabelFormat = LBLFMT_AUDACITY;
					}
					else
					{
						GlobalSettings.TranscriptionLabelFormat = LBLFMT_HTK;
					}
				}
				else if (settingName == "viterbi.skipstate")
				{
					GlobalSettings.ViterbiSkipState = atoi(settingValue.c_str());
				}
				else if (settingName == "score.acceptancethreshold")
				{
					std::replace(settingValue.begin(), settingValue.end(), ',', '.');

					GlobalSettings.ScoreAcceptanceThreshold = ::atof(settingValue.c_str());
				}
				else if (settingName == "score.usegeometricconfidencescore")
				{
					if (settingValue == "true")
					{
						GlobalSettings.ScoreUseGeometricConfidenceScore = true;
					}
					else
					{
						GlobalSettings.ScoreUseGeometricConfidenceScore = false;
					}
				}
				else if (settingName == "log.loglevel")
				{
					if (settingValue == "log_error")
					{
						GlobalSettings.LogLogLevel = LOG_ERROR;
					}
					else if (settingValue == "log_warn")
					{
						GlobalSettings.LogLogLevel = LOG_WARN;
					}
					else if (settingValue == "log_info")
					{
						GlobalSettings.LogLogLevel = LOG_INFO;
					}
					else if (settingValue == "log_debug")
					{
						GlobalSettings.LogLogLevel = LOG_DEBUG;
					}
					else
					{
						GlobalSettings.LogLogLevel = LOG_TRACE;
					}
				}
				else if (settingName == "speechanalysis.numthreads")
				{
					GlobalSettings.SpeechAnalysisNumThreads = atoi(settingValue.c_str());
				}
			}
		}

		infile.close();
	}
}

void LoadBatchFile(const std::string& fileName)
{
	std::ifstream infile (fileName);

	if (!infile)
		throw std::runtime_error("Batch Input File: specified file: \"" + fileName + "\" does not exist or is inaccessible");

	std::string line;

	if (infile.is_open())
	{
		while (std::getline(infile, line))
		{
			if (line.empty()) continue;
			if (line.substr(0, 2).compare("//") == 0) continue;

			std::string buffer;
			std::stringstream ss(line);
			std::vector<std::string> elements;
			while( ss.good() )
			{
				std::getline(ss, buffer, ',');
				elements.push_back(buffer);
			}

			if (elements.size() >= 1)
			{
				GlobalSettings.DecodingFileNames.push_back(elements[0]);

				if (elements.size() >= 2)
				{
					if (elements[1].compare("htk") == 0)
						GlobalSettings.DecodingSourceTypes.push_back(AUDSRC_HTK);
					else
						GlobalSettings.DecodingSourceTypes.push_back(AUDSRC_WAV);
				}
				else if (GlobalSettings.DecodingSourceTypes.size() > 0)
				{
					// If no source type specified, use the previous line's value if available
					GlobalSettings.DecodingSourceTypes.push_back(
						GlobalSettings.DecodingSourceTypes[GlobalSettings.DecodingSourceTypes.size() - 1]);
				}
				else
				{
					GlobalSettings.DecodingSourceTypes.push_back(AUDSRC_HTK);
				}
			}
		}

		GlobalSettings.DecodingNumFiles = GlobalSettings.DecodingFileNames.size();

		infile.close();
	}
}

void ReadAndInitialiseSpecAnalysisSettings(const std::string& fileName)
{
	std::ifstream infile (fileName);

	if (!infile)
		throw std::runtime_error("Spectral Analysis Settings File: specified file: \"" + fileName + "\" does not exist or is inaccessible");

	std::string line;

	if (infile.is_open())
	{
		while (std::getline(infile, line))
		{
			if (line.empty()) continue;
			if (line.substr(0, 2).compare("//") == 0) continue;

			std::string buffer;
			std::stringstream ss(line);
			std::vector<std::string> elements;
			while( ss.good() )
			{
				std::getline(ss, buffer, ':');
				elements.push_back(buffer);
			}

			if (elements.size() >= 2)
			{
				std::string settingName = elements[0];
				std::transform(settingName.begin(), settingName.end(), settingName.begin(), ::tolower);
				std::string settingValue = elements[1];
				std::transform(settingValue.begin(), settingValue.end(), settingValue.begin(), ::tolower);

				if (settingValue.length() == 0) continue;

				Logger(LOG_TRACE) << settingName << ":\t" << settingValue << std::endl;

				if (settingName == "frame")
				{
					GlobalSettings.SpeechAnalysisParamaters.FrmLength = atoi(settingValue.c_str());
				}
				else if (settingName == "shift")
				{
					GlobalSettings.SpeechAnalysisParamaters.Shift = atoi(settingValue.c_str());
				}
				else if (settingName == "windowing")
				{
					GlobalSettings.SpeechAnalysisParamaters.Window = (atoi(settingValue.c_str()) > 0);
				}
				else if (settingName == "emphasise" || settingName == "emphasize")
				{
					GlobalSettings.SpeechAnalysisParamaters.PreEmp = (atoi(settingValue.c_str()) > 0);
				}
				else if (settingName == "pwr spec")
				{
					GlobalSettings.SpeechAnalysisParamaters.Power = (atoi(settingValue.c_str()) > 0);
				}
				else if (settingName == "channels")
				{
					GlobalSettings.SpeechAnalysisParamaters.nChn = atoi(settingValue.c_str());
				}
				else if (settingName == "bands")
				{
					GlobalSettings.SpeechAnalysisParamaters.nBands = atoi(settingValue.c_str());
				}
				else if (settingName == "low cut")
				{
					GlobalSettings.SpeechAnalysisParamaters.LowCut = atoi(settingValue.c_str());
				}
				else if (settingName == "high cut")
				{
					GlobalSettings.SpeechAnalysisParamaters.HighCut = atoi(settingValue.c_str());
				}
				else if (settingName == "mel")
				{
					GlobalSettings.SpeechAnalysisParamaters.Mel = (atoi(settingValue.c_str()) > 0);
				}
				else if (settingName == "log amp" || settingName == "log") // "Log amp" in v2.06, "Log" in v2.10
				{
					GlobalSettings.SpeechAnalysisParamaters.LogAmp = (atoi(settingValue.c_str()) > 0);
				}
				else if (settingName == "dct")
				{
					GlobalSettings.SpeechAnalysisParamaters.nDCTBand = atoi(settingValue.c_str());
				}
				else if (settingName == "c0")
				{
					GlobalSettings.SpeechAnalysisParamaters.C0 = (atoi(settingValue.c_str()) > 0);
				}
				else if (settingName == "lifter")
				{
					GlobalSettings.SpeechAnalysisParamaters.Lift = atoi(settingValue.c_str());
				}
				else if (settingName == "mean rem")
				{
					GlobalSettings.SpeechAnalysisParamaters.RemoveCepstralMean = (atoi(settingValue.c_str()) > 0);
				}
				else if (settingName == "dc-filter")
				{
					GlobalSettings.SpeechAnalysisParamaters.DCFilt = atoi(settingValue.c_str());
				}
				else if (settingName == "delta (+-)")
				{
					GlobalSettings.SpeechAnalysisParamaters.Delta = atoi(settingValue.c_str());
				}
				else if (settingName == "deldel (+-)")
				{
					GlobalSettings.SpeechAnalysisParamaters.DeltaDelta = atoi(settingValue.c_str());
				}
				else if (settingName == "del only")
				{
					GlobalSettings.SpeechAnalysisParamaters.DelOnly = (atoi(settingValue.c_str()) > 0);
				}
				else if (settingName == "out spec" || settingName == "i/o spec") // "Out spec" in v2.06, "I/O spec" in v2.10
				{
					settingValue.erase(std::remove_if(settingValue.begin(), settingValue.end(), ::isspace), settingValue.end());
					GlobalSettings.SpeechAnalysisParamaters.OutputFileExtension = settingValue;
				}
			}
		}

		infile.close();
	}
}

void ReadAndInitialiseTrainingSettings(const std::string& fileName)
{
	std::ifstream infile (fileName);

	if (!infile)
		throw std::runtime_error("Training Settings File: specified file: \"" + fileName + "\" does not exist or is inaccessible");

	std::string line;

	if (infile.is_open())
	{
		while (std::getline(infile, line))
		{
			if (line.empty()) continue;
			if (line.substr(0, 2).compare("//") == 0) continue;

			std::string buffer;
			std::stringstream ss(line);
			std::vector<std::string> elements;
			bool once = false;
			while( ss.good() )
			{
				if (!once) std::getline(ss, buffer, ':');
				else std::getline(ss, buffer);
				elements.push_back(buffer);
				once = !once;
			}

			if (elements.size() >= 2)
			{
				std::string settingName = elements[0];
				std::transform(settingName.begin(), settingName.end(), settingName.begin(), ::tolower);
				std::string settingValue = elements[1];
				std::transform(settingValue.begin(), settingValue.end(), settingValue.begin(), ::tolower);

				if (settingValue.length() == 0) continue;

				Logger(LOG_TRACE) << settingName << ":\t" << settingValue << std::endl;

				if (settingName == "continuous")
				{
					GlobalSettings.TrainingOptions.Continuous = (bool)atoi(settingValue.c_str());
				}
				else if (settingName == "tie state")
				{
					GlobalSettings.TrainingOptions.TieState = atoi(settingValue.c_str());
				}
				else if (settingName == "skip state")
				{
					GlobalSettings.ViterbiSkipState = atoi(settingValue.c_str());
				}
				else if (settingName == "mdl iter")
				{
					GlobalSettings.TrainingOptions.ModelIterations = std::vector<int>();
					GlobalSettings.TrainingOptions.ModelIterations.resize(3);

					std::string bufferMdlIter;
					std::stringstream ssMdlIter(settingValue);
					std::vector<std::string> elementsMdlIter;

					while (ssMdlIter >> bufferMdlIter) elementsMdlIter.push_back(bufferMdlIter);

					for (int i = 0; i < 3; i++)
					{
						GlobalSettings.TrainingOptions.ModelIterations[i] = atoi(elementsMdlIter[i].c_str());
					}
				}
				else if (settingName == "tr streams")
				{
					GlobalSettings.TrainingOptions.TrStreams = atoi(settingValue.c_str());
				}
				else if (settingName == "min var")
				{
					settingValue.erase(std::remove(settingValue.begin(), settingValue.end(), ' '), settingValue.end());
					GlobalSettings.TrainingOptions.MinimumVariance = std::stod(settingValue.c_str());
				}
				else if (settingName == "mul penalty")
				{
					GlobalSettings.TrainingOptions.MulPenalty = atoi(settingValue.c_str());
				}
				else if (settingName == "add penalty")
				{
					GlobalSettings.TrainingOptions.AddPenalty = atoi(settingValue.c_str());
				}
				else if (settingName == "n-best")
				{
					GlobalSettings.TrainingOptions.NBest = std::vector<int>();
					GlobalSettings.TrainingOptions.NBest.resize(3);

					std::string bufferNBest;
					std::stringstream ssNBest(settingValue);
					std::vector<std::string> elementsNBest;

					while (ssNBest >> bufferNBest) elementsNBest.push_back(bufferNBest);

					for (int i = 0; i < 2; i++)
					{
						GlobalSettings.TrainingOptions.NBest[i] = atoi(elementsNBest[i].c_str());
					}
				}
				else if (settingName == "output dir")
				{
					settingValue.erase(std::remove(settingValue.begin(), settingValue.end(), ' '), settingValue.end());
					GlobalSettings.TrainingOptions.OutputDirectory = settingValue;
				}
				else if (settingName == "model dir")
				{
					settingValue.erase(std::remove(settingValue.begin(), settingValue.end(), ' '), settingValue.end());
					GlobalSettings.TrainingOptions.OutputDirectory += "/" + settingValue;
				}
				else if (settingName == "master label")
				{
					settingValue.erase(std::remove(settingValue.begin(), settingValue.end(), ' '), settingValue.end());
					GlobalSettings.TrainingOptions.MasterLabelFile = settingValue;
				}
				else if (settingName == "double mixtures")
				{
					GlobalSettings.TrainingOptions.DoubleMixtures = atoi(settingValue.c_str());
				}
				else if (settingName == "max audio dur")
				{
					GlobalSettings.TrainingMaxLengthMillisecondsPerFile = atoi(settingValue.c_str());
				}
			}
		}

		infile.close();
	}
}

void LoadComparisonFile(const std::string& fileName)
{
	std::ifstream infile (fileName);

	if (!infile)
		throw std::runtime_error("Comparison Input File: specified file: \"" + fileName + "\" does not exist or is inaccessible");

	std::string line;

	bool first = true;
	if (infile.is_open())
	{
		while (std::getline(infile, line))
		{
			if (line.empty()) continue;
			if (line.substr(0, 2).compare("//") == 0) continue;

			if (first)
			{
				line.erase(std::remove(line.begin(), line.end(), '\"'), line.end());
				GlobalSettings.ScoreOutputDirectory = line;
				first = false;
				continue;
			}

			std::string buffer;
			std::stringstream ss(line);
			std::vector<std::string> elements;
			while( ss.good() )
			{
				std::getline(ss, buffer, ',');
				elements.push_back(buffer);
			}

			if (elements.size() >= 1)
			{
				ScoreLabelPair labels = ScoreLabelPair();
				labels.Test = elements[0];

				if (elements.size() >= 2)
				{
					labels.Reference = (elements[1]);
				}
				else if (GlobalSettings.ScoreLabels.size() > 0)
				{
					// If no proper label specified, use the previous line's value if available
					labels.Reference =
						GlobalSettings.ScoreLabels[GlobalSettings.ScoreLabels.size() - 1].Reference;
				}
				else
				{
					throw std::runtime_error("Comparison File: no proper label file specified for result label file: " + elements[0]);
				}

				if (elements.size() >= 3 && elements[2] != "")
				{
					labels.StartOffsetMilliseconds = (unsigned long)std::stol(elements[2].c_str());
				}
				else
				{
					labels.StartOffsetMilliseconds = 0;
				}

				if (elements.size() >= 4 && elements[3] != "")
				{
					labels.EndOffsetMilliseconds = (unsigned long)std::stol(elements[3].c_str());
				}
				else
				{
					labels.EndOffsetMilliseconds = 0;
				}

				GlobalSettings.ScoreLabels.push_back(labels);
			}
		}

		infile.close();
	}
}
