/****************************************************************************
** Form interface generated from reading ui file 'materialdialog.ui'
**
** Created: Thu Jan 2 13:42:16 2003
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#ifndef MATERIALDIALOG_H
#define MATERIALDIALOG_H

#include <qvariant.h>
#include <qdialog.h>
#include "material.h"
class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class QGroupBox;
class QLabel;
class QListView;
class QListViewItem;
class QPushButton;
class QTextEdit;

class MaterialDialog : public QDialog
{ 
    Q_OBJECT

public:
    MaterialDialog( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~MaterialDialog();

    QPushButton* buttonHelp;
    QGroupBox* detailsGroupBox;
    QTextEdit* MaterialText;
    QLabel* SolidColorLabel;
    QLabel* SolidColor;
    QLabel* FaceGrainLabel;
    QLabel* FaceGrainPixmap;
    QLabel* EndGrainLabel;
    QLabel* EndGrainPixmap;
    QLabel* EdgeGrainLabel;
    QLabel* EdgeGrainPixmap;
    QListView* MaterialList;
    QPushButton* buttonOk;
    QPushButton* buttonCancel;


public slots:
    virtual void init();
    virtual void MaterialList_selectionChanged( QListViewItem * item );
    virtual void setMaterial( const Material * material );
    virtual void buttonHelp_clicked();

protected:
    QGridLayout* MaterialDialogLayout;
    QHBoxLayout* detailsGroupBoxLayout;
    QVBoxLayout* Layout19;
    QGridLayout* Layout10;
    QGridLayout* Layout16;
    QGridLayout* Layout17;
    QGridLayout* Layout18;
};

#endif // MATERIALDIALOG_H
