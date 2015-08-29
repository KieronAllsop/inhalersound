// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G
#ifndef QT_AUDIO_WAVE_HEADER_HPP_INCLUDED
#define QT_AUDIO_WAVE_HEADER_HPP_INCLUDED
// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G

// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I

// qt::audio Includes
#include "qt/audio/format.hpp"

// Standard Library Includes
#include <cstring>
#include <iostream>

// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I

// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
namespace qt {
namespace audio {
// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n


class wav_header
{
public:

    using format_t      = qt::audio::format;

public:

    static constexpr const char* riff()
    {
        return "RIFF";
    }

    static constexpr const char* wave()
    {
        return "WAVE";
    }

    static constexpr const char* fmt()
    {
        return "fmt ";
    }

    static constexpr const char* data()
    {
        return "data";
    }

    struct riff_header
    {
        char        id[4];
        int32_t     length;
    };

    struct wave_header
    {
        char        id[4];
    };

    struct chunk_header
    {
        char        id[4];
        uint32_t    length;
    };

    struct format_chunk
    {
        int16_t format_tag;
        int16_t channel_count;
        int32_t sample_rate;
        int32_t bytes_per_sec;
        int16_t bytes_per_sample;
        int16_t bits_per_sample;
    };

    struct format_tag
    {
        enum enum_t
        {
            pcm = 1,
            ieee_float = 3
        };
    };

public:

    wav_header( const format_t& Format )
    {
        std::memcpy( &Riff_, riff(), sizeof(Riff_.id) );
        Riff_.length = sizeof( wav_header );
        std::memcpy( &Wave_,  wave(), sizeof(Wave_.id) );
        std::memcpy( &Fmt_, fmt(), sizeof(Fmt_.id ) );
        Fmt_.length = sizeof( format_chunk );
        FmtData_ = format_chunk_from( Format );
        std::memcpy( &Data_, data(), sizeof(Data_.id ) );
        Data_.length = 0;
    }

    void update_size( std::size_t Size, std::size_t HeaderSize = 0 )
    {
        Riff_.length = Size + ( HeaderSize ? HeaderSize : sizeof( wav_header ) );
        Data_.length = Size;
    }

    std::size_t file_size() const
    {
        return Riff_.length;
    }

    std::size_t data_size() const
    {
        return Data_.length;
    }

private:

    riff_header     Riff_;
    wave_header     Wave_;
    chunk_header    Fmt_;
    format_chunk    FmtData_;
    chunk_header    Data_;

public:

    static format_chunk format_chunk_from( const format_t& Format )
    {
        format_chunk FmtData;
        FmtData.channel_count = Format.channel_count();
        FmtData.sample_rate   = Format.sample_rate();

        switch( Format.sample_type() )
        {
            case sample_type_t::unsigned_int8 :
            {
                FmtData.format_tag       = format_tag::pcm;
                FmtData.bytes_per_sample = 1;
                FmtData.bits_per_sample  = 8;
                break;
            }
            case sample_type_t::signed_int16 :
            {
                FmtData.format_tag       = format_tag::pcm;
                FmtData.bytes_per_sample = 2;
                FmtData.bits_per_sample  = 16;
                break;
            }
            case sample_type_t::signed_int32 :
            {
                FmtData.format_tag       = format_tag::pcm;
                FmtData.bytes_per_sample = 4;
                FmtData.bits_per_sample  = 32;
                break;
            }
            case sample_type_t::floating_point :
            {
                FmtData.format_tag       = format_tag::ieee_float;
                FmtData.bytes_per_sample = 4;
                FmtData.bits_per_sample  = 32;
                break;
            }
            default :
            {
                FmtData.format_tag       = 0;
                FmtData.bytes_per_sample = 0;
                FmtData.bits_per_sample  = 0;
                break;
            }
        }
        FmtData.bytes_per_sec = Format.sample_rate() * Format.channel_count() * FmtData.bytes_per_sample;

        return FmtData;
    }

    static format_t format_from( const format_chunk& Format )
    {
        int             SampleSize      = Format.bits_per_sample;
        sample_type_t   SampleType      = sample_type_t::unknown;
        int             SampleRate      = Format.sample_rate;
        int             ChannelCount    = Format.channel_count;

        if( SampleSize == 8 )
        {
            SampleType = sample_type_t::unsigned_int8;
        }
        else if( SampleSize == 16 )
        {
            SampleType = sample_type_t::signed_int16;
        }
        else if( SampleSize == 32 )
        {
            SampleType = sample_type_t::signed_int32;
        }
        else
        {
            SampleType = sample_type_t::unknown;
        }

        return format_t( SampleType, SampleRate, ChannelCount, "audio/pcm" );
    }


};


// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
} // audio
} // qt
// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n


// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G
#endif // QT_AUDIO_WAVE_HEADER_HPP_INCLUDED
