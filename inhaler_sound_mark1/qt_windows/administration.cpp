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
(   const shared_schema_t& Schema,
    QWidget *Parent) :
    QDialog(Parent),
    Ui_(new Ui::Administration),
    Schema_( Schema )
{
    Ui_->setupUi(this);
}

Administration::
~Administration()
{
    delete Ui_;
}
