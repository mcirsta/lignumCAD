/****************************************************************************
** Form interface generated from reading ui file 'assemblyconstraintform.ui'
**
** Created: Wed Dec 18 08:52:14 2002
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#ifndef ASSEMBLYCONSTRAINTFORM_H
#define ASSEMBLYCONSTRAINTFORM_H

#include <qvariant.h>
#include <qwidget.h>
class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class QButtonGroup;
class QFrame;
class QLabel;
class QPushButton;
class QTextBrowser;

class AssemblyConstraintForm : public QWidget
{ 
    Q_OBJECT

public:
    AssemblyConstraintForm( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~AssemblyConstraintForm();

    QLabel* TextLabel1;
    QButtonGroup* constraintButtonGroup;
    QPushButton* matePushButton;
    QPushButton* alignPushButton;
    QPushButton* mateOffsetPushButton;
    QPushButton* alignOffsetPushButton;
    QFrame* Line1;
    QPushButton* helpPushButton;
    QFrame* Line2;
    QPushButton* cancelPushButton;
    QFrame* Line3;
    QLabel* constraintsTextLabel;
    QTextBrowser* constraintListBrowser;


public slots:
    virtual void helpPushButton_clicked();

protected:
    QVBoxLayout* AssemblyConstraintFormLayout;
    QVBoxLayout* constraintButtonGroupLayout;
};

#endif // ASSEMBLYCONSTRAINTFORM_H
