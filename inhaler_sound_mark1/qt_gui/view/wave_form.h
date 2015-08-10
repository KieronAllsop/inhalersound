// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G
#ifndef QT_GUI_VIEW_WAVE_FORM_H_INCLUDED
#define QT_GUI_VIEW_WAVE_FORM_H_INCLUDED
// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G

// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I

// qt::audio Includes
#include "qt/audio/raw_data.hpp"

// QT Includes
#include <QWidget>
#include <QPixmap>

// C++ Standard Library Includes
#include <vector>
#include <memory>
#include <cstdint>
#include <chrono>

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

    void                    set_play_position           (   std::chrono::nanoseconds Position   );

    void                    resizeEvent                 (   QResizeEvent* Event   );

    void                    paintEvent                  (   QPaintEvent* Event   );

private:

    void                    paint_static_preview        (   int Width, int Height   );

private:

    struct preview_sample_t
    {
        double Min;
        double Max;
        double Mean;
        double StdDev;
    };

private:

    void                    reset_play_position         ();

    void                    create_preview_wave         ();

    std::size_t             preview_index               (   std::size_t Sample, std::size_t Channel   ) const;

    preview_sample_t&       preview_sample              (   std::size_t Sample, std::size_t Channel   );

    const preview_sample_t& preview_sample              (   std::size_t Sample, std::size_t Channel   ) const;

    std::size_t             preview_size                () const;

private:

    shared_data_t                   Data_;
    std::vector<preview_sample_t>   Preview_;
    std::chrono::nanoseconds        PlayPosition_;
    double                          PlayPercent_;

    QPixmap                         StaticPreview_;
    std::vector<QRectF>             PreviewChannelRect_;

};

// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
} // end view
} // end qt_gui
// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n


// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G
#endif // QT_GUI_VIEW_WAVE_FORM_H_INCLUDED
