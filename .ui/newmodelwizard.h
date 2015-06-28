/****************************************************************************
** Form interface generated from reading ui file 'newmodelwizard.ui'
**
** Created: Wed Dec 18 08:52:13 2002
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#ifndef NEWMODELWIZARD_H
#define NEWMODELWIZARD_H

#include <qvariant.h>
#include <qwizard.h>
class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class QGroupBox;
class QLabel;
class QLineEdit;
class QSpinBox;
class QTextEdit;
class QWidget;
class lCFileChooser;
class QRadioButton;
class QButtonGroup;

class NewModelWizard : public QWizard
{ 
    Q_OBJECT

public:
    NewModelWizard( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~NewModelWizard();

    QWidget* NewModelPage;
    QLineEdit* modelNameEdit;
    QLabel* fileNameLabel;
    QLabel* versionLabel;
    QSpinBox* versionSpinBox;
    QLabel* modificationLabel;
    QLabel* descriptionLabel;
    QTextEdit* descriptionEdit;
    QLabel* revisionLabel;
    lCFileChooser* modelFileChooser;
    QSpinBox* revisionSpinBox;
    QLabel* creationLabel;
    QLabel* newWizardPixmap;
    QLabel* modelNameLabel;
    QLabel* creationDateLabel;
    QLabel* modificationDateLabel;
    QWidget* InitialPagePage;
    QLabel* newWizardPixmap_2;
    QGroupBox* initialPageGroupBox;


public slots:
    virtual void init();
    virtual void NewModelWizard_helpClicked();
    void selectedPage( uint & type );
    void unsetInitialPages( void );

private slots:
    void modelNameEdit_textChanged( const QString & text );
    void initialPageSelected( void );
    void NewModelWizard_selected( const QString & );

protected:
    QGridLayout* NewModelPageLayout;
    QVBoxLayout* Layout4;
    QGridLayout* InitialPagePageLayout;
    QVBoxLayout* initialPageGroupBoxLayout;

    QButtonGroup* initialPageButtonGroup;
    QPtrList<QRadioButton> initialPageRadioButtons;
};

#endif // NEWMODELWIZARD_H
