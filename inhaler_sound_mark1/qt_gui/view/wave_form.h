// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G
#ifndef QT_GUI_VIEW_WAVE_FORM_H_INCLUDED
#define QT_GUI_VIEW_WAVE_FORM_H_INCLUDED
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
#include <functional>

// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I


// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
namespace qt_gui {
namespace view {
// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n


class wave_form : public QWidget
{
    Q_OBJECT

public:

    using data_t                = qt::audio::raw_data;
    using shared_data_t         = std::shared_ptr<data_t>;
    using nanoseconds_t         = std::chrono::nanoseconds;
    using selection_handler_t   = std::function<void( const nanoseconds_t& Start, const nanoseconds_t& End )>;

public:

    explicit                wave_form                   (   const selection_handler_t& SelectionHandler,
                                                            QWidget* Parent = 0   );

    void                    reset                       (   const shared_data_t& Data   );

    void                    set_play_position           (   nanoseconds_t Position   );

    void                    reset_play_position         ();

protected:

    void                    resizeEvent                 (   QResizeEvent* Event   );

    void                    paintEvent                  (   QPaintEvent* Event   );

    void                    mousePressEvent             (   QMouseEvent* Event   );

    void                    mouseMoveEvent              (   QMouseEvent* Event   );

    void                    mouseReleaseEvent           (   QMouseEvent* Event   );

private:

    struct preview_sample_t
    {
        double Min;
        double Max;
        double Mean;
        double StdDev;
    };

    enum class selection_mode
    {
        disabled,
        start,
        adjusting_start,
        adjusting_end,
        grab_start,
        grab_end
    };

private:

    void                    create_preview_wave         	();

    bool                    position_in_channel_rect    	(   int x, int y   ) const;

    void                    limit_selection_to_boundaries	(   int Position, const selection_mode& Mode   );

    void                    update_start_and_end        	();

    void                    paint_static_preview        	(   int Width, int Height   );

    std::size_t             preview_index               	(   std::size_t Sample, std::size_t Channel   ) const;

    preview_sample_t&       preview_sample              	(   std::size_t Sample, std::size_t Channel   );

    const preview_sample_t& preview_sample              	(   std::size_t Sample, std::size_t Channel   ) const;

    std::size_t             preview_size                	() const;

    unsigned                calculate_maxres                ();

private:

    selection_handler_t             SelectionHandler_;
    shared_data_t                   Data_;
    std::vector<preview_sample_t>   Preview_;
    nanoseconds_t                   PlayPosition_;

    double                          PlayPercent_;

    QPixmap                         StaticPreview_;
    std::vector<QRectF>             PreviewChannelRect_;

    selection_mode                  SelectionMode_;

    double                          SelectionStart_;
    double                          SelectionEnd_;
};

// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
} // end view
} // end qt_gui
// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n


// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G
#endif // QT_GUI_VIEW_WAVE_FORM_H_INCLUDED
