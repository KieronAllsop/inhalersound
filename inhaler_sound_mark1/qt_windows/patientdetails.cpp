// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I
// Qt Includes
// None

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
