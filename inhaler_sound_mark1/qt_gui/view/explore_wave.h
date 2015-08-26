// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G
#ifndef QT_GUI_VIEW_EXPLORE_WAVE_H_INCLUDED
#define QT_GUI_VIEW_EXPLORE_WAVE_H_INCLUDED
// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G

// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I

// Inhaler Includes
#include "inhaler/data_retriever.hpp"
#include "inhaler/label_file_editor.hpp"

// qt::audio Includes
#include "qt/audio/raw_data.hpp"
#include "qt/audio/audio_player.hpp"
#include "qt/audio/vocabulary_kind.hpp"

// QT Includes
#include <QFrame>
#include <QtMultimedia/QMediaPlayer>

// Boost Library Includes
#include <boost/date_time/posix_time/posix_time.hpp>

// C++ Standard Library Includes
#include <memory>
#include <locale>
#include <vector>

// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I

// Forward Declarations
class QLabel;
class QPushButton;
class QRadioButton;
class QLineEdit;
class QTreeView;
class QStandardItemModel;
class QModelIndex;
class QSlider;

namespace qt_gui {
namespace view {

    class wave_form;
    class wave_zoom;

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
    using vocabulary_t              = qt::audio::labelled_vocabulary;
    using label_editor_t            = inhaler::label_file_editor;
    using shared_label_editor_t     = std::shared_ptr<inhaler::label_file_editor>;

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

    void                    set_label_headers           ();

private:

    void                    on_label_typed              ();

    void                    on_clear_wave_label         ();

    void                    on_add_wave_label           ();

    void                    on_row_selection_changed    (   const QModelIndex& Current,
                                                            const QModelIndex& Previous   );

    void                    on_remove_wave_label        ();

    void                    on_edit_wave_label          ();

    void                    on_cancel_edit_label        ();

    void                    on_commit_changes           ();

    void                    on_revert_changes           ();

    void                    on_start_left_arrow         ();

    void                    on_end_left_arrow           ();

    void                    on_start_right_arrow        ();

    void                    on_end_right_arrow          ();

    void                    on_slider_changed           ();

    std::string             to_string                   (   const std::size_t& Sample   );

private:

    void                    enable_playing              ();

    void                    disable_playing             ();

    std::string             to_string                   (   const boost::posix_time::ptime& Timestamp   )
                                                        const;

    void                    set_play_position           (   const std::chrono::milliseconds& Position   );

    void                    set_zoom_sample_labels      ();

    void                    handler_selection_update    (   const std::chrono::nanoseconds& Start,
                                                            const std::chrono::nanoseconds& End,
                                                            const std::size_t& StartSample,
                                                            const std::size_t& EndSample   );

private:

    void                    on_play_pause_wave          ();

    void                    on_stop_wave                ();

    void                    on_selected_wave            (   const bool& Checked   );

    void                    on_clear_selection          ();

    void                    handle_player_status        (   player_t::play_status_t Status,
                                                            const player_t::milliseconds_t& Position   );

    void                    handle_player_buffer        (   player_t::probe_status_t Status,
                                                            const player_t::buffer_t& Buffer   );

private:

    patient_wave_details_t          WaveDetails_;
    shared_data_retriever_t         DataRetriever_;
    std::shared_ptr<player_t>       Player_;
    shared_data_t                   Data_;
    shared_label_editor_t           LabelEditor_;
    std::chrono::nanoseconds        SelectionStart_;
    std::chrono::nanoseconds        SelectionEnd_;
    std::size_t                     StartSample_;
    std::size_t                     EndSample_;
    std::vector<vocabulary_t>       LabelData_;
    std::vector<vocabulary_t>       OldLabelData_;

    boost::posix_time::time_facet*  TimestampFacet_;
    std::locale                     TimestampLocale_;

    QLabel*                         WaveName_Label_;
    QFrame*                         WaveView_Frame_;
    QFrame*                         WaveZoom_Frame_;
    wave_form*                      WaveFormView_;
    wave_zoom*                      WaveZoomStartView_;
    wave_zoom*                      WaveZoomEndView_;

    QPushButton*                    PlayPauseWave_Button_;
    QPushButton*                    StopWave_Button_;
    QLabel*                         Position_Label_;

    QRadioButton*                   PlaySelection_;
    QPushButton*                    ClearSelection_;

    QLabel*                         StartZoomPosition_Label_;
    QLabel*                         EndZoomPosition_Label_;
    QLabel*                         LabelWave_Label_;
    QLabel*                         EventLabelling_Label_;

    QLineEdit*                      LabelWave_LineEdit_;
    QPushButton*                    AddWaveLabel_Button_;
    QPushButton*                    RemoveLabelRow_Button_;
    QPushButton*                    EditLabelRow_Button_;
    QPushButton*                    CancelEditLabel_Button_;
    QPushButton*                    CommitToDatabase_Button_;
    QPushButton*                    Revert_Button_;

    QTreeView*                      LabelTreeView_;
    QStandardItemModel*             LabelModel_;

    QSlider*                        ZoomSample_Slider_;
    QLabel*                         MinSample_Label_;
    QLabel*                         MaxSample_Label_;
    QLabel*                         ZoomSlider_Label_;
    QLabel*                         ZoomIncrement_Label_;
    QPushButton*                    Start_FineTune_Lower_;
    QPushButton*                    Start_FineTune_Higher_;
    QPushButton*                    End_FineTune_Lower_;
    QPushButton*                    End_FineTune_Higher_;

    bool                            SelectionMade_;
    bool                            RowSelected_;
    bool                            BeingEdited_;
    bool                            LabelFileChanged_;
    int                             SelectedRow_;
    int                             EditedRow_;
    std::size_t                     ZoomIncrement_;

};

// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
} // end view
} // end qt_gui
// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n


// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G
#endif // QT_GUI_VIEW_EXPLORE_WAVE_H_INCLUDED
