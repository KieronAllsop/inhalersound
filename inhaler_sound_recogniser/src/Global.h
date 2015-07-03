#pragma once

#include <cstdlib>
#include <cmath>
#include <limits>
#include <string>
#include <vector>

const long double PI = (std::atan(1.0) * 4.0);

const float BIGNEG = (float)-1e+20;
const float BIGPOS = std::numeric_limits<float>::infinity();
const double MINPOS = 1.7e-308;
const double MAXPOS = 1.7e+308;

const int EXPDBPNEGLIMIT = -750;

const float PERTURB	= 0.1;

const double Log2Pi = log(2.0 * PI);

const std::string FILLER_LABEL = "Filler";
const std::string COUGH_LABEL = "Cough_Event";

enum Mode
{
	MODE_TEST,
	MODE_TRAIN,
	MODE_SPECTRA, 
	MODE_SCORE
};

enum AudioSource
{
	AUDSRC_HTK,
	AUDSRC_WAV, 
	AUDSRC_NONE
};

enum LogLevel
{
	LOG_TRACE = 5,
	LOG_DEBUG = 4,
	LOG_INFO = 3,
	LOG_WARN = 2,
	LOG_ERROR = 1
};

enum LabelFormat
{
	LBLFMT_AUDACITY,
	LBLFMT_HTK
};

typedef struct
{
	bool Continuous;
	int TieState;
	std::vector<int> ModelIterations;
	int TrStreams;
	float MinimumVariance;
	int MulPenalty;
	int AddPenalty;
	std::vector<int> NBest;
	int DurWin;

	std::string OutputDirectory;

	std::string MasterLabelFile;

	bool DoubleMixtures;
} TrainingParameters;

typedef struct
{
	// Analysis parameters
	// FFT
    int FrmLength;
    int Shift;
	bool PreEmp;
	bool Window;
	bool Power;

	// Decorrelation filter mode
	int DCFilt;

	// Filter Bank
	int nChn;				// Number of channels over entire band
	int LowCut;
	int HighCut;
	bool Mel;
	bool LogAmp;

	// Delta
	int Delta;
	int DeltaDelta;
	bool DelOnly;

	// Sub-Band
	int nBands;         	// Number of sub bands
	bool C0;

	// Cepstrum
	int nDCTBand;			// Number of DCT coefficients per sub-band
	int Lift;
	bool RemoveCepstralMean;

	// I/O
	std::string OutputFileExtension;
} SpeechAnalysisParamaters_t;

typedef struct
{
	std::string Reference;
	std::string Test;
	unsigned long StartOffsetMilliseconds; // Used if files were split up for testing
	unsigned long EndOffsetMilliseconds;
} ScoreLabelPair;

typedef struct 
{
	// Mode
	Mode ProgramMode;

	// Code
	std::string CodeFileName;

	// Vocabulary
	std::string VocabularyVocabularyFileName;
	std::string VocabularyStateFileName;
	std::string VocabularyMixtureFileName;

	// Bigram
	std::string BigramFileName;
	float BigramCrossUnitPenalty;
	float BigramFixedPenalty;

	// Speech input parameters
	std::string SpeechAnalysisInputParameterFile;
	SpeechAnalysisParamaters_t SpeechAnalysisParamaters;
	int SpeechAnalysisNumThreads;

	// Decoding
	std::vector<AudioSource> DecodingSourceTypes;
	std::vector<std::string> DecodingFileNames;
	int DecodingNumFiles;
	int DecodingBufferSize;
	int DecodingElementSize;
	double DecodingPruningThreshold;
	int DecodingSamplingRate;
	int DecodingNumThreads;

	// Transcription
	LabelFormat TranscriptionLabelFormat;

	// Training
	std::vector<std::string> TrainingAcousticFileNames;
	std::vector<std::string> TrainingLabelFileNames;
	int TrainingNumFiles;
	std::string TrainingParameterFile;
	TrainingParameters TrainingOptions;
	int TrainingMaxLengthMillisecondsPerFile;

	// Viterbi
	int ViterbiSkipState;

	// Confidence Scoring
	float ScoreAcceptanceThreshold;
	bool ScoreUseGeometricConfidenceScore;
	std::vector<ScoreLabelPair> ScoreLabels;
	std::string ScoreOutputDirectory;

	// Logging
	LogLevel LogLogLevel;

} Settings;

extern Settings GlobalSettings;
