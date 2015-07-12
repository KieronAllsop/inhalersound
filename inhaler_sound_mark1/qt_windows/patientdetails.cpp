// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I
// Standard Includes
#include <vector>
#include <fstream>

// Boost Library Includes
#include <boost/filesystem.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

// Quince Includes
#include <quince/quince.h>

// Qt Includes
#include <QFileDialog>

// Header Include
#include "qt_windows/patientdetails.h"

// Qt UI Generated Include
#include "qt_windows/ui_patientdetails.h"
// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I


PatientDetails::
PatientDetails
(   const shared_schema_t& schema,
    QWidget* parent) :
    QDialog(parent),
    ui_(new Ui::PatientDetails),
    schema_( schema )
{
    ui_->setupUi(this);
}

PatientDetails::
~PatientDetails()
{
    delete ui_;
}

void PatientDetails::on_pushButton_selectFiles_clicked()
{
    QStringList FileNames = QFileDialog::getOpenFileNames(
                            this,
                            "Select one or more wave files to open",
                            "/home",
                            "Wave Files (*.wav)");

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

                auto PatientID = schema_->get_patient_id("Kieron","Allsop","1972-Oct-14","BT191YX");

                if (PatientID)
                {
                schema_->insert_wave(*PatientID,"Accuhaler", Filename.string(), WriteTime, Data);
                }
            }
     }

}
