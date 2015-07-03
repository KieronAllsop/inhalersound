#pragma once

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "BufferedASignal.h"
#include "FeatureVectorLoader.h"
#include "Global.h"
#include "Logger.h"
#include "SIG.h"

typedef struct 
{
	int nSamples;
	int sampPeriod; // value of 1 = 100ns
	short int sampSize;
	short int parmKind;
} HTKHeader;

class HTKData : public IFeatureVectorLoader
{
public:
	HTKData();
	~HTKData();

	virtual void ExtractToBuffer(const std::string& fileName, BufferedASignal* buffer);

	ASegment LoadCompleteAcousticAndLabelHTKData(const std::string& acousticFileName, const std::string& labelFileName);
	void LoadCompleteAcousticHTKData(const std::string& fileName, ASegment& aSegment);
	void LoadCompleteLabelHTKData(const std::string& fileName, ASegment& aSegment);

	void ScaleLabelBoundaries(ASegment& aSegment);

	void WriteAcousticHTKData(const std::string& fileName, ASegment& aSegment, int sampPeriod);

	HTKHeader ReadHeaderOnly(const std::string& fileName);
};

