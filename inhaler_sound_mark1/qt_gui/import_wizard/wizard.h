// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G
#ifndef QT_GUI_IMPORT_WIZARD_IMPORT_WIZARD_H_INCLUDED
#define QT_GUI_IMPORT_WIZARD_IMPORT_WIZARD_H_INCLUDED
// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G

// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I

// inhaler Includes
#include "inhaler/server.hpp"

// analysis Includes
#include "analysis/speech_spectra_settings.hpp"

// Qt Includes
#include <QWizard>

// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I

// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
namespace qt_gui {
namespace import_wizard {
// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n


//! \headerfile wizard.h
//! \author     Kieron Allsop
//!
class wizard : public QWizard
{
    Q_OBJECT

public:

    using shared_schema_t    = inhaler::server::shared_schema_t;
    using patient_t          = shared_schema_t::element_type::patient_t;
    using shared_settings_t  = std::shared_ptr<analysis::speech_spectra_settings>;

    explicit        wizard          (   const patient_t& Patient,
                                        const shared_schema_t& Schema,
                                        const shared_settings_t& Settings,
                                        QWidget* parent = 0   );

    void            accept          () Q_DECL_OVERRIDE;

private:

    shared_schema_t         Schema_;
    shared_settings_t       Settings_;

};


// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
} // end import_wizard
} // end qt_gui
// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n

// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G
#endif // QT_GUI_IMPORT_WIZARD_IMPORT_WIZARD_H_INCLUDED
