#ifndef NEWMODELWIZARD_H
#define NEWMODELWIZARD_H

#include <QWizard>

#include <vector>

#include "ui_newmodelwizard.h"

class QButtonGroup;
class QRadioButton;
class QWizardPage;

class NewModelWizard : public QWizard, public Ui::NewModelWizard
{
  Q_OBJECT

public:
  explicit NewModelWizard( QWidget* parent = nullptr, const char* name = nullptr,
                           bool modal = false,
                           Qt::WindowFlags flags = Qt::WindowFlags() );
  ~NewModelWizard() override;

public slots:
  virtual void init();
  virtual void NewModelWizard_helpClicked();
  void selectedPage( uint& type );
  void unsetInitialPages();
  void showNewModelPage();

private slots:
  void modelNameEdit_textChanged( const QString& text );
  void initialPageSelected( int id );
  void NewModelWizard_currentIdChanged( int id );

private:
  void focusInitialPage();
  void focusModelPage();
  bool hasInitialPageSelection() const;
  int pageId( const QWizardPage* wizard_page ) const;

  QButtonGroup* initialPageButtonGroup = nullptr;
  std::vector<QRadioButton*> initialPageRadioButtons;
};

#endif // NEWMODELWIZARD_H
