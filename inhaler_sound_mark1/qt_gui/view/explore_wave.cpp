// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I

// Self Include
#include "qt_gui/view/explore_wave.h"

// qt_gui/view includes
#include "qt_gui/view/wave_form.h"

// Qt Includes
#include <QPushButton>
#include <QLabel>
#include <QSizePolicy>
#include <QUrl>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QStyle>
#include <QRadioButton>

// C++ Standard Library Includes
#include <chrono>

// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I

// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
namespace qt_gui {
namespace view {
// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n

explore_wave::
explore_wave
(   QWidget* Parent )
: QFrame( Parent )
, SelectionStart_		( std::chrono::nanoseconds(0) )
, SelectionEnd_			( std::chrono::nanoseconds(0) )
, TimestampFacet_       ( new boost::posix_time::time_facet() )
, TimestampLocale_      ( std::locale(), TimestampFacet_ )

// Create Widgets
, WaveName_Label_ ( new QLabel( tr("<h2>No Wave Selected</h2>"), this ) )
, WaveView_Frame_ ( new QFrame( this ) )

, WaveFormView_
    (   new qt_gui::view::wave_form
        (   [this]( const std::chrono::nanoseconds& Start, const std::chrono::nanoseconds& End )
            {
                handler_selection_update( Start, End );
            },
            this   )   )

, PlayPauseWave_Button_ ( new QPushButton( this ) )
, StopWave_Button_      ( new QPushButton( this ) )
, Position_Label_       ( new QLabel( this ) )
, PlaySelection_        ( new QRadioButton( this ) )
, ClearSelection_       ( new QPushButton( this ) )
, SelectionMade_        ( false )
{
    TimestampFacet_->format( "%Y-%m-%d %H:%M" );

    initialise_widgets();

    initialise_layout();

    connect_event_handlers();

    reset_interface();
}


explore_wave::
~explore_wave()
{
    Player_->stop();
}


void explore_wave::connect_event_handlers()
{
    connect( PlayPauseWave_Button_, &QPushButton::released, [this](){ on_play_pause_wave(); } );
    connect( StopWave_Button_, &QPushButton::released, [this](){ on_stop_wave(); } );
    connect( PlaySelection_, &QRadioButton::clicked, [this]( const bool& Checked ){ on_selected_wave( Checked );} );
    connect( ClearSelection_, &QPushButton::released, [this](){ on_clear_selection(); } );
}


void explore_wave::initialise_widgets()
{
    PlayPauseWave_Button_->setIcon( style()->standardIcon( QStyle::SP_MediaPlay ) );
    StopWave_Button_->setIcon( style()->standardIcon( QStyle::SP_MediaStop ) );
    PlaySelection_->setText( "Selection Only" );
    PlaySelection_->setEnabled( false );
    ClearSelection_->setText( "Clear Selection" );
    ClearSelection_->setEnabled( false );
    disable_playing();
}


void explore_wave::reset_interface()
{
    /// TODO if needed
}


void explore_wave::initialise_layout()
{
    QVBoxLayout* MasterLayout = new QVBoxLayout();

    WaveView_Frame_->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding ) );
    WaveView_Frame_->setFrameStyle( QFrame::StyledPanel | QFrame::Sunken );

    QHBoxLayout* WaveButtonsLayout = new QHBoxLayout();

    WaveButtonsLayout->addWidget( PlayPauseWave_Button_, 0, Qt::AlignLeft );
    WaveButtonsLayout->addWidget( PlaySelection_, 0, Qt::AlignLeft );
    WaveButtonsLayout->addWidget( StopWave_Button_, 0, Qt::AlignLeft );
    WaveButtonsLayout->addWidget( ClearSelection_, 0, Qt::AlignLeft );
    WaveButtonsLayout->addStretch();
    WaveButtonsLayout->addWidget( Position_Label_, 0, Qt::AlignRight );

    QVBoxLayout* WaveFrameLayout = new QVBoxLayout();

    WaveFormView_->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding ) );
    WaveFrameLayout->addLayout( WaveButtonsLayout );
    WaveFrameLayout->addWidget( WaveFormView_ );
    WaveView_Frame_->setLayout( WaveFrameLayout );

    MasterLayout->addWidget( WaveName_Label_, 0, Qt::AlignLeft );
    MasterLayout->addWidget( WaveView_Frame_ );

    setLayout( MasterLayout );
}


void explore_wave::
reset
(   const patient_wave_details_t& WaveDetails,
    const shared_data_retriever_t& DataRetriever,
    const shared_data_t& Data )
{
    if( Player_ )
    {
        Player_->stop();
    }

    WaveDetails_ = WaveDetails;
    DataRetriever_ = DataRetriever;
    WaveFormView_->reset( Data );

    SelectionStart_ = std::chrono::nanoseconds( 0 );
    SelectionEnd_   = std::chrono::nanoseconds( 0 );

    // Display wave details
    WaveName_Label_
        ->setText
            ( tr( "<h2>%1 (from Inhaler <i>%2</i> recorded at <i>%3</i>)</h2>" )
                .arg( QString::fromUtf8( WaveDetails.name().c_str() ) )
                .arg( QString::fromUtf8( WaveDetails.inhaler_model().c_str() ) )
                .arg( QString::fromUtf8( to_string( WaveDetails.modified_time() ).c_str() ) ) );

    set_play_position( std::chrono::milliseconds( 0 ) );

    PlayPauseWave_Button_->setIcon( style()->standardIcon( QStyle::SP_MediaPlay ) );
    PlayPauseWave_Button_->update();

    if( !Data->linked_file().empty() )
    {
        Player_
            = std::make_shared<player_t>
                (   Data->linked_file(),
                    Data->duration(),
                    [this]( player_t::play_status_t Status, const player_t::milliseconds_t& Position )
                    {
                        handle_player_status( Status, Position );
                    },
                    [this]( player_t::probe_status_t Status, const player_t::buffer_t& Buffer )
                    {
                        handle_player_buffer( Status, Buffer );
                    }
                );

        enable_playing();
    }
    else
    {
        disable_playing();
    }
}


void explore_wave::enable_playing()
{
    PlayPauseWave_Button_->setEnabled( true );
    StopWave_Button_->setEnabled( true );
}


void explore_wave::disable_playing()
{
    PlayPauseWave_Button_->setEnabled( false );
    StopWave_Button_->setEnabled( false );
}


std::string explore_wave::
to_string( const boost::posix_time::ptime& Timestamp ) const
{
    std::stringstream Stream;
    Stream.imbue( TimestampLocale_ );
    Stream << Timestamp;
    return Stream.str();
}


void explore_wave::
set_play_position( const std::chrono::milliseconds& Position )
{
    std::chrono::duration<double> Time = Position;
    Position_Label_->setText( QString( "%1 secs" ).arg( Time.count(), 0, 'f', 3 ) );
    WaveFormView_->set_play_position( Position );
}


void explore_wave::
handler_selection_update
(   const std::chrono::nanoseconds& Start,
    const std::chrono::nanoseconds& End   )
{
    if( PlaySelection_->isEnabled()
            && SelectionMade_ == true
            && ( SelectionStart_ != Start || SelectionEnd_ != End ) )
    {
        PlaySelection_->click();
        PlaySelection_->click();
    }

    SelectionStart_ = Start;
    SelectionEnd_ = End;
    SelectionMade_ = true;

    PlaySelection_->setEnabled( true );
    ClearSelection_->setEnabled( true );
}


void explore_wave::
on_clear_selection()
{
    WaveFormView_->reset_play_position();
    Player_->clear_play_selection();
    Player_->reset_player();
    SelectionMade_ = false;
    PlaySelection_->setChecked( false );
    PlaySelection_->setEnabled( false );
    ClearSelection_->setEnabled( false );
}


void explore_wave::
on_selected_wave( const bool& Checked )
{
    if( Checked )
    {
        Player_->set_play_selection( SelectionStart_, SelectionEnd_ );
        Player_->reset_player();
    }
    else
    {
        Player_->clear_play_selection();
        Player_->reset_player();
    }
}


void explore_wave::
on_play_pause_wave()
{
    Player_->play_pause();
}


void explore_wave::
on_stop_wave()
{
    Player_->stop();
}


void explore_wave::
handle_player_status
(   player_t::play_status_t Status,
    const player_t::milliseconds_t& Position   )
{
    using play_status_t = player_t::play_status_t;

    switch( Status )
    {
        case play_status_t::playing :
        {
            PlayPauseWave_Button_->setIcon( style()->standardIcon( QStyle::SP_MediaPause ) );
            PlayPauseWave_Button_->update();
            set_play_position( Position );
            break;
        }
        case play_status_t::paused :
        {
            PlayPauseWave_Button_->setIcon( style()->standardIcon( QStyle::SP_MediaPlay ) );
            PlayPauseWave_Button_->update();
            set_play_position( Position );
            break;
        }
        case play_status_t::stopped :
        {
            PlayPauseWave_Button_->setIcon( style()->standardIcon( QStyle::SP_MediaPlay ) );
            PlayPauseWave_Button_->update();
            set_play_position( Position );
            break;
        }
        case play_status_t::position_changed :
        {
            set_play_position( Position );
            break;
        }
    }
}


void explore_wave::
handle_player_buffer
(   player_t::probe_status_t Status,
    const player_t::buffer_t& Buffer   )
{
    if( Status == player_t::probe_status_t::buffer_ready )
    {
        /// TODO - display dB value? live spectrum?
        for( unsigned s = 0; s < Buffer.samples_per_channel(); ++s )
        {
            for( unsigned c = 0; c < Buffer.format().channel_count(); ++c )
            {
//                 std::cout << Buffer.normalised_sample( s, c ) << std::endl;
            }
        }
    }
}


// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
} // end view
} // end qt_gui
// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n