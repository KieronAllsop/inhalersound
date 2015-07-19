// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G
#ifndef QT_WINDOWS_PROCESS_SOUNDS_PROCESS_FILES_H_INCLUDED
#define QT_WINDOWS_PROCESS_SOUNDS_PROCESS_FILES_H_INCLUDED
// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G

// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I

// Standard Library Includes
// none

// Qt Includes
#include <QWizardPage>

// Custom Includes
#include "data_model/schema.hpp"

// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I

// Forward Declarations
class QLabel;
class QPushButton;
class QProgressBar;

namespace inhaler
{
    class wave_importer;
}

class ProcessSoundsProcessFiles : public QWizardPage
{
    Q_OBJECT

public:

//    bool isComplete() const;

    using shared_importer_t = std::shared_ptr<inhaler::wave_importer>;

    ProcessSoundsProcessFiles    (   const shared_importer_t& Importer,
                                     QWidget* Parent=0   );

private slots:

    void                    on_ImportFiles_clicked();

private:
    // Data Variables
    shared_importer_t       Importer_;

    // Owned Widgets
    QLabel*                 ProcessingFiles_Label_;
    QLabel*                 ProcessComplete_Label_;
    QLabel*                 Progress_Label_ ;
    QPushButton*            ImportFiles_;
    QProgressBar*           ProgressBar_;

};

// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G
#endif // QT_WINDOWS_PROCESS_SOUNDS_PROCESS_FILES_H_INCLUDED
