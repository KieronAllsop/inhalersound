// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G
#ifndef QT_WINDOWS_PROCESS_SOUNDS_INTRO_PAGE_H_INCLUDED
#define QT_WINDOWS_PROCESS_SOUNDS_INTRO_PAGE_H_INCLUDED
// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G

// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I

// Standard Library Includes
// none

// Qt Includes
#include <QWizardPage>

// Forward Declarations
class QLabel;

// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I

class ProcessSoundsIntroPage : public QWizardPage
{
    Q_OBJECT

public:
                    ProcessSoundsIntroPage      (   QWidget* Parent=0   );

private:

    // Owned Widgets
    QLabel* Introduction_Label_;
};

// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G
#endif // QT_WINDOWS_PROCESS_SOUNDS_INTRO_PAGE_H_INCLUDED

