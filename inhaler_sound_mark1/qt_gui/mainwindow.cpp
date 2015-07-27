// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I
// Standard Library Includes
// none

// Qt Includes
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QWidget>
#include <QStackedLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QDesktopWidget>
#include <QApplication>
#include <QStyle>
#include <QPalette>

// Custom Includes
#include "inhaler/server.hpp"
#include "qt_gui/prompt/login.h"
#include "qt_gui/prompt/get_patient.h"
#include "qt_gui/play_wave.h"
#include "qt_gui/import_wizard/wizard.h"
#include "qt_gui/view/explore_patient.h"

// Self Include
#include "qt_gui/mainwindow.h"

// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I

// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
namespace qt_gui {
// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n


MainWindow::MainWindow
(   const shared_server_t& Server,
    QWidget* parent  )
  : QMainWindow(parent)

  , Server_             ( Server )
  , WaveImporter_       ( new inhaler::wave_importer( Server_->connect_to_schema() ) )
  , SignalComplete_     ( new application::signal_state_complete() )
  , StackedLayout_      ( new QStackedLayout() )
  , LoginPrompt_        ( new qt_gui::prompt::login( Server_, SignalComplete_, this ) )
  , GetPatientPrompt_   ( new qt_gui::prompt::get_patient( WaveImporter_, SignalComplete_, this ) )
  , ExplorePatientView_ ( new qt_gui::view::explore_patient( Server_, WaveImporter_, this ) )
//  , ImportWizard_       ( new qt_gui::import_wizard::wizard( Server_->connect_to_schema(), this ) )
//  , PlayWave_           ( new qt_gui::play_wave( Server_->connect_to_schema(), WaveImporter_, DataRetriever_, this ) )
{
    // Initialise main_window

//    setWindowFlags( Qt::FramelessWindowHint );
    setAttribute( Qt::WA_NoSystemBackground, true );
    setAttribute( Qt::WA_TranslucentBackground, true );
    setContentsMargins( 0, 0, 0, 0 );

    // Master Layout

    StackedLayout_->addWidget( get_prompt_for( LoginPrompt_ ) );
    StackedLayout_->addWidget( get_prompt_for( GetPatientPrompt_ ) );
    StackedLayout_->addWidget( get_view_for( ExplorePatientView_ ) );

    QWidget *widget = new QWidget();
    widget->setLayout(StackedLayout_);
    setCentralWidget(widget);

    setGeometry
    (   QStyle::alignedRect
        (   Qt::LeftToRight,
            Qt::AlignCenter,
            size(),
            QApplication::desktop()->availableGeometry()  )  );

    SignalComplete_->connect
        (   [&]( const application::state& State )
            {
                on_state_complete( State );
            }
        );

    LoginPrompt_->initialise_connection();
}


QWidget* MainWindow::
get_prompt_for( QFrame* Prompt )
{
    Prompt->setFrameStyle( QFrame::StyledPanel | QFrame::Plain );

    QHBoxLayout* Box = new QHBoxLayout();
    Box->addWidget( Prompt, 0, Qt::AlignCenter );

    QFrame* Frame = new QFrame( this );
    Frame->setLayout( Box );

    auto Palette = palette();
    Palette.setBrush( QPalette::Window, QColor(0, 0, 0, 208) );
    Frame->setPalette( Palette );
    Frame->setAutoFillBackground( true );

    Prompt->setPalette( palette().color(QPalette::Background) );
    Prompt->setAutoFillBackground(true);

    return Frame;
}


QWidget* MainWindow::
get_view_for( QFrame* View )
{
    View->setPalette( palette().color( QPalette::Background ) );
    View->setAutoFillBackground(true);

    return View;
}


void MainWindow::
on_state_complete( const application::state& State )
{
    using state_t = application::state;

    switch( State )
    {
        case state_t::login_as_data_technician :
        case state_t::login_as_diagnosing_doctor :
        {
            StackedLayout_->setCurrentIndex( display_view::get_patient );
            break;
        }
        case state_t::get_patient :
        {
            const data_model::patient& Patient = *(WaveImporter_->patient());
            ExplorePatientView_->populate_patient_form( Patient );
            StackedLayout_->setCurrentIndex( display_view::explore_patient );
            break;
        }
        default :
        {
            // TODO
            break;
        }
    }
}


// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
} // end qt_gui
// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
