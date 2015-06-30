#ifndef LIGNUMCADMAINWINDOW
#define LIGNUMCADMAINWINDOW
#include "ui_lignumcadmainwindow.h"
#include <QTableWidget>

class lignumCADMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    lignumCADMainWindow(QWidget *parent = 0);
    Ui::lignumCADMainWindow* getUi ();

private:
    Ui::lignumCADMainWindow ui;


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
    QTableWidget* modelHierarchyList();
    void showView();
    void setHomeDir( QDir & home_dir );

};



#endif // LIGNUMCADMAINWINDOW

