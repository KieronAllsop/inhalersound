// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I
// Standard Includes
// None

// Boost Library Includes
// None

// Quince Includes
// None

// Qt Includes
#include <QVBoxLayout>
#include <QLabel>

// Custom Includes
#include "inhaler/wave_importer.hpp"
#include "inhaler/data_retriever.hpp"

// Header Include
#include "qt_gui/play_wave.h"
// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I


// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
namespace qt_gui {
// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n



PlayWave::PlayWave
    (   const shared_importer_t& Importer,
        QWidget* Parent )
      : QDialog( Parent )
      , Importer_( Importer )
      , PageTitle_( new QLabel ( "Wave Player test area", this ) )



{


        // Master Layout is a Vertical Box Layout
        QVBoxLayout* MasterLayout = new QVBoxLayout();
        MasterLayout->addWidget( PageTitle_ );



}

// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
} // end qt_gui
// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
