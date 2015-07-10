// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I
// Standard Includes
#include <vector>

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
    QStringList files = QFileDialog::getOpenFileNames(
                this,
                "Select one or more wave files to open",
                "/home",
                "Wave Files (*.wav)");

    int file_count = 0;
    QStringList list = files;
    //std::vector<uint8_t> vlist;
    QStringList::iterator it = list.begin();
    while(it != list.end()) {
        //vlist.push_back(*it);
        //myProcessing(*it);
        //schema_->insert_wave(schema_->get_patient_id("Kieron","Allsop","1972-Oct-14","BT191YX"),
        //                     "Accuhaler", "2015-Jul-05", *it);
        ++file_count;
        ++it;

    }

}
