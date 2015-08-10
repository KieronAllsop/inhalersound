// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G
#ifndef QT_GUI_IMPORT_WIZARD_IMPORT_WIZARD_H_INCLUDED
#define QT_GUI_IMPORT_WIZARD_IMPORT_WIZARD_H_INCLUDED
// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G

// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I

// Custom Includes
#include "inhaler/server.hpp"

// Qt Includes
#include <QWizard>

// C++ Standard Library Includes
// none

// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I

// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
namespace qt_gui {
namespace import_wizard {
// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n

class wizard : public QWizard
{
    Q_OBJECT

public:

    using shared_schema_t    = inhaler::server::shared_schema_t;
    using patient_t          = shared_schema_t::element_type::patient_t;

    explicit        wizard          (   const patient_t& Patient,
                                        const shared_schema_t& Schema,
                                        QWidget* parent = 0   );

    void            accept          () Q_DECL_OVERRIDE;

private:

    shared_schema_t Schema_;
};

// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
} // end import_wizard
} // end qt_gui
// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n

// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G
#endif // QT_GUI_IMPORT_WIZARD_IMPORT_WIZARD_H_INCLUDED
