// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I
// Standard Includes
#include <memory>

// Boost Library Includes
// None

// Quince Includes
#include <quince/quince.h>

// Qt Includes
#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QSizePolicy>

// Inhaler Includes
#include "inhaler/wave_importer.hpp"

// Wizard Page Includes
#include "qt_gui/import_wizard/intro_page.h"
#include "qt_gui/import_wizard/get_patient_page.h"
#include "qt_gui/import_wizard/select_files_page.h"
#include "qt_gui/import_wizard/process_files_page.h"

// Self Include
#include "qt_gui/import_wizard/wizard.h"
// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I

// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
namespace qt_gui {
namespace import_wizard {
// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n

wizard::
wizard
(   const shared_schema_t& Schema,
    QWidget* Parent ):
    QWizard( Parent ),
    Schema_( Schema )
{
    auto WaveImporter = std::make_shared<inhaler::wave_importer>( Schema_ );

    // Add Wizard pages
    addPage( new intro_page() );
    addPage( new get_patient_page( WaveImporter ) );
    addPage( new select_files_page( WaveImporter ) );
    addPage( new process_files_page( WaveImporter ) );
    setWindowTitle( "Process Patient Data Wizard" );
}

void wizard::
accept()
{
    // Might need stuff here, might not
    QDialog::accept();
}

// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
} // end import_wizard
} // end qt_gui
// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
