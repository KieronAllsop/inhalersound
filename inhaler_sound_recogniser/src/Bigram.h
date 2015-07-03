#pragma once

#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "Global.h"
#include "Vocabulary.h"

typedef struct 
{
	unsigned short int nSource;
    std::vector<unsigned short int> Source;
    std::vector<float> Probabilities;
} BGMUnit;

class TBigram
{
public:
	TBigram(const std::string& fileName);
	~TBigram();

	std::vector<BGMUnit> To;

	int nBeginUnits;
	std::vector<int> BeginUnits;

	int nEndUnits;
	std::vector<int> EndUnits;

private:
	void Load(const std::string& fileName);

	void Normalise();

    void ToLogScale();

    void ApplyPenaltyScale();
    
    void ApplyFixedPenalty();
};

extern TBigram* Bigram;
