// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G
#ifndef QT_AUDIO_WAVE_DECODER_HPP_INCLUDED
#define QT_AUDIO_WAVE_DECODER_HPP_INCLUDED
// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G

// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I

// qt::audio Includes
#include "qt/audio/decode_status.hpp"
#include "qt/audio/audio_buffer.hpp"

// Qt Includes
#include <QEvent>
#include <QObject>
#include <QCoreApplication>

// Boost Includes
#include <boost/filesystem.hpp>

// Standard Library Includes
#include <functional>
#include <string>
#include <cstring>
#include <atomic>
#include <thread>
#include <fstream>
#include <iostream>

// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I

// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
namespace qt {
namespace audio {
// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n


class buffer_event : public QEvent
{
public:

    using status_t = decode_status_t;
    using buffer_t = audio_buffer;

public:

    static QEvent::Type type()
    {
        static auto Type = QEvent::Type( QEvent::registerEventType() );
        return Type;
    }

public:

    buffer_event( std::vector<char>&& Data, status_t Status, const buffer_t& Buffer )
    : QEvent( type() )
    , Data_     ( std::move( Data ) )
    , Status_   ( Status )
    , Buffer_   ( Buffer )
    {
    }

    status_t status() const
    {
        return Status_;
    }

    buffer_t buffer() const
    {
        return Buffer_;
    }

private:

    std::vector<char>   Data_;
    status_t            Status_;
    buffer_t            Buffer_;
};



class event_sink : public QObject
{
public:

    using status_t          = decode_status_t;
    using buffer_t          = audio_buffer;
    using buffer_handler_t  = std::function< void( status_t Status, const buffer_t& Buffer ) >;

public:

    event_sink( const buffer_handler_t& Handler )
    : QObject()
    , Handler_( Handler )
    {
    }

    bool event( QEvent* Event )
    {
        if( Event->type() == buffer_event::type() )
        {
            if( auto* BufferEvent = dynamic_cast<buffer_event*>( Event ) )
            {
                Handler_( BufferEvent->status(), BufferEvent->buffer() );
                return true;
            }
        }
        return QObject::event( Event );
    }
private:

    buffer_handler_t Handler_;
};



class wave_decoder
{
public:

    using status_t          = decode_status_t;
    using buffer_t          = audio_buffer;
    using format_t          = buffer_t::format_t;
    using path_t            = boost::filesystem::path;
    using buffer_handler_t  = std::function< void( status_t Status, const buffer_t& Buffer ) >;

public:

    wave_decoder( const wave_decoder& other ) = delete;
    wave_decoder& operator=( const wave_decoder& other ) = delete;

    explicit wave_decoder( const path_t& Path, const buffer_handler_t& Handler )
    : Path_( Path )
    , EventSink_( Handler )
    , Finished_( false )
    {
    }

    void start()
    {
        stop();
        std::cout << "Start wave_decoder" << std::endl;
        Thread_ = std::thread
            (   [this]()
                {
                    read_wav_file( Path_ );
                }
            );
    }

    void stop()
    {
        Finished_ = true;
        if( Thread_.joinable() )
        {
            Thread_.join();
        }
        Finished_ = false;
    }

    ~wave_decoder()
    {
        stop();


    }

private:

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
        int16_t format_tag;         // 0x0001->PCM, ...
        int16_t channel_count;      // Number of channels
        int32_t sample_rate;        // Sampling rate
        int32_t bytes_per_sec;      // Bytes per second
        int16_t bytes_per_sample;   // Bytes per sample
        int16_t bits_per_sample;    // Bits per sample
    };

    struct format_tag
    {
        enum enum_t
        {
            pcm = 1
        };
    };


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


    format_t read_wav_header( std::ifstream& Stream )
    {
        riff_header Riff;
        Stream.read( reinterpret_cast<char*>( &Riff ), sizeof(Riff) );
        if( std::strncmp( Riff.id, riff(), sizeof(Riff.id) ) != 0 )
        {
            QCoreApplication::postEvent( &EventSink_, new buffer_event( std::vector<char>(), status_t::wrong_decoder_for_stream, buffer_t() ) );
            return format_t();
        }

        wave_header Wave;
        Stream.read( reinterpret_cast<char*>( &Wave ), sizeof(Wave) );
        if( std::strncmp( Wave.id, wave(), sizeof(Wave.id) ) != 0 )
        {
            QCoreApplication::postEvent( &EventSink_, new buffer_event( std::vector<char>(), status_t::wrong_decoder_for_stream, buffer_t() ) );
            return format_t();
        }

        chunk_header FmtChunk;
        Stream.read( reinterpret_cast<char*>( &FmtChunk ), sizeof(FmtChunk) );
        if( std::strncmp( FmtChunk.id, fmt(), sizeof(FmtChunk.id) ) != 0 )
        {
            QCoreApplication::postEvent( &EventSink_, new buffer_event( std::vector<char>(), status_t::format_not_supported, buffer_t() ) );
            return format_t();
        }

        format_chunk FormatChunk;
        Stream.read( reinterpret_cast<char*>( &FormatChunk ), sizeof(FormatChunk) );
        if( FormatChunk.format_tag != format_tag::pcm )
        {
            QCoreApplication::postEvent( &EventSink_, new buffer_event( std::vector<char>(), status_t::format_not_supported, buffer_t() ) );
            return format_t();
        }

        // Skip any remaning bytes for this chunk
        FmtChunk.length -= sizeof(format_chunk);
        if( FmtChunk.length )
        {
            Stream.seekg( FmtChunk.length, std::ios::cur );
        }

        return format_from( FormatChunk );
    }


    std::size_t read_wav_data_length( std::ifstream& Stream )
    {
        chunk_header DataHeader;
        Stream.read( reinterpret_cast<char*>( &DataHeader ), sizeof(DataHeader) );

        while( std::strncmp( DataHeader.id, data(), sizeof(DataHeader.id) ) != 0 )
        {
            Stream.read( reinterpret_cast<char*>( &DataHeader ), sizeof(DataHeader) );
        }
        return DataHeader.length;
    }


    void read_wav_data( std::ifstream& Stream, std::size_t Length, const format& Format )
    {
        // Calculate an appropriate buffer length based on the sample-rate
        // and how much time we want to hold in the buffer

        auto BytesPerSample = Format.sample_byte_size();
        auto ChannelCount   = Format.channel_count();
        auto SampleRate     = Format.sample_rate();

        int BufferSizeMilliseconds = 25;

        std::size_t BufferSize = ( SampleRate / 1000 ) * BufferSizeMilliseconds * ChannelCount * BytesPerSample;

        std::size_t DataLength = Length;

        while( !Finished_ && DataLength )
        {
            std::size_t Size = BufferSize < DataLength ? BufferSize : DataLength;

            std::vector<char> Data( Size );
            Stream.read( &Data[0], Size );

            auto SamplesPerChannel = Size / ChannelCount / BytesPerSample;
            auto Duration = std::chrono::nanoseconds( SamplesPerChannel ) * 1'000'000'000 / SampleRate;

            buffer_t
                Buffer
                    (   Format,
                        SamplesPerChannel,
                        Duration,
                        static_cast<const void*>( &Data[0] ),
                        Size   );

            QCoreApplication::postEvent( &EventSink_, new buffer_event( std::move( Data ), status_t::buffer_ready, Buffer ) );

            DataLength -= Size;
        }
        if( DataLength == 0 )
        {
            QCoreApplication::postEvent( &EventSink_, new buffer_event( std::vector<char>(), status_t::finished, buffer_t() ) );
        }
        else
        {
            QCoreApplication::postEvent( &EventSink_, new buffer_event( std::vector<char>(), status_t::operation_aborted, buffer_t() ) );
        }
    }


    void read_wav_file( const path_t& Path )
    {
        std::ifstream WavFile( Path.c_str(), std::ios::binary );
        if( !WavFile )
        {
            QCoreApplication::postEvent( &EventSink_, new buffer_event( std::vector<char>(), status_t::resource_error, buffer_t() ) );
            return;
        }

        if( auto Format = read_wav_header( WavFile ) )
        {
            auto Length = read_wav_data_length( WavFile );

            auto SampleLength = Length / Format.channel_count() / Format.sample_byte_size();
            auto Duration = std::chrono::nanoseconds( SampleLength ) * 1'000'000'000 / Format.sample_rate();

            QCoreApplication::postEvent
                (   &EventSink_,
                    new buffer_event
                        (   std::vector<char>(),
                            status_t::started,
                            buffer_t( Format, SampleLength, Duration, nullptr, 0 )   )   );

            read_wav_data( WavFile, Length, Format );
        }
    }

private:

    path_t              Path_;
    event_sink          EventSink_;
    std::atomic<bool>   Finished_;
    std::thread         Thread_;
};


// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
} // audio
} // qt
// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n


// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G
#endif // QT_AUDIO_WAVE_DECODER_HPP_INCLUDED
