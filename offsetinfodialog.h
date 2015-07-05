#ifndef OFFSETINFODIALOG
#define OFFSETINFODIALOG

#include "lcdefaultlengthconstraint.h"
#include "ui_offsetinfodialog.h"

class OffsetInfoDialog : public QDialog
{
    Q_OBJECT

public:
    OffsetInfoDialog( QWidget* parent = 0 );
    Ui::OffsetInfoDialog* getUi();

public slots:
    virtual void buttonHelp_clicked();
};


#endif // OFFSETINFODIALOG

