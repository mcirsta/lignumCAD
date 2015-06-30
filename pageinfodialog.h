#ifndef PAGEINFODIALOG
#define PAGEINFODIALOG

#include "ui_pageinfodialog.h"

class PageInfoDialog : public QDialog {
    Q_OBJECT

public:
    PageInfoDialog(QWidget *parent = 0);

private:
    Ui::PageInfoDialog ui;
};


#endif // PAGEINFODIALOG

