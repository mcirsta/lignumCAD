/****************************************************************************
** Form interface generated from reading ui file 'referencelineinfodialog.ui'
**
** Created: Wed Dec 18 08:52:13 2002
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#ifndef REFERENCELINEINFODIALOG_H
#define REFERENCELINEINFODIALOG_H

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

class ReferenceLineInfoDialog : public QDialog
{ 
    Q_OBJECT

public:
    ReferenceLineInfoDialog( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~ReferenceLineInfoDialog();

    QLabel* referenceLineNameLabel;
    QLineEdit* nameEdit;
    QGroupBox* definitionGroupBox;
    QLabel* referenceLabel;
    QLineEdit* referenceLineEdit;
    QButtonGroup* offsetButtonGroup;
    QRadioButton* specifiedRadioButton;
    lCDefaultLengthSpinBox* offsetLengthSpinBox;
    QFrame* Line1;
    QRadioButton* importedRadioButton;
    lCConstraintChooser* constraintChooser;
    QButtonGroup* orientationButtonGroup;
    QRadioButton* horizontalButton;
    QRadioButton* verticalButton;
    QPushButton* buttonHelp;
    QPushButton* buttonOk;
    QPushButton* buttonCancel;


public slots:
    virtual void buttonHelp_clicked();

protected:
    QGridLayout* ReferenceLineInfoDialogLayout;
    QHBoxLayout* Layout15;
    QGridLayout* definitionGroupBoxLayout;
    QGridLayout* offsetButtonGroupLayout;
    QGridLayout* orientationButtonGroupLayout;
    QHBoxLayout* Layout1;
};

#endif // REFERENCELINEINFODIALOG_H
