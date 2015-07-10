#ifndef PARAMETERINFODIALOG
#define PARAMETERINFODIALOG

#include "ui_parameterinfodialog.h"

class ParameterInfoDialog : public QDialog
{
    Q_OBJECT

public:
    ParameterInfoDialog( QWidget* parent = 0 );
    Ui::ParameterInfoDialog* getUi();


public slots:
    virtual void buttonHelp_clicked();

private:
    Ui::ParameterInfoDialog ui;
};

#endif // PARAMETERINFODIALOG

