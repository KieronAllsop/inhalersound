// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G
#ifndef QT_AUDIO_WAV_DATA_HPP_INCLUDED
#define QT_AUDIO_WAV_DATA_HPP_INCLUDED
// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G

// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I

// qt::audio Includes
#include "qt/audio/audio_buffer.hpp"
#include "qt/audio/wav_header.hpp"

// Boost Includes
#include <boost/exception/all.hpp>
#include <boost/filesystem.hpp>

// Standard Library Includes
#include <vector>
#include <string>
#include <cstring>
#include <chrono>

// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I

// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
namespace qt {
namespace audio {
// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n

class wav_data
{
public:

    using path_t        = boost::filesystem::path;
    using format_t      = qt::audio::format;
    using duration_t    = std::chrono::nanoseconds;
    using buffer_t      = audio_buffer;

public:

    wav_data( const wav_data& other ) = delete;
    wav_data& operator=( const wav_data& other ) = delete;

    wav_data()
    : HeaderSize_( 0 )
    , FrameSize_( 0 )
    , FrameShift_( 0 )
    {
    }

    wav_data
    (   std::vector<char>&& Data   )
    : HeaderSize_( 0 )
    , Data_( Data )
    , FrameSize_( 0 )
    , FrameShift_( 0 )
    {
        const char* BufferStart;
        format_t Format;
        std::tie( BufferStart, Format ) = process_wav_header( &Data_[0] );
        HeaderSize_ = BufferStart - &Data_[0];
        Buffer_ = buffer_t( Format, static_cast<const void*>( BufferStart ), Data_.size()-HeaderSize_ );
    }

    // Modifiers
    void add_buffer( const buffer_t& Buffer )
    {
        if(     Buffer_.format().sample_type() != sample_type_t::unknown
            &&  Buffer_.format() != Buffer.format() )
        {
            BOOST_THROW_EXCEPTION( exception::incorrect_format() );
        }
        if( Data_.empty() )
        {
            write_wav_header( Buffer.format() );
        }

        auto WavSize = header().file_size();

        // Ignore zero padding first
        Data_.resize( WavSize + Buffer.size() );
        std::memcpy( static_cast<void*>( &Data_[WavSize] ), Buffer.data(), Buffer.size() );

        auto NewWavDataSize = header().data_size() + Buffer.size();

        // Now add zero padding
        auto ZeroPadSize = zero_padding_required( samples_per_channel( NewWavDataSize, Buffer.format() ) );

        Data_.resize( Data_.size() + ZeroPadSize, 0 );

        Buffer_ = buffer_t( Buffer.format(), static_cast<const void*>( &Data_[header_size()] ), Data_.size()-header_size() );

        // The wav should only report the wav data size and not include zero padding
        update_wav_header( Buffer_.size() - ZeroPadSize );
    }

    void zero_pad( std::size_t FrameSize, std::size_t FrameShift )
    {
        if( FrameSize != FrameSize_ && FrameShift != FrameShift_ )
        {
            FrameSize_ = FrameSize;
            FrameShift_ = FrameShift;
            auto ZeroPadSize = zero_padding_required( samples_per_channel( header().data_size(), Buffer_.format() ) );
std::cout << "zero_pad - ZeroPadSize = " << ZeroPadSize << std::endl;
            Data_.resize( header().file_size() + ZeroPadSize, 0 );
std::cout << "zero_pad - data size after resize = " << Data_.size() << std::endl;
std::cout << "zero_pad - next line is Buffer_ = buffer_t( Buffer_.format(), static_cast<const void*>( &Data_[header_size()] ), Data_.size()-header_size() )" << std::endl;
            Buffer_ = buffer_t( Buffer_.format(), static_cast<const void*>( &Data_[header_size()] ), Data_.size()-header_size() );
        }
    }

    // Observers

    const format_t& format() const
    {
        return Buffer_.format();
    }

    std::size_t samples_per_channel() const
    {
        return Buffer_.samples_per_channel();
    }

    duration_t duration() const
    {
        return Buffer_.duration();
    }

    duration_t duration_from( std::size_t Samples ) const
    {
        return Buffer_.duration_from( Samples );
    }

    const buffer_t& buffer() const
    {
        return Buffer_;
    }

    std::size_t header_size() const
    {
        return HeaderSize_;
    }

    const wav_header& header() const
    {
        return *static_cast<const wav_header*>( static_cast<const void*>( &Data_[0] ) );
    }

    // Access to the whole buffer including header and zero padding
    const void* data() const
    {
        return static_cast<const void*>( &Data_[0] );
    }

    // Size of whole buffer including header and zero padding
    std::size_t size() const
    {
        return Data_.size();
    }

    template<class T>
    T scaled_sample( std::size_t Index, std::size_t Channel ) const
    {
        return Buffer_.scaled_sample<T>( Index, Channel );
    }

private:

    std::pair<const char*, format_t> process_wav_header( const char* Data )
    {
        auto Riff = reinterpret_cast<const wav_header::riff_header*>( Data );
        if( std::strncmp( Riff->id, wav_header::riff(), sizeof(Riff->id) ) != 0 )
        {
            BOOST_THROW_EXCEPTION( exception::incorrect_format() );
        }
        Data += sizeof( wav_header::riff_header );

        auto Wave = reinterpret_cast<const wav_header::wave_header*>( Data );
        if( std::strncmp( Wave->id, wav_header::wave(), sizeof(Wave->id) ) != 0 )
        {
            BOOST_THROW_EXCEPTION( exception::incorrect_format() );
        }
        Data += sizeof( wav_header::wave_header );

        auto FmtChunk = reinterpret_cast<const wav_header::chunk_header*>( Data );
        if( std::strncmp( FmtChunk->id, wav_header::fmt(), sizeof(FmtChunk->id) ) != 0 )
        {
            BOOST_THROW_EXCEPTION( exception::incorrect_format() );
        }
        Data += sizeof( wav_header::chunk_header );

        auto FormatChunk = reinterpret_cast<const wav_header::format_chunk*>( Data );
        if( FormatChunk->format_tag != wav_header::format_tag::pcm )
        {
            BOOST_THROW_EXCEPTION( exception::incorrect_format() );
        }
        Data += sizeof( wav_header::format_chunk );

        // Skip any remaning bytes for this chunk
        auto Padding = FmtChunk->length - sizeof(wav_header::format_chunk);
        Data += Padding;

        // Read the next bytes as a data chunk header
        auto DataHeader = reinterpret_cast<const wav_header::chunk_header*>( Data );
        Data += sizeof( wav_header::chunk_header );

        // Is this the data chunk header?
        while( std::strncmp( DataHeader->id, wav_header::data(), sizeof(DataHeader->id) ) != 0 )
        {
            // If not then read the next set of bytes and see if it is the data chunk header
            DataHeader = reinterpret_cast<const wav_header::chunk_header*>( Data );
            Data += sizeof( wav_header::chunk_header );
        }

        return std::make_pair( Data, wav_header::format_from( *FormatChunk ) );
    }

    void write_wav_header( const format_t& Format )
    {
        wav_header Header( Format );
        Data_.resize( sizeof( wav_header ) );
        std::memcpy( &Data_[0], &Header, sizeof(wav_header) );
        HeaderSize_ = sizeof( wav_header );
    }

    void update_wav_header( std::size_t DataSize )
    {
        wav_header* Header = static_cast<wav_header*>( static_cast<void*>( &Data_[0] ) );
        Header->update_size( DataSize, HeaderSize_ );
    }

    static
    std::size_t samples_per_channel( std::size_t DataSize, const format_t& Format )
    {
std::cout << "samples_per_channel - DataSize = " << DataSize << std::endl;
std::cout << "samples_per_channel - Format.sample_byte_size = " << Format.sample_byte_size() << std::endl;
std::cout << "samples_per_channel - Format.channel_count = " << Format.channel_count() << std::endl;
std::cout << "Calculation is DataSize / Format.sample_byte_size() / Format.channel_count()" << std::endl;
        return DataSize / Format.sample_byte_size() / Format.channel_count();
    }

    static
    std::size_t bytes_from_samples( std::size_t Samples, const format_t& Format )
    {
std::cout << "bytes_from_samples - Samples = " << Samples << std::endl;
std::cout << "bytes_from_samples - Format.sample_byte_size = " << Format.sample_byte_size() << std::endl;
std::cout << "bytes_from_samples - Format.channel_count = " << Format.channel_count() << std::endl;
std::cout << "Calculation is Samples * Format.sample_byte_size() * Format.channel_count()" << std::endl;
        return Samples * Format.sample_byte_size() * Format.channel_count();
    }

    std::size_t zero_padding_required( std::size_t SampleSize )
    {
std::cout << "zero_padding_required - SampleSize = " << SampleSize << std::endl;
        if( !FrameSize_ || FrameSize_ == SampleSize )
        {
            return 0;
        }

        std::size_t PaddingSamples = 0;
        if( FrameSize_ < SampleSize )
        {
            auto Divisor = FrameShift_ ? FrameShift_ : FrameSize_;
std::cout << "zero_padding_required - Divisor = " << Divisor << std::endl;
            auto Remainder = ( SampleSize - FrameSize_ ) % Divisor;
std::cout << "zero_padding_required - Remainder = " << Remainder << std::endl;
            if( Remainder < FrameShift_ )
            {
                PaddingSamples = FrameShift_ - Remainder;
            }
            else if( Remainder > FrameShift_ )
            {
                PaddingSamples = FrameShift_ - Remainder;
            }
        }
        else
        {
            PaddingSamples = FrameSize_ - SampleSize;
        }
std::cout << "zero_padding_required - PaddingSamples = " << PaddingSamples << std::endl;
        return bytes_from_samples( PaddingSamples, Buffer_.format() );
    }


private:

    std::size_t                 HeaderSize_;
    std::vector<char>           Data_;
    std::size_t                 FrameSize_;
    std::size_t                 FrameShift_;
    buffer_t                    Buffer_;
};

// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
} // audio
} // qt
// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n


// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G
#endif // QT_AUDIO_WAV_DATA_HPP_INCLUDED
