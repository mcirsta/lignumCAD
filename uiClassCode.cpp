#include "lignumcadmainwindow.h"
#include "newmodelwizard.h"
#include "modelinfodialog.h"

lignumCADMainWindow::lignumCADMainWindow(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);
    init();
}

Ui::lignumCADMainWindow* lignumCADMainWindow::getUi()
{
    return &ui;
}



aboutDialog* lignumCADMainWindow::about_dialog_ = 0;


void toGrayScale ( QImage image )
{
    for ( int j = 0; j < image.height(); j++ ) {
    for ( int i = 0; i < image.width(); i++ ) {
        QRgb pixel = image.pixel(i,j);
        uchar gray = qGray( pixel );
        uchar alpha = qAlpha( pixel )/4;
        image.setPixel( i, j, qRgba(gray,gray,gray,alpha) );
    }
    }
}

QImage trimTransparent ( QImage image )
{
  int i, j;
  for ( j = image.height()-1; j > 0; j-- ) {
    for ( i = 0; i < image.width(); i++ ) {
      if ( qAlpha( image.pixel( i, j ) ) != 0 ) goto done_height;
    }
  }
 done_height:
  int height = j + 1;

  for ( i = image.width()-1; i > 0; i-- ) {
    for ( j = 0; j < image.height(); j++ ) {
      if ( qAlpha( image.pixel( i, j ) ) != 0 ) goto done_width;
    }
  }
 done_width:
  int width = i + 1;

  return image.copy( 0, 0, width, height );
}

void addDisabledIcon ( QAction* action )
{
    QIconSet icon_set( action->iconSet().pixmap() );
    QImage image = icon_set.pixmap().convertToImage();
    toGrayScale( image );
    icon_set.setPixmap(QPixmap( image ), QIconSet::Small,
               QIconSet::Disabled );
    action->setIconSet( icon_set );
}

void lignumCADMainWindow::init ()
{
    // Write the title text
    titleLabel->setText( tr( "\n%1 Version %2.%3\nCopyright %4 %5\n%6\ne-mail: %7" ).
             arg( lC::STR::LIGNUMCAD ).
             arg( lC::STR::VERSION_MAJOR ).
             arg( lC::STR::VERSION_MINOR ).
             arg( lC::STR::COPYRIGHT_DATES ).
             arg( lC::STR::LIGNUMCOMPUTINGINC ).
             arg( lC::STR::LIGNUMCOMPUTINGURL ).
             arg( lC::STR::LIGNUMCOMPUTINGEMAIL ) );
    // Make disabled iconSets by converting the loaded pixmaps into
    // grayscale images.
    addDisabledIcon( fileSaveAction );
    addDisabledIcon( fileSaveAsAction );
    addDisabledIcon( filePrintAction );
    addDisabledIcon( exportPageAction );
    addDisabledIcon( modelInfoAction );
    addDisabledIcon( editUndoAction );
    addDisabledIcon( editRedoAction );
    addDisabledIcon( editCutAction );
    addDisabledIcon( editCopyAction );
    addDisabledIcon( editPasteAction );
    addDisabledIcon( editFindAction );
    addDisabledIcon( insertSketchAction );
    addDisabledIcon( insertPartAction );
    addDisabledIcon( insertAssemblyAction );
    addDisabledIcon( insertDrawingAction );
    addDisabledIcon( toolDeleteModelAction );

    // Construct smaller versions of the Page icons (used, at least, in
    // the What's This for the Tab bar).
    QMimeSourceFactory::defaultFactory()->
      setImage( "sketch_min.png", trimTransparent( lC::lookupImage( "sketch.png")));
    QMimeSourceFactory::defaultFactory()->
      setImage( "part_min.png", trimTransparent( lC::lookupImage( "part.png")));
    QMimeSourceFactory::defaultFactory()->
      setImage("assembly_min.png",trimTransparent(lC::lookupImage("assembly.png")));
    QMimeSourceFactory::defaultFactory()->
      setImage( "drawing_min.png", trimTransparent(lC::lookupImage("drawing.png")));

    // (Request: should be able to do this in Designer)
    QToolButton* whatsThis = QWhatsThis::whatsThisButton( toolBar );
    whatsThisAction->setIconSet( whatsThis->iconSet() );

    message_label_ = new QLabel(  tr( "lignumCAD 2002 lignum Computing, Inc."), statusBar(), "messageLabel" );
    QWhatsThis::add( message_label_,
             tr( "Lists program status and other messages." ) );
    statusBar()->addWidget( message_label_, 1 );

    information_label_ = new QLabel( tr( "none" ), statusBar(), "informationLabel" );
    QWhatsThis::add( information_label_,
             tr( "Shows the status of the current user interface action."));
    statusBar()->addWidget( information_label_, 0, true );

    page_label_ = new QLabel( tr( "Page: %1" ).arg( "none" ), statusBar(), "pageLabel" );
    QWhatsThis::add( page_label_,
             tr( "Shows the name of the current page." ) );
    statusBar()->addWidget( page_label_, 0, true );

    scale_label_ = new QLabel( tr( "Scale: 1 : 1" ), statusBar(), "scaleLabel" );
    QWhatsThis::add( scale_label_,
             tr( "Shows the scale of the current page." ) );
    statusBar()->addWidget( scale_label_, 0, true );
}

// The MainWindow delegates most of its actions to DesignBookView.

void lignumCADMainWindow::fileNew()
{
    design_book_view_->newModel();
}

void lignumCADMainWindow::fileOpen()
{
    design_book_view_->open();
}

void lignumCADMainWindow::fileSave()
{
    design_book_view_->save();
}

void lignumCADMainWindow::fileSaveAs()
{
    design_book_view_->saveAs();
}

void lignumCADMainWindow::filePrint()
{
  design_book_view_->print();
}

void lignumCADMainWindow::fileExit()
{
  // Can this actually veto exiting?

  if ( design_book_view_ != 0 )
    if ( !design_book_view_->aboutToExit() )
      return;

  // Save the window layout

  QSettings settings;
  QString buffer;
  QTextOStream layout_settings( &buffer );
  layout_settings << *this;
  settings.writeEntry( "/lignumComputing/lignumCAD/Geometries/Layout", buffer);

  delete design_book_view_;

  qApp->exit();
}

void lignumCADMainWindow::editUndo()
{
    // Well, although CommandHistory is a singleton and we could call
    // undo directly from here, there is also the little problem of
    // updating the screen. Since the main window doesn't know about
    // the OpenGL view, we have to delegate this to the DesignBookView
    // (which is fine really since almost all the undo/redo operations
    // affect something the user sees).
    design_book_view_->undo();
}

void lignumCADMainWindow::editRedo()
{
    // See comment in ::editUndo...
    design_book_view_->redo();
}

void lignumCADMainWindow::editCut()
{
    design_book_view_->cut();
}

void lignumCADMainWindow::editCopy()
{
   design_book_view_->copy();
}

void lignumCADMainWindow::editPaste()
{
   design_book_view_->paste();
}

void lignumCADMainWindow::editFind()
{

}

void lignumCADMainWindow::helpIndex()
{

}

void lignumCADMainWindow::helpContents()
{
  System::showManual( home_dir_ );
}

void lignumCADMainWindow::helpAbout()
{
    if ( about_dialog_ == 0 )
    about_dialog_ = new aboutDialog();

    about_dialog_->exec();
}

void lignumCADMainWindow::fileExport()
{
  design_book_view_->exportPage();
}

void lignumCADMainWindow::showView( const char * file_name )
{
    QString new_file_name( file_name );
    if ( !new_file_name.isEmpty() ) {
    // Apply a modest amount of checking to a command-line file name
    QFileInfo info( new_file_name );
    if ( !info.exists() ) {
        QMessageBox mb( tr( lC::STR::LIGNUMCAD ),
                tr( "The file \"%1\" does not exist." ).arg( new_file_name ),
                QMessageBox::Information,
                QMessageBox::Yes | QMessageBox::Default,
                QMessageBox::No,
                QMessageBox::Cancel );
        mb.setButtonText( QMessageBox::Yes, tr( "Select another file" ) );
        mb.setButtonText( QMessageBox::No, tr( "Create a new model" ) );
        mb.setButtonText( QMessageBox::Cancel, tr( "Exit lignumCAD" ) );

        switch ( mb.exec() ) {
        case QMessageBox::Yes:
            new_file_name =
            QFileDialog::getOpenFileName( QString::null,
                          tr( "lignumCAD (*.lcad);;All Files (*)" ),
                          0,
                          "open file dialog",
                          tr( "Choose a file" ) );
        break;
        case QMessageBox::No:
            new_file_name = QString::null;
        break;
        case QMessageBox::Cancel:
            exit(0 );
        }
    }
    }

    model_hierarchy_view_ = new QDockWindow( QDockWindow::InDock, this, "modelHierarchyView" );
    model_hierarchy_view_->setResizeEnabled( true );
    model_hierarchy_view_->setCaption( tr( "Model Hierarchy" ) );
    addDockWindow( model_hierarchy_view_, DockBottom );

    // See if the user has already set up a layout that they like.
    // NOTE: THE Qt CODE FOR THIS EXPECTS THE DOCKWINDOW TO HAVE
    // ALREADY BEEN CREATED! IT EXPECTS THE CAPTION NAME TO BE THE
    // SAME AS WHAT'S IN THE SETTINGS FILE.
    QSettings settings;
    QString buffer;
    buffer = settings.readEntry( lC::Setting::DOCK_GEOMETRIES );
    if ( !buffer.isEmpty() ) {
    QTextIStream ts( &buffer );
    ts >> *this;
    }

    model_hierarchy_list_ = new QTreeView( model_hierarchy_view_, "modelHierarchyList" );
    model_hierarchy_list_->setUniformRowHeights(true);
    model_hierarchy_list_->setRootIsDecorated( true );
    QStringList headerStrings ( tr( "Name" ) , tr( "Type" ) , tr( "Detail" ) );
    model_hierarchy_data->setHorizontalHeaderLabels(headerStrings);
    model_hierarchy_list_->addColumn(  );
    model_hierarchy_list_->addColumn(  );
    model_hierarchy_list_->addColumn( );
    model_hierarchy_list_->setSortingEnabled(false);

    ui.model_hierarchy_view_->boxLayout()->addWidget( model_hierarchy_list_ );
    model_hierarchy_list_->show();
    QWhatsThis::add( model_hierarchy_list_,
             tr( "<p><b>Model Hierarchy</b></p>\
<p>Shows a list of the pages and figures which compose \
the model.</p><p>You can rename some items by click-pause-clicking on the \
name field. Also note that you have to press <b>Enter</b> after typing in \
the name in order for the name change to be detected.</p>" ) );

    if ( !new_file_name.isEmpty() )
    design_book_view_ = new DesignBookView( this, new_file_name );
    else
    design_book_view_ = new DesignBookView( this );
}

void lignumCADMainWindow::fileModelInfo()
{
   design_book_view_->editModelInfo();
}

void lignumCADMainWindow::closeEvent( QCloseEvent * ce )
{
  if ( design_book_view_ != 0 )
    if ( !design_book_view_->aboutToExit() )
      return;
  ce->accept();
}

void lignumCADMainWindow::scaleChanged( const Ratio & scale )
{
    scale_label_->setText( tr( "Scale %1 : %2" ).arg( scale.numerator() ).arg( scale.denominator() ) );
}

void lignumCADMainWindow::pageChanged( const QString& name )
{
    page_label_->setText( tr( "Page: %1" ).arg( lC::formatName( name ) ) );
}

void lignumCADMainWindow::updateInformation( const QString& information )
{
  information_label_->setText( information );
}

QStandardItem* lignumCADMainWindow::modelHierarchyList()
{
    //TODO figure out what to return here
    return model_hierarchy_data->item(0);
}

void lignumCADMainWindow::showView()
{
    showView( 0 );
}


void lignumCADMainWindow::setHomeDir( QDir & home_dir )
{
    home_dir_ = home_dir;
}




NewModelWizard::NewModelWizard( QWidget *parent )
    : QWidget(parent)
{
    ui.setupUi(this);
}


PreferencesDialog::PreferencesDialog( QWidget *parent )
    : QWidget(parent)
{
    ui.setupUi(this);
}

Ui::PreferencesDialog* getUi() {
    return &ui;
}

PreferencesDialog::PageInfoDialog(QWidget *parent )
    : QWidget(parent)
{
    ui.setupUi(this);
}

Ui::PageInfoDialog* PageInfoDialog::getUi()
{
    return *ui;
}

NewModelWizard::NewModelWizard()
    :QWidget (parent)
{
    ui.setupUi(this);
    init();
}

Ui::NewModelWizard* NewModelWizard::getUi()
{
    return &ui;
}

void NewModelWizard::init()
{
    // Alright, admittedly this is kind of convoluted. The purpose of this complexity is
    // to allow page types to be added or deleted without having to look a dozen places
    // in the code for where to add or delete switches. So, we dynamically construct
    // radio buttons for each page type. The (invisible) button group makes them exclusive.

    initialPageGroupBox->setColumnLayout( 0, Qt::Vertical );

    initialPageButtonGroup = new QButtonGroup( this, "pageButtonGroup" );
    initialPageButtonGroup->hide();
    // Aquire the list of id's which identify the various page types. This list should be ordered
    // in a "logical" order.
    QValueVector<uint> page_ids = PageFactory::instance()->pageIDs();

    QValueVector<uint>::const_iterator id = page_ids.begin();
    for ( ; id != page_ids.end(); ++id ) {
    QString text = qApp->translate( "lignumCADMainWindow",
                    PageFactory::instance()->selection( *id ) );
    // Note: radio buttons are a child of the GroupBox
    QRadioButton* button = new QRadioButton( text, initialPageGroupBox );
    // But their exclusivity is controlled by the ButtonGroup (this is entirely so
    // that they can use their own id's).
    initialPageButtonGroup->insert( button, *id );
    QToolTip::add( button,
               qApp->translate( "lignumCADMainWindow",
                    PageFactory::instance()->toolTip( *id ) ) );
    QWhatsThis::add( button,
             qApp->translate( "lignumCADMainWindow",
                      PageFactory::instance()->whatsThis( *id ) ) );
    initialPageGroupBox->layout()->add( button );
    // Keep a list of these around since QButtonGroup doesn't have an iterator and our
    // ids are not necessarily sequential.
    initialPageRadioButtons.append( button );
    }

    connect( initialPageButtonGroup, SIGNAL( clicked(int) ), SLOT(initialPageSelected() ) );
}
/*!
  * This is a little gloss on entering a new model name. The file name string is
  * updated as the model name is typed.
  * \param text current text of model name widget.
  */
void NewModelWizard::modelNameEdit_textChanged( const QString & text )
{
  modelFileChooser->setFileName( text + ".lcad" );
  modelFileChooser->setEdited( true );
}

/*!
 * Allow the user to finish the new model wizard now.
 */
void NewModelWizard::initialPageSelected( void )
{
  finishButton()->setEnabled( true );
  finishButton()->setDefault( true );
}

/*!
 * Improve the response to the keyboard by manipulating the focus.
 * The focus can get stuck in the description TextEdit widget. (The page
 * title parameter is ignored.)
 */
void NewModelWizard::NewModelWizard_selected( const QString & )
{
  if ( currentPage() == NewModelPage ) {
    modelNameEdit->setFocus();
    nextButton()->setDefault( true );
  }
  else if ( currentPage() == InitialPagePage ) {
    initialPageButtonGroup->setFocus();
    finishButton()->setDefault( true );
  }
}


void NewModelWizard::NewModelWizard_helpClicked()
{
    if ( currentPage() == NewModelPage ) {
    QWhatsThis::display( tr( "<p><b>New Model Page</b></p>\
<p>Enter the basic information about the model. \
(This is sometimes called the <i>metadata</i>.) \
The metadata includes:\
<ul>\
<li>Model name</li>\
<li>File to save the model in (defaults to <i>ModelName</i>.lcad)</li>\
<li>Initial version and revision numbers</li>\
<li>Optional description of the model</li>\
<li>Date and time of creation</li>\
<li>Date and time of last modification</li>\
</ul>\
The creation and last modification times \
are updated automatically.</p>\
<p>When the fields are filled in to your satisfaction, \
click the <b>Next</b> button \
(or press <b>Enter</b> or <b>Alt+N</b>) to \
proceed to the next page.</p>\
<p>If you click the <b>Cancel</b> button \
(or press <b>ESC</b> or <b>Alt+C</b>), \
no new model will be created.</p>" ) );
    }
    else if ( currentPage() == InitialPagePage ) {
    QWhatsThis::display( tr( "<p><b>Initial Page</b></p>\
<p><i>lignumCAD</i> requires that you start your \
model with at least one page. Typically, you would \
start with a few sketches to establish the overall \
dimensions of the model; but, you are free to \
start with any page type.</p>\
<p>After you choose a page type \
from the list, the <b>Finish</b> button will be activated. \
You can click that \
(or press <b>Enter</b> or <b>Alt+F</b>) and \
the new model with the specified initial page \
will be created.</p>\
<p>You can also click <b>Back</b> \
(or press <b>Alt+B</b>) \
to edit the model information some more.</p>\
<p>If you click <b>Cancel</b> \
(or press <b>ESC</b> or <b>Alt+C</b>), \
no new model will be created.</p>" ) );
    }
}

/*!
  * Evidently, this is the only way to introduce a public method into a Designer
  * dialog. Would prefer to return the value, but we'll have to settle for the
  * side-effect instead.
  * \param type set to the name of the selected initial page type.
  */
void NewModelWizard::selectedPage( uint & type )
{
    QPtrListIterator<QRadioButton> rb( initialPageRadioButtons );
    for ( ; rb.current() != 0; ++rb ) {
    if ( (*rb)->isChecked() ) {
        type = initialPageButtonGroup->id( *rb );
        return;
    }
    }
    // Should not happen.
   type = 0;
}

/*!
  * Turn off any page radio buttons which happen to be on.
  */
void NewModelWizard::unsetInitialPages( void )
{
    QPtrListIterator<QRadioButton> rb( initialPageRadioButtons );
    for ( ; rb.current() != 0; ++rb )
    (*rb)->setChecked( false );
}


ModelInfoDialog::ModelInfoDialog ()
    :QWidget ( parent )
{
    ui.setupUi( this );
}

Ui::ModelInfoDialog* ModelInfoDialog::getUi ()
{
    return &ui;
}
