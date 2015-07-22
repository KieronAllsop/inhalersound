// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I
// Standard Includes
// None

// Boost Library Includes
// None

// Quince Includes
// None

// Qt Includes
#include <QVBoxLayout>

// Custom Includes
#include "inhaler/wave_importer.hpp"

// Header Include
#include "qt_gui/play_wave.h"
// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I

PlayWave::PlayWave
    (   const shared_importer_t& Importer,
        QWidget* Parent ):
        QDialog( Parent ),
        Importer_( Importer )
{
        // Master Layout is a Vertical Box Layout
        QVBoxLayout* MasterLayout = new QVBoxLayout();



}
