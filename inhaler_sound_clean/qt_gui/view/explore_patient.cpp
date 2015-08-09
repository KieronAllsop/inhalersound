// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I

// Self Include
#include "qt_gui/view/explore_patient.h"

// Inhaler Includes
#include "inhaler/wave_importer.hpp"
#include "qt/audio/format.hpp"

// qt_gui Includes
#include "qt_gui/import_wizard/wizard.h"
#include "qt_gui/view/explore_wave.h"

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
#include <QTemporaryFile>
#include <QMediaPlayer>
#include <QUrl>
#include <QApplication>

// Standard Library Includes
// None

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

// Create Widgets
, PageTitle_Label_      ( new QLabel( this ) )
, ChangePatient_Button_ ( new QPushButton( tr("Change"), this ) )

, Title_Label_          ( new QLabel( this ) )
, Forename_Label_       ( new QLabel( this ) )
, MiddleName_Label_     ( new QLabel( this ) )
, Surname_Label_        ( new QLabel( this ) )
, DateOfBirth_Label_    ( new QLabel( this ) )
, Postcode_Label_       ( new QLabel( this ) )

, ImportWaves_Button_   ( new QPushButton( tr("Import Files"), this ) )
, OpenWave_Button_      ( new QPushButton( tr("View File"), this ) )
, WaveFiles_View_       ( new QTreeView( this ) )
, WaveFiles_            ( new QStandardItemModel ( this ) )
, WaveFiles_Root_       ( WaveFiles_->invisibleRootItem() )

, ExploreWaveView_      ( new qt_gui::view::explore_wave( this ) )

, Splitter_             ( new QSplitter( this ) )

{
    TimestampFacet_->format( "%Y-%m-%d %H:%M" );

    initialise_widgets();

    initialise_layout();

    connect_event_handlers();

    reset_interface();
}


void explore_patient::
connect_event_handlers()
{
    connect( ImportWaves_Button_, &QPushButton::released, [this](){ on_import_waves(); } );
    connect( ChangePatient_Button_, &QPushButton::released, [this](){ on_change_patient(); } );
    connect( OpenWave_Button_, &QPushButton::released, [this](){ on_open_wave();} );

    connect
        (   WaveFiles_View_->selectionModel(),
            &QItemSelectionModel::currentChanged,
            [this]( const QModelIndex& Current, const QModelIndex& Previous )
            {
                on_wave_selection_changed( Current, Previous );
            }
        );
}


void explore_patient::
initialise_widgets()
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

    WaveFiles_View_->setColumnWidth( 0, 200 );
    WaveFiles_View_->setColumnWidth( 1, 80 );
    WaveFiles_View_->setColumnWidth( 2, 60 );
    WaveFiles_View_->setColumnWidth( 3, 120 );

    ChangePatient_Button_->setDefault( false );
    ImportWaves_Button_->setDefault( true );

    disable_load_wave();
}


void explore_patient::
initialise_layout()
{
    QVBoxLayout* MasterLayout = new QVBoxLayout();

    QHBoxLayout* TitleLayout   = new QHBoxLayout();
    QVBoxLayout* DetailsLayout = new QVBoxLayout();

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

    WaveButtonsLayout->addWidget( OpenWave_Button_, 0, Qt::AlignRight );

    DetailsLayout->addLayout( WaveButtonsLayout, 0 );

    QWidget* DetailsWidget = new QWidget( this );
    auto DetailsSizePolicy = QSizePolicy();
    DetailsSizePolicy.setHorizontalStretch( 1 );
    DetailsWidget->setSizePolicy( DetailsSizePolicy );
    DetailsWidget->setLayout( DetailsLayout );

    Splitter_->addWidget( DetailsWidget );

    auto WaveSizePolicy = QSizePolicy();
    WaveSizePolicy.setHorizontalStretch( 3 );
    ExploreWaveView_->setSizePolicy( WaveSizePolicy );

    Splitter_->addWidget( ExploreWaveView_ );

    MasterLayout->addLayout( TitleLayout );
    MasterLayout->addWidget( Splitter_, 1 );

    setLayout( MasterLayout );
}


void explore_patient::
reset_interface()
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

    // retrieve all data from database
    const auto& Waves = DataRetriever_->wave_files();

    // populate view with retrieved data
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
    // run import files wizard
    qt_gui::import_wizard::wizard Wizard( DataRetriever_->patient(), Schema_ );
    if( Wizard.exec() )
    {
        for( auto Wave = DataRetriever_->updated_wave_data();
             Wave != DataRetriever_->wave_files().end();
             ++Wave )
        {
            // update list with new data
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
            enable_load_wave( *Selected_Wave_ );
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
enable_load_wave( const patient_wave_details_t& Wave )
{
    std::cout
        << "Selected wave is [" << Wave.name()
        << "], inhaler [" << Wave.inhaler_model()
        << "], size [" << Wave.size()
        << "], modified_time [" << Wave.modified_time()
        << "], import_time [" << Wave.import_time()
        << "]" << std::endl;

    OpenWave_Button_->setEnabled( true );
}


// TODO: amend above function to this when cout no longer needed for debugging
//void explore_patient::
//enable_load_wave()
//{
//    OpenWave_Button_->setEnabled( true );
//}


void explore_patient::
on_open_wave()
{
    if( Selected_Wave_ )
    {
        QApplication::setOverrideCursor( Qt::WaitCursor );

        // retrieve selected wav file
        auto Data = DataRetriever_->retrieve_wave( *Selected_Wave_ );

        // make temp file from retrieved data
        QTemporaryFile AudioFile;
        AudioFile.setAutoRemove( false );
        if( AudioFile.open() )
        {
            AudioFile.write( static_cast<const char*>( static_cast<const void*>( &Data[0] ) ), Data.size() );
        }
        auto Path = AudioFile.fileName();
        AudioFile.close();

        // sets up a new decoder
        Decoder_
            = std::make_shared<decoder_t>
                (   Path.toStdString(),
                    [this]( decoder_t::status_t Status, const decoder_t::buffer_t& Buffer )
                    {
                        handle_audio_decode( Status, Buffer );
                    }
                );

        // add raw data to WaveData
        WaveData_ = std::make_shared<qt::audio::raw_data>( Decoder_->path(), true );

        // start decoder
        Decoder_->start();
    }
}


void explore_patient::
handle_audio_decode( decoder_t::status_t Status, const decoder_t::buffer_t& Buffer )
{
    if( Status != decoder_t::status_t::buffer_ready )
    {
        std::cout << "Status = " << c_str( Status ) << std::endl;
    }

    if( !is_error( Status ) )
    {
        if( Status == decoder_t::status_t::buffer_ready )
        {
            // create buffers for the data
            WaveData_->add_buffer( Buffer );
        }
        else if( Status == decoder_t::status_t::finished )
        {

            // Print out whole WAV file for debugging
//            auto Sample = static_cast<const u_int8_t*>(WaveData_->data());
//            for( int i=1; i<WaveData_->samples_per_channel() ; ++Sample, ++i )
//            {
//                if ( i == 1)
//                {
//                    std::cout << "Print all samples from the WAV as signed integers" << std::endl;
//                }

//                std::cout
//                    << std::setw(4)
//                    << ( static_cast<short>( *Sample ) - 128 )
//                    << " ";

//                if( i%16 == 0 )
//                {
//                    std::cout << std::endl;
//                }
//            }

            ExploreWaveView_->reset( *Selected_Wave_, DataRetriever_, WaveData_ );

            QApplication::restoreOverrideCursor();
        }
    }
    else
    {
        if( Status != decoder_t::status_t::operation_aborted )
        {
            /// TODO Report Error
        }
        QApplication::restoreOverrideCursor();
    }

}


void explore_patient::
disable_load_wave()
{
    OpenWave_Button_->setEnabled( false );
}


// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
} // end view
} // end qt_gui
// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
