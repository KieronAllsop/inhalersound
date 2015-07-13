// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I
// Qt Includes
// None

// Standard includes
#include <thread>
#include <chrono>

// Header Include
#include "qt_windows/login.h"
#include "qt_windows/patientdetails.h"
#include "qt_windows/administration.h"

// Qt UI Generated Include
#include "qt_windows/ui_login.h"
// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I


Login::
Login
(   const shared_schema_t& schema,
    QWidget* parent ) :
    QMainWindow( parent ),
    ui_( new Ui::Login ),
    schema_( schema )
{
    ui_->setupUi( this );
}

Login::
~Login()
{
    delete ui_;
}


void Login::on_pushButton_clicked()
{
    QString Username, Password;
    Username=ui_->lineEdit_username->text();
    Password=ui_->lineEdit_password->text();

    bool ValidUser = false;
    std::string UserRole = "Invalid user";

    // this returns the complete tuple of user details from the database - this can then be used
    // if a user wishes to alter any login details, such as title, name, email
    auto UserDetails=
            schema_->
            validate_user(
                ValidUser,
                UserRole,
                Username.toStdString(),
                Password.toStdString() );

    if( ValidUser )
    {
        if (UserRole == "DataTechnician" ||
                UserRole == "DiagnosingDoctor")
        {
            this->hide();
            PatientDetails patientDetails( schema_, this);
            patientDetails.setModal(true);
            patientDetails.exec();
        }
        else
        {
            this->hide();
            Administration administration( schema_, this);
            administration.setModal(true);
            administration.exec();
        }
   }
   else
   {
        ui_->label->setText(QString::fromStdString(UserRole));
   }

}

void Login::initialise_server_connection()
{
    bool Connected = false;

    do
    {
        try
        {
           schema_->open_all_tables( Connected );
           schema_->initial_population();
        }
        catch( const quince::failed_connection_exception& Error )
        {
           ui_->label->setText(Error.what());
           std::this_thread::sleep_for ( std::chrono::seconds(5) );
           ui_->label->setText("Trying again...");
        }

    }
    while( !Connected );
}
