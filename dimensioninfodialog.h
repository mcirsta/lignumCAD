#ifndef DIMENSIONINFODIALOG_H
#define DIMENSIONINFODIALOG_H

#include <QDialog>

#include "ui_dimensioninfodialog.h"

class DimensionInfoDialog : public QDialog, public Ui::DimensionInfoDialog
{
  Q_OBJECT

public:
  explicit DimensionInfoDialog( QWidget* parent = nullptr, const char* name = nullptr,
                                bool modal = false,
                                Qt::WindowFlags flags = Qt::WindowFlags() );
  ~DimensionInfoDialog() override;

public slots:
  virtual void buttonHelp_clicked();
};

#endif // DIMENSIONINFODIALOG_H
