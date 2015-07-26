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
#include "inhaler/server.hpp"
#include "inhaler/wave_importer.hpp"
#include "inhaler/data_retriever.hpp"
#include "data_model/schema.hpp"
#include "inhaler/patient_wave_details.hpp"

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
//    auto PatientData = Importer_->patient();

//    // This all works
//    std::cout << PatientData->id << std::endl;
//    std::cout << PatientData->title << std::endl;
//    std::cout << PatientData->forename << std::endl;
//    std::cout << PatientData->middlename << std::endl;
//    std::cout << PatientData->surname << std::endl;
//    std::cout << PatientData->date_of_birth << std::endl;
//    std::cout << PatientData->postcode << std::endl;

//    //Patient_ = Importer_->patient()*;

    const data_model::patient& Patient = *(Importer_->patient());
    std::make_shared<inhaler::data_retriever>( Patient, Schema_ );


}

// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
} // end qt_gui
// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
