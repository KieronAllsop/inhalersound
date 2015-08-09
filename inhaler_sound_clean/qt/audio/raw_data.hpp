// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G
#ifndef QT_AUDIO_RAW_DATA_HPP_INCLUDED
#define QT_AUDIO_RAW_DATA_HPP_INCLUDED
// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G

// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I

// qt::audio Includes
#include "qt/audio/audio_buffer.hpp"

// Boost Includes
#include <boost/exception/all.hpp>
#include <boost/filesystem.hpp>

// Standard Library Includes
#include <stdexcept>
#include <vector>
#include <string>
#include <cstring>
#include <chrono>
#include <iostream>

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

    using path_t        = boost::filesystem::path;
    using format_t      = qt::audio::format;
    using duration_t    = std::chrono::nanoseconds;
    using buffer_t      = audio_buffer;

public:

    raw_data( const raw_data& other ) = delete;
    raw_data& operator=( const raw_data& other ) = delete;

    // adds data from
    raw_data
    (   const path_t& LinkedFile,
        bool RemoveOnDestruct   )
    : LinkedFile_       ( LinkedFile )
    , RemoveLinkedFile_ ( RemoveOnDestruct )
    {
    }

    raw_data
    (   const format& Format,
        std::vector<char>&& Data   )
    : Data_
        ( Data )
    , Buffer_
        (   Format,
            Data.size() / Format.sample_byte_size() / Format.channel_count(),
            std::chrono::nanoseconds( Data.size() / Format.sample_byte_size() / Format.channel_count() ) * 1'000'000'000 / Format.sample_rate(),
            static_cast<const void*>( &Data_[0] ),
            Data_.size()   )
    , RemoveLinkedFile_
        ( false )
    {
    }

    raw_data()
    : RemoveLinkedFile_             ( false )
    {
    }

    ~raw_data()
    {
        if( !LinkedFile_.empty() && RemoveLinkedFile_ )
        {
            if( exists( LinkedFile_ ) )
            {
                remove( LinkedFile_ );
            }
        }
    }

    // Modifiers
    void add_buffer( const buffer_t& Buffer )
    {
        if(     Buffer_.format().sample_type() != sample_type_t::unknown
            &&  Buffer_.format() != Buffer.format() )
        {
            BOOST_THROW_EXCEPTION( exception::incorrect_format() );
        }

        auto SamplesPerChannel = Buffer_.samples_per_channel() + Buffer.samples_per_channel();
        auto Duration = Buffer_.duration() + std::chrono::nanoseconds( Buffer.samples_per_channel() ) * 1'000'000'000 / Buffer.format().sample_rate();

        std::size_t Size = Data_.size();
        Data_.resize( Data_.size() + Buffer.size() );
        std::memcpy( static_cast<void*>( &Data_[Size] ), Buffer.data(), Buffer.size() );

        std::cout << "Assign new buffer" << std::endl;
        Buffer_ = buffer_t( Buffer.format(), SamplesPerChannel, Duration, static_cast<const void*>( &Data_[0] ), Data_.size() );
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

    const path_t& linked_file() const
    {
        return LinkedFile_;
    }

    const void* data() const
    {
        return Buffer_.data();
    }

    std::size_t size() const
    {
        return Buffer_.size();
    }

    template<class T>
    const T& sample( std::size_t Index, std::size_t Channel ) const
    {
        return Buffer_.sample<T>( Index, Channel );
    }

    double normalised_sample( std::size_t Index, std::size_t Channel ) const
    {
        return Buffer_.normalised_sample( Index, Channel );
    }

private:

    // vector of chars to represent the bytes of the file - raw data basically
    std::vector<char>           Data_;
    buffer_t                    Buffer_;  // stores format and access to data
    path_t                      LinkedFile_;
    bool                        RemoveLinkedFile_;
};

// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
} // audio
} // qt
// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n


// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G
#endif // QT_AUDIO_RAW_DATA_HPP_INCLUDED
