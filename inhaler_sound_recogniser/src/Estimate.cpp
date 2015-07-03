#include "Estimate.h"

#include "Global.h"
#include "Helper.h"

TViterbiDecoder* Segment = new TViterbiDecoder();

TEstimate::TEstimate()
{
    Alpha = Beta = 0;
    Time_State = 0;
}

// Destructor
TEstimate::~TEstimate()
{
    Remove();
	if (Segment) delete Segment;
}

// Accumulate using uniform segmentation
void
TEstimate::AccByUniSeg(ASegment& Signal)
{
    // Uniform segment the signal
    Segment->Uniform(&Signal);

    // Process each frame
    for (int t = 0; t < Signal.nFrames; t++)
    {
        // Frame
        std::vector<float>* v = &(Signal.Frames[t].Vector);
        // State
        unsigned int i = Segment->State[t];
        // Mixture
		for (int k = 0; k < Model->States[i]->nGaussians; k++)
        {
			TGaussian* a = &(Model->AStates[i]->Gaussians[k]);
            for (int n = 0; n < Code->VectorSize; n++)
            {
                a->Mean[n] += (*v)[n];
				a->Covariance[n] += (*v)[n] * (*v)[n];
            }
			a->Weight += 1;
        }
    }
}

// Finish UniSeg accumulation
void
TEstimate::UniSegModel()
{
    // Finish states
    for (unsigned int i = 0; i < Code->nStates; i++)
    {
		std::vector<TGaussian>* a = &(Code->AStates[i].Gaussians);
		std::vector<TGaussian>* m = &(Code->States[i].Gaussians);

        // Overall weight in this state
        float SumWei = 0.0;
		for (int k = 0; k < Code->States[i].nGaussians; k++)
        {
            SumWei += (*a)[k].Weight;
        }
        if (SumWei == 0.0)
        {
            continue;
        }

		for (int k = 0; k < Code->States[i].nGaussians; k++)
        {
            // Wei
			(*m)[k].Weight = (*a)[k].Weight / SumWei;
			if ((*m)[k].Weight == 0.0)
            {
                continue;
            }
            // Mean
            for (int n = 0; n < Code->VectorSize; n++)
            {
				(*m)[k].Mean[n] = (*a)[k].Mean[n] / (*a)[k].Weight;
            }
            // Covar
            for (int n = 0; n < Code->VectorSize; n++)
            {
				float z = (*a)[k].Covariance[n] / (*a)[k].Weight - (*m)[k].Mean[n] * (*m)[k].Mean[n];
				(*m)[k].Covariance[n] = (z > GlobalSettings.TrainingOptions.MinimumVariance) ? z : GlobalSettings.TrainingOptions.MinimumVariance;
            }
            // Log det
            for (int d = 0; d < Model->nStreams; d++)
            {
				(*m)[k].Determinant[d] = 0.0;
                for (int n = d * Model->StreamSize; n < (d + 1)*Model->StreamSize; n++)
                {
					(*m)[k].Determinant[d] += log((*m)[k].Covariance[n]);
                }
            }
        }
    }
}

// Accumulate using forward-backward algorithm
void
TEstimate::AccByTrueFB(ASegment& Signal)
{
    // Space
    Space_TrueFB(Signal);

    // FB probabilities Alpha, Beta
    double prob = AlphaBeta(Signal);
    if (prob <= 0.0 || prob > BIGPOS)
    {
        return;
    }

    // Process each frame
    for (int t = 0; t < Signal.nFrames; t++)
    {
        // State
        for (unsigned int i = 0; i < Model->nStates; i++)
        {
            // State-dependent part of ksai
            double ksai = 0.0;
            if (t == 0)
            {
                ksai = (i == 0) ? 1.0 : 0.0;
            }
            else
            {
				unsigned int j0 = Model->TransitionsIn[i];
                for (unsigned int j = j0; j <= i; j++)
                {
                    ksai += Alpha[t - 1][j];
                }
            }
            ksai *= Beta[t][i];
            if (ksai <= 0.0L)
            {
                continue;
            }

            // De-scale
            ksai /= prob;

            // Mixture
			for (int k = 0; k < Model->States[i]->nGaussians; k++)
            {
				if (t >= Segment->Prob_Time_State_Mix.size()) 
				{
					Logger(LOG_ERROR) << "Prob_Time_State_Mix.size(): " << Segment->Prob_Time_State_Mix.size() << ", t: " << t << std::endl;
				}
				if (i >= Segment->Prob_Time_State_Mix[t].size()) 
				{
					Logger(LOG_ERROR) << "Prob_Time_State_Mix[" << t << "].size(): " << Segment->Prob_Time_State_Mix[t].size() << 
						", s: " << i << std::endl;
				}
				if (k >= Segment->Prob_Time_State_Mix[t][i].size())
				{
					Logger(LOG_ERROR) << "Prob_Time_State_Mix[" << t << "][" << i << "].size(): " << 
					Segment->Prob_Time_State_Mix[t][i].size() << ", k: " << k << std::endl;
				}

				// Combine ksai with mixture probability
                AccAGauss(Signal, t,i,k,ksai*Segment->Pr0R(t,i,k));
            }
        }
    }
}

// Finish forward-backward model
void
TEstimate::FBModel()
{
    // Finish states
    for (unsigned int i = 0; i < Code->nStates; i++)
    {
		std::vector<TGaussian>* a = &(Code->AStates[i].Gaussians);
		std::vector<TGaussian>* m = &(Code->States[i].Gaussians);

        // Overall weight in this state
        float SumWei = 0.0;
		for (int k = 0; k < Code->States[i].nGaussians; k++)
        {
			SumWei += (*a)[k].Weight;
        }

        // Not defunction the whole state
        if (SumWei == 0.)
        {
			for (int k = 0; k < Code->States[i].nGaussians; k++)
			{
				if ((*m)[k].Weight != 0.0)
                {
                    SumWei += 1;
                }
			}
			for (int k = 0; k < Code->States[i].nGaussians; k++)
			{
				if ((*m)[k].Weight != 0.0)
                {
					(*m)[k].Weight = 1 / SumWei;
                }
			}
            // Keep the old parameters for state i
            continue;
        }

		for (int k = 0; k < Code->States[i].nGaussians; k++)
        {
            // wei
			(*m)[k].Weight = (*a)[k].Weight / SumWei;
			if ((*m)[k].Weight == 0.0)
            {
                continue;
            }
            // Mean
            for (int n = 0; n < Code->VectorSize; n++)
            {
				(*m)[k].Mean[n] = (*a)[k].Mean[n] / (*a)[k].Weight;
            }
            // Covar
            for (int n = 0; n < Code->VectorSize; n++)
            {
				float z = (*a)[k].Covariance[n] / (*a)[k].Weight;
				(*m)[k].Covariance[n] = (z > GlobalSettings.TrainingOptions.MinimumVariance) ? z : GlobalSettings.TrainingOptions.MinimumVariance;
            }
            // Log det
            for (int d = 0; d < Model->nStreams; d++)
            {
				(*m)[k].Determinant[d] = 0.0;
                for (int n = d * Model->StreamSize; n < (d + 1) * Model->StreamSize; n++)
                {
					(*m)[k].Determinant[d] += log((*m)[k].Covariance[n]);
                }
            }
        }
    }
}

// Increase the number of mixtures by 1, successful return true
bool
TEstimate::Split()
{
    // Standard deviation
    float* Dev = new float[Code->VectorSize];

    bool okay = false;
    for (unsigned int i = 0; i < Code->nStates; i++)
    {
        // Has reached the required nMix
		if (Code->States[i].nGaussians == Code->AStates[i].nGaussians)
        {
            continue;
        }

        // State to be split
		std::vector<TGaussian>* m = &(Code->States[i].Gaussians);

        // Find the maximum weight component
        int MaxCom;
        float MaxWei = 0.0;
		for (int k = 0; k < Code->States[i].nGaussians; k++)
		{
			if ((*m)[k].Weight > MaxWei)
            {
				MaxWei = (*m)[k].Weight;
                MaxCom = k;
            }
		}

		// Split the maximum component into 2
		Code->States[i].Gaussians.resize(Code->States[i].nGaussians + 1);
		for (int k = Code->States[i].nGaussians; k < Code->States[i].Gaussians.size(); k++)
		{
			// Initialise new Gaussians
			Code->States[i].Gaussians[k] = TGaussian();
			Code->States[i].Gaussians[k].Mean = std::vector<float>();
			Code->States[i].Gaussians[k].Mean.resize(Code->VectorSize);
			Code->States[i].Gaussians[k].Covariance = std::vector<float>();
			Code->States[i].Gaussians[k].Covariance.resize(Code->VectorSize);
			Code->States[i].Gaussians[k].Determinant = std::vector<float>();
			Code->States[i].Gaussians[k].Determinant.resize(Code->nStreams);
		}

        // Use standard deviation as perturbing value
        for (int n = 0; n < Code->VectorSize; n++)
        {
			Dev[n] = sqrt((*m)[MaxCom].Covariance[n]) * PERTURB;
        }

        // Split MaxCom into 2 components
        // Wei
		(*m)[MaxCom].Weight /= 2;
		(*m)[Code->States[i].nGaussians].Weight = (*m)[MaxCom].Weight;
        // Mean
        for (int n = 0; n < Code->VectorSize; n++)
        {
			(*m)[Code->States[i].nGaussians].Mean[n] = (*m)[MaxCom].Mean[n] + Dev[n];
            (*m)[MaxCom].Mean[n] -= Dev[n];
        }
        // Covar
        for (int n = 0; n < Code->VectorSize; n++)
        {
			(*m)[Code->States[i].nGaussians].Covariance[n] = (*m)[MaxCom].Covariance[n];
        }
        // Det
        for (int d = 0; d < Model->nStreams; d++)
        {
			(*m)[Code->States[i].nGaussians].Determinant[d] = (*m)[MaxCom].Determinant[d];
        }

        // Mixture increment
		Code->States[i].nGaussians++;
        okay = true;
    }
    delete [] Dev;
    return okay;
}

// Double number of mixtures, successful return true, over Code->States
bool
TEstimate::Split2()
{
    // Standard deviation
    float* Dev = new float[Code->VectorSize];

    bool okay = false;
    for (unsigned int i = 0; i < Code->nStates; i++)
    {
        // Has reached the required nMix
		if (Code->States[i].nGaussians == Code->AStates[i].nGaussians)
        {
            continue;
        }

        // State to be splited
		std::vector<TGaussian>* m = &(Code->States[i].Gaussians);

		// Split every mixture component into two
		Code->States[i].Gaussians.resize(Code->States[i].nGaussians * 2);
		for (int k = Code->States[i].nGaussians; k < Code->States[i].Gaussians.size(); k++)
		{
			// Initialise new Gaussians
			Code->States[i].Gaussians[k] = TGaussian();
			Code->States[i].Gaussians[k].Mean = std::vector<float>();
			Code->States[i].Gaussians[k].Mean.resize(Code->VectorSize);
			Code->States[i].Gaussians[k].Covariance = std::vector<float>();
			Code->States[i].Gaussians[k].Covariance.resize(Code->VectorSize);
			Code->States[i].Gaussians[k].Determinant = std::vector<float>();
			Code->States[i].Gaussians[k].Determinant.resize(Code->nStreams);
		}

        // Split every mixture component into two
        for (int k = 0; k < Code->States[i].nGaussians; k++)
        {
            // Use standard deviation as perturbing value
            for (int n = 0; n < Code->VectorSize; n++)
            {
				Dev[n] = sqrt((*m)[k].Covariance[n]) * PERTURB;
            }

            // Split
            // Wei
			(*m)[k].Weight /= 2;
			(*m)[Code->States[i].nGaussians + k].Weight = (*m)[k].Weight;
            // Mean
            for (int n = 0; n < Code->VectorSize; n++)
            {
				(*m)[Code->States[i].nGaussians + k].Mean[n] = (*m)[k].Mean[n] + Dev[n];
				(*m)[k].Mean[n] -= Dev[n];
            }
            // Covar
            for (int n = 0; n < Code->VectorSize; n++)
            {
				(*m)[Code->States[i].nGaussians + k].Covariance[n] = (*m)[k].Covariance[n];
            }
            // Det
            for (int d = 0; d < Model->nStreams; d++)
            {
				(*m)[Code->States[i].nGaussians + k].Determinant[d] = (*m)[k].Determinant[d];
            }
        }

        // Mixture increment
        Code->States[i].nGaussians *= 2;
        okay = true;
    }
    delete [] Dev;
    return okay;
}

// Weighted accumulation of vector t to a Gauss in state s and mixture k
void
TEstimate::AccAGauss(ASegment& Signal, int t,
                     int s,
                     int k,
                     double w)
{
    if (w <= 0.0)
    {
        return;
    }

	if (t > Signal.Frames.size())
	{
		Logger(LOG_ERROR) << "Signal.Frames.size(): " << Signal.Frames.size() << ", t: " << t << std::endl;
	}
	if (s > Model->AStates.size())
	{
		Logger(LOG_ERROR) << "Model->AStates.size(): " << Model->AStates.size() << ", s: " << s << std::endl;
	}
	if (s > Model->States.size())
	{
		Logger(LOG_ERROR) << "Model->States.size(): " << Model->States.size() << ", s: " << s << std::endl;
	}
	if (k > Model->AStates[s]->Gaussians.size())
	{
		Logger(LOG_ERROR) << "Model->AStates[s]->Gaussians.size(): " << Model->AStates[s]->Gaussians.size() << 
			", s: " << s << ", k: " << k << std::endl;
	}
	if (k > Model->States[s]->Gaussians.size())
	{
		Logger(LOG_ERROR) << "Model->States[s]->Gaussians.size(): " << Model->States[s]->Gaussians.size() << 
			", s: " << s << ", k: " << k << std::endl;
	}

	std::vector<float>* v = &(Signal.Frames[t].Vector);
	TGaussian* a = &(Model->AStates[s]->Gaussians[k]);
	TGaussian* m = &(Model->States[s]->Gaussians[k]);

	if (a->Mean.size() != Code->VectorSize || a->Covariance.size() != Code->VectorSize)
	{
		// When using double mixtures on an odd number of states, AStates has fewer mixes than States
		Logger(LOG_ERROR) << "a->Mean.size(): " << a->Mean.size() << std::endl;
		Logger(LOG_ERROR) << "a->Covariance.size(): " << a->Covariance.size() << std::endl;
		Logger(LOG_ERROR) << "t: " << t << ", s: " << s << ", k: " << k << ", w: " << w << std::endl;
	}

    for (int n = 0; n < Code->VectorSize; n++)
    {
        a->Mean[n] += w * (*v)[n];
        float z = (*v)[n] - m->Mean[n];
		a->Covariance[n] += w * z * z;
    }
	a->Weight += w;
}

// Alpha Beta for FB algorithm
double
TEstimate::AlphaBeta(ASegment& Signal)
{
    // Scaling factors
    std::vector<double> Scale = std::vector<double>();
	for (int i = 0; i < Signal.nFrames; i++)
	{
		Scale.push_back(0.0);
	}

    // Alpha
    for (int t = 0; t < Signal.nFrames; t++)
    {
        for (unsigned int j = 0; j < Model->nStates; j++)
        {
            double a = 0.0L;
            if (t == 0)
            {
                a = (j == 0) ? 1.0 : 0.0;
            }
            else
            {
				unsigned int i0 = Model->TransitionsIn[j];
                for (unsigned int i = i0; i <= j; i++)
                {
                    a += Alpha[t - 1][i];
                }
            }

            if (a != 0.0L)
            {
                Time_State[t][j] = 1;
                Alpha[t][j] = a * Segment->Pr0(&Signal, t, j);
            }
            else
            {
                Alpha[t][j] = 0.0L;
            }
        }

        // Scaling
        double c = 0.0L;
        for (unsigned int i = 0; i < Model->nStates; i++)
        {
            c += Alpha[t][i];
        }

        // Newly added (May, 2000)
        if (c == 0.)
        {
            return 0.0;
        }

        for (unsigned int i = 0; i < Model->nStates; i++)
        {
            Alpha[t][i] /= c;
        }
        Scale[t] = c;
    }

    // Beta
    char* Ovf1 = new char[30];
    char* Ovf2 = new char[30];
    for (int t = Signal.nFrames-1; t >= 0; t--)
    {
        int t1 = t + 1;
        for (unsigned int i = 0; i < Model->nStates; i++)
        {
            double b = 0.0L;
            if (t1 == Signal.nFrames)
            {
                b = (i == Model->nStates - 1) ? 1.0 : 0.0;
            }
            else
            {
				unsigned int j0 = Model->TransitionsOut[i];
                for (unsigned int j = i; j <= j0; j++)
                {
                    if (Time_State[t1][j])
                    {
                        b += Beta[t1][j] * Segment->Pr0R(t1, j);
                    }
                }
            }

            // Ignore this signal if b overflows
            sprintf(Ovf1, "%e", b);
            if (atoi(strrchr(Ovf1, 'e') + 1) > 300)
            {
                delete [] Ovf1;
                delete [] Ovf2;
                return MAXPOS;
            }

            if (b != 0.0L)
            {
                // Ignore this signal if Beta overflows
                sprintf(Ovf1, "%e", b);
                sprintf(Ovf2, "%e", Scale[t]);
                if (atoi(strrchr(Ovf1, 'e') + 1) - atoi(strrchr(Ovf2, 'e') + 1) > 300)
                {
                    delete [] Ovf1;
                    delete [] Ovf2;
                    return MAXPOS;
                }
                Beta[t][i] = b / Scale[t];
            }
            else
            {
                Beta[t][i] = 0.0L;
            }
        }
    }
    delete [] Ovf1;
    delete [] Ovf2;

    // Log probability
    Pr = 0.0;
    for (int t = 0; t < Signal.nFrames; t++)
    {
        Pr += log(Scale[t]);
    }

    // Scaled probability
    double prob = 0.0L;
    for (unsigned int i = 0; i < Model->nStates; i++)
    {
        prob += Alpha[0][i] * Beta[0][i];
    }
    prob *= Scale[0];

    return prob;
}

// Space for TrueFB
void
TEstimate::Space_TrueFB(ASegment& Signal)
{
    Remove();
    Alpha = new double*[Signal.nFrames];
    Beta = new double*[Signal.nFrames];
    for (int t = 0; t < Signal.nFrames; t++)
    {
        Alpha[t] = new double[Model->nStates];
        Beta[t] = new double[Model->nStates];

		for (int s = 0; s < Model->nStates; s++)
		{
			Alpha[t][s] = 0.0;
			Beta[t][s] = 0.0;
		}
    }

    Time_State = new bool*[Signal.nFrames];
    for (int t = 0; t < Signal.nFrames; t++)
    {
        Time_State[t] = new bool[Model->nStates];
        for (unsigned int i = 0; i < Model->nStates; i++)
        {
            Time_State[t][i] = 0;
        }
    }
    // Reset
    nFrames = Signal.nFrames;
}

// Remove
void
TEstimate::Remove()
{
    if (Alpha)
    {
        for (int t = 0; t < nFrames; t++)
        {
            delete [] Alpha[t];
        }
        delete [] Alpha;
        Alpha = 0;
    }
    if (Beta)
    {
        for (int t = 0; t < nFrames; t++)
        {
            delete [] Beta[t];
        }
        delete [] Beta;
        Beta = 0;
    }
    if (Time_State)
    {
        for (int t = 0; t < nFrames; t++)
        {
            delete [] Time_State[t];
        }
        delete [] Time_State;
        Time_State = 0;
    }
}
