#ifndef MODELINFODIALOG
#define MODELINFODIALOG

#include "ui_modelinfodialog.h"

class ModelInfoDialog : public QDialog {
    Q_OBJECT

public:
    ModelInfoDialog(QWidget *parent = 0);

private:
    Ui::ModelInfoDialog ui;
};

#endif // MODELINFODIALOG

