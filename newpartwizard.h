#ifndef NEWPARTWIZARD_H
#define NEWPARTWIZARD_H

#include <QMap>
#include <QWizard>

#include <vector>

#include "part.h"
#include "ui_newpartwizard.h"

class lCDefaultLengthConstraint;
class QScrollArea;
class QTreeWidgetItem;
class QVBoxLayout;
class QWizardPage;
class QWidget;
class PartMetadata;
class PartView;

class NewPartWizard : public QWizard, public Ui::NewPartWizard
{
  Q_OBJECT

public:
  explicit NewPartWizard( QWidget* parent = nullptr, const char* name = nullptr,
                          bool modal = false,
                          Qt::WindowFlags flags = Qt::WindowFlags() );
  ~NewPartWizard() override;

public slots:
  virtual void init();
  virtual void partLibraryListView_currentChanged( QTreeWidgetItem* item );
  virtual void NewPartWizard_currentIdChanged( int );
  const PartParameterMap& parameters();
  const PartMetadata* part();
  virtual void NewPartWizard_helpClicked();
  void showInitialPartPage();
  void setPartView( PartView* part_view );

protected:
  bool validateCurrentPage() override;

private slots:
  void updateValidity( double );
  QString trC( const QString& string );

private:
  void focusInitialPartPage();
  void focusPartParametersPage();
  int pageId( const QWizardPage* page ) const;
  PartMetadata* selectedPart() const;

  QMap<QTreeWidgetItem*, PartMetadata*> parts_;
  QScrollArea* scroll_area_ = nullptr;
  std::vector<lCDefaultLengthConstraint*> labels_;
  QMap<QString, QTreeWidgetItem*> groups_;
  PartParameterMap parameter_labels_;
  QWidget* scroll_widget_ = nullptr;
  QVBoxLayout* scroll_layout_ = nullptr;
  PartView* part_view_ = nullptr;
};

#endif // NEWPARTWIZARD_H
