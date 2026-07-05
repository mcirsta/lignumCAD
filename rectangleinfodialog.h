#ifndef RECTANGLEINFODIALOG_H
#define RECTANGLEINFODIALOG_H

#include <QDialog>

#include "ui_rectangleinfodialog.h"

class RectangleInfoDialog : public QDialog, public Ui::RectangleInfoDialog
{
  Q_OBJECT

public:
  explicit RectangleInfoDialog( QWidget* parent = nullptr, const char* name = nullptr,
                                bool modal = false,
                                Qt::WindowFlags flags = Qt::WindowFlags() );
  ~RectangleInfoDialog() override;

public slots:
  virtual void init();
  virtual void rectangleStyleSlider_valueChanged( int style );
  virtual void buttonHelp_clicked();
};

#endif // RECTANGLEINFODIALOG_H
