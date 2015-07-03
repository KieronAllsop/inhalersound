#include <fstream>
#include <iostream>
#include <sstream>
#include <cmath>

#include "Code.h"

#include "Vocabulary.h"

TCode::TCode()
{
}


TCode::~TCode()
{
}

void TCode::Load(const std::string& fileName)
{
	std::ifstream infile(fileName);

	if (!infile)
		throw std::runtime_error("Code: specified Code file: \"" + fileName + "\" does not exist or is inaccessible");

	if (infile.is_open())
	{
		try
		{
			infile.ignore(20,'='), infile >> nStates;
			infile.ignore(20,'='), infile >> nStreams;
			infile.ignore(20,'='), infile >> StreamSize;

			VectorSize = nStreams * StreamSize;

			States.resize(nStates);

			int stateIndex = -1;
			int stateNumMix = -1;

			int mixIndex = -1;
			int nStrmsInMix = -1;

			bool needToCalculateLogDet = false;

			std::string line;

			bool noReadLine = false;

			readLine:
			while (noReadLine || std::getline(infile, line))
			{
				if (noReadLine) noReadLine = false;

				if (line.empty()) continue;
				if (line.substr(0, 2).compare("//") == 0) continue;

				std::string buffer;
				std::stringstream ss(line);
				std::vector<std::string> elements;

				// Split current line by whitespace
				while (ss >> buffer)
					elements.push_back(buffer);

				if (elements.size() == 0) continue;

				if (elements[0] == "State:")
				{
					stateIndex = atoi(elements[1].c_str());
					stateNumMix = atoi(elements[2].substr(5, std::string::npos).c_str()); // nMix=...

					States[stateIndex].nGaussians = stateNumMix;
					States[stateIndex].Gaussians.resize(stateNumMix);

					continue;
				}
				else if (elements[0] == "Mix:")
				{
					mixIndex = atoi(elements[1].c_str());
					nStrmsInMix = atoi(elements[2].substr(7, std::string::npos).c_str()); // nStrms=...

					States[stateIndex].Gaussians[mixIndex] = TGaussian();

					States[stateIndex].Gaussians[mixIndex].Mean.resize(nStrmsInMix * StreamSize);
					States[stateIndex].Gaussians[mixIndex].Covariance.resize(nStrmsInMix * StreamSize);
					States[stateIndex].Gaussians[mixIndex].Determinant.resize(nStrmsInMix);

					std::vector<std::string> mixLines = std::vector<std::string>();

					std::string mixLine;
					while (std::getline(infile, mixLine))
					{
						if (mixLine.empty())
						{
							continue;
						}
						else if (mixLine.substr(0, 2).compare("//") == 0)
						{
							continue;
						}
						else if (mixLine.substr(0, 5).compare("\tMix:") == 0 || mixLine.substr(0, 6).compare("State:") == 0)
						{
							line = mixLine;
							noReadLine = true;
							break;
						}

						mixLines.push_back(mixLine);
					}

					needToCalculateLogDet = (mixLines.size() <= (1 + nStrmsInMix + nStrmsInMix));

					// Read weight
					States[stateIndex].Gaussians[mixIndex].Weight = atof(mixLines[0].c_str());

					// Read means
					for (int i = 0; i < nStrmsInMix; i++)
					{
						std::string bufferMean;
						std::stringstream ssMean(mixLines[1 + i]);
						std::vector<std::string> elementsMean;

						// Split current line by whitespace
						while (ssMean >> bufferMean)
							elementsMean.push_back(bufferMean);

						for (int j = 0; j < StreamSize; j++)
						{
							States[stateIndex].Gaussians[mixIndex].Mean[(i * StreamSize) + j] = atof(elementsMean[j].c_str());
						}
					}

					// Read convariances
					for (int i = 0; i < nStrmsInMix; i++)
					{
						std::string bufferCov;
						std::stringstream ssCov(mixLines[1 + nStrmsInMix + i]);
						std::vector<std::string> elementsCov;

						// Split current line by whitespace
						while (ssCov >> bufferCov)
							elementsCov.push_back(bufferCov);

						for (int j = 0; j < StreamSize; j++)
						{
							States[stateIndex].Gaussians[mixIndex].Covariance[(i * StreamSize) + j] = atof(elementsCov[j].c_str());
						}
					}

					// Log determinants
					if (!needToCalculateLogDet)
					{
						std::string bufferDet;
						std::stringstream ssDet(mixLines[1 + nStrmsInMix + nStrmsInMix]);
						std::vector<std::string> elementsDet;

						// Split current line by whitespace
						while (ssDet >> bufferDet)
							elementsDet.push_back(bufferDet);

						for (int i = 0; i < nStrmsInMix; i++)
						{
							for (int j = 0; j < StreamSize; j++)
							{
								States[stateIndex].Gaussians[mixIndex].Determinant[i] += atof(elementsDet[j].c_str());
							}
						}
					}
					else
					{
						for (int i = 0; i < nStrmsInMix; i++)
						{
							for (int j = 0; j < StreamSize; j++)
							{
								States[stateIndex].Gaussians[mixIndex].Determinant[i] +=
									std::log(States[stateIndex].Gaussians[mixIndex].Covariance[(i * nStrmsInMix) + j]);
							}
						}
					}
				}
			}

			infile.close();
		}
		catch (std::exception e)
		{
			infile.close();
			throw;
		}
	}
}

void
TCode::CreateEmpty(int numStates, int numStreams, int streamSize)
{
	nStates = numStates;
	nStreams = numStreams;
	StreamSize = streamSize;
	VectorSize = nStreams * StreamSize;

	// Create States and AStates
	States = std::vector<TGaussianMixture>();
	States.resize(nStates);
	AStates = std::vector<TGaussianMixture>();
	AStates.resize(nStates);

    for (unsigned int i = 0; i < nStates; i++)
	{
		short int nGaus = Vocabulary->GetUnitNumMixturesByPhysicalState(i);

		States[i].nGaussians = nGaus;
		AStates[i].nGaussians = nGaus;

		States[i].Gaussians = std::vector<TGaussian>();
		States[i].Gaussians.resize(States[i].nGaussians);
		AStates[i].Gaussians = std::vector<TGaussian>();
		AStates[i].Gaussians.resize(AStates[i].nGaussians);

         for (int k = 0; k < nGaus; k++)
		 {
			 States[i].Gaussians[k].Mean = std::vector<float>();
			 States[i].Gaussians[k].Mean.resize(VectorSize);
			 AStates[i].Gaussians[k].Mean = std::vector<float>();
			 AStates[i].Gaussians[k].Mean.resize(VectorSize);

			 States[i].Gaussians[k].Covariance = std::vector<float>();
			 States[i].Gaussians[k].Covariance.resize(VectorSize);
			 AStates[i].Gaussians[k].Covariance = std::vector<float>();
			 AStates[i].Gaussians[k].Covariance.resize(VectorSize);

			 States[i].Gaussians[k].Determinant = std::vector<float>();
			 States[i].Gaussians[k].Determinant.resize(nStreams);
			 AStates[i].Gaussians[k].Determinant = std::vector<float>();
			 AStates[i].Gaussians[k].Determinant.resize(nStreams);
         }
    }

	Zero(States);
	Zero(AStates);
}

void
TCode::Zero(std::vector<TGaussianMixture>& states)
{
	// Set all values to zero
	for (unsigned int i = 0; i < states.size(); i++)
	{
		for (int k = 0; k < states[i].nGaussians; k++)
		{
			states[i].Gaussians[k].Weight = 0.0;

			for (int n = 0; n < StreamSize; n++)
			{
				states[i].Gaussians[k].Mean[n] = states[i].Gaussians[k].Covariance[n] = 0.0;
			}
			for (int d = 0; d < nStreams; d++)
			{
				states[i].Gaussians[k].Determinant[d] = 0.0;
			}
         }
    }
}

int
TCode::GetMaxNumGaussians()
{
	int max = -1;
	for (int i = 0; i < nStates; i++)
	{
		if (States[i].nGaussians > max)
		{
			max = States[i].nGaussians;
		}
	}
	return max;
}

void TCode::Save(const std::string& fileName)
{
	std::ofstream outfile(fileName);

	outfile << "nStates=" << nStates << std::endl;
	outfile << "nStrms=" << nStreams << std::endl;
	outfile << "StrmSize=" << StreamSize << std::endl;
	outfile << std::endl;

	outfile.precision(8);
	outfile.setf(std::ios::left);
	outfile.setf(std::ios::showpoint);
	outfile.setf(std::ios::showpos);
	outfile.setf(std::ios::dec);

	for (int i = 0; i < States.size(); i++)
	{
		outfile << "State: " << i << "\tnMix=" << States[i].nGaussians << std::endl;

		for (int j = 0; j < States[i].nGaussians; j++)
		{
			outfile << "\tMix: " << j << " nStrms=" << nStreams << std::endl;

			// Weight
			outfile << States[i].Gaussians[j].Weight << std::endl;

			// Mean
			for (int k = 0; k < nStreams; k++)
			{
				for (int m = 0; m < StreamSize; m++)
				{
					outfile << States[i].Gaussians[j].Mean[(k * StreamSize) + m] << "  ";
				}
				outfile << std::endl;
			}
			outfile << std::endl;

			// Covariance
			for (int k = 0; k < nStreams; k++)
			{
				for (int m = 0; m < StreamSize; m++)
				{
					outfile << States[i].Gaussians[j].Covariance[(k * StreamSize) + m] << "  ";
				}
				outfile << std::endl;
			}
			outfile << std::endl;

			// Log determinants
			for (int n = 0; n < (StreamSize * nStreams); n++)
			{
				outfile << std::log(States[i].Gaussians[j].Covariance[n]) << "  ";
			}
            outfile << std::endl;

		}

		outfile << std::endl;
	}

	outfile.close();
}
