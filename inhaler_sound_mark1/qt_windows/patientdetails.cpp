#include "patientdetails.h"
#include "qt_windows/ui_patientdetails.h"

PatientDetails::PatientDetails(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PatientDetails)
{
    ui->setupUi(this);
}

PatientDetails::~PatientDetails()
{
    delete ui;
}
