/****************************************************************************
** Form interface generated from reading ui file 'modelinfodialog.ui'
**
** Created: Wed Dec 18 08:52:13 2002
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#ifndef MODELINFODIALOG_H
#define MODELINFODIALOG_H

#include <qvariant.h>
#include <qdialog.h>
class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class QLabel;
class QLineEdit;
class QPushButton;
class QSpinBox;
class QTextEdit;
class lCFileChooser;

class ModelInfoDialog : public QDialog
{ 
    Q_OBJECT

public:
    ModelInfoDialog( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~ModelInfoDialog();

    QPushButton* buttonHelp;
    QPushButton* buttonOk;
    QPushButton* buttonCancel;
    QLineEdit* modelNameEdit;
    QLabel* versionLabel;
    QSpinBox* revisionSpinBox;
    QLabel* fileNameLabel;
    QSpinBox* versionSpinBox;
    QLabel* creationDateLabel;
    QLabel* modificationLabel;
    QLabel* modificationDateLabel;
    QLabel* modelNameLabel;
    QLabel* creationLabel;
    QTextEdit* descriptionEdit;
    QLabel* descriptionLabel;
    QLabel* revisionLabel;
    lCFileChooser* modelFileChooser;


public slots:
    virtual void buttonHelp_clicked();

private slots:
    void modelNameEdit_textChanged( const QString & text );

protected:
    QGridLayout* ModelInfoDialogLayout;
    QHBoxLayout* Layout1;
};

#endif // MODELINFODIALOG_H
