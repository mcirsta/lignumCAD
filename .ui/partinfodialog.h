/****************************************************************************
** Form interface generated from reading ui file 'partinfodialog.ui'
**
** Created: Wed Dec 18 08:52:14 2002
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#ifndef PARTINFODIALOG_H
#define PARTINFODIALOG_H

#include <qvariant.h>
#include <qdialog.h>
class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class QLabel;
class QLineEdit;
class QPushButton;

class PartInfoDialog : public QDialog
{ 
    Q_OBJECT

public:
    PartInfoDialog( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~PartInfoDialog();

    QPushButton* buttonHelp;
    QPushButton* buttonOk;
    QPushButton* buttonCancel;
    QLineEdit* nameEdit;
    QLabel* nameLabel;


public slots:
    virtual void buttonHelp_clicked();

protected:
    QGridLayout* PartInfoDialogLayout;
    QHBoxLayout* Layout1;
};

#endif // PARTINFODIALOG_H
