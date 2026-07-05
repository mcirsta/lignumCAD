#ifndef ASSEMBLYADDDIALOG_H
#define ASSEMBLYADDDIALOG_H

#include <QDialog>

#include "ui_assemblyadddialog.h"

class QTreeWidgetItem;

class AssemblyAddDialog : public QDialog, public Ui::AssemblyAddDialog
{
  Q_OBJECT

public:
  explicit AssemblyAddDialog( QWidget* parent = nullptr, const char* name = nullptr,
                              bool modal = false,
                              Qt::WindowFlags flags = Qt::WindowFlags() );
  ~AssemblyAddDialog() override;

public slots:
  virtual void modelListView_selectionChanged( QTreeWidgetItem* item );
  virtual void buttonHelp_clicked();
};

#endif // ASSEMBLYADDDIALOG_H
