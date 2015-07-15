// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I
// Standard Includes
// None

// Boost Includes
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/optional.hpp>

// Qt Includes
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QFormLayout>
#include <QDateEdit>
#include <QCalendarWidget>
#include <QLineEdit>
#include <QDate>
#include <QWizardPage>

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
, EnterPatientDetails_Label_    ( new QLabel( "Please enter patient details (all fields must be completed)", this ) )
, FirstName_Label_              ( new QLabel( "First name", this ) )
, LastName_Label_               ( new QLabel( "Last name", this ) )
, DOB_Label_                    ( new QLabel( "Date of birth", this ) )
, Postcode_Label_               ( new QLabel( "Postcode", this ) )
, PatientRetrieved_Label_       ( new QLabel( "", this ) )
, TryAgain_Label_               ( new QLabel( "", this ) )
, FirstName_Edit_               ( new QLineEdit( this ) )
, LastName_Edit_                ( new QLineEdit( this ) )
, PostCode_Edit_                ( new QLineEdit( this ) )
, Calendar_Widget_              ( new QCalendarWidget( this ) )
, DOB_DateEdit_                 ( new QDateEdit( QDate::currentDate() ) )
, RetrievePatient_Button_       ( new QPushButton( "Retrieve", this ) )
{
    setTitle( "Select Patient to associate files with" );

    // Set up event handling
    connect( RetrievePatient_Button_,   SIGNAL( released() ),                   this, SLOT( on_retrieve_clicked() ) );
    connect( FirstName_Edit_,           SIGNAL( textChanged(const QString&) ),  this, SLOT( on_text_credentials_changed(const QString&) ) );
    connect( LastName_Edit_,            SIGNAL( textChanged(const QString&) ),  this, SLOT( on_text_credentials_changed(const QString&) ) );
    connect( DOB_DateEdit_,             SIGNAL( dateChanged(const QDate&) ),    this, SLOT( on_date_credentials_changed(const QDate&) ) );
    connect( PostCode_Edit_,            SIGNAL( textChanged(const QString&) ),  this, SLOT( on_text_credentials_changed(const QString&) ) );
    connect( PatientRetrieved_Label_,   SIGNAL( textChanged(const QString&) ),  this, SIGNAL( completeChanged() ) );

    // Initialise State
    RetrievePatient_Button_->setDefault( true );      // Handle keyboard enter
    RetrievePatient_Button_->setEnabled( false );     // Disabled by default

    Calendar_Widget_->setVisible( false );
    DOB_DateEdit_->setCalendarPopup( Calendar_Widget_ );
    DOB_DateEdit_->setMinimumDate( QDate::currentDate().addYears(-120) );
    DOB_DateEdit_->setMaximumDate( QDate::currentDate() );
    DOB_DateEdit_->setDisplayFormat( "dd-MMM-yyyy" );

    // Initialise Layout

    // Master Layout is a Vertical Box Layout
    QVBoxLayout* MasterLayout = new QVBoxLayout;

    QHBoxLayout* TableHeader = new QHBoxLayout;
    TableHeader->addWidget( EnterPatientDetails_Label_ );
    TableHeader->addWidget( RetrievePatient_Button_ );

    QHBoxLayout* TableFooter = new QHBoxLayout;
    TableFooter->addWidget( TryAgain_Label_ );
    TableFooter->addWidget( PatientRetrieved_Label_ );

    QFormLayout* CredentialsForm = new QFormLayout;
    CredentialsForm->addRow( FirstName_Label_, FirstName_Edit_ );
    CredentialsForm->addRow( LastName_Label_, LastName_Edit_ );
    CredentialsForm->addRow( DOB_Label_, DOB_DateEdit_ );
    CredentialsForm->addRow( Postcode_Label_, PostCode_Edit_ );


    MasterLayout->addLayout( TableHeader );
    MasterLayout->addLayout( CredentialsForm );
    MasterLayout->addLayout( TableFooter );
    setLayout( MasterLayout );

    adjustSize();
}

// TODO: Get this to work!!!
// Disable NEXT button until a patient has been retrieved
bool ProcessSoundsGetPatientPage::isComplete() const
{
    return( !PatientRetrieved_Label_->text().isEmpty() );
}

void ProcessSoundsGetPatientPage::on_text_credentials_changed( const QString& Text )
{
    update_retrieval_state();
}

void ProcessSoundsGetPatientPage::on_date_credentials_changed( const QDate& Date )
{
    if( Date != QDate::currentDate() )
    {
    DateChanged_ = true;
    update_retrieval_state();
    }
}

void ProcessSoundsGetPatientPage::update_retrieval_state()
{
    RetrievePatient_Button_
        ->setEnabled
            (       FirstName_Edit_->text().size()
                &&  LastName_Edit_->text().size()
                &&  PostCode_Edit_->text().size()
                &&  DateChanged_ );
}

void ProcessSoundsGetPatientPage::on_retrieve_clicked()
{
    auto FirstName = FirstName_Edit_->text().toStdString();
    auto LastName = LastName_Edit_->text().toStdString();
    auto Postcode = PostCode_Edit_->text().toStdString();
    const auto& EnteredDate = DOB_DateEdit_->date();

    auto DateOfBirth
        = boost::posix_time::ptime
            (   boost::gregorian::date
                    ( EnteredDate.year(), EnteredDate.month(), EnteredDate.day() ),
                boost::posix_time::time_duration( 0, 0, 0 ) );

    setPatientID
        ( Schema_->get_patient_id
            ( FirstName, LastName, DateOfBirth, Postcode ) );

    if( PatientID_ )
    {
        PatientRetrieved_Label_->setText( "Patient successfully retrieved" );
        TryAgain_Label_->clear();
    }
    else
    {
        TryAgain_Label_->setText( "No match! Please try again" );
        PatientRetrieved_Label_->clear();
    }
}


