// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I
// Qt Includes
// None

// Header Include
#include "qt_windows/login.h"
#include "qt_windows/patientdetails.h"

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
    schema_->open_all_tables();
    schema_->initial_population();
    ui_->setupUi( this );
}

Login::
~Login()
{
    delete ui_;
}


void Login::on_pushButton_clicked()
{
    QString username, password;
    username=ui_->lineEdit_username->text();
    password=ui_->lineEdit_password->text();

    bool valid_user = false;
    auto user_role = schema_->validate_user( valid_user, username.toStdString(), password.toStdString() );

    if( valid_user )
    {
         ui_->label->setText(QString::fromStdString(user_role));  // displays system admin user name on label
         if (user_role == "DataTechnician" || user_role == "DiagnosingDoctor")
         {
            this->hide();
            PatientDetails patientDetails( schema_, this);
            patientDetails.setModal(true);
            patientDetails.exec();
         }
         else
         {
             // System Admin bit goes here
         }
    }
    else
    {
         ui_->label->setText(QString::fromStdString(user_role));
    }

}