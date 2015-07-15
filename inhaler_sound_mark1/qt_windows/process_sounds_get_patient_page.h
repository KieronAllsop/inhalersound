// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G
#ifndef QT_WINDOWS_PROCESS_SOUNDS_GET_PATIENT_PAGE_H_INCLUDED
#define QT_WINDOWS_PROCESS_SOUNDS_GET_PATIENT_PAGE_H_INCLUDED
// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G

// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I

// Standard Library Includes
// none

// Qt Includes
#include <QWizardPage>

// Custom Includes
#include "data_model/schema.hpp"

// Boost Includes
#include <boost/optional.hpp>

// Quince Includes
#include <quince/quince.h>

// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I

// Forward Declarations
class QLabel;
class QLineEdit;
class QDateEdit;
class QPushButton;


class ProcessSoundsGetPatientPage : public QWizardPage
{
    Q_OBJECT

public:

    boost::optional<quince::serial> getPatientID() {  return PatientID_; }
    void setPatientID( boost::optional<quince::serial> PatientID ) { PatientID_ = PatientID; }

    using shared_schema_t = std::shared_ptr<data_model::schema>;

    ProcessSoundsGetPatientPage     (   const shared_schema_t& Schema,
                                        QWidget* Parent=0   );

private slots:

    void on_retrieve_clicked();
    void on_text_credentials_changed( const QString& Text );
    void on_date_credentials_changed( const QDate& Date );

private:
    // Data Variables
    shared_schema_t     Schema_;
    boost::optional<quince::serial> PatientID_;
    bool DateChanged_ = false;
    void update_retrieval_state();

    // Owned Widgets
    QLabel*             EnterPatientDetails_Label_;
    QLabel*             FirstName_Label_;
    QLabel*             LastName_Label_;
    QLabel*             DOB_Label_;
    QLabel*             Postcode_Label_;
    QLabel*             RetrievalOutcome_Label_;
    QLineEdit*          FirstName_Edit_;
    QLineEdit*          LastName_Edit_;
    QLineEdit*          PostCode_Edit_;
    QDateEdit*          DOB_DateEdit_;
    QPushButton*        RetrievePatient_Button_;

};

// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G
#endif // QT_WINDOWS_PROCESS_SOUNDS_GET_PATIENT_PAGE_H_INCLUDED
