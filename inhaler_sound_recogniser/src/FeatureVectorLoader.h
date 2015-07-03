#pragma once

#include <string>
#include <vector>

#include "BufferedASignal.h"
#include "SIG.h"
#include "Semaphore.h"

class IFeatureVectorLoader
{
public:
	IFeatureVectorLoader() {}
	virtual ~IFeatureVectorLoader() {}

	virtual void ExtractToBuffer(const std::string& fileName, BufferedASignal* buffer) = 0;
};
