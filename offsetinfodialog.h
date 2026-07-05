#ifndef OFFSETINFODIALOG_H
#define OFFSETINFODIALOG_H

#include <QDialog>

#include "ui_offsetinfodialog.h"

class OffsetInfoDialog : public QDialog, public Ui::OffsetInfoDialog
{
  Q_OBJECT

public:
  explicit OffsetInfoDialog( QWidget* parent = nullptr, const char* name = nullptr,
                             bool modal = false,
                             Qt::WindowFlags flags = Qt::WindowFlags() );
  ~OffsetInfoDialog() override;

public slots:
  virtual void buttonHelp_clicked();
};

#endif // OFFSETINFODIALOG_H
