/****************************************************************************
** Form interface generated from reading ui file 'preferencesdialog.ui'
**
** Created: Wed Dec 18 08:52:13 2002
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#ifndef PREFERENCESDIALOG_H
#define PREFERENCESDIALOG_H

#include <qvariant.h>
#include <qdialog.h>
#include "openglexample.h"
class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class QButtonGroup;
class QFrame;
class QGroupBox;
class QLabel;
class QLineEdit;
class QListBox;
class QListBoxItem;
class QPushButton;
class QRadioButton;
class QSlider;
class QTabWidget;
class QWidget;
class lCColorChooser;
class lCDefaultComboBox;
class lCDefaultFileChooser;
class lCDefaultLengthSpinBox;
class lCDefaultRatioSpinBox;
class lCFontChooser;

class PreferencesDialog : public QDialog
{ 
    Q_OBJECT

public:
    PreferencesDialog( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~PreferencesDialog();

    QPushButton* buttonHelp;
    QPushButton* buttonOk;
    QPushButton* buttonCancel;
    QGroupBox* exampleBox;
    QFrame* exampleFrame;
    QTabWidget* preferencesTabWidget;
    QWidget* identificationPage;
    QLineEdit* businessLocationLineEdit;
    QLabel* businessLocationLabel;
    QLabel* businessNameLabel;
    QLineEdit* businessNameLineEdit;
    QGroupBox* GroupBox10;
    QFrame* logoFrame;
    QLabel* logoLabel;
    lCDefaultFileChooser* logoFileChooser;
    QWidget* unitsPage;
    QButtonGroup* formatButtonGroup;
    QRadioButton* fractionalRadioButton;
    QRadioButton* decimalRadioButton;
    QLabel* precisionLabel;
    lCDefaultComboBox* precisionComboBox;
    QGroupBox* UnitsBox;
    QListBox* unitsListBox;
    QWidget* colorSchemePage;
    QRadioButton* predefinedRadioButton;
    QListBox* colorSchemeListBox;
    QGroupBox* foregroundGroupBox;
    lCColorChooser* geometryColorChooser;
    QLabel* geometryColorLabel;
    QLabel* annotationColorLabel;
    lCColorChooser* annotationColorChooser;
    QLabel* gridColorLabel;
    lCColorChooser* gridColorChooser;
    QGroupBox* constraintColorGroupBox;
    lCColorChooser* constraintPrimaryColorChooser;
    QLabel* constrainSecondaryColorLabel;
    lCColorChooser* constraintSecondaryColorChooser;
    QLabel* constraintPrimaryColorLabel;
    QGroupBox* backgroundGroupBox;
    QSlider* backgroundSlider;
    lCColorChooser* solidBackgroundColorChooser;
    lCColorChooser* gradientBackgroundColorChooser;
    lCDefaultFileChooser* patternBackgroundFileChooser;
    QLabel* solidBackgroundLabel;
    QLabel* gradientBackgoundLabel;
    QLabel* patternBackgroundLabel;
    QFrame* Line5;
    QRadioButton* customRadioButton;
    QWidget* otherStylesPage;
    QGroupBox* dimensionGroupBox;
    QLabel* fontLabel;
    QLabel* arrowHeadLengthLabel;
    QLabel* arrowHeadWidthLabel;
    QLabel* arrowHeadStyleLabel;
    QLabel* clearanceLabel;
    QLabel* extensionOffsetLabel;
    lCDefaultComboBox* arrowHeadStyleComboBox;
    QLabel* lineThicknessLabel;
    lCFontChooser* dimensionFontChooser;
    lCDefaultLengthSpinBox* arrowHeadLengthSpinBox;
    lCDefaultRatioSpinBox* arrowHeadWidthRatioSpinBox;
    lCDefaultLengthSpinBox* clearanceLengthSpinBox;
    lCDefaultLengthSpinBox* lineThicknessLengthSpinBox;
    lCDefaultLengthSpinBox* extensionOffsetLengthSpinBox;
    QGroupBox* annotationGroupBox;
    lCFontChooser* annotationFontChooser;
    QLabel* annotationFontLabel;
    QGroupBox* handleGroupBox;
    QLabel* handleSizeLabel;
    lCDefaultLengthSpinBox* handleLengthSpinBox;


public slots:
    void update( void );
    virtual void updateUnitsDisplays( LengthUnit * length_unit, UnitFormat format, int precision );
    virtual void buttonHelp_clicked();

private slots:
    virtual void init();
    void logoFileChooser_fileNameChanged( const QString & file_name );
    void unitsListBox_currentChanged( QListBoxItem * item );
    void formatButtonGroup_clicked( int id );
    void precisionComboBox_valueChanged( int value );
    void updatePredefinedScheme( int id );
    void colorSchemeListBox_currentChanged( QListBoxItem * );
    void geometryColorChooser_colorChanged( const QColor & color );
    void annotationColorChooser_colorChanged( const QColor & color );
    void gridColorChooser_colorChanged( const QColor & color );
    void constraintPrimaryColorChooser_colorChanged( const QColor & color );
    void constraintSecondaryColorChooser_colorChanged( const QColor & color );
    void backgroundSlider_valueChanged( int style );
    void solidBackgroundColorChooser_colorChanged( const QColor & color );
    void gradientBackgroundColorChooser_colorChanged( const QColor & color );
    void patternBackgroundFileChooser_fileNameChanged( const QString & file );
    void dimensionFontChooser_fontChanged( const QString & font );
    void arrowHeadLengthSpinBox_valueChanged( double length );
    void arrowHeadStyleComboBox_valueChanged( int style );
    void clearanceLengthSpinBox_valueChanged( double clearance );
    void lineThicknessLengthSpinBox_valueChanged( double );
    void extensionOffsetLengthSpinBox_valueChanged( double offset );
    void annotationFontChooser_fontChanged( const QString & font );
    void handleLengthSpinBox_valueChanged( double size );
    void arrowHeadWidthRatioSpinBox_valueChanged( const Ratio & ratio );

protected:
    QGridLayout* PreferencesDialogLayout;
    QHBoxLayout* Layout1;
    QGridLayout* exampleBoxLayout;
    QGridLayout* identificationPageLayout;
    QGridLayout* GroupBox10Layout;
    QGridLayout* logoFrameLayout;
    QGridLayout* unitsPageLayout;
    QVBoxLayout* Layout8;
    QGridLayout* formatButtonGroupLayout;
    QHBoxLayout* Layout7;
    QGridLayout* UnitsBoxLayout;
    QGridLayout* colorSchemePageLayout;
    QVBoxLayout* Layout7_3;
    QHBoxLayout* Layout5;
    QGridLayout* foregroundGroupBoxLayout;
    QGridLayout* constraintColorGroupBoxLayout;
    QGridLayout* backgroundGroupBoxLayout;
    QGridLayout* otherStylesPageLayout;
    QGridLayout* dimensionGroupBoxLayout;
    QVBoxLayout* Layout9;
    QGridLayout* annotationGroupBoxLayout;
    QHBoxLayout* handleGroupBoxLayout;

    OpenGLExample* example_;
};

#endif // PREFERENCESDIALOG_H
