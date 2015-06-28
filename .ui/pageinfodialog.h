/****************************************************************************
** Form interface generated from reading ui file 'pageinfodialog.ui'
**
** Created: Wed Dec 18 08:52:13 2002
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#ifndef PAGEINFODIALOG_H
#define PAGEINFODIALOG_H

#include <qvariant.h>
#include <qdialog.h>
class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class QLabel;
class QLineEdit;
class QPushButton;

class PageInfoDialog : public QDialog
{ 
    Q_OBJECT

public:
    PageInfoDialog( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~PageInfoDialog();

    QPushButton* buttonHelp;
    QPushButton* buttonOk;
    QPushButton* buttonCancel;
    QLabel* nameLabel;
    QLineEdit* nameEdit;


public slots:
    virtual void buttonHelp_clicked();

protected:
    QGridLayout* PageInfoDialogLayout;
    QHBoxLayout* Layout1;
};

#endif // PAGEINFODIALOG_H
