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

// Custom Includes
#include "inhaler/wave_importer.hpp"

// Custom Includes
#include "qt_windows/process_sounds_intro_page.h"
#include "qt_windows/process_sounds_import_files.h"
#include "qt_windows/process_sounds_process_files.h"
#include "qt_windows/process_sounds_get_patient_page.h"

// Header Include
#include "qt_windows/patientdetails.h"
// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I


PatientDetails::
PatientDetails
(   const shared_schema_t& Schema,
    QWidget* Parent ):
    QWizard( Parent ),
    Schema_( Schema )
{
    auto WaveImporter = std::make_shared<inhaler::wave_importer>( Schema_ );

    // Add Wizard pages
    addPage( new ProcessSoundsIntroPage() );
    addPage( new ProcessSoundsGetPatientPage( WaveImporter ) );
    addPage( new ProcessSoundsImportFiles( WaveImporter ) );
    addPage( new ProcessSoundsProcessFiles( WaveImporter ) );
    setWindowTitle( "Process Patient Data Wizard" );
}

void PatientDetails::accept()
{
    // Might need stuff here, might not
    QDialog::accept();
}
