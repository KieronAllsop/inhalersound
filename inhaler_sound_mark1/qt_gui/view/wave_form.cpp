// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I
// Self Include
#include "qt_gui/view/wave_form.h"

// QT Includes
#include "QPainter"

// Standard Library Includes
#include <stdio.h>

// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I


// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
namespace qt_gui {
namespace view {
// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n

wave_form::
wave_form
(   QWidget* Parent   )
: QWidget( Parent )
{
}


void wave_form::
reset( const shared_data_t& Data )
{
//     setUpdatesEnabled( false );
    Data_ = Data;
//     setUpdatesEnabled( true )
}


void wave_form::
paintEvent( QPaintEvent* Event )
{
    QPainter painter(this);


    painter.fillRect(rect(), Qt::black);


}


// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
} // end view
} // end qt_gui
// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
