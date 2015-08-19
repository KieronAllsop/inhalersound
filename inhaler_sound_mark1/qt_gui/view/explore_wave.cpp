// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I

// Self Include
#include "qt_gui/view/explore_wave.h"

// qt_gui/view includes
#include "qt_gui/view/wave_form.h"
#include "qt_gui/view/wave_zoom.h"
//#include "qt_gui/view/wave_zoom_start.h"
//#include "qt_gui/view/wave_zoom_end.h"

// Qt Includes
#include <QPushButton>
#include <QLabel>
#include <QSizePolicy>
#include <QUrl>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QStyle>
#include <QRadioButton>
#include <QLineEdit>
#include <QTreeView>
#include <QStandardItemModel>
#include <QStringList>
#include <QMessageBox>

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
, WaveZoom_Frame_ ( new QFrame( this ) )

, WaveFormView_
    (   new qt_gui::view::wave_form
        (   [this]( const std::chrono::nanoseconds& Start, const std::chrono::nanoseconds& End,
                    const std::size_t& StartSample, const std::size_t& EndSample )
            {
                handler_selection_update( Start, End, StartSample, EndSample );
            },
            this   )   )

, WaveZoomStartView_
    (   new qt_gui::view::wave_zoom( this ) )

, WaveZoomEndView_
    (   new qt_gui::view::wave_zoom( this ) )

, PlayPauseWave_Button_     ( new QPushButton( this ) )
, StopWave_Button_          ( new QPushButton( this ) )
, Position_Label_           ( new QLabel( this ) )
, PlaySelection_            ( new QRadioButton( this ) )
, ClearSelection_           ( new QPushButton( this ) )
, StartZoomPosition_Label_  ( new QLabel( this ) )
, EndZoomPosition_Label_    ( new QLabel( this ) )
, LabelWave_Label_          ( new QLabel( this ) )
, LabelWave_LineEdit_       ( new QLineEdit( this ) )
, AddWaveLabel_Button_      ( new QPushButton( this ) )
, ClearWaveLineEdit_Button_ ( new QPushButton( this ) )
, RemoveLabelRow_Button_    ( new QPushButton( this ) )
, EditLabelRow_Button_      ( new QPushButton( this ) )
, LabelTreeView_            ( new QTreeView( this ) )
, LabelModel_               ( new QStandardItemModel( this ) )
, Start_FineTune_Lower_     ( new QPushButton( this ) )
, Start_FineTune_Higher_    ( new QPushButton( this ) )
, End_FineTune_Lower_       ( new QPushButton( this ) )
, End_FineTune_Higher_      ( new QPushButton( this ) )
, SelectionMade_            ( false )
, RowSelected_              ( false )
, BeingEditted_             ( false )
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


void explore_wave::
connect_event_handlers()
{
    connect( PlayPauseWave_Button_, &QPushButton::released, [this](){ on_play_pause_wave(); } );
    connect( StopWave_Button_, &QPushButton::released, [this](){ on_stop_wave(); } );
    connect( PlaySelection_, &QRadioButton::clicked, [this]( const bool& Checked ){ on_selected_wave( Checked ); } );
    connect( ClearSelection_, &QPushButton::released, [this](){ on_clear_selection(); } );
    connect( LabelWave_LineEdit_, &QLineEdit::textChanged, [this](){ on_label_typed(); } );
    connect( ClearWaveLineEdit_Button_, &QPushButton::released, [this](){ on_clear_wave_label(); } );
    connect( RemoveLabelRow_Button_, &QPushButton::released, [this](){ on_remove_wave_label(); } );
    connect( AddWaveLabel_Button_, &QPushButton::released, [this](){ on_add_wave_label(); } );
    connect( EditLabelRow_Button_, &QPushButton::released, [this](){ on_edit_wave_label(); } );
    connect( Start_FineTune_Lower_, &QPushButton::pressed, [this](){ on_start_left_arrow(); } );
    connect( Start_FineTune_Higher_, &QPushButton::pressed, [this](){ on_start_right_arrow(); } );
    connect( End_FineTune_Lower_, &QPushButton::pressed, [this](){ on_end_left_arrow(); } );
    connect( End_FineTune_Higher_, &QPushButton::pressed, [this](){ on_end_right_arrow(); } );

    connect
        (   LabelTreeView_->selectionModel(),
            &QItemSelectionModel::currentChanged,
            [this]( const QModelIndex& Current, const QModelIndex& Previous )
            {
                on_row_selection_changed( Current, Previous );
            }
        );

}


void explore_wave::
on_edit_wave_label()
{
    if( RowSelected_ )
    {
        std::string Label = LabelData_.at( SelectedRow_ ).label_name();
        LabelWave_LineEdit_->setText( QString::fromStdString( Label ) );
        WaveFormView_->reset_play_position();

        StartSample_ = LabelData_.at( SelectedRow_ ).label_start();
        auto LabelStart = ( std::chrono::nanoseconds( StartSample_ * 1000000000 / Data_->format().sample_rate() ) );
        std::cout << "LabelStart.count = " << LabelStart.count() << std::endl;
        WaveFormView_->set_label_start( LabelStart );

        EndSample_ = LabelData_.at( SelectedRow_ ).label_end();
        auto LabelEnd = ( std::chrono::nanoseconds( EndSample_ * 1000000000 / Data_->format().sample_rate() ) );
        WaveFormView_->set_label_end( LabelEnd );
    }
}


void explore_wave::
on_add_wave_label()
{
    auto Item = qt::audio::labelled_vocabulary( LabelWave_LineEdit_->text().toStdString(),
                                                StartSample_, EndSample_ );
    LabelData_.push_back( Item );

    QList<QStandardItem*> Items;
    Items.append( new QStandardItem( QString( tr( "%1" ).arg( StartSample_ ) ) ) );
    Items.append( new QStandardItem( QString( tr( "%1" ).arg( EndSample_ ) ) ) );
    Items.append( new QStandardItem( QString( LabelWave_LineEdit_->text() ) ) );
    LabelModel_->appendRow( Items );
}


void explore_wave::
on_remove_wave_label()
{
    if( RowSelected_ )
    {
        QMessageBox Confirm;
        Confirm.setText("Delete Row");
        Confirm.setInformativeText( "Are you sure you want to delete this label?" );
        Confirm.setStandardButtons( QMessageBox::Ok | QMessageBox::Cancel );
        Confirm.setDefaultButton( QMessageBox::Ok );

        if( Confirm.exec() == QMessageBox::Ok )
        {
            LabelData_.erase( LabelData_.begin() + SelectedRow_ );
            LabelModel_->removeRows( SelectedRow_, 1 );
        }
        RowSelected_ = false;
        LabelTreeView_->reset();
        SelectedRow_ = 0;
    }
}


void explore_wave::
on_row_selection_changed( const QModelIndex& Current, const QModelIndex& Previous )
{
    RowSelected_ = true;
    RemoveLabelRow_Button_->setEnabled( true );
    SelectedRow_ = Current.row();
}


void explore_wave::
initialise_widgets()
{
    PlayPauseWave_Button_->setIcon( style()->standardIcon( QStyle::SP_MediaPlay ) );
    StopWave_Button_->setIcon( style()->standardIcon( QStyle::SP_MediaStop ) );

    PlaySelection_->setText( "Selection Only" );
    PlaySelection_->setEnabled( false );
    ClearSelection_->setText( "Clear Selection" );
    ClearSelection_->setEnabled( false );

    StartZoomPosition_Label_->setText( "Start Sample" );
    EndZoomPosition_Label_->setText( "End Sample" );

    LabelWave_Label_->setText( "Enter Word" );
    LabelWave_LineEdit_->setEnabled( false );
    AddWaveLabel_Button_->setIcon( style()->standardIcon( QStyle::SP_DialogApplyButton ) );
    AddWaveLabel_Button_->setEnabled( false );
    ClearWaveLineEdit_Button_->setIcon( style()->standardIcon( QStyle::SP_LineEditClearButton ) );
    ClearWaveLineEdit_Button_->setEnabled( false );
    RemoveLabelRow_Button_->setIcon( style()->standardIcon( QStyle::SP_DialogDiscardButton ) );
    RemoveLabelRow_Button_->setEnabled( false );
    EditLabelRow_Button_->setIcon( style()->standardIcon( QStyle::SP_DialogOpenButton ) );
    EditLabelRow_Button_->setEnabled( true );  // fix this

    Start_FineTune_Lower_->setIcon( style()->standardIcon( QStyle::SP_ArrowLeft ) );
    Start_FineTune_Lower_->setEnabled( false );
    Start_FineTune_Lower_->setAutoRepeat( true );
    Start_FineTune_Higher_->setIcon( style()->standardIcon( QStyle::SP_ArrowRight ) );
    Start_FineTune_Higher_->setEnabled( false );
    Start_FineTune_Higher_->setAutoRepeat( true );
    End_FineTune_Lower_->setIcon( style()->standardIcon( QStyle::SP_ArrowLeft ) );
    End_FineTune_Lower_->setEnabled( false );
    End_FineTune_Lower_->setAutoRepeat( true );
    End_FineTune_Higher_->setIcon( style()->standardIcon( QStyle::SP_ArrowRight ) );
    End_FineTune_Higher_->setEnabled( false );
    End_FineTune_Higher_->setAutoRepeat( true );

    LabelTreeView_->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding ) );
    LabelTreeView_->setModel( LabelModel_ );
    LabelTreeView_->setSelectionBehavior( QTreeView::SelectRows );
    LabelTreeView_->setUniformRowHeights( true );
    LabelTreeView_->setSelectionMode( QTreeView::SingleSelection );

    LabelModel_->setColumnCount( 3 );

    set_label_headers();

    disable_playing();
}


void explore_wave
::set_label_headers()
{
    QStringList Headers;
    Headers
        << "Start"
        << "End"
        << "Word";
    LabelModel_->setHorizontalHeaderLabels( Headers );
}


void explore_wave::
reset_interface()
{
    /// TODO if needed
}


void explore_wave::
initialise_layout()
{
    QVBoxLayout* MasterLayout = new QVBoxLayout();

    WaveView_Frame_->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding ) );
    WaveView_Frame_->setFrameStyle( QFrame::StyledPanel | QFrame::Sunken );

    WaveZoom_Frame_->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding ) );
    WaveZoom_Frame_->setFrameStyle( QFrame::StyledPanel | QFrame::Sunken );

    QHBoxLayout* WaveButtonsLayout = new QHBoxLayout();

    WaveButtonsLayout->addWidget( PlayPauseWave_Button_, 0, Qt::AlignLeft );
    WaveButtonsLayout->addWidget( PlaySelection_, 0, Qt::AlignLeft );
    WaveButtonsLayout->addWidget( StopWave_Button_, 0, Qt::AlignLeft );
    WaveButtonsLayout->addWidget( ClearSelection_, 0, Qt::AlignLeft );
    WaveButtonsLayout->addStretch();
    WaveButtonsLayout->addWidget( Position_Label_, 0, Qt::AlignRight );

    QHBoxLayout* WaveZoomLabelLayout = new QHBoxLayout();

    WaveZoomLabelLayout->addStretch( 22 );
    WaveZoomLabelLayout->addWidget( Start_FineTune_Lower_ );
    WaveZoomLabelLayout->addWidget( StartZoomPosition_Label_ );
    WaveZoomLabelLayout->addWidget( Start_FineTune_Higher_ );
    WaveZoomLabelLayout->addStretch( 34 );
    WaveZoomLabelLayout->addWidget( LabelWave_Label_, 0, Qt::AlignCenter );
    WaveZoomLabelLayout->addStretch( 34 );
    WaveZoomLabelLayout->addWidget( End_FineTune_Lower_ );
    WaveZoomLabelLayout->addWidget( EndZoomPosition_Label_ );
    WaveZoomLabelLayout->addWidget( End_FineTune_Higher_ );
    WaveZoomLabelLayout->addStretch( 22 );

    QHBoxLayout* WaveZoomButtonsLayout = new QHBoxLayout();

    WaveZoomButtonsLayout->addWidget( AddWaveLabel_Button_, 0, Qt::AlignCenter );
    WaveZoomButtonsLayout->addWidget( ClearWaveLineEdit_Button_, 0, Qt::AlignCenter );

    QVBoxLayout* WaveZoomDetailEntry = new QVBoxLayout();

    WaveZoomDetailEntry->addWidget( LabelWave_LineEdit_ );
    WaveZoomDetailEntry->addLayout( WaveZoomButtonsLayout );
    WaveZoomDetailEntry->addWidget( LabelTreeView_ );
    WaveZoomDetailEntry->addWidget( EditLabelRow_Button_, 0, Qt::AlignCenter );
    WaveZoomDetailEntry->addWidget( RemoveLabelRow_Button_, 0, Qt::AlignCenter );
    WaveZoomDetailEntry->addStretch();

    QHBoxLayout* WaveZoomLayout = new QHBoxLayout();

    WaveZoomStartView_->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding ) );
    WaveZoomEndView_->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding ) );

    WaveZoomLayout->addWidget( WaveZoomStartView_, 4 );
    WaveZoomLayout->addLayout( WaveZoomDetailEntry, 2 );
    WaveZoomLayout->addWidget( WaveZoomEndView_, 4 );

    QVBoxLayout* WaveFrameLayout = new QVBoxLayout();

    WaveFormView_->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding ) );
    WaveFrameLayout->addLayout( WaveButtonsLayout );
    WaveFrameLayout->addWidget( WaveFormView_ );
    WaveView_Frame_->setLayout( WaveFrameLayout );

    QVBoxLayout* WaveZoomFrameLayout = new QVBoxLayout();

    WaveZoomFrameLayout->addLayout( WaveZoomLabelLayout );
    WaveZoomFrameLayout->addLayout( WaveZoomLayout );
    WaveZoom_Frame_->setLayout( WaveZoomFrameLayout );

    MasterLayout->addWidget( WaveName_Label_, 0, Qt::AlignLeft );
    MasterLayout->addWidget( WaveView_Frame_, 1, 0 );
    MasterLayout->addWidget( WaveZoom_Frame_, 1, 0 );

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

    Data_ = Data;
    WaveDetails_ = WaveDetails;
    DataRetriever_ = DataRetriever;
    WaveFormView_->reset( Data );
    WaveZoomStartView_->reset( Data );
    WaveZoomEndView_->reset( Data );

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
    Position_Label_->setText( tr( "%1 secs" ).arg( Time.count(), 0, 'f', 3 ) );
    WaveFormView_->set_play_position( Position );
}


void explore_wave::
set_zoom_sample_labels( )
{
    std::chrono::duration<double> StartTime = SelectionStart_;
    std::chrono::duration<double> EndTime = SelectionEnd_;
    StartZoomPosition_Label_->setText( tr( "Sample %1, Time %2 secs" ).arg( StartSample_ ).arg( StartTime.count(), 0, 'f', 3 ) );
    EndZoomPosition_Label_->setText( tr( "Sample %1, Time %2 secs" ).arg( EndSample_ ).arg( EndTime.count(), 0, 'f', 3 ) );
}


void explore_wave::
on_start_left_arrow()
{
    std::size_t SampleChange = 10;
    auto TimeChange = ( std::chrono::nanoseconds( SampleChange * 1000000000 / Data_->format().sample_rate() ) );
    WaveFormView_->set_selection_start( -TimeChange );
}


void explore_wave::
on_start_right_arrow()
{
    std::size_t SampleChange = 10;
    auto TimeChange = ( std::chrono::nanoseconds( SampleChange * 1000000000 / Data_->format().sample_rate() ) );
    WaveFormView_->set_selection_start( TimeChange );
}


void explore_wave::
on_end_left_arrow()
{
    std::size_t SampleChange = 10;
    auto TimeChange = ( std::chrono::nanoseconds( SampleChange * 1000000000 / Data_->format().sample_rate() ) );
    WaveFormView_->set_selection_end( -TimeChange );
}


void explore_wave::
on_end_right_arrow()
{
    std::size_t SampleChange = 10;
    auto TimeChange = ( std::chrono::nanoseconds( SampleChange * 1000000000 / Data_->format().sample_rate() ) );
    WaveFormView_->set_selection_end( TimeChange );

}


void explore_wave::
on_label_typed()
{
    AddWaveLabel_Button_->setEnabled( true );
    ClearWaveLineEdit_Button_->setEnabled( true );
}


void explore_wave::
on_clear_wave_label()
{
    LabelWave_LineEdit_->clear();
}


void explore_wave::
handler_selection_update
(   const std::chrono::nanoseconds& Start,
    const std::chrono::nanoseconds& End,
    const std::size_t& StartSample,
    const std::size_t& EndSample    )
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
    StartSample_ = StartSample;
    EndSample_ = EndSample;

    WaveZoomStartView_->set_play_position( Start );
    WaveZoomEndView_->set_play_position( End );

    set_zoom_sample_labels();

    SelectionMade_ = true;

    PlaySelection_->setEnabled( true );

    LabelWave_LineEdit_->setEnabled( true );
    AddWaveLabel_Button_->setEnabled( true );
    ClearWaveLineEdit_Button_->setEnabled( true );
    Start_FineTune_Lower_->setEnabled( true );
    Start_FineTune_Higher_->setEnabled( true );
    End_FineTune_Lower_->setEnabled( true );
    End_FineTune_Higher_->setEnabled( true );

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
    LabelWave_LineEdit_->setEnabled( false );
    AddWaveLabel_Button_->setEnabled( false );
    ClearWaveLineEdit_Button_->setEnabled( false );
    Start_FineTune_Lower_->setEnabled( false );
    Start_FineTune_Higher_->setEnabled( false );
    End_FineTune_Lower_->setEnabled( false );
    End_FineTune_Higher_->setEnabled( false );
    StartSample_ = 0;
    EndSample_ = 0;

    WaveZoomStartView_->clear();
    WaveZoomEndView_->clear();

    set_zoom_sample_labels();
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
