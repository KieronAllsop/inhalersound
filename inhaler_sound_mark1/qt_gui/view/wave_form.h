// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G
#ifndef QT_GUI_VIEW_WAVE_FORM_H_INCLUDED
#define QT_GUI_VIEW_WAVE_FORM_H_INCLUDED
// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G

// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I

// qt::audio Includes
#include "qt/audio/raw_data.hpp"

// QT Includes
#include <QWidget>

// C++ Standard Library Includes
#include <vector>
#include <memory>

// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I


// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
namespace qt_gui {
namespace view {
// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n


class wave_form : public QWidget
{
    Q_OBJECT

public:

    using data_t        = qt::audio::raw_data;
    using shared_data_t = std::shared_ptr<data_t>;

public:

    explicit                wave_form                   (   QWidget* Parent = 0   );

    void                    reset                       (   const shared_data_t& Data   );

    void                    paintEvent                  (   QPaintEvent* Event   );

private:




private:

    shared_data_t Data_;

};

// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
} // end view
} // end qt_gui
// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n


// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G
#endif // QT_GUI_VIEW_WAVE_FORM_H_INCLUDED
