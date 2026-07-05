#ifndef MODELINFODIALOG_H
#define MODELINFODIALOG_H

#include <QDialog>

#include "ui_modelinfodialog.h"

class ModelInfoDialog : public QDialog, public Ui::ModelInfoDialog
{
  Q_OBJECT

public:
  explicit ModelInfoDialog( QWidget* parent = nullptr, const char* name = nullptr,
                            bool modal = false,
                            Qt::WindowFlags flags = Qt::WindowFlags() );
  ~ModelInfoDialog() override;

public slots:
  virtual void buttonHelp_clicked();

private slots:
  void modelNameEdit_textChanged( const QString& text );
};

#endif // MODELINFODIALOG_H
