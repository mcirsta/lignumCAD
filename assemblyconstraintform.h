#ifndef ASSEMBLYCONSTRAINTFORM_H
#define ASSEMBLYCONSTRAINTFORM_H

#include <QWidget>

#include "ui_assemblyconstraintform.h"

class AssemblyConstraintForm : public QWidget, public Ui::AssemblyConstraintForm
{
  Q_OBJECT

public:
  explicit AssemblyConstraintForm( QWidget* parent = nullptr, const char* name = nullptr,
                                   Qt::WindowFlags flags = Qt::WindowFlags() );
  ~AssemblyConstraintForm() override;

public slots:
  virtual void helpPushButton_clicked();
};

#endif // ASSEMBLYCONSTRAINTFORM_H
