#ifndef PARAMETERINFODIALOG_H
#define PARAMETERINFODIALOG_H

#include <QDialog>

#include "ui_parameterinfodialog.h"

class ParameterInfoDialog : public QDialog, public Ui::ParameterInfoDialog
{
  Q_OBJECT

public:
  explicit ParameterInfoDialog( QWidget* parent = nullptr, const char* name = nullptr,
                                bool modal = false,
                                Qt::WindowFlags flags = Qt::WindowFlags() );
  ~ParameterInfoDialog() override;

public slots:
  virtual void buttonHelp_clicked();
};

#endif // PARAMETERINFODIALOG_H
