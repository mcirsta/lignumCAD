#ifndef CENTERLINEINFODIALOG_H
#define CENTERLINEINFODIALOG_H

#include <QDialog>

#include "ui_centerlineinfodialog.h"

class CenterlineInfoDialog : public QDialog, public Ui::CenterlineInfoDialog
{
  Q_OBJECT

public:
  explicit CenterlineInfoDialog( QWidget* parent = nullptr, const char* name = nullptr,
                                 bool modal = false,
                                 Qt::WindowFlags flags = Qt::WindowFlags() );
  ~CenterlineInfoDialog() override;

public slots:
  virtual void buttonHelp_clicked();
};

#endif // CENTERLINEINFODIALOG_H
