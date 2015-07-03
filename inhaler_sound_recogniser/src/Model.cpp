#include "Model.h"
#include "Global.h"


TModel::TModel()
{
	States = std::vector<TGaussianMixture*>();
	nStates = 0;
	nStreams = Code->nStreams;
	StreamSize = Code->StreamSize;
}

TModel::~TModel()
{
}

void
TModel::MatchVocabulary()
{
	// nStates of the vocabulary
	nStates = Vocabulary->nLogicalStates;

    // Create model
    States.resize(nStates);
    unsigned int i = 0;
    for (int n = 0; n < Vocabulary->nUnits; n++)
	{
		for (int j = 0; j < Vocabulary->Units[n].nStates; j++)
		{
			States[i++] = &Code->States[Vocabulary->Units[n].PhysicalStates[j]];
		}
	}
}

void
TModel::MatchLabel(ASegment& aSegment)
{
    	// Positions of units in vocabulary
	std::vector<int> k = std::vector<int>();
	k.resize(aSegment.nUnits);
	for (int n = 0; n < aSegment.nUnits; n++)
	{
		int unitInd = Vocabulary->GetUnitPositionByName(aSegment.Units[n].Label);
		if (unitInd < 0)
		{
			aSegment.Units.erase(aSegment.Units.begin() + n);
			aSegment.nUnits--;
		}
    	k[n] = unitInd;
	}

	// nStates of the model
	nStates = 0;
	for(int n = 0; n < aSegment.nUnits; n++)
	{
	    nStates += Vocabulary->Units[k[n]].nStates;
	}

	// Create default state transition steps for each state
	TransitionsIn = std::vector<short int>();
	TransitionsIn.resize(nStates);
	TransitionsOut = std::vector<short int>();
	TransitionsOut.resize(nStates);

	for (unsigned int i = 0; i < nStates; i++)
	{
		TransitionsIn[i] = i - 1 - GlobalSettings.ViterbiSkipState;
	    if (TransitionsIn[i] < 0)
	    {
	        TransitionsIn[i] = 0;
	    }
		TransitionsOut[i] = i + 1 + GlobalSettings.ViterbiSkipState;
	    if (TransitionsOut[i] >= (int)nStates)
	    {
	        TransitionsOut[i] = nStates - 1;
	    }
	}

	// Create model & accumulator
	States = std::vector<TGaussianMixture*>();
	States.resize(nStates);
    AStates = std::vector<TGaussianMixture*>();
	AStates.resize(nStates);

	// Create model logical states
	LStates = std::vector<unsigned int>();
	LStates.resize(nStates);

	unsigned int i = 0;
	for (int n = 0; n < aSegment.nUnits; n++)
	{
	    // Add a unit to the model
		std::vector<int>* s = &(Vocabulary->Units[k[n]].PhysicalStates);
	    for (int j = 0; j < Vocabulary->Units[k[n]].nStates; j++)
	    {
	        States[i] = &(Code->States[(*s)[j]]);
	        AStates[i] = &(Code->AStates[(*s)[j]]);
			LStates[i] = Vocabulary->Units[k[n]].StartLogicalPosition + j;
	        i++;
	    }
	}
}
