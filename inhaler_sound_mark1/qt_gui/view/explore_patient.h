// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G
#ifndef QT_GUI_VIEW_EXPLORE_PATIENT_HPP_INCLUDED
#define QT_GUI_VIEW_EXPLORE_PATIENT_HPP_INCLUDED
// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G

// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I

// Inhaler Includes
#include "inhaler/server.hpp"
#include "inhaler/data_retriever.hpp"

// Qt Includes
#include <QFrame>

// Boost Library Includes
#include <boost/optional.hpp>

// Standard Library Includes
#include <functional>
#include <locale>

// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I


// Forward Declarations
class QLabel;
class QLineEdit;
class QPushButton;
class QTreeView;
class QSplitter;
class QStandardItemModel;
class QStandardItem;


// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
namespace qt_gui {
namespace view {
// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n


class explore_patient : public QFrame
{
    Q_OBJECT

public:

    using shared_server_t           = std::shared_ptr<inhaler::server>;
    using shared_schema_t           = inhaler::server::shared_schema_t;
    using data_retriever_t          = inhaler::data_retriever;
    using shared_data_retriever_t   = std::shared_ptr<inhaler::data_retriever>;
    using patient_wave_details_t    = data_retriever_t::patient_wave_details_t;
    using call_on_complete_t        = std::function< void() >;

public:

    explicit                explore_patient             (   const call_on_complete_t& CallOnComplete,
                                                            QWidget* Parent = 0   );


    void                    reset                       (   const shared_data_retriever_t& DataRetriever,
                                                            const shared_schema_t& Schema   );
private:

    void                    initialise_widgets          ();

    void                    initialise_layout           ();

    void                    connect_event_handlers      ();

    void                    reset_interface             ();

    void                    enable_load_wave            (   const patient_wave_details_t& Wave   );

    void                    disable_load_wave           ();

private:

    void                    on_import_waves             ();

    void                    on_change_patient           ();

    void                    on_wave_selection_changed   (   const QModelIndex& Current,
                                                            const QModelIndex& Previous );

    void                    on_open_wave_file           ();

private:

    std::string             to_string                   (   const boost::posix_time::ptime& Timestamp   )
                                                        const;

    void                    add_wave_to_waves_view      (   const patient_wave_details_t& Wave  );

private:

    call_on_complete_t              CallOnComplete_;
    shared_data_retriever_t         DataRetriever_;
    shared_schema_t                 Schema_;

    boost::posix_time::time_facet*  TimestampFacet_;
    std::locale                     TimestampLocale_;

    // Owned Widgets
    QLabel*             PageTitle_Label_;
    QPushButton*        ChangePatient_Button_;

    // Patient Details
    QLabel*             Title_Label_;
    QLabel*             Forename_Label_;
    QLabel*             MiddleName_Label_;
    QLabel*             Surname_Label_;
    QLabel*             DateOfBirth_Label_;
    QLabel*             Postcode_Label_;

    QPushButton*        ImportWaves_Button_;
    QPushButton*        OpenWave_Button_;

    QTreeView*                              WaveFiles_View_;
    QStandardItemModel*                     WaveFiles_;
    QStandardItem*                          WaveFiles_Root_;
    std::map<std::string, QStandardItem*>   WaveFiles_ImportParents_;
    boost::optional<patient_wave_details_t> Selected_Wave_;

    QPushButton*        PlayPauseWave_Button_;
    QPushButton*        StopWave_Button_;

    QSplitter*          Splitter_;

    QLabel*             WaveName_Label_;
    QFrame*             WaveView_Frame_;

//    QPushButton*        PlayWaveTest_;
};

// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
} // end view
} // end qt_gui
// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n


// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G
#endif // QT_GUI_VIEW_EXPLORE_PATIENT_HPP_INCLUDED
