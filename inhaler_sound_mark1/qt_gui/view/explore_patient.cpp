// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I
// Standard Library Includes
// None

// Boost Library Includes
#include <boost/optional/optional_io.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/posix_time/posix_time_io.hpp>

// Qt Includes
#include <QFormLayout>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QString>

// Self Include
#include "qt_gui/view/explore_patient.h"

// Custom Includes
#include "inhaler/wave_importer.hpp"
#include "inhaler/data_retriever.hpp"
// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I

// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
namespace qt_gui {
namespace view {
// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n

explore_patient::
explore_patient
(   const shared_server_t& Server,
    const shared_importer_t& Importer,
//    const shared_state_complete_t& SignalComplete,
    QWidget* Parent )
: QFrame( Parent )

, Server_               ( Server )
, Importer_             ( Importer )

//, DataRetriever_        ( new inhaler::data_retriever( *(WaveImporter_->patient()), Server_->connect_to_schema() ) )
//, SharedSignalComplete_ ( SignalComplete )
//, SignalComplete_       ( *SharedSignalComplete_ )

// Create Widgets
, PageTitle_Label_      ( new QLabel( "Explore Patient", this ) )
, Title_Label_          ( new QLabel( "Title", this ) )
, Forename_Label_       ( new QLabel( "Forename", this ) )
, MiddleName_Label_     ( new QLabel( "Middle name", this ) )
, Surname_Label_        ( new QLabel( "Surname", this ) )
, DateOfBirth_Label_    ( new QLabel( "Date of birth", this ) )
, Postcode_Label_       ( new QLabel( "Postcode", this ) )
, Title_Edit_           ( new QLineEdit( this ) )
, Forename_Edit_        ( new QLineEdit( this ) )
, MiddleName_Edit_      ( new QLineEdit( this ) )
, Surname_Edit_         ( new QLineEdit( this ) )
, DateOfBirth_Edit_     ( new QLineEdit( this ) )
, Postcode_Edit_        ( new QLineEdit( this ) )

//, PlayWave_           ( new qt_gui::play_wave( Server_->connect_to_schema(), WaveImporter_, DataRetriever_, this ) )
{

  // MasterLayout
  QVBoxLayout* MasterLayout = new QVBoxLayout();

  QHBoxLayout* TopRow = new QHBoxLayout();

  QFormLayout* PatientDetails = new QFormLayout();

  PatientDetails->addRow( Title_Label_,         Title_Edit_ );
  PatientDetails->addRow( Forename_Label_,      Forename_Edit_ );
  PatientDetails->addRow( MiddleName_Label_,    MiddleName_Edit_ );
  PatientDetails->addRow( Surname_Label_,       Surname_Edit_ );
  PatientDetails->addRow( DateOfBirth_Label_,   DateOfBirth_Edit_ );
  PatientDetails->addRow( Postcode_Label_,      Postcode_Edit_ );

  Title_Edit_->setDisabled( true );
  Forename_Edit_->setDisabled( true );
  MiddleName_Edit_->setDisabled( true );
  Surname_Edit_->setDisabled( true );
  DateOfBirth_Edit_->setDisabled( true );
  Postcode_Edit_->setDisabled( true );

  TopRow->addLayout(PatientDetails);
  MasterLayout->addWidget(PageTitle_Label_);
  MasterLayout->addLayout(TopRow);

  setLayout( MasterLayout );
  adjustSize();

}


void explore_patient::
populate_patient_form( const data_model::patient& Patient )
{

    Title_Edit_->setText( QString::fromStdString( Patient.title ) );
    Forename_Edit_->setText( QString::fromStdString( Patient.forename ) );
    if ( !Patient.middlename )
    {
        MiddleName_Edit_->setText( "" );
    } else {
        MiddleName_Edit_->setText( QString::fromStdString( *Patient.middlename ) );
    }
    Surname_Edit_->setText( QString::fromStdString( Patient.surname ) );

    std::string DOB = to_simple_string(Patient.date_of_birth);


    DateOfBirth_Edit_->setText( QString::fromStdString( DOB ) );
    Postcode_Edit_->setText( QString::fromStdString( Patient.postcode ) );

        //        auto DataRetriever = std::make_shared<inhaler::data_retriever>( Patient, Server_->connect_to_schema() );

}

// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
} // end view
} // end qt_gui
// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
