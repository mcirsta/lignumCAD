#ifndef PARTINFODIALOG
#define PARTINFODIALOG

#include "ui_partinfodialog.h"

class PartInfoDialog : public QDialog
{
    Q_OBJECT

public:
    PartInfoDialog( QWidget* parent = 0 );
    Ui::PartInfoDialog* getUi();


public slots:
    virtual void buttonHelp_clicked();

private:
    Ui::PartInfoDialog ui;
};


#endif // PARTINFODIALOG

