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

// Custom Includes
#include "inhaler/server.hpp"
#include "qt_gui/login.h"
#include "qt_gui/play_wave.h"

// Header Includes
#include "qt_gui/mainwindow.h"

// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I

// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
namespace qt_gui {
// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)

  , ExplanationLabel_   ( new QLabel        ( "Welcome to the Inhaler Sound analyiser ", this ) )
  , LoginLabel_         ( new QLabel        ( "PLease select Login or Register", this ) )
  , DataLabel_          ( new QLabel        ( "You can choose to import data or play a wave file", this ) )
  , LoginButton_        ( new QPushButton   ( "Login", this ) )
  , RegisterButton_     ( new QPushButton   ( "Register (play wave)", this ) )
  , ImportWizButton_    ( new QPushButton   ( "Open Importation Wizard", this ) )
  , PlayWaveButton_     ( new QPushButton   ( "Play Wave File", this ) )
  , StackedLayout_      ( new QStackedLayout( ) )

{
    //RegisterButton_->setEnabled( false );
    resize(QDesktopWidget().availableGeometry(this).size() * 0.8);

    // experiment time!
    auto Server = std::make_shared<inhaler::server>();
    qt_gui::login_dialog* LoginWindow = new qt_gui::login_dialog( Server );
    //qt_gui::login_dialog LoginWindow( Server );
    LoginWindow->initialise_connection();






    PlayWave* playwave = new PlayWave(Importer_,this);

    connect( LoginButton_, SIGNAL( released() ),   this, SLOT( on_login_clicked() ) );
    connect( RegisterButton_, SIGNAL( released()), this, SLOT( on_register_clicked())) ;

    QVBoxLayout* MasterLayout = new QVBoxLayout();
    MasterLayout->addWidget( ExplanationLabel_ );

// Login or Register stack -------------------------------------
    QVBoxLayout* VLoginSplit = new QVBoxLayout();
    VLoginSplit->addWidget( LoginButton_ );
    VLoginSplit->addWidget( RegisterButton_ );

    QHBoxLayout* HLoginSplit = new QHBoxLayout();
    HLoginSplit->addWidget( LoginLabel_ );
    HLoginSplit->addLayout( VLoginSplit );

    QGroupBox* LoginLayout = new QGroupBox();
    LoginLayout->setLayout( HLoginSplit );

// DataTechician landing screen --------------------------------
    QVBoxLayout* VDataTechSplit = new QVBoxLayout();
    VDataTechSplit->addWidget( ImportWizButton_ );
    VDataTechSplit->addWidget( PlayWaveButton_ );

    QHBoxLayout* HDataTechSplit = new QHBoxLayout();
    HDataTechSplit->addWidget( DataLabel_ );
    HDataTechSplit->addLayout( VDataTechSplit );

    QGroupBox* DataTechLayout = new QGroupBox();
    DataTechLayout->setLayout( HDataTechSplit );


// Stacked Layout ----------------------------------------------
    StackedLayout_->addWidget(LoginWindow);
    StackedLayout_->addWidget(DataTechLayout);
    StackedLayout_->addWidget(playwave);

    MasterLayout->addLayout(StackedLayout_);
    QWidget *widget = new QWidget();
    widget->setLayout(MasterLayout);
    setCentralWidget(widget);
}

void MainWindow::
on_register_clicked()
{
    StackedLayout_->setCurrentIndex(2);
}

void MainWindow::
on_login_clicked()
{
    auto Server = std::make_shared<inhaler::server>();

    qt_gui::login_dialog LoginWindow( Server );
    //hide();
    LoginWindow.initialise_connection();
    LoginWindow.exec();

}

void MainWindow::
move_stack_to_datatech()
{
    StackedLayout_->setCurrentIndex(1);
    ExplanationLabel_->setText( "Data Technician Landing Page");
}

// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
} // end qt_gui
// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
