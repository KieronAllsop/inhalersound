// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I

// Self Include
#include "qt_gui/view/explore_patient.h"

// qt_gui Includes
#include "qt_gui/import_wizard/wizard.h"

// Inhaler Includes
#include "inhaler/wave_importer.hpp"

// Qt Includes
#include <QFormLayout>
#include <QPushButton>
#include <QLabel>
#include <QTreeView>
#include <QSizePolicy>
#include <QSplitter>
#include <QMessageBox>
#include <QDate>
#include <QList>
#include <QStandardItem>
#include <QMetaType>
#include <QtMultimedia/QMediaPlayer>
#include <QTemporaryFile>
#include <QApplication>
#include <QHeaderView>

// Standard Library Includes
#include <vector>
#include <fstream>

// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I


// Register our const patient_wave_details* type with QVariant
Q_DECLARE_METATYPE( const inhaler::patient_wave_details* );


// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
namespace qt_gui {
namespace view {
// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n

explore_patient::
explore_patient
(   const call_on_complete_t& CallOnComplete,
    QWidget* Parent )
: QFrame( Parent )
, CallOnComplete_( CallOnComplete )

, TimestampFacet_       ( new boost::posix_time::time_facet() )
, TimestampLocale_      ( std::locale(), TimestampFacet_ )
, Player_               ( new QMediaPlayer( this ) )
, Playing_              ( false )

// Create Widgets
, PageTitle_Label_      ( new QLabel( this ) )
, ChangePatient_Button_ ( new QPushButton( tr( "Change" ), this ) )

, Title_Label_          ( new QLabel( this ) )
, Forename_Label_       ( new QLabel( this ) )
, MiddleName_Label_     ( new QLabel( this ) )
, Surname_Label_        ( new QLabel( this ) )
, DateOfBirth_Label_    ( new QLabel( this ) )
, Postcode_Label_       ( new QLabel( this ) )

, ImportWaves_Button_   ( new QPushButton( tr( "Import Files" ), this ) )
, OpenWave_Button_      ( new QPushButton( tr( "View File" ), this ) )
, WaveFiles_View_       ( new QTreeView( this ) )
, WaveFiles_            ( new QStandardItemModel ( this ) )
, WaveFiles_Root_       ( WaveFiles_->invisibleRootItem() )
, PlayPauseWave_Button_ ( new QPushButton( tr( "Play" ), this ) )
, StopWave_Button_      ( new QPushButton( tr( "Stop" ), this ) )

, Splitter_             ( new QSplitter( this ) )

, WaveStatus_Label_     ( new QLabel( tr( "<h2><b>No Wave Selected</b></h2>" ), this ) )
, WaveSelected_Label_   ( new QLabel( this ) )
, WaveView_Frame_       ( new QFrame( this ) )
{
    TimestampFacet_->format( "%Y-%m-%d %H:%M" );

    initialise_widgets();

    initialise_layout();

    connect_event_handlers();

    reset_interface();
}


void explore_patient::connect_event_handlers()
{
    connect( ImportWaves_Button_, &QPushButton::released, [this](){ on_import_waves(); } );
    connect( ChangePatient_Button_, &QPushButton::released, [this](){ on_change_patient(); } );
    connect( OpenWave_Button_, &QPushButton::released, [this](){ on_open_wave();} );
    connect( PlayPauseWave_Button_, &QPushButton::released, [this](){ on_play_wave();} );
    connect( StopWave_Button_, &QPushButton::released, [this](){ on_stop_wave();} );

    connect
        (   WaveFiles_View_->selectionModel(),
            &QItemSelectionModel::currentChanged,
            [this]( const QModelIndex& Current, const QModelIndex& Previous )
            {
                on_wave_selection_changed( Current, Previous );
            }
        );
}


void explore_patient::initialise_widgets()
{
    WaveFiles_View_->setModel( WaveFiles_ );
    WaveFiles_View_->setSelectionBehavior( QTreeView::SelectRows );
    WaveFiles_View_->setEditTriggers( QTreeView::NoEditTriggers );
    WaveFiles_View_->setSelectionMode( QTreeView::SingleSelection );
    WaveFiles_View_->setAlternatingRowColors( true );
    WaveFiles_View_->setUniformRowHeights( true );

    WaveFiles_->setColumnCount(4);
    QStringList Headers;
    Headers
        << "Filename"
        << "Inhaler"
        << "Size"
        << "Date";
    WaveFiles_->setHorizontalHeaderLabels( Headers );

    ChangePatient_Button_->setDefault( false );
    ImportWaves_Button_->setDefault( true );
    PlayPauseWave_Button_->setEnabled( false );
    StopWave_Button_->setEnabled( false );

    disable_load_wave();
}


void explore_patient::initialise_layout()
{
    QVBoxLayout* MasterLayout = new QVBoxLayout();

    QHBoxLayout* TitleLayout   = new QHBoxLayout();
    QVBoxLayout* DetailsLayout = new QVBoxLayout();
    QVBoxLayout* WaveLayout    = new QVBoxLayout();

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

    QWidget* DetailsWidget = new QWidget( this );
    auto DetailsSizePolicy = QSizePolicy();
    DetailsSizePolicy.setHorizontalStretch( 1 );
    DetailsWidget->setSizePolicy( DetailsSizePolicy );
    DetailsWidget->setLayout( DetailsLayout );

    Splitter_->addWidget( DetailsWidget );

    WaveView_Frame_->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding ) );
    WaveView_Frame_->setFrameStyle( QFrame::StyledPanel | QFrame::Sunken );

    QHBoxLayout* WaveLayoutHeader = new QHBoxLayout;
    WaveLayoutHeader->addWidget( WaveStatus_Label_ );
    WaveLayoutHeader->addWidget( WaveSelected_Label_ );

    WaveLayout->addLayout( WaveLayoutHeader );
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


void explore_patient::reset_interface()
{
    // TODO
}


std::string explore_patient::
to_string( const boost::posix_time::ptime& Timestamp ) const
{
    std::stringstream Stream;
    Stream.imbue( TimestampLocale_ );
    Stream << Timestamp;
    return Stream.str();
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

    PageTitle_Label_->setText( Name );

    // Load patient details view

    Title_Label_->setText( QString::fromUtf8( Patient.title.c_str() ) );
    Forename_Label_->setText( QString::fromUtf8( Patient.forename.c_str() ) );

    std::string MiddleName = Patient.middlename ? *Patient.middlename : "";
    MiddleName_Label_->setText( QString::fromUtf8( MiddleName.c_str() ) );

    Surname_Label_->setText( QString::fromUtf8( Patient.surname.c_str() ) );

    auto DateOfBirth
        = QDate
            ( Patient.date_of_birth.date().year(),
              Patient.date_of_birth.date().month(),
              Patient.date_of_birth.date().day() );

    DateOfBirth_Label_->setText( DateOfBirth.toString( Qt::RFC2822Date ) );

    Postcode_Label_->setText( QString::fromUtf8( Patient.postcode.c_str() ) );

    // Clear current waves view
    WaveFiles_View_->setSortingEnabled( false );
    WaveFiles_->removeRows( 0, WaveFiles_->rowCount() );

    WaveFiles_Root_ = WaveFiles_->invisibleRootItem();

    const auto& Waves = DataRetriever_->wave_files();

    for( const auto& Wave: Waves )
    {
        add_wave_to_waves_view( *Wave.get() );
    }

    WaveFiles_View_->setSortingEnabled( true );
    WaveFiles_View_->sortByColumn( 0, Qt::DescendingOrder );
}


void explore_patient::
on_import_waves()
{
    qt_gui::import_wizard::wizard Wizard( DataRetriever_->patient(), Schema_ );
    if( Wizard.exec() )
    {
        for( auto Wave = DataRetriever_->updated_wave_data();
             Wave != DataRetriever_->wave_files().end();
             ++Wave )
        {
            add_wave_to_waves_view( *Wave->get() );
        }
    }
}


void explore_patient::
add_wave_to_waves_view
( const patient_wave_details_t& Wave )
{
    auto ImportTime = to_string( Wave.import_time() );

    QStandardItem* Parent;
    if( WaveFiles_ImportParents_.count( ImportTime ) )
    {
        Parent = WaveFiles_ImportParents_[ ImportTime ];
    }
    else
    {
        Parent = new QStandardItem( QString::fromUtf8( ImportTime.c_str() ) );
        WaveFiles_Root_->appendRow( Parent );
        WaveFiles_ImportParents_[ImportTime] = Parent;
    }

    QVariant WaveData;
    WaveData.setValue( &Wave );

    auto ModifiedTime = to_string( Wave.modified_time() );

    QList<QStandardItem*> Items;
    Items.append( new QStandardItem( QString::fromUtf8( Wave.name().c_str() ) ) );
    Items.append( new QStandardItem( QString::fromUtf8( Wave.inhaler_model().c_str() ) ) );
    Items.append( new QStandardItem( QString::fromUtf8( boost::lexical_cast<std::string>( Wave.size() ).c_str() ) ) );
    Items.append( new QStandardItem( QString::fromUtf8( ModifiedTime.c_str() ) ) );

    for( auto& Item: Items )
    {
        Item->setData( WaveData );
    }

    Parent->appendRow( Items );
}


void explore_patient::
on_wave_selection_changed
(   const QModelIndex& Current,
    const QModelIndex& Previous )
{
    if( auto Model = dynamic_cast<const QStandardItemModel*>( Current.model() ) )
    {
        auto Data = Model->itemFromIndex( Current )->data();
        // We only add the patient_wave_details to items that list wave details
        if( Data.canConvert<const patient_wave_details_t*>() )
        {
            auto WaveDetails = Data.value<const patient_wave_details_t*>();
            Selected_Wave_ = *WaveDetails;
            enable_load_wave();
        }
        // so if we don't have the data we are something else like an import time group
        else
        {
            disable_load_wave();
        }
    }
}


void explore_patient::
on_change_patient()
{
    QMessageBox Confirm;
    Confirm.setText("Change Patient");
    Confirm.setInformativeText( "Are you sure you want to change patient?" );
    Confirm.setStandardButtons( QMessageBox::Ok | QMessageBox::Cancel );
    Confirm.setDefaultButton( QMessageBox::Ok );

    if( Confirm.exec() == QMessageBox::Ok )
    {
        // Stop playing waves etc etc
        CallOnComplete_();
    }
}


void explore_patient::
enable_load_wave()
{
    OpenWave_Button_->setEnabled( true );
}


void explore_patient::
disable_load_wave()
{
    OpenWave_Button_->setEnabled( false );
}


void explore_patient::
on_open_wave()
{
    if( Selected_Wave_ )
    {
        QApplication::setOverrideCursor( Qt::WaitCursor );

        auto Data = DataRetriever_->retrieve_wave( *Selected_Wave_ );

        QApplication::restoreOverrideCursor();

        QTemporaryFile AudioFile;
        AudioFile.setAutoRemove( false );
        if( AudioFile.open() )
        {
            AudioFile.write( static_cast<const char*>( static_cast<const void*>( &Data[0] ) ), Data.size() );
        }
        auto Path = AudioFile.fileName();
        AudioFile.close();

        auto ModifiedTime = to_string( Selected_Wave_->modified_time() );

        WaveStatus_Label_->setText( "<h2><b>Loaded</b></h2>" );

        WaveSelected_Label_
            ->setText
                ( "<h2>File: <i>"
                  + QString::fromStdString( Selected_Wave_->name() )
                  + "</i>, Inhaler: <i>"
                  + QString::fromStdString( Selected_Wave_->inhaler_model() )
                  + "</i>, Recorded: <i>"
                  + QString::fromStdString( ModifiedTime )
                  + "</i></h2>" );

        Player_->setMedia( QUrl::fromLocalFile( Path ) );
        PlayPauseWave_Button_->setEnabled( true );
    }
}


void explore_patient::
on_play_wave()
{
    if( Playing_ )
    {
        Player_->pause();
        PlayPauseWave_Button_->setText( "Play" );
        WaveStatus_Label_->setText( "<h2>Paused</h2>" );
        Playing_ = false;
    }
    else
    {
        Player_->play();
        Playing_ = true;
        WaveStatus_Label_->setText( "<h2>Playing</h2>" );
        StopWave_Button_->setEnabled( true );
        PlayPauseWave_Button_->setText( "Pause" );
    }
}


void explore_patient::
on_stop_wave()
{
    Player_->stop();
    Playing_ = false;
    WaveStatus_Label_->setText( "<h2>Stopped</h2>" );
    StopWave_Button_->setEnabled( false );
    PlayPauseWave_Button_->setText( "Play" );
}


// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
} // end view
} // end qt_gui
// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
