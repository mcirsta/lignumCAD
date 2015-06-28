/****************************************************************************
** Form interface generated from reading ui file 'centerlineinfodialog.ui'
**
** Created: Wed Dec 18 08:52:13 2002
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#ifndef CENTERLINEINFODIALOG_H
#define CENTERLINEINFODIALOG_H

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

class CenterlineInfoDialog : public QDialog
{ 
    Q_OBJECT

public:
    CenterlineInfoDialog( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~CenterlineInfoDialog();

    QPushButton* buttonHelp;
    QPushButton* buttonOk;
    QPushButton* buttonCancel;
    QGroupBox* definitionGroupBox;
    QLabel* referenceLabel;
    QLineEdit* referenceLineEdit;
    QButtonGroup* offsetButtonGroup;
    QRadioButton* specifiedRadioButton;
    lCDefaultLengthSpinBox* offsetLengthSpinBox;
    QFrame* Line1;
    QRadioButton* importedRadioButton;
    lCConstraintChooser* constraintChooser;
    QLabel* centerlineNameLabel;
    QLineEdit* nameEdit;
    QButtonGroup* orientationButtonGroup;
    QRadioButton* horizontalButton;
    QRadioButton* verticalButton;


public slots:
    virtual void buttonHelp_clicked();

protected:
    QGridLayout* CenterlineInfoDialogLayout;
    QHBoxLayout* Layout1;
    QGridLayout* definitionGroupBoxLayout;
    QGridLayout* offsetButtonGroupLayout;
    QHBoxLayout* Layout15;
    QGridLayout* orientationButtonGroupLayout;
};

#endif // CENTERLINEINFODIALOG_H
