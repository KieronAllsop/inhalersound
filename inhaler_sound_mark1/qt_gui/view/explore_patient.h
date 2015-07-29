// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G
#ifndef QT_GUI_VIEW_EXPLORE_PATIENT_HPP_INCLUDED
#define QT_GUI_VIEW_EXPLORE_PATIENT_HPP_INCLUDED
// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G

// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I
// Standard Library Includes
#include <functional>

// Boost Includes
#include <boost/date_time/posix_time/posix_time.hpp>

// Qt Includes
#include <QFrame>

// Application Includes
//#include "application/state.hpp"

// Inhaler Includes
#include "inhaler/server.hpp"
#include "inhaler/data_retriever.hpp"

// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I


// Forward Declarations
class QLabel;
class QLineEdit;
class QPushButton;
class QTreeView;
class QSplitter;
class QStandardItemModel;


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
    using shared_data_retriever_t   = std::shared_ptr<inhaler::data_retriever>;
    using call_on_complete_t        = std::function< void() >;

public:

    explicit                explore_patient             (   const call_on_complete_t& CallOnComplete,
                                                            QWidget* Parent = 0   );


    void                    reset                       (   const shared_data_retriever_t& DataRetriever,
                                                            const shared_schema_t& Schema   );

//private slots:

//    void                   play_wave_file();


private:

    void                    on_import_waves             ();

    call_on_complete_t              CallOnComplete_;
    shared_data_retriever_t         DataRetriever_;
    shared_schema_t                 Schema_;
    boost::posix_time::time_facet*  DOBFacet_;
    std::locale                     DateLocale_;

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
    QTreeView*          WaveFiles_View_;
    QStandardItemModel* WaveFiles_;
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
