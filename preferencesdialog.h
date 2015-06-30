#ifndef PREFERENCESDIALOG
#define PREFERENCESDIALOG

#include "ui_preferencesdialog.h"

class PreferencesDialog : public QDialog {
    Q_OBJECT

public:
    PreferencesDialog(QWidget *parent = 0);

private:
    Ui::PreferencesDialog ui;
};

#endif // PREFERENCESDIALOG

