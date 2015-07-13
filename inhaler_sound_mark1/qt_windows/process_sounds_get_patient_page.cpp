// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I
// Standard Includes
// None

// Boost Library Includes
// None

// Qt Includes
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>

// Header Include
#include "qt_windows/process_sounds_get_patient_page.h"
// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I

ProcessSoundsGetPatientPage::
ProcessSoundsGetPatientPage
(   const shared_schema_t& Schema,
    QWidget* Parent )

: QWizardPage( Parent )

, Schema_( Schema )

// Create Widgets
, SelectPatient_Label_  ( new QLabel( "Search for existing Patient, or Register new Patient", this ) )
, SelectPatient_Button_ ( new QPushButton( "Search", this ) )
{
    setTitle( "Select Patient to associate files with" );
    // Set up event handling

}

