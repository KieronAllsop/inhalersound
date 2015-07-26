// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G
#ifndef QT_GUI_MAINWINDOW_H_INCLUDED
#define QT_GUI_MAINWINDOW_H_INCLUDED
// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G

// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I
// Header Includes
#include <QMainWindow>

// Custom Includes
#include "inhaler/server.hpp"
#include "inhaler/wave_importer.hpp"
#include "inhaler/data_retriever.hpp"


// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I

// Forward Declarations
class QLabel;
class QPushButton;
class QStackedLayout;
class QWidget;


namespace qt_gui {
    class login_dialog;
    class play_wave;

    namespace import_wizard {
        class wizard;
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


class MainWindow : public QMainWindow
{
    Q_OBJECT
public:

//    using shared_schema_t = inhaler::server::shared_schema_t;
    using shared_server_t = std::shared_ptr<inhaler::server>;
    using shared_importer_t = std::shared_ptr<inhaler::wave_importer>;
    using shared_data_retriever_t = std::shared_ptr<inhaler::data_retriever>;

    explicit MainWindow(const shared_server_t& Server,
                        QWidget *parent = 0);


private slots:

    void                move_stack_datatech();
    void                move_stack_importwiz();
    void                move_stack_playwave();

public slots:

    void                import_wizard_finished(int Result);



private:

    shared_server_t                 Server_;
    shared_importer_t               WaveImporter_;
    shared_data_retriever_t         DataRetriever_;
//    shared_schema_t                 Schema_;


    // widgets
    QLabel*                         ExplanationLabel_;
    QLabel*                         DataLabel_;
    QPushButton*                    ImportWizardButton_;
    QPushButton*                    PlayWaveButton_;
    QStackedLayout*                 StackedLayout_;
    qt_gui::login_dialog*           LoginPrompt_;
    qt_gui::import_wizard::wizard*  ImportWizard_;
    qt_gui::play_wave*              PlayWave_;
};

// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
} // end qt_gui
// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n

// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G
#endif // QT_GUI_MAINWINDOW_H_INCLUDED
