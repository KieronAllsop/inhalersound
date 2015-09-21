// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I

// Self Include
#include "qt_gui/mainwindow.h"

// inhaler Includes
#include "inhaler/server.hpp"
#include "inhaler/data_retriever.hpp"

// qt_gui Includes
#include "qt_gui/prompt/login.h"
#include "qt_gui/prompt/get_patient.h"
#include "qt_gui/view/explore_patient.h"

// Qt Includes
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QWidget>
#include <QStackedLayout>
#include <QHBoxLayout>
#include <QDesktopWidget>
#include <QApplication>
#include <QStyle>
#include <QPalette>

// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I

// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
namespace qt_gui {
// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n


//! \class  mainwindow.cpp
//! \author Kieron Allsop
//!
//! \brief  Main window of the system. Contains StackedWidgets to show the different
//!         views required in the system.
//!
MainWindow::MainWindow
(   const shared_server_t& Server,
    const shared_settings_t& Settings,
    QWidget* parent  )
: QMainWindow(parent)

, Server_
    ( Server )

, Settings_
    ( Settings )

, StackedLayout_
    ( new QStackedLayout() )

, LoginPrompt_
    ( new qt_gui::prompt::login
        (   Server_,
            [this]( const user_t& User, const shared_schema_t& Schema )
            {
                on_login( User, Schema );
            },
            this   ) )

, GetPatientPrompt_
    ( new qt_gui::prompt::get_patient
        (   [this]( const patient_t& Patient )
            {
                on_get_patient( Patient );
            },
            this   ) )

, ExplorePatientView_
    ( new qt_gui::view::explore_patient
        (   [this]()
            {
                on_leave_patient();
            },
            this   ) )
{
    // Initialise main_window

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
    Palette.setBrush( QPalette::Window, QColor( 0, 0, 0, 208 ) );
    Frame->setPalette( Palette );
    Frame->setAutoFillBackground( true );

    Prompt->setPalette( palette().color( QPalette::Background ) );
    Prompt->setAutoFillBackground( true );

    return Frame;
}


QWidget* MainWindow::
get_view_for( QFrame* View )
{
    View->setPalette( palette().color( QPalette::Background ) );
    View->setAutoFillBackground( true );

    return View;
}


void MainWindow::
on_login( const data_model::user& User, const shared_schema_t& Schema )
{
    Schema_ = Schema;
    GetPatientPrompt_->reset( std::make_shared<inhaler::patient_retriever>( Schema ) );

    StackedLayout_->setCurrentIndex( display_view::get_patient );
}


void MainWindow::
on_get_patient( const data_model::patient& Patient )
{
    auto DataRetriever = std::make_shared<inhaler::data_retriever>( Patient, Schema_ );
    ExplorePatientView_->reset( DataRetriever, Schema_, Settings_ );
    resize_window( 0.9 );
    StackedLayout_->setCurrentIndex( display_view::explore_patient );
}


void MainWindow::
on_leave_patient()
{
    GetPatientPrompt_->reset( std::make_shared<inhaler::patient_retriever>( Schema_ ) );

    StackedLayout_->setCurrentIndex( display_view::get_patient );
}


/// TODO: facilitate logout
void MainWindow::
on_logout()
{
//    Schema_.reset();
//    LoginPrompt_->reset();
//
//    StackedLayout_->setCurrentIndex( display_view::login );
}


void MainWindow::
resize_window( const double& Fraction )
{
    resize( QDesktopWidget().availableGeometry( this ).size() * Fraction );

    setGeometry
    (   QStyle::alignedRect
        (   Qt::LeftToRight,
            Qt::AlignCenter,
            size(),
            QApplication::desktop()->availableGeometry()  )  );
}


// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
} // end qt_gui
// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
