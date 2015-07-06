// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I
// Qt Includes
// None

// Header Include
#include "qt_windows/mainwindow.h"

// Qt UI Generated Include
#include "qt_windows/ui_mainwindow.h"
// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I


MainWindow::
MainWindow
(   const shared_schema_t& schema,
    QWidget* parent ) 
: QMainWindow( parent )
, ui_( new Ui::MainWindow )
, schema_( schema )
{
    schema_->open_all_tables();
    schema_->initial_population();
    ui_->setupUi( this );

}


MainWindow::
~MainWindow()
{
    delete ui_;
}


void MainWindow::on_pushButton_clicked()
{
    QString username, password;
    username=ui_->lineEdit_username->text();
    password=ui_->lineEdit_password->text();

    if(schema_->valid_login(username, password))
    {

    }

}
