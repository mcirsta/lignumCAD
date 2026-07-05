#ifndef REFERENCELINEINFODIALOG_H
#define REFERENCELINEINFODIALOG_H

#include <QDialog>

#include "ui_referencelineinfodialog.h"

class ReferenceLineInfoDialog : public QDialog, public Ui::ReferenceLineInfoDialog
{
  Q_OBJECT

public:
  explicit ReferenceLineInfoDialog( QWidget* parent = nullptr, const char* name = nullptr,
                                    bool modal = false,
                                    Qt::WindowFlags flags = Qt::WindowFlags() );
  ~ReferenceLineInfoDialog() override;

public slots:
  virtual void buttonHelp_clicked();
};

#endif // REFERENCELINEINFODIALOG_H
