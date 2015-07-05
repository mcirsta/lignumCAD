/*
 * drawingview.cpp
 *
 * DrawingView class: a view of a Drawing.
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
#include <QMenu>
#include <qaction.h>
#include <qtabbar.h>
#include <qlineedit.h>
#include <qpushbutton.h>

#include "constants.h"
#include "command.h"
#include "model.h"
#include "lignumcadmainwindow.h"
#include "designbookview.h"
#include "listviewitem.h"
#include "dimension.h"
#include "pageinfodialog.h"
#include "pagefactory.h"
#include "drawingview.h"

/*!
 * DrawingViewCreate is a memento of the current state of a Drawing and its view.
 * It handles creating and deleting both objects.
 */
class DrawingViewCreate : public CreateObject {
  //! When CommandHistory undoes or redoes the creation of this object,
  //! the Design Book view is the only handle which is required. Everything
  //! else can be looked up.
  DesignBookView* design_book_view_;
  //! The details of the drawing and view are stored as XML.
  QDomDocument xml_doc_;
public:
  /*!
   * Create a memento of the current state of the drawing and its view.
   * \param drawing_view view object of the drawing.
   */
  DrawingViewCreate ( DrawingView* drawing_view )
  {
    design_book_view_ = drawing_view->parent();

    QDomElement root = xml_doc_.createElement( lC::STR::MEMENTO );

    root.setAttribute( lC::STR::NAME, drawing_view->dbURL().toString() );

    drawing_view->drawing()->write( root );
    drawing_view->write( root );

    xml_doc_.appendChild( root );
  }
  //! Destructor does not do much.
  ~DrawingViewCreate ( void ) {}
  /*!
   * (Re)create the drawing object and its view as specified in
   * the XML representation.
   */
  void create ( void )
  {
    QDomNodeList drawing_list = xml_doc_.elementsByTagName( lC::STR::DRAWING );

    if ( drawing_list.length() > 0 ) {

      QDomNodeList drawing_view_list =
	xml_doc_.elementsByTagName( lC::STR::DRAWING_VIEW );

      if ( drawing_view_list.length() > 0 ) {
	uint drawing_id =
	  drawing_list.item(0).toElement().attribute( lC::STR::ID ).toUInt();

	new Drawing( drawing_id, drawing_list.item(0).toElement(),
		     design_book_view_->model() );

	DrawingView* drawing_view =
	  new DrawingView( design_book_view_, drawing_view_list.item(0).toElement() );

	design_book_view_->addPageView( drawing_view );
	design_book_view_->showPageView( drawing_view );
      }
    }
  }
  /*!
   * Delete the drawing object and its view as specified in the
   * XML representation.
   */
  void remove ( void )
  {
    QDomNodeList drawing_view_list =xml_doc_.elementsByTagName(lC::STR::DRAWING_VIEW);

    if ( drawing_view_list.length() > 0 ) {

      QDomElement drawing_view_element = drawing_view_list.item(0).toElement();

      QString drawing_path = drawing_view_element.attribute( lC::STR::DRAWING );

      DrawingView* drawing_view =
	dynamic_cast<DrawingView*>( design_book_view_->lookup( drawing_path ) );

      design_book_view_->deletePage( drawing_view );
    }
  }
  /*!
   * \return the XML representation.
   */
  QDomDocument* representation ( void ) { return &xml_doc_; }
  /*!
   * Append the contents of the XML representation to the argument XML
   * document.
   * \param xml_rep CommandHistory's global XML document.
   */
  void write ( QDomDocument* xml_rep )
  {
    QDomElement rep_root = xml_rep->firstChild().toElement();
    QDomElement doc_root = xml_doc_.firstChild().toElement();

    rep_root.setAttribute( lC::STR::NAME, doc_root.attribute( lC::STR::NAME ) );

    QDomNodeList drawing_list = xml_doc_.elementsByTagName( lC::STR::DRAWING );

    if ( drawing_list.length() > 0 ) {

      QDomNode element = xml_rep->importNode( drawing_list.item(0), true );
      rep_root.appendChild( element );
    }

    QDomNodeList drawing_view_list =
      xml_doc_.elementsByTagName( lC::STR::DRAWING_VIEW );

    if ( drawing_view_list.length() > 0 ) {

      QDomNode element = xml_rep->importNode( drawing_view_list.item(0), true );
      rep_root.appendChild( element );
    }
  }
  /*!
   * If the creation of a drawing is immediately followed by a rename,
   * merge this into the representation rather than having a separate
   * undo step.
   * \param rename the rename command.
   * \return true if the rename command applied to this object.
   */
  bool mergeRename ( RenameCommand* rename )
  {
    // First, make sure the rename refers to this object. Start by extracting
    // the old path (which is stored in the view element).
    QDomNodeList memento_list = xml_doc_.elementsByTagName( lC::STR::MEMENTO );

    if ( memento_list.length() > 0 ) {
      QString path = memento_list.item(0).toElement().attribute( lC::STR::NAME );

      if ( path != rename->oldDBURL().toString( ) )
	return false;

      // Additional sanity check: make sure the object and its view have elements.

      QDomNodeList drawing_list = xml_doc_.elementsByTagName( lC::STR::DRAWING );
      QDomNodeList drawing_view_list =
	xml_doc_.elementsByTagName( lC::STR::DRAWING_VIEW );

      if ( drawing_list.length() == 0 || drawing_view_list.length() == 0 )
	return false;

      // Update the name elements in the object and it's view.

      memento_list.item(0).toElement().setAttribute( lC::STR::NAME,
                         rename->newDBURL().toString( ) );

      drawing_list.item(0).toElement().
	setAttribute( lC::STR::NAME, rename->newDBURL().name() );

      drawing_view_list.item(0).toElement().setAttribute( lC::STR::DRAWING,
                      rename->newDBURL().toString( ) );

      return true;
    }

    return false;
  }
};

PageInfoDialog* DrawingView::config_dialog_ = 0;

DrawingView::DrawingView ( Drawing* drawing, DesignBookView* parent )
  : PageView( parent ), drawing_( drawing )
{
  init();
}

DrawingView::DrawingView ( DesignBookView* parent, const QDomElement& xml_rep )
  : PageView( parent )
{
  DBURL db_url( xml_rep.attribute( lC::STR::DRAWING ) );

  drawing_ = dynamic_cast<Drawing*>( model()->lookup( db_url ) );

  init();
}

DrawingView::~DrawingView ()
{
  clearFigureViews();

  delete list_view_item_;

  if ( drawing_ )
    model()->removePage( drawing_ );
}

// Canonical init routine.

void DrawingView::init ( void )
{
  tabIcon =   lC::lookupPixmap( "drawing.png" );
  tabText = lC::formatTabName( drawing_->name() );

  ListViewItem* previous_item = parent()->previousItem( drawing_->id() );

  list_view_item_ = new ListViewItem( parent()->modelListItem(), previous_item );

  list_view_item_->setData( lC::formatName( drawing_->name() )
                + QString( " <%1>" ).arg( drawing_->id(),
                          lC::NAME ) );
  list_view_item_->setData( trC( lC::STR::DRAWING ),
                            lC::TYPE );
  //TODO
  //list_view_item_->setOpen( true );
  //list_view_item_->setRenameEnabled( lC::NAME, true );

  //connect( list_view_item_, SIGNAL( nameChanged( const QString& ) ),
  //	   SLOT( listNameChanged( const QString& ) ) );

  connect( drawing_, SIGNAL( nameChanged( const QString& ) ),
	   SLOT( updateName( const QString& ) ) );

  if ( config_dialog_ == 0 )
    config_dialog_ = new PageInfoDialog;
}

// Do any post-creation configuration.

bool DrawingView::configure ( void )
{
  // Just set the name of the drawing. More later...

  config_dialog_->getUi()->nameEdit->setText( lC::formatName( name() ) );
  config_dialog_->getUi()->nameEdit->selectAll();
  config_dialog_->getUi()->nameEdit->setFocus();
  config_dialog_->getUi()->buttonOk->setDefault( true );

 redo:
  int ret = config_dialog_->exec();

  if ( ret == QDialog::Rejected ) return false;

  if ( config_dialog_->getUi()->nameEdit->isModified() ) {
    // DesignBookView handles checking the name and putting up the error dialog
    // if necessary.
    int ret = parent()->uniquePageName( this,
                    config_dialog_->getUi()->nameEdit->text(),
					drawing_->type() );

    switch ( ret ) {
    case lC::Rejected:
      return false;
    case lC::Redo:
      config_dialog_->getUi()->nameEdit->setText( lC::formatName( drawing_->name() ) );
      goto redo;
    }

    drawing_->setName( config_dialog_->getUi()->nameEdit->text() );
  }

  return true;
}

CreateObject* DrawingView::memento ( void )
{
  return new DrawingViewCreate( this );
}

void DrawingView::setName ( const QString& name )
{
  DBURL old_db_url = drawing_->dbURL();

  drawing_->setName( name );

  CommandHistory::instance().addCommand( new RenameCommand( "rename drawing",
							    model(),
							    old_db_url,
							    drawing_->dbURL() ) );
}

// Check a list view rename for uniqueness.

void DrawingView::listNameChanged ( const QString& name )
{
  int ret = parent()->uniquePageName( this, name, drawing_->type() );

  switch ( ret ) {
  case lC::OK:
    setName( name );
    break;
  case lC::Redo:
      //TODO
      //list_view_item_->startRename( lC::NAME );
  case lC::Rejected:
    updateName( drawing_->name() ); // Repaint list item with old name.
  }
}

void DrawingView::show ( void ) const
{
  lCMW()->getUi()->toolMenu->clear();

  lCMW()->getUi()->toolMenu->addAction(  lCMW()->getUi()->toolViewAction );
}

/*!
 * Add this view's XML description to the output documennt
 */
void DrawingView::write ( QDomElement& xml_rep ) const
{
  QDomDocument document = xml_rep.ownerDocument();
  QDomElement view_element = document.createElement( lC::STR::DRAWING_VIEW );
  view_element.setAttribute( lC::STR::DRAWING, drawing_->dbURL().toString() );
  xml_rep.appendChild( view_element );
}
/*
 * Append some useful actions to the OpenGL view context menu.
 */
void DrawingView::startDisplay ( QMenu* /*context_menu*/ )
{
  connect( drawing_, SIGNAL( nameChanged( const QString& ) ),
	   lCMW(), SLOT( pageChanged( const QString& ) ) );
}
/*
 * Clean up when are not the current page.
 */
void DrawingView::stopDisplay ( QMenu* /*context_menu*/ )
{
  disconnect( drawing_, SIGNAL( nameChanged( const QString& ) ),
	      lCMW(), SLOT( pageChanged( const QString& ) ) );
}

/*!
 * Draw the view.
 */
void DrawingView::draw ( void ) const
{}

/*!
 * Draw (in GL_SELECT mode) the elements indicated by the given
 * selection type. For speed, this is generally a simplified version
 * of the view.
 * \param select_type the selection mode type.
 */
void DrawingView::select ( SelectionType /*select_type*/ ) const
{
}

void DrawingView::updateName ( const QString& /*name*/ )
{
  //TODO update name
  tabText = lC::formatTabName( drawing_->name() );
  list_view_item_->setData( lC::formatName( drawing_->name() )
                + QString( " <%1>" ).arg( drawing_->id() ),
                            lC::NAME );
}

/*
 * Determine which figure to create from its simplified XML representation.
 */
void DrawingView::pasteFigure ( const QDomElement& /*xml_rep*/ )
{
}

void DrawingView::cancelOperation ( void )
{
  SelectedNames none;
  highlightFigures( none );

  deactivateFigures();

  emit newInformation( trC( lC::STR::NONE ) );
}

/*!
 * Instance of assembly metadata for a part.
 */
class DrawingViewMetadata : public PageMetadata {
public:
  /*!
   * Construct a (what amounts to a const) drawing metadata object.
   */
  DrawingViewMetadata ( void )
    : PageMetadata ( lC::STR::DRAWING,
		     "new_drawing.png",
		     QT_TRANSLATE_NOOP( "lignumCADMainWindow", "&Drawing" ),
		     "&Drawing...",
		     "Insert a new Drawing",
		     "<p><b>Insert|Drawing</b></p><p>Click this button to create a new drawing. The drawing will contain the final dimensions needed to fabricate a part. (More than one part can also appear on a drawing.)</p>" )
  {
    //    cout << "Constructing drawing metadata" << endl;
    PageFactory::instance()->addPageMetadata( Drawing::PAGE_ID, this );
  }
  /*!
   * Create a new drawing and its view.
   * \return view handle for the drawing and its view.
   */
   DrawingView* create ( DesignBookView* parent ) const
  {
    QString name = parent->uniqueName( &Drawing::newName, lC::STR::DRAWING );

    Drawing* drawing = new Drawing( parent->model()->newID(), name,
				    parent->model() );
    DrawingView* drawing_view = new DrawingView( drawing, parent );
    parent->addPageView( drawing_view );
    return drawing_view;
  }

  /*!
   * Create a drawing from its XML representation.
   * \return handle for the drawing.
   */
   Drawing* create ( Model* parent, const QDomElement& xml_rep ) const
  {
    uint drawing_id = xml_rep.attribute( lC::STR::ID ).toUInt();
    return new Drawing( drawing_id, xml_rep, parent );
  }

  /*!
   * Create a drawing view from its XML representation.
   * \return handle for the drawing view.
   */
   DrawingView* create ( DesignBookView* parent, const QDomElement& xml_rep ) const
  {
    DrawingView* drawing_view = new DrawingView( parent, xml_rep );
    parent->addPageView( drawing_view );
    return drawing_view;
  }

  /*!
   * Retrieve the QAction which can create this page.
   * \param lCMW pointer to the main window where the actions are stored.
   * \return the action associated with creating an instance of this page.
   */
  QAction* action ( lignumCADMainWindow* lCMW ) const
  {
    return lCMW->getUi()->insertDrawingAction;
  }
};

DrawingViewMetadata drawing_view_metadata;
