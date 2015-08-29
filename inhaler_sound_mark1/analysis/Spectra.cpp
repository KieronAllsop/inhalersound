// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I

// Self Include
#include "Spectra.h"

// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I


TSpectra::
TSpectra()
{
    Vector = std::vector<float>();
    DelVec = std::vector<float>();
    DelDelVec = std::vector<float>();
    Header = new HTKHeader;
}


TSpectra::
~TSpectra()
{
    delete Header;
}


void TSpectra::
Create( int nBnd, int bSize, int nVecs, int nStr, int samplingRate )
{
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
    if( 0 )                                            //GlobalSettings.SpeechAnalysisParamaters.DelOnly = 0
    {
        nStr -= 1;
    }

    // Header for overall sub-band spectra using MainForm
    Header->nSamples = nVectors;
    Header->sampPeriod = 80 * 1.0e+7 / samplingRate;   //GlobalSettings.SpeechAnalysisParamaters.Shift = 80
    Header->sampSize = ( short int )( nStr * nBands * bndSize * sizeof( float ) );
    Header->parmKind = 9;               // User kind
}


void TSpectra::
Output( std::string fileName, TSpectra* a1, TSpectra* a2 )
{
    std::ofstream outfile( fileName, std::ios::binary );

    if( !outfile )
    {
        throw std::runtime_error( "TSpectra: Specified file: \"" + fileName + "\" cannot be written to" );
    }

    // Write HTK header
    if( a1 )
    {
        Header->sampSize += a1->Header->sampSize;
    }

    if( a2 )
    {
        Header->sampSize += a2->Header->sampSize;
    }

    outfile.write( ( char* )Header, sizeof( HTKHeader ) );

    // Write multi-stream multi-band spectral sequence
    for( int n = 0; n < nVectors; ++n )
    {
        for( int b = 0; b < nBands; ++b )
        {
            Output( outfile, b, n, this );
            if( a1 )
            {
                Output( outfile, b, n, a1 );
            }
            if( a2 )
            {
                Output( outfile, b, n, a2 );
            }
         }
    }
    outfile.close();
}


void TSpectra::
Output( std::ofstream& outfile, int b, int n, TSpectra* a )
{
    // Vector/band offset
    int v0 = ( n * a->nBands + b ) * a->bndSize;

    // Static spectra
    if( 0 == 0 )                                        //GlobalSettings.SpeechAnalysisParamaters.DelOnly = 0
    {
        float* v = &a->Vector.at( 0 ) + v0;
        outfile.write( ( char* )v, a->bndSize * sizeof( float ) );
    }

    // Delta spectra
    if( a->DelVec.size() > 0 )
    {
        float* v = &a->DelVec.at( 0 ) + v0;
        outfile.write( ( char* )v, a->bndSize * sizeof( float ) );
    }

    // Delta-delta spectra
    if( a->DelDelVec.size() > 0 )
    {
        float* v = &a->DelDelVec.at( 0 ) + v0;
        outfile.write( ( char* )v, a->bndSize * sizeof( float ) );
    }
}
