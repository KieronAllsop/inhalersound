// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G
#ifndef QT_GUI_PLAY_WAVE_H_INCLUDED
#define QT_GUI_PLAY_WAVE_H_INCLUDED
// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G

// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I
// Standard Library Includes
// none

// Qt Includes
#include <QDialog>

// Custom Includes
#include "inhaler/wave_importer.hpp"
#include "inhaler/data_retriever.hpp"
#include "inhaler/server.hpp"
// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I

// Forward Declarations
class QLabel;
class QPushButton;


// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
namespace qt_gui {
// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n


class play_wave : public QDialog
{
    Q_OBJECT

public:

    using shared_schema_t = inhaler::server::shared_schema_t;
    using shared_importer_t = std::shared_ptr<inhaler::wave_importer>;
    using shared_data_retriever_t = std::shared_ptr<inhaler::data_retriever>;

    explicit    play_wave ( const shared_schema_t& Schema,
                            const shared_importer_t& Importer,
                            const shared_data_retriever_t& DataRetriever,
                            QWidget* parent = 0 );

private slots:

    void                   on_get_data_clicked();

private:

    shared_schema_t         Schema_;
    shared_importer_t       Importer_;
    shared_data_retriever_t DataRetriever_;

    QLabel*                 PageTitle_;
    QPushButton*            GetData_;



};

// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
} // end qt_gui
// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n


// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G
#endif // QT_GUI_PLAY_WAVE_H_INCLUDED
