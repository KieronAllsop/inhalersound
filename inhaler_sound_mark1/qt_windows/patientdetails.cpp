// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I
// Standard Includes
// None

// Boost Library Includes
// None

// Quince Includes
#include <quince/quince.h>

// Qt Includes
#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QSizePolicy>

// Header Include
#include "qt_windows/patientdetails.h"

// Custom Includes
#include "qt_windows/process_sounds_intro_page.h"
#include "qt_windows/process_sounds_import_files.h"
#include "qt_windows/process_sounds_confirm_files.h"
#include "qt_windows/process_sounds_get_patient_page.h"
// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I


PatientDetails::
PatientDetails
(   const shared_schema_t& Schema,
    QWidget* Parent ):
    QWizard( Parent ),
    Schema_( Schema )
{
    // Add Wizard pages
    addPage( new ProcessSoundsIntroPage() );
    addPage( new ProcessSoundsGetPatientPage( Schema_ ) );
    addPage( new ProcessSoundsImportFiles( Schema_ ) );
    addPage( new ProcessSoundsConfirmFiles( Schema_ ) );
    setWindowTitle( "Process Patient Data Wizard" );
}

void PatientDetails::accept()
{
    // Might need stuff here, might not
    QDialog::accept();
}


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
