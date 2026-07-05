#ifndef PAGEINFODIALOG_H
#define PAGEINFODIALOG_H

#include <QDialog>

#include "ui_pageinfodialog.h"

class PageInfoDialog : public QDialog, public Ui::PageInfoDialog
{
  Q_OBJECT

public:
  explicit PageInfoDialog( QWidget* parent = nullptr, const char* name = nullptr,
                           bool modal = false,
                           Qt::WindowFlags flags = Qt::WindowFlags() );
  ~PageInfoDialog() override;

public slots:
  virtual void buttonHelp_clicked();
};

#endif // PAGEINFODIALOG_H
