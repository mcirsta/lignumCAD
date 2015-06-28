/****************************************************************************
** Form interface generated from reading ui file 'newpartdialog.ui'
**
** Created: Tue Oct 8 08:05:07 2002
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#ifndef NEWPARTDIALOG_H
#define NEWPARTDIALOG_H

#include <qvariant.h>
#include <qdialog.h>
class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class QButtonGroup;
class QFrame;
class QLabel;
class QPushButton;
class QRadioButton;
class lCConstraintChooser;
class lCDefaultLengthSpinBox;

class NewPartDialog : public QDialog
{ 
    Q_OBJECT

public:
    NewPartDialog( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~NewPartDialog();

    QLabel* TextLabel1;
    QButtonGroup* widthButtonGroup;
    QRadioButton* widthSpecifiedRadioButton;
    lCDefaultLengthSpinBox* widthLengthSpinBox;
    QFrame* Line1;
    QRadioButton* widthImportedRadioButton;
    lCConstraintChooser* widthConstraintChooser;
    QButtonGroup* heightButtonGroup;
    QRadioButton* heightSpecifiedRadioButton;
    lCDefaultLengthSpinBox* heightLengthSpinBox;
    QFrame* Line1_2;
    QRadioButton* heightImportedRadioButton;
    lCConstraintChooser* heightConstraintChooser;
    QButtonGroup* depthButtonGroup;
    QRadioButton* depthSpecifiedRadioButton;
    lCDefaultLengthSpinBox* depthLengthSpinBox;
    QFrame* Line1_3;
    QRadioButton* depthImportedRadioButton;
    lCConstraintChooser* depthConstraintChooser;
    QPushButton* buttonHelp;
    QPushButton* buttonOk;
    QPushButton* buttonCancel;


protected:
    QGridLayout* NewPartDialogLayout;
    QGridLayout* widthButtonGroupLayout;
    QGridLayout* heightButtonGroupLayout;
    QGridLayout* depthButtonGroupLayout;
    QHBoxLayout* Layout1;
};

#endif // NEWPARTDIALOG_H
