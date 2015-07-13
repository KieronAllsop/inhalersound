// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G
#ifndef QT_WINDOWS_PROCESS_SOUNDS_CONFIRM_PAGE_H_INCLUDED
#define QT_WINDOWS_PROCESS_SOUNDS_CONFIRM_PAGE_H_INCLUDED
// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G

// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I

// Standard Library Includes
// none

// Qt Includes
#include <QWizardPage>

// Custom Includes
#include "data_model/schema.hpp"

// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I

class ProcessSoundsConfirmFiles : public QWizardPage
{
    Q_OBJECT

public:

    using shared_schema_t = std::shared_ptr<data_model::schema>;

    ProcessSoundsConfirmFiles    (   const shared_schema_t& Schema,
                                     QWidget* Parent=0   );

private:
    // Data Variables
    shared_schema_t     Schema_;

};

// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G
#endif // QT_WINDOWS_PROCESS_SOUNDS_CONFIRM_PAGE_H_INCLUDED
