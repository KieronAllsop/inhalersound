// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G
#ifndef QT_AUDIO_RAW_DATA_HPP_INCLUDED
#define QT_AUDIO_RAW_DATA_HPP_INCLUDED
// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G

// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I

// qt::audio Includes
#include "qt/audio/format.hpp"
#include "qt/audio/decode_buffer.hpp"

// Boost Includes
#include <boost/exception/all.hpp>

// Standard Library Includes
#include <string>
#include <stdexcept>
#include <cstring>

// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I

// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
namespace qt {
namespace audio {
// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n

namespace exception
{
     struct incorrect_format : virtual boost::exception, virtual std::exception {};
}

class raw_data
{
public:

    raw_data( const raw_data& other ) = delete;
    raw_data& operator=( const raw_data& other ) = delete;

    raw_data
    (   int SampleBitSize,
        sample_type_t SampleType,
        int SampleRate,
        int ChannelCount,
        const std::string& Codec,
        std::vector<char>&& Data   )
    : SampleBitSize_( SampleBitSize )
    , SampleByteSize_( SampleBitSize / 8 )
    , SampleType_( SampleType )
    , SampleRate_( SampleRate )
    , ChannelCount_( ChannelCount )
    , SamplesPerChannel_( Data.size() / SampleByteSize_ / ChannelCount )
    , Codec_( Codec )
    , Data_( Data )
    {
    }

    raw_data()
    : SampleBitSize_( 0 )
    , SampleByteSize_( 0 )
    , SampleType_( sample_type_t::unknown )
    , SampleRate_( 0 )
    , ChannelCount_( 0 )
    , SamplesPerChannel_( 0 )
    {
    }

    // Modifiers
    void add_buffer( const decode_buffer& Buffer )
    {
        if( SampleType_ == sample_type_t::unknown )
        {
            // Initialise the format
            SampleBitSize_      = Buffer.sample_bit_size();
            SampleByteSize_     = Buffer.sample_byte_size();
            SampleType_         = Buffer.sample_type();
            SampleRate_         = Buffer.sample_rate();
            ChannelCount_       = Buffer.channel_count();
            Codec_              = Buffer.codec();
        }
        else if(    SampleBitSize_  != Buffer.sample_bit_size()
                ||  SampleType_     != Buffer.sample_type()
                ||  SampleRate_     != Buffer.sample_rate()
                ||  ChannelCount_   != Buffer.channel_count()
                ||  Codec_          != Buffer.codec()   )
        {
            BOOST_THROW_EXCEPTION( exception::incorrect_format() );
        }

        SamplesPerChannel_ += Buffer.samples_per_channel();

        std::size_t Size = Data_.size();
        Data_.resize( Data_.size() + Buffer.size() );
        std::memcpy( static_cast<void*>( &Data_[Size] ), Buffer.data(), Buffer.size() );
    }

    // Observers

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
        return static_cast<const void*>( &Data_[0] );
    }

    std::size_t size() const
    {
        return Data_.size();
    }

private:

    int                 SampleBitSize_;
    int                 SampleByteSize_;
    sample_type_t       SampleType_;
    int                 SampleRate_;
    int                 ChannelCount_;
    int                 SamplesPerChannel_;
    std::string         Codec_;
    std::vector<char>   Data_;
};

// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
} // audio
} // qt
// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n


// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G
#endif // QT_AUDIO_RAW_DATA_HPP_INCLUDED
