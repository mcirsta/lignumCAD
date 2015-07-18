#ifndef NEWPARTWIZARD
#define NEWPARTWIZARD

#include "ui_newpartwizard.h"
#include "lcdefaultlengthconstraint.h"
#include "part.h"
#include "partview.h"

#include <QWizard>
#include <QScrollArea>

class NewPartWizard : public QWizard
{
    Q_OBJECT

public:
    NewPartWizard( QWidget* parent = 0);
    Ui::NewPartWizard* getUi();

public slots:
    virtual void init();
    virtual void partLibraryListView_currentChanged(QListWidgetItem *item );
    virtual void NewPartWizard_selected( const QString & );
    const QHash<QString,std::shared_ptr<lCDefaultLengthConstraint>>& parameters( void );
    const PartMetadata * part( void );
    virtual void NewPartWizard_helpClicked();
    void setPartView( PartView * part_view );

private slots:
    void updateValidity( double );
    void validateName( void );
    QString trC( const QString & string );

private:
    Ui::NewPartWizard ui;

protected:
    QScrollArea* scroll_view_;
    QMap<QListWidgetItem*,PartMetadata*> parts_;
    QList<std::shared_ptr<lCDefaultLengthConstraint>> labels_;
    QHash<QString, std::shared_ptr<ListViewItem>> groups_;
    QHash<QString,std::shared_ptr<lCDefaultLengthConstraint>> parameter_labels_;
    QWidget* scroll_vbox_;
    PartView* part_view_;
};


#endif // NEWPARTWIZARD

