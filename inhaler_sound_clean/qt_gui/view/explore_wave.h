// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G
#ifndef QT_GUI_VIEW_EXPLORE_WAVE_H_INCLUDED
#define QT_GUI_VIEW_EXPLORE_WAVE_H_INCLUDED
// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G

// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I

// Inhaler Includes
#include "inhaler/data_retriever.hpp"

// qt::audio Includes
#include "qt/audio/raw_data.hpp"
#include "qt/audio/audio_player.hpp"

// QT Includes
#include <QFrame>
#include <QMediaPlayer>

// Boost Library Includes
#include <boost/date_time/posix_time/posix_time.hpp>

// C++ Standard Library Includes
#include <memory>
#include <locale>


// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I

// Forward Delcarations
class QLabel;
class QPushButton;

namespace qt_gui {
namespace view {

    class wave_form;

}
}

// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
namespace qt_gui {
namespace view {
// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n


class explore_wave : public QFrame
{
    Q_OBJECT

public:

    using data_retriever_t          = inhaler::data_retriever;
    using shared_data_retriever_t   = std::shared_ptr<inhaler::data_retriever>;
    using patient_wave_details_t    = data_retriever_t::patient_wave_details_t;
    using data_t                    = qt::audio::raw_data;
    using shared_data_t             = std::shared_ptr<data_t>;
    using player_t                  = qt::audio::audio_player;

public:

    explicit                explore_wave                (   QWidget* Parent = 0   );

                            ~explore_wave               ();

    void                    reset                       (   const patient_wave_details_t& WaveDetails,
                                                            const shared_data_retriever_t& DataRetriever,
                                                            const shared_data_t& Data   );

private:

    void                    initialise_widgets          ();

    void                    initialise_layout           ();

    void                    connect_event_handlers      ();

    void                    reset_interface             ();

private:

    void                    enable_playing              ();

    void                    disable_playing             ();

    std::string             to_string                   (   const boost::posix_time::ptime& Timestamp   )
                                                        const;

    void                    set_play_position           (   const std::chrono::milliseconds& Position   );

private:

    void                    on_play_pause_wave          ();

    void                    on_stop_wave                ();

    void                    handle_player_status        (   player_t::play_status_t Status,
                                                            const player_t::milliseconds_t& Position   );

    void                    handle_player_buffer        (   player_t::probe_status_t Status,
                                                            const player_t::buffer_t& Buffer   );

private:

    patient_wave_details_t          WaveDetails_;
    shared_data_retriever_t         DataRetriever_;
    std::shared_ptr<player_t>       Player_;

    boost::posix_time::time_facet*  TimestampFacet_;
    std::locale                     TimestampLocale_;

    QLabel*                         WaveName_Label_;
    QFrame*                         WaveView_Frame_;
    wave_form*                      WaveFormView_;

    QPushButton*                    PlayPauseWave_Button_;
    QPushButton*                    StopWave_Button_;
    QLabel*                         Position_Label_;

};

// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
} // end view
} // end qt_gui
// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n


// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G
#endif // QT_GUI_VIEW_EXPLORE_WAVE_H_INCLUDED
