// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I

// Self Include
#include "qt_gui/view/wave_zoom_start.h"

// Qt Includes
#include <QPainter>

// Boost Includes

// C++ Standard Library Includes
#include <iostream>
#include <cmath>

// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I


// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
namespace qt_gui {
namespace view {
// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n


wave_zoom_start::
wave_zoom_start
(   QWidget* Parent   )
: QWidget(Parent)
, PlayPosition_( nanoseconds_t( 0 ) )
{
}


void wave_zoom_start::
reset( const shared_data_t& Data )
{
    std::cout << "reset wave zoom start" << std::endl;
    Data_ = Data;
    create_wave();
    paint_static_wave( size().width(), size().height() );
    update();
}


void wave_zoom_start::
set_play_position( nanoseconds_t Position )
{
    PlayPosition_ = Position;
}


void wave_zoom_start::
create_wave()
{
    std::cout << "create wave zoom start" << std::endl;
}


std::size_t wave_zoom_start::
wave_index( std::size_t Sample, std::size_t Channel ) const
{
    return Sample*Data_->format().channel_count()+Channel;
}


wave_zoom_start::wave_sample_t& wave_zoom_start::
wave_sample( std::size_t Sample, std::size_t Channel )
{
    return Wave_[ wave_index( Sample, Channel ) ];
}


const wave_zoom_start::wave_sample_t& wave_zoom_start::
wave_sample( std::size_t Sample, std::size_t Channel ) const
{
    return Wave_[ wave_index( Sample, Channel ) ];
}


std::size_t wave_zoom_start::
wave_size() const
{
    return Wave_.size() / Data_->format().channel_count();
}


void wave_zoom_start::
resizeEvent( QResizeEvent* Event )
{
    if( !Data_ )
    {
        return;
    }
    paint_static_wave( size().width(), size().height() );
}


void wave_zoom_start::
paintEvent( QPaintEvent* Event )
{
    if( !Data_ )
    {
        return;
    }
    QPainter Painter(this);

    Painter.drawPixmap( 0, 0, StaticWaveView_ );

    Painter.setRenderHint( QPainter::Antialiasing );

    auto ChannelCount = Data_->format().channel_count();

//    for( unsigned c=0; c<ChannelCount; ++c )
//    {
//        auto& ChannelRect = PreviewChannelRect_[c];

//        const double XStart = ChannelRect.left();
//        const double Width = ChannelRect.width();

//        // Overlay play PlayPosition
//        if( PlayPosition_.count() )
//        {
//            Painter.setPen( QColor( 214, 214, 43, 255 ) );

//            double Position = XStart + Width * PlayPercent_;
//            Painter.drawLine( QLine( Position, ChannelRect.top(), Position, ChannelRect.bottom() ) );
//        }
//        if( SelectionStart_ > 0.0 || SelectionEnd_ > 0.0 )
//        {
//            Painter.setPen( QColor( 83, 140, 214, 127 ) );
//            if( SelectionEnd_ )
//            {
//                Painter.drawLine( QLineF( SelectionStart_, ChannelRect.top(), SelectionStart_, ChannelRect.bottom() ) );
//                auto Width = SelectionEnd_ - SelectionStart_;
//                auto SelectionRect = QRectF( SelectionStart_, ChannelRect.top(), Width, ChannelRect.height() );
//                Painter.fillRect( SelectionRect, QColor( 83, 160, 224, 45 ) );
//                Painter.drawLine( QLineF( SelectionEnd_, ChannelRect.top(), SelectionEnd_, ChannelRect.bottom() ) );
//            }
//        }
//    }
}


void wave_zoom_start::
paint_static_wave( int Width, int Height )
{
    StaticWaveView_ = QPixmap( Width, Height );
    QPainter Painter( &StaticWaveView_ );

    Painter.setRenderHint( QPainter::Antialiasing );

    Painter.fillRect( Painter.window(), QWidget::palette().color( QWidget::backgroundRole() ) );

    auto ChannelCount = Data_->format().channel_count();

    auto ChannelHeight = Painter.window().height() / ChannelCount;

    WaveChannelRect_.resize( ChannelCount );

    auto YTickSize = 5;
    auto NumberOfTicks = 21;

    std::vector<QString> YLabels = { "+1.0", "+0.8", "+0.6", "+0.4", "+0.2", "0.0", "-0.2", "-0.4", "-0.6", "-0.8", "-1.0" };

    auto Font = Painter.font();
    Font.setPointSize( Painter.font().pointSize() -1 );
    Painter.setFont( Font );

    QFontMetrics FontMetrics = Painter.fontMetrics();

    auto LabelHeight = FontMetrics.height();
    auto LabelWidth  = FontMetrics.width( YLabels[1] );
    auto LabelStartPointOffset = 34;

    for( unsigned c=0; c<ChannelCount; ++c )
    {
        // Draw Background
        WaveChannelRect_[c] = QRectF( 0, c*ChannelHeight, Painter.window().width(), ChannelHeight );
        auto& ChannelRect = WaveChannelRect_[c];

        ChannelRect.adjust( 2, 4, -2, -4 );
        QPainterPath ChannelPath;
        ChannelPath.addRoundedRect( ChannelRect, 9, 9 );
        Painter.fillPath( ChannelPath, QColor( 24, 24, 24, 255 ) );
        Painter.drawPath( ChannelPath );

        // Calculate Axes
        ChannelRect.adjust( 32, 8, -16, -8 );
        Painter.fillRect( ChannelRect, QColor( 12, 12, 12, 255 ) );
        auto XAxis_Y = ChannelRect.top() + ChannelRect.height() / 2;
        auto XAxisLine = QLineF( ChannelRect.left(), XAxis_Y, ChannelRect.right() - 2, XAxis_Y );
        auto YAxisLine = QLineF( ChannelRect.left(), ChannelRect.top(), ChannelRect.left(), ChannelRect.bottom() );

        // Draw Axis
        Painter.setPen( QColor( 255, 255, 255, 255 ) );
        Painter.drawLine( XAxisLine );
        Painter.drawLine( YAxisLine );

        // Draw ticks on Y-axis
        auto YTickMax = XAxis_Y - ChannelRect.top();
        auto YTickStart = ChannelRect.left() - YTickSize;

        for( int t=0; t<NumberOfTicks; ++t )
        {
            auto VerticalPosition = ( YTickMax * t/10 ) + ChannelRect.top();
            auto YTick = QLineF( YTickStart, VerticalPosition, ChannelRect.left(), VerticalPosition );
            Painter.drawLine( YTick );
        }

        // Label Ticks on Y-axis
        auto LabelStartPoint = ChannelRect.left() - LabelStartPointOffset;

        for( unsigned y=0; y<YLabels.size(); ++y )
        {
            auto VerticalPosition = ( YTickMax * y/5 ) + ChannelRect.top() - LabelHeight / 2;
            auto YLabelPosition = QRectF( LabelStartPoint, VerticalPosition, LabelWidth, LabelHeight );
            Painter.drawText( YLabelPosition, Qt::AlignRight, YLabels[y] );
        }

        // Draw Wave Preview
        const double Size   = wave_size();

        ChannelRect.adjust( +1, 0, -2, 0 );

        const double XStart = ChannelRect.left();
        const double YStart = ChannelRect.top();
        const double Width  = ChannelRect.width();
        const double Height = ChannelRect.height();

//        for( unsigned p=0; p<Size; ++p )
//        {
//            const auto& Sample = wave_sample( p, c );

//            auto Left = XStart + p * Width / Size;
//            auto Right = Left;

//            // Get Top and Bottom for Min and Max
//            auto ExtentTop = YStart + ( 1.0 - Sample.Max ) * Height / 2;
//            auto ExtentBottom = YStart + ( 1.0 - Sample.Min ) * Height / 2;

//            Painter.setPen( QColor( 68, 119, 59, 255 ) );
//            Painter.drawLine( QLineF( Left, ExtentTop, Right, ExtentBottom ) );

//            // Get Top and Bottom for 1 Standard Deviation from mean
//            auto StdDevTop = YStart + ( 1.0 - Sample.Mean - Sample.StdDev ) * Height / 2;
//            auto StdDevBottom = YStart + ( 1.0 - Sample.Mean + Sample.StdDev ) * Height / 2;

//            Painter.setPen( QColor( 93, 146, 83, 255 ) );
//            Painter.drawLine
//                (   QLineF
//                    (   Left,
//                        StdDevTop < ExtentTop ? ExtentTop : StdDevTop,
//                        Right,
//                        StdDevBottom > ExtentBottom ? ExtentBottom : StdDevBottom   )   );
        }

        // Overlay X-Axis
//        Painter.setPen( QColor( 255, 255, 255, 127 ) );
//        Painter.drawLine( XAxisLine );

        // Overlay Named Regions
        /// TODO
    }



// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
} // end view
} // end qt_gui
// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
