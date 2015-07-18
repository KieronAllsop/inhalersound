// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I
// Standard Includes
// None

// Boost Library Includes
// None

// Qt Includes
#include <QLabel>
#include <QPushButton>


// Header Include
#include "qt_windows/process_sounds_confirm_files.h"
// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I

ProcessSoundsConfirmFiles::
ProcessSoundsConfirmFiles
(   const shared_schema_t& Schema,
    QWidget* Parent )

: QWizardPage( Parent )

, Schema_( Schema )

// Create Widgets
, ImportFiles_Label_        ( new QLabel(               "Check list to ensure correct files have been chosen", this ) )
, ImportConfirmation_Label_ ( new QLabel(               "", this ) )
, ImportFiles_Button_       ( new QPushButton(          "Import Audio Files", this ) )

{

    setTitle( "Confirm files, add to Database and submit for Feature Extraction");

    // Set up event handling
  //  connect( ImportFiles_Button_, SIGNAL( released() ), this, SLOT( on_ImportFiles_clicked() ) );


    // Initialise Widgets







}


void ProcessSoundsConfirmFiles::on_ImportFiles_clicked()
{



}







