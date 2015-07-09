// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G
#ifndef PATIENTDETAILS_H
#define PATIENTDETAILS_H
// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G

// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I
// Standard Library Includes
// none

// Qt Includes
#include <QDialog>

// Custom Includes
#include "data_model/schema.hpp"
// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I


namespace Ui
{
    class PatientDetails;
}

class PatientDetails : public QDialog
{
    Q_OBJECT

public:

    using shared_schema_t = std::shared_ptr<data_model::schema>;

    explicit PatientDetails ( const shared_schema_t& schema,
                              QWidget* parent = 0);

             ~PatientDetails();

private slots:
    void on_pushButton_selectFiles_clicked();

private:
    Ui::PatientDetails* ui_;
    shared_schema_t schema_;
};

// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G
#endif // PATIENTDETAILS_H
