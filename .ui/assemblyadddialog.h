/****************************************************************************
** Form interface generated from reading ui file 'assemblyadddialog.ui'
**
** Created: Wed Dec 18 08:52:14 2002
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#ifndef ASSEMBLYADDDIALOG_H
#define ASSEMBLYADDDIALOG_H

#include <qvariant.h>
#include <qdialog.h>
class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class QListView;
class QListViewItem;
class QPushButton;

class AssemblyAddDialog : public QDialog
{ 
    Q_OBJECT

public:
    AssemblyAddDialog( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~AssemblyAddDialog();

    QPushButton* buttonHelp;
    QPushButton* buttonOk;
    QPushButton* buttonCancel;
    QListView* modelListView;


public slots:
    virtual void modelListView_selectionChanged( QListViewItem * item );
    virtual void buttonHelp_clicked();

protected:
    QGridLayout* AssemblyAddDialogLayout;
    QHBoxLayout* Layout1;
};

#endif // ASSEMBLYADDDIALOG_H
