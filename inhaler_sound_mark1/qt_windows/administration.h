// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G
#ifndef ADMINISTRATION_H
#define ADMINISTRATION_H
// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G

// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I
// Standard Library Includes
// none

// Qt Includes
#include <QDialog>

// Custom Includes
#include "inhaler/server.hpp"
// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I


namespace Ui
{
    class Administration;
}

class Administration : public QDialog
{
    Q_OBJECT

public:

    using shared_schema_t = inhaler::server::shared_schema_t;

    explicit Administration ( const shared_schema_t& Schema,
                              QWidget* Parent = 0);

             ~Administration();

private:
    Ui::Administration *Ui_;
    shared_schema_t Schema_;
};

// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G
#endif // ADMINISTRATION_H

