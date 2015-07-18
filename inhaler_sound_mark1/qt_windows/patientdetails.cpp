// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I
// Standard Includes
// None

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

// Header Include
#include "qt_windows/patientdetails.h"

// Custom Includes
#include "qt_windows/process_sounds_intro_page.h"
#include "qt_windows/process_sounds_import_files.h"
#include "qt_windows/process_sounds_confirm_files.h"
#include "qt_windows/process_sounds_get_patient_page.h"
// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I


PatientDetails::
PatientDetails
(   const shared_schema_t& Schema,
    QWidget* Parent ):
    QWizard( Parent ),
    Schema_( Schema )
{
    // Add Wizard pages
    addPage( new ProcessSoundsIntroPage() );
    addPage( new ProcessSoundsGetPatientPage( Schema_ ) );
    addPage( new ProcessSoundsImportFiles( Schema_ ) );
    addPage( new ProcessSoundsConfirmFiles( Schema_ ) );
    setWindowTitle( "Process Patient Data Wizard" );
}

void PatientDetails::accept()
{
    // Might need stuff here, might not
    QDialog::accept();
}
