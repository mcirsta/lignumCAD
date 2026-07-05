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

#include <QButtonGroup>
#include <QComboBox>
#include <QString>
#include <QTextEdit>
#include <QTreeWidget>
#include <QWhatsThis>

namespace {
  void applyObjectName( QObject* object, const char* name )
  {
    if ( name != nullptr )
      object->setObjectName( QString::fromLatin1( name ) );
  }

  void connectAboutDialog( aboutDialog* dialog )
  {
    QObject::connect( dialog->buttonOk, SIGNAL( clicked() ), dialog, SLOT( accept() ) );
  }

  void connectAnnotationInfoDialog( AnnotationInfoDialog* dialog )
  {
    QObject::connect( dialog->buttonOk, SIGNAL( clicked() ), dialog, SLOT( accept() ) );
    QObject::connect( dialog->buttonCancel, SIGNAL( clicked() ), dialog, SLOT( reject() ) );
    QObject::connect( dialog->boldButton, SIGNAL( toggled(bool) ), dialog, SLOT( boldButton_toggled(bool) ) );
    QObject::connect( dialog->italicButton, SIGNAL( toggled(bool) ), dialog, SLOT( italicButton_toggled(bool) ) );
    QObject::connect( dialog->underlineButton, SIGNAL( toggled(bool) ), dialog, SLOT( underlineButton_toggled(bool) ) );
    QObject::connect( dialog->colorButton, SIGNAL( clicked() ), dialog, SLOT( colorButton_clicked() ) );
    QObject::connect( dialog->annotationTextEdit, &QTextEdit::currentCharFormatChanged,
                      dialog, [dialog]() {
                        dialog->annotationTextEdit_currentColorChanged( dialog->annotationTextEdit->textColor() );
                        dialog->annotationTextEdit_currentFontChanged( dialog->annotationTextEdit->currentFont() );
                      } );
    QObject::connect( dialog->sizesComboBox, &QComboBox::textActivated,
                      dialog, &AnnotationInfoDialog::sizesComboBox_activated );
    QObject::connect( dialog->buttonHelp, SIGNAL( clicked() ), dialog, SLOT( buttonHelp_clicked() ) );
  }

  void connectAssemblyAddDialog( AssemblyAddDialog* dialog )
  {
    QObject::connect( dialog->buttonOk, SIGNAL( clicked() ), dialog, SLOT( accept() ) );
    QObject::connect( dialog->buttonCancel, SIGNAL( clicked() ), dialog, SLOT( reject() ) );
    dialog->modelListView->setSelectionMode( QAbstractItemView::SingleSelection );
    dialog->modelListView->setSelectionBehavior( QAbstractItemView::SelectRows );
    QObject::connect( dialog->modelListView, &QTreeWidget::currentItemChanged,
                      dialog, [dialog]( QTreeWidgetItem* current, QTreeWidgetItem* ) {
                        dialog->modelListView_selectionChanged( current );
                      } );
    QObject::connect( dialog->buttonHelp, SIGNAL( clicked() ), dialog, SLOT( buttonHelp_clicked() ) );
  }

  void connectAssemblyConfigDialog( AssemblyConfigDialog* dialog )
  {
    QObject::connect( dialog->buttonOk, SIGNAL( clicked() ), dialog, SLOT( accept() ) );
    QObject::connect( dialog->buttonCancel, SIGNAL( clicked() ), dialog, SLOT( reject() ) );
    dialog->modelListView->setSelectionMode( QAbstractItemView::SingleSelection );
    dialog->modelListView->setSelectionBehavior( QAbstractItemView::SelectRows );
    QObject::connect( dialog->modelListView, &QTreeWidget::currentItemChanged,
                      dialog, [dialog]( QTreeWidgetItem* current, QTreeWidgetItem* ) {
                        dialog->modelListView_selectionChanged( current );
                      } );
    QObject::connect( dialog->buttonHelp, SIGNAL( clicked() ), dialog, SLOT( buttonHelp_clicked() ) );
  }

  void connectAssemblyConstraintForm( AssemblyConstraintForm* form )
  {
    QButtonGroup* constraint_button_group = new QButtonGroup( form );
    constraint_button_group->setExclusive( true );
    constraint_button_group->addButton( form->matePushButton );
    constraint_button_group->addButton( form->alignPushButton );
    constraint_button_group->addButton( form->mateOffsetPushButton );
    constraint_button_group->addButton( form->alignOffsetPushButton );
    QObject::connect( form->helpPushButton, SIGNAL( clicked() ), form, SLOT( helpPushButton_clicked() ) );
  }

  void connectBasicDialogButtons( QDialog* dialog, QPushButton* buttonOk,
                                  QPushButton* buttonCancel, QPushButton* buttonHelp )
  {
    QObject::connect( buttonOk, SIGNAL( clicked() ), dialog, SLOT( accept() ) );
    QObject::connect( buttonCancel, SIGNAL( clicked() ), dialog, SLOT( reject() ) );
    QObject::connect( buttonHelp, SIGNAL( clicked() ), dialog, SLOT( buttonHelp_clicked() ) );
  }

  void connectLignumCADMainWindow( lignumCADMainWindow* window )
  {
    QObject::connect( window->fileNewAction, SIGNAL( triggered() ), window, SLOT( fileNew() ) );
    QObject::connect( window->fileOpenAction, SIGNAL( triggered() ), window, SLOT( fileOpen() ) );
    QObject::connect( window->fileSaveAction, SIGNAL( triggered() ), window, SLOT( fileSave() ) );
    QObject::connect( window->fileSaveAsAction, SIGNAL( triggered() ), window, SLOT( fileSaveAs() ) );
    QObject::connect( window->filePrintAction, SIGNAL( triggered() ), window, SLOT( filePrint() ) );
    QObject::connect( window->fileExitAction, SIGNAL( triggered() ), window, SLOT( fileExit() ) );
    QObject::connect( window->editUndoAction, SIGNAL( triggered() ), window, SLOT( editUndo() ) );
    QObject::connect( window->editRedoAction, SIGNAL( triggered() ), window, SLOT( editRedo() ) );
    QObject::connect( window->editCutAction, SIGNAL( triggered() ), window, SLOT( editCut() ) );
    QObject::connect( window->editCopyAction, SIGNAL( triggered() ), window, SLOT( editCopy() ) );
    QObject::connect( window->editPasteAction, SIGNAL( triggered() ), window, SLOT( editPaste() ) );
    QObject::connect( window->editFindAction, SIGNAL( triggered() ), window, SLOT( editFind() ) );
    QObject::connect( window->helpContentsAction, SIGNAL( triggered() ), window, SLOT( helpContents() ) );
    QObject::connect( window->helpAboutAction, SIGNAL( triggered() ), window, SLOT( helpAbout() ) );
    QObject::connect( window->exportPageAction, SIGNAL( triggered() ), window, SLOT( fileExport() ) );
    QObject::connect( window->modelInfoAction, SIGNAL( triggered() ), window, SLOT( fileModelInfo() ) );
    QObject::connect( window->whatsThisAction, &QAction::triggered,
                      window, []() { QWhatsThis::enterWhatsThisMode(); } );
  }

  void connectMaterialDialog( MaterialDialog* dialog )
  {
    QObject::connect( dialog->buttonOk, SIGNAL( clicked() ), dialog, SLOT( accept() ) );
    QObject::connect( dialog->buttonCancel, SIGNAL( clicked() ), dialog, SLOT( reject() ) );
    dialog->MaterialList->setSelectionMode( QAbstractItemView::SingleSelection );
    dialog->MaterialList->setSelectionBehavior( QAbstractItemView::SelectRows );
    QObject::connect( dialog->MaterialList, &QTreeWidget::currentItemChanged,
                      dialog, [dialog]( QTreeWidgetItem* current, QTreeWidgetItem* ) {
                        dialog->MaterialList_selectionChanged( current );
                      } );
    QObject::connect( dialog->buttonHelp, SIGNAL( clicked() ), dialog, SLOT( buttonHelp_clicked() ) );
  }

  void connectModelInfoDialog( ModelInfoDialog* dialog )
  {
    QObject::connect( dialog->buttonOk, SIGNAL( clicked() ), dialog, SLOT( accept() ) );
    QObject::connect( dialog->buttonCancel, SIGNAL( clicked() ), dialog, SLOT( reject() ) );
    QObject::connect( dialog->modelNameEdit, SIGNAL( textChanged(QString) ), dialog, SLOT( modelNameEdit_textChanged(QString) ) );
    QObject::connect( dialog->buttonHelp, SIGNAL( clicked() ), dialog, SLOT( buttonHelp_clicked() ) );
  }

  void connectNewModelWizard( NewModelWizard* wizard )
  {
    QObject::connect( wizard->modelNameEdit, SIGNAL( textChanged(QString) ), wizard, SLOT( modelNameEdit_textChanged(QString) ) );
    QObject::connect( wizard, SIGNAL( currentIdChanged(int) ), wizard, SLOT( NewModelWizard_currentIdChanged(int) ) );
    QObject::connect( wizard, SIGNAL( helpRequested() ), wizard, SLOT( NewModelWizard_helpClicked() ) );
  }

  void connectNewPartWizard( NewPartWizard* wizard )
  {
    wizard->partLibraryListView->setSelectionMode( QAbstractItemView::SingleSelection );
    wizard->partLibraryListView->setSelectionBehavior( QAbstractItemView::SelectRows );
    QObject::connect( wizard->partLibraryListView, &QTreeWidget::currentItemChanged,
                      wizard, [wizard]( QTreeWidgetItem* current, QTreeWidgetItem* ) {
                        wizard->partLibraryListView_currentChanged( current );
                      } );
    QObject::connect( wizard, SIGNAL( currentIdChanged(int) ), wizard, SLOT( NewPartWizard_currentIdChanged(int) ) );
    QObject::connect( wizard, SIGNAL( helpRequested() ), wizard, SLOT( NewPartWizard_helpClicked() ) );
  }

  void connectPreferencesDialog( PreferencesDialog* dialog )
  {
    QObject::connect( dialog->buttonOk, SIGNAL( clicked() ), dialog, SLOT( accept() ) );
    QObject::connect( dialog->buttonCancel, SIGNAL( clicked() ), dialog, SLOT( reject() ) );
    QObject::connect( dialog->logoFileChooser, SIGNAL( fileNameChanged(QString) ), dialog, SLOT( logoFileChooser_fileNameChanged(QString) ) );
    QObject::connect( dialog->unitsListBox, SIGNAL( currentItemChanged(QListWidgetItem*,QListWidgetItem*) ), dialog, SLOT( unitsListBox_currentChanged(QListWidgetItem*) ) );
    QButtonGroup* format_button_group = new QButtonGroup( dialog );
    format_button_group->setExclusive( true );
    format_button_group->addButton( dialog->fractionalRadioButton, 0 );
    format_button_group->addButton( dialog->decimalRadioButton, 1 );
    QObject::connect( format_button_group, SIGNAL( idClicked(int) ), dialog, SLOT( formatButtonGroup_clicked(int) ) );
    QObject::connect( dialog->precisionComboBox, SIGNAL( valueChanged(int) ), dialog, SLOT( precisionComboBox_valueChanged(int) ) );
    QObject::connect( dialog->colorSchemeListBox, SIGNAL( currentItemChanged(QListWidgetItem*,QListWidgetItem*) ), dialog, SLOT( colorSchemeListBox_currentChanged(QListWidgetItem*) ) );
    QObject::connect( dialog->geometryColorChooser, SIGNAL( colorChanged(QColor) ), dialog, SLOT( geometryColorChooser_colorChanged(QColor) ) );
    QObject::connect( dialog->annotationColorChooser, SIGNAL( colorChanged(QColor) ), dialog, SLOT( annotationColorChooser_colorChanged(QColor) ) );
    QObject::connect( dialog->gridColorChooser, SIGNAL( colorChanged(QColor) ), dialog, SLOT( gridColorChooser_colorChanged(QColor) ) );
    QObject::connect( dialog->constraintPrimaryColorChooser, SIGNAL( colorChanged(QColor) ), dialog, SLOT( constraintPrimaryColorChooser_colorChanged(QColor) ) );
    QObject::connect( dialog->constraintSecondaryColorChooser, SIGNAL( colorChanged(QColor) ), dialog, SLOT( constraintSecondaryColorChooser_colorChanged(QColor) ) );
    QObject::connect( dialog->backgroundSlider, SIGNAL( valueChanged(int) ), dialog, SLOT( backgroundSlider_valueChanged(int) ) );
    QObject::connect( dialog->solidBackgroundColorChooser, SIGNAL( colorChanged(QColor) ), dialog, SLOT( solidBackgroundColorChooser_colorChanged(QColor) ) );
    QObject::connect( dialog->gradientBackgroundColorChooser, SIGNAL( colorChanged(QColor) ), dialog, SLOT( gradientBackgroundColorChooser_colorChanged(QColor) ) );
    QObject::connect( dialog->patternBackgroundFileChooser, SIGNAL( fileNameChanged(QString) ), dialog, SLOT( patternBackgroundFileChooser_fileNameChanged(QString) ) );
    QObject::connect( dialog->dimensionFontChooser, SIGNAL( fontChanged(QString) ), dialog, SLOT( dimensionFontChooser_fontChanged(QString) ) );
    QObject::connect( dialog->arrowHeadLengthSpinBox, SIGNAL( valueChanged(double) ), dialog, SLOT( arrowHeadLengthSpinBox_valueChanged(double) ) );
    QObject::connect( dialog->arrowHeadStyleComboBox, SIGNAL( valueChanged(int) ), dialog, SLOT( arrowHeadStyleComboBox_valueChanged(int) ) );
    QObject::connect( dialog->clearanceLengthSpinBox, SIGNAL( valueChanged(double) ), dialog, SLOT( clearanceLengthSpinBox_valueChanged(double) ) );
    QObject::connect( dialog->lineThicknessLengthSpinBox, SIGNAL( valueChanged(double) ), dialog, SLOT( lineThicknessLengthSpinBox_valueChanged(double) ) );
    QObject::connect( dialog->extensionOffsetLengthSpinBox, SIGNAL( valueChanged(double) ), dialog, SLOT( extensionOffsetLengthSpinBox_valueChanged(double) ) );
    QObject::connect( dialog->annotationFontChooser, SIGNAL( fontChanged(QString) ), dialog, SLOT( annotationFontChooser_fontChanged(QString) ) );
    QObject::connect( dialog->handleLengthSpinBox, SIGNAL( valueChanged(double) ), dialog, SLOT( handleLengthSpinBox_valueChanged(double) ) );
    QObject::connect( dialog->arrowHeadWidthRatioSpinBox, SIGNAL( valueChanged(Ratio) ), dialog, SLOT( arrowHeadWidthRatioSpinBox_valueChanged(Ratio) ) );
    QObject::connect( dialog->buttonHelp, SIGNAL( clicked() ), dialog, SLOT( buttonHelp_clicked() ) );
  }

  void connectRectangleInfoDialog( RectangleInfoDialog* dialog )
  {
    QObject::connect( dialog->buttonOk, SIGNAL( clicked() ), dialog, SLOT( accept() ) );
    QObject::connect( dialog->buttonCancel, SIGNAL( clicked() ), dialog, SLOT( reject() ) );
    QObject::connect( dialog->rectangleStyleSlider, SIGNAL( valueChanged(int) ), dialog, SLOT( rectangleStyleSlider_valueChanged(int) ) );
    QObject::connect( dialog->buttonHelp, SIGNAL( clicked() ), dialog, SLOT( buttonHelp_clicked() ) );
  }
}

aboutDialog::aboutDialog( QWidget* parent, const char* name, bool modal,
                          Qt::WindowFlags flags )
  : QDialog( parent, flags )
{
  applyObjectName( this, name );
  setModal( modal );
  setupUi( this );
  connectAboutDialog( this );
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
  connectAnnotationInfoDialog( this );
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
  connectAssemblyAddDialog( this );
}

AssemblyAddDialog::~AssemblyAddDialog() = default;

AssemblyConfigDialog::AssemblyConfigDialog( QWidget* parent, const char* name,
                                            bool modal, Qt::WindowFlags flags )
  : QDialog( parent, flags )
{
  applyObjectName( this, name );
  setModal( modal );
  setupUi( this );
  connectAssemblyConfigDialog( this );
  init();
}

AssemblyConfigDialog::~AssemblyConfigDialog() = default;

AssemblyConstraintForm::AssemblyConstraintForm( QWidget* parent, const char* name,
                                                Qt::WindowFlags flags )
  : QWidget( parent, flags )
{
  applyObjectName( this, name );
  setupUi( this );
  connectAssemblyConstraintForm( this );
}

AssemblyConstraintForm::~AssemblyConstraintForm() = default;

CenterlineInfoDialog::CenterlineInfoDialog( QWidget* parent, const char* name,
                                            bool modal, Qt::WindowFlags flags )
  : QDialog( parent, flags )
{
  applyObjectName( this, name );
  setModal( modal );
  setupUi( this );
  connectBasicDialogButtons( this, buttonOk, buttonCancel, buttonHelp );
}

CenterlineInfoDialog::~CenterlineInfoDialog() = default;

DimensionInfoDialog::DimensionInfoDialog( QWidget* parent, const char* name,
                                          bool modal, Qt::WindowFlags flags )
  : QDialog( parent, flags )
{
  applyObjectName( this, name );
  setModal( modal );
  setupUi( this );
  connectBasicDialogButtons( this, buttonOk, buttonCancel, buttonHelp );
}

DimensionInfoDialog::~DimensionInfoDialog() = default;

lignumCADMainWindow::lignumCADMainWindow( QWidget* parent, const char* name,
                                          Qt::WindowFlags flags )
  : QMainWindow( parent, flags )
{
  applyObjectName( this, name );
  setupUi( this );
  connectLignumCADMainWindow( this );
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
  connectMaterialDialog( this );
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
  connectModelInfoDialog( this );
}

ModelInfoDialog::~ModelInfoDialog() = default;

NewModelWizard::NewModelWizard( QWidget* parent, const char* name,
                                bool modal, Qt::WindowFlags flags )
  : QWizard( parent, flags )
{
  applyObjectName( this, name );
  setModal( modal );
  setupUi( this );
  connectNewModelWizard( this );
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
  connectNewPartWizard( this );
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
  connectBasicDialogButtons( this, buttonOk, buttonCancel, buttonHelp );
}

OffsetInfoDialog::~OffsetInfoDialog() = default;

PageInfoDialog::PageInfoDialog( QWidget* parent, const char* name,
                                bool modal, Qt::WindowFlags flags )
  : QDialog( parent, flags )
{
  applyObjectName( this, name );
  setModal( modal );
  setupUi( this );
  connectBasicDialogButtons( this, buttonOk, buttonCancel, buttonHelp );
}

PageInfoDialog::~PageInfoDialog() = default;

ParameterInfoDialog::ParameterInfoDialog( QWidget* parent, const char* name,
                                          bool modal, Qt::WindowFlags flags )
  : QDialog( parent, flags )
{
  applyObjectName( this, name );
  setModal( modal );
  setupUi( this );
  connectBasicDialogButtons( this, buttonOk, buttonCancel, buttonHelp );
}

ParameterInfoDialog::~ParameterInfoDialog() = default;

PartInfoDialog::PartInfoDialog( QWidget* parent, const char* name,
                                bool modal, Qt::WindowFlags flags )
  : QDialog( parent, flags )
{
  applyObjectName( this, name );
  setModal( modal );
  setupUi( this );
  connectBasicDialogButtons( this, buttonOk, buttonCancel, buttonHelp );
}

PartInfoDialog::~PartInfoDialog() = default;

PreferencesDialog::PreferencesDialog( QWidget* parent, const char* name,
                                      bool modal, Qt::WindowFlags flags )
  : QDialog( parent, flags )
{
  applyObjectName( this, name );
  setModal( modal );
  setupUi( this );
  connectPreferencesDialog( this );
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
  connectRectangleInfoDialog( this );
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
  connectBasicDialogButtons( this, buttonOk, buttonCancel, buttonHelp );
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
