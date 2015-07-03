#pragma once

#include <stdexcept>
#include <string>
#include <vector>

#include <boost/date_time.hpp>
#include <boost/thread.hpp>

#include "BufferedASignal.h"
#include "FeatureVectorLoader.h"
#include "Logger.h"
#include "Semaphore.h"
#include "SIG.h"
#include "Transcribe.h"
#include "ViterbiDecoder.h"

class ThreadedDecoder
{
public:
	ThreadedDecoder(const std::string& fileName, int bufferSize, IFeatureVectorLoader* featureVectorLoader);
	~ThreadedDecoder();

	void StartDecoding();

private:
	ThreadedDecoder();

	void Init(const std::string& fileName, IFeatureVectorLoader* featureVectorLoader);

	std::string FileName;

	BufferedASignal Buffer;

	IFeatureVectorLoader* FeatureVectorLoader;

    std::vector<PUnit> TUnit;
};

