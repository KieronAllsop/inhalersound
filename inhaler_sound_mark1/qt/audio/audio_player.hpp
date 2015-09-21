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

// Standard Library Includes
#include <functional>
#include <memory>

// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I

// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
namespace qt {
namespace audio {
// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n


//! \class  audio_player.hpp
//! \author Kieron Allsop
//!
//! \brief  To enable playing of a WAV file which has been imported into
//!         the system.
//!
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
    , Start_( nanoseconds_t( 0 ) )
    , End_( Duration )
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
        if( Player_.state() == QMediaPlayer::PlayingState )
        {
            Probe_.setSource( static_cast<QMediaObject*>( nullptr ) );
        }
        stop();
    }


    // Observers -------------------------------------------------------------
    const path_t& path() const
    {
        return Path_;
    }

    const nanoseconds_t& duration() const
    {
        return Duration_;
    }


    // Modifiers -------------------------------------------------------------
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
            ProbeHandler_( probe_status_t::operation_aborted, buffer_t() );
        }
        Player_.stop();
    }


    void mute_on()
    {
        Player_.setMuted( true );
    }


    void mute_off()
    {
        Player_.setMuted( false );
    }


    void reset_player()
    {
        mute_on();
        play_pause();
        stop();
        mute_off();
    }


    bool set_play_selection( const nanoseconds_t& Start, const nanoseconds_t& End )
    {
        if( Start >= End || Start >= Duration_ )
        {
            return false;
        }

        if( Start < nanoseconds_t(0) )
        {
            Start_ = nanoseconds_t(0);
        }
        else
        {
            Start_ = std::chrono::duration_cast<milliseconds_t>( Start );
        }

        if( End > Duration_ )
        {
            End_ = Duration_;
        }
        else
        {
            End_ = std::chrono::duration_cast<milliseconds_t>( End );
        }

        Player_.stop();
        return true;
    }


    void clear_play_selection()
    {
        Start_ = nanoseconds_t( 0 );
        End_ = nanoseconds_t( Duration_ );
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
        if( milliseconds_t( Milliseconds ) > End_ )
        {
            Player_.stop();
        }
    }


    void handle_player_state_changed( QMediaPlayer::State State )
    {
        if( State == QMediaPlayer::StoppedState )
        {
            auto StartPosition = std::chrono::duration_cast<milliseconds_t>( Start_ );
            if( StartPosition > milliseconds_t(0) )
            {
                Player_.setPosition( StartPosition.count() );
            }
            PlayHandler_( play_status_t::stopped, StartPosition );
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
    nanoseconds_t           Start_;
    nanoseconds_t           End_;

};


// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
} // audio
} // qt
// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n

// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G
#endif // QT_AUDIO_AUDIO_PLAYER_HPP_INCLUDED
