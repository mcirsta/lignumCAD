#ifndef ASSEMBLYADDDIALOG
#define ASSEMBLYADDDIALOG

#include "ui_assemblyadddialog.h"

class AssemblyAddDialog : public QDialog
{
    Q_OBJECT

public:
    AssemblyAddDialog( QWidget* parent = 0 );
    Ui::AssemblyAddDialog* getUi();

public slots:
    virtual void modelListView_selectionChanged( ListViewItem * item );
    virtual void buttonHelp_clicked();

private:
    Ui::AssemblyAddDialog ui;
};

#endif // ASSEMBLYADDDIALOG

