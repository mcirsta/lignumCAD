/*
 * designbookview.cpp
 *
 * DesignBookView class: the primary user interface.
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
#include <qapplication.h>
#include <qmainwindow.h>
#include <qaction.h>
#include <QMenu>
#include <qlineedit.h>
#include <qfiledialog.h>
#include <qmessagebox.h>
#include <qspinbox.h>
#include <qlineedit.h>
#include <qtextedit.h>
#include <qlabel.h>
#include <qradiobutton.h>
#include <qpushbutton.h>
#include <qbuttongroup.h>
#include <qstatusbar.h>
#include <qimage.h>
#include <qsettings.h>
#include <qregexp.h>
#include <qdom.h>
#include <qpainter.h>
#include <qpicture.h>
#include <qslider.h>
#include <qlayout.h>

#include <QDesktopWidget>


#include "configuration.h"
#include "openglview.h"
#include "openglexample.h"
#include "openglprinter.h"
#include "tabbarcontext.h"
#include "lignumcadmainwindow.h"
#include "pagefactory.h"
#include "pageview.h"
#include "model.h"
#include "lcfilechooser.h"
#include "lcfontchooser.h"
#include "lccolorchooser.h"
#include "lcdefaultfilechooser.h"
#include "lcdefaultlengthspinbox.h"
#include "lcdefaultratiospinbox.h"
#include "lcdefaultcombobox.h"
#include "listviewitem.h"
#include "command.h"
#include "printer.h"
#include "units.h"
#include "businessinfo.h"
#include "usersettings.h"
#include "designbookview.h"

#include "lcdebug.h"

//#define LAYOUT_COMPREHENSION

PreferencesDialog* DesignBookView::preferences_dialog_ = 0;
OpenGLExample* DesignBookView::preferences_example_ = 0;
Printer* DesignBookView::printer_ = 0;
OpenGLPrinter* DesignBookView::opengl_printer_ = 0;


// The parent design book view connects the create action to this slot.

void CreatePage::createPage ( void )
{
  DesignBookView* view = dynamic_cast<DesignBookView*>( parent() );

  PageView* page_view = PageFactory::instance()->create( id_, view );

  if ( !page_view->configure() ) {
    view->deletePage( page_view );
    return;
  }

  view->modelChanged( true );

  CommandHistory::instance().
    addCommand( new CreateCommand( QString( "create %1").arg( page_view->type() ),
				   page_view->memento() ) );

  view->showPageView( page_view );
}

// Should this class be a singleton? Are we ever going to have a
// MDI version of lignumCAD?

DesignBookView::DesignBookView ( lignumCADMainWindow* lCMW )
  : QWidget( lCMW ), lCMW_( lCMW ), gui_visible_( false ),
    model_( 0 ), printing_( false )
{
  setObjectName("designbookview");
  init();

  // Can't really display the OpenGL and Tabbar views until there is
  // something to display. (In particular, unless QTabbar has some
  // child Tabs, it won't have a height!). So, make the user do
  // something first.

  Model* model = new Model ( tr( "unnamed" ), QString::null );

  uint initial_page_id;

  if ( ! newModelWizard( model, initial_page_id ) ) {
    delete model;
    return;
  }

  model_ = model;

  PageView* page_view = PageFactory::instance()->create( initial_page_id, this);

  // I don't know. For my sanity's sake, creating the initial page is
  // not sufficient to flag the model as modified. However, the page configure
  // method may consider differently.
  model_->setChanged( false );

  // Give the user a chance to configure the page and possibly decide against
  // creating it after all.
  if ( !page_view->configure() ) {
    deletePage( page_view );
    showView();
    return;
  }

  CommandHistory::instance().
    addCommand( new CreateCommand( QString( "create %1" ).arg( page_view->type() ),
				   page_view->memento() ) );

  page_tabbar_->setCurrentIndex(page_view->tabIdx() );
  page_tabbar_->update();
  page_view->show();
  opengl_view_->setPageView( page_view );

  showView();
}

DesignBookView::DesignBookView ( lignumCADMainWindow* lCMW, const QString file_name )
  : QWidget( lCMW ), lCMW_( lCMW ), gui_visible_( false ),
    model_( 0 ), printing_( false )
{
  setObjectName( "designbookview" );
  init();

  if ( !read( file_name ) ) return;

  if ( model_ == 0 ) return;

  showView();
}

DesignBookView::~DesignBookView ()
{
  page_views_.clear();		// Must be empty before destroying model
  if ( model_ != 0 ) delete model_;
  delete opengl_view_;
}

// Handle the common activities of constructing a Design Book.

void DesignBookView::init ( void )
{
  app_palette_ = qApp->desktop()->palette();

  // Recover the user's default settings.
  QSettings settings;
  bool ok;

  Ui::lignumCADMainWindow* lCMW_Ui = lCMW_->getUi();

 //TODO check if value exist, otherwise write


  QString business_name =
    settings.value( lC::Setting::Business::NAME ).toString();

  if ( ok )
    BusinessInfo::instance().setName( business_name );

  QString business_location =
    settings.value( lC::Setting::Business::LOCATION ).toString();

  if ( ok )
    BusinessInfo::instance().setLocation( business_location );

  QString business_logo =
    settings.value( lC::Setting::Business::LOGO ).toString();

  if ( ok )
    BusinessInfo::instance().setLogo( business_logo );

  QStringList length_unit =
    settings.value ( lC::Setting::LENGTH_UNIT, ',').toStringList();

  if ( ok )
    UnitsBasis::instance()->setLengthUnit( length_unit );

  QString scheme =
    settings.value ( lC::Setting::ColorScheme::BASE ).toString();

  if ( !scheme.isEmpty() )
    OpenGLGlobals::instance()->setDefaultScheme( scheme );

  bool custom =
    settings.value( lC::Setting::ColorScheme::CUSTOM, false ).toBool();

  if ( ok ) {
    if ( !custom ) {
      OpenGLGlobals::instance()->setPredefinedScheme( true );
      OpenGLGlobals::instance()->setScheme( OpenGLGlobals::instance()->at() );
    }
    else {
      OpenGLGlobals::instance()->setPredefinedScheme( false );

      QString color_name;

      color_name = settings.
    value( lC::Setting::ColorScheme::GEOMETRY ).toString();

      if ( ok ) {
	QColor color( color_name );
	OpenGLGlobals::instance()->setGeometryColor( color );
      }

      color_name = settings.
    value( lC::Setting::ColorScheme::ANNOTATION ).toString();

      if ( ok ) {
	QColor color( color_name );
	OpenGLGlobals::instance()->setAnnotationColor( color );
      }

      color_name = settings.
    value( lC::Setting::ColorScheme::GRID ).toString();

      if ( ok ) {
	QColor color( color_name );
	OpenGLGlobals::instance()->setGridColor( color );
      }

      color_name = settings.
    value( lC::Setting::ColorScheme::CONSTRAINT_PRIMARY ).toString();

      if ( ok ) {
	QColor color( color_name );
	OpenGLGlobals::instance()->setConstraintPrimaryColor( color );
      }

      color_name = settings.
    value(lC::Setting::ColorScheme::CONSTRAINT_SECONDARY ).toString();

      if ( ok ) {
	QColor color( color_name );
	OpenGLGlobals::instance()->setConstraintSecondaryColor( color );
      }

      QString style = settings.
        value( lC::Setting::ColorScheme::BACKGROUND_STYLE ).toString();

      if ( ok )
	OpenGLGlobals::instance()->
	  setBackgroundStyle( lC::Background::backgroundStyle( style ) );

      color_name = settings.
    value( lC::Setting::ColorScheme::BACKGROUND ).toString();

      if ( ok ) {
	QColor color( color_name );
	OpenGLGlobals::instance()->setBackgroundColor( color );
      }

      color_name = settings.
    value( lC::Setting::ColorScheme::GRADIENT ).toString();

      if ( ok ) {
	QColor color( color_name );
	OpenGLGlobals::instance()->setGradientColor( color );
      }

      QString pattern_file = settings.
    value( lC::Setting::ColorScheme::PATTERN_FILE ).toString();

      if ( ok )
	OpenGLGlobals::instance()->setPatternFile( pattern_file );
    }
  }

  QString dimension_font =
    settings.value( lC::Setting::Dimension::FONT ).toString();

  if ( !dimension_font.isEmpty() )
    OpenGLGlobals::instance()->setDimensionFont( dimension_font );

  double arrow_head_length =
    settings.value( lC::Setting::Dimension::ARROW_HEAD_LENGTH, 0 ).toDouble();

  if ( ok )
    OpenGLGlobals::instance()->setArrowHeadLength( arrow_head_length );

  int arrow_head_width_ratio =
    settings.value( lC::Setting::Dimension::ARROW_HEAD_WIDTH_RATIO, 0 ).toInt();

  if ( ok )
    OpenGLGlobals::instance()->
      setArrowHeadWidthRatio( Ratio( arrow_head_width_ratio ) );

  QString arrow_head_style =
    settings.value( lC::Setting::Dimension::ARROW_HEAD_STYLE ).toString();

  if ( ok )
    OpenGLGlobals::instance()->
      setArrowHeadStyle( lC::arrowHeadStyle( arrow_head_style ) );

  double clearance =
    settings.value( lC::Setting::Dimension::CLEARANCE, 0 ).toDouble();

  if ( ok )
    OpenGLGlobals::instance()->setClearance( clearance );

  double line_thickness =
    settings.value ( lC::Setting::Dimension::LINE_THICKNESS, 0 ).toDouble();

  if ( ok )
    OpenGLGlobals::instance()->setLineThickness( line_thickness );

  double extension_line_offset =
    settings.value ( lC::Setting::Dimension::EXTENSION_LINE_OFFSET, 0).toDouble();

  if ( ok )
    OpenGLGlobals::instance()->setExtensionLineOffset( extension_line_offset );

  QString annotation_font =
    settings.value ( lC::Setting::Annotation::FONT ).toString();

  if ( !annotation_font.isEmpty() )
    OpenGLGlobals::instance()->setAnnotationFont( annotation_font );

  double handle_size =
    settings.value ( lC::Setting::Handle::SIZE, 0 ).toDouble();

  if ( ok )
    OpenGLGlobals::instance()->setHandleSize( handle_size );

  new_model_wizard_ = new NewModelWizard( lCMW_ );

  page_info_dialog_ = new PageInfoDialog( lCMW_ );

  opengl_view_ = new OpenGLView( this, "openGLView", lCMW_, 0 );
  opengl_view_->setFocus();

  page_tabbar_ = new TabBarContext( this, "pageTabBarContext" );
  page_tabbar_->setShape( QTabBar::RoundedSouth );

//#ifndef LAYOUT_COMPREHENSION
//  // Without at least one tab, QTabBar starts with an initial height
//  // of 0, from which the layout never seems to recover. It's probably
//  // something wrong with the specification of QGLWidget's resizing
//  // preferences. Look at this later. For now...
//  page_tabbar_->setMinimumHeight( 32 );
//  page_tabbar_->addTab( new QTab( "###dummy###" ) );
//#endif

  // Add the available pages to the Insert menu and the Tab Bar context
  // menu.

  QVector<uint> page_ids = PageFactory::instance()->pageIDs();
  QVector<uint>::const_iterator id = page_ids.begin();
  // This loop assumes that the page creation menu items are the first
  // items inserted into both the menu bar Insert menu and the tab bar
  // context popup menu. (QAction really needs some more identifying
  // information.)
  for ( int index = 0; id != page_ids.end(); ++id, ++index ) {
    QAction* insertAction = PageFactory::instance()->action( *id, lCMW_ );

    page_tabbar_->contextMenu()->addAction(insertAction);
    lCMW_Ui->insertMenu->addAction ( insertAction );
    lCMW_Ui->insertToolbar->addAction ( insertAction );

    connect( insertAction, SIGNAL( activated() ),
	     new CreatePage( this, *id ), SLOT( createPage() ) );

    insertAction->setEnabled( false );
  }

  page_tabbar_->contextMenu()->addSeparator();
  page_tabbar_->contextMenu()->addAction( lCMW_Ui->renamePageAction );
  page_tabbar_->contextMenu()->addAction( lCMW_Ui->deletePageAction );

  connect( opengl_view_, SIGNAL( scale( const Ratio& ) ),
	   lCMW_, SLOT( scaleChanged( const Ratio& ) ) );

  connect( page_tabbar_, SIGNAL( selected( int ) ), SLOT( pageChanged( int ) ) );

  connect( lCMW_Ui->editPreferencesAction, SIGNAL( activated() ),
	   SLOT( editPreferences() ) );

  connect( lCMW_Ui->renamePageAction, SIGNAL( activated() ), SLOT( renamePage() ) );
  connect( lCMW_Ui->deletePageAction, SIGNAL( activated() ), SLOT( deletePage() ) );

  connect( this, SIGNAL( setCaption( const QString& ) ),
       lCMW_, SLOT( setCaption( const QString& ) ) );
  connect( this, SIGNAL( pageChanged( const QString& ) ),
       lCMW_, SLOT( pageChanged( const QString& ) ) );

  // Disable the actions we can't take.
  lCMW_Ui->fileSaveAction->setEnabled( false );
  lCMW_Ui->fileSaveAsAction->setEnabled( false );
  lCMW_Ui->filePrintAction->setEnabled( false );
  lCMW_Ui->exportPageAction->setEnabled( false );
  lCMW_Ui->modelInfoAction->setEnabled( false );

  lCMW_Ui->editUndoAction->setEnabled( false );
  lCMW_Ui->editRedoAction->setEnabled( false );
  lCMW_Ui->editCutAction->setEnabled( false );
  lCMW_Ui->editCopyAction->setEnabled( false );
  lCMW_Ui->editPasteAction->setEnabled( false );
  lCMW_Ui->editFindAction->setEnabled( false );

  if ( preferences_dialog_ == 0 )
    preferences_dialog_ = new PreferencesDialog( lCMW_ );

  if ( printer_ == 0 )
    printer_ = new Printer;
  // note: need a real printer class...
  if ( opengl_printer_ == 0 )
    opengl_printer_ = new OpenGLPrinter( this, "openGLPrinter", opengl_view_ );

  model_list_item_ = new ListViewItem( lCMW_->modelHierarchyList() );

  connect( &CommandHistory::instance(), SIGNAL( undoRedoChanged(bool, bool ) ),
	   SLOT( updateUndoRedo(bool, bool) ) );

  model_info_dialog_ = new ModelInfoDialog( lCMW_ );
}

/*
 * The name (of the model)
 */
QString DesignBookView::name ( void ) const
{
  return model_->name();
}

// The database ID (of the model).

DBURL DesignBookView::dbURL ( void ) const
{
  return model_->dbURL();
}

// Show the design book GUI.

void DesignBookView::showView ( void )
{
  // Enable the actions we can now take.

  lCMW_->getUi()->fileSaveAction->setEnabled( model_->changed() );
  lCMW_->getUi()->fileSaveAsAction->setEnabled( true );
  lCMW_->getUi()->filePrintAction->setEnabled( true );
  lCMW_->getUi()->exportPageAction->setEnabled( true );
  lCMW_->getUi()->modelInfoAction->setEnabled( true );

  lCMW_->getUi()->editFindAction->setEnabled( true );

  QVector<uint> page_ids = PageFactory::instance()->pageIDs();
  QVector<uint>::const_iterator id = page_ids.begin();
  for ( int index = 0; id != page_ids.end(); ++id, ++index ) {
    QAction* insertAction = PageFactory::instance()->action( *id, lCMW_ );
    insertAction->setEnabled( true );
  }

  emit setCaption( tr( "lignumCAD: %1-%2.%3%4" ).
		   arg( lC::formatName( model_->name() ) ).
		   arg( model_->version() ).
		   arg( model_->revision() ).
       arg( model_->changed() ? tr( "*", "model changed flag" ) : QString::null ) );

  if ( not gui_visible_ ) {
    old_central_widget_ = lCMW_->centralWidget();
    old_central_widget_->hide();
    lCMW_->setCentralWidget( this );
    show();
    gui_visible_ = true;
//#ifndef LAYOUT_COMPREHENSION
//    if ( page_tabbar_->count() > 0 ) {
//      QTab* tab0 = page_tabbar_->tabAt( new QPoint() );
//      if ( tab0 != 0 && tab0->text() == "###dummy###" )
//	page_tabbar_->removeTab( tab0 );
//    }
//#endif
  }

  model_list_item_->setText( lC::NAME, lC::STR::NAME_ID.
			     arg( lC::formatName( model_->name() ) ).
			     arg( model_->id() ) );
  model_list_item_->setText( lC::TYPE, trC( lC::STR::MODEL ) );
  model_list_item_->setOpen( true );
  model_list_item_->setRenameEnabled( lC::NAME, true );

  connect( model_, SIGNAL( nameChanged( const QString& ) ),
	   SLOT( updateName( const QString& ) ) );

  connect( model_list_item_, SIGNAL( nameChanged( const QString& ) ),
	   SLOT( setName( const QString& ) ) );
}

// Hide the design book GUI.

void DesignBookView::hideView ( void )
{
  //  vbox_layout_->hide();
  lCMW_->setCentralWidget( old_central_widget_ );
  old_central_widget_->show();
  gui_visible_ = false;
}

// If the program is about to Exit, make sure the user has saved
// the current model.

bool DesignBookView::aboutToExit ( void )
{
  if ( model_ ) {
    if ( model_->changed() ) {
      QMessageBox mb( trC( lC::STR::LIGNUMCAD ),
		      tr( "The current model has been modified.\n"
			  "Do you really want to exit lignumCAD?" ),
		      QMessageBox::Information,
		      QMessageBox::Yes | QMessageBox::Default,
		      QMessageBox::No,
		      QMessageBox::Cancel, lCMW() );
      mb.setButtonText( QMessageBox::Yes, tr( "Discard changes" ) );
      mb.setButtonText( QMessageBox::No, tr( "Save it first" ) );
      mb.setButtonText( QMessageBox::Cancel, tr( "Cancel exit" ) );

      switch ( mb.exec() ) {
      case QMessageBox::Yes:
	//      clear();
	break;
      case QMessageBox::No:
	if ( !save() ) return false;
	break;
      case QMessageBox::Cancel:
	return false;
      }
    }
    // Release the memory allocated by lignumCAD. Mostly for debugging
    // dynamic memory allocations.
    clear();
  }

  return true;
}

// There can only be one page of each type with the same name.
// Check to make sure this name is unique for the type of page.

bool DesignBookView::uniquePageName ( const QString name ) const
{
  QPtrListIterator< PageView > pv( page_views_ );

  for ( ; pv.current(); ++pv ) {

    if ( (*pv) == page_views_.current() )
      continue;

    else if ( lC::formatName( (*pv)->name() ) == name &&
	      (*pv)->type() == page_views_.current()->type() )
      return false;
  }
  return true;
}

// There can only be one page of each type with the same name.
// Check to make sure this name is unique for the type of page.
// This version does the complaining.

lC::RenameStatus DesignBookView::uniquePageName ( const PageView* page_view,
						  const QString& name,
						  const QString& type )
  const
{
  QPtrListIterator< PageView > pv( page_views_ );

  for ( ; pv.current(); ++pv ) {
    // Skip one's self.
    if ( pv.current() == page_view ) continue;

    if ( lC::formatName( pv.current()->name() ) == name &&
	 pv.current()->type() == type ) {
      QMessageBox mb( trC( lC::STR::LIGNUMCAD ),
		      tr( "The name \"%1\" for a page of type %2 already exists." ).
		      arg( name ).arg( trC( type ) ),
		      QMessageBox::Warning,
		      QMessageBox::Yes | QMessageBox::Default,
		      QMessageBox::Cancel,
		      QMessageBox::NoButton );
      mb.setButtonText( QMessageBox::Yes, tr( "Enter another name" ) );
      mb.setButtonText( QMessageBox::Cancel, tr( "Cancel page edit" ) );

      switch ( mb.exec() ) {
      case QMessageBox::Yes:
	return lC::Redo;
      case QMessageBox::Cancel:
	return lC::Rejected;
      }
      break;
    }
  }

  return lC::OK;
}

// Add the given page to the Design Book. Note: after a lot of
// travail, this function does NOT show() the Page. The PageViews are
// a virtual hierarchy, but this function is invoked by the PageView
// constructor itself.  Therefore, the subclass (i.e., the real
// object) is not finished being constructed when this is
// called. Thus, the virtual PageView::show() method is not callable
// until the subclass is completely constructed. So, DesignBookView
// show()s the page after it is completely constructed.

void DesignBookView::addPageView ( PageView* page_view )
{
#ifndef LAYOUT_COMPREHENSION
  if ( page_tabbar_->count() > 0 ) {
    QTab* tab0 = page_tabbar_->tabAt( 0 );
    if ( tab0 != 0 && tab0->text() == "###dummy###" ) {
      page_tabbar_->removeTab( tab0 );
    }
  }
#endif
  // This now needs to be a little bit more careful when adding a page
  // view. The order of page ids needs to be maintained.
  uint p = 0;
  for ( ; p < page_views_.count(); ++p ) {
    if ( page_views_.at( p )->id() > page_view->id() ) break;
  }
  page_views_.insert( p, page_view );
  page_tabs_.insert( page_view->tab(), page_view );
  page_tabbar_->insertTab( page_view->tab(), p );
  page_tabbar_->update();

  connect( page_view, SIGNAL( cutCopyChanged(bool) ),
	   SLOT( updateCutCopy(bool) ) );

  lCMW_->editPasteAction->setEnabled( true );
#if 0
  // Actually, let the caller decide if this is a meaningful
  // change. Examples of adding a page which are not meaningful: an
  // initial sketch page with no contents, loading the model from a
  // file.
  if ( !model_->changed() )
    modelChanged( true );
#endif
}

// Give (fully-constructed) pages a chance to show themselves

void DesignBookView::showPageView ( PageView* page_view )
{
  // The goal here is to insure consistency between the various
  // GUI components and the internal state of this object. The issue
  // is that QTabBar::addTab does not make the added tab the current
  // tab. So, when a page is added, its tab may not be selected. However,
  // the first tab that is added IS selected but QTabBar does not
  // emit the pageChanged signal.

  // So, if the given page view's tab is not currently selected, just
  // make it current and the QTabBar will fire the pageChanged signal.
  if ( page_tabbar_->currentTab() != page_view->tab()->identifier() )
    page_tabbar_->setCurrentTab( page_view->tab() );

  else {
    // Otherwise, we have to do this all ourselves
    page_views_.find( page_view );
    page_view->show();
    opengl_view_->setPageView( page_view );

    emit pageChanged( page_view->name() );
  }
}

// Remove a page from the Design Book. This merely unlinks the page.
// (Not currently used, apparently.)

void DesignBookView::removePageView ( PageView* page_view )
{
  page_views_.removeRef( page_view );
}

// Return the "last" PageView in the list. (Used to find it's ListView item.)

PageView* DesignBookView::lastPageView ( void )
{
  return page_views_.last();
}

// Search for the place where this page view goes in the hierarchy list.

QListViewItem* DesignBookView::previousItem ( uint id ) const
{
  QListViewItem* previous_item = 0;
  QListViewItem* item = model_list_item_->firstChild();
  QPtrListIterator<PageView> p( page_views_ );
  for ( ; p.current() != 0; ++p ) {
    if ( p.current()->id() > id ) break;

    previous_item = item;
    item = item->nextSibling();
  }
  return previous_item;
}

// If a view detects a change to the model, its reported here.

void DesignBookView::modelChanged ( bool status )
{
  if ( status ) {
    model_->setChanged( true );

    emit setCaption( tr( "lignumCAD: %1-%2.%3%4" ).
		     arg( lC::formatName( model_->name() ) ).
		     arg( model_->version() ).
		     arg( model_->revision() ).
		     arg( tr( "*", "model changed flag" ) ) );

    lCMW_->fileSaveAction->setEnabled( true );
  }
  else {
    model_->setChanged( false );

    emit setCaption( tr( "lignumCAD: %1-%2.%3" ).
		     arg( lC::formatName( model_->name() ) ).
		     arg( model_->version() ).
		     arg( model_->revision() ) );

    lCMW_->fileSaveAction->setEnabled( false );
  }
}

// (Destructively) erase the contents of the model.

void DesignBookView::clear ( void )
{
  opengl_view_->setPageView( 0 );

  for ( PageView* page_view = page_views_.first();
	page_view != 0;
	page_view = page_views_.current() ) {

    page_tabbar_->removeTab( page_view->tab() );
    page_tabs_.remove( page_view->tab() );
    page_views_.remove();	// i.e., remove the current page_view
    //    delete page_view;
  }

  if ( model_ != 0 )
    delete model_;

  model_ = 0;
}

// Undo the last command. This is here since the OpenGL view will
// generally have to be modified as a result of the undoing. For
// efficiency, we don't want to do updateGL more than we have to.
// So, make all the changes first and then redraw the screen.
// (If the redraw was at a lower level, then more routine changes
// could fire many redraws). Also saves a little code in the mementos.

void DesignBookView::undo ( void )
{
  CommandHistory::instance().undo();
  modelChanged( !CommandHistory::instance().canUndo() );

  view()->updateGL();
}

// Just like undo, redo's will generally need to redraw the display.
// So, do the redraw all at once after everything is updated
// internally.

void DesignBookView::redo ( void )
{
  CommandHistory::instance().redo();
  modelChanged( !CommandHistory::instance().canUndo() );

  view()->updateGL();
}

// Cut (delete). Delegate to the current page since it knows what's
// activated.

void DesignBookView::cut ( void )
{
  if ( page_views_.current() != 0 ) {
    page_views_.current()->cut();
    view()->updateGL();
  }
}

// Copy. Delegate to the current page since it knows what's activated.

void DesignBookView::copy ( void )
{
  if ( page_views_.current() != 0 )
    page_views_.current()->copy();
}

// Paste. (Well, this will require some care...)

void DesignBookView::paste ( void )
{
  if ( page_views_.current() != 0 )
    page_views_.current()->paste();
}

/*
 * Let the user adjust the display attributes to taste.
 */
void DesignBookView::editPreferences ( void )
{
  // Sets all the widgets in the dialog to the current state.
  preferences_dialog_->update();

  int ret = preferences_dialog_->exec();

  if ( ret == QDialog::Rejected ) return;

  // Save any changes the user made in the global settings box.
  // What happens if you have more than one instance open?...
  QSettings settings;

  // Recover any changes made to the default business information

  if ( preferences_dialog_->businessNameLineEdit->edited() ) {
    QString name = preferences_dialog_->businessNameLineEdit->text();
    BusinessInfo::instance().setName( name );

    if ( !name.isEmpty() )
      settings.writeEntry( lC::Setting::Business::NAME, name );
    else
      settings.removeEntry( lC::Setting::Business::NAME );
  }

  if ( preferences_dialog_->businessLocationLineEdit->edited() ) {
    QString location = preferences_dialog_->businessLocationLineEdit->text();
    BusinessInfo::instance().setLocation( location );

    if ( !location.isEmpty() )
      settings.writeEntry( lC::Setting::Business::LOCATION, location );
    else
      settings.removeEntry( lC::Setting::Business::LOCATION );
  }

  if ( preferences_dialog_->logoFileChooser->edited() ) {
    QString logo = preferences_dialog_->logoFileChooser->fileName();
    BusinessInfo::instance().setLogo( logo );

    if ( !logo.isEmpty() )
      settings.writeEntry( lC::Setting::Business::LOGO, logo );
    else
      settings.removeEntry( lC::Setting::Business::LOGO );
  }

  // Recover any changes made to the default unit preference.
  // (For now, this applies to everything. Eventaully, we might
  // do this on a per model basis...)

  bool unit_modified = false;

  if ( preferences_dialog_->unitsListBox->currentItem() !=
       UnitsBasis::instance()->at() ) {
    UnitsBasis::instance()->
      setLengthUnit( preferences_dialog_->unitsListBox->currentItem() );
    unit_modified = true;
  }

  if ( preferences_dialog_->fractionalRadioButton->isOn() &&
       UnitsBasis::instance()->format() != FRACTIONAL ) {
    UnitsBasis::instance()->setFormat( FRACTIONAL );
    unit_modified = true;
  }
  else if ( preferences_dialog_->decimalRadioButton->isOn() &&
	    UnitsBasis::instance()->format() != DECIMAL ) {
    UnitsBasis::instance()->setFormat( DECIMAL );
    unit_modified = true;
  }

  if ( preferences_dialog_->precisionComboBox->value() !=
       UnitsBasis::instance()->precision() ) {
    UnitsBasis::instance()->setPrecision( preferences_dialog_->precisionComboBox->
					  value() );
    unit_modified = true;
  }

  if ( unit_modified ) {
    QStringList length_unit;

    length_unit << QString( "%1[%2]" ).arg( UnitsBasis::instance()->name() ).
      arg( UnitsBasis::instance()->abbreviation() );
  
    length_unit << unitFormatText( UnitsBasis::instance()->format() );

    length_unit << QString::number( UnitsBasis::instance()->precision() );

    settings.writeEntry( lC::Setting::LENGTH_UNIT, length_unit, ',' );
  }

  // Recover any changes made to the default color scheme preference.

  if ( preferences_dialog_->predefinedRadioButton->isOn() ) {
    if ( !OpenGLGlobals::instance()->isPredefinedScheme() ) {
      OpenGLGlobals::instance()->setPredefinedScheme( true );
    }

    if ( preferences_dialog_->colorSchemeListBox->currentItem() !=
	 OpenGLGlobals::instance()->at() ) {
      // This effectively discards any modifications which the user
      // made to the custom settings. So, I guess the question is:
      // Should toggling from custom to predefined change the
      // values on the custom side?...I think so because you're
      // saying I want the attributes of the selected predefined entry
      // and there is only one current set of values.
      OpenGLGlobals::instance()->
	setScheme( preferences_dialog_->colorSchemeListBox->currentItem() );
    }

    settings.writeEntry( lC::Setting::ColorScheme::CUSTOM, false );

    settings.writeEntry( lC::Setting::ColorScheme::BASE,
			 OpenGLGlobals::instance()->scheme().name() );
  }
  else {
    settings.writeEntry( lC::Setting::ColorScheme::CUSTOM, true );

    settings.writeEntry( lC::Setting::ColorScheme::BASE,
			 OpenGLGlobals::instance()->scheme().name() );

    if ( OpenGLGlobals::instance()->isPredefinedScheme() )
      OpenGLGlobals::instance()->setPredefinedScheme( false );

    if ( preferences_dialog_->geometryColorChooser->edited() ) {
      OpenGLGlobals::instance()->
	setGeometryColor( preferences_dialog_->geometryColorChooser->color() );

      settings.writeEntry( lC::Setting::ColorScheme::GEOMETRY,
			   OpenGLGlobals::instance()->geometryColor().name() );
    }

    if ( preferences_dialog_->annotationColorChooser->edited() ) {
      OpenGLGlobals::instance()->
	setAnnotationColor( preferences_dialog_->annotationColorChooser->color() );

      settings.writeEntry( lC::Setting::ColorScheme::ANNOTATION,
			   OpenGLGlobals::instance()->annotationColor().name() );
    }

    if ( preferences_dialog_->gridColorChooser->edited() ) {
      OpenGLGlobals::instance()->
	setGridColor( preferences_dialog_->gridColorChooser->color() );

      settings.writeEntry( lC::Setting::ColorScheme::GRID,
			   OpenGLGlobals::instance()->gridColor().name() );
    }

    if ( preferences_dialog_->constraintPrimaryColorChooser->edited() ) {
      OpenGLGlobals::instance()->
	setConstraintPrimaryColor( preferences_dialog_->
				   constraintPrimaryColorChooser->color() );

      settings.writeEntry( lC::Setting::ColorScheme::CONSTRAINT_PRIMARY,
			   OpenGLGlobals::instance()->
			   constraintPrimaryColor().name() );
    }

    if ( preferences_dialog_->constraintSecondaryColorChooser->edited() ) {
      OpenGLGlobals::instance()->
	setConstraintSecondaryColor( preferences_dialog_->
				     constraintSecondaryColorChooser->color() );

      settings.writeEntry( lC::Setting::ColorScheme::CONSTRAINT_SECONDARY,
			   OpenGLGlobals::instance()->
			   constraintSecondaryColor().name() );
    }

    if ( preferences_dialog_->backgroundSlider->value() !=
	 OpenGLGlobals::instance()->backgroundStyle() ) {
      OpenGLGlobals::instance()->
	setBackgroundStyle( (lC::Background::Style)preferences_dialog_->
			    backgroundSlider->value() );

      settings.writeEntry( lC::Setting::ColorScheme::BACKGROUND_STYLE,
		    lC::Background::backgroundStyleText( OpenGLGlobals::instance()->
							 backgroundStyle() ) );
    }

    if ( preferences_dialog_->solidBackgroundColorChooser->edited() ) {
      OpenGLGlobals::instance()->
	setBackgroundColor( preferences_dialog_->solidBackgroundColorChooser->
			    color() );

      settings.writeEntry( lC::Setting::ColorScheme::BACKGROUND,
			   OpenGLGlobals::instance()->backgroundColor().name() );
    }

    if ( preferences_dialog_->gradientBackgroundColorChooser->edited() ) {
      OpenGLGlobals::instance()->
	setGradientColor( preferences_dialog_->gradientBackgroundColorChooser->
			  color() );

      settings.writeEntry( lC::Setting::ColorScheme::GRADIENT,
			   OpenGLGlobals::instance()->gradientColor().name() );
    }

    if ( preferences_dialog_->patternBackgroundFileChooser->edited() ) {
      OpenGLGlobals::instance()->
	setPatternFile( preferences_dialog_->patternBackgroundFileChooser->
			fileName() );

      settings.writeEntry( lC::Setting::ColorScheme::PATTERN_FILE,
			   OpenGLGlobals::instance()->patternFile() );
    }
  }
  // This may be surprising to the user. The attributes themselves
  // will be the same, but the (hidden) defaults may change.
  if ( preferences_dialog_->colorSchemeListBox->currentItem() !=
       OpenGLGlobals::instance()->at() ) {
    OpenGLGlobals::instance()->
      setDefaultScheme( preferences_dialog_->colorSchemeListBox->currentItem() );
  }

  // Recover any changes made to the *other* attribute preferences.

  if ( preferences_dialog_->dimensionFontChooser->edited() ) {
    QString font = preferences_dialog_->dimensionFontChooser->font();
    OpenGLGlobals::instance()->setDimensionFont( font );

    if ( !font.isEmpty() )
      settings.writeEntry( lC::Setting::Dimension::FONT, font );
    else
      settings.removeEntry( lC::Setting::Dimension::FONT );
  }

  if ( preferences_dialog_->arrowHeadLengthSpinBox->length()
       != OpenGLGlobals::instance()->arrowHeadLength() ) {
    OpenGLGlobals::instance()->
      setArrowHeadLength( preferences_dialog_->arrowHeadLengthSpinBox->length() );

    settings.writeEntry( lC::Setting::Dimension::ARROW_HEAD_LENGTH,
			 OpenGLGlobals::instance()->arrowHeadLength() );
  }

  if ( preferences_dialog_->arrowHeadWidthRatioSpinBox->ratio()
       != OpenGLGlobals::instance()->arrowHeadWidthRatio() ) {
    OpenGLGlobals::instance()->
      setArrowHeadWidthRatio( preferences_dialog_->arrowHeadWidthRatioSpinBox->
			      ratio() );

    settings.writeEntry( lC::Setting::Dimension::ARROW_HEAD_WIDTH_RATIO,
			 OpenGLGlobals::instance()->arrowHeadWidthRatio().serial());
  }

  if ( preferences_dialog_->arrowHeadStyleComboBox->value()
       != OpenGLGlobals::instance()->arrowHeadStyle() ) {
    OpenGLGlobals::instance()->
      setArrowHeadStyle( (lC::ArrowHeadStyle)preferences_dialog_->
			 arrowHeadStyleComboBox->value() );

    settings.writeEntry( lC::Setting::Dimension::ARROW_HEAD_STYLE,
			 lC::arrowHeadStyleText( OpenGLGlobals::instance()->
						 arrowHeadStyle() ) );
  }

  if ( preferences_dialog_->clearanceLengthSpinBox->length()
       != OpenGLGlobals::instance()->clearance() ) {
    OpenGLGlobals::instance()->
      setClearance( preferences_dialog_->clearanceLengthSpinBox->length() );

    settings.writeEntry( lC::Setting::Dimension::CLEARANCE,
			 OpenGLGlobals::instance()->clearance() );
  }

  if ( preferences_dialog_->lineThicknessLengthSpinBox->length()
       != OpenGLGlobals::instance()->lineThickness() ) {
    OpenGLGlobals::instance()->
      setLineThickness( preferences_dialog_->lineThicknessLengthSpinBox->length() );

    settings.writeEntry( lC::Setting::Dimension::LINE_THICKNESS,
			 OpenGLGlobals::instance()->lineThickness() );
  }

  if ( preferences_dialog_->extensionOffsetLengthSpinBox->length()
       != OpenGLGlobals::instance()->extensionLineOffset() ) {
    OpenGLGlobals::instance()->
      setExtensionLineOffset( preferences_dialog_->extensionOffsetLengthSpinBox->
			      length() );

    settings.writeEntry( lC::Setting::Dimension::EXTENSION_LINE_OFFSET,
			OpenGLGlobals::instance()->extensionLineOffset() );
  }

  if ( preferences_dialog_->annotationFontChooser->edited() ) {
    QString font = preferences_dialog_->annotationFontChooser->font();
    OpenGLGlobals::instance()->setAnnotationFont( font );

    if ( !font.isEmpty() )
      settings.writeEntry( lC::Setting::Annotation::FONT, font );
    else
      settings.removeEntry( lC::Setting::Annotation::FONT );
  }

  if ( preferences_dialog_->handleLengthSpinBox->length()
       != OpenGLGlobals::instance()->handleSize() ) {
    OpenGLGlobals::instance()->
      setHandleSize( preferences_dialog_->handleLengthSpinBox->length() );

    settings.writeEntry( lC::Setting::Handle::SIZE,
			 OpenGLGlobals::instance()->handleSize() );
  }
}

// Activated when the page's tab is clicked on (or QTabBar::setCurrentTab()
// is called!).

void DesignBookView::pageChanged ( int id )
{
  QTab* tab = page_tabbar_->tab( id );
  PageView* page_view = page_tabs_[tab];

  if ( page_view == 0 ) return;

  // As a side effect: Set this page to be the current item in the list
  page_views_.find( page_view );
  page_view->show();
  opengl_view_->setPageView( page_view );

  // Scroll the model hierarchy to show the page, too.
  model_list_item_->listView()->ensureItemVisible( page_view->listViewItem() );

  emit pageChanged( page_view->name() );
}

// Rename the current page, and thus, the object described on the page.

void DesignBookView::renamePage ( void )
{
  if ( page_views_.current() == 0 ) return;

 RENAME:			// Maybe we can do better than this...
  page_info_dialog_->nameEdit->
    setText( lC::formatName( page_views_.current()->name() ) );

  page_info_dialog_->nameEdit->selectAll();
  page_info_dialog_->nameEdit->setFocus();
  page_info_dialog_->buttonOk->setDefault( true );

  int ret = page_info_dialog_->exec();

  if ( ret == QDialog::Rejected )
    return;

  if ( page_info_dialog_->nameEdit->edited() ) {
    if ( uniquePageName( page_info_dialog_->nameEdit->text() ) ) {
      page_views_.current()->setName( page_info_dialog_->nameEdit->text() );

      emit pageChanged( page_views_.current()->name() );
    }
    else {
      QMessageBox mb( trC( lC::STR::LIGNUMCAD ),
		      tr( "The name \"%1\" for a page of type %2 already exists." ).
		      arg( page_info_dialog_->nameEdit->text() ).
		      arg( trC( page_views_.current()->type() ) ),
		      QMessageBox::Information,
		      QMessageBox::Yes | QMessageBox::Default,
		      QMessageBox::Cancel,
		      QMessageBox::NoButton );
      mb.setButtonText( QMessageBox::Yes, tr( "Enter another name" ) );
      mb.setButtonText( QMessageBox::Cancel, tr( "Cancel name change" ) );

      switch ( mb.exec() ) {
      case QMessageBox::Yes:
	goto RENAME;
      case QMessageBox::Cancel:
	return;
      }
    }
  }
}

// Delete the current page from the Design Book.

void DesignBookView::deletePage ( void )
{
  if ( page_views_.current() == 0 ) return;

  QPtrList<PageBase> usages = model_->whereUsed( page_views_.current()->dbURL() );

  if ( usages.count() > 0 ) {
    QStringList usage_name_list;
    for ( uint i = 0; i < usages.count(); ++i )
      usage_name_list << QString( "%1.%2" ).arg( usages.at(i)->name() ).
	arg( tr( usages.at(i)->type() ) );

    QMessageBox mb( trC( lC::STR::LIGNUMCAD ),
		    tr( "<p>You cannot delete \"%1\" because it is referenced by "
			"other assemblies:<br>"
			"<ul><li>%2</li></ul></p>" ).
		    arg( page_views_.current()->name() ).
		    arg( usage_name_list.join( "</li><li>" ) ),
		    QMessageBox::Information,
		    QMessageBox::Ok | QMessageBox::Default,
		    QMessageBox::NoButton,
		    QMessageBox::NoButton, lCMW_ );

    mb.exec();

    return;
  }

  CommandHistory::instance().
    addCommand( new DeleteCommand( QString( "delete %1" ).
				   arg( page_views_.current()->type() ),
				   page_views_.current()->memento() ) );

  deletePage( page_views_.current() );

  // Only mark the model as changed on a page deletion if the user
  // has actually invoked the "Delete Page" action.
  modelChanged( true );
}

void DesignBookView::deletePage ( PageView* page_view )
{
  if ( page_view == 0 ) return;

  disconnect( page_view, SIGNAL( cutCopyChanged(bool) ),
	      this, SLOT( updateCutCopy(bool) ) );

  opengl_view_->setPageView( 0 );

  page_tabbar_->removeTab( page_view->tab() );
  page_tabs_.remove( page_view->tab() );
  page_views_.removeRef( page_view );

  // Activate whatever page is now current in the TabBar
  QTab* tab = page_tabbar_->tab( page_tabbar_->currentTab() );
  if ( tab != 0 ) {
    page_view = page_tabs_[tab];
    page_views_.find( page_view );
    page_view->show();
    page_tabbar_->setCurrentTab( tab );
    opengl_view_->setPageView( page_view );

    emit pageChanged( page_view->name() );
  }
  else {
    lCMW_->editPasteAction->setEnabled( false );

    emit pageChanged( trC( lC::STR::NONE ) );
  }
}

// Find the page view which is looking at the given item. This
// highlights the ambiguous nature of the relation between items and
// views (are they one-to-one or one-to-many?).

View* DesignBookView::lookup ( const DBURL& db_url )
{
  QStringList path_components = QStringList::split( "/", db_url.path() );

  if ( path_components.front() != model_->name() )
    return 0;			// Not this model!

  // Pop the model name off the list
  path_components.erase( path_components.begin() );

  QPtrListIterator< PageView > pv( page_views_ );

  // The front path component is the name of a page with ".type" appended
  // to it.
  int dot_pos = path_components.front().findRev( '.' );
  QString name = path_components.front().left( dot_pos );
  QString type = path_components.front().right( path_components.front().length()
						- dot_pos - 1 );

  for ( ; pv.current(); ++pv ) {
    if ( pv.current()->name() == name && pv.current()->type() == type ) {

      // Pop the page name off the list
      path_components.erase( path_components.begin() );

      if ( path_components.empty() )
	return pv.current();
      else
	return pv.current()->lookup( path_components );
    }
  }

  return 0;
}

// Find the OpenGL selection name path for a given id path.

std::vector<GLuint> DesignBookView::lookup ( const QValueVector<uint>& id_path ) const
{
  QValueVector<uint> my_path = id_path;

  if ( my_path.empty() || my_path[0] != model_->id() )
    return std::vector<GLuint>();	// Not this model! Really an error...

  // Pop the model id off the list
  my_path.erase( my_path.begin() );

  if ( my_path.empty() )
    return std::vector<GLuint>();	// Really an error...
#if 0
  QMap<uint,PageBase*>::const_iterator p = pages_.find( my_path[0] );

  if ( p != pages_.end() ) {
    my_path.erase( my_path.begin() );
    
    if ( !my_path.empty() )
      return p.data()->lookup( my_path );
  }
#else
  QPtrListIterator<PageView> p( page_views_ );
  for ( ; p.current() != 0; ++p ) {
    if ( p.current()->id() == my_path[0] ) {
      my_path.erase( my_path.begin() );
      return p.current()->lookup( my_path );
    }
  }
#endif
  return std::vector<GLuint>();	// Really an error...
}

// Replace the current model with a new one.

void DesignBookView::newModel ( void )
{
  // If the current model has been changed, give the user a chance to save it
  if ( model_ && model_->changed() ) {
    QMessageBox mb( trC( lC::STR::LIGNUMCAD ),
		    tr( "The current model has been modified.\n"
			"Do you really want to replace it with a new model?" ),
		    QMessageBox::Information,
		    QMessageBox::Yes | QMessageBox::Default,
		    QMessageBox::No,
		    QMessageBox::Cancel );
    mb.setButtonText( QMessageBox::Yes, tr( "Discard changes" ) );
    mb.setButtonText( QMessageBox::No, tr( "Save it first" ) );
    mb.setButtonText( QMessageBox::Cancel, tr( "Cancel creation of new model" ) );

    switch ( mb.exec() ) {
    case QMessageBox::Yes:
      break;
    case QMessageBox::No:
      if ( !save() ) return;
      break;
    case QMessageBox::Cancel:
      return;
    }
  }

  // Well, the user could cancel the Wizard and then we'd have done
  // nothing but erase their current model...so, let's give the Wizard
  // his own copy of the model for starters.

  Model* model = new Model ( tr( "unnamed" ), QString::null );

  uint initial_page_id;

  if ( ! newModelWizard( model, initial_page_id ) ) {
    delete model;
    return;
  }

  clear();

  model_ = model;

  PageView* page_view = PageFactory::instance()->create( initial_page_id, this );

  // I don't know. For my sanity's sake, creating the first page is
  // not sufficient to flag the model as modified. Let the
  // configuration step decide if it's worth it.
  model_->setChanged( false );

  // Give the user a chance to configure the page and possibly decide against
  // creating it after all.
  if ( !page_view->configure() ) {
    deletePage( page_view );
    return;
  }

  page_tabbar_->setCurrentTab( page_view->tab() );
  page_view->show();
  opengl_view_->setPageView( page_view );

  showView();
}

// Read an existing model from the file.

void DesignBookView::open ( void )
{
  // If the current model has been changed, give the user a chance to save it
  if ( model_ && model_->changed() ) {
    QMessageBox mb( trC( lC::STR::LIGNUMCAD ),
		    tr( "The current model has been modified.\n"
			"Do you really want to replace it with a new model?" ),
		    QMessageBox::Information,
		    QMessageBox::Yes | QMessageBox::Default,
		    QMessageBox::No,
		    QMessageBox::Cancel );
    mb.setButtonText( QMessageBox::Yes, tr( "Discard changes" ) );
    mb.setButtonText( QMessageBox::No, tr( "Save it first" ) );
    mb.setButtonText( QMessageBox::Cancel, tr( "Cancel opening of new model" ) );

    switch ( mb.exec() ) {
    case QMessageBox::Yes:
      break;
    case QMessageBox::No:
      if ( !save() ) return;
      break;
    case QMessageBox::Cancel:
      return;
    }
  }

  QString file_name =
    QFileDialog::getOpenFileName( QString::null,
                                  tr( "lignumCAD (*.lcad);;All Files (*)" ),
                                  0,
                                  "open file dialog",
                                  tr( "Choose a file" ) );

  if ( file_name.isEmpty() )
    return;

  if ( !read( file_name ) )
    return;

  page_views_.first();
  page_tabbar_->setCurrentTab( page_views_.current()->tab() );
  page_tabbar_->update();
  page_views_.current()->show();
  opengl_view_->setPageView( page_views_.current() );

  showView();
}

// Save the model.

bool DesignBookView::save ( void )
{
  bool success;

  if ( model_->writeFileName().isEmpty() )
    success = saveAs();
  else
    success = write();

  return success;
}

// Save the model after prompting the user for a new file name.

bool DesignBookView::saveAs ( void )
{
  bool success = false;

  QString file_name;
  if ( !model_->readFileName().isEmpty() )
    file_name = model_->readFileName();
  else
    file_name = model_->writeFileName();

  file_name = QFileDialog::getSaveFileName( file_name,
					   tr( "lignumCAD (*.lcad);;All Files (*)"),
					    0,
					    "save file dialog",
					    tr( "Choose a file" ) );
  if ( !file_name.isEmpty() ) {
 
    if ( !file_name.endsWith( lC::STR::LCAD_FILE_EXT ) )
      file_name += lC::STR::LCAD_FILE_EXT;
 
    model_->setReadFileName( QString::null ); // Force a file existence check
    model_->setWriteFileName( file_name );
 
    success = write();
  }

  return success;
}
/*
 * Return the appropriate OpenGL context.
 */
OpenGLView* DesignBookView::view ( void ) const
{
  if ( printing_ )
    return opengl_printer_;
  else
    return opengl_view_;
}
/*
 * Print the design note book.
 */
void DesignBookView::print ( void )
{
  if ( page_views_.count() == 0 ) return;

  printer_->setDocName( lC::formatName( model_->name() ) );
  printer_->setCreator( tr( "%1 v%2.%3" ).
			arg( lC::STR::LIGNUMCAD ).
			arg( lC::STR::VERSION_MAJOR ).
			arg( lC::STR::VERSION_MINOR ) );

  bool ok = printer_->setup();

  if ( !ok ) return;

  printing_ = true;

  opengl_printer_->makeCurrent();

  QPainter painter( printer_ );
  QPaintDeviceMetrics pdm( printer_ );

  // OK, deduct 1/2" margin all around. For a QRect, that's 1/2" in
  // from the top and left edges and a width of 'page width - 2 *
  // DPI * 1/2"' and a height of 'page height - 2 * DPI * 1/2"'.
  QRect margin( (int)rint( pdm.logicalDpiX() * .5 ),
		(int)rint( pdm.logicalDpiY() * .5 ),
		pdm.width() - pdm.logicalDpiX(),
		pdm.height() - pdm.logicalDpiY() );

  painter.setViewport( margin );

  // The window has the same size as the viewport (including having
  // units of DOTS [aka device units]), but the upper left corner is
  // (0,0) in window coodinates.

  margin.moveBy( -margin.x(), -margin.y() );

  // Reverse the polarity of the Y axis here.
  painter.setWindow( 0, margin.height(), margin.width(), -margin.height() );

  // Print the first page...
  uint page_no = 1;

  QPtrListIterator< PageView > pv( page_views_ );

  opengl_printer_->print( *pv, painter, page_no, page_views_.count() );

  // ...then any more which happen to be there. (Evidently, painter is
  // not flushed by QPrinter::newPage(); you have to draw some more to get
  // rid of the old stuff. And, you get an extra page if there
  // are residual graphics in painter.)

  for ( ++pv, ++page_no; *pv != 0; ++pv, ++page_no ){
    printer_->newPage();

    opengl_printer_->print( *pv, painter, page_no, page_views_.count() );
  }

  printing_ = false;
  opengl_view_->redisplay();
}
/*
 * Export the current page as some kind of metafile.
 */
void DesignBookView::exportPage ( void )
{
  if ( page_views_.current() == 0 ) return;

  QString export_file =
    QFileDialog::getSaveFileName( QString::null,
				  tr( "EMF (*.emf)" ),
				  lCMW_,
				  "export file dialog",
				  tr( "Enter a file name for page export" ) );

  if ( export_file.isEmpty() ) return;

  printing_ = true;

  opengl_printer_->makeCurrent();

  opengl_printer_->exportPage( page_views_.current(), opengl_view_, export_file,
			       page_views_.at()+1, page_views_.count() );

  printing_ = false;
  opengl_view_->redisplay();
}

// Visit all the Elements in the XML file and create the corresponding
// objects.

bool DesignBookView::read ( const QString file_name )
{
  QFile f( file_name );
  QDomDocument doc( lC::STR::LIGNUMCAD );
 
  if ( f.open( IO_ReadOnly ) ) {
    QString errorString;
    int line, column;
    if ( !doc.setContent( &f, false, &errorString, &line, &column ) ) {
      QMessageBox::information( qApp->mainWidget(),
				tr( "Load model" ),
				tr( "XML read failed at line %1, column %2." ).
				arg( line ).arg( column ) );
      f.close();
      return false;
    }
  }
  else {
    QMessageBox::information( qApp->mainWidget(),
			      tr( "Load model" ),
			      tr( "Failed to open file." ) );
    return false;
  }
 
  f.close();
 
  // Probably safe to delete the old stuff now...
  clear();

  // Well, who wants to wait?
  qApp->setOverrideCursor( Qt::WaitCursor );
  qApp->processEvents( 1 );

  QDomElement docElement = doc.documentElement();

  if ( docElement.tagName() != lC::STR::LIGNUMCAD ) {
    qApp->restoreOverrideCursor();

    QMessageBox::information( lCMW(),
			      tr( "Load model" ),
			      tr( "'%1'\n"
				  "does not contain a lignumCAD model!" ).
			      arg( file_name ) );

    return false;
  }
  else {
    uint file_version = docElement.attribute( lC::STR::LVERSION ).toUInt();
    if ( file_version < lC::FILE_VERSION ) {
      qApp->restoreOverrideCursor();

      QMessageBox::information( lCMW(),
				tr( "Load model" ),
				tr( "lignumCAD cannot read a file with a format\n"
				    "version earlier than %1.\n"
				    "'%2'\n"
				    "contains a version %3 file" ).
				arg( lC::FILE_VERSION ).arg( file_name ).
				arg( file_version ) );

      return false;
    }
  }

  QDomNode n = docElement.firstChild();

  while ( !n.isNull() ) {
    QDomElement e = n.toElement();
    if ( !e.isNull() ) {
      if ( e.tagName() == lC::STR::MODEL )
	model_ = new Model( file_name, e );
      else if ( e.tagName() == lC::STR::VIEWS ) {
	restoreViews( e );
      }
    }

    qApp->processEvents( 1 );

    n = n.nextSibling();
  }

  qApp->restoreOverrideCursor();

  if ( model_ == 0 )
    return false; // An invalid model file
#if 0
  model_->dumpObjectInfo();
  QMap<uint, PageBase*>::const_iterator p = model_->pages().begin();
  for ( ; p != model_->pages().end(); ++p ) {
    cout << endl;
    p.data()->dumpObjectInfo();
    p.data()->dumpInfo();
  }
#endif
  modelChanged( false );

  return true;
}

// Restore the views read from the input file. This isn't much more than a
// convenience function.

void DesignBookView::restoreViews ( const QDomElement& xml_rep )
{
  QDomNode n = xml_rep.firstChild();

  while ( !n.isNull() ) {
    QDomElement e = n.toElement();
    if ( !e.isNull() ) {
      PageFactory::instance()->create( this, e );
    }
    // This needs some more thought...
    //qApp->processEvents( 1 );

    n = n.nextSibling();
  }
}

// Traverse the model tree (for now, the PageViews) and construct a
// DOM. Then, write the DOM to the given file.

bool DesignBookView::write ( void )
{
  // Alert the user that an existing file is being over-written.
  // *** Possibly check to see if the existing read file has changed
  // too ***
  // *** I/O errors may require more detail in the return value ***

  if ( model_->writeFileName() != model_->readFileName() ) {
    while ( true ) {
      QFileInfo info( model_->writeFileName() );
      if ( info.exists() ) {
	QMessageBox mb( trC( lC::STR::LIGNUMCAD ),
			tr( "The file \"%1\" already exists.\n"
			    "Do you really want to over-write it?" ).
			arg( model_->writeFileName() ),
			QMessageBox::Information,
			QMessageBox::Yes | QMessageBox::Default,
			QMessageBox::No,
			QMessageBox::Cancel );
	mb.setButtonText( QMessageBox::Yes, tr( "Pick a new file name" ) );
	mb.setButtonText( QMessageBox::No, tr( "Over-write it" ) );
	mb.setButtonText( QMessageBox::Cancel, tr( "Cancel saving of model" ) );

	int choice = mb.exec();

	if ( choice == QMessageBox::Yes ) {
	  QString file_name =
	    QFileDialog::getSaveFileName( QString::null,
					  tr( "lignumCAD (*.lcad);;All Files (*)" ),
					  0,
					  "save file dialog",
					  tr( "Choose a file" ) );
	  // If file_name is empty, assume the user picked Cancel and apply
	  // it to the whole saving process
	  if ( file_name.isEmpty() )
	    return false;
	  else if ( !file_name.endsWith( lC::STR::LCAD_FILE_EXT ) )
	    file_name += lC::STR::LCAD_FILE_EXT;

	  // If it's the same as what we read it from, just use it
	  else if ( file_name == model_->readFileName() ) {
	    model_->setWriteFileName( file_name );
	    break;
	  }
	  // Otherwise, it's another new file name and we need to test
	  // it in this loop, again.
	  else {
	    model_->setWriteFileName( file_name );
	  }
	}
	else if ( choice == QMessageBox::No )
	  break;
	else if ( choice == QMessageBox::Cancel )
	  return false;
      }
      else
	break;
    }
  }

  QFile file( model_->writeFileName() );
 
  if ( !file.open( IO_WriteOnly ) ) {
    QMessageBox::information( qApp->mainWidget(),
                              tr( "Save model" ),
                              tr( "Couldn't save model" ) );
    return false;
  }

  QDomDocument document( lC::STR::LIGNUMCAD );
  QDomElement root = document.createElement( lC::STR::LIGNUMCAD );
  root.setAttribute( lC::STR::LVERSION, lC::FILE_VERSION );
  document.appendChild( root );

  // First, the model geometric data...

  model_->write( root );

  // ...then, the views of that data.

  QDomElement views_element = document.createElement( lC::STR::VIEWS );
  root.appendChild( views_element );

  QPtrListIterator< PageView > p( page_views_ );

  for ( ; p.current(); ++p )
    (*p)->write( views_element );

  // Serialize the XML document to the file

  QTextStream ts( &file );

  ts << document;

  file.close();

  // If that was all successful, we can reset the modification flag on
  // the model
  model_->setReadFileName( model_->writeFileName() );

  modelChanged( false );

  lCMW_->statusBar()->message( tr( "%1 saved." ).
			       arg( lC::formatName( model_->name() ) ), 4000 );

  return true;
}

// The New Model Wizard handler.

bool DesignBookView::newModelWizard ( Model* model, uint& initial_page_id )
{
  new_model_wizard_->showPage( new_model_wizard_->NewModelPage );

  new_model_wizard_->modelNameEdit->setText( model->name() );
  new_model_wizard_->modelFileChooser->setFileName( model->writeFileName() );
 
  new_model_wizard_->versionSpinBox->setValue( model->version() );
  new_model_wizard_->revisionSpinBox->setValue( model->revision() );
 
  new_model_wizard_->descriptionEdit->setText( model->description() );
 
  new_model_wizard_->creationDateLabel->setText( model->created().toString() );
  new_model_wizard_->modificationDateLabel->setText( model->modified().toString() );

  new_model_wizard_->unsetInitialPages();

  new_model_wizard_->modelNameEdit->setFocus();
  new_model_wizard_->nextButton()->setDefault( true );

  int ret = new_model_wizard_->exec();

  if ( ret == QDialog::Rejected ) return false;
 
  if ( new_model_wizard_->modelNameEdit->edited() ) {
    model->setName( new_model_wizard_->modelNameEdit->text() );
    //    model_list_item_->setText( lC::NAME, model->name() );
  }
 
  if ( new_model_wizard_->modelFileChooser->edited() )
    model->setWriteFileName( new_model_wizard_->modelFileChooser->fileName() );
 
  if ( new_model_wizard_->descriptionEdit->isModified() )
    model->setDescription( new_model_wizard_->descriptionEdit->text() );
 
  if ( (uint)new_model_wizard_->versionSpinBox->value() != model->version() )
    model->setVersion( new_model_wizard_->versionSpinBox->value() );
 
  if ( (uint)new_model_wizard_->revisionSpinBox->value() != model->revision() )
    model->setRevision( new_model_wizard_->revisionSpinBox->value() );

  new_model_wizard_->selectedPage( initial_page_id );

  emit setCaption( tr( "lignumCAD: %1-%2.%3" ).
		   arg( model->name() ).
		   arg( model->version() ).
		   arg( model->revision() ) );

  return true;
}

// Pop-up a dialog to allow the user to edit the model's metadata.

void DesignBookView::editModelInfo ( void )
{
  model_info_dialog_->modelNameEdit->setText( lC::formatName( model_->name() ) );
  model_info_dialog_->modelFileChooser->setFileName( model_->writeFileName() );
 
  model_info_dialog_->versionSpinBox->setValue( model_->version() );
  model_info_dialog_->revisionSpinBox->setValue( model_->revision() );
 
  model_info_dialog_->descriptionEdit->setText( model_->description() );
 
  model_info_dialog_->creationDateLabel->setText( model_->created().toString() );
  model_info_dialog_->modificationDateLabel->setText( model_->modified().toString() );

  model_info_dialog_->modelNameEdit->setFocus();
  model_info_dialog_->buttonOk->setDefault( true );

  int ret = model_info_dialog_->exec();
 
  if ( ret == QDialog::Rejected ) return;

  if ( model_info_dialog_->modelNameEdit->edited() ) {
    setName( model_info_dialog_->modelNameEdit->text() );
  }

  if ( model_info_dialog_->modelFileChooser->edited() )
    model_->setWriteFileName( model_info_dialog_->modelFileChooser->fileName() );
 
  if ( model_info_dialog_->descriptionEdit->isModified() )
    model_->setDescription( model_info_dialog_->descriptionEdit->text() );
 
  if ( (uint)model_info_dialog_->versionSpinBox->value() != model_->version() )
    model_->setVersion( model_info_dialog_->versionSpinBox->value() );
 
  if ( (uint)model_info_dialog_->revisionSpinBox->value() != model_->revision() )
    model_->setRevision( model_info_dialog_->revisionSpinBox->value() );

  if ( model_->changed() ) {
    emit setCaption( tr( "lignumCAD: %1-%2.%3%4" ).
		     arg( lC::formatName( model_->name() ) ).
		     arg( model_->version() ).
		     arg( model_->revision() ).
		     arg( tr( "*", "model changed flag" ) ) );

    lCMW_->fileSaveAction->setEnabled( true );
  }
}

// Rename the model through the hierarchy view

void DesignBookView::setName ( const QString& name )
{
  if ( lC::formatName( name ) == model_->name() ) return;

  DBURL old_db_url = model_->dbURL();

  model_->setName( name );

  CommandHistory::instance().addCommand( new RenameCommand( "rename model",
							    model(),
							    old_db_url,
							    model_->dbURL() ) );
}

void DesignBookView::updateName ( const QString& name )
{
  model_->setWriteFileName( name + lC::STR::LCAD_FILE_EXT );
  model_->setChanged( true );

  model_list_item_->setText( lC::NAME, lC::STR::NAME_ID.
			     arg( lC::formatName( model_->name() ) ).
			     arg( model_->id() ) );

  emit setCaption( tr( "lignumCAD: %1-%2.%3%4" ).
		   arg( lC::formatName( model_->name() ) ).
		   arg( model_->version() ).
		   arg( model_->revision() ).
		   arg( tr( "*", "model changed flag" ) ) );
}

/*
 * Set the Undo/Redo actions to the requisite state.
 */
void DesignBookView::updateUndoRedo ( bool undo_available, bool redo_available )
{
  lCMW_->editUndoAction->setEnabled( undo_available );
  lCMW_->editRedoAction->setEnabled( redo_available );
}

/*
 * Set the Cut/Copy/Paste actions to the requisite state.
 */
void DesignBookView::updateCutCopy ( bool copy_available )
{
  lCMW_->editCutAction->setEnabled( copy_available );
  lCMW_->editCopyAction->setEnabled( copy_available );
}

// Generate a unique name for the given type.

QString DesignBookView::uniqueName ( QString (*newName)( void ),const QString& type)
{
  QString name = newName();

 redo:
  QPtrListIterator<PageView> pv( page_views_ );

  for ( ; pv.current() != 0; ++pv ) {
    if ( pv.current()->name() == name && pv.current()->type() == type ) {
      name = newName();
      goto redo;
    }
  }

  return name;
}
