#pragma once

#include <vector>

#include "SIG.h"
#include "Vocabulary.h"

class TTranscribe
{
public:
	TTranscribe();
	~TTranscribe();

	// Transcribe given state sequence into U
    void State2Unit(std::vector<unsigned int>* StateSeq, std::vector<PUnit>* U);

	void MergeIgnoredUnits(std::vector<PUnit>& U);

	void RemoveIgnoredUnits(std::vector<PUnit>& U);

	void OffsetBoundaries(std::vector<PUnit>& U, unsigned long millisecondsOffset);

	void RenameUnitsByGroup(std::vector<int>& group, std::vector<PUnit>& U, const std::string& name);

	void MergeUnitsByGroup(const std::string& label, std::vector<PUnit>& U);

	void AddFillerToGaps(std::vector<PUnit>& U);

	void OutputLabel(const std::string& fileName, std::vector<PUnit>& U);
};

