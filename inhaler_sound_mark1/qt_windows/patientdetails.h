#ifndef PATIENTDETAILS_H
#define PATIENTDETAILS_H

#include <QDialog>

namespace Ui {
class PatientDetails;
}

class PatientDetails : public QDialog
{
    Q_OBJECT

public:
    explicit PatientDetails(QWidget *parent = 0);
    ~PatientDetails();

private:
    Ui::PatientDetails *ui;
};

#endif // PATIENTDETAILS_H
