/****************************************************************************
** Form interface generated from reading ui file 'rectangleinfodialog.ui'
**
** Created: Wed Dec 18 08:52:13 2002
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#ifndef RECTANGLEINFODIALOG_H
#define RECTANGLEINFODIALOG_H

#include <qvariant.h>
#include <qdialog.h>
class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class QButtonGroup;
class QFrame;
class QGroupBox;
class QLabel;
class QLineEdit;
class QPushButton;
class QRadioButton;
class QSlider;
class lCColorChooser;
class lCConstraintChooser;
class lCDefaultComboBox;
class lCDefaultFileChooser;
class lCDefaultLengthSpinBox;
class lCDefaultSpinBox;

class RectangleInfoDialog : public QDialog
{ 
    Q_OBJECT

public:
    RectangleInfoDialog( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~RectangleInfoDialog();

    QLabel* nameLabel;
    QGroupBox* xOriginGroupBox;
    QLabel* xOriginReferenceLabel;
    QLineEdit* xOriginReferenceLineEdit;
    QButtonGroup* xOriginButtonGroup;
    QRadioButton* xOriginSpecifiedRadioButton;
    lCDefaultLengthSpinBox* xOriginOffsetLengthSpinBox;
    QFrame* Line1_3;
    QRadioButton* xOriginImportedRadioButton;
    lCConstraintChooser* xOriginConstraintChooser;
    QGroupBox* yOriginGroupBox;
    QLabel* yOriginReferenceLabel;
    QLineEdit* yOriginReferenceLineEdit;
    QButtonGroup* yOriginButtonGroup;
    QRadioButton* yOriginSpecifiedRadioButton;
    lCDefaultLengthSpinBox* yOriginOffsetLengthSpinBox;
    QFrame* Line1_3_2;
    QRadioButton* yOriginImportedRadioButton;
    lCConstraintChooser* yOriginConstraintChooser;
    QButtonGroup* heightButtonGroup;
    QRadioButton* heightSpecifiedRadioButton;
    lCDefaultLengthSpinBox* heightLengthSpinBox;
    QFrame* Line1_2;
    QRadioButton* heightImportedRadioButton;
    lCConstraintChooser* heightConstraintChooser;
    QLineEdit* nameEdit;
    QGroupBox* dimDisplaySideGroupBox;
    QRadioButton* belowButton;
    QRadioButton* rightButton;
    QRadioButton* leftButton;
    QRadioButton* aboveButton;
    QPushButton* buttonOk;
    QPushButton* buttonCancel;
    QButtonGroup* widthButtonGroup;
    QRadioButton* widthSpecifiedRadioButton;
    lCDefaultLengthSpinBox* widthLengthSpinBox;
    QFrame* Line1;
    QRadioButton* widthImportedRadioButton;
    lCConstraintChooser* widthConstraintChooser;
    QGroupBox* rectangleStyleGroupBox;
    QLabel* defaultStyleLabel;
    QLabel* wireframeStyleLabel;
    lCColorChooser* wireframeStyleColorChooser;
    lCDefaultComboBox* edgeStyleComboBox;
    QLabel* edgeStyleLabel;
    QSlider* rectangleStyleSlider;
    lCDefaultSpinBox* hiddenStyleSpinBox;
    QLabel* hiddenStyleLabel;
    QLabel* solidStyleLabel;
    lCColorChooser* solidStyleColorChooser;
    QLabel* textureStyleLabel;
    lCDefaultFileChooser* textureStyleFileChooser;
    QPushButton* buttonHelp;


public slots:
    virtual void init();
    virtual void rectangleStyleSlider_valueChanged( int style );
    virtual void buttonHelp_clicked();

protected:
    QGridLayout* RectangleInfoDialogLayout;
    QGridLayout* xOriginGroupBoxLayout;
    QGridLayout* xOriginButtonGroupLayout;
    QGridLayout* yOriginGroupBoxLayout;
    QGridLayout* yOriginButtonGroupLayout;
    QGridLayout* heightButtonGroupLayout;
    QGridLayout* dimDisplaySideGroupBoxLayout;
    QGridLayout* Layout2;
    QGridLayout* widthButtonGroupLayout;
    QGridLayout* rectangleStyleGroupBoxLayout;
};

#endif // RECTANGLEINFODIALOG_H
