// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I
// Self Include
#include "qt_gui/view/wave_form.h"

// QT Includes
#include <QPainter>
#include <QMouseEvent>

// Boost Includes
#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/stats.hpp>
#include <boost/accumulators/statistics/min.hpp>
#include <boost/accumulators/statistics/max.hpp>
#include <boost/accumulators/statistics/mean.hpp>
#include <boost/accumulators/statistics/variance.hpp>

// C++ Standard Library Includes
#include <iostream>
#include <cmath>

// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I


// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
namespace qt_gui {
namespace view {
// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n

wave_form::
wave_form
(   QWidget* Parent   )
: QWidget( Parent )
, PlayPosition_             ( std::chrono::nanoseconds( 0 ) )
, PlayPercent_              ( 0 )
, MouseReleased_            ( false )
, SelectionStartPosition_   ( 0 )
, SelectionEndPosition_     ( 0 )
, SelectionCurrentPosition_ ( 0 )

{
//    setMouseTracking( true );
}


void wave_form::
reset( const shared_data_t& Data )
{
    Data_ = Data;
    reset_play_position();
    create_preview_wave();
    paint_static_preview( size().width(), size().height() );
    update();
}


void wave_form::
reset_play_position()
{
    PlayPosition_ = std::chrono::nanoseconds( 0 );
    PlayPercent_ = 0;
}


void wave_form::
create_preview_wave()
{
    unsigned MaxRes = 2000;
    unsigned Size = MaxRes < Data_->samples_per_channel() ? MaxRes : Data_->samples_per_channel();
    unsigned WindowSize = Data_->samples_per_channel() / Size;
    if( Data_->samples_per_channel() % Size )
    {
        WindowSize += 1;
    }

    namespace ba = boost::accumulators;

    using accumulator_t
        = ba::accumulator_set
            <   double,
                ba::stats
                <   ba::tag::min,
                    ba::tag::max,
                    ba::tag::mean,
                    ba::tag::variance   >   >;

    auto ChannelCount = Data_->format().channel_count();

    Preview_ = std::vector<preview_sample_t>( Size * ChannelCount );

    unsigned p = 0;
    unsigned w = 0;

    std::vector<accumulator_t> Accumulator( ChannelCount );

    for( unsigned s = 0; s < Data_->samples_per_channel(); ++s )
    {
        ++w %= WindowSize;

        for( unsigned c = 0; c < ChannelCount; ++c )
        {
            if( w == 0 )
            {
                preview_sample( p,c )
                    = { ba::min( Accumulator[c] ),
                        ba::max( Accumulator[c] ),
                        ba::mean( Accumulator[c] ),
                        std::sqrt( ba::variance( Accumulator[c] ) ) };

                Accumulator[c] = accumulator_t();
            }
            Accumulator[c]( Data_->normalised_sample( s, c ) );
        }
        if( w == 0 )
        {
            ++p;
        }
    }
}


void wave_form::
set_play_position( std::chrono::nanoseconds Position )
{
    PlayPosition_ = Position;
    PlayPercent_ = static_cast<double>( PlayPosition_.count() ) / Data_->duration().count();
    update();
}


std::size_t wave_form::
preview_index( std::size_t Sample, std::size_t Channel ) const
{
    return Sample*Data_->format().channel_count()+Channel;
}


wave_form::preview_sample_t& wave_form::
preview_sample( std::size_t Sample, std::size_t Channel )
{
    return Preview_[ preview_index( Sample, Channel ) ];
}


const wave_form::preview_sample_t& wave_form::
preview_sample( std::size_t Sample, std::size_t Channel ) const
{
    return Preview_[ preview_index( Sample, Channel ) ];
}


std::size_t wave_form::
preview_size() const
{
    return Preview_.size() / Data_->format().channel_count();
}


void wave_form::
resizeEvent( QResizeEvent* Event )
{
    if( !Data_ )
    {
        return;
    }

    paint_static_preview( size().width(), size().height() );
}


void wave_form::
paintEvent( QPaintEvent* Event )
{
    if( !Data_ )
    {
        return;
    }
    QPainter Painter(this);

    Painter.drawPixmap( 0, 0, StaticPreview_ );

    Painter.setPen( Qt::gray );
    Painter.setRenderHint( QPainter::Antialiasing );

    auto ChannelCount = Data_->format().channel_count();

    for( unsigned c=0; c<ChannelCount; ++c )
    {
        auto& ChannelRect = PreviewChannelRect_[c];

        const double XStart = ChannelRect.left() + 1;
        const double Width = ChannelRect.width() - 2;

        // Draw selected start line
        if( SelectionStartPosition_ > 0 )
        {
            auto StartLeft  = SelectionStartPosition_;
            auto StartRight = StartLeft;
            Painter.setPen( QColor( 83, 140, 214, 127 ) );
            Painter.drawLine( QLineF( StartLeft, ChannelRect.top(), StartRight, ChannelRect.bottom() ) );
        }

        // Draw rectangle to show selected area
        if( SelectionCurrentPosition_ > 0 )
        {
            auto StartLeft       = SelectionStartPosition_;
            auto CurrentLeft     = SelectionCurrentPosition_;
            auto SelectionWidth  = CurrentLeft - StartLeft;
            auto SelectionHeight = ChannelRect.bottom() - ChannelRect.top();

            Painter.setPen( QColor( 83, 140, 214, 127 ) );
            Painter.drawRect( StartLeft, ChannelRect.top(), SelectionWidth, SelectionHeight );
            auto SelectedArea = QRectF( StartLeft, ChannelRect.top(), SelectionWidth, SelectionHeight );
            Painter.fillRect( SelectedArea, QColor( 83, 160, 214, 45 ) );
        }

        // Draw selected end line
        if( SelectionEndPosition_ > 0 )
        {
            auto EndLeft  = SelectionEndPosition_;
            auto EndRight = EndLeft;
            Painter.setPen( QColor( 83, 140, 214, 127 ) );
            Painter.drawLine( QLineF( EndLeft, ChannelRect.top(), EndRight, ChannelRect.bottom() ) );
        }

        // Overlay play PlayPosition
        if( PlayPosition_.count() )
        {
            Painter.setPen( QColor( 214, 214, 43, 255 ) );

            double Position = XStart + Width * PlayPercent_;
            Painter.drawLine( QLine( Position, ChannelRect.top(), Position, ChannelRect.bottom() ) );
        }
    }
}


void wave_form::
paint_static_preview( int Width, int Height )
{
    StaticPreview_ = QPixmap( Width, Height );
    QPainter Painter( &StaticPreview_ );

    Painter.setRenderHint( QPainter::Antialiasing );

    Painter.fillRect( Painter.window(), QWidget::palette().color( QWidget::backgroundRole() ) );

    auto ChannelCount = Data_->format().channel_count();

    auto ChannelHeight = Painter.window().height() / ChannelCount;

    PreviewChannelRect_.resize( ChannelCount );

    QString PlusOne     ( "+1.0" );
    QString PlusPoint5  ( "+0.5" );
    QString Zero        ( "0.0" );
    QString MinusPoint5 ( "-0.5" );
    QString MinusOne    ( "-1.0" );

    auto Font = Painter.font();
    Font.setPointSize( Painter.font().pointSize()-1 );
    Painter.setFont( Font );

    QFontMetrics FontMetrics = Painter.fontMetrics();

    auto LabelHeight = FontMetrics.height();
    auto LabelWidth  = FontMetrics.width( PlusPoint5 );

    auto YTickSize = 5;
    auto NumberOfTicks = 21;

    for( unsigned c=0; c<ChannelCount; ++c )
    {
        // Draw Background
        PreviewChannelRect_[c] = QRectF( 0, c*ChannelHeight, Painter.window().width(), ChannelHeight );
        auto& ChannelRect = PreviewChannelRect_[c];

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
            auto YTick = QLineF( YTickStart, ( YTickMax * t/10 ) + ChannelRect.top(), ChannelRect.left(), ( YTickMax * t/10 ) + ChannelRect.top() );
            Painter.drawLine( YTick );
        }

        // Label Ticks on Y-axis
        auto LabelStartPoint = 34;

        auto PlusOneTop = ChannelRect.top() - LabelHeight /2;
        auto PlusPoint5Top = ( YTickMax * 0.5 ) + ChannelRect.top() - LabelHeight /2;
        auto ZeroTop = XAxis_Y - LabelHeight /2;
        auto MinusPoint5Top = ( YTickMax * 1.5 ) + ChannelRect.top() - LabelHeight /2;
        auto MinusOneTop = ChannelRect.bottom() - LabelHeight /2;

        auto PlusOneLabel     = QRectF( ChannelRect.left() - LabelStartPoint, PlusOneTop,     LabelWidth,  LabelHeight );
        auto PlusPoint5Label  = QRectF( ChannelRect.left() - LabelStartPoint, PlusPoint5Top,  LabelWidth,  LabelHeight );
        auto ZeroLabel        = QRectF( ChannelRect.left() - LabelStartPoint, ZeroTop,        LabelWidth,  LabelHeight );
        auto MinusPoint5Label = QRectF( ChannelRect.left() - LabelStartPoint, MinusPoint5Top, LabelWidth,  LabelHeight );
        auto MinusOneLabel    = QRectF( ChannelRect.left() - LabelStartPoint, MinusOneTop,    LabelWidth,  LabelHeight );

        Painter.drawText( PlusOneLabel,     Qt::AlignRight, PlusOne );
        Painter.drawText( PlusPoint5Label,  Qt::AlignRight, PlusPoint5 );
        Painter.drawText( ZeroLabel,        Qt::AlignRight, Zero );
        Painter.drawText( MinusPoint5Label, Qt::AlignRight, MinusPoint5 );
        Painter.drawText( MinusOneLabel,    Qt::AlignRight, MinusOne );

        const double Size   = preview_size();
        const double XStart = ChannelRect.left() + 1;
        const double YStart = ChannelRect.top();
        const double Width  = ChannelRect.width() - 2;
        const double Height = ChannelRect.height();

        // Draw Wave Preview
        for( unsigned p=0; p<Size; ++p )
        {
            const auto& Sample = preview_sample( p, c );

            auto Left = XStart + p * Width / Size;
            auto Right = Left;

            // Get Top and Bottom for Min and Max
            auto ExtentTop = YStart + ( 1.0 - Sample.Max ) * Height / 2;
            auto ExtentBottom = YStart + ( 1.0 - Sample.Min ) * Height / 2;

            Painter.setPen( QColor( 68, 119, 59, 255 ) );
            Painter.drawLine( QLineF( Left, ExtentTop, Right, ExtentBottom ) );

            // Get Top and Bottom for 1 Standard Deviation from mean
            auto StdDevTop = YStart + ( 1.0 - Sample.Mean - Sample.StdDev ) * Height / 2;
            auto StdDevBottom = YStart + ( 1.0 - Sample.Mean + Sample.StdDev ) * Height / 2;

            Painter.setPen( QColor( 93, 146, 83, 255 ) );
            Painter.drawLine
                (   QLineF
                    (   Left,
                        StdDevTop < ExtentTop ? ExtentTop : StdDevTop,
                        Right,
                        StdDevBottom > ExtentBottom ? ExtentBottom : StdDevBottom   )   );
        }

        // Overlay X-Axis
        Painter.setPen( QColor( 255, 255, 255, 127 ) );
        Painter.drawLine( XAxisLine );


        // Overlay Named Regions
        /// TODO
    }
}


bool wave_form::
position_in_channel_rect( int x, int y ) const
{
    for( const auto& Rect: PreviewChannelRect_ )
    {
        if( Rect.contains( x, y ) )
        {
            return true;
        }
    }
    return false;
}


void wave_form::
mousePressEvent(QMouseEvent* MousePress)
{
    auto PointClicked = MousePress->localPos();

    if( !position_in_channel_rect( PointClicked.x(), PointClicked.y() )
        ||  MousePress->button() != Qt::LeftButton )
    {
        return;
    }
    else
    {
        SelectionStartPosition_ = PointClicked.x();
        SelectionCurrentPosition_ = 0;
        SelectionEndPosition_ = 0;
        MouseReleased_ = false;
        update();
    }
}


void wave_form::
mouseMoveEvent(QMouseEvent* MouseMove)
{
    auto PointClicked = MouseMove->localPos();

    if( !position_in_channel_rect( PointClicked.x(), PointClicked.y() ) )
    {
        return;
    }
    else
    {
        SelectionCurrentPosition_ = PointClicked.x();

        if( !MouseReleased_ )
        {
           update();
        }
    }
}


void wave_form::
mouseReleaseEvent(QMouseEvent* MouseRelease)
{
    auto PointClicked = MouseRelease->localPos();

    if( !position_in_channel_rect( PointClicked.x(), PointClicked.y() )
        ||  MouseRelease->button() != Qt::LeftButton )
    {
        return;
    }
    else
    {
        SelectionEndPosition_ = PointClicked.x();
        MouseReleased_ = true;
        update();
    }
}


// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
} // end view
} // end qt_gui
// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
