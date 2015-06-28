/* -*- c++ -*-
 * lignumcadmainwindow.ui.h
 *
 * Header for the lignumCADMainWindow classes
 * Copyright (C) 2002 lignum Computing, Inc. <lignumcad@lignumcomputing.com>
 * $Id$
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */
/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename slots use Qt Designer which will
** update this file, preserving your code. Create an init() slot in place of
** a constructor, and a destroy() slot in place of a destructor.
*****************************************************************************/

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
    titleLabel->setText( tr( "\n%1 Version %2.%3\nCopyright © %4 %5\n%6\ne-mail: %7" ).
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
    
    message_label_ = new QLabel(  tr( "lignumCAD © 2002 lignum Computing, Inc."), statusBar(), "messageLabel" );
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
    
    model_hierarchy_list_ = new QListView( model_hierarchy_view_, "modelHierarchyList" );
    model_hierarchy_list_->setRootIsDecorated( true );
    model_hierarchy_list_->addColumn( tr( "Name" ) );
    model_hierarchy_list_->addColumn( tr( "Type" ) );
    model_hierarchy_list_->addColumn( tr( "Detail" ) );
    model_hierarchy_list_->setSorting( -1 );
    
    model_hierarchy_view_->boxLayout()->addWidget( model_hierarchy_list_ );
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

QListView* lignumCADMainWindow::modelHierarchyList()
{
    return model_hierarchy_list_;
}

void lignumCADMainWindow::showView()
{
    showView( 0 );
}


void lignumCADMainWindow::setHomeDir( QDir & home_dir )
{
    home_dir_ = home_dir;
}
