#pragma once

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "HTKData.h"

class TSpectra
{
public:
	TSpectra();
	~TSpectra();

	// Create spectra of nVecs, each vector has nBnd, each band has bSize, of nStr
         // 	nStr=1: Static spectra
         // 	nStr=2: Static + Delta spectra
         //	nStr=3: Static + Delta + Delta_Delta spectra
	void Create(int nBnd, int bSize, int nVecs, int nStr, int samplingRate);

    	// Output multi-stream multi-band spectra (with two optional streams a1 and a2)
    void Output(std::string fileName, TSpectra* a1 = 0, TSpectra* a2 = 0);

    	// Sub-band spectral streams
    std::vector<float> Vector;		// Static spectra
    std::vector<float> DelVec;		// Delta spectra
    std::vector<float> DelDelVec;	// Delta-delta specta

    int nBands;		// Number of bands
    int bndSize; 		// Size of each band vector
    int nVectors;		// Number of spectral vectors in each band

    	// Spectra header
	HTKHeader* Header;

private:
	// Save band b of vector n of stream a
	void Output(std::ofstream& of, int b, int n, TSpectra* a);
};

