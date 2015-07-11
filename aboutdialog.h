#ifndef ABOUTDIALOG
#define ABOUTDIALOG

#include "ui_aboutdialog.h"

class aboutDialog : public QDialog
{
    Q_OBJECT

public:
    aboutDialog( QWidget* parent = 0 );

public slots:
    virtual void init();

private:
    Ui::aboutDialog ui;
};

#endif // ABOUTDIALOG

