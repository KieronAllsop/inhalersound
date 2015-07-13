// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G
#ifndef QT_WINDOWS_LOGIN_HPP_INCLUDED
#define QT_WINDOWS_LOGIN_HPP_INCLUDED
// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G

// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I
// Standard Library Includes
#include <memory>

// Qt Includes
#include <QMainWindow>

// Custom Includes
#include "data_model/schema.hpp"
#include "qt_windows/patientdetails.h"
#include "qt_windows/administration.h"
// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I


// This is only needed because we are using a "ui" file to load information
// about the MainWindow when we call setupUi( this ) in the constructor. This
// is a forward declaration for a class that will be generated from processing
// the associated ui file by running "uic"
namespace Ui 
{
    class Login;
}    
    
class Login : public QMainWindow
{
    Q_OBJECT

public:

    void initialise_server_connection();

    using shared_schema_t = std::shared_ptr<data_model::schema>;
       
    explicit Login ( const shared_schema_t& Schema,
                          QWidget* Parent = 0 );
    
             ~Login();

    
private slots:
    void on_pushButton_clicked();

private:
    
    Ui::Login* Ui_;
    shared_schema_t Schema_;
};

// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G
#endif // QT_WINDOWS_LOGIN_HPP_INCLUDED
