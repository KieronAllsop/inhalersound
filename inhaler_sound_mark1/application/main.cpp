// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I

// Local Includes
#include "version.hpp"

// qt_gui Includes
#include "qt_gui/mainwindow.h"

// inhaler Includes
#include "inhaler/server.hpp"

// analysis Includes
#include "analysis/speech_spectra_settings.hpp"

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


//! \abstract   Application to facilitate the assessment of inhaler technique
//!             by using automatic sound recognition.
//!
//! \author     Kieron Allsop... MSc Student 40142115
//! \date       September 2015
//! \version    01.00
//!


//! \brief      Initialises the Logger. Sets parameters needed by the spdlog
//!             library, such as location, filename and log rotation.
//!
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


//! \brief      Entry point of application. Sets up program options, creates
//!             the shared server and shared settings. Loads first gui class.
//!
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
        return 0;
    }

    if( vm.count( "version" ) )
    {
        std::cout << inhalersound::build::identity::report() << std::endl;
        return 0;
    }

    initialise_logger();

    QApplication GuiApplication( argc, argv );

    auto Server = std::make_shared<inhaler::server>();
    auto Settings = analysis::speech_spectra_settings::create_from_file();

    qt_gui::MainWindow mainWindow( Server, Settings );
    mainWindow.show();

    return GuiApplication.exec();
}
