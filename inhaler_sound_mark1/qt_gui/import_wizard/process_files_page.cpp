// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I

// qt_gui Includes
#include "qt_gui/import_wizard/process_files_page.h"

// inhaler Includes
#include "inhaler/wave_importer.hpp"

// Qt Includes
#include <QLabel>
#include <QProgressBar>
#include <QVBoxLayout>
#include <QCoreApplication>

// C++ Standard Library Includes
// None

// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I

// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
namespace qt_gui {
namespace import_wizard {
// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n


process_files_page::
process_files_page
(   const shared_importer_t& Importer,
    QWidget* Parent )

: QWizardPage       ( Parent )

, Importer_         ( Importer )
, Cancelled_        ( false )
, Complete_         ( true )
, DuplicateFile_    ( false )

// Create Widgets
, ProcessingFiles_Label_        ( new QLabel        ( "Selected wave files will be added to the Database and then are undergo the initial stage of processing"))
, ProgressLabel_				( new QLabel		( this ) )
, ProgressBar_  				( new QProgressBar	( this ) )
{
    setTitle( "Import and Process sound files");

//    setPixmap( QWizard::WatermarkPixmap, QPixmap( "watermark.png" ) );

    ProcessingFiles_Label_->setWordWrap(true);

    // Master Layout is a Vertical Box Layout
    QVBoxLayout* MasterLayout = new QVBoxLayout();


    // Initialise Widgets
    MasterLayout->addWidget( ProcessingFiles_Label_ );
    MasterLayout->addWidget( ProgressBar_ );
    MasterLayout->addWidget( ProgressLabel_ );
    setLayout( MasterLayout );

    completeChanged();
}


process_files_page::
~process_files_page()
{
    Cancelled_ = true;
    if( ProcessingThread_.joinable() )
    {
        ProcessingThread_.join();
    }
}


void process_files_page::
initializePage()
{
    ProgressBar_->setMinimum( 0 );
    ProgressBar_->setMaximum( Importer_->wave_files().size()-1 );
    ProgressBar_->reset();

    start_processing_files();
}


bool process_files_page::
isComplete() const
{
    if( Complete_ )
    {
        if( DuplicateFile_ )
        {
            ProgressLabel_->setText( tr( "Halted due to duplicate wave file(s)!\nClose Wizard and try again." ) );
        }
        else
        {
        ProgressLabel_->setText( tr( "Complete!" ) );
        }
    }
    return Complete_;
}


bool process_files_page::
event( QEvent* Event )
{
    if( Event->type() == import_status_event::type() )
    {
        if( auto* StatusEvent = dynamic_cast<import_status_event*>( Event ) )
        {
            on_import_status( StatusEvent->wave_file(), StatusEvent->number(), StatusEvent->import_status() );
            return true;
        }
    }
    return QWizardPage::event( Event );
}


void process_files_page::
on_import_status
(   const wave_details_t& WaveFile,
    int Number,
    import_status_t Status   )
{
    ProgressLabel_
        ->setText
            (   tr( "%1 wave file [%2]" )
                    .arg( QString::fromUtf8( c_str( Status ) ) )
                    .arg( QString::fromUtf8( WaveFile.path().filename().string().c_str() ) )   );

    if( Status == inhaler::import_status::finished )
    {
        ProgressBar_->setValue( Number );
    }
}


void process_files_page::
start_processing_files()
{
    ProcessingThread_ = std::thread( [&](){ process_files(); } );
}


void process_files_page::
process_files()
{
    try
    {
        Importer_->import_wave_files
            (   [&]( const inhaler::wave_details& WaveFile, int Number, inhaler::import_status Status )
                {
                    if( Cancelled_ )
                    {
                        return;
                    }
                    QCoreApplication::postEvent( this, new import_status_event( WaveFile, Number, Status ) );
                }
            );
        Complete_ = true;
    }
    catch( const inhaler::exception::duplicate_wave_file& Error )
    {
        DuplicateFile_ = true;
    }

    completeChanged();
}

// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
} // end import_wizard
} // end qt_gui
// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n







