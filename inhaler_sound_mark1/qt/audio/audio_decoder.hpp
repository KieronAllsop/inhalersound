// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G
#ifndef QT_AUDIO_AUDIO_DECODER_HPP_INCLUDED
#define QT_AUDIO_AUDIO_DECODER_HPP_INCLUDED
// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G

// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I

// qt::audio Includes
#include "qt/audio/decode_status.hpp"
#include "qt/audio/audio_buffer.hpp"
#include "qt/audio/wave_decoder.hpp"

// Qt Includes
#include <QtMultimedia/QAudioDecoder>
#include <QtMultimedia/QAudioBuffer>
#include <QString>

// Boost Includes
#include <boost/filesystem.hpp>

// C++ Standard Library Includes
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
    using buffer_t          = audio_buffer;
    using format_t          = buffer_t::format_t;
    using path_t            = boost::filesystem::path;
    using buffer_handler_t  = std::function< void( status_t Status, const buffer_t& Buffer ) >;

public:

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
            (   &Decoder_, &QAudioDecoder::stateChanged,
                [this]( QAudioDecoder::State State ){ on_audio_decoder_state_changed( State ); }   );

        QObject::connect
            (   &Decoder_, &QAudioDecoder::formatChanged,
                [this]( const QAudioFormat& Format ){ on_audio_decoder_format_changed( Format ); }   );

        QObject::connect
            (   &Decoder_, static_cast<void (QAudioDecoder::*)(QAudioDecoder::Error)>(&QAudioDecoder::error),
                [this]( QAudioDecoder::Error Error ){ on_audio_decoder_error( Error ); }   );
    }

    virtual ~audio_decoder()
    {
        stop();
    }

    // Observers

    const path_t& path() const
    {
        return Path_;
    }

    // Modifiers

    void start()
    {
        stop();
        Decoder_.setSourceFilename( QString::fromUtf8( Path_.c_str() ) );
        Decoder_.start();
    }

    void stop()
    {
        if( WaveDecoder_ )
        {
            WaveDecoder_->stop();
        }
        else
        {
            bool Decoding = Decoding_;
            if( Decoding_ )
            {
                Decoder_.stop();
                if( Decoding )
                {
                    Handler_( status_t::operation_aborted, buffer_t() );
                }
            }
        }
    }

private:

    static format_t format_from( const QAudioFormat& Format )
    {
        auto SampleType = sample_type_t::unknown;

        switch( Format.sampleType() )
        {
            case QAudioFormat::SignedInt:
            {
                switch( Format.sampleSize() )
                {
                    case 16 : SampleType = sample_type_t::signed_int16; break;
                    case 32 : SampleType = sample_type_t::signed_int32; break;
                    default :
                    {
                        SampleType = sample_type_t::unknown;
                    }
                }
                break;
            }
            case QAudioFormat::UnSignedInt:
            {
                if( Format.sampleSize() == 8 )
                {
                    SampleType = sample_type_t::unsigned_int8;
                }
                else
                {
                    SampleType = sample_type_t::unknown;
                }
                break;
            }
            case QAudioFormat::Float:
            {
                SampleType = sample_type_t::floating_point;
                break;
            }
            default :
            {
                SampleType = sample_type_t::unknown;
                break;
            }
        }

        return
            format_t
                (   SampleType,
                    Format.sampleRate(),
                    Format.channelCount(),
                    Format.codec().toStdString()   );
    }


    void on_audio_decoder_buffer()
    {
        QAudioBuffer AudioBuffer = Decoder_.read();
        if( AudioBuffer.isValid() )
        {
            auto Format = format_from( AudioBuffer.format() );

            buffer_t Buffer
                    (   Format,
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

    void on_audio_decoder_state_changed( QAudioDecoder::State State )
    {
        Decoding_ = State == QAudioDecoder::DecodingState;
        if( Decoding_ )
        {
            Handler_( status_t::started, buffer_t() );
        }
//         else
//         {
//             std::cout << "Decoding Stopped" << std::endl;
//         }
    }

    void on_audio_decoder_format_changed( const QAudioFormat& Format )
    {
        auto DecodeFormat = format_from( Decoder_.audioFormat() );
        Handler_( status_t::status, buffer_t( DecodeFormat, nullptr, 0 ) );
    }

    void on_audio_decoder_error( QAudioDecoder::Error Error )
    {
        if( Error == QAudioDecoder::ResourceError )
        {
            std::cout << "QAudioDecoder Resource Error = " << Error << ": " << Decoder_.errorString().toStdString() << std::endl;
            Decoder_.stop();

            WaveDecoder_
                = std::make_shared<wave_decoder>
                    (   Path_,
                        [this]( status_t Status, const buffer_t& Buffer )
                        {
                            wave_decoder_handler( Status, Buffer );
                        }
                    );

            WaveDecoder_->start();
        }
        else
        {
            std::cout << "QAudioDecoder Other Error = " << Error << ": " << Decoder_.errorString().toStdString() << std::endl;
            Decoder_.stop();
            auto Status = status_t::operation_aborted;
            switch( Error )
            {
                case QAudioDecoder::NoError             : Status = status_t::operation_aborted;    break;
                case QAudioDecoder::FormatError         : Status = status_t::format_not_supported; break;
                case QAudioDecoder::ResourceError       : Status = status_t::resource_error;       break;
                case QAudioDecoder::AccessDeniedError   : Status = status_t::access_denied;        break;
                case QAudioDecoder::ServiceMissingError : Status = status_t::service_missing;      break;
            }
            Handler_( Status, buffer_t() );
        }
    }

    void wave_decoder_handler( status_t Status, const buffer_t& Buffer )
    {
        if( Status != status_t::wrong_decoder_for_stream )
        {
            Handler_( Status, Buffer );
        }
        else
        {
            WaveDecoder_.reset();
            Handler_( status_t::resource_error, buffer_t() );
        }
    }

private:

    QAudioDecoder                   Decoder_;
    std::shared_ptr<wave_decoder>   WaveDecoder_;
    bool                            Decoding_;
    path_t                          Path_;
    buffer_handler_t                Handler_;
};



// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
} // audio
} // qt
// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n


// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G
#endif // QT_AUDIO_AUDIO_DECODER_HPP_INCLUDED
