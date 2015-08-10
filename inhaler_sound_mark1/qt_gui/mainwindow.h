// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G
#ifndef QT_GUI_MAINWINDOW_H_INCLUDED
#define QT_GUI_MAINWINDOW_H_INCLUDED
// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G

// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I

// Inhaler Includes
#include "inhaler/server.hpp"

// Qt Includes
#include <QMainWindow>

// C++ Standard Library Includes
// None

// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I

// Forward Declarations
class QLabel;
class QPushButton;
class QStackedLayout;
class QWidget;
class QFrame;


namespace qt_gui
{
    namespace prompt
    {
        class login;
        class get_patient;
    }

    namespace view
    {
        class explore_patient;
    }
}

namespace inhaler {
    class server;
}


// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
namespace qt_gui {
// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n

struct display_view
{
    enum enum_t
    {
        login           = 0,
        get_patient     = 1,
        explore_patient = 2
    };
};


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    using shared_server_t   = std::shared_ptr<inhaler::server>;
    using shared_schema_t   = inhaler::server::shared_schema_t;
    using user_t            = shared_schema_t::element_type::user_t;
    using patient_t         = shared_schema_t::element_type::patient_t;

public:

    explicit            MainWindow                  (   const shared_server_t& Server,
                                                        QWidget *parent = 0   );

    void                resize_window               (   const double& Fraction );

private:

    QWidget*            get_prompt_for              (   QFrame* Prompt   );
    QWidget*            get_view_for                (   QFrame* View   );

private:

    void                on_login                    (   const user_t& User,
                                                        const shared_schema_t& Schema   );

    void                on_logout                   ();

    void                on_get_patient              (   const patient_t& Patient   );

    void                on_leave_patient            ();

private:

    shared_server_t                 Server_;
    shared_schema_t                 Schema_;

    // widgets
    QStackedLayout*                 StackedLayout_;
    qt_gui::prompt::login*          LoginPrompt_;
    qt_gui::prompt::get_patient*    GetPatientPrompt_;
    qt_gui::view::explore_patient*  ExplorePatientView_;

};

// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
} // end qt_gui
// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n

// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G
#endif // QT_GUI_MAINWINDOW_H_INCLUDED
