// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I

// Self Include
#include "qt_gui/import_wizard/intro_page.h"

// Qt Includes
#include <QVBoxLayout>
#include <QLabel>

// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I

// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
namespace qt_gui {
namespace import_wizard {
// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n


//! \class  intro_page.cpp
//! \author Kieron Allsop
//!
//! \brief  First page of file importation wizard; an introduction.
//!
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

    Introduction_Label_->setWordWrap(true);

    QVBoxLayout* Layout = new QVBoxLayout;
    Layout->addWidget( Introduction_Label_ );
    setLayout( Layout );

}


// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
} // end import_wizard
} // end qt_gui
// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
