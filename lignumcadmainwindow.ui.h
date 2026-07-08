/* -*- c++ -*-
 * lignumcadmainwindow.ui.h
 *
 * Header for the lignumCADMainWindow classes
 * Copyright (C) 2002 lignum Computing, Inc. <lignumcad@lignumcomputing.com>
 * Copyright (C) 2026 Marius Cirsta <mforce2@gmail.com>
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

#include <QByteArray>
#include <QCoreApplication>
#include <QDockWidget>
#include <QFileDialog>
#include <QMessageBox>
#include <QPushButton>
#include <QSettings>
#include <QStatusBar>
#include <QStyle>
#include <QStringList>

#include "listviewitem.h"

aboutDialog* lignumCADMainWindow::about_dialog_ = 0;

namespace {
  constexpr int DOCK_LAYOUT_VERSION = 1;
}

QString trMainWindowConstant ( const QString& text )
{
    return QCoreApplication::translate( "Constants", text.toUtf8().constData() );
}

void lignumCADMainWindow::init ()
{
    if ( whatsThisAction->icon().isNull() )
	whatsThisAction->setIcon( style()->standardIcon( QStyle::SP_TitleBarContextHelpButton ) );
    
    message_label_ = new QLabel(  tr( "lignumCAD © 2002 lignum Computing, Inc."), statusBar() );
    message_label_->setObjectName( "messageLabel" );
    message_label_->setWhatsThis( tr( "Lists program status and other messages." ) );
    statusBar()->addWidget( message_label_, 1 );
    
    information_label_ = new QLabel( tr( "none" ), statusBar() );
    information_label_->setObjectName( "informationLabel" );
    information_label_->setWhatsThis( tr( "Shows the status of the current user interface action."));
    statusBar()->addPermanentWidget( information_label_, 0 );

    page_label_ = new QLabel( tr( "Page: %1" ).arg( "none" ), statusBar() );
    page_label_->setObjectName( "pageLabel" );
    page_label_->setWhatsThis( tr( "Shows the name of the current page." ) );
    statusBar()->addPermanentWidget( page_label_, 0 );

    scale_label_ = new QLabel( tr( "Scale: 1 : 1" ), statusBar() );
    scale_label_->setObjectName( "scaleLabel" );
    scale_label_->setWhatsThis( tr( "Shows the scale of the current page." ) );
    statusBar()->addPermanentWidget( scale_label_, 0 );
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
  settings.setValue( lC::Setting::DOCK_GEOMETRIES,
		     QString::fromLatin1( saveState( DOCK_LAYOUT_VERSION ).toBase64() ) );

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
	    QMessageBox mb( QMessageBox::Information,
			    trMainWindowConstant( lC::STR::LIGNUMCAD ),
			    tr( "The file \"%1\" does not exist." ).arg( new_file_name ),
			    QMessageBox::NoButton,
			    this );
	    QPushButton* select_file_button =
		mb.addButton( tr( "Select another file" ), QMessageBox::AcceptRole );
	    QPushButton* create_model_button =
		mb.addButton( tr( "Create a new model" ), QMessageBox::NoRole );
	    QPushButton* exit_button =
		mb.addButton( tr( "Exit lignumCAD" ), QMessageBox::RejectRole );
	    mb.setDefaultButton( select_file_button );
	    mb.exec();

	    if ( mb.clickedButton() == select_file_button ) {
		new_file_name =
		    QFileDialog::getOpenFileName( this,
						  tr( "Choose a file" ),
						  QString(),
						  tr( "lignumCAD (*.lcad);;All Files (*)" ) );
	    }
	    else if ( mb.clickedButton() == create_model_button )
		new_file_name = QString();
	    else if ( mb.clickedButton() == exit_button )
		exit(0 );
	}
    }    
    
    model_hierarchy_view_ = new QDockWidget( tr( "Model Hierarchy" ), this );
    model_hierarchy_view_->setObjectName( "modelHierarchyView" );
    
    model_hierarchy_list_ = new ModelHierarchyTreeWidget( model_hierarchy_view_ );
    model_hierarchy_list_->setObjectName( "modelHierarchyList" );
    model_hierarchy_list_->setRootIsDecorated( true );
    model_hierarchy_list_->setHeaderLabels( QStringList()
					      << tr( "Name" )
					      << tr( "Type" )
					      << tr( "Detail" ) );
    model_hierarchy_list_->setSortingEnabled( false );
    model_hierarchy_list_->setWhatsThis(
		     tr( "<p><b>Model Hierarchy</b></p>\
<p>Shows a list of the pages and figures which compose \
the model.</p><p>You can rename some items by click-pause-clicking on the \
name field. Also note that you have to press <b>Enter</b> after typing in \
the name in order for the name change to be detected.</p>" ) );
    model_hierarchy_view_->setWidget( model_hierarchy_list_ );

    if ( !new_file_name.isEmpty() )
	design_book_view_ = new DesignBookView( this, new_file_name );
    else
	design_book_view_ = new DesignBookView( this );

    addDockWidget( Qt::BottomDockWidgetArea, model_hierarchy_view_ );

    // See if the user has already set up a layout that they like.
    // NOTE: THE Qt CODE FOR THIS EXPECTS THE DOCKWINDOW TO HAVE
    // ALREADY BEEN CREATED! IT EXPECTS THE CAPTION NAME TO BE THE
    // SAME AS WHAT'S IN THE SETTINGS FILE.
    QSettings settings;
    QString buffer = settings.value( lC::Setting::DOCK_GEOMETRIES ).toString();
    if ( !buffer.isEmpty() )
	restoreState( QByteArray::fromBase64( buffer.toLatin1() ),
		      DOCK_LAYOUT_VERSION );
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

ModelHierarchyTreeWidget* lignumCADMainWindow::modelHierarchyList()
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
