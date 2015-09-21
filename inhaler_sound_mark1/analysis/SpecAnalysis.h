// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G
#ifndef ANALYSIS_SPECANALYSIS_H_INCLUDED
#define ANALYSIS_SPECANALYSIS_H_INCLUDED
// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G

// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I

// analysis Includes
#include "analysis/Spectra.h"
#include "analysis/speech_spectra_settings.hpp"

// qt::audio Includes
#include "qt/audio/wav_data.hpp"

// Standard Library Includes
#include <vector>

// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I

// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
namespace analysis {
// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n


typedef struct
{
    std::vector<float> Vector;
} AFrame;


//! \headerfile SpecAnalysis.h
//! \author     Adapted from code originally written by Prof Ji Ming and amended by
//!             Fiachra Murray
//!
class TSpecAnalysis
{

public:

    using wav_data_t        = qt::audio::wav_data;
    using shared_settings_t = std::shared_ptr<analysis::speech_spectra_settings>;

public:

                            TSpecAnalysis       ();

                            ~TSpecAnalysis      ();

    std::vector<AFrame>     Execute             (   wav_data_t& wav,
                                                    std::ostream& Ostream,
                                                    const shared_settings_t& Settings  );

    TSpectra*               Fea;
    int                     nVectors;
    short int               VecSize;

private:
                                                // Output spectra
    TSpectra*               Cep;                // Cepstra
    TSpectra*               Dfb;                // Decorrelated FB spectra
    TSpectra*               Spe;                // FB Spectra

    // Overall control
    void                    SpecAnalysis        (   wav_data_t& wav   );

    // Initialise filter bank
    void                    InitFB              ();

    int                     Rate;

    // Analysis parameters
    int                     FrmLength;
    int                     FrmShift;
    int                     nFFT;               // Number of FFT points
    int                     nFFT2;              // Number of FFT points
    int                     nChn;				// Number of channels over entire band
    int                     nBands;         	// Number of sub bands
    int                     nChnBand;			// Number of channels per sub-band
    int                     nDCTBand;			// Number of DCT coefficients per sub-band
    int                     DCFilt;				// Decorrelation filter mode
    int                     nStrms;				// Number of streams in each sub-band

    // Pre-processing, FFT and DCT constants
    std::vector<float> 					HamWin;		// Hamming window
    float  								EmpCoe;     // Pre-emphasize
    std::vector<std::vector<float>> 	Cos;		// Cos function for FFT
    std::vector<std::vector<float>> 	Sin;		// Sin function for FFT
    std::vector<std::vector<float>> 	CosD;		// Cos function for DCT
    std::vector<float> 					CepWei;		// Cep lifter

    // Filter bank constants
    float                   FScale;             // Frequency scale: linear or mel
    std::vector<int>        Lin2Chn;            // Linear frequency to channel index
    float                   LCut, HCut;         // Cut-off frequencies (discrete linear)
    std::vector<float>      FCent;              // Filetr central frequencies
    std::vector<float>      FFunc;              // Filter transfer function
    bool                    InitFBDone;         // Initialization indicator

    // Pre-processing for frame x
    void                    PreProcess          (   float* x   );

    // Log energy of frame x
    float                   LogEnergy           (   float* x   );

    // FFT of frame x, return modulus in s
    void                    FFT                 (   float* x, float* s   );

    // Filter bank analysis for modulus s, return FB spectra in u
    void                    FBFilter            (   float* s, float* u   );

    // Log FB spectra u
    void                    LogFB               (   float* u   );

    // Linear to mel frequency
    float                   L2M                 (   int k   );

    // DCT for log FB spectra u, return in c
    void                    DCT                 (   float* u, float* c   );

    // Cepstrum lifter for c
    void                    CepLifter           (   float* c, int cSize   );

    // Decorrelate FB spectra u, return in c, H(z)=1 - z^-1
    void                    DecorFB_1           (   float* u, float* c   );

    // Decorrelate FB spectra u, return in c, H(z)=z^+1 - z^-1
    void                    DecorFB_2           (   float* u, float* c   );

    // Normalize log energy for a sequence e of nFrames
    void                    NormLogEne          (   float* e, int nFrames   );

    // Compute delta spectra for Vec of size VecSize, length nVecs, into Del
    void                    DelSpectra          (   int DelWin, float* Vec, float* Del,
                                                    int VecSize, int nVecs   );

    // Remove spectral mean for Vec of size VecSize, length nVecs
    void                    MeanRemove          (   float* Vec, int VecSize, int nVecs   );

    // Noise spectrum estimate
    float*                  NSpec;

    // Estimate noise spectrum of size vSize using the first nFrms of vec
    void                    EstNoiseSpec        (   float* vec, int vSize, int nFrms   );

    shared_settings_t       Settings_;

};

extern TSpecAnalysis* SpecAnalysis;


// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
} // analysis
// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n

// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G
#endif // ANALYSIS_SPECANALYSIS_H_INCLUDED
