#ifndef DIMENSIONINFODIALOG
#define DIMENSIONINFODIALOG

#include "ui_dimensioninfodialog.h"

class DimensionInfoDialog : public QDialog
{
    Q_OBJECT

public:
    DimensionInfoDialog( QWidget* parent = 0 );
    Ui::DimensionInfoDialog* getUi();


public slots:
    virtual void buttonHelp_clicked();

private:
    Ui::DimensionInfoDialog ui;
};

#endif // DIMENSIONINFODIALOG

