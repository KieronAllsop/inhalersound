// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I

// Self Include
#include "analysis/SpecAnalysis.h"

// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I

// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
namespace analysis {
// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n


//! \class  SpecAnalysis.cpp
//! \author Adapted from code originally written by Prof Ji Ming and amended by
//!         Fiachra Murray
//!
//! \brief  Class which performs feature extraction
//!
TSpecAnalysis::
TSpecAnalysis()
{

    const long double PI = (std::atan(1.0) * 4.0);

    auto Settings = analysis::speech_spectra_settings::create_from_file();
    Settings_ = Settings;

    // Analysis parameters
    FrmLength = Settings_->frame_length();
    FrmShift = Settings_->frame_shift();
    nFFT = 2;

    while( FrmLength > nFFT )
    {
        nFFT *= 2;
    }

    nFFT2 = nFFT / 2;
    nChn = Settings_->channels();
    nBands = Settings_->bands();
    nChnBand = nChn / nBands;
    nDCTBand = Settings_->dct();
    DCFilt = Settings_->dc_filter();

    // Output sub - band spectral streams
    nStrms = 1;
    if( Settings_->delta() )
    {
        nStrms = 2;
    }
    if( Settings_->delta_delta() )
    {
        nStrms = 3;
    }

    // Pre - processing constants
    HamWin = std::vector<float>();
    HamWin.resize( FrmLength );
    for( int n = 0; n < FrmLength; ++n )
    {
        HamWin[n] = 0.54 - 0.46 * cos( 2 * n * PI / ( FrmLength - 1 ) );
    }
    EmpCoe = 0.97;

    // FFT constants
    Cos = std::vector<std::vector<float>>();
    Cos.resize( nFFT2 );
    Sin = std::vector<std::vector<float>>();
    Sin.resize( nFFT2 );
    for( int k = 0; k < nFFT2; ++k )
    {
        Cos[k] = std::vector<float>();
        Cos[k].resize( nFFT );
        Sin[k] = std::vector<float>();
        Sin[k].resize( nFFT );
        for( int n = 0; n < nFFT; ++n )
        {
            Cos[k][n] = cos( 2 * PI * k * n / nFFT );
            Sin[k][n] = sin( 2 * PI * k * n / nFFT );
        }
    }

    // DCT cpostants
    if( nDCTBand > 0 )
    {
        CosD = std::vector<std::vector<float>>();
        CosD.resize( nDCTBand );
        for( int k = 0; k < nDCTBand; ++k )
        {
            CosD[k] = std::vector<float>();
            CosD[k].resize( nChnBand );
            for( int n = 0; n < nChnBand; ++n )
            {
                CosD[k][n]
                    = sqrt( 2.0 / nChnBand )
                        * cos( PI * ( k + 1 )
                            * ( 2 * n + 1 ) / ( 2 * nChnBand ) );
            }
        }

        if( Settings_->lifter() > 1 )
        {
            int L = Settings_->lifter();
            CepWei = std::vector<float>();
            CepWei.resize( nDCTBand );
            for( int k = 0; k < nDCTBand; ++k )
            {
                CepWei[k] = 1.0 + ( L / 2.0 ) * sin( PI * ( k + 1 ) / L );
            }
        }
    }

    // Decorrelated FB nChn
    if( DCFilt )
    {
        nChn += DCFilt;
    }

    // Filter bank
    InitFBDone = false;
}

TSpecAnalysis::
~TSpecAnalysis()
{
}


// Execute
std::vector <AFrame> TSpecAnalysis::
Execute( wav_data_t& wav, std::ostream& Ostream, const shared_settings_t& Settings )
{
    Settings_ = Settings;

    Rate = wav.format().sample_rate();

    //  Output spectra
    Fea = 0;
    Cep = Dfb = Spe = 0;
    if( nDCTBand )
    {
        // Cep
        Cep = new TSpectra;
        Fea = Cep;
    }
    if( DCFilt )
    {
        // Dfb
        Dfb = new TSpectra;
        Fea = Dfb;
    }
    if( Cep == 0 && Dfb == 0 )
    {
        // Spe
        Spe = new TSpectra;
        Fea = Spe;
    }

    if( !InitFBDone )
    {
        InitFB();
        InitFBDone = true;
    }

    // Spectral analysis
    SpecAnalysis( wav );

    // Write features into Frames
    std::vector<AFrame> frames = std::vector<AFrame>();
    nVectors = Fea->nVectors;
    VecSize = Fea->Header->sampSize / sizeof( float );
    frames.resize( nVectors );
    for( int i = 0; i < nVectors; ++i )
    {
        frames[i] = AFrame();
        frames[i].Vector.resize( VecSize );
    }

    int index1 = 0;
    int index2 = 0;
    for( int t = 0; t < nVectors; ++t )
    {
        index1 = 0;
        for( int b = 0; b < Fea->nBands; ++b )
        {
            // Vector / band offset
            int v0 = ( t * Fea->nBands + b ) * Fea->bndSize;

            // Static spectra
            if( Settings_->del_only() == 0 )
            {
                index2 = 0;
                for( int n = 0; n < Fea->bndSize; ++n )
                {
                    frames[t].Vector[index1++] = Fea->Vector[v0 + index2++];
                }
            }
            // Delta spectra
            if( Fea->DelVec.size() > 0 )
            {
                index2 = 0;
                for( int n = 0; n < Fea->bndSize; ++n )
                {
                    frames[t].Vector[index1++] = Fea->DelVec[v0 + index2++];
                }
            }
            // Delta - delta spectra
            if( Fea->DelDelVec.size() > 0 )
            {
                index2 = 0;
                for( int n = 0; n < Fea->bndSize; ++n )
                {
                    frames[t].Vector[index1++] = Fea->DelDelVec[v0 + index2++];
                }
            }
        }
    }

    Fea->Output( Ostream );

    if( Cep )
    {
        delete Cep;
    }
    if( Dfb )
    {
        delete Dfb;
    }
    if( Spe )
    {
        delete Spe;
    }

    return frames;
}


// Overall control
void TSpecAnalysis::
SpecAnalysis( wav_data_t& wav )
{
    // Pad the last frame of the file if needed - otherwise the file is truncated
    // and up to (frame_shift - 1) frames will not be processed.
    if( Settings_->zero_pad_last_frame() )
    {
        wav.zero_pad( Settings_->frame_length(), Settings_->frame_shift() );
    }

    // Number of Frames
    int nFrames = ( wav.samples_per_channel() - Settings_->frame_length() ) / Settings_->frame_shift() + 1;

    // Create output multi - band spectra
    if( Cep )
    {
        Cep->Create( nBands, nDCTBand, nFrames, nStrms, Rate, Settings_->del_only(), Settings_->frame_shift() );
    }
    if( Dfb )
    {
        Dfb->Create( nBands, nChnBand, nFrames, nStrms, Rate, Settings_->del_only(), Settings_->frame_shift() );
    }
    if( Spe )
    {
        Spe->Create( nBands, nChnBand, nFrames, nStrms, Rate, Settings_->del_only(), Settings_->frame_shift() );
    }

    // Temp spectra
    std::vector<float> s = std::vector<float>();
    s.resize( nFrames * nFFT2 );
    std::vector<float> u = std::vector<float>();
    u.resize( nFrames * nChn );

    // FFT analysis
    float* a1 = &s[0];
    std::vector<float> x = std::vector<float>();
    x.resize(nFFT);
    for( int i = 0; i < nFrames; ++i )
    {
        // Get a frame
        auto w = i * FrmShift;

        for( int n = 0; n < FrmLength; ++n )
        {
            x[n] = wav.scaled_sample<short>( w, 0 );
        }
        for( int n = FrmLength; n < nFFT; ++n )
        {
            x[n] = 0.0;
        }

        // Pre - processing
        PreProcess( &x[0] );

        // FFT
        FFT( &x[0], a1 );
        a1 += nFFT2;
    }

    // Filter - bank analysis
    a1 = &s[0];
    float* a2 = &u[0];
    for( int i = 0; i < nFrames; ++i )
    {
        FBFilter( a1, a2 );
        a1 += nFFT2;
        a2 += nChn;
    }

    // Log FB spectra
    if( Settings_->log_amp() )
    {
        a1 = &u[0];
        for( int i = 0; i < nFrames; ++i )
        {
            LogFB( a1 );
            a1 += nChn;
        }
    }

    // Sub - band cepstra
    if( Cep )
    {
        a1 = &u[0];
        a2 = &Cep->Vector[0];
        for( int i = 0; i < nFrames; ++i )
        {
            for( int b = 0; b < nBands; ++b )
            {
                DCT( a1, a2 );
                a1 += nChnBand;
                a2 += nDCTBand;
            }
        }
    }

    // Decorrelated FB spectra
    if( Dfb )
    {
        a1 = &u[0];
        a2 = &Dfb->Vector[0];
        for( int i = 0; i < nFrames; ++i )
        {
            if( DCFilt == 1 )
            {
                DecorFB_1( a1, a2 );
            }
            if( DCFilt == 2 )
            {
                DecorFB_2( a1, a2 );
            }
            a1 += nChn;

            // Decorrelated FB nChn
            a2 += ( nChn - DCFilt );
        }
    }

    // FB spectra
    if( Spe )
    {
        a1 = &u[0];
        a2 = &Spe->Vector[0];
        for( int i = 0; i < nFrames; ++i )
        {
            for( int n = 0; n < nChn; ++n )
            {
                a2[n] = a1[n];
            }
            a1 += nChn;
            a2 += nChn;
        }
    }

    // Generate delta spectra
    int DelWin = Settings_->delta();
    if( DelWin )
    {
        if( Cep )
        {
            DelSpectra( DelWin,
                        &Cep->Vector[0],
                        &Cep->DelVec[0],
                        nBands * nDCTBand, nFrames );
        }
        if( Dfb )
        {
            // Decorrelated FB nChn
            DelSpectra( DelWin,
                        &Dfb->Vector[0],
                        &Dfb->DelVec[0],
                        nChn - DCFilt, nFrames );
        }
        if( Spe )
        {
            DelSpectra( DelWin,
                        &Spe->Vector[0],
                        &Spe->DelVec[0],
                        nChn, nFrames );
        }
    }

    // Generate delta - delta spectra
    DelWin = Settings_->delta_delta();
    if( DelWin )
    {
        if( Cep )
        {
            DelSpectra( DelWin,
                        &Cep->DelVec[0],
                        &Cep->DelDelVec[0],
                        nBands * nDCTBand, nFrames );
        }
        if( Dfb )
        {
            // Decorrelated FB nChn
            DelSpectra( DelWin,
                        &Dfb->DelVec[0],
                        &Dfb->DelDelVec[0],
                        nChn - DCFilt, nFrames );
        }
        if( Spe )
        {
            DelSpectra( DelWin,
                        &Spe->DelVec[0],
                        &Spe->DelDelVec[0],
                        nChn, nFrames );
        }
    }

    // Remove cepstral mean
    if( Settings_->mean_rem() )
    {
        if( Cep )
        {
            MeanRemove( &Cep->Vector[0], nBands * nDCTBand, nFrames);
        }
    }
}


// Preprocessing
void TSpecAnalysis::
PreProcess( float* x )
{
    // pre - emphasize
    if( Settings_->emphasize() )
    {
        for( int n = FrmLength - 1; n > 0; --n )
        {
            x[n] -= x[n - 1] * EmpCoe;
        }
        x[0] *= 1.0 - EmpCoe;
    }

    // Ham windowing
    if( Settings_->windowing() )
    {
        for( int n = 0; n < FrmLength; ++n )
        {
            x[n] *= HamWin[n];
        }
    }
}


// Log energy
float TSpecAnalysis::
LogEnergy( float* x )
{
    float e = 0.;
    for( int n = 0; n < FrmLength; ++n )
    {
        e += ( x[n] * x[n] );
    }
    if( e == 0. )
    {
        e = -3.4e+30;
    }
    else
    {
        e = log( e );
    }

    return e;
}


//  FFT
void TSpecAnalysis::
FFT( float* x, float* s )
{
    float* a = new float[nFFT];

    // FFT coefficients
    for( int k = 0; k < nFFT2; ++k )
    {
        int k1 = k + k;
        int k2 = k1 + 1;

        a[k1] = 0.;
        a[k2] = 0.;
        for( int n = 0; n < nFFT; ++n )
        {
            a[k1] += x[n] * Cos[k][n];
            a[k2] += x[n] * Sin[k][n];
        }
    }

    // FFT power spectrum
    for( int k = 0; k < nFFT2; ++k )
    {
        int k1 = k + k;
        float r = a[k1];
        float i = a[k1 + 1];
        s[k] = r * r + i * i;
    }
    delete [] a;

    // FFT amplitude spectrum
    if( Settings_->power_spectrum() == 0 )
    {
        for( int k = 0; k < nFFT2; ++k )
        {
            s[k] = sqrt( s[k] );
        }
    }
}


// Filter bank filtering
void TSpecAnalysis::
FBFilter( float* s, float* u )
{
    for( int n = 0; n < nChn; ++n )
    {
        u[n] = 0.;
    }

    for( int k = ( int )LCut; k <= ( int )HCut; ++k )
    {
        float a = FFunc[k] * s[k - 1];
        int n = Lin2Chn[k];
        if( n > 0 )
        {
            u[n - 1] += a;
        }
        if( n < nChn )
        {
            u[n] += s[k - 1] - a;
        }
    }
}


// Log FB spectra
void TSpecAnalysis::
LogFB( float* u )
{
    for( int n = 0; n < nChn; ++n )
    {
        float a = u[n];
        if( a < 1.0 )
        {
            a = 1.0;
        }
        u[n] = log( a );
    }
}


// DCT
void TSpecAnalysis::
DCT( float* u, float* c )
{
    // C0
    float c0 = 0.;
    for( int n = 0; n < nChnBand; ++n )
    {
        c0 += u[n];
    }
    c0 *= sqrt( 1.0 / ( float )nChnBand );

    // Ck
    for( int k = 0; k < nDCTBand; ++k )
    {
        c[k] = 0.;
        for( int n = 0; n < nChnBand; ++n )
        {
            c[k] += u[n] * CosD[k][n];
        }
    }

    // Lifter
    if( Settings_->lifter() > 1 )
    {
        CepLifter( c,nDCTBand );
    }

    // Use C0
    if( Settings_->c0() )
    {
        for( int k = nDCTBand - 1; k > 0; --k )
        {
            c[k] = c[k - 1];
        }
        c[0] = c0;
    }
}


// Cepstrum lifter
void TSpecAnalysis::
CepLifter( float* c, int cSize )
{
    for( int k = 0; k < cSize; ++k )
    {
        c[k] *= CepWei[k];
    }
}


// Decorrelate FB spectra u, return in c, H(z) = 1  -  z^ - 1
void TSpecAnalysis::
DecorFB_1( float* u, float* c )
{
    int k = 0;
    for( int n = 1; n < nChn; ++n )
    {
        c[k] = u[n] - u[n - 1];
        // c[k] = u[n] - 0.5 * u[n - 1];
        ++k;
    }
}


// Decorrelate FB spectra u, return in c, H(z) = z^ + 1  -  z^ - 1
void TSpecAnalysis::
DecorFB_2( float* u, float* c )
{
    int k = 0;
    for( int n = 1; n < nChn - 1; ++n )
    {
        c[k] = u[n + 1] - u[n - 1];
        ++k;
    }
}


// Normalize log energy
void TSpecAnalysis::
NormLogEne( float* e, int nFrames )
{
    float MaxE = e[0];
    for( int i = 1; i < nFrames; ++i )
        if ( e[i] > MaxE )
        {
            MaxE = e[i];
        }

    float MinE = MaxE - ( 50.0 * log( 10.0 ) ) / 10.0;

    for( int i = 0; i < nFrames; ++i )
    {
        if( e[i] < MinE )
        {
            e[i] = MinE;
        }
        e[i] = 1.0 + ( e[i] - MaxE ) * 0.1;
    }
}


// Compute delta spectra for Vec of size VecSize, length nVecs, into Del
void TSpecAnalysis::
DelSpectra( int DelWin, float* Vec, float* Del, int VecSize, int nVecs )
{
    // Calculate delta using regression
    int N = 0;
    for( int d = 1; d <= DelWin; ++d )
    {
        N += d * d;
    }
    N *= 2;

    for( int i = 0; i < nVecs; ++i )
    {
        float* D = Del + i * VecSize;
        for( int n = 0; n < VecSize; ++n )
        {
            D[n] = 0.;
        }

        for( int d = 1; d <= DelWin; ++d )
        {
            int d0 = ( i - d > 0 )? i - d : 0;
            int d1 = ( i + d < nVecs - 1 )? i + d : nVecs - 1;

            float* V0 = Vec + d0 * VecSize;
            float* V1 = Vec + d1 * VecSize;

            for( int n = 0; n < VecSize; ++n )
            {
                D[n] += d * ( V1[n] - V0[n] );
            }
        }

        for( int n = 0; n < VecSize; ++n )
        {
            D[n] /= N;
        }
    }
}


// Remove spectral mean
void TSpecAnalysis::
MeanRemove( float* Vec, int VecSize, int nVecs )
{
    for( int n = 0; n < VecSize; ++n )
    {
        float* v = Vec + n;

        // Get mean for a component
        float mean = 0.;
        for( int i = 0; i < nVecs; ++i )
        {
            mean +=  * v;
            v += VecSize;
        }
        mean /= nVecs;

        // Remove mean from that component
        v = Vec + n;
        for( int i = 0; i < nVecs; ++i )
        {
             * v -= mean;
            v += VecSize;
        }
    }
}


// Initialize filter bank
void TSpecAnalysis::
InitFB()
{
    // Frequency scale
    FScale = ( Settings_->mel() )? ( double )Rate / ( nFFT * 700. ) : 0.;

    int LowCut = Settings_->low_cut();
    // FBank cut - off frequencies (discrete linear and / or mel)
    LCut = 2;
    HCut = nFFT / 2;
    float MLCut = 0, MHCut = ( FScale > 0. )? L2M( nFFT / 2 + 1 ) : nFFT / 2 + 1;
    if( LowCut > 0 )
    {
        LCut = ( ( double )LowCut / ( double )Rate ) * nFFT + 2.5;
        MLCut = ( FScale > 0. )? 1127 * log(1 + ( double )LowCut / 700. ) : LCut;
        if( LCut < 2 )
        {
            LCut = 2;
        }
    }

    int HighCut = Settings_->high_cut();
    if( HighCut > 0 )
    {
        HCut = ( ( double )HighCut / ( double )Rate ) * nFFT + 0.5;
        MHCut = ( FScale > 0. )? 1127 * log( 1 + ( double )HighCut / 700. ) : HCut;
        if( HCut > nFFT / 2 )
        {
            HCut = nFFT / 2;
        }
    }

    // Filter central frequencies
    FCent = std::vector<float>();
    FCent.resize( nChn + 2 );
    for( int n = 1; n <= nChn + 1; ++n )
    {
        FCent[n] = ( (float )n / ( float )( nChn + 1 ) ) * ( MHCut - MLCut ) + MLCut;
    }

    // Frequency to channel conversion
    Lin2Chn = std::vector<int>();
    Lin2Chn.resize( nFFT / 2 + 1) ;
    for( int k = 1,n = 1; k <= nFFT / 2; ++k )
    {
        float k1 = L2M( k );
        while( FCent[n] < k1 && n < nChn + 1 )
        {
            ++n;
        }
        Lin2Chn[k] = n - 1;
    }

    // Filter transfer function
    FFunc = std::vector<float>();
    FFunc.resize( nFFT / 2 + 1 );
    for( int k = 1; k <= nFFT / 2; ++k )
    {
        int n = Lin2Chn[k];
        if( n > 0 )
        {
            FFunc[k] = ( FCent[n + 1] - L2M( k ) ) / ( FCent[n + 1] - FCent[n] );
        }
        else
        {
            FFunc[k] = ( FCent[1] - L2M( k ) ) / ( FCent[1] - MLCut );
        }
    }
}


// Liner to mel frequency conversion
float TSpecAnalysis::
L2M( int k )
{
    if( FScale == 0. )
    {
        return ( float )k;
    }
    else
    {
        return 1127 * log( 1 + ( k - 1 ) * FScale );
    }
}


// Estimate noise spectrum of size vSize using the first nFrms of vec
void TSpecAnalysis::
EstNoiseSpec( float* vec, int vSize, int nFrms )
{
    for( int n = 0; n < vSize; ++n )
    {
        NSpec[n] = 0.;
    }

    // Estimate power spectrum
    if( Settings_->power_spectrum() )
        for( int i = 0; i < nFrms; ++i )
        {
            float* v = vec + i * vSize;
            for( int n = 0; n < vSize; ++n )
            {
                NSpec[n] += v[n];
            }
        }
    else
        for( int i = 0; i < nFrms; ++i )
        {
            float* v = vec + i * vSize;
            for( int n = 0; n < vSize; ++n )
            {
                NSpec[n] += v[n] * v[n];
            }
        }

    for( int n = 0; n < vSize; ++n )
    {
        NSpec[n] /= nFrms;
    }
}


// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
} // analysis
// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
