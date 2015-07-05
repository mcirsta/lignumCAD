#ifndef ASSEMBLYCONFIGDIALOG
#define ASSEMBLYCONFIGDIALOG

#include "ui_assemblyconfigdialog.h"

class AssemblyConfigDialog : public QDialog
{
    Q_OBJECT

public:
    AssemblyConfigDialog( QWidget* parent = 0 );
    Ui::AssemblyConfigDialog* getUi ();


public slots:
    virtual void init();
    virtual void modelListView_selectionChanged( ListViewItem * item );
    virtual void buttonHelp_clicked();
};


#endif // ASSEMBLYCONFIGDIALOG

