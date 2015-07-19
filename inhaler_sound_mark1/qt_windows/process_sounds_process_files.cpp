// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I
// Standard Includes
// None

// Boost Library Includes
#include <boost/format.hpp>

// Qt Includes
#include <QLabel>
#include <QPushButton>
#include <QProgressBar>
#include <QVBoxLayout>

// Importer Includes
#include "inhaler/wave_importer.hpp"

// Header Include
#include "qt_windows/process_sounds_process_files.h"
// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I

ProcessSoundsProcessFiles::
ProcessSoundsProcessFiles
(   const shared_importer_t& Importer,
    QWidget* Parent )

: QWizardPage( Parent )

, Importer_ ( Importer )

// Create Widgets
, ProcessingFiles_Label_            ( new QLabel        ( "Selected wave files will be added to the Database and are undergoing the initial stage of processing"))
, ProcessComplete_Label_            ( new QLabel        ( "All files successfully processed", this ) )
, Progress_Label_                   ( new QLabel        ( "", this ) )
, ImportFiles_                      ( new QPushButton   ( "Import & Process Files" ) )
, ProgressBar_                      ( new QProgressBar  ( this ) )
{
    setTitle( "Import and Process sound files");

//    setPixmap( QWizard::WatermarkPixmap, QPixmap( "watermark.png" ) );

    ProcessComplete_Label_->setWordWrap(true);

    // Set up event handling
    connect( ImportFiles_ , SIGNAL( released() ), this, SLOT( on_ImportFiles_clicked() ) );


    // Initialise Widgets
    QVBoxLayout* Layout = new QVBoxLayout;
    Layout->addWidget( ImportFiles_ );
    Layout->addWidget( ProcessingFiles_Label_ );
    Layout->addWidget( ProgressBar_ );
    Layout->addWidget( Progress_Label_ );
    Layout->addWidget( ProcessComplete_Label_ );
    setLayout( Layout );

}

void ProcessSoundsProcessFiles::on_ImportFiles_clicked()
{
    auto Max = Importer_->wave_files().size();

    std::cout << "inside import files clicked" << std::endl;

    ProgressBar_->setMinimum( 0 );
    ProgressBar_->setMaximum( Max-1 );

    Importer_->import_wave_files
        (   [&]( const inhaler::wave_file_details& WaveFile, int Index, inhaler::import_status Status )
            {
                auto Message = str( boost::format( "Processing wave file [%s]" ) % WaveFile.path().string() );

                Progress_Label_->setText( Message.c_str() );

                ProgressBar_->setValue( Index );

                if( Status == inhaler::import_status::finished )
                {

                }
            }
        );
}








