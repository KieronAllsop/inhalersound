// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I

// Local Includes
#include "version.hpp"

// Custom Includes
#include "inhaler/server.hpp"
#include "qt_gui/mainwindow.h"

// spdlog includes
#include "spdlog/spdlog.h"

// Qt Includes
#include <QApplication>
#include <QMainWindow>

// Boost Includes
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/detail/utf8_codecvt_facet.hpp>

// Standard Library Includes
#include <iostream>

// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I

void initialise_logger()
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
    // allows use of file names in a another language as C++ uses a char array for strings
    std::locale Utf8Locale( std::locale(), new boost::filesystem::detail::utf8_codecvt_facet );
    boost::filesystem::path::imbue( Utf8Locale );

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
        return 0; //was 1
    }

    if( vm.count( "version" ) )
    {
        std::cout << inhalersound::build::identity::report() << std::endl;
        return 0; // was 1
    }


    initialise_logger();

    QApplication GuiApplication( argc, argv );

    auto Server = std::make_shared<inhaler::server>();

    qt_gui::MainWindow mainWindow( Server );
    mainWindow.show();

    return GuiApplication.exec();
}
