#ifndef NEWPARTWIZARD_H
#define NEWPARTWIZARD_H

#include <QMap>
#include <QWizard>

#include <vector>

#include "part.h"
#include "ui_newpartwizard.h"

class lCDefaultLengthConstraint;
class QListViewItem;
class QScrollView;
class QVBox;
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
  virtual void partLibraryListView_currentChanged( QListViewItem* item );
  virtual void NewPartWizard_selected( const QString& );
  const PartParameterMap& parameters();
  const PartMetadata* part();
  virtual void NewPartWizard_helpClicked();
  void setPartView( PartView* part_view );

private slots:
  void updateValidity( double );
  void validateName();
  QString trC( const QString& string );

private:
  QMap<QListViewItem*, PartMetadata*> parts_;
  QScrollView* scroll_view_ = nullptr;
  std::vector<lCDefaultLengthConstraint*> labels_;
  QMap<QString, QListViewItem*> groups_;
  PartParameterMap parameter_labels_;
  QVBox* scroll_vbox_ = nullptr;
  PartView* part_view_ = nullptr;
};

#endif // NEWPARTWIZARD_H
