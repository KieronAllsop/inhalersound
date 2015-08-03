// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G
#ifndef QT_AUDIO_AUDIO_DECODER_HPP_INCLUDED
#define QT_AUDIO_AUDIO_DECODER_HPP_INCLUDED
// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G

// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I

// qt::audio Includes
#include "qt/audio/decode_status.hpp"
#include "qt/audio/decode_buffer.hpp"
#include "qt/audio/wave_decoder.hpp"

// Qt Includes
#include <QtMultimedia/QAudioDecoder>
#include <QtMultimedia/QAudioBuffer>
#include <QString>

// Boost Includes
#include <boost/filesystem.hpp>

// Standard Library Includes
#include <functional>
#include <memory>
#include <fstream>
#include <iostream>

// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I

// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
namespace qt {
namespace audio {
// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n

class audio_decoder
{
public:

    using status_t          = decode_status_t;
    using buffer_t          = decode_buffer;
    using path_t            = boost::filesystem::path;
    using buffer_handler_t  = std::function< void( status_t Status, const buffer_t& Buffer ) >;

public:

    // Make this class non-copyable
    audio_decoder( const audio_decoder& other ) = delete;
    audio_decoder& operator=( const audio_decoder& other ) = delete;

    explicit audio_decoder( const path_t& Path, const buffer_handler_t& Handler )
    : Path_( Path )
    , Handler_( Handler )
    {
        QObject::connect
            (   &Decoder_, &QAudioDecoder::bufferReady,
                [this](){ on_audio_decoder_buffer(); }   );

        QObject::connect
            (   &Decoder_, &QAudioDecoder::finished,
                [this](){ on_audio_decoder_finished(); }   );

        QObject::connect
            (   &Decoder_, static_cast<void (QAudioDecoder::*)(QAudioDecoder::Error)>(&QAudioDecoder::error),
                [this]( QAudioDecoder::Error Error ){ on_audio_decoder_error( Error ); }   );
    }

    virtual ~audio_decoder()
    {
        Decoder_.stop();
    }

    void start()
    {
        Decoder_.setSourceFilename( QString::fromUtf8( Path_.c_str() ) );
        Decoder_.start();
    }

private:

    void on_audio_decoder_buffer()
    {
        QAudioBuffer AudioBuffer = Decoder_.read();
        if( AudioBuffer.isValid() )
        {
            auto Format = AudioBuffer.format();

            auto SampleType = sample_type_t::unknown;

            switch( Format.sampleType() )
            {
                case QAudioFormat::SignedInt:   SampleType = sample_type_t::signed_int; break;
                case QAudioFormat::UnSignedInt: SampleType = sample_type_t::unsigned_int; break;
                case QAudioFormat::Float:       SampleType = sample_type_t::floating_point; break;
                default :                       SampleType = sample_type_t::unknown; break;
            }

            buffer_t Buffer
                    (   Format.sampleSize(),
                        SampleType,
                        Format.sampleRate(),
                        Format.channelCount(),
                        AudioBuffer.byteCount() / Format.bytesPerFrame(),
                        Format.codec().toStdString(),
                        AudioBuffer.constData(),
                        AudioBuffer.byteCount()  );

            Handler_( status_t::buffer_ready, Buffer );
        }
        else
        {
            Handler_( status_t::invalid_buffer, buffer_t() );
        }
    }

    void on_audio_decoder_finished()
    {
        Handler_( status_t::finished, buffer_t() );
    }

    void on_audio_decoder_error( QAudioDecoder::Error Error )
    {
        if( Error == QAudioDecoder::ResourceError )
        {
            std::cout << "Can't open file using QAudioDecoder, try wave_decoder instead" << std::endl;
            Decoder_.stop();

            WaveDecoder_ = std::make_shared<wave_decoder>( Path_, Handler_ );
            WaveDecoder_->start();
        }
        else
        {
            Decoder_.stop();
            Handler_( status_t::error, buffer_t() );
        }
    }

private:

    QAudioDecoder                   Decoder_;
    std::shared_ptr<wave_decoder>   WaveDecoder_;
    path_t                          Path_;
    buffer_handler_t                Handler_;
};



// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
} // audio
} // qt
// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n


// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G
#endif // QT_AUDIO_AUDIO_DECODER_HPP_INCLUDED
