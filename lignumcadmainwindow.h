#ifndef LIGNUMCADMAINWINDOW_H
#define LIGNUMCADMAINWINDOW_H

#include <QDir>
#include <QMainWindow>

#include "ratio.h"
#include "ui_lignumcadmainwindow.h"

class QCloseEvent;
class QLabel;
class QListView;
class QDockWindow;
class DesignBookView;
class aboutDialog;

class lignumCADMainWindow : public QMainWindow, public Ui::lignumCADMainWindow
{
  Q_OBJECT

public:
  explicit lignumCADMainWindow( QWidget* parent = nullptr, const char* name = nullptr,
                                Qt::WindowFlags flags = Qt::WindowFlags() );
  ~lignumCADMainWindow() override;

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
  virtual void showView( const char* file_name );
  virtual void fileModelInfo();
  virtual void scaleChanged( const Ratio& scale );
  virtual void pageChanged( const QString& name );
  virtual void updateInformation( const QString& information );
  virtual QListView* modelHierarchyList();
  virtual void showView();
  void setHomeDir( QDir& home_dir );

protected:
  void closeEvent( QCloseEvent* ce ) override;

private:
  QDir home_dir_;
  QDockWindow* model_hierarchy_view_ = nullptr;
  QLabel* page_label_ = nullptr;
  QLabel* scale_label_ = nullptr;
  static aboutDialog* about_dialog_;
  DesignBookView* design_book_view_ = nullptr;
  QLabel* message_label_ = nullptr;
  QLabel* information_label_ = nullptr;
  QListView* model_hierarchy_list_ = nullptr;
};

#endif // LIGNUMCADMAINWINDOW_H
