 // I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I

// Self Include
#include "qt_gui/view/explore_wave.h"

// qt_gui/view includes
#include "qt_gui/view/wave_form.h"
#include "qt_gui/view/wave_zoom.h"

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
#include <QSlider>

// C++ Standard Library Includes
#include <chrono>
#include <string>
#include <sstream>

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

, PlayPauseWave_Button_             ( new QPushButton( this ) )
, StopWave_Button_                  ( new QPushButton( this ) )
, Position_Label_                   ( new QLabel( this ) )
, PlaySelection_                    ( new QRadioButton( this ) )
, ClearSelection_                   ( new QPushButton( this ) )
, StartZoomPosition_Label_          ( new QLabel( this ) )
, EndZoomPosition_Label_            ( new QLabel( this ) )
, LabelWave_Label_                  ( new QLabel( this ) )

, LabelWave_LineEdit_               ( new QLineEdit( this ) )
, AddWaveLabel_Button_              ( new QPushButton( this ) )
, RemoveLabelRow_Button_            ( new QPushButton( this ) )
, EditLabelRow_Button_              ( new QPushButton( this ) )
, CancelEditLabel_Button_           ( new QPushButton( this ) )
, CommitToDatabase_Button_          ( new QPushButton( this ) )
, Revert_Button_                    ( new QPushButton( this ) )
, LabelTreeView_                    ( new QTreeView( this ) )
, LabelModel_                       ( new QStandardItemModel( this ) )

, ZoomSample_Slider_                ( new QSlider( this ) )
, MinSample_Label_                  ( new QLabel( this ) )
, MaxSample_Label_                  ( new QLabel( this ) )
, ZoomSlider_Label_                 ( new QLabel( this ) )
, ZoomIncrement_Label_              ( new QLabel( this ) )
, Start_FineTune_Lower_             ( new QPushButton( this ) )
, Start_FineTune_Higher_            ( new QPushButton( this ) )
, End_FineTune_Lower_               ( new QPushButton( this ) )
, End_FineTune_Higher_              ( new QPushButton( this ) )

, SelectionMade_                    ( false )
, RowSelected_                      ( false )
, BeingEdited_                      ( false )
, LabelFileChanged_                 ( false )
, SelectedRow_                      ( -1 )
, EditedRow_                        ( -1 )
, ZoomIncrement_                    ( 10 )
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
    if( Player_ )
    {
        Player_->stop();
    }
}


void explore_wave::
initialise_widgets()
{
    PlayPauseWave_Button_->setIcon( style()->standardIcon( QStyle::SP_MediaPlay ) );
    PlayPauseWave_Button_->setToolTip( "Play" );
    PlayPauseWave_Button_->setToolTipDuration( 1000 );

    StopWave_Button_->setIcon( style()->standardIcon( QStyle::SP_MediaStop ) );
    StopWave_Button_->setToolTip( "Stop" );
    StopWave_Button_->setToolTipDuration( 1000 );

    PlaySelection_->setText( "Selection Only" );
    PlaySelection_->setEnabled( false );

    ClearSelection_->setText( "Clear Selection" );
    ClearSelection_->setEnabled( false );

    StartZoomPosition_Label_->setText( "Start Sample" );
    EndZoomPosition_Label_->setText( "End Sample" );

    LabelWave_Label_->setText( "Enter Event" );

    LabelWave_LineEdit_->setEnabled( false );
    LabelWave_LineEdit_->setClearButtonEnabled( true );

    AddWaveLabel_Button_->setText( "Add Label" );
    AddWaveLabel_Button_->setEnabled( false );

    RemoveLabelRow_Button_->setText( "Delete Row" );
    RemoveLabelRow_Button_->setEnabled( false );

    EditLabelRow_Button_->setText( "Edit Row" );
    EditLabelRow_Button_->setEnabled( false );

    CancelEditLabel_Button_->setText( "Cancel Edit" );
    CancelEditLabel_Button_->setEnabled( false );

    CommitToDatabase_Button_->setText( "Commit changes" );
    CommitToDatabase_Button_->setEnabled( false );

    Revert_Button_->setText( "Revert changes" );
    Revert_Button_->setEnabled( false );

    LabelTreeView_->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding ) );
    LabelTreeView_->setModel( LabelModel_ );
    LabelTreeView_->setSelectionBehavior( QTreeView::SelectRows );
    LabelTreeView_->setUniformRowHeights( true );
    LabelTreeView_->setSelectionMode( QTreeView::SingleSelection );
    LabelTreeView_->setEditTriggers( QTreeView::NoEditTriggers );

    LabelModel_->setColumnCount( 3 );

    set_label_headers();

    ZoomSample_Slider_->setEnabled( false );
    ZoomSample_Slider_->setOrientation(Qt::Horizontal);
    ZoomSample_Slider_->setMinimum( 1 );
    ZoomSample_Slider_->setMaximum( 100 );
    ZoomSample_Slider_->setTickInterval( 10 );
    ZoomSample_Slider_->setSliderPosition( 10 );
    ZoomSample_Slider_->setSingleStep( 1 );
    ZoomSample_Slider_->setPageStep( 10 );
    ZoomSample_Slider_->setTickPosition( QSlider::TicksAbove );

    MinSample_Label_->setText( "1" );
    MaxSample_Label_->setText( "100" );

    ZoomSlider_Label_->setText( "Zoom Precision =" );

    ZoomIncrement_Label_->setText( "10 samples" );

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

    disable_playing();
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

    QHBoxLayout* SliderLayout = new QHBoxLayout();

    SliderLayout->addWidget( MinSample_Label_ );
    SliderLayout->addWidget( ZoomSample_Slider_ );
    SliderLayout->addWidget( MaxSample_Label_ );

    QHBoxLayout* SliderLabelLayout = new QHBoxLayout();

    SliderLabelLayout->addStretch();
    SliderLabelLayout->addWidget( ZoomSlider_Label_ );
    SliderLabelLayout->addWidget( ZoomIncrement_Label_ );
    SliderLabelLayout->addStretch();

    QHBoxLayout* LeftWaveZoomLabelLayout = new QHBoxLayout();

    LeftWaveZoomLabelLayout->addStretch();
    LeftWaveZoomLabelLayout->addWidget( Start_FineTune_Lower_ );
    LeftWaveZoomLabelLayout->addWidget( StartZoomPosition_Label_ );
    LeftWaveZoomLabelLayout->addWidget( Start_FineTune_Higher_ );
    LeftWaveZoomLabelLayout->addStretch();

    QHBoxLayout* RightWaveZoomLabelLayout = new QHBoxLayout();

    RightWaveZoomLabelLayout->addStretch();
    RightWaveZoomLabelLayout->addWidget( End_FineTune_Lower_ );
    RightWaveZoomLabelLayout->addWidget( EndZoomPosition_Label_ );
    RightWaveZoomLabelLayout->addWidget( End_FineTune_Higher_ );
    RightWaveZoomLabelLayout->addStretch();

    WaveZoomLabelLayout->addLayout( LeftWaveZoomLabelLayout, 4 );
    WaveZoomLabelLayout->addLayout( SliderLayout, 2 );
    WaveZoomLabelLayout->addLayout( RightWaveZoomLabelLayout, 4 );

    QHBoxLayout* ViewEditDeleteRow = new QHBoxLayout();

    ViewEditDeleteRow->addWidget( EditLabelRow_Button_ );
    ViewEditDeleteRow->addWidget( CancelEditLabel_Button_ );
    ViewEditDeleteRow->addWidget( RemoveLabelRow_Button_ );

    QHBoxLayout* DatabaseInteractionRow = new QHBoxLayout();

    DatabaseInteractionRow->addWidget( CommitToDatabase_Button_ );
    DatabaseInteractionRow->addWidget( Revert_Button_ );

    QHBoxLayout* WordEntryLayout = new QHBoxLayout();

    WordEntryLayout->addWidget( LabelWave_Label_, 0, Qt::AlignLeft );
    WordEntryLayout->addWidget( LabelWave_LineEdit_ );

    QVBoxLayout* LabelDetailEntry = new QVBoxLayout();

    LabelDetailEntry->addLayout( SliderLabelLayout );
    LabelDetailEntry->addStretch();
    LabelDetailEntry->addLayout( WordEntryLayout );
    LabelDetailEntry->addWidget( AddWaveLabel_Button_ );
    LabelDetailEntry->addWidget( LabelTreeView_ );
    LabelDetailEntry->addLayout( ViewEditDeleteRow );
    LabelDetailEntry->addLayout( DatabaseInteractionRow );


    QHBoxLayout* WaveZoomLayout = new QHBoxLayout();

    WaveZoomStartView_->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding ) );
    WaveZoomEndView_->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding ) );

    WaveZoomLayout->addWidget( WaveZoomStartView_, 4 );
    WaveZoomLayout->addLayout( LabelDetailEntry, 2 );
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
connect_event_handlers()
{
    connect( PlayPauseWave_Button_, &QPushButton::released, [this](){ on_play_pause_wave(); } );
    connect( StopWave_Button_, &QPushButton::released, [this](){ on_stop_wave(); } );
    connect( PlaySelection_, &QRadioButton::clicked, [this]( const bool& Checked ){ on_selected_wave( Checked ); } );
    connect( ClearSelection_, &QPushButton::released, [this](){ on_clear_selection(); } );
    connect( LabelWave_LineEdit_, &QLineEdit::textChanged, [this](){ on_label_typed(); } );
    connect( RemoveLabelRow_Button_, &QPushButton::released, [this](){ on_remove_wave_label(); } );
    connect( AddWaveLabel_Button_, &QPushButton::released, [this](){ on_add_wave_label(); } );
    connect( EditLabelRow_Button_, &QPushButton::released, [this](){ on_edit_wave_label(); } );
    connect( CancelEditLabel_Button_, &QPushButton::released, [this](){ on_cancel_edit_label(); } );
    connect( Revert_Button_, &QPushButton::released, [this](){ on_revert_changes(); } );
    connect( Start_FineTune_Lower_, &QPushButton::pressed, [this](){ on_start_left_arrow(); } );
    connect( Start_FineTune_Higher_, &QPushButton::pressed, [this](){ on_start_right_arrow(); } );
    connect( End_FineTune_Lower_, &QPushButton::pressed, [this](){ on_end_left_arrow(); } );
    connect( End_FineTune_Higher_, &QPushButton::pressed, [this](){ on_end_right_arrow(); } );
    connect( ZoomSample_Slider_, &QSlider::valueChanged, [this](){ on_slider_changed(); } );
    connect( CommitToDatabase_Button_, &QPushButton::released, [this](){ on_commit_changes(); } );


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
reset_interface()
{
    /// TODO if needed
}


void explore_wave::
on_revert_changes()
{
    LabelTreeView_->setEnabled( false );

    QMessageBox Confirm;
    Confirm.setText("Revert Changes");
    Confirm.setInformativeText( "Are you sure you want revert all changes?" );
    Confirm.setStandardButtons( QMessageBox::Ok | QMessageBox::Cancel );
    Confirm.setDefaultButton( QMessageBox::Ok );

    if( Confirm.exec() == QMessageBox::Ok )
    {
        LabelData_.clear();
        for( const auto& OldLabelRow: OldLabelData_)
        {
            LabelData_.push_back( OldLabelRow );
        }
        LabelFileChanged_ = false;
        Revert_Button_->setEnabled( false );
        LabelModel_->clear();
        set_label_headers();

        if( LabelData_.size() > 0 )
        {
            for( const auto& LabelRow: LabelData_)
            {
                QList<QStandardItem*> Items;
                Items.append( new QStandardItem( QString( tr( "%1" ).arg( LabelRow.label_start() ) ) ) );
                Items.append( new QStandardItem( QString( tr( "%1" ).arg( LabelRow.label_end() ) ) ) );
                Items.append( new QStandardItem( QString::fromStdString( LabelRow.label_name() ) ) );
                LabelModel_->appendRow( Items );
            }
        }
    }
    RemoveLabelRow_Button_->setEnabled( false );
    EditLabelRow_Button_->setEnabled( false );
    LabelTreeView_->setEnabled( true );
    LabelTreeView_->reset();
}


void explore_wave::
on_cancel_edit_label()
{
    BeingEdited_ = false;
    RowSelected_ = false;
    LabelWave_LineEdit_->clear();
    LabelWave_LineEdit_->setEnabled( false );
    RemoveLabelRow_Button_->setEnabled( false );
    EditLabelRow_Button_->setEnabled( false );
    CancelEditLabel_Button_->setEnabled( false );
    EditedRow_ = -1;
    SelectedRow_ = -1;
    on_clear_selection();
    LabelTreeView_->setEnabled( true );
    LabelTreeView_->reset();
}


void explore_wave::
on_slider_changed()
{
    int SliderValue = ZoomSample_Slider_->sliderPosition();
    ZoomIncrement_ = SliderValue;
    if( SliderValue == 1 )
    {
        ZoomIncrement_Label_->setText( QString( tr( "%1 sample").arg( QString::fromStdString( to_string( ZoomIncrement_) ) ) ) );
    }
    else
    {
        ZoomIncrement_Label_->setText( QString( tr( "%1 samples").arg( QString::fromStdString( to_string( ZoomIncrement_) ) ) ) );
    }
}


std::string explore_wave::
to_string( const std::size_t& Sample )
{
    std::stringstream StringStream;
    StringStream << Sample;
    return StringStream.str();
}


void explore_wave::
on_commit_changes()
{
    LabelTreeView_->setEnabled( false );

    QMessageBox Confirm;
    Confirm.setText("Commit Changes");
    Confirm.setInformativeText( "Are you sure you want to commit this label file to the database?" );
    Confirm.setStandardButtons( QMessageBox::Ok | QMessageBox::Cancel );
    Confirm.setDefaultButton( QMessageBox::Ok );

    if( Confirm.exec() == QMessageBox::Ok )
    {
        LabelEditor_->delete_all_wave_labels();
        LabelEditor_->add_wave_labels( LabelData_ );
        Revert_Button_->setEnabled( false );
        OldLabelData_.clear();
        for( const auto& LabelRow: LabelData_)
        {
            OldLabelData_.push_back( LabelRow );
        }
        LabelFileChanged_ = false;
        CommitToDatabase_Button_->setEnabled( false );
    }
    RemoveLabelRow_Button_->setEnabled( false );
    EditLabelRow_Button_->setEnabled( false );
    LabelTreeView_->setEnabled( true );
    LabelTreeView_->reset();
}


void explore_wave::
on_edit_wave_label()
{
    if( RowSelected_ )
    {
        LabelTreeView_->setEnabled( false );

        BeingEdited_ = true;
        CommitToDatabase_Button_->setEnabled( false );
        CancelEditLabel_Button_->setEnabled( true );
        RemoveLabelRow_Button_->setEnabled( false );
        EditLabelRow_Button_->setEnabled( false );

        EditedRow_ = SelectedRow_;

        std::string Label = LabelData_[SelectedRow_].label_name();
        LabelWave_LineEdit_->setText( QString::fromStdString( Label ) );
        WaveFormView_->reset_play_position();

        StartSample_ = LabelData_[SelectedRow_].label_start();
        auto LabelStart = ( std::chrono::nanoseconds( StartSample_ * 1'000'000'000 / Data_->format().sample_rate() ) );
        WaveFormView_->set_selection_start( LabelStart );

        EndSample_ = LabelData_[SelectedRow_].label_end();
        auto LabelEnd = ( std::chrono::nanoseconds( EndSample_ * 1'000'000'000 / Data_->format().sample_rate() ) );
        WaveFormView_->set_selection_end( LabelEnd );

        PlaySelection_->click();
        PlaySelection_->click();
    }
}


void explore_wave::
on_add_wave_label()
{
    LabelTreeView_->setEnabled( false );

    if( BeingEdited_ )
    {
        auto Item = qt::audio::labelled_vocabulary( LabelWave_LineEdit_->text().toStdString(),
                                                    StartSample_, EndSample_ );
        LabelData_[ EditedRow_ ] = Item;

        QList<QStandardItem*> Items;
        Items.append( new QStandardItem( QString( tr( "%1" ).arg( StartSample_ ) ) ) );
        Items.append( new QStandardItem( QString( tr( "%1" ).arg( EndSample_ ) ) ) );
        Items.append( new QStandardItem( QString( LabelWave_LineEdit_->text() ) ) );

        LabelModel_->removeRows( EditedRow_, 1 );
        LabelModel_->insertRow( EditedRow_, Items );
        BeingEdited_ = false;
        CancelEditLabel_Button_->setEnabled( false );
        EditedRow_ = -1;
        SelectedRow_ = -1;

    }
    else
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

    LabelWave_LineEdit_->clear();
    LabelWave_LineEdit_->setEnabled( false );
    RemoveLabelRow_Button_->setEnabled( false );
    EditLabelRow_Button_->setEnabled( false );
    LabelTreeView_->setEnabled( true );
    LabelTreeView_->reset();

    if( OldLabelData_.size() > 0 )
    {
        Revert_Button_->setEnabled( true );
    }

    LabelFileChanged_ = true;
    CommitToDatabase_Button_->setEnabled( true );
}


void explore_wave::
on_remove_wave_label()
{
    if( RowSelected_ )
    {
        LabelTreeView_->setEnabled( false );

        QMessageBox Confirm;
        Confirm.setText("Delete Row");
        Confirm.setInformativeText( "Are you sure you want to delete this label?" );
        Confirm.setStandardButtons( QMessageBox::Ok | QMessageBox::Cancel );
        Confirm.setDefaultButton( QMessageBox::Ok );

        if( Confirm.exec() == QMessageBox::Ok )
        {
            LabelData_.erase( LabelData_.begin() + SelectedRow_ );
            LabelModel_->removeRows( SelectedRow_, 1 );
            CommitToDatabase_Button_->setEnabled( true );
        }
        RowSelected_ = false;
        SelectedRow_ = -1;
        RemoveLabelRow_Button_->setEnabled( false );
        EditLabelRow_Button_->setEnabled( false );

        if( OldLabelData_.size() > 0 || LabelFileChanged_ )
        {
            Revert_Button_->setEnabled( true );
        }
        LabelTreeView_->setEnabled( true );
        LabelTreeView_->reset();
    }
}


void explore_wave::
on_row_selection_changed( const QModelIndex& Current, const QModelIndex& Previous )
{
    RowSelected_ = true;
    RemoveLabelRow_Button_->setEnabled( true );
    EditLabelRow_Button_->setEnabled( true );
    SelectedRow_ = Current.row();
}


void explore_wave::
set_label_headers()
{
    QStringList Headers;
    Headers
        << "Start"
        << "End"
        << "Event";
    LabelModel_->setHorizontalHeaderLabels( Headers );
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
    ZoomSample_Slider_->setEnabled( true );

    LabelEditor_ = std::make_shared<label_editor_t>( DataRetriever_->schema(), DataRetriever_->patient(), WaveDetails );

    LabelData_.clear();
    OldLabelData_.clear();
    LabelModel_->clear();
    set_label_headers();

    LabelData_ = LabelEditor_->retrieve_wave_labels();

    if( LabelData_.size() > 0 )
    {
        for( const auto& LabelRow: LabelData_)
        {
            QList<QStandardItem*> Items;
            Items.append( new QStandardItem( QString( tr( "%1" ).arg( LabelRow.label_start() ) ) ) );
            Items.append( new QStandardItem( QString( tr( "%1" ).arg( LabelRow.label_end() ) ) ) );
            Items.append( new QStandardItem( QString::fromStdString( LabelRow.label_name() ) ) );
            LabelModel_->appendRow( Items );
            OldLabelData_.push_back( LabelRow );
        }
    }

    StartSample_ = 0;
    EndSample_ = 0;
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

    if( ClearSelection_->isEnabled() )
    {
        ClearSelection_->setEnabled( false );
        on_clear_selection();
    }

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


void explore_wave::
enable_playing()
{
    PlayPauseWave_Button_->setEnabled( true );
    StopWave_Button_->setEnabled( true );
}


void explore_wave::
disable_playing()
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
    std::size_t NewSample = StartSample_ - ZoomIncrement_;
    auto TimeChanged = ( std::chrono::nanoseconds( NewSample * 1'000'000'000 / Data_->format().sample_rate() ) );
    WaveFormView_->set_selection_start( TimeChanged );
}


void explore_wave::
on_start_right_arrow()
{
    std::size_t NewSample = StartSample_ + ZoomIncrement_;
    auto TimeChanged = ( std::chrono::nanoseconds( NewSample * 1'000'000'000 / Data_->format().sample_rate() ) );
    WaveFormView_->set_selection_start( TimeChanged );
}


void explore_wave::
on_end_left_arrow()
{
    std::size_t NewSample = EndSample_ - ZoomIncrement_;
    auto TimeChanged = ( std::chrono::nanoseconds( NewSample * 1'000'000'000 / Data_->format().sample_rate() ) );
    WaveFormView_->set_selection_end( TimeChanged );
}


void explore_wave::
on_end_right_arrow()
{
    std::size_t NewSample = EndSample_ + ZoomIncrement_;
    auto TimeChanged = ( std::chrono::nanoseconds( NewSample * 1'000'000'000 / Data_->format().sample_rate() ) );
    WaveFormView_->set_selection_end( TimeChanged );
}


void explore_wave::
on_label_typed()
{
    if( LabelWave_LineEdit_->text().length() == 0 )
    {
        AddWaveLabel_Button_->setEnabled( false );
    }
    else
    {
        AddWaveLabel_Button_->setEnabled( true );
    }
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
            && ( SelectionStart_ != Start || SelectionEnd_ != End || Start == End ) )
    {
        PlaySelection_->click();
        PlaySelection_->click();
    }

    SelectionStart_ = Start;
    SelectionEnd_ = End;
    StartSample_ = StartSample;
    EndSample_ = EndSample;

    if( SelectionStart_ > std::chrono::nanoseconds( 0 ) )
    {
        SelectionMade_ = true;
        PlaySelection_->setEnabled( true );
    }

    WaveZoomStartView_->set_play_position( Start );
    WaveZoomEndView_->set_play_position( End );

    set_zoom_sample_labels();

    LabelWave_LineEdit_->setEnabled( true );
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
    Start_FineTune_Lower_->setEnabled( false );
    Start_FineTune_Higher_->setEnabled( false );
    End_FineTune_Lower_->setEnabled( false );
    End_FineTune_Higher_->setEnabled( false );
    StartSample_ = 0;
    EndSample_ = 0;
    SelectionStart_ = std::chrono::nanoseconds( 0 );
    SelectionEnd_ = std::chrono::nanoseconds( 0 );

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
