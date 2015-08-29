// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G
#ifndef ANALYSIS_HTKDATA_H_INCLUDED
#define ANALYSIS_HTKDATA_H_INCLUDED
// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G

// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I
// None
// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I

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

// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G
#endif // ANALYSIS_HTKDATA_H_INCLUDED
