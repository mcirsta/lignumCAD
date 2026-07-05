#ifndef PREFERENCESDIALOG_H
#define PREFERENCESDIALOG_H

#include <QDialog>

#include "ratio.h"
#include "units.h"
#include "ui_preferencesdialog.h"

class QListWidgetItem;
class LengthUnit;
class OpenGLExample;

class PreferencesDialog : public QDialog, public Ui::PreferencesDialog
{
  Q_OBJECT

public:
  explicit PreferencesDialog( QWidget* parent = nullptr, const char* name = nullptr,
                              bool modal = false,
                              Qt::WindowFlags flags = Qt::WindowFlags() );
  ~PreferencesDialog() override;

public slots:
  void update();
  virtual void updateUnitsDisplays( LengthUnit* length_unit, UnitFormat format, int precision );
  virtual void buttonHelp_clicked();

private slots:
  virtual void init();
  void logoFileChooser_fileNameChanged( const QString& file_name );
  void unitsListBox_currentChanged( QListWidgetItem* item );
  void formatButtonGroup_clicked( int id );
  void precisionComboBox_valueChanged( int value );
  void updatePredefinedScheme( int id );
  void colorSchemeListBox_currentChanged( QListWidgetItem* );
  void geometryColorChooser_colorChanged( const QColor& color );
  void annotationColorChooser_colorChanged( const QColor& color );
  void gridColorChooser_colorChanged( const QColor& color );
  void constraintPrimaryColorChooser_colorChanged( const QColor& color );
  void constraintSecondaryColorChooser_colorChanged( const QColor& color );
  void backgroundSlider_valueChanged( int style );
  void solidBackgroundColorChooser_colorChanged( const QColor& color );
  void gradientBackgroundColorChooser_colorChanged( const QColor& color );
  void patternBackgroundFileChooser_fileNameChanged( const QString& file );
  void dimensionFontChooser_fontChanged( const QString& font );
  void arrowHeadLengthSpinBox_valueChanged( double length );
  void arrowHeadStyleComboBox_valueChanged( int style );
  void clearanceLengthSpinBox_valueChanged( double clearance );
  void lineThicknessLengthSpinBox_valueChanged( double );
  void extensionOffsetLengthSpinBox_valueChanged( double offset );
  void annotationFontChooser_fontChanged( const QString& font );
  void handleLengthSpinBox_valueChanged( double size );
  void arrowHeadWidthRatioSpinBox_valueChanged( const Ratio& ratio );

private:
  OpenGLExample* example_ = nullptr;
};

#endif // PREFERENCESDIALOG_H
