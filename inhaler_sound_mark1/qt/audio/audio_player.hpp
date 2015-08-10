// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G
#ifndef QT_AUDIO_AUDIO_PLAYER_HPP_INCLUDED
#define QT_AUDIO_AUDIO_PLAYER_HPP_INCLUDED
// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G

// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I

// qt::audio Includes
#include "qt/audio/play_status.hpp"
#include "qt/audio/probe_status.hpp"
#include "qt/audio/audio_buffer.hpp"

// Qt Includes
#include <QtMultimedia/QMediaPlayer>
#include <QtMultimedia/QAudioProbe>
#include <QtMultimedia/QAudioBuffer>

// Boost Includes
#include <boost/filesystem.hpp>

// C++ Standard Library Includes
#include <functional>
#include <memory>

// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I

// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
namespace qt {
namespace audio {
// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n

class audio_player
{
public:

    using buffer_t          = audio_buffer;
    using format_t          = buffer_t::format_t;
    using path_t            = boost::filesystem::path;
    using milliseconds_t    = std::chrono::milliseconds;
    using nanoseconds_t     = std::chrono::nanoseconds;
    using play_handler_t    = std::function< void( play_status_t Status, const milliseconds_t& Position ) >;
    using buffer_handler_t  = std::function< void( probe_status_t Status, const buffer_t& Buffer ) >;

    using play_status_t     = qt::audio::play_status_t;
    using probe_status_t    = qt::audio::probe_status_t;

public:

    audio_player( const audio_player& other ) = delete;
    audio_player& operator=( const audio_player& other ) = delete;

    explicit audio_player
        (   const path_t& Path,
            const nanoseconds_t& Duration,
            const play_handler_t& PlayHandler,
            const buffer_handler_t& ProbeHandler   )
    : Path_( Path )
    , Duration_( Duration )
    , PlayHandler_( PlayHandler )
    , ProbeHandler_( ProbeHandler )
    {
        QObject::connect
            (   &Player_, &QMediaPlayer::positionChanged,
                [this]( qint64 Milliseconds ){ handle_player_position_changed( Milliseconds ); }   );

        QObject::connect
            (   &Player_, &QMediaPlayer::stateChanged,
                [this]( QMediaPlayer::State State ){ handle_player_state_changed( State ); }   );

        Player_.setMedia( QUrl::fromLocalFile( QString::fromUtf8( Path_.c_str() ) ) );
        set_notification_interval( std::chrono::duration_cast<milliseconds_t>( Duration ) );

        if( ProbeHandler_ )
        {
            QObject::connect
                (   &Probe_, &QAudioProbe::audioBufferProbed,
                    [this]( const QAudioBuffer& Buffer ){ handle_audio_buffer( Buffer ); }   );

            if( !Probe_.setSource( &Player_ ) )
            {
                ProbeHandler_( probe_status_t::monitoring_not_supported, buffer_t() );
            }
        }
    }

    virtual ~audio_player()
    {
        stop();
    }

    // Observers

    const path_t& path() const
    {
        return Path_;
    }

    const nanoseconds_t& duration() const
    {
        return Duration_;
    }

    // Modifiers

    void play_pause()
    {
        if( Player_.state() != QMediaPlayer::PlayingState )
        {
            Player_.play();
        }
        else
        {
            Player_.pause();
        }
    }

    void stop()
    {
        if( Player_.state() == QMediaPlayer::PlayingState )
        {
            Probe_.setSource( static_cast<QMediaObject*>( nullptr ) );
            ProbeHandler_( probe_status_t::operation_aborted, buffer_t() );
        }
        Player_.stop();
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


    void set_notification_interval( const milliseconds_t& Duration )
    {
        auto Interval = Duration.count() / 1000;
        if( Interval > 100 )
        {
            Interval = 107;
        }
        else if( Interval < 50 )
        {
            Interval = 57;
        }
        Player_.setNotifyInterval( Interval );
    }


    void handle_audio_buffer( const QAudioBuffer& AudioBuffer )
    {
        if( AudioBuffer.isValid() )
        {
            auto Format = format_from( AudioBuffer.format() );

            buffer_t Buffer( Format, AudioBuffer.constData(), AudioBuffer.byteCount()  );

            ProbeHandler_( probe_status_t::buffer_ready, Buffer );
        }
        else
        {
            ProbeHandler_( probe_status_t::invalid_buffer, buffer_t() );
        }
    }

    void handle_player_position_changed( qint64 Milliseconds )
    {
        if( Player_.state() != QMediaPlayer::StoppedState )
        {
            PlayHandler_( play_status_t::position_changed, milliseconds_t( Milliseconds ) );
        }
    }

    void handle_player_state_changed( QMediaPlayer::State State )
    {
        if( State == QMediaPlayer::StoppedState )
        {
            PlayHandler_( play_status_t::stopped, milliseconds_t(0) );
        }
        else if( State == QMediaPlayer::PlayingState )
        {
            PlayHandler_( play_status_t::playing, milliseconds_t( Player_.position() ) );
        }
        else if( State == QMediaPlayer::PausedState )
        {
            PlayHandler_( play_status_t::paused, milliseconds_t( Player_.position() ) );
        }
    }

private:

    QMediaPlayer            Player_;
    QAudioProbe             Probe_;
    path_t                  Path_;
    nanoseconds_t           Duration_;
    play_handler_t          PlayHandler_;
    buffer_handler_t        ProbeHandler_;
};



// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
} // audio
} // qt
// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n


// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G
#endif // QT_AUDIO_AUDIO_PLAYER_HPP_INCLUDED
