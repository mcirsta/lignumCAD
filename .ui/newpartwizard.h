/****************************************************************************
** Form interface generated from reading ui file 'newpartwizard.ui'
**
** Created: Wed Dec 18 08:52:13 2002
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#ifndef NEWPARTWIZARD_H
#define NEWPARTWIZARD_H

#include <qvariant.h>
#include <qdict.h>
#include <qscrollview.h>
#include <qvbox.h>
#include <qwizard.h>
class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class QGroupBox;
class QLabel;
class QLineEdit;
class QListView;
class QListViewItem;
class QWidget;
class PartView;
class lCDefaultLengthConstraint;
class PartMetadata;

class NewPartWizard : public QWizard
{ 
    Q_OBJECT

public:
    NewPartWizard( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~NewPartWizard();

    QWidget* initialPartPage;
    QLabel* PixmapLabel5;
    QLineEdit* nameEdit;
    QLabel* nameLabel;
    QGroupBox* partLibraryGroupBox;
    QListView* partLibraryListView;
    QWidget* partParametersPage;
    QLabel* PixmapLabel5_2;
    QGroupBox* partParameterFrame;


public slots:
    virtual void init();
    virtual void partLibraryListView_currentChanged( QListViewItem * item );
    virtual void NewPartWizard_selected( const QString & );
    const QDict<lCDefaultLengthConstraint>& parameters( void );
    const PartMetadata * part( void );
    virtual void NewPartWizard_helpClicked();
    void setPartView( PartView * part_view );

private slots:
    void updateValidity( double );
    void validateName( void );
    QString trC( const QString & string );

protected:
    QGridLayout* initialPartPageLayout;
    QVBoxLayout* Layout6;
    QGridLayout* partLibraryGroupBoxLayout;
    QGridLayout* partParametersPageLayout;
    QVBoxLayout* Layout6_2;
    QGridLayout* partParameterFrameLayout;

    QMap<QListViewItem*,PartMetadata*> parts_;
    QScrollView* scroll_view_;
    QPtrList<lCDefaultLengthConstraint> labels_;
    QDict<QListViewItem> groups_;
    QDict<lCDefaultLengthConstraint> parameter_labels_;
    QVBox* scroll_vbox_;
    PartView* part_view_;
};

#endif // NEWPARTWIZARD_H
