/****************************************************************************
** Form interface generated from reading ui file 'lignumcadmainwindow.ui'
**
** Created: Fri Jan 3 15:55:53 2003
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#ifndef LIGNUMCADMAINWINDOW_H
#define LIGNUMCADMAINWINDOW_H

#include <qvariant.h>
#include <qdir.h>
#include <qmainwindow.h>
#include "ratio.h"
class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class QAction;
class QActionGroup;
class QToolBar;
class QPopupMenu;
class QLabel;
class QListView;
class DesignBookView;
class aboutDialog;

class lignumCADMainWindow : public QMainWindow
{ 
    Q_OBJECT

public:
    lignumCADMainWindow( QWidget* parent = 0, const char* name = 0, WFlags fl = WType_TopLevel );
    ~lignumCADMainWindow();

    QLabel* mainPixmapLabel;
    QLabel* titleLabel;
    QMenuBar *menubar;
    QPopupMenu *fileMenu;
    QPopupMenu *editMenu;
    QPopupMenu *PopupMenu;
    QPopupMenu *insertMenu;
    QPopupMenu *toolMenu;
    QPopupMenu *helpMenu;
    QToolBar *toolBar;
    QToolBar *insertToolbar;
    QAction* fileNewAction;
    QAction* fileOpenAction;
    QAction* fileSaveAction;
    QAction* fileSaveAsAction;
    QAction* filePrintAction;
    QAction* fileExitAction;
    QAction* editUndoAction;
    QAction* editRedoAction;
    QAction* editCutAction;
    QAction* editCopyAction;
    QAction* editPasteAction;
    QAction* editFindAction;
    QAction* helpContentsAction;
    QAction* helpAboutAction;
    QAction* insertSketchAction;
    QAction* insertPartAction;
    QAction* insertAssemblyAction;
    QAction* insertDrawingAction;
    QAction* toolRectangleAction;
    QAction* toolDimensionAction;
    QAction* toolReferenceLineAction;
    QAction* toolCenterlineAction;
    QAction* deletePageAction;
    QAction* renamePageAction;
    QAction* toolStraightCutAction;
    QAction* toolHoleAction;
    QAction* toolFreeCutAction;
    QAction* toolAddModelAction;
    QAction* toolViewAction;
    QAction* exportPageAction;
    QAction* modelInfoAction;
    QAction* toolEdgeTreatmentAction;
    QAction* toolMaterialAction;
    QAction* toolJointAction;
    QAction* viewRestoreAction;
    QAction* editPreferencesAction;
    QAction* toolAlignmentAction;
    QAction* cancelReferenceLineAction;
    QAction* cancelRectangleAction;
    QAction* cancelCenterlineAction;
    QAction* cancelDimensionAction;
    QAction* cancelAlignmentAction;
    QAction* toolAnnotationAction;
    QAction* cancelAnnotationAction;
    QAction* toolConstraintDeleteAction;
    QAction* cancelConstraintDeleteAction;
    QAction* whatsThisAction;
    QAction* viewLeftAction;
    QAction* viewRightAction;
    QAction* viewFrontAction;
    QAction* viewBackAction;
    QAction* viewBottomAction;
    QAction* viewTopAction;
    QAction* toggleCSysAction;
    QAction* cancelAddModelAction;
    QAction* cancelAssemblyConstraintAction;
    QAction* toolDeleteModelAction;
    QAction* zoomInAction;
    QAction* zoomOutAction;


public slots:
    virtual void init();
    virtual void fileNew();
    virtual void fileOpen();
    virtual void fileSave();
    virtual void fileSaveAs();
    virtual void filePrint();
    virtual void fileExit();
    virtual void editUndo();
    virtual void editRedo();
    virtual void editCut();
    virtual void editCopy();
    virtual void editPaste();
    virtual void editFind();
    virtual void helpIndex();
    virtual void helpContents();
    virtual void helpAbout();
    virtual void fileExport();
    virtual void showView( const char * file_name );
    virtual void fileModelInfo();
    virtual void closeEvent( QCloseEvent * ce );
    virtual void scaleChanged( const Ratio & scale );
    virtual void pageChanged( const QString & name );
    virtual void updateInformation( const QString & information );
    virtual QListView * modelHierarchyList();
    virtual void showView();
    void setHomeDir( QDir & home_dir );

protected:
    QGridLayout* lignumCADMainWindowLayout;

    QDir home_dir_;
    QDockWindow* model_hierarchy_view_;
    QLabel* page_label_;
    QLabel* scale_label_;
    static aboutDialog* about_dialog_;
    DesignBookView* design_book_view_;
    QLabel* message_label_;
    QLabel* information_label_;
    QListView* model_hierarchy_list_;
};

#endif // LIGNUMCADMAINWINDOW_H
