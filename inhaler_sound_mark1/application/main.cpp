
// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I
// Standard Library Includes
#include <iostream>

// Boost Includes
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>

// Quince Includes
#include <quince_postgresql/database.h>

// Qt Includes
#include <QApplication>

// Custom Includes
#include "qt_windows/login.h"
#include "data_model/schema.hpp"

// Local Includes
#include "version.hpp"

// spdlog includes
#include "spdlog/spdlog.h"
// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I

void logger()
{
    auto log_dir = boost::filesystem::initial_path() / "logfiles";

    if( !exists( log_dir ) )
    {
        create_directories( log_dir );
    }

    auto ConsoleLog = spdlog::stdout_logger_mt( "console" );
    ConsoleLog->info( inhalersound::build::identity::product_version() );

    std::string basename = "logfiles/inhalersound_log";
    auto FileLogger = spdlog::rotating_logger_mt( "FileLogger", basename, 1024, 9, true );
    FileLogger->info( inhalersound::build::identity::report() );
}



int main( int argc, char *argv[] )
{

    boost::program_options::options_description OptionsDescription( "Program Options" );
    OptionsDescription.add_options()
    ( "help", "produce help message" )
    ( "version,v", "print version string" )
    ;

    boost::program_options::variables_map vm;
    boost::program_options::store( boost::program_options::parse_command_line( argc, argv, OptionsDescription ), vm );
    boost::program_options::notify( vm );

    if( vm.count( "help" ) )
    {
        std::cout << OptionsDescription << "\n";
        return 1;
    }

    if( vm.count( "version" ) )
    {
        std::cout << inhalersound::build::identity::report() << std::endl;
        return 1;
    }

    logger();

    QApplication GuiApplication( argc, argv );

    // TODO: Hard code this or make it possible to change?
    const quince_postgresql::database
        Database("localhost", "inhaler", "inhaler", "inhalersound", "inhalersound");

    // Create a shared instance of a single Schema
    auto Schema = std::make_shared<data_model::schema>( Database );

    // Pass the shared pointer to the Schema into our Login instance so it
    // can make use of it (Dependency Injection - preferred way to share state)
    Login window( Schema );

    window.show();

    window.initialise_server_connection();

    return GuiApplication.exec();
}
