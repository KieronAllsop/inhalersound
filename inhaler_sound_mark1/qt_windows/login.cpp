// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I
// Qt Includes
#include <QStyle>
#include <QDesktopWidget>
#include <QApplication>

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
(   const shared_schema_t& Schema,
    QWidget* Parent ) :
    QMainWindow( Parent ),
    Ui_( new Ui::Login ),
    Schema_( Schema )
{
    Ui_->setupUi( this );

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
    delete Ui_;
}


void Login::on_pushButton_clicked()
{
    QString Username, Password;
    Username=Ui_->lineEdit_username->text();
    Password=Ui_->lineEdit_password->text();

    bool ValidUser = false;
    std::string UserRole = "Invalid user";

    // this returns the complete tuple of user details from the database - this can then be used
    // if a user wishes to alter any login details, such as title, name, email
    auto UserDetails=
        Schema_->
            validate_user(
                ValidUser,
                UserRole,
                Username.toStdString(),
                Password.toStdString() );

    if( ValidUser )
    {
        if(UserRole == "DataTechnician" ||
               UserRole == "DiagnosingDoctor")
        {
            this->hide();
            PatientDetails patientDetails( Schema_, this);
            patientDetails.setModal(true);
            patientDetails.exec();
        }
        else
        {
            this->hide();
            Administration administration( Schema_, this);
            administration.setModal(true);
            administration.exec();
        }
   }
   else
   {
        Ui_->label->setText(QString::fromStdString(UserRole));
   }

}

void Login::initialise_server_connection()
{
    bool Connected = false;

    do
    {
        try
        {
           Connected = Schema_->open_all_tables();
           Schema_->initial_population();
        }
        catch( const quince::failed_connection_exception& Error )
        {
           Ui_->label->setText(Error.what());
           std::this_thread::sleep_for ( std::chrono::seconds(5) );
           Ui_->label->setText("Trying again...");
        }

    }
    while( !Connected );
}
