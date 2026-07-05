#ifndef PARTINFODIALOG_H
#define PARTINFODIALOG_H

#include <QDialog>

#include "ui_partinfodialog.h"

class PartInfoDialog : public QDialog, public Ui::PartInfoDialog
{
  Q_OBJECT

public:
  explicit PartInfoDialog( QWidget* parent = nullptr, const char* name = nullptr,
                           bool modal = false,
                           Qt::WindowFlags flags = Qt::WindowFlags() );
  ~PartInfoDialog() override;

public slots:
  virtual void buttonHelp_clicked();
};

#endif // PARTINFODIALOG_H
