// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G
#ifndef QT_AUDIO_FORMAT_HPP_INCLUDED
#define QT_AUDIO_FORMAT_HPP_INCLUDED
// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G

// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I

// Standard Library Includes
#include <cstdint>
#include <string>
#include <ostream>

// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I

// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
namespace qt {
namespace audio {
// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n


enum class sample_type_t
{
    unknown,
    unsigned_int8,
    signed_int16,
    signed_int32,
    floating_point
};

inline
constexpr const char* c_str( const sample_type_t& Type )
{
    switch( Type )
    {
        case sample_type_t::unknown         : return "unknown";
        case sample_type_t::unsigned_int8   : return "unsigned_int8";
        case sample_type_t::signed_int16    : return "signed_int16";
        case sample_type_t::signed_int32    : return "signed_int32";
        case sample_type_t::floating_point  : return "floating_point";
    }
    return nullptr;
}

template< sample_type_t SampleTypeT >
struct sample_type
{
    typedef void value_type;
};

template<>
struct sample_type<sample_type_t::unsigned_int8>
{
    typedef std::uint8_t value_type;
};

template<>
struct sample_type<sample_type_t::signed_int16>
{
    typedef std::int16_t value_type;
};

template<>
struct sample_type<sample_type_t::signed_int32>
{
    typedef std::int32_t value_type;
};

template<>
struct sample_type<sample_type_t::floating_point>
{
    typedef float value_type;
};

template<sample_type_t SampleTypeT>
using sample_type_value_t = typename sample_type<SampleTypeT>::value_type;


inline
constexpr std::size_t bits_per_sample( sample_type_t Type )
{
    switch( Type )
    {
        case sample_type_t::unknown        : return 0;
        case sample_type_t::unsigned_int8  : return 8;
        case sample_type_t::signed_int16   : return 16;
        case sample_type_t::signed_int32   : return 32;
        case sample_type_t::floating_point : return sizeof(float)*8;
    }
    return 0;
}


inline
constexpr std::size_t bytes_per_sample( sample_type_t Type )
{
    switch( Type )
    {
        case sample_type_t::unknown        : return 0;
        case sample_type_t::unsigned_int8  : return 1;
        case sample_type_t::signed_int16   : return 2;
        case sample_type_t::signed_int32   : return 4;
        case sample_type_t::floating_point : return sizeof(float);
    }
    return 0;
}


class format
{
public:

    format()
    : SampleType_( sample_type_t::unknown )
    , SampleRate_( 0 )
    , ChannelCount_( 0 )
    , Codec_( "" )
    {
    }

    explicit format
    (   sample_type_t SampleType,
        std::size_t SampleRate,
        std::size_t ChannelCount,
        const std::string& Codec   )
    : SampleType_( SampleType )
    , SampleRate_( SampleRate )
    , ChannelCount_( ChannelCount )
    , Codec_( Codec )
    {
    }

    explicit operator bool() const
    {
        return SampleType_ != sample_type_t::unknown;
    }

    std::size_t sample_bit_size() const
    {
        return bits_per_sample( SampleType_ );
    }

    std::size_t sample_byte_size() const
    {
        return bytes_per_sample( SampleType_ );
    }

    sample_type_t sample_type() const
    {
        return SampleType_;
    }

    std::size_t sample_rate() const
    {
        return SampleRate_;
    }

    std::size_t channel_count() const
    {
        return ChannelCount_;
    }

    const std::string& codec() const
    {
        return Codec_;
    }

public:

    friend
    bool operator==( const format& Left, const format& Right )
    {
        return
                Left.SampleType_   == Right.SampleType_
            &&  Left.SampleRate_   == Right.SampleRate_
            &&  Left.ChannelCount_ == Right.ChannelCount_
            &&  Left.Codec_        == Right.Codec_;
    }

    friend
    bool operator!=( const format& Left, const format& Right )
    {
        return !( Left == Right );
    }

private:

    sample_type_t SampleType_;     // unsigned_int8, etc
    std::size_t   SampleRate_;
    std::size_t   ChannelCount_;
    std::string   Codec_;
};


template<class CharT, class CharTraitsT>
std::basic_ostream<CharT,CharTraitsT>&
operator<<( std::basic_ostream<CharT,CharTraitsT>& Stream, const format& Format )
{
    Stream
        << Format.channel_count() << "-channel "
        << c_str( Format.sample_type() ) << " " << Format.codec()
        << " @ " << Format.sample_rate() << "Hz";

    return Stream;
}


// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
} // audio
} // qt
// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n


// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G
#endif // QT_AUDIO_FORMAT_HPP_INCLUDED
