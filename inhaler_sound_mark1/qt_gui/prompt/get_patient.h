// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G
#ifndef QT_GUI_PROMPT_GET_PATIENT_H_INCLUDED
#define QT_GUI_PROMPT_GET_PATIENT_H_INCLUDED
// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G

// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I

// Standard Library Includes
#include <memory>

// Qt Includes
#include <QFrame>

// Application Includes
#include "application/state.hpp"


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

// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
namespace qt_gui {
namespace prompt {
// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n

class get_patient : public QFrame
{
    Q_OBJECT

public:

    using shared_importer_t         = std::shared_ptr<inhaler::wave_importer>;
    using state_complete_t          = application::signal_state_complete;
    using shared_state_complete_t   = std::shared_ptr<state_complete_t>;

public:

                get_patient                 (   const shared_importer_t& Importer,
                                                const shared_state_complete_t& SignalComplete,
                                                QWidget* Parent=0   );

private slots:

    void        on_retrieve_clicked         ();
    void        on_text_credentials_changed ( const QString& Text );
    void        on_date_credentials_changed ( const QDate& Date );
    void        on_finished_clicked         ();

private:

    void        update_retrieval_state      ();

private:
    // Data Variables
    shared_importer_t           Importer_;
    shared_state_complete_t     SharedSignalComplete_;
    state_complete_t&           SignalComplete_;
    bool                        DateChanged_;

    // Owned Widgets
    QLabel*             Title_Label_;
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
    QPushButton*        Finish_Button_;

};

// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
} // end prompt
} // end qt_gui
// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n

// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G
#endif // QT_GUI_PROMPT_GET_PATIENT_H_INCLUDED
