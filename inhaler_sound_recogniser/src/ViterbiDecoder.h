#pragma once

#include <vector>

#include "Bigram.h"
#include "BufferedASignal.h"
#include "FeatureVectorConsumer.h"
#include "Logger.h"
#include "Model.h"
#include "Vocabulary.h"

class TViterbiDecoder : public IFeatureVectorConsumer
{
public:
	TViterbiDecoder();
	~TViterbiDecoder();

	virtual void DecodeFromBuffer(BufferedASignal* buffer);

	// These two don't use multi-threading with producer as the audio samples will be so small
	void Uniform(ASegment* chunk);
    void Align(ASegment* chunk);

	float LogPr(ASegment* chunk, int t, unsigned int s);
	double Gauss(ASegment* chunk, int t, unsigned int s, int k);
	double Gauss(ASegment* chunk, int t, unsigned int s, int k, int d);

	std::vector<unsigned int> State; // The best sequence of states
	float Pr;

	std::vector<float> ProbSequence;

	// Used in training
	// Probability records
    std::vector<std::vector<double>> Prob_Time_State;
    std::vector<std::vector<std::vector<double>>> Prob_Time_State_Mix;

    void Space(ASegment* chunk); // Space for probability records

	double Pr0(ASegment* chunk, int t, unsigned int s); // Probability of frame t in state s, with values recorded
    double Pr0R(int t, unsigned int s); // Retrieve recorded probability of frame t in state s
    double Pr0R(int t, unsigned int s, int k); // Retrieve recorded probability of frame t in state s and mixture k
    float LogPr0(ASegment* chunk, int t, unsigned int s); // Log probability of frame t in state s, with values recorded

private:
	// For each state s, holds the best predecessor at time t
	std::vector<std::vector<unsigned int>> Path;
	// For each state s, holds the current path cost 
	std::vector<float> Prob;

	// For each state s, holds the probability of the being in state s at time t
	std::vector<std::vector<float>> ProbFrameAtStateTime; 

	// For each word in the vocabulary, holds the path cost of the last state of the best preceding word
   	std::vector<float> BestPreUnitProb;
	// For each word in the vocabulary, holds the last state of the best preceding word
   	std::vector<int> BestPreUnitState;

	unsigned int nFrames;

	// Viterbi algorithm
	void ViterbiInitialisation(ASegment* chunk);
	void ViterbiRecursion(ASegment* chunk, unsigned int timeFrom, unsigned int timeTo);
	void ViterbiTermination();

	void SetState();
    void SetProb();
    void SetProbSequence();
    void SetProbFrameAtStateTime();
    void SetPath();
    void SetBestPre();
};

