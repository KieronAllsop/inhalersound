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
#include <vector>
#include <string>
#include <cstring>
#include <chrono>

// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I

// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
namespace qt {
namespace audio {
// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n


//! \class  raw_data.hpp
//! \author Kieron Allsop
//!
//! \brief  To store the WAV file data that has been chosen by the user to view.
//!
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

    raw_data
    (   const path_t& LinkedFile,
        bool RemoveOnDestruct   )
    : Size_( 0 )
    , FrameSize_( 0 )
    , FrameShift_( 0 )
    , LinkedFile_       ( LinkedFile )
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
            static_cast<const void*>( &Data_[0] ),
            Data_.size()   )
    , Size_
        ( Data_.size() )
    , FrameSize_( 0 )
    , FrameShift_( 0 )
    , RemoveLinkedFile_( false )
    {
    }


    raw_data()
    : RemoveLinkedFile_( false )
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

        // Ignore zero padding
        Data_.resize( Size_ + Buffer.size() );
        std::memcpy( static_cast<void*>( &Data_[Size_] ), Buffer.data(), Buffer.size() );
        Size_ = Data_.size();
        // Now add zero padding
        auto ZeroPadSize = zero_padding_required( Size_ );
        Data_.resize( Size_+ZeroPadSize, 0 );

        Buffer_ = buffer_t( Buffer.format(), static_cast<const void*>( &Data_[0] ), Data_.size() );
    }


    void zero_pad( std::size_t FrameSize, std::size_t FrameShift )
    {
        if( FrameSize != FrameSize_ && FrameShift != FrameShift_ )
        {
            auto ZeroPadSize = zero_padding_required( Size_ );
            Data_.resize( Size_+ZeroPadSize, 0 );
            Buffer_ = buffer_t( Buffer_.format(), static_cast<const void*>( &Data_[0] ), Data_.size() );
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


    const path_t& linked_file() const
    {
        return LinkedFile_;
    }


    const buffer_t& buffer() const
    {
        return Buffer_;
    }


    const void* data() const
    {
        return static_cast<const void*>( &Data_[0] );
    }


    std::size_t size() const
    {
        return Size_;
    }


    std::size_t zero_padded_size() const
    {
        return Buffer_.size();
    }


    template<class T>
    T scaled_sample( std::size_t Index, std::size_t Channel ) const
    {
        return Buffer_.scaled_sample<T>( Index, Channel );
    }

private:

    std::size_t zero_padding_required( std::size_t DataSize )
    {
        if( FrameSize_ )
        {
            auto RemainderFrames = ( DataSize - FrameSize_ ) % FrameShift_;
            if( RemainderFrames < FrameShift_ )
            {
                return FrameShift_ - RemainderFrames;
            }
            else if( RemainderFrames > FrameShift_ )
            {
                return FrameShift_ - RemainderFrames;
            }
        }
        return 0;
    }

private:

    std::vector<char>           Data_;
    buffer_t                    Buffer_;
    std::size_t                 Size_;
    std::size_t                 FrameSize_;
    std::size_t                 FrameShift_;
    path_t                      LinkedFile_;
    bool                        RemoveLinkedFile_;

};


// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
} // audio
} // qt
// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n

// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G
#endif // QT_AUDIO_RAW_DATA_HPP_INCLUDED
