#pragma once

#include <stdexcept>
#include <vector>

#include "Code.h"
#include "GMM.h"
#include "Vocabulary.h"
#include "SIG.h"

class TModel
{
public:
	TModel();
	~TModel();

	std::vector<TGaussianMixture*> States;
	unsigned int nStates;
    short int nStreams;
    short int StreamSize;

	void MatchVocabulary();

	// Steps of states transition into and out of each state (used only in training)
    std::vector<short int> TransitionsIn;
    std::vector<short int> TransitionsOut;

	// Model accumulator
	std::vector<TGaussianMixture*> AStates;

    // Model logical states (used only in training)
    std::vector<unsigned int> LStates;

	void MatchLabel(ASegment& aSegment);
};

extern TModel* Model;

