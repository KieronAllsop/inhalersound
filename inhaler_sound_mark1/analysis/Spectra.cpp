// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I

// Self Include
#include "analysis/Spectra.h"

// Standard Library Includes
#include <ostream>

// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I

// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
namespace analysis {
// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n


//! \class  Spectra.cpp
//! \brief  Produces the output file from feature extraction
//!
TSpectra::
TSpectra()
{
    Vector = std::vector<float>();
    DelVec = std::vector<float>();
    DelDelVec = std::vector<float>();
    Header = new HTKHeader;
    DelOnly_ = 0;
}


TSpectra::
~TSpectra()
{
    delete Header;
}


void TSpectra::
Create( int nBnd, int bSize, int nVecs, int nStr, int samplingRate, int DelOnly, int FrameShift )
{
    DelOnly_ = DelOnly;

    nBands = nBnd;
    bndSize = bSize;
    nVectors = nVecs;

    // Static spectra
    Vector.resize( nVectors * nBands * bndSize );

    // Delta sepctra
    if( nStr >= 2 )
    {
        DelVec.resize(nVectors * nBands * bndSize);
    }

    // Delta-delta spectra
    if( nStr >= 3 )
    {
        DelDelVec.resize(nVectors * nBands * bndSize);
    }

    // Stream number reduction: delta only
    if( DelOnly )
    {
        nStr -= 1;
    }

    // Header for overall sub-band spectra using MainForm
    Header->nSamples = nVectors;
    Header->sampPeriod = FrameShift * 1.0e+7 / samplingRate;
    Header->sampSize = ( short int )( nStr * nBands * bndSize * sizeof( float ) );
    Header->parmKind = 9;               // User kind
}


void TSpectra::
Output( std::ostream& Ostream, TSpectra* a1, TSpectra* a2 )
{

    // Write HTK header
    if( a1 )
    {
        Header->sampSize += a1->Header->sampSize;
    }

    if( a2 )
    {
        Header->sampSize += a2->Header->sampSize;
    }

    Ostream.write( ( char* )Header, sizeof( HTKHeader ) );

    // Write multi-stream multi-band spectral sequence
    for( int n = 0; n < nVectors; ++n )
    {
        for( int b = 0; b < nBands; ++b )
        {
            Output( Ostream, b, n, this );
            if( a1 )
            {
                Output( Ostream, b, n, a1 );
            }
            if( a2 )
            {
                Output( Ostream, b, n, a2 );
            }
        }
    }
}


void TSpectra::
Output( std::ostream& Ostream, int b, int n, TSpectra* a )
{

    // Vector/band offset
    int v0 = ( n * a->nBands + b ) * a->bndSize;

    // Static spectra
    if( DelOnly_ == 0 )
    {
        float* v = &a->Vector.at( 0 ) + v0;
        Ostream.write( ( char* )v, a->bndSize * sizeof( float ) );
    }

    // Delta spectra
    if( a->DelVec.size() > 0 )
    {
        float* v = &a->DelVec.at( 0 ) + v0;
        Ostream.write( ( char* )v, a->bndSize * sizeof( float ) );
    }

    // Delta-delta spectra
    if( a->DelDelVec.size() > 0 )
    {
        float* v = &a->DelDelVec.at( 0 ) + v0;
        Ostream.write( ( char* )v, a->bndSize * sizeof( float ) );
    }
}


// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
} // analysis
// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
