// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G
#ifndef QT_GUI_MAINWINDOW_H_INCLUDED
#define QT_GUI_MAINWINDOW_H_INCLUDED
// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G

// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I
// Header Includes
#include <QMainWindow>

// Custom Includes
#include "inhaler/server.hpp"
// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I

// Forward Declarations
class QLabel;
class QPushButton;
class QStackedLayout;
class QWidget;

namespace qt_gui {
    class login_dialog;

    namespace import_wizard {
        class wizard;
    }
}

// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
namespace qt_gui {
// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n


class MainWindow : public QMainWindow
{
    Q_OBJECT
public:

    using shared_server_t = std::shared_ptr<inhaler::server>;

    explicit MainWindow(const shared_server_t& Server,
                        QWidget *parent = 0);


private slots:

    void                move_stack_datatech();
    void                move_stack_importwiz();

public slots:
    void                import_wizard_finished(int Result);

//    void                move_stack_playwave();

private:

    shared_server_t Server_;

    // widgets
    QLabel*                         ExplanationLabel_;
    QLabel*                         DataLabel_;
    QPushButton*                    ImportWizardButton_; // change to wizard
    QPushButton*                    PlayWaveButton_;
    QStackedLayout*                 StackedLayout_;
    qt_gui::login_dialog*           LoginPrompt_;   // new
    qt_gui::import_wizard::wizard*  ImportWizard_;
};

// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
} // end qt_gui
// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n

// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G
#endif // QT_GUI_MAINWINDOW_H_INCLUDED
