#pragma once

#include "SIG.h"
#include "ViterbiDecoder.h"

class TEstimate
{
public:
	// Constrcutor
	TEstimate();
    	// Destructor
    ~TEstimate();

    	// Accumulate Model->AState using Signal->Frame, uniform segmentation
    void AccByUniSeg(ASegment& Signal);
    	// Finish UniSeg accumulation, over Code->State
    void UniSegModel();

    	// Accumulate Model->AState using Signal->Frame, forward-backward
    void AccByTrueFB(ASegment& Signal);
    	// Finish forward-backward model, over Code->State
    void FBModel();

    	// Increase mixtures by 1, successful return true, over Code->State
    bool Split();
    	// Double number of mixtures, successful return true, over Code->State
    bool Split2();

    	// Log probability, for Signal->Frame over Model->State
    float Pr;

private:
		// Alpha Beta for FB algorithm
	double AlphaBeta(ASegment& Signal);
		// Weighted accumulation of vector t to a Gauss in state s and mixture k
	void AccAGauss(ASegment& Signal, int t,
    		    	int s,
                  	int k,
                  	double w);

		// Spaces
	double** Alpha;
    double** Beta;
    bool** Time_State;
    int nFrames;
    void Space_TrueFB(ASegment& Signal);
    void Remove();
};

