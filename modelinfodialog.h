#ifndef MODELINFODIALOG
#define MODELINFODIALOG

#include "ui_modelinfodialog.h"

class ModelInfoDialog : public QDialog {
    Q_OBJECT

public:
    ModelInfoDialog(QWidget *parent = 0);
     Ui::ModelInfoDialog* getUi();

private:
    Ui::ModelInfoDialog ui;

public slots:
    virtual void buttonHelp_clicked();

private slots:
    void modelNameEdit_textChanged( const QString & text );
};

#endif // MODELINFODIALOG

