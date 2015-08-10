// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G
#ifndef QT_AUDIO_AUDIO_BUFFER_HPP_INCLUDED
#define QT_AUDIO_AUDIO_BUFFER_HPP_INCLUDED
// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G

// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I

// qt::audio Includes
#include "qt/audio/format.hpp"

// Standard Library Includes
#include <string>
#include <cassert>
#include <chrono>
#include <type_traits>
#include <limits>

// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I

// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
namespace qt {
namespace audio {
// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n

class audio_buffer
{
public:

    using sample_type_t = qt::audio::sample_type_t;
    using duration_t    = std::chrono::nanoseconds;
    using format_t      = qt::audio::format;

public:

    audio_buffer() noexcept
    : SamplesPerChannel_( 0 )
    , Duration_         ( 0 )
    , NormalisedSample_ ( nullptr )
    , Data_             ( nullptr )
    , Size_             ( 0 )
    {
    }

    explicit audio_buffer
    (   const format& Format,
        const void* Data,
        std::size_t Size   ) noexcept
    : Format_           ( Format )
    , SamplesPerChannel_( Size / Format.channel_count() / Format.sample_byte_size() )
    , Duration_         ( duration_t( SamplesPerChannel_ ) * 1'000'000'000 / Format.sample_rate() )
    , NormalisedSample_ ( nullptr )
    , Data_             ( Data )
    , Size_             ( Size )
    {
        initialise_normalised_sample( Format_.sample_type() );
    }

    explicit audio_buffer
    (   const format& Format,
        std::size_t SamplesPerChannel   ) noexcept
    : Format_           ( Format )
    , SamplesPerChannel_( SamplesPerChannel )
    , Duration_         ( duration_t( SamplesPerChannel_ ) * 1'000'000'000 / Format.sample_rate() )
    , NormalisedSample_ ( nullptr )
    , Data_             ( nullptr )
    , Size_             ( 0 )
    {
    }

    audio_buffer( const audio_buffer& Other ) noexcept
    {
        Format_             = Other.Format_;
        SamplesPerChannel_  = Other.SamplesPerChannel_;
        Duration_           = Other.Duration_;
        Data_               = Other.Data_;
        Size_               = Other.Size_;
        initialise_normalised_sample( Format_.sample_type() );
    }

    audio_buffer& operator=( const audio_buffer& Other ) noexcept
    {
        audio_buffer Temp( Other );
        swap( *this, Temp );
        initialise_normalised_sample( Format_.sample_type() );
        return *this;
    }

    friend void swap( audio_buffer& Lhs, audio_buffer& Rhs ) noexcept
    {
        using std::swap;
        swap( Lhs.Format_,              Rhs.Format_ );
        swap( Lhs.SamplesPerChannel_,   Rhs.SamplesPerChannel_ );
        swap( Lhs.Duration_,            Rhs.Duration_ );
        swap( Lhs.Data_,                Rhs.Data_ );
        swap( Lhs.Size_,                Rhs.Size_ );
        Lhs.initialise_normalised_sample( Lhs.Format_.sample_type() );
        Rhs.initialise_normalised_sample( Rhs.Format_.sample_type() );
    }

    const format_t& format() const
    {
        return Format_;
    }

    std::size_t samples_per_channel() const
    {
        return SamplesPerChannel_;
    }

    const duration_t duration() const
    {
        return Duration_;
    }

    const void* data() const
    {
        return Data_;
    }

    std::size_t size() const
    {
        return Size_;
    }

    template<class T>
    const T& sample_cast( std::size_t Index, std::size_t Channel ) const
    {
        assert( sizeof(T) == Format_.sample_byte_size() );
        assert( Channel < Format_.channel_count() );
        assert( Index < SamplesPerChannel_ );

        return *static_cast<const T*>( static_cast<const void*>( static_cast<const char*>( Data_ ) + offset( Index, Channel ) ) );
    }

    template<class T>
    typename std::enable_if_t< std::is_integral<T>{} && std::is_signed<T>{}, T >
    scaled_sample( std::size_t Index, std::size_t Channel ) const
    {
        assert( Channel < Format_.channel_count() );
        assert( Index < SamplesPerChannel_ );

        switch( Format_.sample_type() )
        {
            case sample_type_t::unsigned_int8 :
            {
                return (1 << (sizeof(T)-1)*8) * ( sample_cast<std::int8_t>( Index, Channel ) ^ 0x80 );
            }
            case sample_type_t::signed_int16 :
            {
                auto Value = sample_cast<std::int16_t>( Index, Channel );
                return sizeof(T) < 2 ? Value / ( 1 << 8 ) : Value * ( 1 << ( sizeof(T)-2 )*8 );
            }
            case sample_type_t::signed_int32 :
            {
                auto Value = sample_cast<std::int32_t>( Index, Channel );
                return sizeof(T) < 4 ? Value / ( 1 << ( 4-sizeof(T) )*8 ) : Value * ( 1 << ( sizeof(T)-4 )*8 );
            }
            case sample_type_t::floating_point :
            {
                double Value = sample_cast<float>( Index, Channel );
                Value *= std::numeric_limits<T>::max();
                return static_cast<T>( Value );
            }
            default :
            {
                return 0;
            }
        }
        return 0;
    }

    template<class T>
    typename std::enable_if_t< std::is_integral<T>{} && std::is_unsigned<T>{}, T >
    scaled_sample( std::size_t Index, std::size_t Channel ) const
    {
        return scaled_sample< std::make_signed_t<T> >( Index, Channel ) ^ ( 1 << ( sizeof(T) * 8 -1 ) );
    }

    template<class T>
    typename std::enable_if_t< std::is_floating_point<T>{}, T >
    scaled_sample( std::size_t Index, std::size_t Channel ) const
    {
        return static_cast<T>( normalised_sample( Index, Channel ) );
    }

    double normalised_sample( std::size_t Index, std::size_t Channel ) const
    {
        assert( NormalisedSample_ );
        assert( Channel < Format_.channel_count() );
        assert( Index < SamplesPerChannel_ );

        return (this->*NormalisedSample_)( Index, Channel );
    }

private:

    std::size_t offset( std::size_t Index, std::size_t Channel ) const
    {
        return Index * Format_.sample_byte_size() * Format_.channel_count() + Channel * Format_.sample_byte_size();
    }

    // return a double value between 1.0 and -1.0 to enable drawing of the WAV
    double normalised_unsigned_int8( std::size_t Index, std::size_t Channel ) const
    {
        double Value = (1 << 24) * ( static_cast<std::int32_t>( sample_cast<std::int8_t>( Index, Channel ) ^ 0x80 ) );
        return Value / 0x7F'FF'FF'FF;
    }

    double normalised_signed_int16( std::size_t Index, std::size_t Channel ) const
    {
        double Value = (1 << 16) * static_cast<std::int32_t>( sample_cast<std::int16_t>( Index, Channel ) );
        return Value / 0x7F'FF'FF'FF;
    }

    double normalised_signed_int32( std::size_t Index, std::size_t Channel ) const
    {
        double Value = sample_cast<std::int32_t>( Index, Channel );
        return Value / 0x7F'FF'FF'FF;
    }

    double normalised_float( std::size_t Index, std::size_t Channel ) const
    {
        return sample_cast<float>( Index, Channel );
    }

    void initialise_normalised_sample( sample_type_t SampleType )
    {
        switch( SampleType )
        {
            case sample_type_t::unsigned_int8 :
            {
                NormalisedSample_ = &audio_buffer::normalised_unsigned_int8;
                break;
            }
            case sample_type_t::signed_int16 :
            {
                NormalisedSample_ = &audio_buffer::normalised_signed_int16;
                break;
            }
            case sample_type_t::signed_int32 :
            {
                NormalisedSample_ = &audio_buffer::normalised_signed_int32;
                break;
            }
            case sample_type_t::floating_point :
            {
                NormalisedSample_ = &audio_buffer::normalised_float;
                break;
            }
            default :
            {
                NormalisedSample_ = nullptr;
                break;
            }
        }
    }

private:

    typedef double ( audio_buffer::*normalised_sample_func_t )( std::size_t, std::size_t ) const;

    format_t                    Format_;
    std::size_t                 SamplesPerChannel_;
    duration_t                  Duration_;
    normalised_sample_func_t    NormalisedSample_;
    const void*                 Data_;              // pointer to Data
    std::size_t                 Size_;              // total size
};


// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
} // audio
} // qt
// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n


// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G
#endif // QT_AUDIO_AUDIO_BUFFER_HPP_INCLUDED
