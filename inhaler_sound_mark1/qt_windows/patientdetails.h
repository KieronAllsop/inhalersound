// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G
#ifndef QT_WINDOWS_PATIENTDETAILS_H_INCLUDED
#define QT_WINDOWS_PATIENTDETAILS_H_INCLUDED
// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G

// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I
// Standard Library Includes
// none

// Qt Includes
#include <QWizard>

// Custom Includes
#include "inhaler/server.hpp"
// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I


class PatientDetails : public QWizard
{
    Q_OBJECT

public:

    using shared_schema_t = inhaler::server::shared_schema_t;

    explicit    PatientDetails  ( const shared_schema_t& Schema,
                                  QWidget* parent = 0 );

    void        accept          () Q_DECL_OVERRIDE;

private:

    shared_schema_t Schema_;
};

// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G
#endif // QT_WINDOWS_PATIENTDETAILS_H_INCLUDED
