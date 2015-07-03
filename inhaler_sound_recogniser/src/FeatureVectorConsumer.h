#pragma once

#include <string>
#include <vector>

#include "BufferedASignal.h"

class IFeatureVectorConsumer
{
public:
	IFeatureVectorConsumer() {}
	virtual ~IFeatureVectorConsumer() {}

	virtual void DecodeFromBuffer(BufferedASignal* buffer) = 0;
};

