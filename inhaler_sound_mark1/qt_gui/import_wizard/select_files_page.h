// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G
#ifndef QT_GUI_IMPORT_WIZARD_SELECT_FILES_PAGE_H_INCLUDED
#define QT_GUI_IMPORT_WIZARD_SELECT_FILES_PAGE_H_INCLUDED
// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G

// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I

// Standard Library Includes
#include <memory>

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

namespace inhaler
{
    class wave_importer;
}

// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
namespace qt_gui {
namespace import_wizard {
// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n

class select_files_page : public QWizardPage
{
    Q_OBJECT

public:

    using shared_importer_t = std::shared_ptr<inhaler::wave_importer>;

public:
                    select_files_page           (   const shared_importer_t& Importer,
                                                    QWidget* Parent=0   );

private slots:

    void            on_select_files_clicked      ();
    void            on_confirm_button_clicked    ();
    void            on_inhaler_selected        	 (QString);

private:
    // Data Variables
    shared_importer_t   Importer_;
    QStringList         FileNames_;
    bool                Confirmed_ = false;
    bool            	isComplete                  () const;

    // Owned Widgets
    QLabel*             SelectFiles_Label_;
    QLabel*             SelectInhaler_Label_;
    QLabel*             ConfirmFiles_Label_;
    QPushButton*        SelectFiles_Button_;
    QPushButton*        ConfirmFiles_Button_;
    QTreeView*          AudioFiles_View_;
    QStandardItemModel* AudioFiles_;
    QComboBox*          SelectInhaler_;
};

// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
} // end import_wizard
} // end qt_gui
// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n

// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G
#endif // QT_GUI_IMPORT_WIZARD_SELECT_FILES_PAGE_H_INCLUDED

