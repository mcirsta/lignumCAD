/****************************************************************************
** Form interface generated from reading ui file 'assemblyconfigdialog.ui'
**
** Created: Wed Dec 18 08:52:14 2002
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#ifndef ASSEMBLYCONFIGDIALOG_H
#define ASSEMBLYCONFIGDIALOG_H

#include <qvariant.h>
#include <qdialog.h>
class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class QLabel;
class QLineEdit;
class QListView;
class QListViewItem;
class QPushButton;

class AssemblyConfigDialog : public QDialog
{ 
    Q_OBJECT

public:
    AssemblyConfigDialog( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~AssemblyConfigDialog();

    QPushButton* buttonHelp;
    QPushButton* buttonOk;
    QPushButton* buttonCancel;
    QLabel* nameLabel;
    QLineEdit* nameEdit;
    QListView* modelListView;
    QLabel* TextLabel2;


public slots:
    virtual void init();
    virtual void modelListView_selectionChanged( QListViewItem * item );
    virtual void buttonHelp_clicked();

protected:
    QGridLayout* AssemblyConfigDialogLayout;
    QHBoxLayout* Layout1;
};

#endif // ASSEMBLYCONFIGDIALOG_H
