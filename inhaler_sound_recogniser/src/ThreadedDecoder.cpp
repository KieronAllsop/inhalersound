#include "ThreadedDecoder.h"

#include "Score.h"

ThreadedDecoder::ThreadedDecoder(const std::string& fileName, int bufferSize, IFeatureVectorLoader* featureVectorLoader) 
	: Buffer(bufferSize)
{
	Init(fileName, featureVectorLoader);
}

ThreadedDecoder::~ThreadedDecoder(void)
{
	delete FeatureVectorLoader;
}

void ThreadedDecoder::Init(const std::string& fileName, IFeatureVectorLoader* featureVectorLoader)
{
	FeatureVectorLoader = featureVectorLoader;

	FileName = fileName;

	TUnit = std::vector<PUnit>();
}

void ThreadedDecoder::StartDecoding()
{
	boost::thread_group tgroup;
	Logger(LOG_DEBUG) << "About to start feature loading/extraction and recognition" << std::endl;
	TViterbiDecoder viterbiDecoder;
	try
	{
		tgroup.create_thread(boost::bind(&IFeatureVectorLoader::ExtractToBuffer, FeatureVectorLoader, FileName, &Buffer));
	
		viterbiDecoder = TViterbiDecoder();
	
		tgroup.create_thread(boost::bind(&TViterbiDecoder::DecodeFromBuffer, &viterbiDecoder, &Buffer));
	
		tgroup.join_all();
	}
	catch (std::exception& e)
	{
		tgroup.interrupt_all();
		throw;
	}

	if (viterbiDecoder.ProbSequence.size() == 0)
	{
		// Can't catch exceptions between threads so have to check whether there was an error 
		// by checking whether the sequence of states is empty
		throw std::runtime_error("Recognition error");
	}

	Logger(LOG_INFO) << "Recognition complete" << std::endl;
	
	// Transcribe
	std::vector<PUnit> U = std::vector<PUnit>();
	TTranscribe transcribe = TTranscribe();
	transcribe.State2Unit(&viterbiDecoder.State, &U);

	// Calculate confidence scores
	TScore score = TScore();
	bool acceptKeywords = score.Verify(&U, &viterbiDecoder.ProbSequence, false);

	std::string labelName = FileName;
	int periodIndex = labelName.rfind('.');
	if (std::string::npos != periodIndex)
	{
		labelName.erase(periodIndex);
	}
	labelName += ".lbl";

	// Output transcription to label file
	transcribe.OutputLabel(labelName, U);
	Logger(LOG_INFO) << "Saved transcription to: " << labelName << std::endl;

	if (!acceptKeywords)
	{
		Logger(LOG_INFO) << std::endl << FileName << std::endl << "No keywords found." << std::endl;
	}
}
