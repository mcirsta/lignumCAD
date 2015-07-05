#ifndef NEWPARTWIZARD
#define NEWPARTWIZARD

#include "ui_newpartwizard.h"
#include "lcdefaultlengthconstraint.h"
#include "part.h"
#include "partview.h"

#include "QWizard"

class NewPartWizard : public QWizard
{
    Q_OBJECT

public:
    NewPartWizard( QWidget* parent = 0);
    Ui::NewPartWizard* getUi();

public slots:
    virtual void init();
    virtual void partLibraryListView_currentChanged( ListViewItem * item );
    virtual void NewPartWizard_selected( const QString & );
    const QHash <int,lCDefaultLengthConstraint>& parameters( void );
    const PartMetadata * part( void );
    virtual void NewPartWizard_helpClicked();
    void setPartView( PartView * part_view );

private slots:
    void updateValidity( double );
    void validateName( void );
    QString trC( const QString & string );

private:
    Ui::NewPartWizard ui;
};


#endif // NEWPARTWIZARD

