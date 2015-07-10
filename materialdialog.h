#ifndef MATERIALDIALOG
#define MATERIALDIALOG

#include "ui_materialdialog.h"

class MaterialDialog : public QDialog
{
    Q_OBJECT

public:
    MaterialDialog( QWidget* parent = 0 );
    Ui::MaterialDialog* getUi();

public slots:
    virtual void init();
    virtual void MaterialList_selectionChanged( ListViewItem * item );
    virtual void setMaterial(const Material *material );
    virtual void buttonHelp_clicked();

private:
    Ui::MaterialDialog ui;
};

#endif // MATERIALDIALOG

