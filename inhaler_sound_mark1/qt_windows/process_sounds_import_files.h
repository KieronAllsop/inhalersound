// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G
#ifndef QT_WINDOWS_PROCESS_SOUNDS_IMPORT_FILES_H_INCLUDED
#define QT_WINDOWS_PROCESS_SOUNDS_IMPORT_FILES_H_INCLUDED
// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G

// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I

// Standard Library Includes
// none

// Qt Includes
#include <QWizardPage>
#include <QStringList>

// Custom Includes
#include "data_model/schema.hpp"

// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I

// Forward Declarations
class QLabel;
class QPushButton;
class QTreeView;
class QStandardItemModel;
class QComboBox;
//class QAbstractButton;

namespace inhaler
{
    class wave_importer;
}

class ProcessSoundsImportFiles : public QWizardPage
{
    Q_OBJECT

public:

    using shared_importer_t = std::shared_ptr<inhaler::wave_importer>;

public:

                    ProcessSoundsImportFiles    (  const shared_importer_t& Importer,
                                                   QWidget* Parent=0   );

    bool            isComplete                  () const;

private slots:

    void            on_SelectFiles_clicked      ();
    void            on_Confirm_Button_Clicked    ();
    void            on_Inhaler_selected         (QString);

private:
    // Data Variables
    shared_importer_t   Importer_;
    QStringList         FileNames_;
    bool                Confirmed_ = false;

    // Owned Widgets
    QLabel*             SelectFiles_Label_;
    QLabel*             SelectInhaler_Label_;
    QLabel*             ConfirmFiles_Label_;
    QPushButton*        SelectFiles_Button_;
    QPushButton*        ConfirmFiles_Button_;
    QTreeView*          AudioFiles_View_;
    QStandardItemModel* AudioFiles_;
    QComboBox*          SelectInhaler_;
    //QAbstractButton*    NextButton_;
};

// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G
#endif // QT_WINDOWS_PROCESS_SOUNDS_IMPORT_FILES_H_INCLUDED

