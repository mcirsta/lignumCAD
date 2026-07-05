#include "aboutdialog.h"
#include "annotationinfodialog.h"
#include "assemblyadddialog.h"
#include "assemblyconfigdialog.h"
#include "assemblyconstraintform.h"
#include "centerlineinfodialog.h"
#include "dimensioninfodialog.h"
#include "lignumcadmainwindow.h"
#include "materialdialog.h"
#include "modelinfodialog.h"
#include "newmodelwizard.h"
#include "newpartwizard.h"
#include "offsetinfodialog.h"
#include "pageinfodialog.h"
#include "parameterinfodialog.h"
#include "partinfodialog.h"
#include "preferencesdialog.h"
#include "rectangleinfodialog.h"
#include "referencelineinfodialog.h"

#include "configuration.h"
#include "constants.h"
#include "designbookview.h"
#include "material.h"
#include "openglexample.h"
#include "pagefactory.h"
#include "partview.h"
#include "systemdependencies.h"
#include "units.h"
#include "usersettings.h"

#include <QString>

namespace {
  void applyObjectName( QObject* object, const char* name )
  {
    if ( name != nullptr )
      object->setObjectName( QString::fromLatin1( name ) );
  }
}

aboutDialog::aboutDialog( QWidget* parent, const char* name, bool modal,
                          Qt::WindowFlags flags )
  : QDialog( parent, flags )
{
  applyObjectName( this, name );
  setModal( modal );
  setupUi( this );
  init();
}

aboutDialog::~aboutDialog() = default;

AnnotationInfoDialog::AnnotationInfoDialog( QWidget* parent, const char* name,
                                            bool modal, Qt::WindowFlags flags )
  : QDialog( parent, flags )
{
  applyObjectName( this, name );
  setModal( modal );
  setupUi( this );
  init();
}

AnnotationInfoDialog::~AnnotationInfoDialog() = default;

AssemblyAddDialog::AssemblyAddDialog( QWidget* parent, const char* name,
                                      bool modal, Qt::WindowFlags flags )
  : QDialog( parent, flags )
{
  applyObjectName( this, name );
  setModal( modal );
  setupUi( this );
}

AssemblyAddDialog::~AssemblyAddDialog() = default;

AssemblyConfigDialog::AssemblyConfigDialog( QWidget* parent, const char* name,
                                            bool modal, Qt::WindowFlags flags )
  : QDialog( parent, flags )
{
  applyObjectName( this, name );
  setModal( modal );
  setupUi( this );
  init();
}

AssemblyConfigDialog::~AssemblyConfigDialog() = default;

AssemblyConstraintForm::AssemblyConstraintForm( QWidget* parent, const char* name,
                                                Qt::WindowFlags flags )
  : QWidget( parent, flags )
{
  applyObjectName( this, name );
  setupUi( this );
}

AssemblyConstraintForm::~AssemblyConstraintForm() = default;

CenterlineInfoDialog::CenterlineInfoDialog( QWidget* parent, const char* name,
                                            bool modal, Qt::WindowFlags flags )
  : QDialog( parent, flags )
{
  applyObjectName( this, name );
  setModal( modal );
  setupUi( this );
}

CenterlineInfoDialog::~CenterlineInfoDialog() = default;

DimensionInfoDialog::DimensionInfoDialog( QWidget* parent, const char* name,
                                          bool modal, Qt::WindowFlags flags )
  : QDialog( parent, flags )
{
  applyObjectName( this, name );
  setModal( modal );
  setupUi( this );
}

DimensionInfoDialog::~DimensionInfoDialog() = default;

lignumCADMainWindow::lignumCADMainWindow( QWidget* parent, const char* name,
                                          Qt::WindowFlags flags )
  : QMainWindow( parent, flags )
{
  applyObjectName( this, name );
  setupUi( this );
  init();
}

lignumCADMainWindow::~lignumCADMainWindow() = default;

MaterialDialog::MaterialDialog( QWidget* parent, const char* name,
                                bool modal, Qt::WindowFlags flags )
  : QDialog( parent, flags )
{
  applyObjectName( this, name );
  setModal( modal );
  setupUi( this );
  init();
}

MaterialDialog::~MaterialDialog() = default;

ModelInfoDialog::ModelInfoDialog( QWidget* parent, const char* name,
                                  bool modal, Qt::WindowFlags flags )
  : QDialog( parent, flags )
{
  applyObjectName( this, name );
  setModal( modal );
  setupUi( this );
}

ModelInfoDialog::~ModelInfoDialog() = default;

NewModelWizard::NewModelWizard( QWidget* parent, const char* name,
                                bool modal, Qt::WindowFlags flags )
  : QWizard( parent, flags )
{
  applyObjectName( this, name );
  setModal( modal );
  setupUi( this );
  init();
}

NewModelWizard::~NewModelWizard() = default;

NewPartWizard::NewPartWizard( QWidget* parent, const char* name,
                              bool modal, Qt::WindowFlags flags )
  : QWizard( parent, flags )
{
  applyObjectName( this, name );
  setModal( modal );
  setupUi( this );
  init();
}

NewPartWizard::~NewPartWizard() = default;

OffsetInfoDialog::OffsetInfoDialog( QWidget* parent, const char* name,
                                    bool modal, Qt::WindowFlags flags )
  : QDialog( parent, flags )
{
  applyObjectName( this, name );
  setModal( modal );
  setupUi( this );
}

OffsetInfoDialog::~OffsetInfoDialog() = default;

PageInfoDialog::PageInfoDialog( QWidget* parent, const char* name,
                                bool modal, Qt::WindowFlags flags )
  : QDialog( parent, flags )
{
  applyObjectName( this, name );
  setModal( modal );
  setupUi( this );
}

PageInfoDialog::~PageInfoDialog() = default;

ParameterInfoDialog::ParameterInfoDialog( QWidget* parent, const char* name,
                                          bool modal, Qt::WindowFlags flags )
  : QDialog( parent, flags )
{
  applyObjectName( this, name );
  setModal( modal );
  setupUi( this );
}

ParameterInfoDialog::~ParameterInfoDialog() = default;

PartInfoDialog::PartInfoDialog( QWidget* parent, const char* name,
                                bool modal, Qt::WindowFlags flags )
  : QDialog( parent, flags )
{
  applyObjectName( this, name );
  setModal( modal );
  setupUi( this );
}

PartInfoDialog::~PartInfoDialog() = default;

PreferencesDialog::PreferencesDialog( QWidget* parent, const char* name,
                                      bool modal, Qt::WindowFlags flags )
  : QDialog( parent, flags )
{
  applyObjectName( this, name );
  setModal( modal );
  setupUi( this );
  init();
}

PreferencesDialog::~PreferencesDialog() = default;

RectangleInfoDialog::RectangleInfoDialog( QWidget* parent, const char* name,
                                          bool modal, Qt::WindowFlags flags )
  : QDialog( parent, flags )
{
  applyObjectName( this, name );
  setModal( modal );
  setupUi( this );
  init();
}

RectangleInfoDialog::~RectangleInfoDialog() = default;

ReferenceLineInfoDialog::ReferenceLineInfoDialog( QWidget* parent, const char* name,
                                                  bool modal, Qt::WindowFlags flags )
  : QDialog( parent, flags )
{
  applyObjectName( this, name );
  setModal( modal );
  setupUi( this );
}

ReferenceLineInfoDialog::~ReferenceLineInfoDialog() = default;

#include "aboutdialog.ui.h"
#include "annotationinfodialog.ui.h"
#include "assemblyadddialog.ui.h"
#include "assemblyconfigdialog.ui.h"
#include "assemblyconstraintform.ui.h"
#include "centerlineinfodialog.ui.h"
#include "dimensioninfodialog.ui.h"
#include "lignumcadmainwindow.ui.h"
#include "materialdialog.ui.h"
#include "modelinfodialog.ui.h"
#include "newmodelwizard.ui.h"
#include "newpartwizard.ui.h"
#include "offsetinfodialog.ui.h"
#include "pageinfodialog.ui.h"
#include "parameterinfodialog.ui.h"
#include "partinfodialog.ui.h"
#include "preferencesdialog.ui.h"
#include "rectangleinfodialog.ui.h"
#include "referencelineinfodialog.ui.h"
