// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G
#ifndef QT_AUDIO_WAVE_DECODER_HPP_INCLUDED
#define QT_AUDIO_WAVE_DECODER_HPP_INCLUDED
// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G

// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I

// qt::audio Includes
#include "qt/audio/decode_status.hpp"
#include "qt/audio/decode_buffer.hpp"

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
    using buffer_t = decode_buffer;

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
    using buffer_t          = decode_buffer;
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
    using buffer_t          = decode_buffer;
    using path_t            = boost::filesystem::path;
    using buffer_handler_t  = std::function< void( status_t Status, const buffer_t& Buffer ) >;

public:

    wave_decoder( const wave_decoder& other ) = delete;
    wave_decoder& operator=( const wave_decoder& other ) = delete;

    explicit wave_decoder( const path_t& Path, const buffer_handler_t& Handler )
    : Path_( Path )
    , EventSink_( Handler )
    , Finished_( false )
    , Codec_( "audio/pcm" )
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
        return "fmt";
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


    void read_wav_file( const path_t& Path )
    {
        std::cout << "wave_decoder: open file " << Path.c_str() << std::endl;

        std::ifstream WavFile( Path.c_str(), std::ios::binary );
        if( !WavFile )
        {
            QCoreApplication::postEvent( &EventSink_, new buffer_event( std::vector<char>(), status_t::error, buffer_t() ) );
        }

        riff_header Riff;
        WavFile.read( reinterpret_cast<char*>( &Riff ), sizeof(Riff) );
        if( std::strncmp( Riff.id, riff(), sizeof(Riff.id) ) != 0 )
        {
            QCoreApplication::postEvent( &EventSink_, new buffer_event( std::vector<char>(), status_t::format_not_supported, buffer_t() ) );
        }

        wave_header Wave;
        WavFile.read( reinterpret_cast<char*>( &Wave ), sizeof(Wave) );
        if( std::strncmp( Wave.id, wave(), sizeof(Wave.id) ) != 0 )
        {
            QCoreApplication::postEvent( &EventSink_, new buffer_event( std::vector<char>(), status_t::format_not_supported, buffer_t() ) );
        }

        chunk_header FmtChunk;
        WavFile.read( reinterpret_cast<char*>( &FmtChunk ), sizeof(FmtChunk) );
        if( std::strcmp( FmtChunk.id, fmt() ) != 0 )
        {
            QCoreApplication::postEvent( &EventSink_, new buffer_event( std::vector<char>(), status_t::format_not_supported, buffer_t() ) );
        }

        format_chunk FormatChunk;
        WavFile.read( reinterpret_cast<char*>( &FormatChunk ), sizeof(FormatChunk) );
        if( FormatChunk.format_tag != format_tag::pcm )
        {
            QCoreApplication::postEvent( &EventSink_, new buffer_event( std::vector<char>(), status_t::format_not_supported, buffer_t() ) );
        }

        // Skip any remaning bytes for this chunk
        FmtChunk.length -= sizeof(format_chunk);
        if( FmtChunk.length )
        {
            WavFile.seekg( FmtChunk.length, std::ios::cur );
        }

        std::cout << "wave_decoder: successfully read header" << std::endl;

        int             SampleSize          = FormatChunk.bits_per_sample;
        sample_type_t   SampleType          = sample_type_t::unknown;
        int             SampleRate          = FormatChunk.sample_rate;
        int             ChannelCount        = FormatChunk.channel_count;
        int             BytesPerSample      = FormatChunk.bytes_per_sample;

        if( SampleSize == 8 )
        {
            SampleType = sample_type_t::unsigned_int;
        }
        else
        {
            SampleType = sample_type_t::signed_int;
        }

        std::cout
            << "Wav Format " << SampleRate << " Hz in "
            << ChannelCount << " channels ("
            << c_str( SampleType ) << " " << SampleSize
            << " bits per sample, codec: " << Codec_
            << ")" << std::endl;

        chunk_header DataHeader;
        WavFile.read( reinterpret_cast<char*>( &DataHeader ), sizeof(DataHeader) );

        while( std::strncmp( DataHeader.id, data(), sizeof(DataHeader.id) ) != 0 )
        {
            WavFile.read( reinterpret_cast<char*>( &DataHeader ), sizeof(DataHeader) );
        }

        // Calculate an appropriate buffer length based on the sample-rate
        // and how much time we want to hold in the buffer
        int BufferSizeMilliseconds = 25;
        std::size_t BufferSize = ( SampleRate / 1000 ) * BufferSizeMilliseconds * ChannelCount * BytesPerSample;

        int SamplesPerChannel = BufferSize / ChannelCount / BytesPerSample;

        std::size_t DataLength = DataHeader.length;

        std::cout << "wave_decoder: BufferSize = " << BufferSize << std::endl;
        std::cout << "wave_decoder: DataLength = " << DataLength << std::endl;

        while( !Finished_ && DataLength )
        {
            std::size_t Size = BufferSize < DataLength ? BufferSize : DataLength;

            std::vector<char> Data;
            Data.reserve( Size );
            Data.assign
                (   std::istreambuf_iterator<char>( WavFile ),
                    std::istreambuf_iterator<char>()   );

            buffer_t
                Buffer
                    ( SampleSize,
                        SampleType,
                        SampleRate,
                        ChannelCount,
                        Size == BufferSize ? SamplesPerChannel : Size/ChannelCount/BytesPerSample,
                        Codec_,
                        static_cast<const void*>( &Data[0] ),
                        Size );

            QCoreApplication::postEvent( &EventSink_, new buffer_event( std::move( Data ), status_t::buffer_ready, Buffer ) );

            DataLength -= Size;
        }

        QCoreApplication::postEvent( &EventSink_, new buffer_event( std::vector<char>(), status_t::finished, buffer_t() ) );
    }

private:

    path_t              Path_;
    event_sink          EventSink_;
    std::atomic<bool>   Finished_;
    std::thread         Thread_;
    std::string         Codec_;
};


// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
} // audio
} // qt
// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n


// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G
#endif // QT_AUDIO_WAVE_DECODER_HPP_INCLUDED
