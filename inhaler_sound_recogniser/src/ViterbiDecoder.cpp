#include <iostream>
#include <sstream>

#include "ViterbiDecoder.h"


TViterbiDecoder::TViterbiDecoder()
{
}


TViterbiDecoder::~TViterbiDecoder()
{
}

void TViterbiDecoder::DecodeFromBuffer(BufferedASignal* buffer)
{
	unsigned int processedFrames = 0;
	unsigned int processedChunks = 0;
	nFrames = 1;
	try
	{
		while (processedFrames < nFrames)
		{
			// Get a chunk of audio once it's been added to buffer
			ASegment* chunk = buffer->RequestChunk();

			nFrames = buffer->SumFrames;

			if (nFrames < 1)
			{
				throw std::runtime_error("Viterbi: Cannot perform recognition on empty feature vector chunk.");
			}
			
			std::stringstream ss;
			ss << "Consuming chunk " << ++processedChunks << " containing " << chunk->nFrames << 
				" feature vectors" << std::endl;
			Logger(LOG_DEBUG) << ss.str();

			// Viterbi Decoding

			// Initialisation
			if (processedFrames == 0)
			{
				ViterbiInitialisation(chunk);
			}

			// Recursion
			ViterbiRecursion(chunk, ((processedFrames == 0) ? 1 : processedFrames), (processedFrames + chunk->nFrames));

			// Free up one space
			buffer->FinishConsumingChunk();

			processedFrames += chunk->nFrames;
		}

		// Termination
		ViterbiTermination();
	}
	catch(boost::thread_interrupted& e)
	{
		Logger(LOG_ERROR) << "Viterbi Decoder: Decoding halted." << std::endl;
		return;
	}
	catch (std::exception& e)
	{
		Logger(LOG_ERROR) << "ERROR: " << e.what() << std::endl;
		ProbSequence.clear();
	}
}

void 
TViterbiDecoder::ViterbiInitialisation(ASegment* chunk)
{
	// Initialise empty arrays
	SetState(); 
	SetProbSequence();
    SetProb(); 
	SetPath(); 
	SetProbFrameAtStateTime();
    SetBestPre();

	// Initialisation - for each possible starting state, set its path cost to its emission probability for t = 0
	for (int w = 0; w < Bigram->nBeginUnits; w++)
	{
		unsigned int i = Vocabulary->Units[Bigram->BeginUnits[w]].StartLogicalPosition;
		float p = LogPr(chunk, 0, i);
		Prob[i] = p;
        Path[i][0] = i;
        ProbFrameAtStateTime[i][0] = p;
	}
}

void 
TViterbiDecoder::ViterbiRecursion(ASegment* chunk, unsigned int timeFrom, unsigned int timeTo)
{
	int skipPlus = GlobalSettings.ViterbiSkipState + 1;

	for (unsigned int t = timeFrom; t < timeTo; t++)
	{
		unsigned int signalFrameIndex = t % GlobalSettings.DecodingElementSize;

		// For each word in the vocabulary, find the best preceding word	
		for (int w = 0; w < Vocabulary->nUnits; w++)
		{
			BestPreUnitProb[w] = BIGNEG;
			BestPreUnitState[w] = -1;

			// Best preceeding unit of word w
			for (int v = 0; v < Bigram->To[w].nSource; v++)
			{
				// Exit state of unit v
				int i = Vocabulary->Units[Bigram->To[w].Source[v]].EndLogicalPosition - 1;

				if (Prob[i] > BIGNEG)
				{
					// Bigram transition probability
					float p = Prob[i] + Bigram->To[w].Probabilities[v];

					if (p > BestPreUnitProb[w])
					{
						BestPreUnitProb[w] = p;
						BestPreUnitState[w] = i;
					}
				}
			}
		}

		// Find the best state sequence within the current word

		int w = Vocabulary->nUnits - 1;
		for (int j = Model->nStates - 1; j >= 0; j--)
		{
			// The position of the state within the current word
			int j0 = j - Vocabulary->Units[w].StartLogicalPosition;

			// If it's the first state in the word
			if (j0 == 0)
			{
				// Self-loop unless better found
				Path[j][t] = j;

				// From a previous model
				int i = BestPreUnitState[w]; // Best preceding word
				if (i >= 0) // A path exists to i
				{
					float p = BestPreUnitProb[w];

					if (p > Prob[j]) // Better than current path (else self-loop)
					{
						Prob[j] = p;
						Path[j][t] = i;
					}
				}

				// To next word
				w--;
			}
			else
			{
				// Else search the preceding states within the word

				// Self-loop unless better found
				Path[j][t] = j;

				// For the preceding state in the word after the first one, or more if skipping states allowed
				int i0 = (j0 <= skipPlus) ? j0 : skipPlus;
				for (int i = 1; i <= i0; i++)
				{
					float p = Prob[j - i];

					if (p > Prob[j]) // Better than current path (else self-loop)
					{
						Prob[j] = p;
						Path[j][t] = j - i;
					}
				}
			} 
		} 

		// Add to the current path cost for each state that state's emission probability for t
		for (unsigned int j = 0; j < Model->nStates; j++)
		{
			if (Prob[j] > BIGNEG) 
			{
				float p = LogPr(chunk, signalFrameIndex, j);
				Prob[j] += p;
				ProbFrameAtStateTime[j][t] = p;
			}
		}
		
		// Pruning
		if (t < nFrames - 1) 
		{
			// Best path
			double maxProb = BIGNEG;
			for (unsigned int i = 0; i < Model->nStates; i++)
			{
				if (Prob[i] > maxProb)
				{
					maxProb = Prob[i];
				}
			}
			// Thresholding
			double T = maxProb - GlobalSettings.DecodingPruningThreshold;
			for (unsigned int i = 0; i < Model->nStates; i++)
			{
				if (Prob[i] < T)
				{
					Prob[i] = BIGNEG;
				}
			}
		}
		
	} 
}

void 
TViterbiDecoder::ViterbiTermination()
{
	// Termination - end in the last states of EUnits
	Pr = BIGNEG;
	int endState = -1;

	// Find the word whose last state has the highest probability path
	for (int w = 0; w < Bigram->nEndUnits; w++)
	{
		unsigned int i = Vocabulary->Units[Bigram->EndUnits[w]].EndLogicalPosition - 1;
		if (Prob[i] > Pr)
		{
			Pr = Prob[i];
			endState = i;
		}
	}

	if (endState < 0)
		throw std::runtime_error("Viterbi: An end state could not be reached for the given audio signal. This may be due " 
		"to constraints in the Bigram Model or to over-zealous pruning.");

	// Backtracing state sequence, 
	// Starting at the end, find the best 'from' state, then for that state, find the 
	// best 'from' state ... until the beginning
	State[nFrames - 1] = endState;
	for (int t = nFrames - 2; t >= 0; t--)
	{
		endState = Path[endState][t + 1];
		State[t] = endState;
	}

	// Backtracing probability sequence
	for (unsigned int t = 0; t < nFrames; t++)
	{
		ProbSequence[t] = ProbFrameAtStateTime[State[t]][t];
	}

	// Complete

	Logger(LOG_INFO) << "Viterbi Complete" << std::endl;

	Logger(LOG_TRACE) << "State sequence: " << std::endl;
	std::stringstream strm;
	for (unsigned int i = 0; i < State.size(); i++)
	{
		strm << State[i] << std::endl;
	}
	Logger(LOG_TRACE) << strm.str();
}

void TViterbiDecoder::Uniform(ASegment* chunk)
{
	// Uniform segmentation

	nFrames = chunk->nFrames;

	SetState();
    
	// Uniform segmentation
	int SegLen = chunk->nFrames / Model->nStates;
	
	if (SegLen == 0) SegLen = 1;

	unsigned int i = 0;
	for (int t = 0; t < chunk->nFrames; t++)
	{
		State[t] = i;
		if (((t + 1) % SegLen) != 0)
		{
			if (i < Model->nStates - 1)
			{
				i++;
			}
		}
	}
	Pr = 0.0;
}

void TViterbiDecoder::Align(ASegment* chunk)
{
	// Viterbi alignment (probability values recorded)

	Space(chunk);

	SetState();
	SetProb();
	SetPath();

	// Initialization: start from the first state
    Prob[0] = LogPr0(chunk, 0, 0);
    Path[0][0] = 0;

	// Recursion
   	for (int t = 1; t < nFrames; t++)
	{
		for (int j = Model->nStates - 1; j >= 0; j--)
		{
			// First state in the model
         	if (j == 0)
			{
              	// Self-loop
              	Path[j][t] = j;
			}
			else
			{
				// Other states in the model
				// Self-loop
              	Path[j][t] = j;

				// From a previous state
                int i0 = j - Model->TransitionsIn[j];
				for (int i = 1; i <= i0; i++)
				{
					float p = Prob[j - i];

                    if (p > Prob[j]) 
					{
                        Prob[j] = p;
                        Path[j][t] = j - i;
                    }
				}
			}
		}

		// Calculate likelihoods
        for(unsigned int j = 0; j <Model->nStates; j++) 
		{
         	if (Prob[j] > BIGNEG)
			{
              	Prob[j] += LogPr0(chunk, t,j);
			}
        }
	}

	// Termination: end in the last state
    Pr = Prob[Model->nStates - 1];
   	int endState = Model->nStates - 1;

	// Path backtracing
   	State[nFrames - 1] = endState;
    for(int t = nFrames - 2; t >= 0; t--) 
	{
    	endState = Path[endState][t + 1];
        State[t] = endState;
   	}
}

// Log probability of frame t in state s
float TViterbiDecoder::LogPr(ASegment* chunk, int t, unsigned int s)
{
	// Mixture
	std::vector<TGaussian>* mix = &Model->States[s]->Gaussians;
	int nMix = Model->States[s]->nGaussians;

	// Mixture gaussian
	double p = 0.0;
	int nStreams = Model->nStreams;
	if (nStreams == 1)
	{
		for (int k = 0; k < nMix; k++) 
		{
			p += (double)(*mix)[k].Weight * Gauss(chunk, t, s, k);
		}
	}
  
	p = (p > MINPOS) ? log(p) : EXPDBPNEGLIMIT;
	return p;
}

double
TViterbiDecoder::Gauss(ASegment* chunk, int t, unsigned int s, int k)
{
	// Function
	std::vector<float>* m = &Model->States[s]->Gaussians[k].Mean;
	std::vector<float>* c = &Model->States[s]->Gaussians[k].Covariance;
	std::vector<float>* h = &Model->States[s]->Gaussians[k].Determinant;

    // Frame & size
	std::vector<float>* v = &chunk->Frames[t].Vector;
	int n1 = (*v).size();

    // Gauss
    float e = n1 * Log2Pi;
	for (int n = 0; n < n1; n++) 
	{
		float z = (*v)[n] - (*m)[n];
		e += (z * z) / (*c)[n];
    }

    for (int d = 0; d < Model->nStreams; d++)
	{
		e += (*h)[d];
	}

    return exp(-e / 2.0);
}

// Gauss of frame t in state s, mixture k, stream d
double
TViterbiDecoder::Gauss(ASegment* chunk, int t, unsigned int s, int k, int d)
{
    // Function
	std::vector<float>* m = &Model->States[s]->Gaussians[k].Mean;
	std::vector<float>* c = &Model->States[s]->Gaussians[k].Covariance;
	std::vector<float>* h = &Model->States[s]->Gaussians[k].Determinant;

    // Frame & stream
	std::vector<float>* v = &chunk->Frames[t].Vector;
	int n0 = d * Model->StreamSize;
	int n1 = n0 + Model->StreamSize;

    // Gauss
    float e = (n1 - n0) * Log2Pi;
	for (int n = n0; n < n1; n++) 
	{
    	float z = (*v)[n] - (*m)[n];
        e += (z * z) / (*c)[n];
    }
    e += (*h)[d];

    return exp(-e / 2.0);
}

double TViterbiDecoder::Pr0(ASegment* chunk, int t, unsigned int s)
{
	// Reset record space for each new signal or model
	if(t == 0 && s == 0)
	{
    	Space(chunk);
	}

	// Mixture
	std::vector<TGaussian>* mix = &Model->States[s]->Gaussians;
	int nMix = Model->States[s]->nGaussians;

    // Mixture gaussian
	double p = 0.0;
	for (int k = 0; k < nMix; k++) 
	{
		double q = (double)(*mix)[k].Weight * Gauss(chunk, t, s, k);
        p += q;

        Prob_Time_State_Mix[t][s][k] = q;
    }

    Prob_Time_State[t][s] = p;

    return p;
}

double TViterbiDecoder::Pr0R(int t, unsigned int s)
{
	return Prob_Time_State[t][s];
}

double TViterbiDecoder::Pr0R(int t, unsigned int s, int k)
{
	if (t >= Prob_Time_State_Mix.size()) 
		Logger(LOG_ERROR) << "Prob_Time_State_Mix.size(): " << Prob_Time_State_Mix.size() << ", t: " << t << std::endl;
	if (s >= Prob_Time_State_Mix[t].size()) 
		Logger(LOG_ERROR) << "Prob_Time_State_Mix[" << t << "].size(): " << Prob_Time_State_Mix[t].size() << ", s: " << s << std::endl;
	if (k >= Prob_Time_State_Mix[t][s].size())
		Logger(LOG_ERROR) << "Prob_Time_State_Mix[" << t << "][" << s << "].size(): " << 
		Prob_Time_State_Mix[t][s].size() << ", k: " << k << std::endl;
	return Prob_Time_State_Mix[t][s][k];
}

float TViterbiDecoder::LogPr0(ASegment* chunk, int t, unsigned int s)
{
	double p = Pr0(chunk, t, s);
    p = (p > MINPOS) ? log(p) : EXPDBPNEGLIMIT;
    return (float)p;
}

void
TViterbiDecoder::Space(ASegment* chunk)
{
	// Create new rceords for a new signal and model
	nFrames = chunk->nFrames;

	Prob_Time_State = std::vector<std::vector<double>>();
	Prob_Time_State.resize(nFrames);
	Prob_Time_State_Mix = std::vector<std::vector<std::vector<double>>>();
	Prob_Time_State_Mix.resize(nFrames);

	for (int t = 0; t < nFrames; t++)
	{
		Prob_Time_State[t] = std::vector<double>();
		Prob_Time_State[t].resize(Model->nStates);

		Prob_Time_State_Mix[t] = std::vector<std::vector<double>>();
		Prob_Time_State_Mix[t].resize(Model->nStates);

		for (unsigned int i = 0; i < Model->nStates; i++)
		{
			Prob_Time_State_Mix[t][i] = std::vector<double>();
			Prob_Time_State_Mix[t][i].resize(Model->States[i]->nGaussians);
		}
	}
}

void 
TViterbiDecoder::SetState()
{
	State = std::vector<unsigned int>();
	State.resize(nFrames);
}

void 
TViterbiDecoder::SetProb()
{
	Prob = std::vector<float>();
	Prob.resize(Model->nStates);

	for (unsigned int i = 0; i < Prob.size(); i++)
	{
		Prob[i] = BIGNEG;
	}
}

void
TViterbiDecoder::SetPath()
{
	Path = std::vector<std::vector<unsigned int>>();
	Path.resize(Model->nStates);

	for (unsigned int i = 0; i < Path.size(); i++)
	{
		Path[i] = std::vector<unsigned int>();
		Path[i].resize(nFrames);
	}

	for (unsigned int i = 0; i < Path.size(); i++)
	{
		for (unsigned int j = 0; j < nFrames; j++)
		{
			Path[i][j] = 0;
		}
	}
}

void
TViterbiDecoder::SetProbSequence()
{
	ProbSequence = std::vector<float>();
	ProbSequence.resize(nFrames);
}

void 
TViterbiDecoder::SetProbFrameAtStateTime()
{
	ProbFrameAtStateTime = std::vector<std::vector<float>>();
	ProbFrameAtStateTime.resize(Model->nStates);

	for (unsigned int i = 0; i < ProbFrameAtStateTime.size(); i++)
	{
		ProbFrameAtStateTime[i] = std::vector<float>();
		ProbFrameAtStateTime[i].resize(nFrames);
	}
}

void 
TViterbiDecoder::SetBestPre()
{
	BestPreUnitProb = std::vector<float>();
	BestPreUnitProb.resize(Vocabulary->nUnits);

	BestPreUnitState = std::vector<int>();
	BestPreUnitState.resize(Vocabulary->nUnits);
}
