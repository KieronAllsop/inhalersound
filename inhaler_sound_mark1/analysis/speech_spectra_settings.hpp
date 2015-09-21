// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G
#ifndef ANALYSIS_SPEECH_SPECTRA_SETTINGS_HPP_INCLUDED
#define ANALYSIS_SPEECH_SPECTRA_SETTINGS_HPP_INCLUDED
// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G

// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I

// Standard Library Includes
#include <string>
#include <algorithm>
#include <iostream>
#include <memory>
#include <sstream>

// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I

// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
namespace analysis {
// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n


//! \class  speech_spectra_setings.hpp
//! \author Kieron Allsop
//!
//! \brief  Default settings are set. They can be over-riden if a file named
//!         "spectra.spe" is placed in the application directory.
//!
class speech_spectra_settings
{

public:

    speech_spectra_settings
        ( int FrameLength
        , int FrameShift
        , int Windowing
        , int Emphasize
        , int PowerSpectrum
        , int Channels
        , int LowCut
        , int HighCut
        , int Mel
        , int LogAmp
        , int Bands
        , int DCT
        , int C0
        , int Lifter
        , int MeanRem
        , int DCFilter
        , int Delta
        , int DeltaDelta
        , int DelOnly
        , int RemoveSilence
        , std::string OutputExtension
        , bool ExportMfcFile
        , bool ZeroPadLastFrame )
    {
        FrameLength_        = FrameLength;
        FrameShift_         = FrameShift;
        Windowing_          = Windowing;
        Emphasize_          = Emphasize;
        PowerSpectrum_      = PowerSpectrum;
        Channels_           = Channels;
        LowCut_             = LowCut;
        HighCut_            = HighCut;
        Mel_                = Mel;
        LogAmp_             = LogAmp;
        Bands_              = Bands;
        DCT_                = DCT;
        C0_                 = C0;
        Lifter_             = Lifter;
        MeanRem_            = MeanRem;
        DCFilter_           = DCFilter;
        Delta_              = Delta;
        DeltaDelta_         = DeltaDelta;
        DelOnly_            = DelOnly;
        RemoveSilence_      = RemoveSilence;
        OutputExtension_    = OutputExtension;
        ExportMfcFile_      = ExportMfcFile;
        ZeroPadLastFrame_   = ZeroPadLastFrame;
    }

public:

    // Observers

    const int& frame_length() const
    {
        return FrameLength_;
    }


    const int& frame_shift() const
    {
        return FrameShift_;
    }


    const int& windowing() const
    {
        return Windowing_;
    }


    const int& emphasize() const
    {
        return Emphasize_;
    }


    const int& power_spectrum() const
    {
        return PowerSpectrum_;
    }


    const int& channels() const
    {
        return Channels_;
    }


    const int& low_cut() const
    {
        return LowCut_;
    }


    const int& high_cut() const
    {
        return HighCut_;
    }

    const int& mel() const
    {
        return Mel_;
    }


    const int& log_amp() const
    {
        return LogAmp_;
    }


    const int& bands() const
    {
        return Bands_;
    }


    const int& dct() const
    {
        return DCT_;
    }


    const int& c0() const
    {
        return C0_;
    }


    const int& lifter() const
    {
        return Lifter_;
    }


    const int& mean_rem() const
    {
        return MeanRem_;
    }


    const int& dc_filter() const
    {
        return DCFilter_;
    }


    const int& delta() const
    {
        return Delta_;
    }


    const int& delta_delta() const
    {
        return DeltaDelta_;
    }


    const int& del_only() const
    {
        return DelOnly_;
    }


    const int& remove_silence() const
    {
        return RemoveSilence_;
    }


    const std::string& output_extension() const
    {
        return OutputExtension_;
    }


    const bool& export_mfcdata_to_disk() const
    {
        return ExportMfcFile_;
    }


    const bool& zero_pad_last_frame() const
    {
        return ZeroPadLastFrame_;
    }

public:

    static
    std::shared_ptr<analysis::speech_spectra_settings> create_from_file()
    {
        // Default values
        int FrameLength             = 160;
        int FrameShift              = 80;
        int Windowing               = 1;
        int Emphasize               = 1;
        int PowerSpectrum           = 1;
        int Channels                = 23;
        int LowCut                  = 100;
        int HighCut                 = 3500;
        int Mel                     = 1;
        int LogAmp                  = 1;
        int Bands                   = 1;
        int DCT                     = 12;
        int C0                      = 1;
        int Lifter                  = 1;
        int MeanRem                 = 1;
        int DCFilter                = 0;
        int Delta                   = 3;
        int DeltaDelta              = 2;
        int DelOnly                 = 0;
        int RemoveSilence           = 0;
        std::string OutputExtension = "mfc";
        bool ExportMfcFile          = false;
        bool ZeroPadLastFrame       = false;

        std::string FileToImport = "spectra.spe";
        std::string Line;
        std::ifstream SettingsFile (FileToImport);

        if( SettingsFile.is_open() )
        {

            int UserFrameLength = FrameLength;
            int UserFrameShift = FrameShift;

            while( std::getline( SettingsFile, Line ) )
            {

                if( Line.empty() )
                {
                    continue;
                }

                if( Line.substr( 0, 2 ).compare( "//" ) == 0 )
                {
                    continue;
                }

                std::string Buffer;
                std::stringstream StringStream( Line );
                std::vector< std::string > Elements;

                while( StringStream.good() )
                {
                    std::getline( StringStream, Buffer, ':' );
                    Elements.emplace_back( Buffer );
                }

                if( Elements.size() >= 2 )
                {
                    std::string SettingName = Elements[0];
                    std::transform( SettingName.begin(), SettingName.end(), SettingName.begin(), ::tolower );
                    std::string SettingValue = Elements[1];
                    std::transform( SettingValue.begin(), SettingValue.end(), SettingValue.begin(), ::tolower );

                    if( SettingValue.length() == 0 )
                    {
                        continue;
                    }

                    if( SettingName == "frame" )
                    {
                        UserFrameLength = atoi( SettingValue.c_str() );
                    }
                    else if( SettingName == "shift" )
                    {
                        UserFrameShift = atoi( SettingValue.c_str() );
                    }

                    // Windowing must be 0 or 1
                    else if( SettingName == "windowing" )
                    {
                        auto UserWindowing = atoi( SettingValue.c_str() );
                        if( UserWindowing == 0 || UserWindowing == 1 )
                        {
                            Windowing = UserWindowing;
                        }
                    }

                    // Emphasize must be 0 or 1
                    else if( SettingName == "emphasise" || SettingName == "emphasize" )
                    {
                        auto UserEmphasize = atoi( SettingValue.c_str() );
                        if( UserEmphasize == 0 || UserEmphasize == 1 )
                        {
                            Emphasize = UserEmphasize;
                        }
                    }

                    // Power Spectrum must be 0 or 1
                    else if( SettingName == "pwr spec" )
                    {
                        auto UserPowerSpectrum = atoi( SettingValue.c_str() );
                        if( UserPowerSpectrum == 0 || UserPowerSpectrum == 1 )
                        {
                            PowerSpectrum = UserPowerSpectrum;
                        }
                    }

                    // Only the default value of 23 is allowed in this system
                    else if( SettingName == "channels" )
                    {
                        continue;
                    }

                    // Low Cut range is 0 to 150
                    else if( SettingName == "low cut" )
                    {
                        auto UserLowCut = atoi( SettingValue.c_str() );
                        if( UserLowCut >= 0 && UserLowCut <= 150 )
                        {
                            LowCut = UserLowCut;
                        }
                    }

                    // High Cut range is 3500 to 4000
                    else if( SettingName == "high cut" )
                    {
                        auto UserHighCut = atoi( SettingValue.c_str() );
                        if( UserHighCut >= 3500 && UserHighCut <= 4000 )
                        {
                            HighCut = UserHighCut;
                        }
                    }

                    // Mel must be 0 or 1
                    else if( SettingName == "mel" )
                    {
                        auto UserMel = atoi( SettingValue.c_str() );
                        if( UserMel == 0 || UserMel == 1 )
                        {
                            Mel = UserMel;
                        }
                    }

                    // Log Amp must be 0 or 1
                    else if( SettingName == "log amp" || SettingName == "log" )
                    {
                        auto UserLogAmp = atoi( SettingValue.c_str() );
                        if( UserLogAmp == 0 || UserLogAmp == 1 )
                        {
                            LogAmp = UserLogAmp;
                        }
                    }

                    // Only the default value of 1 is allowed in this system
                    else if( SettingName == "bands" )
                    {
                        continue;
                    }

                    // Only the default value of 12 is allowed in this system
                    else if( SettingName == "dct" )
                    {
                        continue;
                    }

                    // c0 must be 0 or 1
                    else if( SettingName == "c0" )
                    {
                        auto UserC0 = atoi( SettingValue.c_str() );
                        if( UserC0 == 0 || UserC0 == 1 )
                        {
                            C0 = UserC0;
                        }
                    }

                    // Lifter must be 0 or 1
                    else if( SettingName == "lifter" )
                    {
                        auto UserLifter = atoi( SettingValue.c_str() );
                        if( UserLifter == 0 || UserLifter == 1 )
                        {
                            Lifter = UserLifter;
                        }
                    }

                    // Mean Rem must be 0 or 1
                    else if( SettingName == "mean rem" )
                    {
                        auto UserMeanRem = atoi( SettingValue.c_str() );
                        if( UserMeanRem == 0 || UserMeanRem == 1 )
                        {
                            MeanRem = UserMeanRem;
                        }
                    }

                    // DC Filter can only be 0 in this system
                    else if( SettingName == "dc-filter" )
                    {
                        continue;
                    }

                    // Delta can be 0, 2 or 3
                    else if( SettingName == "delta (+-)" )
                    {
                        auto UserDelta = atoi( SettingValue.c_str() );
                        if( UserDelta == 0 || UserDelta == 2 || UserDelta == 3 )
                        {
                            Delta = UserDelta;
                        }
                    }

                    // Delta Delta can be 0 or 2
                    else if( SettingName == "deldel (+-)" )
                    {
                        auto UserDeltaDelta = atoi( SettingValue.c_str() );
                        if( UserDeltaDelta == 0 || UserDeltaDelta == 2 )
                        {
                            DeltaDelta = UserDeltaDelta;
                        }
                    }

                    // Del Only can only be 0 in this system
                    else if( SettingName == "del only" )
                    {
                        continue;
                    }

                    // Remove Silence can be 0 or 1
                    else if( SettingName == "sil rem" )
                    {
                        auto UserRemoveSilence = atoi( SettingValue.c_str() );
                        if( UserRemoveSilence == 0 || UserRemoveSilence ==1 )
                        {
                            RemoveSilence = UserRemoveSilence;
                        }
                    }

                    // Extension of feature file can only be .mfc in this system
                    else if( SettingName == "out spec" || SettingName == "i/o spec")
                    {
                        continue;
                    }

                    // This can be set to true (1) or false (0)
                    else if( SettingName == "export mfc" )
                    {
                        auto UserExportMfcFile = atoi( SettingValue.c_str() );
                        if( UserExportMfcFile == 0 || UserExportMfcFile == 1 )
                        {
                            ExportMfcFile = UserExportMfcFile;
                        }
                    }

                    // This can be set to true (1) or false (0)
                    else if( SettingName == "zero pad" )
                    {
                        auto UserZeroPadLastFrame = atoi( SettingValue.c_str() );
                        if( UserZeroPadLastFrame == 0 || UserZeroPadLastFrame == 1 )
                        {
                            ZeroPadLastFrame = UserZeroPadLastFrame;
                        }
                    }
                }
            }
            SettingsFile.close();

            // Frame and Shift are related so cannot be validated until both values
            // have been read in
            if( UserFrameLength >= 2 && UserFrameLength > UserFrameShift )
            {
                FrameLength = UserFrameLength;
            }

            if( UserFrameShift >= 1 && UserFrameShift < UserFrameLength )
            {
                FrameShift = UserFrameShift;
            }
        }

        return std::make_shared<analysis::speech_spectra_settings>
                     (   FrameLength, FrameShift, Windowing, Emphasize, PowerSpectrum,
                         Channels, LowCut, HighCut, Mel, LogAmp, Bands, DCT, C0,
                         Lifter, MeanRem, DCFilter, Delta, DeltaDelta, DelOnly,
                         RemoveSilence, OutputExtension, ExportMfcFile, ZeroPadLastFrame  );

    }

private:

    // FFT Parameters
    int             FrameLength_;
    int             FrameShift_;
    int             Windowing_;
    int             Emphasize_;
    int             PowerSpectrum_;

    // Filter Bank Parameters
    int             Channels_;
    int             LowCut_;
    int             HighCut_;
    int             Mel_;

    // Log FFT/FB Parameters
    int             LogAmp_;

    // Sub-Band Parameters
    int             Bands_;

    // Cepstrum Parameters
    int             DCT_;
    int             C0_;
    int             Lifter_;
    int             MeanRem_;

    // Decorrelated FB Parameters
    int             DCFilter_;

    // Delta Parameters
    int             Delta_;
    int             DeltaDelta_;
    int             DelOnly_;

    // Remove Silence Parameters
    int             RemoveSilence_;

    // File Parameters
    std::string     OutputExtension_;

    // Advanced User Setting - if set to true Feature File
    // will be exported to disk on wave file import
    bool            ExportMfcFile_;

    // Advanced User Setting - if set to true the remainder samples
    // at the end of the wave file, which would otherwise be truncated
    // during framing, are added to a final frame which is padded
    // with zeroes
    bool            ZeroPadLastFrame_;

};


// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
} // analysis
// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n

// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G
#endif // ANALYSIS_SPEECH_SPECTRA_SETTINGS_HPP_INCLUDED
