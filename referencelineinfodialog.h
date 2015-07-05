#ifndef REFERENCELINEINFODIALOG
#define REFERENCELINEINFODIALOG

#include "ui_referencelineinfodialog.h"

class ReferenceLineInfoDialog : public QDialog
{
    Q_OBJECT

public:
    ReferenceLineInfoDialog( QWidget* parent = 0 );
    Ui::ReferenceLineInfoDialog* getUi();


public slots:
    virtual void buttonHelp_clicked();

private:
    Ui::ReferenceLineInfoDialog ui;
};


#endif // REFERENCELINEINFODIALOG

