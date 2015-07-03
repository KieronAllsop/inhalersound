#include "SpecAnalysisMode.h"

#include <boost/date_time.hpp>
#include <boost/filesystem.hpp>
#include <boost/thread.hpp>

#include "Global.h"
#include "SpecAnalysis.h"
#include "WAVData.h"


TSpecAnalysisMode::TSpecAnalysisMode()
{
}

TSpecAnalysisMode::~TSpecAnalysisMode()
{
}

void 
TSpecAnalysisMode::Run()
{
	int nThreads = (GlobalSettings.DecodingNumFiles >= GlobalSettings.SpeechAnalysisNumThreads) ? 
		(GlobalSettings.SpeechAnalysisNumThreads) : (GlobalSettings.DecodingNumFiles);
	
	std::vector<int> nFilesPerThread = std::vector<int>();
	nFilesPerThread.resize(nThreads);
	int nDecFilesDivNumThreads = (GlobalSettings.DecodingNumFiles > nThreads) ? 
		GlobalSettings.DecodingNumFiles / GlobalSettings.SpeechAnalysisNumThreads : 1;
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

	nFinished = 0;

	boost::thread_group tgroup;
	try
	{
		Logger(LOG_INFO) << "Start feature calculation" << std::endl;
		Logger(LOG_INFO) << "Using " << nThreads << " threads" << std::endl;

		int start = 0;
		for (int i = 0; i < nThreads; i++)
		{
			int end = (start + nFilesPerThread[i]);

			tgroup.create_thread(boost::bind(&TSpecAnalysisMode::ConvertRange, this, start, end));

			start += nFilesPerThread[i];
		}

		tgroup.create_thread(boost::bind(&TSpecAnalysisMode::DisplayProgress, this));

		tgroup.join_all();

		Logger(LOG_INFO) << std::endl << "Feature calculation complete" << std::endl;
	}
	catch (std::exception& e)
	{
		tgroup.interrupt_all();
		throw;
	}
}

void 
TSpecAnalysisMode::ConvertRange(int startIndex, int endIndex)
{
	TSpecAnalysis SpecAnalysis = TSpecAnalysis();
	WAVData wavReader = WAVData();

	std::stringstream ss;

	ss << "Thread started with range: " << startIndex << " to " << endIndex << std::endl;
	Logger(LOG_DEBUG) << ss.str();
	ss.str(std::string());

	for (int i = startIndex; i < endIndex; i++)
	{
		std::string fileName = GlobalSettings.DecodingFileNames[i];
		
		std::string outFeaturesFileName = fileName;
		// Remove extension if present
		int periodIndex = outFeaturesFileName.rfind('.');
		if (std::string::npos != periodIndex)
		{
			outFeaturesFileName.erase(periodIndex);
		}

		outFeaturesFileName += "." + GlobalSettings.SpeechAnalysisParamaters.OutputFileExtension;

		ss << "About to calculate features for: Index: " << i << ", File: " << 
			fileName << ", Out: " << outFeaturesFileName << std::endl;
		Logger(LOG_TRACE) << ss.str();
		ss.str(std::string());

		if (GlobalSettings.DecodingSourceTypes[i] == AUDSRC_WAV)
		{
			WAV wav = wavReader.ReadFullWAV(fileName);
			
			SpecAnalysis.Execute(wav, outFeaturesFileName);
		}
		else
		{
			std::stringstream ss;
			ss << "Info: Skipping feature calculation for audio file: " << fileName << 
				" as the file is not in WAV format" << std::endl;
			Logger(LOG_INFO) << ss.str();
		}

		ss << "Calculated features for: Index: " << i << ", File: " << 
			fileName << ", Out: " << outFeaturesFileName << std::endl;
		Logger(LOG_TRACE) << ss.str();
		ss.str(std::string());

		nFinished.fetch_add(1);
	}
}

void 
TSpecAnalysisMode::DisplayProgress()
{
	int progress;

	int previous = -1;
	while ((progress = nFinished) < GlobalSettings.DecodingNumFiles)
	{
		if (progress != previous)
		{
			int percent = (progress * 100) / GlobalSettings.DecodingNumFiles;
			int remaining = 100 - percent;

			Logger(LOG_INFO) << "\r" << progress << "/" << GlobalSettings.DecodingNumFiles << ": " << percent << "% completed: ";

			Logger(LOG_INFO) << std::string(percent / 2, '|');
			Logger(LOG_INFO) << std::string(remaining / 2, '-');

			Logger(LOG_INFO).flush();

			previous = progress;
		}
		else
		{
			boost::this_thread::sleep(boost::posix_time::milliseconds(150));
		}
	}
}
