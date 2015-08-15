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
, PlayPosition_( std::chrono::nanoseconds( 0 ) )
{
}


void wave_zoom_start::
reset( const shared_data_t& Data )
{
    Data_ = Data;
    update();
}


void wave_zoom_start::
set_play_position( std::chrono::nanoseconds Position )
{
    PlayPosition_ = Position;
    update();
}


void wave_zoom_start::
paintEvent( QPaintEvent* Event )
{
    if( !Data_ )
    {
        return;
    }
    QPainter Painter(this);

    Painter.setRenderHint( QPainter::Antialiasing );

    auto ChannelCount = Data_->format().channel_count();

    auto ChannelHeight = Painter.window().height() / ChannelCount;

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
        auto ChannelRect = QRectF( 0, c*ChannelHeight, Painter.window().width(), ChannelHeight );
        ChannelRect.adjust( 2, 4, -2, -4 );
        QPainterPath ChannelPath;
        ChannelPath.addRoundedRect( ChannelRect, 9, 9 );
        Painter.fillPath( ChannelPath, QColor( 24, 24, 24, 255 ) );
        Painter.drawPath( ChannelPath );

        // Calculate Axes
        ChannelRect.adjust( 32, 8, -16, -8 );
        Painter.fillRect( ChannelRect, QColor( 12, 12, 12, 255 ) );
        auto XAxis_Y = ChannelRect.top() + ChannelRect.height() / 2;
        auto XAxisLine = QLineF( ChannelRect.left(), XAxis_Y, ChannelRect.right(), XAxis_Y );
        auto YAxisLine = QLineF( ChannelRect.left(), ChannelRect.top(), ChannelRect.left(), ChannelRect.bottom() );

        // Draw Axis
        Painter.setPen( QColor( 255, 255, 255, 200 ) );
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

        // Prep for wave zoom
        auto ZoomDisplay = 2*50*Data_->format().sample_rate() / 1000;

        std::vector<QPointF> Points( ZoomDisplay + 1 );

        unsigned long long CentreSample = PlayPosition_.count()*Data_->format().sample_rate() / 1000000000;
        unsigned long long StartSample = 0;
        unsigned SkipStart = 0;
        unsigned SkipEnd = 0;

        // Determine SkipStart
        if( ZoomDisplay/2 < CentreSample )
        {
            StartSample = CentreSample - ZoomDisplay/2;
            SkipStart = 0;
        }
        else if( CentreSample < ZoomDisplay/2 )
        {
            SkipStart = ZoomDisplay/2 - CentreSample;
        }

        // Determine SkipEnd
        if( ( StartSample + ZoomDisplay/2 ) > Data_->samples_per_channel() )
        {
            SkipEnd = StartSample + ZoomDisplay/2 - Data_->samples_per_channel();
        }

        // Add points to vector
        unsigned i=SkipStart;
        for( unsigned long long s=StartSample ; s<StartSample+Points.size()-SkipStart-SkipEnd; ++s, ++i )
        {
            auto Sample = Data_->normalised_sample( s, c );

            auto Xpoint = ChannelRect.left() + i * ChannelRect.width() / Points.size();
            auto Ypoint = ChannelRect.top() + ( 1.0 - Sample ) * ChannelRect.height() / 2;
            Points[i] = QPointF( Xpoint, Ypoint );
        }

        // Draw wave
        Painter.setPen( QColor( 68, 189, 45, 255 ) );
        Painter.drawPolyline( &Points[SkipStart], Points.size()-(SkipStart - SkipEnd) );

        // Overlay X-Axis
        Painter.setPen( QColor( 255, 255, 255, 127 ) );
        Painter.drawLine( XAxisLine );

        // Overlay CentrePosition
        Painter.setPen( QColor( 83, 140, 214, 127 ) );
        auto WindowMiddle = ( ChannelRect.right() + ChannelRect.left() ) / 2;
        Painter.drawLine( QLine( WindowMiddle, ChannelRect.top(), WindowMiddle, ChannelRect.bottom() ) );
    }
}


// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
} // end view
} // end qt_gui
// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
