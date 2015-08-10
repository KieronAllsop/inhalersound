// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I

// Self Include
#include "qt_gui/prompt/login.h"

// Qt Includes
#include <QStyle>
#include <QDesktopWidget>
#include <QApplication>
#include <QFormLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QCoreApplication>

// Asio Includes
#include <asio.hpp>
#include <asio/high_resolution_timer.hpp>

// Boost Library Includes
#include <boost/format.hpp>

// C++ Standard Library Includes
#include <chrono>

// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I

// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
namespace qt_gui {
namespace prompt {
// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n

login::
login
(   const shared_server_t& Server,
    const call_on_complete_t& CallOnComplete,
    QWidget* Parent )
: QFrame( Parent )

, Server_           ( Server )
, CallOnComplete_   ( CallOnComplete )
, Connected_        ( false )

// Create Widgets
, Title_Label_      ( new QLabel( tr("<h1>Please Login</h1>"), this ) )
, Status_Label_     ( new QLabel( tr("Connecting to server..."), this ) )
, Username_Edit_    ( new QLineEdit( this ) )
, Password_Edit_    ( new QLineEdit( this ) )
, Login_Button_     ( new QPushButton( tr("Login"), this ) )

{
    // Set up event handling
    connect( Login_Button_,  &QPushButton::released,  [this](){ on_login_clicked(); } );
    connect( Username_Edit_, &QLineEdit::textChanged, [this]( const QString& Text ){ on_credentials_changed( Text ); } );
    connect( Password_Edit_, &QLineEdit::textChanged, [this]( const QString& Text ){ on_credentials_changed( Text ); } );


    // Initialise State
    Login_Button_->setDefault( true );   // Handle keyboard Enter
    Login_Button_->setEnabled( false );  // Disabled by default

    Status_Label_->setFrameStyle( QFrame::StyledPanel | QFrame::Sunken );
    Status_Label_->setAlignment( Qt::AlignCenter );

    Password_Edit_->setEchoMode( QLineEdit::Password );

    // Initialise Layout

    // Master Layout is a Vertical Box Layout
    QVBoxLayout* MasterLayout = new QVBoxLayout();

    MasterLayout->addWidget( Title_Label_, 0, Qt::AlignCenter );

    QFormLayout* CredentialsForm = new QFormLayout();

    CredentialsForm->addRow( tr("&Username:"), Username_Edit_ );
    CredentialsForm->addRow( tr("&Password:"), Password_Edit_ );

    MasterLayout->addLayout( CredentialsForm );

    MasterLayout->addWidget( Status_Label_, 0 );
    MasterLayout->addWidget( Login_Button_, 0, Qt::AlignCenter );

    setLayout( MasterLayout );

    // Set the size of the window based on the new layout
    // and the widgets it contains
    adjustSize();
}

login::
~login()
{
    if( RetryThread_.joinable() )
    {
        IoContext_.stop();
        RetryThread_.join();
    }
}


bool login::
event( QEvent* Event )
{
    if( Event->type() == connection_status_event::type() )
    {
        if( auto* StatusEvent = dynamic_cast<connection_status_event*>( Event ) )
        {
            on_connection_status( StatusEvent->schema(), StatusEvent->connected(), StatusEvent->retrying(), StatusEvent->attempt() );
            return true;
        }
    }
    return QWidget::event( Event );
}


void login::
on_connection_status
(   const shared_schema_t& Schema,
    bool Connected,
    bool Retrying,
    int  Attempt   )
{
    if( Connected )
    {
        Schema_ = Schema;
        Status_Label_->setText("Connected to Server");
        Connected_ = true;
        update_login_state();
    }
    else if( Retrying )
    {
        Status_Label_
            ->setText
                    (   tr( "Attempt [%1] failed to connect. Trying again..." )
                        .arg( Attempt )   );
    }
    else
    {
        Status_Label_->setText("ERROR: Cannot connect to server");
    }
}


void login::
handle_wait
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


bool login::
connect_to_server( bool Retrying, int Attempt )
{
    try
    {
        auto Schema = Server_->connect_to_schema();
        QCoreApplication::postEvent( this, new connection_status_event( Schema, true, false, Attempt ) );
        return true;
    }
    catch( const inhaler::exception::could_not_connect_to_server& Error )
    {
        QCoreApplication::postEvent( this, new connection_status_event( shared_schema_t(), false, Retrying, Attempt ) );
    }
    return false;
}


void login::
initialise_connection()
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


void login::
on_credentials_changed( const QString& Text )
{
    update_login_state();
}


void login::
update_login_state()
{
    Login_Button_
        ->setEnabled
            (       Connected_
                &&  Username_Edit_->text().size()
                &&  Password_Edit_->text().size()   );
}


void login::
on_login_clicked()
{
    auto Username = Username_Edit_->text().toStdString();
    auto Password = Password_Edit_->text().toStdString();

    auto User = Server_->authenticate( Username, Password );

    if( User )
    {
        CallOnComplete_( *User, Schema_ );
    }
    else
    {
        Status_Label_->setText("ERROR: Username or Password Incorrect. Try again.");
        Username_Edit_->setFocus();
    }
}

// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
} // end prompt
} // end qt_gui
// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
