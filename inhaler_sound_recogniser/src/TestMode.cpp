#include "TestMode.h"

#include "Bigram.h"
#include "Code.h"
#include "FeatureVectorLoader.h"
#include "Helper.h"
#include "HTKData.h"
#include "Logger.h"
#include "Model.h"
#include "ThreadedDecoder.h"
#include "WAVData.h"

TTestMode::TTestMode()
{
	Code = new TCode();
	Logger(LOG_INFO) << "Start loading Code definition" << std::endl;
	Code->Load(GlobalSettings.CodeFileName);
	Logger(LOG_DEBUG) << "nStates: " << Code->nStates << "\nnStreams: " << Code->nStreams << "\nStreamSize: " << Code->StreamSize << std::endl;
	Logger(LOG_INFO) << "Finish loading Code definition" << std::endl;

	Vocabulary = new TVocabulary(
		GlobalSettings.VocabularyVocabularyFileName, 
		GlobalSettings.VocabularyStateFileName, 
		GlobalSettings.VocabularyMixtureFileName);
	Logger(LOG_INFO) << "Start loading Vocabulary, Mixture and State definitions" << std::endl;
	Logger(LOG_DEBUG) << "nUnits: " << Vocabulary->nUnits << std::endl;
	Logger(LOG_INFO) << "Finish loading Vocabulary, Mixture and State definitions" << std::endl;

	Model = new TModel();
	Logger(LOG_INFO) << "Start loading Model" << std::endl;
	Model->MatchVocabulary();
	Logger(LOG_DEBUG) << "nStates: " << Model->nStates << std::endl;
	Logger(LOG_INFO) << "Finish loading Model" << std::endl;

	Logger(LOG_INFO) << "Start loading Bigram" << std::endl;
	Bigram = new TBigram(GlobalSettings.BigramFileName);
	Logger(LOG_DEBUG) << "nBeginUnits: " << Bigram->nBeginUnits << ", nEndUnits: " << Bigram->nEndUnits << std::endl;
	Logger(LOG_INFO) << "Finish loading Bigram" << std::endl;
}


TTestMode::~TTestMode()
{
}

void 
TTestMode::Run()
{
	nThreads = (GlobalSettings.DecodingNumFiles >= GlobalSettings.DecodingNumThreads) ? 
		(GlobalSettings.DecodingNumThreads) : (GlobalSettings.DecodingNumFiles);
	
	nFilesPerThread = std::vector<int>();
	nFilesPerThread.resize(nThreads);
	int nDecFilesDivNumThreads = (GlobalSettings.DecodingNumFiles > nThreads) ? 
		GlobalSettings.DecodingNumFiles / GlobalSettings.DecodingNumThreads : 1;
	for (int i = 0; i < nThreads; i++)
	{
		if (i == nThreads - 1)
		{
			nFilesPerThread[i] = GlobalSettings.DecodingNumFiles - (nDecFilesDivNumThreads * i);
		}
		else
		{
			nFilesPerThread[i] = nDecFilesDivNumThreads;
		}

		Logger(LOG_DEBUG) << "Thread: " << i << " to decode " << nFilesPerThread[i] << " files" << std::endl;
	}

	Progress = std::vector<int>();
	Progress.resize(nThreads);
	nFinished = 0;

	boost::thread_group tgroup;
	try
	{
		Logger(LOG_INFO) << std::endl << "-------------------------------------------------" << std::endl;
		Logger(LOG_INFO) << "Start recognition: " << Helper::CurrentDateTime() << std::endl;
		Logger(LOG_INFO) << "Using " << nThreads << " threads" << std::endl;

		int start = 0;
		for (int i = 0; i < nThreads; i++)
		{
			int end = (start + nFilesPerThread[i]);

			tgroup.create_thread(boost::bind(&TTestMode::DecodeRange, this, i, start, end));

			start += nFilesPerThread[i];
		}

		tgroup.create_thread(boost::bind(&TTestMode::DisplayProgress, this));

		tgroup.join_all();

		Logger(LOG_INFO) << std::endl << "Viterbi recognition complete: " << Helper::CurrentDateTime() << std::endl;
		Logger(LOG_INFO) << std::endl << "-------------------------------------------------" << std::endl;
	}
	catch (std::exception& e)
	{
		tgroup.interrupt_all();
		throw;
	}
}

void 
TTestMode::DecodeRange(int threadIndex, int startIndex, int endIndex)
{
	std::stringstream ss;

	ss << "Thread started with range: " << startIndex << " to " << endIndex << std::endl;
	Logger(LOG_DEBUG) << ss.str();
	ss.str(std::string());

	ThreadedDecoder* Decoder;

	for (int i = startIndex; i < endIndex; i++)
	{
		IFeatureVectorLoader *featureVectorLoader;
		if (GlobalSettings.DecodingSourceTypes[i] == AUDSRC_HTK)
		{
			featureVectorLoader = new HTKData();
		}
		else if (GlobalSettings.DecodingSourceTypes[i] == AUDSRC_WAV)
		{
			featureVectorLoader = new WAVData();
		}
			
		try
		{
			ss << "Thread: " << threadIndex << ": Start Decoding index: " << i << std::endl;
			Logger(LOG_TRACE) << ss.str();
			ss.str(std::string());
			Decoder = new ThreadedDecoder(GlobalSettings.DecodingFileNames[i], GlobalSettings.DecodingBufferSize, featureVectorLoader);
			Decoder->StartDecoding();
			ss << "Thread: " << threadIndex << ": Finish Decoding index: " << i << std::endl;
			Logger(LOG_TRACE) << ss.str();
			ss.str(std::string());
		}
		catch (std::exception& e)
		{
			delete Decoder;
			throw;
		}

		delete Decoder;

		Progress[threadIndex] = (i - startIndex) + 1;
		nFinished.fetch_add(1);
	}
}

void 
TTestMode::DisplayProgress()
{
	int progress;
	std::stringstream ss;

	int previous = -1;
	while ((progress = nFinished) < GlobalSettings.DecodingNumFiles)
	{
		if (progress != previous)
		{
			for (int i = 0; i < nThreads; i++)
			{
				ss << "Thread " << i << ": " << Progress[i] << "/" << nFilesPerThread[i] << "\t";
			}
			ss << std::endl;

			Logger(LOG_INFO) << ss.str();

			Logger(LOG_INFO).flush();

			previous = progress;
			ss.str(std::string());
		}
		else
		{
			boost::this_thread::sleep(boost::posix_time::milliseconds(150));
		}
	}
}
