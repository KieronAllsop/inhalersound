// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G
#ifndef QT_WINDOWS_PROCESS_SOUNDS_GET_PATIENT_PAGE_H_INCLUDED
#define QT_WINDOWS_PROCESS_SOUNDS_GET_PATIENT_PAGE_H_INCLUDED
// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G

// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I

// Standard Library Includes
#include <memory>

// Qt Includes
#include <QWizardPage>


// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I

// Forward Declarations
class QLabel;
class QLineEdit;
class QDateEdit;
class QPushButton;
class QCalendarWidget;

namespace inhaler
{
    class wave_importer;
}

class ProcessSoundsGetPatientPage : public QWizardPage
{
    Q_OBJECT

public:

    using shared_importer_t = std::shared_ptr<inhaler::wave_importer>;

public:

    ProcessSoundsGetPatientPage     (   const shared_importer_t& Importer,
                                        QWidget* Parent=0   );

    bool isComplete() const;

private slots:

    void on_retrieve_clicked();
    void on_text_credentials_changed( const QString& Text );
    void on_date_credentials_changed( const QDate& Date );

private:

    void update_retrieval_state();

private:
    // Data Variables
    shared_importer_t   Importer_;
    bool                DateChanged_;

    // Owned Widgets
    QLabel*             EnterPatientDetails_Label_;
    QLabel*             FirstName_Label_;
    QLabel*             LastName_Label_;
    QLabel*             DOB_Label_;
    QLabel*             Postcode_Label_;
    QLabel*             PatientRetrieved_Label_;
    QLabel*             TryAgain_Label_;
    QLineEdit*          FirstName_Edit_;
    QLineEdit*          LastName_Edit_;
    QLineEdit*          PostCode_Edit_;
    QCalendarWidget*    Calendar_Widget_;
    QDateEdit*          DOB_DateEdit_;
    QPushButton*        RetrievePatient_Button_;

};

// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G
#endif // QT_WINDOWS_PROCESS_SOUNDS_GET_PATIENT_PAGE_H_INCLUDED
