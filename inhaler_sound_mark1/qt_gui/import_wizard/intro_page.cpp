// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I
// Standard Includes
// None

// Boost Library Includes
// None

// Qt Includes
#include <QVBoxLayout>
#include <QLabel>

// Header Include
#include "qt_gui/import_wizard/intro_page.h"
// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I

// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
namespace qt_gui {
namespace import_wizard {
// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n

intro_page::
intro_page( QWidget* Parent )
: QWizardPage( Parent )
, Introduction_Label_
    ( new QLabel
        ( "This wizard will guide you through the process of importing"
          " wave files from an inhaler and associating them with the a"
          " patient for further analysis." ) )
{
    setTitle( "Import and Process Inhaler Sounds" );

//    setPixmap( QWizard::WatermarkPixmap, QPixmap( "watermark.png" ) );

    Introduction_Label_->setWordWrap(true);

    QVBoxLayout* Layout = new QVBoxLayout;
    Layout->addWidget( Introduction_Label_ );
    setLayout( Layout );
}

// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
} // end import_wizard
} // end qt_gui
// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
