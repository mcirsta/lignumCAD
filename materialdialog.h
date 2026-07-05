#ifndef MATERIALDIALOG_H
#define MATERIALDIALOG_H

#include <QDialog>

#include "ui_materialdialog.h"

class QListViewItem;
class Material;

class MaterialDialog : public QDialog, public Ui::MaterialDialog
{
  Q_OBJECT

public:
  explicit MaterialDialog( QWidget* parent = nullptr, const char* name = nullptr,
                           bool modal = false,
                           Qt::WindowFlags flags = Qt::WindowFlags() );
  ~MaterialDialog() override;

public slots:
  virtual void init();
  virtual void MaterialList_selectionChanged( QListViewItem* item );
  virtual void setMaterial( const Material* material );
  virtual void buttonHelp_clicked();
};

#endif // MATERIALDIALOG_H
