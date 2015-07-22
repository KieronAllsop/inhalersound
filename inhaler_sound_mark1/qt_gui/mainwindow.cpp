// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I
// Standard Library Includes
// none

// Qt Includes
#include <QStackedWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>
#include <QLabel>

// Header Includes
#include "mainwindow.h"
// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)

  , Label_  ( new QLabel    ( " Label ") )
  , Button_ ( new QPushButton ( "Button") )
{

    QVBoxLayout *mainWindow = new QVBoxLayout();
    mainWindow->addWidget(Button_);
    mainWindow->addWidget(Label_);

    QWidget *widget = new QWidget();
    widget->setLayout(mainWindow);
    setCentralWidget(widget);
}
