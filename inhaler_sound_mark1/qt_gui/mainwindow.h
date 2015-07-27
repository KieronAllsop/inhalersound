// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G
#ifndef QT_GUI_MAINWINDOW_H_INCLUDED
#define QT_GUI_MAINWINDOW_H_INCLUDED
// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G

// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I

// C++ Standard Library Includes
// None


// Qt Includes
#include <QMainWindow>

// Application Includes
#include "application/state.hpp"

// Inhaler Includes
#include "inhaler/server.hpp"
#include "inhaler/wave_importer.hpp"
#include "inhaler/data_retriever.hpp"


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
    class wave_importer;
    class data_retriever;
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

    using shared_schema_t           = inhaler::server::shared_schema_t;
    using shared_server_t           = std::shared_ptr<inhaler::server>;
    using shared_importer_t         = std::shared_ptr<inhaler::wave_importer>;
    using shared_data_retriever_t   = std::shared_ptr<inhaler::data_retriever>;
    using shared_state_complete_t   = std::shared_ptr<application::signal_state_complete>;

public:

    explicit            MainWindow                  (   const shared_server_t& Server,
                                                        QWidget *parent = 0   );

private:

    QWidget*            get_prompt_for              (   QFrame* Prompt   );
    QWidget*            get_view_for                (   QFrame* View   );

    void                on_state_complete           (   const application::state& State   );

private:

    shared_server_t                 Server_;
    shared_importer_t               WaveImporter_;
//    shared_data_retriever_t         DataRetriever_;
    shared_state_complete_t         SignalComplete_;

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
