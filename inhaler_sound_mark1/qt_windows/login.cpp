// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I
// Standard Library Includes
#include <chrono>

// Boost Library Includes
#include <boost/format.hpp>

// Asio Includes
#include <asio.hpp>
#include <asio/high_resolution_timer.hpp>

// Qt Includes
#include <QStyle>
#include <QDesktopWidget>
#include <QApplication>
#include <QFormLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>

// Qt Window Includes
#include "qt_windows/patientdetails.h"
#include "qt_windows/administration.h"

// Self Include
#include "qt_windows/login.h"

// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I


Login::
Login
(   const shared_server_t& Server,
    QWidget* Parent )
: QDialog( Parent )

, Server_   ( Server )
, Connected_( false )

// Create Widgets
, Title_Label_      ( new QLabel( "Login to Inhalersound", this ) )
, Status_Label_     ( new QLabel( "Cconnecting to server...", this ) )
, Username_Edit_    ( new QLineEdit( this ) )
, Password_Edit_    ( new QLineEdit( this ) )
, Login_Button_     ( new QPushButton( "Login", this ) )

{
    // Set up event handling
    connect( Login_Button_,  SIGNAL( released() ), this, SLOT( on_login_clicked() ) );
    connect( Username_Edit_, SIGNAL( textChanged(const QString&) ), this, SLOT( on_credentials_changed(const QString&) ) );
    connect( Password_Edit_, SIGNAL( textChanged(const QString&) ), this, SLOT( on_credentials_changed(const QString&) ) );

    // Initialise State
    Login_Button_->setDefault( true );   // Handle keyboard Enter
    Login_Button_->setEnabled( false );  // Disabled by default

    Status_Label_->setFrameStyle( QFrame::Panel | QFrame::Sunken );
    Status_Label_->setAlignment( Qt::AlignCenter );

    // Initialise Layout

    // Master Layout is a Vertical Box Layout
    QVBoxLayout* MasterLayout = new QVBoxLayout();

    MasterLayout->addWidget( Title_Label_, 0, Qt::AlignLeft );

    QFormLayout* CredentialsForm = new QFormLayout();

    CredentialsForm->addRow( "&Username:", Username_Edit_ );
    CredentialsForm->addRow( "&Password:", Password_Edit_ );

    MasterLayout->addLayout( CredentialsForm );

    MasterLayout->addWidget( Status_Label_, 0 );
    MasterLayout->addWidget( Login_Button_, 0, Qt::AlignCenter );

    setLayout( MasterLayout );

    // Set the size of the window based on the new layout
    // and the widgets it contains
    adjustSize();

    // Centre the window
    setGeometry
    (   QStyle::alignedRect
        (   Qt::LeftToRight,
            Qt::AlignCenter,
            size(),
            QApplication::desktop()->availableGeometry()  )  );
}

Login::
~Login()
{
    if( RetryThread_.joinable() )
    {
        IoContext_.stop();
        RetryThread_.join();
    }
}


void Login::handle_wait
(   const asio::error_code&     Error,
    int                         Count,
    int                         MaxRetries,
    std::chrono::microseconds   Interval,
    shared_timer_t&             Timer   )
{
    if( !Error )
    {
        bool Retry = ++Count <= MaxRetries;

        bool Connected = connect_to_server( Retry, Count );

        if( !Connected && Retry )
        {
            Timer->expires_after( Interval );
            Timer->async_wait
            (   [ this, Count, MaxRetries, Interval, Timer=Timer ]
                    ( const asio::error_code& Error ) mutable
                {
                    handle_wait( Error, Count, MaxRetries, Interval, Timer );
                }
            );
        }
    }
}


bool Login::connect_to_server( bool Retry, int Attempt )
{
    try
    {
        Schema_ = Server_->connect_to_schema();
        Status_Label_->setText("Connected to Server");
        Connected_ = true;
        update_login_state();
        return true;
    }
    catch( const boost::exception& Error )
    {
        if( Retry )
        {
            QString Message
                = QString::fromUtf8
                    ( str( boost::format("Attempt [%s] failed to connect. Trying again...") % Attempt ).c_str() );

            Status_Label_->setText( Message );
        }
        else
        {
            Status_Label_->setText("ERROR: Cannot connect to server");
        }
    }
    return false;
}


void Login::initialise_connection()
{
    int Count = 1;

    if( !connect_to_server( true, Count ) )
    {
        auto Timer     = std::make_shared<timer_t>( IoContext_ );
        auto Interval  = std::chrono::seconds(5);
        int MaxRetries = 2;

        Timer->expires_after( Interval );
        Timer->async_wait
        (   [ this, Count, MaxRetries, Interval, Timer=Timer ]
                ( const asio::error_code& Error ) mutable
            {
                handle_wait( Error, Count, MaxRetries, Interval, Timer );
            }
        );

        RetryThread_ = std::thread( [&](){ IoContext_.run(); } );
    }
}


void Login::on_credentials_changed( const QString& Text )
{
    update_login_state();
}


void Login::update_login_state()
{
    Login_Button_
        ->setEnabled
            (       Connected_
                &&  Username_Edit_->text().size()
                &&  Password_Edit_->text().size()   );
}


void Login::on_login_clicked()
{
    auto Username = Username_Edit_->text().toStdString();
    auto Password = Password_Edit_->text().toStdString();

    auto User = Server_->authenticate( Username, Password );

    if( User )
    {
        if(     User->user_role == "DataTechnician"
            ||  User->user_role == "DiagnosingDoctor" )
        {
            hide();
            PatientDetails patientDetails( Schema_, this );
            patientDetails.setModal( true );
            patientDetails.exec();
        }
        else
        {
            hide();
            Administration administration( Schema_, this );
            administration.setModal( true );
            administration.exec();
        }
   }
   else
   {
        Status_Label_->setText("ERROR: Username or Password Incorrect. Try again.");
        Username_Edit_->setFocus();
   }
}
