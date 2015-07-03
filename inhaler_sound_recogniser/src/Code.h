#pragma once

#include <stdexcept>
#include <string>

#include "GMM.h"
class TCode
{
public:
	TCode();
	~TCode();

	void Load(const std::string& fileName);
	void Save(const std::string& fileName);

	std::vector<TGaussianMixture> States;
	int nStates;
	int nStreams;
	int StreamSize;
	int VectorSize;

	// Vocabulary must be loaded at this point
	void CreateEmpty(int numStates, int numStreams, int streamSize);

	void Zero(std::vector<TGaussianMixture>& states);

	int GetMaxNumGaussians();

	// Code accumulator
	std::vector<TGaussianMixture> AStates;
};

extern TCode* Code;
