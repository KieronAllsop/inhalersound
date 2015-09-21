// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G
#ifndef QT_GUI_IMPORT_WIZARD_INTRO_PAGE_H_INCLUDED
#define QT_GUI_IMPORT_WIZARD_INTRO_PAGE_H_INCLUDED
// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G

// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I

// inhaler Includes
#include "inhaler/label_file_editor.hpp"

// Qt Includes
#include <QWizardPage>

// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I

// Forward Declarations
class QLabel;

// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
namespace qt_gui {
namespace import_wizard {
// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n


//! \headerfile intro_page.h
//! \author     Kieron Allsop
//!
class intro_page : public QWizardPage
{
    Q_OBJECT

public:
                    intro_page          (   QWidget* Parent=0   );

private:

    // Owned Widgets
    QLabel* Introduction_Label_;

};


// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
} // end import_wizard
} // end qt_gui
// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n

// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G
#endif // QT_GUI_IMPORT_WIZARD_INTRO_PAGE_H_INCLUDED
