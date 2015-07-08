// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I
// Standard Includes
#include <vector>

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

    QStringList list = files;
    QStringList::Iterator it = list.begin();
    while(it != list.end()) {
        //myProcessing(*it);
        ++it;
    }

}
