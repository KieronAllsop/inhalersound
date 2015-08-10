// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G
#ifndef QT_GUI_IMPORT_WIZARD_PROCESS_FILES_PAGE_H_INCLUDED
#define QT_GUI_IMPORT_WIZARD_PROCESS_FILES_PAGE_H_INCLUDED
// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G

// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I

// Inhaler Includes
#include "inhaler/wave_details.hpp"
#include "inhaler/wave_importer.hpp"

// Qt Includes
#include <QWizardPage>
#include <QEvent>

// C++ Standard Library Includes
#include <atomic>
#include <thread>

// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I

// Forward Declarations
class QLabel;
class QPushButton;
class QProgressBar;



// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
namespace qt_gui {
namespace import_wizard {
// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n



class import_status_event : public QEvent
{
public:

    using wave_details_t    = inhaler::wave_details;
    using import_status_t   = inhaler::import_status;

public:

    static QEvent::Type type()
    {
        static auto Type = QEvent::Type( QEvent::registerEventType() );
        return Type;
    }

public:

    import_status_event
        (   const wave_details_t& WaveFile,
            int Number,
            import_status_t Status   )
    : QEvent( type() )
    , WaveFile_ ( WaveFile )
    , Number_   ( Number )
    , Status_   ( Status )
    {
    }

    const wave_details_t& wave_file () const
    {
        return WaveFile_;
    }

    int number() const
    {
        return Number_;
    }

    const import_status_t&  import_status() const
    {
        return Status_;
    }

private:

    wave_details_t  WaveFile_;
    int             Number_;
    import_status_t Status_;
};


class process_files_page : public QWizardPage
{
    Q_OBJECT

public:

    using shared_importer_t = std::shared_ptr<inhaler::wave_importer>;
    using wave_details_t    = inhaler::wave_details;
    using import_status_t   = inhaler::import_status;

public:

                            process_files_page          (   const shared_importer_t& Importer,
                                                            QWidget* Parent=0   );

                            ~process_files_page         ();

    virtual void            initializePage              ();

    virtual bool            isComplete                  () const;

    virtual bool            event                       (   QEvent* Event   );

private:

    void                    start_processing_files      ();

    void                    process_files               ();

    void                    on_import_status            (   const wave_details_t& WaveFile,
                                                            int Number,
                                                            import_status_t Status   );


private:
    // Data Variables
    shared_importer_t   Importer_;
    std::atomic<bool>   Cancelled_;
    std::atomic<bool>   Complete_;
    std::thread         ProcessingThread_;

    // Owned Widgets
    QLabel*             ProcessingFiles_Label_;
    QLabel*             ProgressLabel_;
    QProgressBar*       ProgressBar_;

};

// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
} // end import_wizard
} // end qt_gui
// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n


// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G
#endif // QT_GUI_IMPORT_WIZARD_PROCESS_FILES_PAGE_H_INCLUDED
