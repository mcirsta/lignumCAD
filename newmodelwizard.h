#ifndef NEWMODELWIZARD
#define NEWMODELWIZARD

#include "ui_newmodelwizard.h"

class NewModelWizard : public QWizard
{
    Q_OBJECT

public:
    NewModelWizard(QWidget *parent = 0);
    Ui::NewModelWizard* getUi();

private:
    Ui::NewModelWizard ui;

public slots:
    virtual void init();
    virtual void NewModelWizard_helpClicked();
    void selectedPage( uint & type );
    void unsetInitialPages( void );

private slots:
    void modelNameEdit_textChanged( const QString & text );
    void initialPageSelected( void );
    void NewModelWizard_selected( const QString & );
};


#endif // NEWMODELWIZARD

