// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I

// Qt Includes
#include <QApplication>

// Quince Includes
#include <quince_postgresql/database.h>

// Custom Includes
#include "qt_windows/mainwindow.h"
#include "data_model/schema.hpp"

// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I


int main( int argc, char *argv[] )
{
    QApplication GuiApplication( argc, argv );
    
    // TODO: Decide if you want to hard code this or make it possible to change
    const quince_postgresql::database 
        Database("localhost", "inhaler", "inhaler", "inhaler", "inhalersound");
    
    // Create a shared instance of a single Schema
    auto Schema = std::make_shared<data_model::schema>( Database );
    
    // Pass the shared pointer to the Schema into our MainWindow instance so it
    // can make use of it (Dependency Injection - preferred way to share state)
    MainWindow window( Schema );

    window.show();

    return GuiApplication.exec();
}
