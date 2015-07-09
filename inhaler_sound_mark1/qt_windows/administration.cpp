// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I
// Standard Includes
// none

// Header Include
#include "qt_windows/administration.h"

// Qt UI Generated Include
#include "qt_windows/ui_administration.h"
// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I


Administration::
Administration
(   const shared_schema_t& schema,
    QWidget *parent) :
    QDialog(parent),
    ui_(new Ui::Administration),
    schema_( schema )
{
    ui_->setupUi(this);
}

Administration::
~Administration()
{
    delete ui_;
}
