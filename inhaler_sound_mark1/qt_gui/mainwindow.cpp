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
#include "qt_gui/import_wizard/wizard.h"

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
  , ExplanationLabel_   ( new QLabel        ( "Welcome to the Inhaler Sound analyiser ", this ) )
  , DataLabel_          ( new QLabel        ( "You can choose to import data or play a wave file", this ) )
  , ImportWizardButton_ ( new QPushButton   ( "Open Importation Wizard", this ) )
  , PlayWaveButton_     ( new QPushButton   ( "Play Wave File", this ) )
  , StackedLayout_      ( new QStackedLayout() )
  , LoginPrompt_        ( new qt_gui::login_dialog( Server_, this ) )
  , ImportWizard_       ( new qt_gui::import_wizard::wizard( Server_->connect_to_schema(), this ) )
  , PlayWave_           ( new qt_gui::play_wave( Schema_, WaveImporter_, DataRetriever_, this ) )

{
    //RegisterButton_->setEnabled( false );
    resize(QDesktopWidget().availableGeometry(this).size() * 0.5);

    connect( ImportWizardButton_,   SIGNAL( released() ),       this,  SLOT( move_stack_importwiz() ) );
    connect( ImportWizard_,         SIGNAL( finished(int) ),    this,  SLOT( import_wizard_finished(int) ) );
    connect( PlayWaveButton_,       SIGNAL( released() ),       this,  SLOT( move_stack_playwave() ) );

    // Master Layout -----------------------------------------------
    QVBoxLayout* MasterLayout = new QVBoxLayout();
    MasterLayout->addWidget( ExplanationLabel_ );

    // Stack Index 0 - Login ---------------------------------------

    //

    LoginPrompt_->initialise_connection();

    connect( LoginPrompt_, SIGNAL( change_stacked_layout_index() ), this, SLOT( move_stack_datatech() ) );



    // Stack Index 1 - DataTechician landing screen ----------------
    QVBoxLayout* VDataTechSplit = new QVBoxLayout();
    VDataTechSplit->addWidget( ImportWizardButton_ );
    VDataTechSplit->addWidget( PlayWaveButton_ );

    QHBoxLayout* HDataTechSplit = new QHBoxLayout();
    HDataTechSplit->addWidget( DataLabel_ );
    HDataTechSplit->addLayout( VDataTechSplit );

    QGroupBox* DataTechLayout = new QGroupBox();
    DataTechLayout->setLayout( HDataTechSplit );

    // Stack Index 3 - Play Wave Files -----------------------------
    //PlayWave* playwave = new PlayWave(Importer_,this);

    // Stacked Layout ----------------------------------------------
    StackedLayout_->addWidget( LoginPrompt_ );
    StackedLayout_->addWidget( DataTechLayout );
    StackedLayout_->addWidget( ImportWizard_ );
    StackedLayout_->addWidget( PlayWave_ );

    MasterLayout->addLayout(StackedLayout_);
    QWidget *widget = new QWidget();
    widget->setLayout(MasterLayout);
    setCentralWidget(widget);
}

void MainWindow::
move_stack_datatech()
{
    StackedLayout_->setCurrentIndex(1);
    ExplanationLabel_->setText( "Data Technician Landing Page");
}

void MainWindow::
move_stack_importwiz()
{
    StackedLayout_->setCurrentIndex(2);
    ExplanationLabel_->setText( "Importation wizard");
}

void MainWindow::
import_wizard_finished( int Result )
{
    // Result = 0 if Cancelled
    // Result = 1 if Finished
    StackedLayout_->setCurrentIndex(1);
    ExplanationLabel_->setText( "Data Technician Landing Page");
}


void MainWindow::
move_stack_playwave()
{
    StackedLayout_->setCurrentIndex(3);
    ExplanationLabel_->setText( "Play Wave files");
}

// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
} // end qt_gui
// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
