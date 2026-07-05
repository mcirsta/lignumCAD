#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include <QDialog>

#include "ui_aboutdialog.h"

class aboutDialog : public QDialog, public Ui::aboutDialog
{
  Q_OBJECT

public:
  explicit aboutDialog( QWidget* parent = nullptr, const char* name = nullptr,
                        bool modal = false,
                        Qt::WindowFlags flags = Qt::WindowFlags() );
  ~aboutDialog() override;

public slots:
  virtual void init();
};

#endif // ABOUTDIALOG_H
