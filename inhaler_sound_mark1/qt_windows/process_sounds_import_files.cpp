// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I
// Standard Includes
#include <vector>
#include <fstream>

// Boost Library Includes
#include <boost/filesystem.hpp>
#include <boost/filesystem/detail/utf8_codecvt_facet.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/optional.hpp>
#include <boost/optional/optional_io.hpp>

// Qt Includes
#include <QFileDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSizePolicy>
#include <QTreeView>
#include <QPushButton>
#include <QLabel>
#include <QStandardItemModel>
#include <QAbstractButton>
#include <QComboBox>

// Importer Includes
#include "inhaler/wave_importer.hpp"

// Header Include
#include "qt_windows/process_sounds_import_files.h"
// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I


ProcessSoundsImportFiles::
ProcessSoundsImportFiles
(   const shared_importer_t& Importer,
    QWidget* Parent )

: QWizardPage( Parent )

, Importer_ ( Importer )

// Create Widgets
, SelectFiles_Label_            ( new QLabel             ( "Step 1. Select Inhaler Audio Files", this ) )
, SelectInhaler_Label_          ( new QLabel             ( "Step 2. Select Inhaler Type", this ) )
, ConfirmFiles_Label_           ( new QLabel             ( "Step 3. Confirm Files", this ) )
, SelectFiles_Button_           ( new QPushButton        ( "Select Audio Files", this ) )
, ConfirmFiles_Button_          ( new QPushButton        ( "Confirm Files", this ) )
, AudioFiles_View_              ( new QTreeView          ( this ) )
, AudioFiles_                   ( new QStandardItemModel ( this ) )
, SelectInhaler_                ( new QComboBox          ( this ) )
{
    setTitle( "Select sound files for Processing" );

    // Create instance of Wizard NEXT button
    //NextButton_ = wizard()->button(QWizard::NextButton);

    // Set up event handling
    connect( SelectFiles_Button_,   SIGNAL( released() ),           this, SLOT( on_SelectFiles_clicked() ) );
    connect( ConfirmFiles_Button_,  SIGNAL( clicked() ),            this, SLOT( on_Confirm_Button_Clicked() ) );
    connect( SelectInhaler_,        SIGNAL( highlighted(QString) ), this, SLOT( on_Inhaler_selected(QString) ) );
    //connect( NextButton_,         SIGNAL( clicked() ),            this, SLOT( on_Next_Button_Clicked() ) );

    // Initialise Widgets
    AudioFiles_View_->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding ) );
    AudioFiles_View_->setModel( AudioFiles_ );

    SelectInhaler_->setEnabled(false);
    SelectInhaler_->addItem("Accuhaler");
    SelectInhaler_->setEditable(false);

    ConfirmFiles_Button_->setEnabled(false);

    AudioFiles_->setColumnCount(3);
    QStringList Headers;
    Headers
        << "Name"
        << "Size"
        << "Date";
    AudioFiles_->setHorizontalHeaderLabels( Headers );

    // Initialise Layout

    // Master Layout is a Vertical Box Layout
    QVBoxLayout* MasterLayout = new QVBoxLayout();

    // Make the top Box contain a Horizontal Box Layout
    QHBoxLayout* TopRow = new QHBoxLayout();
    TopRow->addWidget( SelectFiles_Label_, 0, Qt::AlignLeft );
    TopRow->addWidget( SelectFiles_Button_, 0, Qt::AlignLeft );
    TopRow->addStretch();

    // Make the a Horizontal Box Layout to hold labels
    QHBoxLayout* LabelRow = new QHBoxLayout();
    LabelRow->addWidget( SelectInhaler_Label_, 0, Qt::AlignLeft );
    LabelRow->addWidget( ConfirmFiles_Label_, 0, Qt::AlignLeft );

    // Make the a Horizontal Box Layout
    QHBoxLayout* ButtonRow = new QHBoxLayout();
    ButtonRow->addWidget( SelectInhaler_, 0, Qt::AlignLeft );
    ButtonRow->addWidget( ConfirmFiles_Button_, 0, Qt::AlignLeft );


    MasterLayout->addLayout( TopRow );
    MasterLayout->addWidget( AudioFiles_View_ );
    MasterLayout->addLayout( LabelRow );
    MasterLayout->addLayout( ButtonRow );

    setLayout( MasterLayout );

}

bool ProcessSoundsImportFiles::isComplete() const
{
    return( Confirmed_ );
}


void ProcessSoundsImportFiles::
on_Inhaler_selected(QString Inhaler)
{
    Importer_->set_inhaler_model(Inhaler.toStdString());
    ConfirmFiles_Button_->setEnabled(true);
}

void ProcessSoundsImportFiles::
on_Confirm_Button_Clicked()
{
    SelectFiles_Button_->setEnabled(false);
    SelectInhaler_->setEnabled(false);
    ConfirmFiles_Button_->setEnabled(false);
    Confirmed_ = true;
    completeChanged();
}

void ProcessSoundsImportFiles::
on_SelectFiles_clicked()
{
    QStringList FileNames
         = QFileDialog::getOpenFileNames
             (   this,
                 "Select one or more wave files to open",
                 QDir::homePath(),
                 "Wave Files (*.wav)"   );

    if( FileNames.size() )
    {
        AudioFiles_->clear();

        //std::locale Utf8Locale( std::locale(), new boost::filesystem::detail::utf8_codecvt_facet );
        //boost::filesystem::path::imbue( Utf8Locale );

        boost::posix_time::time_facet* TimeFacet = new boost::posix_time::time_facet();
        std::locale TimeLocale( std::locale(), TimeFacet );
        TimeFacet->format( "%Y-%m-%d %H:%M" );

        std::vector< inhaler::wave_file_details > Waves;

        for( const auto& FileName: FileNames )
        {
            boost::filesystem::path Path( FileName.toStdString() );

            auto Name     = Path.filename();
            auto Size     = file_size( Path );
            auto DateTime = boost::posix_time::from_time_t( last_write_time( Path ) );

            Waves.emplace_back( Path, DateTime, Size );

            std::stringstream Date;
            Date.imbue( TimeLocale );
            Date << DateTime;

            QList<QStandardItem*> Items;
            Items.append( new QStandardItem( QString::fromUtf8( Name.native().c_str() ) ) );
            Items.append( new QStandardItem( QString::fromUtf8( boost::lexical_cast<std::string>( Size ).c_str() ) ) );
            Items.append( new QStandardItem( QString::fromUtf8( Date.str().c_str() ) ) );

            AudioFiles_->appendRow( Items );
        }
        Importer_->set_wave_files( std::move( Waves ) );
    }
    SelectInhaler_->setEnabled(true);
}


// TODO: Make this confirm page follow on from previous page

// For now I want to import files and start trying to get a file to play


    /*
    void PatientDetails::on_pushButton_selectFiles_clicked()
    {
        QStringList FileNames
            = QFileDialog::getOpenFileNames
                (   this,
                    "Select one or more wave files to open",
                    "/home",
                    "Wave Files (*.wav)" );

        for( const auto& FileName: FileNames )
        {
            boost::filesystem::path Path( FileName.toStdString() );
            auto Filename = Path.filename();
            auto FileSize  = file_size( Path );
            auto WriteTime = boost::posix_time::from_time_t( last_write_time( Path ) );

            std::ifstream File( Path.c_str(), std::ios::binary );

            if( File )
            {
                std::vector<uint8_t> Data;
                Data.reserve( FileSize );
                Data.assign
                    (   std::istreambuf_iterator<char>( File ),
                        std::istreambuf_iterator<char>()   );

                auto PatientID
                    = Schema_->get_patient_id
                        ( "Kieron", "Allsop", "1972-Oct-14", "BT191YX" );

                if( PatientID )
                {
                Schema_->insert_wave
                        ( *PatientID, "Accuhaler", Filename.string(), WriteTime, Data, FileSize );
                }

            }
        }
    }
    */
