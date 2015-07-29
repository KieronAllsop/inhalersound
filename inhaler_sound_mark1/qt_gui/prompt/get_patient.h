// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G
#ifndef QT_GUI_PROMPT_GET_PATIENT_H_INCLUDED
#define QT_GUI_PROMPT_GET_PATIENT_H_INCLUDED
// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G

// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I

// Standard Library Includes
#include <memory>
#include <functional>

// Boost Library Includes
#include <boost/optional.hpp>

// Qt Includes
#include <QFrame>

// Inhaler Includes
#include "inhaler/patient_retriever.hpp"


// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I

// Forward Declarations
class QLabel;
class QLineEdit;
class QDateEdit;
class QPushButton;
class QCalendarWidget;


// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
namespace qt_gui {
namespace prompt {
// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n

class get_patient : public QFrame
{
    Q_OBJECT

public:

    using patient_retriever_t   = std::shared_ptr<inhaler::patient_retriever>;
    using patient_t             = inhaler::patient_retriever::patient_t;
    using optional_patient_t    = boost::optional<patient_t>;
    using call_on_complete_t    = std::function< void( const patient_t& Patient ) >;

public:

                get_patient                 (   const call_on_complete_t& CallOnComplete,
                                                QWidget* Parent=0   );

    void        reset                       (   const patient_retriever_t& Retriever   );

private slots:

    void        on_retrieve_clicked         ();
    void        on_text_credentials_changed ( const QString& Text );
    void        on_date_credentials_changed ( const QDate& Date );
    void        on_finished_clicked         ();

private:

    void        update_retrieval_state      ();

private:
    // Data Variables
    patient_retriever_t     Retriever_;
    optional_patient_t      Patient_;
    call_on_complete_t      CallOnComplete_;
    bool                    DateChanged_;

    // Owned Widgets
    QLabel*             Title_Label_;
    QLabel*             FirstName_Label_;
    QLabel*             LastName_Label_;
    QLabel*             DOB_Label_;
    QLabel*             Postcode_Label_;
    QLabel*             Status_Label_;
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
