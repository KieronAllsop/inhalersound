// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I
// Standard Includes
// None

// Boost Library Includes
// None

// Qt Includes
#include <QVBoxLayout>
#include <QLabel>

// Header Include
#include "qt_windows/process_sounds_intro_page.h"
// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I


ProcessSoundsIntroPage::
ProcessSoundsIntroPage( QWidget* Parent )
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

