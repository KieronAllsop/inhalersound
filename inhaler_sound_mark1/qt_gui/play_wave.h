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
// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I


class PlayWave : public QDialog
{
    Q_OBJECT

public:

    using shared_importer_t = std::shared_ptr<inhaler::wave_importer>;

    explicit    PlayWave  ( const shared_importer_t& Importer,
                                  QWidget* parent = 0 );

private:

    shared_importer_t   Importer_;
};

// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G
#endif // QT_GUI_PLAY_WAVE_H_INCLUDED
