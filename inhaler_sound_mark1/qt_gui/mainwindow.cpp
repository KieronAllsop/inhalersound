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

// Header Includes
#include "qt_gui/mainwindow.h"

// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I

// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
namespace qt_gui {
// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n


MainWindow::MainWindow
(
    QWidget *parent) :
    QMainWindow(parent)


  , ExplanationLabel_   ( new QLabel        ( "Welcome to the Inhaler Sound analyiser ", this ) )
  , DataLabel_          ( new QLabel        ( "You can choose to import data or play a wave file", this ) )
  , ImportWizButton_    ( new QPushButton   ( "Open Importation Wizard", this ) )
  , PlayWaveButton_     ( new QPushButton   ( "Play Wave File", this ) )
  , StackedLayout_      ( new QStackedLayout( ) )

{
    //RegisterButton_->setEnabled( false );
    resize(QDesktopWidget().availableGeometry(this).size() * 0.8);

// Master Layout -----------------------------------------------
    QVBoxLayout* MasterLayout = new QVBoxLayout();
    MasterLayout->addWidget( ExplanationLabel_ );


// Stack Index 0 - Login ---------------------------------------
    auto Server = std::make_shared<inhaler::server>();
    qt_gui::login_dialog* LoginWindow = new qt_gui::login_dialog( Server );
    LoginWindow->initialise_connection();




    PlayWave* playwave = new PlayWave(Importer_,this);


    connect( LoginWindow, SIGNAL(change_stacked_layout_index(int)), this, SLOT(move_stack(int)) );


// Stack Index 1 - DataTechician landing screen ----------------
    QVBoxLayout* VDataTechSplit = new QVBoxLayout();
    VDataTechSplit->addWidget( ImportWizButton_ );
    VDataTechSplit->addWidget( PlayWaveButton_ );

    QHBoxLayout* HDataTechSplit = new QHBoxLayout();
    HDataTechSplit->addWidget( DataLabel_ );
    HDataTechSplit->addLayout( VDataTechSplit );

    QGroupBox* DataTechLayout = new QGroupBox();
    DataTechLayout->setLayout( HDataTechSplit );

// Stack Index 2 - Import Files Wizard -------------------------

    //auto WaveImporter = std::make_shared<inhaler::wave_importer>( Schema_ );
    qt_gui::import_wizard::wizard* ImportWizard = new qt_gui::import_wizard::wizard( Schema_ );


// Stacked Layout ----------------------------------------------
    StackedLayout_->addWidget(LoginWindow);
    StackedLayout_->addWidget(DataTechLayout);
    StackedLayout_->addWidget(ImportWizard);
    StackedLayout_->addWidget(playwave);

    MasterLayout->addLayout(StackedLayout_);
    QWidget *widget = new QWidget();
    widget->setLayout(MasterLayout);
    setCentralWidget(widget);
}

void MainWindow::
move_stack(int Index)
{
    switch( Index )
    {
        case 1:
            StackedLayout_->setCurrentIndex(1);
            ExplanationLabel_->setText( "Data Technician Landing Page");
    }
}

// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
} // end qt_gui
// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
