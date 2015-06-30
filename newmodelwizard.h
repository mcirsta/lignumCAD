#ifndef NEWMODELWIZARD
#define NEWMODELWIZARD

#include "ui_newmodelwizard.h"

class NewModelWizard : public QWizard
{
    Q_OBJECT

public:
    NewModelWizard(QWidget *parent = 0);

private:
    Ui::NewModelWizard ui;
};


#endif // NEWMODELWIZARD

