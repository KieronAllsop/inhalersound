// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G
#ifndef QT_GUI_VIEW_WAVE_ZOOM_START_H_INCLUDED
#define QT_GUI_VIEW_WAVE_ZOOM_START_H_INCLUDED
// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G

// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I

// qt::audio Includes
#include "qt/audio/raw_data.hpp"

// Qt Includes
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

class wave_zoom_start : public QWidget
{
    Q_OBJECT

public:

    using data_t                = qt::audio::raw_data;
    using shared_data_t         = std::shared_ptr<data_t>;
    using nanoseconds_t         = std::chrono::nanoseconds;

public:

    explicit                wave_zoom_start             (   QWidget* Parent = 0   );

    void                    reset                       (   const shared_data_t& Data   );

    void                    set_play_position           (   nanoseconds_t Position   );

protected:

    void                    resizeEvent                 (   QResizeEvent* Event   );

    void                    paintEvent                  (   QPaintEvent* Event   );

private:

    struct wave_sample_t
    {
        double Point;
    };

private:

    void                    create_wave                 ();

    void                    paint_static_wave           (   int Width, int Height   );

    std::size_t             wave_index                  (   std::size_t Sample, std::size_t Channel   ) const;

    wave_sample_t&          wave_sample                 (   std::size_t Sample, std::size_t Channel   );

    const wave_sample_t&    wave_sample                 (   std::size_t Sample, std::size_t Channel   ) const;

    std::size_t             wave_size                   () const;

private:

    shared_data_t                   Data_;
    std::vector<wave_sample_t>      Wave_;
    nanoseconds_t                   PlayPosition_;

    QPixmap                         StaticWaveView_;
    std::vector<QRectF>             WaveChannelRect_;

};

// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
} // end view
} // end qt_gui
// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n

// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G
#endif // QT_GUI_VIEW_WAVE_ZOOM_START_H_INCLUDED
