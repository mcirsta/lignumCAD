#ifndef ANNOTATIONINFODIALOG_H
#define ANNOTATIONINFODIALOG_H

#include <QDialog>

#include "ui_annotationinfodialog.h"

class AnnotationInfoDialog : public QDialog, public Ui::AnnotationInfoDialog
{
  Q_OBJECT

public:
  explicit AnnotationInfoDialog( QWidget* parent = nullptr, const char* name = nullptr,
                                 bool modal = false,
                                 Qt::WindowFlags flags = Qt::WindowFlags() );
  ~AnnotationInfoDialog() override;

public slots:
  virtual void init();
  virtual void boldButton_toggled( bool bold );
  virtual void italicButton_toggled( bool italic );
  virtual void underlineButton_toggled( bool underline );
  virtual void pointSizeSpinBox_valueChanged( int value );
  virtual void colorButton_clicked();
  virtual void annotationTextEdit_currentColorChanged( const QColor& color );
  virtual void annotationTextEdit_currentFontChanged( const QFont& font );
  virtual void sizesComboBox_activated( const QString& text );
  virtual void buttonHelp_clicked();
};

#endif // ANNOTATIONINFODIALOG_H
