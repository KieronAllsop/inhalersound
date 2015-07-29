// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I
// Standard Includes
// None

// Boost Library Includes
#include <boost/optional/optional_io.hpp>
// Quince Includes
// None

// Qt Includes
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>

// Custom Includes
#include "inhaler/wave_importer.hpp"
#include "inhaler/data_retriever.hpp"

// Header Include
#include "qt_gui/play_wave.h"
// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I


// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
namespace qt_gui {
// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n



play_wave::play_wave
    (   const shared_schema_t& Schema,
        const shared_importer_t& Importer,
        const shared_data_retriever_t& DataRetriever,
        QWidget* Parent )
      : QDialog( Parent )
      , Schema_( Schema )
      , Importer_( Importer )
      , DataRetriever_ ( DataRetriever )
      , PageTitle_( new QLabel ( "Wave Player test area", this ) )
      , GetData_( new QPushButton ( "Get all data", this ) )



{

        connect(GetData_, SIGNAL( released() ), this, SLOT( on_get_data_clicked() ) );

        // Master Layout is a Vertical Box Layout
        QVBoxLayout* MasterLayout = new QVBoxLayout();
        MasterLayout->addWidget( PageTitle_ );



}

void play_wave::
on_get_data_clicked()
{
    auto PatientData = Importer_->patient();
    //std::cout << PatientData->id << std::endl;

    //DataRetriever_ = std::make_shared<inhaler::data_retriever>( Importer_->patient(), Schema_ );
}

// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
} // end qt_gui
// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
