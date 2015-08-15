// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I

// Self Include
#include "qt_gui/view/wave_zoom_end.h"

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


wave_zoom_end::
wave_zoom_end
(   QWidget* Parent   )
: QWidget(Parent)
, PlayPosition_( std::chrono::nanoseconds( 0 ) )
{
}


void wave_zoom_end::
reset( const shared_data_t& Data )
{
    Data_ = Data;
    std::cout << "reset wave zoom start" << std::endl;
    update();
}


void wave_zoom_end::
set_play_position( std::chrono::nanoseconds Position )
{
    PlayPosition_ = Position;
    update();
}


void wave_zoom_end::
paintEvent( QPaintEvent* Event )
{
    if( !Data_ )
    {
        return;
    }
    QPainter painter(this);

    painter.setPen( Qt::gray );
    painter.setRenderHint( QPainter::Antialiasing );

    auto ChannelCount = Data_->format().channel_count();

    auto ChannelHeight = painter.window().height() / ChannelCount;

    for( unsigned c=0; c<ChannelCount; ++c )
    {
        // Draw Background
        auto ChannelRect = QRectF( 0, c*ChannelHeight, painter.window().width(), ChannelHeight );
        ChannelRect.adjust( 2, 4, -2, -4 );
        QPainterPath ChannelPath;
        ChannelPath.addRoundedRect( ChannelRect, 9, 9 );
        painter.fillPath( ChannelPath, QColor( 24, 24, 24, 255 ) );
        painter.drawPath( ChannelPath );

        // Calculate Axes
        ChannelRect.adjust( 16, 8, -16, -8 );
        painter.fillRect( ChannelRect, QColor( 12, 12, 12, 255 ) );
        auto XAxis_Y = ChannelRect.top() + ChannelRect.height() / 2;
        auto XAxisLine = QLineF( ChannelRect.left(), XAxis_Y, ChannelRect.right(), XAxis_Y );
        auto YAxisLine = QLineF( ChannelRect.left(), ChannelRect.top(), ChannelRect.left(), ChannelRect.bottom() );

        // Draw Axis
        painter.setPen( QColor( 255, 255, 255, 255 ) );
        painter.drawLine( XAxisLine );
//        painter.drawLine( YAxisLine );
        auto Delta = 2*50*Data_->format().sample_rate() / 1000;

        std::vector<QPointF> Points( Delta + 1 );

        long long CentreSample = PlayPosition_.count()*Data_->format().sample_rate() / 1000000000;

        long long StartSample = 0;
        int SkipStart = 0;
        int SkipEnd = 0;

        if( Delta/2 < CentreSample )
        {
            StartSample = CentreSample - Delta/2;
            SkipStart = 0;
        }
        else if( CentreSample < Delta/2 )
        {
            SkipStart = Delta/2 - CentreSample;
        }

//        if( (StartSample + Points.size()) > Data_->samples_per_channel() )
//        {
//            Points.resize()
//        }
        auto i=SkipStart;

        for( long long s=StartSample ; s<StartSample+Points.size()-SkipStart-SkipEnd; ++s, ++i )
        {
            auto Sample = Data_->normalised_sample( s, c );

            auto Xpoint = ChannelRect.left() + i * ChannelRect.width() / Points.size();
            auto Ypoint = ChannelRect.top() + ( 1.0 - Sample ) * ChannelRect.height() / 2;
            Points[i] = QPointF( Xpoint, Ypoint );
        }

        painter.drawPolyline( &Points[SkipStart], Points.size()-(SkipStart - SkipEnd) );
    }
}


// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
} // end view
} // end qt_gui
// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
