// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G
#ifndef QT_AUDIO_DECODE_BUFFER_HPP_INCLUDED
#define QT_AUDIO_DECODE_BUFFER_HPP_INCLUDED
// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G

// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I

// qt::audio Includes
#include "qt/audio/format.hpp"

// Standard Library Includes
#include <string>

// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I

// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
namespace qt {
namespace audio {
// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n

class decode_buffer
{
public:

    decode_buffer()
    : SampleBitSize_( 0 )
    , SampleByteSize_( 0 )
    , SampleType_( sample_type_t::unknown )
    , SampleRate_( 0 )
    , ChannelCount_( 0 )
    , SamplesPerChannel_( 0 )
    , Data_( nullptr )
    , Size_( 0 )
    {
    }

    decode_buffer
    (   int SampleBitSize,
        sample_type_t SampleType,
        int SampleRate,
        int ChannelCount,
        int SamplesPerChannel,
        const std::string& Codec,
        const void* Data, std::size_t Size   )
    : SampleBitSize_( SampleBitSize )
    , SampleByteSize_( SampleBitSize/8 )
    , SampleType_( SampleType )
    , SampleRate_( SampleRate )
    , ChannelCount_( ChannelCount )
    , SamplesPerChannel_( SamplesPerChannel )
    , Codec_( Codec )
    , Data_( Data )
    , Size_( Size )
    {
    }

    int sample_bit_size() const
    {
        return SampleBitSize_;
    }

    int sample_byte_size() const
    {
        return SampleByteSize_;
    }

    sample_type_t sample_type() const
    {
        return SampleType_;
    }

    int sample_rate() const
    {
        return SampleRate_;
    }

    int channel_count() const
    {
        return ChannelCount_;
    }

    int samples_per_channel() const
    {
        return SamplesPerChannel_;
    }

    const std::string& codec() const
    {
        return Codec_;
    }

    const void* data() const
    {
        return Data_;
    }

    std::size_t size() const
    {
        return Size_;
    }

private:

    int             SampleBitSize_;
    int             SampleByteSize_;
    sample_type_t   SampleType_;
    int             SampleRate_;
    int             ChannelCount_;
    int             SamplesPerChannel_;
    std::string     Codec_;
    const void*     Data_;
    std::size_t     Size_;
};

// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
} // audio
} // qt
// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n


// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G
#endif // QT_AUDIO_DECODE_BUFFER_HPP_INCLUDED
