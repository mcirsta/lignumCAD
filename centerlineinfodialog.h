#ifndef CENTERLINEINFODIALOG
#define CENTERLINEINFODIALOG

#include "ui_centerlineinfodialog.h"

class CenterlineInfoDialog : public QDialog
{
    Q_OBJECT

public:
    CenterlineInfoDialog( QWidget* parent = 0);
    Ui::CenterlineInfoDialog* getUi();

public slots:
    virtual void buttonHelp_clicked();

private:
    Ui::CenterlineInfoDialog ui;
};

#endif // CENTERLINEINFODIALOG

