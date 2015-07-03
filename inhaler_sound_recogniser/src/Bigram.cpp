#include "Bigram.h"


TBigram::TBigram(const std::string& fileName)
{
	Load(fileName);
}


TBigram::~TBigram()
{
}

void 
TBigram::Load(const std::string& fileName)
{
	std::ifstream infile (fileName);

	if (!infile)
		throw std::runtime_error("Bigram: specified Bigram Model file: \"" + fileName + "\" does not exist or is inaccessible");

	std::string line;

	nBeginUnits = 0;
	nEndUnits = 0;
	BeginUnits = std::vector<int>();
	EndUnits = std::vector<int>();
	To = std::vector<BGMUnit>();
	To.resize(Vocabulary->nUnits);

	if (infile.is_open())
	{
		try
		{
			while (std::getline(infile, line))
			{
				if (line.empty()) continue;
				if (line.substr(0, 2).compare("//") == 0) continue;

				std::string buffer;
				std::stringstream ss(line);
				std::vector<std::string> elements;
				
				// Split current line by whitespace
				while (ss >> buffer)
					elements.push_back(buffer);

				// Begin Units
				if (line.substr(0, 3).compare("B-:") == 0)
				{
					elements.erase(elements.begin()); // The first element will be "B-:"

					nBeginUnits = elements.size();

					if (nBeginUnits > 0)
					{
						BeginUnits.resize(nBeginUnits);
						for (int n = 0; n < nBeginUnits; n++)
						{
							BeginUnits[n] = Vocabulary->GetUnitPositionByName(elements[n]);
							if (BeginUnits[n] < 0)
							{
								std::stringstream strm; strm << "Bigram: Begin Unit: \"" << elements[n] << "\" not found in Vocabulary";
								throw std::runtime_error(strm.str());
							}
						}
					}
				}

				// End Units
				if (line.substr(0, 3).compare("E-:") == 0)
				{
					elements.erase(elements.begin()); // The first element will be "E-:"

					nEndUnits = elements.size();

					if (nEndUnits > 0)
					{
						EndUnits.resize(nEndUnits);
						for (int n = 0; n < nEndUnits; n++)
						{
							EndUnits[n] = Vocabulary->GetUnitPositionByName(elements[n]);
							if (EndUnits[n] < 0)
							{
								std::stringstream strm; strm << "Bigram: End Unit: \"" << elements[n] << "\" not found in Vocabulary";
								throw std::runtime_error(strm.str());
							}
						}
					}
				}

				// Bigram transitions

				// Target word
				std::string word = elements[0];
				elements.erase(elements.begin());
				int wordIndex = Vocabulary->GetUnitPositionByName(word);
				if (wordIndex < 0)
				{
					std::stringstream strm; strm << "Bigram: Target Unit: \"" << word << "\" not found in Vocabulary";
					throw std::runtime_error(strm.str());
				}

				// Number of source units
				To[wordIndex].nSource = elements.size();

				// Source units
				if (elements.size() > 0)
				{
					To[wordIndex].Source.resize(To[wordIndex].nSource);
					To[wordIndex].Probabilities.resize(To[wordIndex].nSource);

					for (int n = 0; n < To[wordIndex].nSource; n++)
					{
						int v = Vocabulary->GetUnitPositionByName(elements[n]);
						if (v < 0)
						{
							std::stringstream strm; strm << "Bigram: Source Unit: \"" << elements[n] << "\" for " << 
								"Target Unit: \"" << word << "\" not found in Vocabulary";
							throw std::runtime_error(strm.str());
						}
						To[wordIndex].Source[n] = (unsigned short int)v;
						To[wordIndex].Probabilities[n] = 1.0;
					}
				}

			}
		}
		catch (std::exception e)
		{
			infile.close();
			throw;
		}
	}

	Normalise();

	ToLogScale();

	ApplyPenaltyScale();
	ApplyFixedPenalty();

	if (nBeginUnits == 0)
	{
		nBeginUnits = Vocabulary->nUnits;
		BeginUnits.resize(nBeginUnits);
		
		for (int n = 0; n < Vocabulary->nUnits; n++)
		{
			BeginUnits[n] = n;
		}
	}

	if (nEndUnits == 0)
	{
		nEndUnits = Vocabulary->nUnits;
		EndUnits.resize(nEndUnits);

		for (int n = 0; n < Vocabulary->nUnits; n++)
		{
			EndUnits[n] = n;
		}
	}
}

void 
TBigram::Normalise()
{
	// Source
	for (int v = 0; v < Vocabulary->nUnits; v++)
	{
		float p = 0.0;

		// Target
		for (int w = 0; w < Vocabulary->nUnits; w++)
		{
			for (int n = 0; n < To[w].nSource; n++)
			{
				if (To[w].Source[n] == v)
				{
					p += To[w].Probabilities[n];
				}
			}
		}

		// Normalise
		for (int w = 0; w < Vocabulary->nUnits; w++)
		{
			for (int n = 0; n < To[w].nSource; n++)
			{
				if (To[w].Source[n] == v)
				{
					To[w].Probabilities[n] /= p;
				}
			}
		}
	}
}

void
TBigram::ToLogScale()
{
	for(int n = 0; n < Vocabulary->nUnits; n++) 
	{
		for(int m = 0; m < To[n].nSource; m++)
		{
			To[n].Probabilities[m] = (To[n].Probabilities[m] == 0.0) ? BIGNEG : log(To[n].Probabilities[m]);
		}
	}
}

void 
TBigram::ApplyPenaltyScale()
{
	if (GlobalSettings.BigramCrossUnitPenalty == 1.0)
		return;

	for (int n = 0; n < Vocabulary->nUnits; n++)
	{
		for (int m = 0; m < To[n].nSource; m++)
		{
			if (To[n].Probabilities[m] > BIGNEG)
			{
				To[n].Probabilities[m] *= GlobalSettings.BigramCrossUnitPenalty;
			}
		}
	}
}

void 
TBigram::ApplyFixedPenalty()
{
	for (int n = 0; n < Vocabulary->nUnits; n++)
	{
    	for (int m = 0; m < To[n].nSource; m++)
		{
         	if (To[n].Probabilities[m] > BIGNEG)
			{
				To[n].Probabilities[m] += GlobalSettings.BigramFixedPenalty;
			}
		}
    }
}
