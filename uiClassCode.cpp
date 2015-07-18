#include "lignumcadmainwindow.h"
#include "newmodelwizard.h"
#include "modelinfodialog.h"
#include "newpartwizard.h"
#include "materialdialog.h"
#include "newmodelwizard.h"
#include "pagefactory.h"
#include "assemblyconfigdialog.h"
#include "assemblyadddialog.h"
#include "listviewitem.h"
#include "assemblyconstraintform.h"
#include "offsetinfodialog.h"
#include "referencelineinfodialog.h"
#include "centerlineinfodialog.h"
#include "dimensioninfodialog.h"
#include "rectangleinfodialog.h"
#include "annotationinfodialog.h"
#include "partinfodialog.h"
#include "parameterinfodialog.h"
#include "aboutdialog.h"

#include "configuration.h"
#include "systemdependencies.h"

#include "Standard_Version.hxx"
#include "freetype/fttypes.h"
#include "freetype/freetype.h"
#include "OGLFT.h"

#include <QSettings>
#include <QMessageBox>
#include <QFileDialog>
#include <QBoxLayout>
#include <QCloseEvent>
#include <QWhatsThis>
#include <QFontDatabase>
#include <QColorDialog>

lignumCADMainWindow::lignumCADMainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    init();
}

Ui::lignumCADMainWindow* lignumCADMainWindow::getUi()
{
    return &ui;
}

aboutDialog::aboutDialog( QWidget* parent )
    :QDialog( parent )
{
    ui.setupUi( this );
    init();
}

/*!
 * Initialize the About dialog.
 */
void aboutDialog::init()
{
    ui.programVersionLabel->setText( tr( "%1 Version %2.%3" ).
                                  arg( lC::STR::LIGNUMCAD ).
                                  arg( lC::STR::VERSION_MAJOR ).
                                  arg( lC::STR::VERSION_MINOR ) );
    ui.copyrightLabel->setText( tr( "Copyright © %1 %2" ).
                             arg( lC::STR::COPYRIGHT_DATES ).
                             arg( lC::STR::LIGNUMCOMPUTINGINC ) );
    ui.contactLabel->setText( tr( "%1\ne-mail: %2" ).
                           arg( lC::STR::LIGNUMCOMPUTINGURL ).
                           arg( lC::STR::LIGNUMCOMPUTINGEMAIL ) );
    // Try to figure out where all these things come from:
    // OpenCASCADE
    ui.openCASCADEVersionLabel->setText( tr( "Version: %1" ).
                                      arg( OCC_VERSION_STRING ) );
    // FreeType (this interface is only available in very recent versions of FreeType)
    {
        FT_Library library = OGLFT::Library::instance();
        FT_Int major, minor, patch;
        FT_Library_Version( library, &major, &minor, &patch );
        ui.freeTypeVersionLabel->setText( tr( "Version: %1.%2.%3" ).arg(major).arg(minor).arg(patch) );
    }
    // Qt
    ui.qtVersionLabel->setText( tr( "Version: %1" ).arg( qVersion() ) );
    // SGI's  OpenGL GLU
    ui.openGLGLUVersionLabel->setText( tr( "Version: %1" ).
                                    arg( (char*)gluGetString( GLU_VERSION ) ) );
    // OpenGL driver
    ui.openGLVersionLabel->setText( tr( "Vendor: %1 Version: %2" ).
                                 arg( (char*)glGetString( GL_VENDOR ) ).
                                 arg( (char*)glGetString( GL_VERSION ) ) );
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

void lignumCADMainWindow::init ()
{
    // Write the title text
    ui.titleLabel->setText( tr( "\n%1 Version %2.%3\nCopyright %4 %5\n%6\ne-mail: %7" ).
                            arg( lC::STR::LIGNUMCAD ).
                            arg( lC::STR::VERSION_MAJOR ).
                            arg( lC::STR::VERSION_MINOR ).
                            arg( lC::STR::COPYRIGHT_DATES ).
                            arg( lC::STR::LIGNUMCOMPUTINGINC ).
                            arg( lC::STR::LIGNUMCOMPUTINGURL ).
                            arg( lC::STR::LIGNUMCOMPUTINGEMAIL ) );

    // Construct smaller versions of the Page icons (used, at least, in
    // the What's This for the Tab bar).
    //TODO
    //    QMimeSourceFactory::defaultFactory()->
    //      setImage( "sketch_min.png", trimTransparent( lC::lookupImage( "sketch.png")));
    //    QMimeSourceFactory::defaultFactory()->
    //      setImage( "part_min.png", trimTransparent( lC::lookupImage( "part.png")));
    //    QMimeSourceFactory::defaultFactory()->
    //      setImage("assembly_min.png",trimTransparent(lC::lookupImage("assembly.png")));
    //    QMimeSourceFactory::defaultFactory()->
    //      setImage( "drawing_min.png", trimTransparent(lC::lookupImage("drawing.png")));

    // (Request: should be able to do this in Designer)
    //TODO in designer
    //QToolButton* whatsThis = QWhatsThis::whatsThisButton( toolBar );
    //whatsThisAction->setIconSet( whatsThis->iconSet() );

    message_label_ = new QLabel(  tr( "lignumCAD 2002 lignum Computing, Inc."), statusBar() );
    message_label_->setObjectName( "messageLabel" );
    message_label_->setWhatsThis( tr( "Lists program status and other messages." ) );
    statusBar()->addWidget( message_label_, 1 );

    information_label_ = new QLabel( tr( "none" ), statusBar() );
    information_label_->setObjectName( "informationLabel" );
    information_label_->setWhatsThis( tr( "Shows the status of the current user interface action."));
    statusBar()->addWidget( information_label_, 0 );

    page_label_ = new QLabel( tr( "Page: %1" ).arg( "none" ), statusBar() );
    page_label_->setObjectName( "pageLabel" );
    page_label_->setWhatsThis( tr( "Shows the name of the current page." ) );
    statusBar()->addWidget( page_label_, 0 );

    scale_label_ = new QLabel( tr( "Scale: 1 : 1" ), statusBar() );
    scale_label_->setObjectName( "scaleLabel" );
    scale_label_->setWhatsThis( tr( "Shows the scale of the current page." ) );
    statusBar()->addWidget( scale_label_, 0 );

    model_hierarchy_data =  std::make_shared<QStandardItemModel>();
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
    settings.setValue("geometry", saveGeometry());
    settings.setValue("windowState", saveState());

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
            QMessageBox mb( tr( lC::STR::cLIGNUMCAD ),
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
                        QFileDialog::getOpenFileName( this,
                                                      tr( "Choose a file" ),
                                                      QString::null,
                                                      tr( "lignumCAD (*.lcad);;All Files (*)" ) );
                break;
            case QMessageBox::No:
                new_file_name = QString::null;
                break;
            case QMessageBox::Cancel:
                exit(0 );
            }
        }
    }

    model_hierarchy_view_ = new QDockWidget( this );
    model_hierarchy_view_->setObjectName( "modelHierarchyView" );
    //model_hierarchy_view_->setResizeEnabled( true );
    model_hierarchy_view_->setWindowIconText( tr( "Model Hierarchy" ) );
    addDockWidget(Qt::BottomDockWidgetArea, model_hierarchy_view_ );

    // See if the user has already set up a layout that they like.
    // NOTE: THE Qt CODE FOR THIS EXPECTS THE DOCKWINDOW TO HAVE
    // ALREADY BEEN CREATED! IT EXPECTS THE CAPTION NAME TO BE THE
    // SAME AS WHAT'S IN THE SETTINGS FILE.
    //TODO needed ?
    //    QSettings settings;
    //    QString buffer;
    //    buffer = settings.readEntry( lC::Setting::DOCK_GEOMETRIES );
    //    if ( !buffer.isEmpty() ) {
    //    QTextIStream ts( &buffer );
    //    ts >> *this;
    //    }

    model_hierarchy_list_ = new QTreeWidget( model_hierarchy_view_ );
    model_hierarchy_list_->setObjectName( "modelHierarchyList" );
    model_hierarchy_list_->setUniformRowHeights(true);
    model_hierarchy_list_->setRootIsDecorated( true );
    QStringList headerStrings;
    headerStrings<< tr( "Name" ) << tr( "Type" ) << tr( "Detail" );
    model_hierarchy_data->setHorizontalHeaderLabels(headerStrings);
    model_hierarchy_list_->setColumnCount( 3 );
    model_hierarchy_list_->setSortingEnabled(false);

    model_hierarchy_view_->setLayout( new QBoxLayout(QBoxLayout::TopToBottom,  model_hierarchy_view_) );
    model_hierarchy_view_->setWidget( model_hierarchy_list_ );
    model_hierarchy_list_->show();
    model_hierarchy_list_->setWhatsThis(
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


PreferencesDialog::PreferencesDialog( QWidget *parent )
    : QDialog(parent)
{
    ui.setupUi(this);
}

Ui::PreferencesDialog* PreferencesDialog::getUi() {
    return &ui;
}

PageInfoDialog::PageInfoDialog(QWidget *parent )
    : QDialog(parent)
{
    ui.setupUi(this);
}

Ui::PageInfoDialog* PageInfoDialog::getUi()
{
    return &ui;
}

NewModelWizard::NewModelWizard(QWidget* parent)
    :QWizard (parent)
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

    //TODO
    //ui.initialPageGroupBox->setColumnLayout( 0, Qt::Vertical );

    initialPageButtonGroupBox = new QGroupBox ( this );
    initialPageButtonGroupBox->setObjectName( "pageButtonGroupbox" );
    initialPageButtonGroupBox->hide();
    initialPageButtonGroup = new QButtonGroup( initialPageButtonGroupBox );
    initialPageButtonGroup->setObjectName( "pageButtonGroup" );
    QVBoxLayout *vbox = new QVBoxLayout( initialPageButtonGroupBox );

    // Aquire the list of id's which identify the various page types. This list should be ordered
    // in a "logical" order.
    QVector<uint> page_ids = PageFactory::instance()->pageIDs();

    QVector<uint>::const_iterator id = page_ids.begin();

    for ( ; id != page_ids.end(); ++id ) {
        QString text = qApp->translate( "lignumCADMainWindow",
                                        PageFactory::instance()->selection( *id ).toLatin1() );
        // Note: radio buttons are a child of the GroupBox
        QRadioButton* button = new QRadioButton( text, ui.initialPageGroupBox );
        // But their exclusivity is controlled by the ButtonGroup (this is entirely so
        // that they can use their own id's).
        initialPageButtonGroup->addButton( button, *id );
        vbox->addWidget( button );
        button->setToolTip( qApp->translate( "lignumCADMainWindow",
                                             PageFactory::instance()->toolTip( *id ).toStdString().c_str() ));
        button->setWhatsThis( qApp->translate( "lignumCADMainWindow",
                                               PageFactory::instance()->whatsThis( *id ).toStdString().c_str() ) );
        // Keep a list of these around since QButtonGroup doesn't have an iterator and our
        // ids are not necessarily sequential.
        initialPageRadioButtons.append( std::shared_ptr<QRadioButton>(button) );
    }

    initialPageButtonGroupBox->setLayout( vbox );
    connect( initialPageButtonGroup, SIGNAL( buttonClicked(int) ), SLOT(initialPageSelected() ) );
}
/*!
  * This is a little gloss on entering a new model name. The file name string is
  * updated as the model name is typed.
  * \param text current text of model name widget.
  */
void NewModelWizard::modelNameEdit_textChanged( const QString & text )
{
    ui.modelFileChooser->setFileName( text + ".lcad" );
    ui.modelFileChooser->setEdited( true );
}

/*!
 * Allow the user to finish the new model wizard now.
 */
void NewModelWizard::initialPageSelected( void )
{
    //TODO
    // finishButton()->setEnabled( true );
    //finishButton()->setDefault( true );
}

/*!
 * Improve the response to the keyboard by manipulating the focus.
 * The focus can get stuck in the description TextEdit widget. (The page
 * title parameter is ignored.)
 */
void NewModelWizard::NewModelWizard_selected( const QString & )
{
    if ( currentPage() == ui.NewModelPage ) {
        ui.modelNameEdit->setFocus();
        //TODO
        //ui.nextButton()->setDefault( true );
    }
    else if ( currentPage() == ui.InitialPagePage ) {
        initialPageButtonGroupBox->setFocus();
        //TODO
        //finishButton()->setDefault( true );
    }
}


void NewModelWizard::NewModelWizard_helpClicked()
{
    if ( currentPage() == ui.NewModelPage ) {
        this->setWhatsThis( tr( "<p><b>New Model Page</b></p>\
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
    else if ( currentPage() == ui.InitialPagePage ) {
        this->setWhatsThis( tr( "<p><b>Initial Page</b></p>\
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
    QListIterator<std::shared_ptr<QRadioButton>> rb( initialPageRadioButtons );
    while ( rb.hasNext() ) {
        if ( rb.peekNext().get()->isChecked() ) {
            type = initialPageButtonGroup->id( rb.peekNext().get() );
            return;
        }
        rb.next();
    }
    // Should not happen.
    type = 0;
}

/*!
  * Turn off any page radio buttons which happen to be on.
  */
void NewModelWizard::unsetInitialPages( void )
{
    QListIterator<std::shared_ptr<QRadioButton>> rb( initialPageRadioButtons );
    while ( rb.hasNext() )
        rb.next()->setChecked( false );
}


ModelInfoDialog::ModelInfoDialog (QWidget *parent)
    :QDialog ( parent )
{
    ui.setupUi( this );
}

Ui::ModelInfoDialog* ModelInfoDialog::getUi ()
{
    return &ui;
}

void ModelInfoDialog::modelNameEdit_textChanged( const QString & text )
{
    ui.modelFileChooser->setFileName( text + ".lcad" );
    ui.modelFileChooser->setEdited( true );
}


void ModelInfoDialog::buttonHelp_clicked()
{
    QWhatsThis::showText( QCursor::pos(), tr( "<p><b>Edit Model Information</b></p>\
                                              <p>Modify the basic information about the model. \
                                              (This is sometimes called the <i>metadata</i>.) \
                                              The metadata includes:\
                                              <ul>\
                                              <li>Model name</li>\
                                              <li>File to save the model in (defaults to <i>ModelName</i>.lcad)</li>\
                                              <li>Version and revision numbers</li>\
                                              <li>Optional description of the model</li>\
                                              <li>Date and time of creation</li>\
                                              <li>Date and time of last modification</li>\
                                              </ul>\
                                              The creation and last modification times \
                                              are updated automatically.</p>\
                                              <p>When the fields are modified to your satisfaction, \
                                              click the <b>OK</b> button (or press <b>Enter</b> or \
                                                                          <b>Alt+O</b>) to \
                                              apply the changes and exit the dialog.</p>\
                                              <p>If you click the <b>Cancel</b> button \
                                              (or press <b>ESC</b> or <b>Alt+C</b>), \
                                              no changes to the model will be made.</p>" ) );

}


NewPartWizard::NewPartWizard(QWidget* parent)
    :QWizard(parent)
{
    ui.setupUi(this);
    init();
}

Ui::NewPartWizard* NewPartWizard::getUi()
{
    return &ui;
}

void NewPartWizard::init()
{
    scroll_view_ = 0;

    //TODO
    //nextButton()->setEnabled( false );
    //finishButton()->setEnabled( false );

    QListIterator<std::shared_ptr<PartMetadata>> part = PartFactory::instance()->parts();
    while ( part.hasNext() ) {
        QMultiHash<QString, std::shared_ptr<ListViewItem>>::const_iterator foundIt
                = groups_.find( trC( part.peekNext()->group() ) );
        std::shared_ptr <ListViewItem> group;
        if ( foundIt == groups_.cend()) {
            group = std::shared_ptr<ListViewItem>(new ListViewItem( trC( part.peekNext()->group() ) ));
            //TODO
            //group->setOpen( true );
            //group->setSelectable( false );
        }
        else {
            group = foundIt.value();
        }
        groups_.insert( trC( part.peekNext()->group() ), group);
        QListWidgetItem* item = new QListWidgetItem( trC( part.peekNext()->name() ) );
        //TODO
        //item->setOpen( true );
        //item->setPixmap( 1, lC::lookupPixmap( part.current()->icon() ) );
        parts_.insert( item, part.peekNext().get() );
        part.next();
    }

    //TODO
    //connect( nextButton(), SIGNAL( clicked() ), SLOT( validateName() ) );
}

void NewPartWizard::partLibraryListView_currentChanged(QListWidgetItem *item )
{
    //TODO
    //    if ( item != 0 ) {
    //        if ( groups_.find( item->text( ) ) != groups_.cend() )
    //            nextButton()->setEnabled( false );
    //        else
    //            nextButton()->setEnabled( true );
    //    }
    //    else
    //        nextButton()->setEnabled( false );
}


void NewPartWizard::NewPartWizard_selected( const QString& /*page_name*/ )
{
    if ( currentPage() == ui.initialPartPage ) {
        partLibraryListView_currentChanged( ui.partLibraryListView->currentItem() );
        ui.partLibraryListView->setFocus();
        //TODO
        //nextButton()->setDefault( true );
    }
    else if ( currentPage() == ui.partParametersPage ) {
        QListWidgetItem* item = ui.partLibraryListView->currentItem();
#if 0
        PartMetadata* part = PartFactory::instance()->part( item->parent()->text(0),
                                                            item->text( 0 ) );
#else
        PartMetadata* part = parts_[ item ];
#endif
        ui.partParameterFrame->setTitle( tr( "&Parameters for %1::%2" ).
                                         arg( trC( part->group() ) ).
                                         arg( lC::formatName( part->name() ) ) );
        // Construct how ever many input parameter fields this part needs.
        uint n_parameters = part->parameterCount();
        if ( scroll_view_ == 0 ) {
            QGridLayout* layout = new QGridLayout( ui.partParameterFrame );
            scroll_view_ = new QScrollArea( ui.partParameterFrame );
            scroll_view_->setObjectName( "parameterScrollView" );
            layout->addWidget( scroll_view_, 0, 0 );
            scroll_vbox_ = new QWidget ( scroll_view_->viewport() );
            scroll_vbox_->setObjectName( "parameterVBox" );
            scroll_view_->setWidget( scroll_vbox_ );
            scroll_view_->show();
        }

        for ( uint n = labels_.count(); n < n_parameters; n++ ) {
            lCDefaultLengthConstraint* length_constraint =
                    new lCDefaultLengthConstraint( scroll_vbox_, "parameterLabel" );
            length_constraint->setLengthLimits( UnitsBasis::instance()->lengthUnit(),
                                                UnitsBasis::instance()->format(),
                                                UnitsBasis::instance()->precision(),
                                                0,
                                                lC::MAXIMUM_DIMENSION, 0 );
            length_constraint->setSpecifiedButtonToolTip( tr( "Use a specified size." ) );
            length_constraint->setSpecifiedButtonWhatsThis( tr("<p><b>Specified Size</b></p> <p>Select this option if you want the parameter to have an independent, specified size.</p>" ) );
            length_constraint->setSpecifiedSpinBoxToolTip( tr( "Enter the size." ) );
            length_constraint->setSpecifiedSpinBoxWhatsThis( tr( "<p><b>Specified Size</b></p> <p>Enter the size of the parameter. The units of the value are given in the default units specified in the application Preferences (so, you do not have to enter the units abbreviation).</p> <p>If the default units format is DECIMAL, then the entered value can have the usual floating point representation, e.g.:</p> <p><code>1.234</code></p> <p>If the default units format is FRACTIONAL, then, in addition to the decimal format, any of the following representations can be typed in:</p> <p><code>1</code> (equals 1.0)</p> <p><code>1/2</code> (equals 0.5)</p> <p><code>1 1/2</code> (equals 1.5, note the blank separating the whole number and the fraction)</p> <p>If you modify the value, you can always go back to the default value when the dialog was opened by clicking the <img src=\"default_active.png\"> button</p>" ) );
            connect( length_constraint, SIGNAL( valueChanged( double ) ),
                     this, SLOT(updateValidity( double ) ) );
            labels_.append( std::shared_ptr<lCDefaultLengthConstraint>(length_constraint) );
        }
        QStringList::const_iterator parameter = part->parameters();
        QList<std::shared_ptr<lCDefaultLengthConstraint>>::const_iterator label = labels_.cbegin();
        parameter_labels_.clear();
        for ( uint n = 0; n < n_parameters; n++ ) {
            std::shared_ptr<lCDefaultLengthConstraint> parameter_label = *label;
            parameter_label->setTitle( trC( *parameter ) );
            parameter_label->setDefaultLength( 0 );
            parameter_label->setLength( 0 );
            parameter_label->show();
            parameter_labels_.insert( *parameter, parameter_label );
            ++parameter;
            ++label;
        }
        for ( ; label != labels_.cend(); ++label ) {
            label->get()->hide();
        }
        scroll_view_->ensureVisible( 0, 0 );
        if ( n_parameters > 0 && labels_.count() > 0 )
            labels_.at(0)->setFocus();
        //TODO
        //finishButton()->setDefault( true );
    }
}


const QHash<QString,std::shared_ptr<lCDefaultLengthConstraint>>& NewPartWizard::parameters( void )
{
    return parameter_labels_;
}


const PartMetadata* NewPartWizard::part( void )
{
    QListWidgetItem* item = ui.partLibraryListView->currentItem();
#if 0
    return PartFactory::instance()->part( item->parent()->text(0), item->text( 0 ) );
#else
    return parts_[ item ];
#endif
}


void NewPartWizard::updateValidity( double )
{
    // One of the parameter boxes was changed, so recheck the validity of the input.
    //TODO
    //    if ( part()->valid(parameter_labels_) )
    //        finishButton()->setEnabled( true );
}


void NewPartWizard::NewPartWizard_helpClicked()
{
    if ( currentPage() == ui.initialPartPage ) {
        QWhatsThis::showText( QCursor::pos(), tr( "<p><b>Initial Part Page</b></p>\
                                                  <p>Each Part is based on a three-dimensional solid geometry model. From this page, \
                                                  you can select the base solid geometry. The list shows the predefined solid templates. There \
                                                  are essentially two kinds of base solids: blanks and customized parts. The blanks \
                                                  include a regular rectangular parallelipiped, called a <em>board</em>, and a \
                                                  cylinder, called a <em>turning</em>. From these shapes, you can generate any \
                                                  other shape through the use of milling operations.</p>\
                                                  <p>The customized parts represent items which are you are more likely to buy pre-made. \
                                                  (Eventually, you will also be able to save your own creations as templates as well.)</p>\
                                                  <p>You should also set the name of the part (although you can change it later, too).</p>\
                                                  <p>When the fields are filled in to your satisfaction, \
                                                  click the <b>Next</b> button \
                                                  (or press <b>Enter</b> or <b>Alt+N</b>) to \
                                                  proceed to the next page.</p>\
                                                  <p>If you click the <b>Cancel</b> button \
                                                  (or press <b>ESC</b> or <b>Alt+C</b>), \
                                                  no Part will be generated.</p>" ) );
    }
    else if ( currentPage() == ui.partParametersPage ) {
        QWhatsThis::showText( QCursor::pos(), tr( "<p><b>Part Parameters Page</b></p>\
                                                  <p>On this page, the parameters (dimensions or sizes) of the template solids are \
                                                  defined. When acceptable \
                                                  values are entered for all parameters, the <b>Finish</b> button will become \
                                                  active and the solid is ready to be generated. You can click that \
                                                  (or press <b>Enter</b> or <b>Alt+F</b>) and \
                                                  the new part will be created.</p>\
                                                  <p>You can also click <b>Back</b> \
                                                  (or press <b>Alt+B</b>) \
                                                  to change the base solid template. (Note that any values you have entered in \
                                                                                      the parameter input boxes will be forgotten when you return to this page, \
                                                                                      even if you select the same \
                                                                                      base solid template.)</p>\
                                                  <p>If you click <b>Cancel</b> \
                                                  (or press <b>ESC</b> or <b>Alt+C</b>), \
                                                  no new Part will be created.</p></p>" ) );
    }
}

void NewPartWizard::validateName( void )
{
    // Check that no part already has this name.
    int ret = part_view_->parent()->uniquePageName( part_view_, ui.nameEdit->text(), lC::STR::PART );
    switch ( ret ) {
    case lC::Redo:
        //TODO
        //showPage( ui.initialPartPage );
        break;
    case lC::Rejected:
        reject();
    }
}


void NewPartWizard::setPartView( PartView * part_view )
{
    part_view_ = part_view;
}


QString NewPartWizard::trC( const QString & string )
{
    return qApp->translate( "Constants", string.toStdString().c_str() );
}

MaterialDialog::MaterialDialog( QWidget* parent )
    :QDialog( parent)
{
    ui.setupUi(this);
    init();
}

Ui::MaterialDialog* MaterialDialog::getUi()
{
    return &ui;
}

void MaterialDialog::init()
{
    QHashIterator<QString,std::shared_ptr<Material>> material = MaterialDatabase::instance().materials();

    //TODO
    //    while ( material.hasNext() ) {

    //        int row = 0;
    //        QListViewItem* class_item = ui.MaterialList->item( row)
    //        while ( class_item != 0 ) {
    //            if ( class_item->text( 0 ) == material.current()->materialClass() ) break;
    //            class_item = class_item->nextSibling();
    //        }
    //        if ( class_item == 0 ) {
    //            class_item = new QListViewItem( MaterialList,
    //                                            material.current()->materialClass() );
    //            class_item->setSelectable( false );
    //            class_item->setOpen( true );
    //        }
    //        new QListViewItem( class_item, material.current()->name() );
    //    }
}


void MaterialDialog::MaterialList_selectionChanged(QListWidgetItem *item )
{
    if ( item == 0 ) {
        ui.MaterialText->setText( QString::null );

        //TODO
        //ui.SolidColor->setPaletteBackgroundColor( colorGroup().background() );

        ui.FaceGrainPixmap->setPixmap( QPixmap() );
        ui.EndGrainPixmap->setPixmap( QPixmap() );
        ui.EdgeGrainPixmap->setPixmap( QPixmap() );
        return;
    }

    Material* material = MaterialDatabase::instance().material( item->text( ) );
    QString material_text;
    QString botanical_text = material->botanical() != "-" ? material->botanical() : tr( "none" );
    material_text = tr( "<p>Common name: %1</p><p>Botanical name: %2</p><p>Other names: %3</p><p>Specific gravity: %4" ).
            arg( material->name() ).
            arg( botanical_text ).
            arg( material->otherNames() ).
            arg( material->specificGravity() );

    QString compression_table;
    compression_table = tr( "<p align=\"center\">Compression Strengths</p><table border=\"1\"><tr><th>ll to Grain FSPL (PSI)</th><th>|| to Grain MCS (PSI)</th><th>T to Grain FSPL (PSI)</th></tr><tr><td>%1</td><td>%2</td><td>%3</td></tr></table>" ).
            arg( material->compStrengthFSPLPar() ).
            arg( material->compStrengthMCSPar() ).
            arg( material->compStrengthFSPLPerp() );
    QString tension_shear_table;
    tension_shear_table = tr( "<p align=\"center\">Tension and Shear Strengths</p><table border=\"1\"><tr><th>Tension T to Grain MTS (PSI)</th><th>Shear || to Grain MSS (PSI)</th></tr><tr><td>%1</td><td>%2</td></tr></table>" ).
            arg( material->tenStrengthMTSPerp() ).
            arg( material->shearStrengthMSSPar() );

    QString bend_table;
    bend_table = tr( "<p align=\"center\">Static Bending Strengths</p><table border=\"1\"><tr><th>FSPL (PSI)</th><th>MR (PSI)</th><th>E (10E6 PSI)</th></tr><tr><td>%1</td><td>%2</td><td>%3</td></tr></table>" ).
            arg( material->bendFSPL() ).
            arg( material->bendMR() ).
            arg( material->bendE() );

    QString annotations = tr( "<p>Notes:<br><font size=\"-1\">FSPL - fiber stress at proportional limit<br>MCS - maximum crushing strength<br>MTS - maximum tensile strength<br>MSS - maximum shear strength<br>MR - modulus of rupture<br>E - modulus of elasticity</font></p>" );

    material_text += compression_table + tension_shear_table + bend_table + annotations;

    ui.MaterialText->setText( material_text );

    //TODO
    //SolidColor->setPaletteBackgroundColor( material->color() );

    ui.FaceGrainPixmap->setPixmap( lC::lookupPixmap( material->faceGrainFile() ) );
    ui.EndGrainPixmap->setPixmap( lC::lookupPixmap( material->endGrainFile() ) );
    ui.EdgeGrainPixmap->setPixmap( lC::lookupPixmap( material->edgeGrainFile() ) );
}


void MaterialDialog::setMaterial( const Material* material )
{
    if ( material == 0 ) {
        ui.MaterialList->clearSelection(); // In single selection mode, the signal is not emitted.
        MaterialList_selectionChanged( 0 );
    }
    else {
        QListWidgetItem* class_item = 0;
        for (int cRow = 0; cRow<ui.MaterialList->count(); cRow++) {
            class_item = ui.MaterialList->item( cRow );
            if ( class_item->text( ) == material->materialClass() )
                break;
        }
        //TODO
        //        if ( class_item != 0 ) {
        //            QListWidgetItem* material_item = class_item->firstChild();
        //            while ( material_item != 0 ) {
        //                if ( material_item->text( ) == material->name() ) {
        //                    MaterialList->setSelected( material_item, true );
        //                    return;
        //                }
        //                material_item = material_item->nextSibling();
        //            }
        //        }
        MaterialList_selectionChanged( 0 );
    }
}


void MaterialDialog::buttonHelp_clicked()
{
    QWhatsThis::showText( QCursor::pos(), tr( "<p><b>Set Material for Part</b><p>\
                                              <p>This dialog allows you to apply a material to the current part. Materials are \
                                              grouped by type (e.g., solid wood, composites, and so on). Select a material from the \
                                              list and some attributes of the material will be displayed in the Material Details box. \
                                              Click the <b>OK</b> button (or press <b>Enter</b> or <b>Alt+O</b>) to apply the material \
                                              to the part. If you click the \
                                              <b>Cancel</b> button (or press <b>Escape</b> or <b>Alt+C</b>), the material currently \
                                              assigned to the part will not be changed.</p>" ) );
}

AssemblyConfigDialog::AssemblyConfigDialog( QWidget* parent )
    :QDialog( parent )
{
    ui.setupUi( this );
    init();
}

Ui::AssemblyConfigDialog* AssemblyConfigDialog::getUi ()
{
    return &ui;
}


void AssemblyConfigDialog::init()
{
    //TODO
    //ui.modelListView->setSorting( -1 );
}

void AssemblyConfigDialog::modelListView_selectionChanged( ListViewItem* item )
{
    if ( item != 0 )
        ui.buttonOk->setEnabled( true );
    else
        ui.buttonOk->setEnabled( false );
}


void AssemblyConfigDialog::buttonHelp_clicked()
{
    QWhatsThis::showText( QCursor::pos(), tr( "<p>Set the name for this assembly and then select the model (either a Part or an Assembly) to form the basis for the assembly. Its origin will coincide with the origin of the assembly.</p>\
                                              <p>Note that the dialog cannot be completed until a model is selected. Canceling the dialog (either by clicking the <b>Cancel</b> button or pressing <b>ESC</b> or <b>Alt+C</b>) will cancel the creation of the assembly as well.</p>" ) );
}



AssemblyAddDialog::AssemblyAddDialog( QWidget* parent )
    :QDialog ( parent )
{
    ui.setupUi( this );
}

Ui::AssemblyAddDialog* AssemblyAddDialog::getUi()
{
    return &ui;
}

void AssemblyAddDialog::modelListView_selectionChanged( ListViewItem* item )
{
    if ( item != 0 )
        ui.buttonOk->setEnabled( true );
    else
        ui.buttonOk->setEnabled( false );
}


void AssemblyAddDialog::buttonHelp_clicked()
{
    QWhatsThis::showText( QCursor::pos(), tr( "Select the model (either a Part or an Assembly) to add to this assembly." ) );
}



AssemblyConstraintForm::AssemblyConstraintForm( QWidget* parent )
    :QWidget ( parent )
{
    ui.setupUi( this );
}

Ui::AssemblyConstraintForm* AssemblyConstraintForm::getUi()
{
    return &ui;
}

void AssemblyConstraintForm::helpPushButton_clicked()
{
    QWhatsThis::showText( QCursor::pos(), tr( "<p>This dialog allows you to define the constraints by which a new model is located in space with respect to the assembly. A typical example would be the location of a table leg with respect to the skirts of the table.</p>\
                                              <p>Defining the model location usually involves making three constraints. (It might help to think of the three constraints corresponding to the normal three spatial dimensions.) The constraints which can be made are:</p>\
                                              <ul><li>Mate: The surfaces of two different models will touch and will face each other.</li>\
                                              <li>Align: The surfaces of two different models will touch and face in the same direction.</li>\
                                              <li>Mate Offset: The surfaces of two different models will face each other but will be separated by a given distance.</li>\
                                              <li>Align Offset: The surfaces of two different models will face in the same direction but will be separated by a given distance</li></ul>\
                                              <p>To create a constraint, select its button. When you click on it, the button will stay in the down position to indicate the type of constraint in progress. Next click on two surfaces: one on the new model and one on the completed assembly. If the two surfaces are compatible, the constraint will be recorded and the position of the new model will be updated to reflect the constraint.</p>\
                                              <p>When the new model is completely constrained, usually after making three constraints, this dialog will disappear. If you decide not to place the new model, click <b>Cancel</b> (or select <b>Cancel Model Addition</b> from the context menu or type <b>Alt-C</b>) and the new model will be removed from the assembly.</p>" ) );
}


OffsetInfoDialog::OffsetInfoDialog( QWidget* parent)
    :QDialog ( parent )
{
    ui.setupUi( this );
}

Ui::OffsetInfoDialog* OffsetInfoDialog::getUi()
{
    return &ui;
}

void OffsetInfoDialog::buttonHelp_clicked()
{
    QWhatsThis::showText( QCursor::pos(), tr("<p><b>Offset Information</b></p>\
                                             <p>This dialog allows you to edit the magnitude of an \
                                             offset. You can either specify a fixed size or you can reference \
                                             another dimension in the model.</p>\
                                             <p>After the offset is adjusted as desired, click on the \
                                             <b>OK</b> button (or press <b>Enter</b> or <b>Alt+O</b>) \
                                             to accept your changes. Clicking the <b>Cancel</b> button \
                                             (or pressing <b>ESC</b> or <b>Alt+C</b>) will exit the \
                                             dialog without applying the changes.</p>" ) );
}



ReferenceLineInfoDialog::ReferenceLineInfoDialog( QWidget* parent )
    :QDialog ( parent )
{
    ui.setupUi( this );
}

Ui::ReferenceLineInfoDialog* ReferenceLineInfoDialog::getUi()
{
    return &ui;
}

void ReferenceLineInfoDialog::buttonHelp_clicked()
{
    QWhatsThis::showText( QCursor::pos(), tr("<p><b>Reference Line Information</b></p>\
                                             <p>This dialog allows you to edit selected properties of a \
                                             reference line. These include:\
                                             <ul>\
                                             <li>Name</li>\
                                             <li>Offset from its reference</li>\
                                             </ul>\
                                             </p>\
                                             <p>After the properties are adjusted as desired, click on the \
                                             <b>OK</b> button (or press <b>Enter</b> or <b>Alt+O</b>) \
                                             to accept your changes. Clicking the <b>Cancel</b> button \
                                             (or pressing <b>ESC</b> or <b>Alt+C</b>) will exit the \
                                             dialog without applying the changes.</p>" ) );
}



CenterlineInfoDialog::CenterlineInfoDialog( QWidget* parent )
    :QDialog ( parent )
{
    ui.setupUi( this );
}

Ui::CenterlineInfoDialog* CenterlineInfoDialog::getUi()
{
    return &ui;
}

void CenterlineInfoDialog::buttonHelp_clicked()
{
    QWhatsThis::showText( QCursor::pos(), tr("<p><b>Centerline Information</b></p>\
                                             <p>This dialog allows you to edit selected properties of a \
                                             centerline. These include:\
                                             <ul>\
                                             <li>Name</li>\
                                             <li>Offset from its reference</li>\
                                             </ul></p>\
                                             <p>After the properties are adjusted as desired, click on the \
                                             <b>OK</b> button (or press <b>Enter</b> or <b>Alt+O</b>) \
                                             to accept your changes. Clicking the <b>Cancel</b> button \
                                             (or pressing <b>ESC</b> or <b>Alt+C</b>) will exit the \
                                             dialog without applying the changes.</p>" ) );
}



DimensionInfoDialog::DimensionInfoDialog( QWidget* parent )
    : QDialog ( parent )
{
    ui.setupUi( this );
}

Ui::DimensionInfoDialog* DimensionInfoDialog::getUi()
{
    return &ui;
}

void DimensionInfoDialog::buttonHelp_clicked()
{
    QWhatsThis::showText( QCursor::pos(), tr("<p><b>Dimension Information</b></p>\
                                             <p>This dialog allows you to edit selected properties of a \
                                             dimension. These include:\
                                             <ul>\
                                             <li>Name</li>\
                                             <li>Offset from its reference</li>\
                                             <li>The side of the references on which the dimension is drawn (annotation side)</li>\
                                             </ul>\
                                             </p>\
                                             <p>After the properties are adjusted as desired, click on the \
                                             <b>OK</b> button (or press <b>Enter</b> or <b>Alt+O</b>) \
                                             to accept your changes. Clicking the <b>Cancel</b> button \
                                             (or pressing <b>ESC</b> or <b>Alt+C</b>) will exit the \
                                             dialog without applying the changes.</p>" ) );
}



RectangleInfoDialog::RectangleInfoDialog( QWidget* parent )
    :QDialog ( parent )
{
    ui.setupUi( this );
}

Ui::RectangleInfoDialog* RectangleInfoDialog::getUi()
{
    return &ui;
}

void RectangleInfoDialog::init ()
{
    // Create invisible button groups connecting the Left/Right, Above/Below
    // radio buttons, respectively.
    QButtonGroup* h = new QButtonGroup( ui.dimDisplaySideGroupBox );
    h->setObjectName( "H" );
    h->addButton( ui.leftButton );
    h->addButton( ui.rightButton );
    QButtonGroup* v = new QButtonGroup( ui.dimDisplaySideGroupBox );
    v->setObjectName(  "V" );
    v->addButton( ui.aboveButton );
    v->addButton( ui.belowButton );

    // Populate the edge stipple combo box. Maybe make a Designer widget?
    ui.edgeStyleComboBox->insertItem( lC::lookupPixmap( "edge_solid.png" ), tr( "Solid" ) );
    ui.edgeStyleComboBox->insertItem( lC::lookupPixmap( "edge_dash.png" ), tr( "Dash" ) );
    ui.edgeStyleComboBox->insertItem( lC::lookupPixmap( "edge_dot.png" ), tr( "Dot" ) );
    ui.edgeStyleComboBox->insertItem( lC::lookupPixmap( "edge_dashdot.png"), tr( "Dash Dot" ) );
    ui.edgeStyleComboBox->insertItem( lC::lookupPixmap( "edge_dashdotdot.png"), tr( "Dash Dot Dot" ) );
}

void RectangleInfoDialog::rectangleStyleSlider_valueChanged( int style )
{
    switch ( style ) {
    case lC::Render::PARENT:
        ui.defaultStyleLabel->setEnabled( true );
        ui.edgeStyleLabel->setEnabled( false );
        ui.edgeStyleComboBox->setEnabled( false );
        ui.wireframeStyleLabel->setEnabled( false );
        ui.wireframeStyleColorChooser->setEnabled( false );
        ui.solidStyleLabel->setEnabled( false );
        ui.solidStyleColorChooser->setEnabled( false );
        ui.textureStyleLabel->setEnabled( false );
        ui.textureStyleFileChooser->setEnabled( false );
        break;
    case lC::Render::STIPPLE:
        ui.defaultStyleLabel->setEnabled( false );
        ui.edgeStyleLabel->setEnabled( true );
        ui.edgeStyleComboBox->setEnabled( true );
        ui.wireframeStyleLabel->setEnabled( false );
        ui.wireframeStyleColorChooser->setEnabled( false );
        ui.solidStyleLabel->setEnabled( false );
        ui.solidStyleColorChooser->setEnabled( false );
        ui.textureStyleLabel->setEnabled( false );
        ui.textureStyleFileChooser->setEnabled( false );
        break;
    case lC::Render::WIREFRAME:
        ui.defaultStyleLabel->setEnabled( false );
        ui.edgeStyleLabel->setEnabled( true );
        ui.edgeStyleComboBox->setEnabled( true );
        ui.wireframeStyleLabel->setEnabled( true );
        ui.wireframeStyleColorChooser->setEnabled( true );
        ui.solidStyleLabel->setEnabled( false );
        ui.solidStyleColorChooser->setEnabled( false );
        ui.textureStyleLabel->setEnabled( false );
        ui.textureStyleFileChooser->setEnabled( false );
        break;
    case lC::Render::SOLID:
        ui.defaultStyleLabel->setEnabled( false );
        ui.edgeStyleLabel->setEnabled( true );
        ui.edgeStyleComboBox->setEnabled( true );
        ui.wireframeStyleLabel->setEnabled( true );
        ui.wireframeStyleColorChooser->setEnabled( true );
        ui.solidStyleLabel->setEnabled( true );
        ui.solidStyleColorChooser->setEnabled( true );
        ui.textureStyleLabel->setEnabled( false );
        ui.textureStyleFileChooser->setEnabled( false );
        break;
    case lC::Render::TEXTURED:
        ui.defaultStyleLabel->setEnabled( false );
        ui.edgeStyleLabel->setEnabled( true );
        ui.edgeStyleComboBox->setEnabled( true );
        ui.wireframeStyleLabel->setEnabled( true );
        ui.wireframeStyleColorChooser->setEnabled( true );
        ui.solidStyleLabel->setEnabled( true );
        ui.solidStyleColorChooser->setEnabled( true );
        ui.textureStyleLabel->setEnabled( true );
        ui.textureStyleFileChooser->setEnabled( true );
        break;
    }
}


void RectangleInfoDialog::buttonHelp_clicked()
{
    QWhatsThis::showText( QCursor::pos(), tr("<p><b>Rectangle Information</b></p>\
                                             <p>This dialog allows you to edit selected properties of a \
                                             rectangle. These include:\
                                             <ul>\
                                             <li>Name</li>\
                                             <li>Width and height</li>\
                                             <li>X and Y origin offsets</li>\
                                             <li>The side on which the width and height dimensions are displayed</li>\
                                             <li>The rendering style</li>\
                                             </ul>\
                                             </p>\
                                             <p>After the properties are adjusted as desired, click on the \
                                             <b>OK</b> button (or press <b>Enter</b> or <b>Alt+O</b>) \
                                             to accept your changes. Clicking the <b>Cancel</b> button \
                                             (or pressing <b>ESC</b> or <b>Alt+C</b>) will exit the \
                                             dialog without applying the changes.</p>" ) );
}



AnnotationInfoDialog::AnnotationInfoDialog( QWidget* parent )
    :QDialog ( parent )
{
    ui.setupUi( this );
}

Ui::AnnotationInfoDialog* AnnotationInfoDialog::getUi()
{
    return &ui;
}

/*!
 * Initialize the Annotation information dialog.
 */
void AnnotationInfoDialog::init()
{
    int width = ui.boldButton->sizeHint().width();
    width = qMax( width, ui.boldButton->sizeHint().height() );
    width = qMax( width, ui.italicButton->sizeHint().width() );
    width = qMax( width, ui.italicButton->sizeHint().height() );
    width = qMax( width, ui.underlineButton->sizeHint().width() );
    width = qMax( width, ui.underlineButton->sizeHint().height() );

    ui.boldButton->setFixedSize( width, width );
    ui.italicButton->setFixedSize( width, width );
    ui.underlineButton->setFixedSize( width, width );

    QFont font;
    if ( !OpenGLGlobals::instance()->annotationFont().isEmpty() )
        font.fromString( OpenGLGlobals::instance()->annotationFont() );

    ui.annotationTextEdit->setFont( font );

    QList<int> sizes = QFontDatabase::standardSizes();
    QList<int>::Iterator it = sizes.begin();
    for ( ; it != sizes.end(); ++it )
        ui.sizesComboBox->addItem( QString::number( *it ) );

    QPixmap sample( 24, 24 );
    //TODO
    //sample.fill( ui.annotationTextEdit->colorGroup().text() );
    QIcon buttonIcon(sample);
    ui.colorButton->setIcon( buttonIcon );
    ui.colorButton->setIconSize( sample.rect().size() );
}

/*!
 * Set the bold attribute of the selected text. Also makes this the current
 * state of the bold  attribute.
 * \param bold new bold attribute.
 */
void AnnotationInfoDialog::boldButton_toggled( bool bold )
{
    QFont textFont = ui.annotationTextEdit->font();
    textFont.setBold( bold );
    ui.annotationTextEdit->setFont( textFont );
}

/*!
 * Set the italic attribute of the selected text. Also makes this the current
 * state of the italic attribute.
 * \param italic new italic attribute.
 */
void AnnotationInfoDialog::italicButton_toggled( bool italic )
{
    ui.annotationTextEdit->setFontItalic( italic );
}

/*!
 * Set the underline attribute of the selected text. Also makes this the
 * current state of the underline attribute.
 * \param underline new underline attribute.
 */
void AnnotationInfoDialog::underlineButton_toggled( bool underline )
{
    ui.annotationTextEdit->setFontUnderline( underline );
}

/*!
 * Set the point size of the selected text. Also makes this the current
 * state of the font size.
 * \param value new point size.
 */
void AnnotationInfoDialog::pointSizeSpinBox_valueChanged( int value )
{
    ui.annotationTextEdit->setFontPointSize(value);
}

/*!
 * Set the color of the selected text. Runs the standard Qt color selector.
 * Also makes this the current state of the color attribute.
 */
void AnnotationInfoDialog::colorButton_clicked()
{
    QColor color = QColorDialog::getColor( ui.annotationTextEdit->textColor(), this );
    ui.annotationTextEdit->setTextColor( color );
    QPixmap sample( 24, 24);
    sample.fill( color );
    QIcon buttonIcon(sample);
    ui.colorButton->setIcon( buttonIcon );
    ui.colorButton->setIconSize( sample.rect().size() );
}

/*!
 * This is invoked when the text edit cursor enters a region of text
 * with a different color from the current color.
 * \param color current color at cursor.
 */
void AnnotationInfoDialog::annotationTextEdit_currentColorChanged( const QColor & color )
{
    QPixmap sample( 24, 24 );
    sample.fill( color );
    QIcon buttonIcon(sample);
    ui.colorButton->setIcon ( buttonIcon );
    ui.colorButton->setIconSize( sample.rect().size() );
}


/*!
 * This is invoked when the text edit cursor enters a region of
 * different attributes from the current attributes.
 * \param font current font at cursor.
 */
void AnnotationInfoDialog::annotationTextEdit_currentFontChanged( const QFont & font )
{
    ui.boldButton->setEnabled( font.bold() );
    ui.italicButton->setEnabled( font.italic() );
    ui.underlineButton->setEnabled( font.underline() );
    ui.sizesComboBox->setCurrentText( QString::number( font.pointSize() ) );
}

/*!
 * This is invoked when the user changes the font point size.
 * \param text point size text from combo box.
 */
void AnnotationInfoDialog::sizesComboBox_activated( const QString & text )
{
    ui.annotationTextEdit->setFontPointSize( text.toInt() );
}

/*!
 * Display brief help message for annotation text editor.
 */
void AnnotationInfoDialog::buttonHelp_clicked()
{
    QWhatsThis::showText( QCursor::pos(), tr("<p><b>Annotation Information</b></p>\
                                             <p>This dialog allows you to edit selected properties of a \
                                             text annotation. These include:\
                                             <ul>\
                                             <li>Name</li>\
                                             <li>Text (including font size, color and style)</li>\
                                             </ul></p>\
                                             <p>After the properties are adjusted as desired, click on the \
                                             <b>OK</b> button (or press <b>Enter</b> or <b>Alt+O</b>) \
                                             to accept your changes. Clicking the <b>Cancel</b> button \
                                             (or pressing <b>ESC</b> or <b>Alt+C</b>) will exit the \
                                             dialog without applying the changes.</p>" ) );

}




PartInfoDialog::PartInfoDialog( QWidget* parent )
    :QDialog ( parent )
{
    ui.setupUi( this );
}

Ui::PartInfoDialog* PartInfoDialog::getUi()
{
    return &ui;
}

void PartInfoDialog::buttonHelp_clicked()
{
    QWhatsThis::showText( QCursor::pos(), tr( "<p><b>Part Information</b></p>\
                                              <p>This dialog lets you modify the overall attributes of a Part. Currently, \
                                              the only attribute you can change is the name of the Part.</p>\
                                              <p>Click the <b>OK</b> button \
                                              (or press <b>Enter</b> or <b>Alt+O</b>) to apply the change. \
                                              If you enter the same name as an existing Part, \
                                              <i>lignumCAD</i> will reject the change.</p>\
                                              <p>You can cancel these changes by clicking the <b>Cancel</b> \
                                              button (or by pressing <b>ESC</b> or <b>Alt+C</b>).</p>" ) );
}




ParameterInfoDialog::ParameterInfoDialog( QWidget* parent )
    :QDialog ( parent )
{
    ui.setupUi( this );
}

Ui::ParameterInfoDialog* ParameterInfoDialog::getUi()
{
    return &ui;
}

void ParameterInfoDialog::buttonHelp_clicked()
{
    QWhatsThis::showText( QCursor::pos(), tr("<p><b>Parameter Information</b></p>\
                                             <p>This dialog allows you to edit the magnitude of a \
                                             parameter. You can either specify a fixed size or you can reference \
                                             another dimension in the model.</p>\
                                             <p>After the parameter is adjusted as desired, click on the \
                                             <b>OK</b> button (or press <b>Enter</b> or <b>Alt+O</b>) \
                                             to accept your changes. Clicking the <b>Cancel</b> button \
                                             (or pressing <b>ESC</b> or <b>Alt+C</b>) will exit the \
                                             dialog without applying the changes.</p>" ) );
}
