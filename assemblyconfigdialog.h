#ifndef ASSEMBLYCONFIGDIALOG_H
#define ASSEMBLYCONFIGDIALOG_H

#include <QDialog>

#include "ui_assemblyconfigdialog.h"

class QListViewItem;

class AssemblyConfigDialog : public QDialog, public Ui::AssemblyConfigDialog
{
  Q_OBJECT

public:
  explicit AssemblyConfigDialog( QWidget* parent = nullptr, const char* name = nullptr,
                                 bool modal = false,
                                 Qt::WindowFlags flags = Qt::WindowFlags() );
  ~AssemblyConfigDialog() override;

public slots:
  virtual void init();
  virtual void modelListView_selectionChanged( QListViewItem* item );
  virtual void buttonHelp_clicked();
};

#endif // ASSEMBLYCONFIGDIALOG_H
