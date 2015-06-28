/****************************************************************************
** Form interface generated from reading ui file 'parameterinfodialog.ui'
**
** Created: Wed Dec 18 08:52:14 2002
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#ifndef PARAMETERINFODIALOG_H
#define PARAMETERINFODIALOG_H

#include <qvariant.h>
#include <qdialog.h>
class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class QPushButton;
class lCDefaultLengthConstraint;

class ParameterInfoDialog : public QDialog
{ 
    Q_OBJECT

public:
    ParameterInfoDialog( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~ParameterInfoDialog();

    QPushButton* buttonHelp;
    QPushButton* buttonOk;
    QPushButton* buttonCancel;
    lCDefaultLengthConstraint* parameterLengthConstraint;


public slots:
    virtual void buttonHelp_clicked();

protected:
    QGridLayout* ParameterInfoDialogLayout;
    QHBoxLayout* Layout1;
};

#endif // PARAMETERINFODIALOG_H
