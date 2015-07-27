// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G
#ifndef QT_GUI_VIEW_EXPLORE_PATIENT_HPP_INCLUDED
#define QT_GUI_VIEW_EXPLORE_PATIENT_HPP_INCLUDED
// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G

// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I
// Standard Library Includes
// None

// Qt Includes
#include <QFrame>

// Application Includes
//#include "application/state.hpp"

// Inhaler Includes
#include "inhaler/server.hpp"
#include "inhaler/wave_importer.hpp"
#include "inhaler/data_retriever.hpp"

// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I


// Forward Declarations
class QLabel;
class QLineEdit;
class QPushButton;

namespace inhaler
{
    class wave_importer;
    class server;
}

// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
namespace qt_gui {
namespace view {
// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n


class explore_patient : public QFrame
{
    Q_OBJECT

public:

    using shared_server_t           = std::shared_ptr<inhaler::server>;
    using shared_schema_t           = inhaler::server::shared_schema_t;
    using shared_importer_t         = std::shared_ptr<inhaler::wave_importer>;
    using shared_data_retriever_t   = std::shared_ptr<inhaler::data_retriever>;

//    using state_complete_t          = application::signal_state_complete;
//    using shared_state_complete_t   = std::shared_ptr<state_complete_t>;

    explicit                explore_patient             (   const shared_server_t& Server,
                                                            const shared_importer_t& Importer,
//                                                            const shared_state_complete_t& SignalComplete,
                                                            QWidget* Parent = 0   );
public:

    void                   populate_patient_form( const data_model::patient& );


private:

    shared_server_t             Server_;
    shared_importer_t           Importer_;


//    shared_data_retriever_t     DataRetriever_;

//    shared_state_complete_t     SharedSignalComplete_;
//    state_complete_t&           SignalComplete_;

    // Owned Widgets
    QLabel*             PageTitle_Label_;
    QLabel*             Title_Label_;
    QLabel*             Forename_Label_;
    QLabel*             MiddleName_Label_;
    QLabel*             Surname_Label_;
    QLabel*             DateOfBirth_Label_;
    QLabel*             Postcode_Label_;
    QLineEdit*          Title_Edit_;
    QLineEdit*          Forename_Edit_;
    QLineEdit*          MiddleName_Edit_;
    QLineEdit*          Surname_Edit_;
    QLineEdit*          DateOfBirth_Edit_;
    QLineEdit*          Postcode_Edit_;

};

// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
} // end view
} // end qt_gui
// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n

// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G
#endif // QT_GUI_VIEW_EXPLORE_PATIENT_HPP_INCLUDED
