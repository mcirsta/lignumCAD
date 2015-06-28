/****************************************************************************
** Form interface generated from reading ui file 'dimensioninfodialog.ui'
**
** Created: Wed Dec 18 08:52:13 2002
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#ifndef DIMENSIONINFODIALOG_H
#define DIMENSIONINFODIALOG_H

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
class lCConstraintChooser;
class lCDefaultLengthSpinBox;

class DimensionInfoDialog : public QDialog
{ 
    Q_OBJECT

public:
    DimensionInfoDialog( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~DimensionInfoDialog();

    QLabel* nameLabel;
    QPushButton* buttonHelp;
    QPushButton* buttonOk;
    QPushButton* buttonCancel;
    QGroupBox* GroupBox1;
    QRadioButton* belowButton;
    QRadioButton* rightButton;
    QRadioButton* leftButton;
    QRadioButton* aboveButton;
    QButtonGroup* orientationGroup;
    QRadioButton* horizontalButton;
    QRadioButton* verticalButton;
    QRadioButton* otherButton;
    QGroupBox* referencesGroupBox;
    QLabel* toReferenceLabel;
    QLineEdit* toReferenceLineEdit;
    QLabel* fromReferenceLabel;
    QLineEdit* fromReferenceLineEdit;
    QButtonGroup* offsetButtonGroup;
    QRadioButton* specifiedRadioButton;
    lCDefaultLengthSpinBox* offsetLengthSpinBox;
    QFrame* Line1;
    QRadioButton* importedRadioButton;
    lCConstraintChooser* constraintChooser;
    QLineEdit* nameEdit;


public slots:
    virtual void buttonHelp_clicked();

protected:
    QGridLayout* DimensionInfoDialogLayout;
    QHBoxLayout* Layout1;
    QGridLayout* GroupBox1Layout;
    QGridLayout* Layout2;
    QGridLayout* orientationGroupLayout;
    QGridLayout* referencesGroupBoxLayout;
    QGridLayout* offsetButtonGroupLayout;
};

#endif // DIMENSIONINFODIALOG_H
