#ifndef RECTANGLEINFODIALOG
#define RECTANGLEINFODIALOG

#include "lcdefaultcombobox.h"

#include "ui_rectangleinfodialog.h"

class RectangleInfoDialog : public QDialog
{
    Q_OBJECT

public:
    RectangleInfoDialog( QWidget* parent = 0 );
    Ui::RectangleInfoDialog* getUi();

public slots:
    virtual void init();
    virtual void rectangleStyleSlider_valueChanged( int style );
    virtual void buttonHelp_clicked();

private:
    Ui::RectangleInfoDialog ui;
};


#endif // RECTANGLEINFODIALOG

