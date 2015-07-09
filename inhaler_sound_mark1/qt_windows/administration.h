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
#include "data_model/schema.hpp"
// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I


namespace Ui
{
    class Administration;
}

class Administration : public QDialog
{
    Q_OBJECT

public:

    using shared_schema_t = std::shared_ptr<data_model::schema>;

    explicit Administration ( const shared_schema_t& schema,
                              QWidget* parent = 0);

             ~Administration();

private:
    Ui::Administration *ui_;
    shared_schema_t schema_;
};

// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G
#endif // ADMINISTRATION_H
