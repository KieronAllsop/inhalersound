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

// Standard Library Includes
#include <iostream>
#include <cmath>
#include <vector>
#include <chrono>

// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I

// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
namespace qt_gui {
namespace view {
// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n


//! \class  wave_form.cpp
//! \author Kieron Allsop
//!
//! \brief  Widget to show a graphical representation of the wave form. User can
//!         play, pause, stop, and make selections on the recording.
//!
wave_form::
wave_form
(   const selection_handler_t& SelectionHandler,
    QWidget* Parent   )
: QWidget( Parent )
, SelectionHandler_         ( SelectionHandler )
, PlayPositionTime_         ( nanoseconds_t( 0 ) )
, PlayPositionPercent_      ( 0.0 )
, SelectionMode_            ( selection_mode::start )
, SelectionStart_           ( 0.0 )
, SelectionEnd_             ( 0.0 )
, StartPosition_            ( nanoseconds_t( 0 ) )
, EndPosition_              ( nanoseconds_t( 0 ) )
, FineTuneStartFactor_      ( 0.0 )
, FineTuneEndFactor_        ( 0.0 )
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
    SelectionStart_ = 0.0;
    SelectionEnd_ = 0.0;
    update();
}


unsigned wave_form::
calculate_maxres()
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
            Accumulator[c]( Data_->scaled_sample<float>( s, c ) );
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
    auto Left = PreviewChannelRect_[0].left();
    auto Width = PreviewChannelRect_[0].width();
    FineTuneStartFactor_ = static_cast<double>( Position.count() ) / Data_->duration().count();
    SelectionStart_ = FineTuneStartFactor_*Width + Left;

    if( SelectionStart_ > Width + Left )
    {
        SelectionStart_ = Left;
    }
    update_start_and_end();
}


void wave_form::
set_selection_end( nanoseconds_t Position )
{
    auto Left = PreviewChannelRect_[0].left();
    auto Width = PreviewChannelRect_[0].width();
    FineTuneEndFactor_ = static_cast<double>( Position.count() ) / Data_->duration().count();
    SelectionEnd_ = FineTuneEndFactor_*Width + Left;

    if( SelectionEnd_ > Width + Left )
    {
        SelectionEnd_ = Width + Left;
    }
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

    double OldOffsetStart = SelectionStart_ - OldChannel.left();
    double OldOffsetEnd   = SelectionEnd_ - OldChannel.left();

    double Scale = NewChannel.width() / OldChannel.width();

    SelectionStart_ = Scale * OldOffsetStart + NewChannel.left();
    SelectionEnd_   = Scale * OldOffsetEnd + NewChannel.left();
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

        // Draw area selected by user
        if( SelectionStart_ > 0.0 || SelectionEnd_ > 0.0 )
        {
            Painter.setPen( QColor( 83, 140, 214, 127 ) );
            if( SelectionEnd_ )
            {
                Painter.drawLine( QLineF( SelectionStart_, ChannelRect.top(), SelectionStart_, ChannelRect.bottom() ) );
                auto Width = SelectionEnd_ - SelectionStart_;
                auto SelectionRect = QRectF( SelectionStart_, ChannelRect.top(), Width, ChannelRect.height() );
                Painter.fillRect( SelectionRect, QColor( 83, 160, 224, 45 ) );
                Painter.drawLine( QLineF( SelectionEnd_, ChannelRect.top(), SelectionEnd_, ChannelRect.bottom() ) );
            }
        }

        // Draw labelled areas
        if( LabelData_.size() > 0 )
        {

            auto Font = Painter.font();
            Font.setPointSize( Painter.font().pointSize() +2 );
            Painter.setFont( Font );

            QFontMetrics FontMetrics = Painter.fontMetrics();
            auto LabelHeight = FontMetrics.height();

            Painter.setPen( QColor( 255, 255, 205, 127 ) );
            for( const auto& LabelRow: LabelData_ )
            {
                // Draw labelled regions
                auto LabelStartSample = LabelRow.label_start();
                auto LabelEndSample = LabelRow.label_end();

                auto LabelStartTime = ( std::chrono::nanoseconds( LabelStartSample * 1'000'000'000 / Data_->format().sample_rate() ) );
                auto LabelEndTime = ( std::chrono::nanoseconds( LabelEndSample * 1'000'000'000 / Data_->format().sample_rate() ) );

                auto LabelStartPositionalFactor = static_cast<double>( LabelStartTime.count() ) / Data_->duration().count();
                auto LabelEndPositionalFactor = static_cast<double>( LabelEndTime.count() ) / Data_->duration().count();

                auto LabelStartSelection = LabelStartPositionalFactor*Width + XStart;
                auto LabelEndSelection = LabelEndPositionalFactor*Width + XStart;


                Painter.drawLine( QLineF( LabelStartSelection, ChannelRect.top(), LabelStartSelection, ChannelRect.bottom() ) );
                auto Width = LabelEndSelection - LabelStartSelection;
                auto SelectionRect = QRectF( LabelStartSelection, ChannelRect.top(), Width, ChannelRect.height() );

                Painter.fillRect( SelectionRect, QColor( 255, 255, 205, 45 ) );
                Painter.drawLine( QLineF( LabelEndSelection, ChannelRect.top(), LabelEndSelection, ChannelRect.bottom() ) );

                // Add event name to labelled regions
                auto LabelEvent = QString::fromStdString( LabelRow.label_name() );
                auto LabelWidth = FontMetrics.width( LabelEvent );
                auto LabelMidPoint = ( LabelEndSelection + LabelStartSelection )/2;
                auto LabelStartPoint = LabelMidPoint - LabelWidth/2;
                auto VerticalPosition = ChannelRect.top() - LabelHeight/2;
                auto EventLabelPosition = QRectF( LabelStartPoint, VerticalPosition, LabelWidth, LabelHeight );

                Painter.setPen( QColor( 255, 255, 205, 200 ) );
                Painter.drawText( EventLabelPosition, Qt::AlignCenter, LabelEvent );
            }
        }
    }
}


void wave_form::
update_label_data(const std::vector<vocabulary_t>& LabelData)
{
    LabelData_.clear();

    for( const auto& LabelRow: LabelData)
    {
        LabelData_.push_back( LabelRow );
    }

    update();
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

    auto StartSample = 0.5 + Data_->samples_per_channel() * ( SelectionStart_ - Left ) / Width;
    StartPosition_ = Data_->duration_from( StartSample );
    EndPosition_ = StartPosition_;
    if( SelectionEnd_ > 0.0 )
    {
        auto EndSample = 0.5 + Data_->samples_per_channel() * ( SelectionEnd_ - Left ) / Width;
        EndPosition_ = Data_->duration_from( EndSample );
    }

    std::size_t StartSelectionSample = StartPosition_.count()*Data_->format().sample_rate() / 1'000'000'000;
    std::size_t EndSelectionSample = EndPosition_.count()*Data_->format().sample_rate() / 1'000'000'000;

    if( SelectionHandler_ )
    {
        SelectionHandler_( StartPosition_, EndPosition_, StartSelectionSample, EndSelectionSample );
    }
}


void wave_form::
limit_selection_to_boundaries( int Position, const selection_mode& Mode )
{
    auto& Boundary = ( Mode == selection_mode::adjusting_start ) ? SelectionStart_ : SelectionEnd_;
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
    if( SelectionEnd_ < SelectionStart_ )
    {
        std::swap( SelectionStart_, SelectionEnd_ );
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
            SelectionStart_ = x;
            SelectionEnd_ = 0.0;
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

                auto& Boundary = ( SelectionMode_ == selection_mode::adjusting_start ) ? SelectionStart_ : SelectionEnd_;
                Boundary = x;
                if( SelectionEnd_ < SelectionStart_ )
                {
                    std::swap( SelectionStart_, SelectionEnd_ );
                }
                update_start_and_end();
                SelectionMode_ = selection_mode::start;
                update();
            }
            else if(    SelectionMode_ == selection_mode::grab_start
                    ||  SelectionMode_ == selection_mode::grab_end )
            {
                QApplication::restoreOverrideCursor();

                auto& Boundary = ( SelectionMode_ == selection_mode::grab_start ) ? SelectionStart_ : SelectionEnd_;
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
                if( SelectionEnd_ < SelectionStart_ )
                {
                    std::swap( SelectionStart_, SelectionEnd_ );
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
                if(     SelectionStart_ > 0.0
                    &&  x > ( SelectionStart_-4.0 )
                    &&  x < ( SelectionStart_+4.0 ) )
                {
                    SelectionMode_ = selection_mode::grab_start;
                    QApplication::setOverrideCursor( Qt::SizeHorCursor );
                }
                else if(    SelectionEnd_ > 0.0
                        &&  x > ( SelectionEnd_-4.0 )
                        &&  x < ( SelectionEnd_+4.0 ) )
                {
                    SelectionMode_ = selection_mode::grab_end;
                    QApplication::setOverrideCursor( Qt::SizeHorCursor );
                }
            }
            else if(    SelectionMode_ == selection_mode::adjusting_start
                    ||  SelectionMode_ == selection_mode::adjusting_end )
            {
                auto& Boundary = SelectionMode_ == selection_mode::adjusting_start ? SelectionStart_ : SelectionEnd_;
                Boundary = x;
                if( SelectionEnd_ < SelectionStart_ )
                {
                    std::swap( SelectionStart_, SelectionEnd_ );
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
            if( SelectionEnd_ < SelectionStart_ )
            {
                std::swap( SelectionStart_, SelectionEnd_ );
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
            if(     SelectionStart_ > 0.0
                &&  x > ( SelectionStart_-4.0 )
                &&  x < ( SelectionStart_+4.0 ) )
            {
                SelectionMode_ = selection_mode::grab_start;
                QApplication::setOverrideCursor( Qt::SizeHorCursor );
            }
            else if(     SelectionEnd_ > 0.0
                &&  x > ( SelectionEnd_-4.0 )
                &&  x < ( SelectionEnd_+4.0 ) )
            {
                SelectionMode_ = selection_mode::grab_end;
                QApplication::setOverrideCursor( Qt::SizeHorCursor );
            }
        }
        else if(    SelectionMode_ == selection_mode::grab_start
                 || SelectionMode_ == selection_mode::grab_end )
        {
            auto& Boundary = SelectionMode_ == selection_mode::grab_start ? SelectionStart_ : SelectionEnd_;
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
