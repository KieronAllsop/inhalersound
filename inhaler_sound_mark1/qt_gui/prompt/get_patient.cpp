// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I
// Standard Includes
// None

// Boost Includes
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>

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

// Importer Includes
#include "inhaler/patient_retriever.hpp"

// Self Include
#include "qt_gui/prompt/get_patient.h"
// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I

// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
namespace qt_gui {
namespace prompt {
// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n

get_patient::
get_patient
(   const call_on_complete_t& CallOnComplete,
    QWidget* Parent )

: QFrame( Parent )

, CallOnComplete_   ( CallOnComplete )

// Create Widgets
, Title_Label_                  ( new QLabel( tr("<h1>Please Identify the Patient</h1>"), this ) )
, FirstName_Label_              ( new QLabel( tr("First name"), this ) )
, LastName_Label_               ( new QLabel( tr("Last name"), this ) )
, DOB_Label_                    ( new QLabel( tr("Date of birth"), this ) )
, Postcode_Label_               ( new QLabel( tr("Postcode"), this ) )
, Status_Label_                 ( new QLabel( "", this ) )
, FirstName_Edit_               ( new QLineEdit( this ) )
, LastName_Edit_                ( new QLineEdit( this ) )
, PostCode_Edit_                ( new QLineEdit( this ) )
, Calendar_Widget_              ( new QCalendarWidget( this ) )
, DOB_DateEdit_                 ( new QDateEdit( QDate::currentDate() ) )
, RetrievePatient_Button_       ( new QPushButton( tr("Retrieve"), this ) )
, Finish_Button_                ( new QPushButton( tr("Done"), this ) )
{
//    setTitle( "Select Patient to associate files with" );

    // Set up event handling
    connect( RetrievePatient_Button_,   SIGNAL( released() ),                   this, SLOT( on_retrieve_clicked() ) );
    connect( Finish_Button_,            SIGNAL( released() ),                   this, SLOT( on_finished_clicked() ) );
    connect( FirstName_Edit_,           SIGNAL( textChanged(const QString&) ),  this, SLOT( on_text_credentials_changed(const QString&) ) );
    connect( LastName_Edit_,            SIGNAL( textChanged(const QString&) ),  this, SLOT( on_text_credentials_changed(const QString&) ) );
    connect( DOB_DateEdit_,             SIGNAL( dateChanged(const QDate&) ),    this, SLOT( on_date_credentials_changed(const QDate&) ) );
    connect( PostCode_Edit_,            SIGNAL( textChanged(const QString&) ),  this, SLOT( on_text_credentials_changed(const QString&) ) );

    // Initialise State
    RetrievePatient_Button_->setDefault( true );   // Handle keyboard enter
    RetrievePatient_Button_->setEnabled( false );  // Disabled by default

    Finish_Button_->setEnabled( false );     // Disabled by default

    Calendar_Widget_->setVisible( false );
    DOB_DateEdit_->setCalendarPopup( Calendar_Widget_ );
    DOB_DateEdit_->setMinimumDate( QDate::currentDate().addYears(-120) );
    DOB_DateEdit_->setMaximumDate( QDate::currentDate().addYears(1) );
    DOB_DateEdit_->setDisplayFormat( "dd-MMM-yyyy" );

    // Initialise Layout

    // Master Layout is a Vertical Box Layout
    QVBoxLayout* MasterLayout = new QVBoxLayout;

    QHBoxLayout* StatusLayout = new QHBoxLayout;
    StatusLayout->addWidget( Status_Label_, 0, Qt::AlignCenter );

    QFormLayout* CredentialsForm = new QFormLayout;
    CredentialsForm->addRow( FirstName_Label_, FirstName_Edit_ );
    CredentialsForm->addRow( LastName_Label_, LastName_Edit_ );
    CredentialsForm->addRow( DOB_Label_, DOB_DateEdit_ );
    CredentialsForm->addRow( Postcode_Label_, PostCode_Edit_ );

    QHBoxLayout* ButtonsLayout = new QHBoxLayout;
    ButtonsLayout->addWidget( RetrievePatient_Button_ );
    ButtonsLayout->addWidget( Finish_Button_ );

    MasterLayout->addWidget( Title_Label_, 0, Qt::AlignCenter );
    MasterLayout->addLayout( CredentialsForm );
    MasterLayout->addLayout( StatusLayout );
    MasterLayout->addLayout( ButtonsLayout );

    setLayout( MasterLayout );

    adjustSize();
}


void get_patient::reset( const patient_retriever_t& Retriever )
{
    Retriever_ = Retriever;
}


void get_patient::on_text_credentials_changed( const QString& Text )
{
    update_retrieval_state();
}


void get_patient::on_date_credentials_changed( const QDate& Date )
{
    if( Date != QDate::currentDate() )
    {
        DateChanged_ = true;
        update_retrieval_state();
    }
}


void get_patient::update_retrieval_state()
{
    RetrievePatient_Button_
        ->setEnabled
            (       FirstName_Edit_->text().size()
                &&  LastName_Edit_->text().size()
                &&  PostCode_Edit_->text().size()
                &&  DateChanged_ );
}


void get_patient::on_retrieve_clicked()
{
    auto FirstName = FirstName_Edit_->text().toStdString();
    auto LastName =  LastName_Edit_ ->text().toStdString();
    auto Postcode =  PostCode_Edit_ ->text().toStdString();
    const auto& EnteredDate = DOB_DateEdit_->date();

    auto DateOfBirth
        = boost::posix_time::ptime
            (   boost::gregorian::date
                    ( EnteredDate.year(), EnteredDate.month(), EnteredDate.day() ),
                boost::posix_time::time_duration( 0, 0, 0 ) );

    Patient_ = Retriever_->get_patient( FirstName, LastName, DateOfBirth, Postcode );

    if( Patient_ )
    {
        Status_Label_->setText( tr("Patient successfully retrieved") );

        Finish_Button_->setEnabled( true );     // Disabled by default
        Finish_Button_->setDefault( true );     // Handle keyboard enter
    }
    else
    {
        Status_Label_->setText( tr("Patient not found. Please try again") );
    }
}


void get_patient::on_finished_clicked()
{
    CallOnComplete_( *Patient_ );
}


// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
} // end prompt
} // end qt_gui
// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n

