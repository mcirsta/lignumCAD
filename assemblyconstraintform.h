#ifndef ASSEMBLYCONSTRAINTFORM
#define ASSEMBLYCONSTRAINTFORM

#include "ui_assemblyconstraintform.h"

class AssemblyConstraintForm : public QWidget
{
    Q_OBJECT

public:
    AssemblyConstraintForm( QWidget* parent = 0 );
    Ui::AssemblyConstraintForm* getUi();

public slots:
    virtual void helpPushButton_clicked();

private:
    Ui::AssemblyConstraintForm ui;
};

#endif // ASSEMBLYCONSTRAINTFORM

