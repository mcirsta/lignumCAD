#ifndef LIGNUMCADMAINWINDOW
#define LIGNUMCADMAINWINDOW

#include "ui_lignumcadmainwindow.h"
#include <QTableWidget>
#include <QTreeWidget>
#include <QStandardItemModel>
#include <QStatusBar>
#include <QDockWidget>

#include "aboutdialog.h"
#include "designbookview.h"

class QListView;
class DesignBookView;

class lignumCADMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    lignumCADMainWindow(QWidget *parent = 0);
    Ui::lignumCADMainWindow* getUi ();

private:
    Ui::lignumCADMainWindow ui;

    QDir home_dir_;
    QDockWidget* model_hierarchy_view_;
    QLabel* page_label_;
    QLabel* scale_label_;
    static aboutDialog* about_dialog_;
    DesignBookView* design_book_view_;
    QLabel* message_label_;
    QLabel* information_label_;
    std::shared_ptr<QStandardItemModel> model_hierarchy_data;
    QTreeWidget* model_hierarchy_list_;

public slots:
    void init ();

    // The MainWindow delegates most of its actions to DesignBookView.

    void fileNew();
    void fileOpen();
    void fileSave();
    void fileSaveAs();
    void filePrint();
    void fileExit();
    void editUndo();
    void editRedo();
    void editCut();
    void editCopy();
    void editPaste();
    void editFind();
    void helpIndex();
    void helpContents();
    void helpAbout();
    void fileExport();
    void showView( const char * file_name );
    void fileModelInfo();
    void closeEvent( QCloseEvent * ce );
    void scaleChanged( const Ratio & scale );
    void pageChanged( const QString& name );
    void updateInformation( const QString& information );
    QStandardItem *modelHierarchyList();
    void showView();
    void setHomeDir( QDir & home_dir );

};



#endif // LIGNUMCADMAINWINDOW

