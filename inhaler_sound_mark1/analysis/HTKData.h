// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G
#ifndef ANALYSIS_HTKDATA_H_INCLUDED
#define ANALYSIS_HTKDATA_H_INCLUDED
// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G

// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I

// No Includes

// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I

// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
namespace analysis {
// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n


//! \header     Header which contains the following struct.
//! \struct     HTKHeader struct used in generation of .mfc files.
//!
typedef struct
{
    int         nSamples;
    int         sampPeriod;         // value of 1 = 100ns
    short int   sampSize;
    short int   parmKind;
} HTKHeader;


class HTKData
{
public:

                HTKData             ();

                ~HTKData            ();
};


// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
} // analysis
// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n

// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G
#endif // ANALYSIS_HTKDATA_H_INCLUDED
