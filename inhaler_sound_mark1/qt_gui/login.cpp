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
#include <QCoreApplication>

// Qt Window Includes
#include "qt_gui/import_wizard/wizard.h"
#include "qt_gui/administration.h"

// Self Include
#include "qt_gui/login.h"

// Custom Includes
#include "qt_gui/mainwindow.h"

// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I

// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
namespace qt_gui {
// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n

login_dialog::
login_dialog
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

login_dialog::
~login_dialog()
{
    if( RetryThread_.joinable() )
    {
        IoContext_.stop();
        RetryThread_.join();
    }
}


bool login_dialog::
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
    return QDialog::event( Event );
}


void login_dialog::
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


void login_dialog::
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


bool login_dialog::
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


void login_dialog::
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


void login_dialog::
on_credentials_changed( const QString& Text )
{
    update_login_state();
}


void login_dialog::
update_login_state()
{
    Login_Button_
        ->setEnabled
            (       Connected_
                &&  Username_Edit_->text().size()
                &&  Password_Edit_->text().size()   );
}


void login_dialog::
on_login_clicked()
{
    auto Username = Username_Edit_->text().toStdString();
    auto Password = Password_Edit_->text().toStdString();

    auto User = Server_->authenticate( Username, Password );

    if( User )
    {
        if(     User->user_role == "DataTechnician"
            ||  User->user_role == "DiagnosingDoctor" )
        {
           // hide();
           // qt_gui::import_wizard::wizard ImportWizard( Schema_, this );
           // ImportWizard.setModal( true );
           // ImportWizard.exec();
           hide();
           qt_gui::MainWindow mainWindow;
           mainWindow.move_stack_to_datatech();
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

// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
} // end qt_gui
// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
