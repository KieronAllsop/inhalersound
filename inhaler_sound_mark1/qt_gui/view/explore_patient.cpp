// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I
// Standard Library Includes
// None

// Boost Library Includes
#include <boost/optional/optional_io.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/posix_time/posix_time_io.hpp>

// Qt Includes
#include <QFormLayout>
#include <QPushButton>
#include <QLabel>
#include <QTreeView>
#include <QSizePolicy>
#include <QSplitter>
#include <QtMultimedia/QMediaPlayer>

// Inhaler Includes
#include "inhaler/wave_importer.hpp"
#include "inhaler/data_retriever.hpp"

// Qt_gui Includes
#include "qt_gui/import_wizard/wizard.h"

// Self Include
#include "qt_gui/view/explore_patient.h"
// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I

// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
namespace qt_gui {
namespace view {
// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n

explore_patient::
explore_patient
(   const call_on_complete_t& CallOnComplete,
    QWidget* Parent )
: QFrame( Parent )

// Create Widgets
, PageTitle_Label_( new QLabel( this ) )
, ChangePatient_Button_( new QPushButton( tr("Change"), this ) )

, Title_Label_          ( new QLabel( this ) )
, Forename_Label_       ( new QLabel( this ) )
, MiddleName_Label_     ( new QLabel( this ) )
, Surname_Label_        ( new QLabel( this ) )
, DateOfBirth_Label_    ( new QLabel( this ) )
, Postcode_Label_       ( new QLabel( this ) )

, ImportWaves_Button_   ( new QPushButton( tr("Import Files"), this ) )
, OpenWave_Button_      ( new QPushButton( tr("View File"), this ) )
, WaveFiles_View_       ( new QTreeView( this ) )
, PlayPauseWave_Button_ ( new QPushButton( this ) )
, StopWave_Button_      ( new QPushButton( this ) )

, Splitter_             ( new QSplitter( this ) )

, WaveName_Label_       ( new QLabel( tr("<h2>No Wave Selected</h2>"), this ) )
, WaveView_Frame_       ( new QFrame( this ) )

//, PlayWaveTest_         ( new QPushButton( "Test Play Wave", this ) )
//, PlayWave_           ( new qt_gui::play_wave( Server_->connect_to_schema(), WaveImporter_, DataRetriever_, this ) )
{

    connect( ImportWaves_Button_, &QPushButton::released, [this](){ on_import_waves(); } );

    // Master Layout is a Vertical Box Layout
    QVBoxLayout* MasterLayout = new QVBoxLayout();

    QHBoxLayout* TitleLayout   = new QHBoxLayout();
    QVBoxLayout* DetailsLayout = new QVBoxLayout();
    QVBoxLayout* WaveLayout    = new QVBoxLayout();

//  connect(PlayWaveTest_, SIGNAL( released() ), this, SLOT( play_wave_file() ) );

    TitleLayout->addWidget( PageTitle_Label_, 0, Qt::AlignLeft );
    TitleLayout->addWidget( ChangePatient_Button_, 0, Qt::AlignRight );

    QFormLayout* PatientDetails = new QFormLayout();

    PatientDetails->addRow( tr("Title"),         Title_Label_ );
    PatientDetails->addRow( tr("Forename"),      Forename_Label_ );
    PatientDetails->addRow( tr("Middlename"),    MiddleName_Label_ );
    PatientDetails->addRow( tr("Surname"),       Surname_Label_ );
    PatientDetails->addRow( tr("Date of Birth"), DateOfBirth_Label_ );
    PatientDetails->addRow( tr("Postcode"),      Postcode_Label_ );

    DetailsLayout->addLayout( PatientDetails, 0 );
    DetailsLayout->addWidget( ImportWaves_Button_, 0, Qt::AlignRight );
    DetailsLayout->addWidget( WaveFiles_View_, 1 );

    QHBoxLayout* WaveButtonsLayout = new QHBoxLayout();

    WaveButtonsLayout->addWidget( PlayPauseWave_Button_, 0, Qt::AlignLeft );
    WaveButtonsLayout->addWidget( StopWave_Button_, 0, Qt::AlignLeft );
    WaveButtonsLayout->addStretch();
    WaveButtonsLayout->addWidget( OpenWave_Button_, 0, Qt::AlignRight );

    DetailsLayout->addLayout( WaveButtonsLayout, 0 );

    ImportWaves_Button_->setDefault( true );

    QWidget* DetailsWidget = new QWidget( this );
    auto DetailsSizePolicy = QSizePolicy();
    DetailsSizePolicy.setHorizontalStretch( 1 );
    DetailsWidget->setSizePolicy( DetailsSizePolicy );
    DetailsWidget->setLayout( DetailsLayout );

    Splitter_->addWidget( DetailsWidget );

    WaveView_Frame_->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding ) );
    WaveView_Frame_->setFrameStyle( QFrame::StyledPanel | QFrame::Sunken );

    WaveLayout->addWidget( WaveName_Label_, 0, Qt::AlignLeft );
    WaveLayout->addWidget( WaveView_Frame_ );

    QWidget* WaveWidget = new QWidget( this );
    auto WaveSizePolicy = QSizePolicy();
    WaveSizePolicy.setHorizontalStretch( 3 );
    WaveWidget->setSizePolicy( WaveSizePolicy );
    WaveWidget->setLayout( WaveLayout );

    Splitter_->addWidget( WaveWidget );

    MasterLayout->addLayout( TitleLayout );
    MasterLayout->addWidget( Splitter_, 1 );

    setLayout( MasterLayout );
}


void explore_patient::
reset
(   const shared_data_retriever_t& DataRetriever,
    const shared_schema_t& Schema   )
{
    DataRetriever_ = DataRetriever;
    Schema_ = Schema;

    const auto& Patient = DataRetriever_->patient();

    auto Name
        = QString("<h1>Viewing Inhaler Data for: %1 %2 %3</h1>")
            .arg( QString::fromUtf8( Patient.title.c_str() ) )
            .arg( QString::fromUtf8( Patient.forename.c_str() ) )
            .arg( QString::fromUtf8( Patient.surname.c_str() ) );

    //std::string DOB = to_simple_string(Patient.date_of_birth);

    PageTitle_Label_->setText( Name );

    // Load patient details view

    Title_Label_      ->setText( QString::fromUtf8( Patient.title.c_str() ) );
    Forename_Label_   ->setText( QString::fromUtf8( Patient.forename.c_str() ) );
//    MiddleName_Label_ ->setText( QString::fromUtf8( Patient.middlename.c_str() ) );
    Surname_Label_    ->setText( QString::fromUtf8( Patient.surname.c_str() ) );
//    DateOfBirth_Label_->setText( QString::fromUtf8( Patient.date_of_birth.c_str() ) );
    Postcode_Label_   ->setText( QString::fromUtf8( Patient.postcode.c_str() ) );

    // Load waves perhaps also
}


void explore_patient::
on_import_waves()
{
    qt_gui::import_wizard::wizard Wizard( DataRetriever_->patient(), Schema_ );
    if( Wizard.exec() )
    {
        // Refresh Wave Tree View
    }
}


//void explore_patient::
//play_wave_file()
//{
//   QMediaPlayer* player = new QMediaPlayer;
//    player->setMedia(QUrl::fromLocalFile("/home/kieron/coding/github/KieronAllsop/inhalersound/inhaler_sound_mark1/initial_data/test.wav"));
//    player->setVolume(100);
//    player->play();
//}

// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
} // end view
} // end qt_gui
// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
