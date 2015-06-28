/****************************************************************************
** Form interface generated from reading ui file 'aboutdialog.ui'
**
** Created: Wed Dec 18 08:52:13 2002
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include <qvariant.h>
#include <qdialog.h>
class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class QFrame;
class QLabel;
class QPushButton;

class aboutDialog : public QDialog
{ 
    Q_OBJECT

public:
    aboutDialog( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~aboutDialog();

    QLabel* PixmapLabel1;
    QLabel* programVersionLabel;
    QLabel* copyrightLabel;
    QLabel* contactLabel;
    QLabel* GPLLabel;
    QFrame* Line1;
    QLabel* freeTypeVersionLabel;
    QLabel* openCASCADELabel;
    QLabel* acknowledgementsLabel;
    QLabel* openGLGLUVersionLabel;
    QLabel* qtLabel;
    QLabel* openCASCADEVersionLabel;
    QLabel* freeTypeLabel;
    QLabel* openGLGLULabel;
    QLabel* qtVersionLabel;
    QFrame* Line1_2;
    QLabel* openGLLabel;
    QLabel* openGLVersionLabel;
    QPushButton* buttonOk;


public slots:
    virtual void init();

protected:
    QVBoxLayout* aboutDialogLayout;
    QHBoxLayout* Layout3;
    QGridLayout* Layout5;
    QHBoxLayout* Layout6;
    QHBoxLayout* Layout2;
};

#endif // ABOUTDIALOG_H
