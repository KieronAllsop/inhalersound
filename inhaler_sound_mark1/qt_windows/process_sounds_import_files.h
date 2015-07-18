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
//class QAbstractButton;


class ProcessSoundsImportFiles : public QWizardPage
{
    Q_OBJECT

public:

    QStringList getFileNames() {  return FileNames_; }
    void setFileNames( QStringList FileNames ) { FileNames_ = FileNames; }



    using shared_schema_t = std::shared_ptr<data_model::schema>;

                    ProcessSoundsImportFiles    (   const shared_schema_t& Schema,
                                                    QWidget* Parent=0   );

private slots:

    void            on_SelectFiles_clicked      ();
    void            on_Next_Button_Clicked      ();


private:
    // Data Variables
    shared_schema_t     Schema_;
    QStringList         FileNames_;

    // Owned Widgets
    QLabel*             SelectFiles_Label_;
    QLabel*             SelectionConfirmation_Label_;
    QPushButton*        SelectFiles_Button_;
    QPushButton*        ImportFiles_Button_;
    QTreeView*          AudioFiles_View_;
    QStandardItemModel* AudioFiles_;
    //QAbstractButton*    NextButton_;
};

// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G
#endif // QT_WINDOWS_PROCESS_SOUNDS_IMPORT_FILES_H_INCLUDED

