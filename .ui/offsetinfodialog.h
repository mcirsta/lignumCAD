/****************************************************************************
** Form interface generated from reading ui file 'offsetinfodialog.ui'
**
** Created: Wed Dec 18 08:52:14 2002
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#ifndef OFFSETINFODIALOG_H
#define OFFSETINFODIALOG_H

#include <qvariant.h>
#include <qdialog.h>
class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class QPushButton;
class lCDefaultLengthConstraint;

class OffsetInfoDialog : public QDialog
{ 
    Q_OBJECT

public:
    OffsetInfoDialog( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~OffsetInfoDialog();

    QPushButton* buttonHelp;
    QPushButton* buttonOk;
    QPushButton* buttonCancel;
    lCDefaultLengthConstraint* offsetLengthConstraint;


public slots:
    virtual void buttonHelp_clicked();

protected:
    QGridLayout* OffsetInfoDialogLayout;
    QHBoxLayout* Layout1;
};

#endif // OFFSETINFODIALOG_H
