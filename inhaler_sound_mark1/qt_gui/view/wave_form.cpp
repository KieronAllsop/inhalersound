// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I

// Self Include
#include "qt_gui/view/wave_form.h"

// Qt Includes
#include <QPainter>
#include <QMouseEvent>
#include <QApplication>

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
#include <vector>

// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I


// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
namespace qt_gui {
namespace view {
// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n

wave_form::
wave_form
(   const selection_handler_t& SelectionHandler,
    QWidget* Parent   )
: QWidget( Parent )
, SelectionHandler_         ( SelectionHandler )
, PlayPositionTime_         ( nanoseconds_t( 0 ) )
, PlayPositionPercent_      ( 0.0 )
, SelectionMode_            ( selection_mode::start )
, SelectionStartPercent_    ( 0.0 )
, SelectionEndPercent_      ( 0.0 )
, NewStartPosition_         ( nanoseconds_t( 0 ) )
, NewEndPosition_           ( nanoseconds_t( 0 ) )
{
    setMouseTracking( true );
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
    PlayPositionTime_ = nanoseconds_t( 0 );
    PlayPositionPercent_ = 0.0;
    SelectionStartPercent_ = 0.0;
    SelectionEndPercent_ = 0.0;
    update();
}


unsigned wave_form::calculate_maxres()
{
    unsigned Res = 1900;
    auto LowestExtra = Data_->samples_per_channel() % Res;
    unsigned BestRes = 1900;

    for( ; Res<2101 ; ++Res )
    {
        if( ( Data_->samples_per_channel() % Res ) < LowestExtra )
        {
            BestRes = Res;
        }
    }

    return BestRes;
}


void wave_form::
create_preview_wave()
{
    unsigned MaxRes = calculate_maxres();

    unsigned Size = MaxRes < Data_->samples_per_channel() ? MaxRes : Data_->samples_per_channel();
    unsigned WindowSize = Data_->samples_per_channel() / Size;
    auto Extra = Data_->samples_per_channel() % Size;

    if( Extra )
    {
        Size += 1+Extra/WindowSize;
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
            if( w == 0 || s == Data_->samples_per_channel()-1 )
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
set_play_position( nanoseconds_t Position )
{
    PlayPositionTime_ = Position;
    PlayPositionPercent_ = static_cast<double>( PlayPositionTime_.count() ) / Data_->duration().count();
    update();
}


void wave_form::
set_selection_start( nanoseconds_t Position )
{
    NewStartPosition_ = Position;
    SelectionStartPercent_ = SelectionStartPercent_ + static_cast<double>( NewStartPosition_.count() ) / Data_->duration().count();
    update_start_and_end();
}


void wave_form::
set_selection_end( nanoseconds_t Position )
{
    NewEndPosition_ = Position;
    SelectionEndPercent_ = SelectionEndPercent_ + static_cast<double>( NewEndPosition_.count() ) / Data_->duration().count();
    update_start_and_end();
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

    QRectF OldChannel = PreviewChannelRect_[0];

    paint_static_preview( size().width(), size().height() );

    QRectF NewChannel = PreviewChannelRect_[0];

    double OldOffsetStart = SelectionStartPercent_ - OldChannel.left();
    double OldOffsetEnd   = SelectionEndPercent_ - OldChannel.left();

    double Scale = NewChannel.width() / OldChannel.width();

    SelectionStartPercent_ = Scale * OldOffsetStart + NewChannel.left();
    SelectionEndPercent_   = Scale * OldOffsetEnd + NewChannel.left();
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

    Painter.setRenderHint( QPainter::Antialiasing );

    auto ChannelCount = Data_->format().channel_count();

    for( unsigned c=0; c<ChannelCount; ++c )
    {
        auto& ChannelRect = PreviewChannelRect_[c];

        const double XStart = ChannelRect.left();
        const double Width = ChannelRect.width();

        // Overlay play PlayPosition
        if( PlayPositionTime_.count() )
        {
            Painter.setPen( QColor( 214, 214, 43, 255 ) );

            double Position = XStart + Width * PlayPositionPercent_;
            Painter.drawLine( QLine( Position, ChannelRect.top(), Position, ChannelRect.bottom() ) );
        }
        if( SelectionStartPercent_ > 0.0 || SelectionEndPercent_ > 0.0 )
        {
            Painter.setPen( QColor( 83, 140, 214, 127 ) );
            if( SelectionEndPercent_ )
            {
                Painter.drawLine( QLineF( SelectionStartPercent_, ChannelRect.top(), SelectionStartPercent_, ChannelRect.bottom() ) );
                auto Width = SelectionEndPercent_ - SelectionStartPercent_;
                auto SelectionRect = QRectF( SelectionStartPercent_, ChannelRect.top(), Width, ChannelRect.height() );
                Painter.fillRect( SelectionRect, QColor( 83, 160, 224, 45 ) );
                Painter.drawLine( QLineF( SelectionEndPercent_, ChannelRect.top(), SelectionEndPercent_, ChannelRect.bottom() ) );
            }
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

        // Draw Wave Preview
        const double Size   = preview_size();

        ChannelRect.adjust( +1, 0, -2, 0 );

        const double XStart = ChannelRect.left();
        const double YStart = ChannelRect.top();
        const double Width  = ChannelRect.width();
        const double Height = ChannelRect.height();

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
update_start_and_end()
{
    auto Left = PreviewChannelRect_[0].left();
    auto Width = PreviewChannelRect_[0].width();

     std::size_t StartSample = 0.5 + Data_->samples_per_channel() * ( SelectionStartPercent_ - Left ) / Width;
     auto StartPosition = Data_->duration_from( StartSample );

     auto EndPosition = StartPosition;

     if( SelectionEndPercent_ > 0.0 )
     {
        std::size_t EndSample = 0.5 + Data_->samples_per_channel() * ( SelectionEndPercent_ - Left ) / Width;
        EndPosition = Data_->duration_from( EndSample );
     }

     std::size_t StartSelectionSample = StartPosition.count()*Data_->format().sample_rate() / 1000000000;
     std::size_t EndSelectionSample = EndPosition.count()*Data_->format().sample_rate() / 1000000000;

     if( SelectionHandler_ )
     {
         SelectionHandler_( StartPosition, EndPosition, StartSelectionSample, EndSelectionSample );
     }
}


void wave_form::
limit_selection_to_boundaries( int Position, const selection_mode& Mode )
{
    auto& Boundary = ( Mode == selection_mode::adjusting_start ) ? SelectionStartPercent_ : SelectionEndPercent_;
    if( Position > PreviewChannelRect_[0].right() )
    {
        Boundary = PreviewChannelRect_[0].right();
    }
    else if( Position < PreviewChannelRect_[0].left() )
    {
        Boundary = PreviewChannelRect_[0].left();
    }
    else
    {
        Boundary = Position;
    }
    if( SelectionEndPercent_ < SelectionStartPercent_ )
    {
        std::swap( SelectionStartPercent_, SelectionEndPercent_ );
    }
}


void wave_form::
mousePressEvent( QMouseEvent* Event )
{
    int x = Event->x();
    int y = Event->y();

    bool MouseInChannels = position_in_channel_rect( x, y );

    if( Event->button() == Qt::LeftButton )
    {
        if( !MouseInChannels )
        {
            SelectionMode_ = selection_mode::disabled;
        }
        else if( SelectionMode_ == selection_mode::start )
        {
            SelectionStartPercent_ = x;
            SelectionEndPercent_ = 0.0;
            update_start_and_end();
            SelectionMode_ = selection_mode::adjusting_end;
            update();
        }
        else if( SelectionMode_ == selection_mode::grab_start )
        {
            SelectionMode_ = selection_mode::adjusting_start;
        }
        else if( SelectionMode_ == selection_mode::grab_end )
        {
            SelectionMode_ = selection_mode::adjusting_end;
        }
    }
}


void wave_form::
mouseReleaseEvent( QMouseEvent* Event )
{
    int x = Event->x();
    int y = Event->y();

    bool MouseInChannels = position_in_channel_rect( x, y );

    if( SelectionMode_ == selection_mode::disabled )
    {
        SelectionMode_ = selection_mode::start;
    }
    else if( Event->button() == Qt::LeftButton  )
    {
        if( MouseInChannels )
        {
            if(     SelectionMode_ == selection_mode::adjusting_start
                ||  SelectionMode_ == selection_mode::adjusting_end )
            {
                QApplication::restoreOverrideCursor();

                auto& Boundary = ( SelectionMode_ == selection_mode::adjusting_start ) ? SelectionStartPercent_ : SelectionEndPercent_;
                Boundary = x;
                if( SelectionEndPercent_ < SelectionStartPercent_ )
                {
                    std::swap( SelectionStartPercent_, SelectionEndPercent_ );
                }
                update_start_and_end();
                SelectionMode_ = selection_mode::start;
                update();
            }
            else if(    SelectionMode_ == selection_mode::grab_start
                    ||  SelectionMode_ == selection_mode::grab_end )
            {
                QApplication::restoreOverrideCursor();

                auto& Boundary = ( SelectionMode_ == selection_mode::grab_start ) ? SelectionStartPercent_ : SelectionEndPercent_;
                Boundary = x;
                update_start_and_end();
                SelectionMode_ = selection_mode::start;
                update();
            }
        }
        else
        {
            if(     SelectionMode_ == selection_mode::adjusting_start
                ||  SelectionMode_ == selection_mode::adjusting_end )
            {
                QApplication::restoreOverrideCursor();

                limit_selection_to_boundaries( x, SelectionMode_ );
                if( SelectionEndPercent_ < SelectionStartPercent_ )
                {
                    std::swap( SelectionStartPercent_, SelectionEndPercent_ );
                }
                update_start_and_end();
                SelectionMode_ = selection_mode::start;
                update();
            }
            else if(    SelectionMode_ == selection_mode::grab_start
                    ||  SelectionMode_ == selection_mode::grab_end )
            {
                QApplication::restoreOverrideCursor();
                SelectionMode_ = selection_mode::start;
                update();
            }
        }
    }
}


void wave_form::
mouseMoveEvent( QMouseEvent* Event )
{
    int x = Event->x();
    int y = Event->y();

    bool MouseInChannels = position_in_channel_rect( x, y );

    if( Event->buttons() & Qt::LeftButton )
    {
        if( MouseInChannels )
        {
            if( SelectionMode_ == selection_mode::start )
            {
                if(     SelectionStartPercent_ > 0.0
                    &&  x > ( SelectionStartPercent_-4.0 )
                    &&  x < ( SelectionStartPercent_+4.0 ) )
                {
                    SelectionMode_ = selection_mode::grab_start;
                    QApplication::setOverrideCursor( Qt::SizeHorCursor );
                }
                else if(    SelectionEndPercent_ > 0.0
                        &&  x > ( SelectionEndPercent_-4.0 )
                        &&  x < ( SelectionEndPercent_+4.0 ) )
                {
                    SelectionMode_ = selection_mode::grab_end;
                    QApplication::setOverrideCursor( Qt::SizeHorCursor );
                }
            }
            else if(    SelectionMode_ == selection_mode::adjusting_start
                    ||  SelectionMode_ == selection_mode::adjusting_end )
            {
                auto& Boundary = SelectionMode_ == selection_mode::adjusting_start ? SelectionStartPercent_ : SelectionEndPercent_;
                Boundary = x;
                if( SelectionEndPercent_ < SelectionStartPercent_ )
                {
                    std::swap( SelectionStartPercent_, SelectionEndPercent_ );
                    SelectionMode_ = ( SelectionMode_ == selection_mode::adjusting_start ) ? selection_mode::adjusting_end : selection_mode::adjusting_start;
                }
                update_start_and_end();
                update();
            }
        }
        else if(    SelectionMode_ == selection_mode::adjusting_start
                ||  SelectionMode_ == selection_mode::adjusting_end )
        {
            limit_selection_to_boundaries( x, SelectionMode_ );
            if( SelectionEndPercent_ < SelectionStartPercent_ )
            {
                std::swap( SelectionStartPercent_, SelectionEndPercent_ );
                SelectionMode_ = ( SelectionMode_ == selection_mode::adjusting_start ) ? selection_mode::adjusting_end : selection_mode::adjusting_start;
            }
            update_start_and_end();
            update();
        }
    }
    else if( !Event->buttons() )
    {
        if( MouseInChannels && SelectionMode_ == selection_mode::start )
        {
            if(     SelectionStartPercent_ > 0.0
                &&  x > ( SelectionStartPercent_-4.0 )
                &&  x < ( SelectionStartPercent_+4.0 ) )
            {
                SelectionMode_ = selection_mode::grab_start;
                QApplication::setOverrideCursor( Qt::SizeHorCursor );
            }
            else if(     SelectionEndPercent_ > 0.0
                &&  x > ( SelectionEndPercent_-4.0 )
                &&  x < ( SelectionEndPercent_+4.0 ) )
            {
                SelectionMode_ = selection_mode::grab_end;
                QApplication::setOverrideCursor( Qt::SizeHorCursor );
            }
        }
        else if(    SelectionMode_ == selection_mode::grab_start
                 || SelectionMode_ == selection_mode::grab_end )
        {
            auto& Boundary = SelectionMode_ == selection_mode::grab_start ? SelectionStartPercent_ : SelectionEndPercent_;
            if( Boundary && ( x <= ( Boundary-4.0 ) || x >= ( Boundary+4.0 ) ) )
            {
                SelectionMode_ = selection_mode::start;
                QApplication::restoreOverrideCursor();
            }
        }
    }
}



// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
} // end view
} // end qt_gui
// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
