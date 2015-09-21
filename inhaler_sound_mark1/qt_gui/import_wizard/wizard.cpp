// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I

// Self Include
#include "qt_gui/import_wizard/wizard.h"

// qt_gui::import_wizard Includes
#include "qt_gui/import_wizard/intro_page.h"
#include "qt_gui/import_wizard/select_files_page.h"
#include "qt_gui/import_wizard/process_files_page.h"

// inhaler Includes
#include "inhaler/wave_importer.hpp"

// Quince Includes
#include <quince/quince.h>

// Qt Includes
#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QSizePolicy>

// Standard Library Includes
#include <memory>

// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I

// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
namespace qt_gui {
namespace import_wizard {
// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n


//! \class  wizard.cpp
//! \author Kieron Allsop
//!
//! \brief  Controlling class for the wave importation wizard
//!
wizard::
wizard
(   const patient_t& Patient,
    const shared_schema_t& Schema,
    const shared_settings_t& Settings,
    QWidget* Parent )
: QWizard( Parent )
, Schema_( Schema )
, Settings_( Settings )
{
    auto WaveImporter = std::make_shared<inhaler::wave_importer>( Patient, Schema_, Settings_ );

    // Add Wizard pages
    addPage( new intro_page() );
    addPage( new select_files_page( WaveImporter ) );
    addPage( new process_files_page( WaveImporter ) );
    setWindowTitle( "Process Patient Data Wizard" );

    QList<QWizard::WizardButton> ButtonLayout;
    ButtonLayout << QWizard::Stretch
                 << QWizard::NextButton
                 << QWizard::FinishButton;
    this->setButtonLayout(ButtonLayout);
}


void wizard::
accept()
{
    QDialog::accept();
}


// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
} // end import_wizard
} // end qt_gui
// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
