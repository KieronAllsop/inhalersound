// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G
#ifndef QT_GUI_MAINWINDOW_H_INCLUDED
#define QT_GUI_MAINWINDOW_H_INCLUDED
// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G

// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I
#include <QMainWindow>
// Custom Includes
#include "inhaler/wave_importer.hpp"
// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I

// Forward Declarations
class QLabel;
class QPushButton;
class QStackedLayout;

// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
namespace qt_gui {
// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n


class MainWindow : public QMainWindow
{
    Q_OBJECT
public:

    using shared_importer_t = std::shared_ptr<inhaler::wave_importer>;

    explicit MainWindow(QWidget *parent = 0);

    void                move_stack_to_datatech();

signals:

public slots:

    void                on_login_clicked();
    void                on_register_clicked();

private:

    QLabel*             ExplanationLabel_;
    QLabel*             LoginLabel_;
    QLabel*             DataLabel_;
    QPushButton*        LoginButton_;
    QPushButton*        RegisterButton_;
    QPushButton*        ImportWizButton_;
    QPushButton*        PlayWaveButton_;
    QStackedLayout*     StackedLayout_;

    shared_importer_t   Importer_;
};

// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
} // end qt_gui
// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n

// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G
#endif // QT_GUI_MAINWINDOW_H_INCLUDED
