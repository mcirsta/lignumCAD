/*
 * rectangleview.cpp
 *
 * Rectangle View classes
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

#include <qaction.h>
#include <qpopupmenu.h>
#include <qbuttongroup.h>
#include <qradiobutton.h>
#include <qcheckbox.h>
#include <qlineedit.h>
#include <qcursor.h>
#include <qslider.h>
#include <qlabel.h>
#include <qmessagebox.h>

#include "style.h"
#include "configuration.h"
#include "line.h"
#include "command.h"
#include "lignumcadmainwindow.h"
#include "cursorfactory.h"
#include "page.h"
#include "pageview.h"
#include "designbookview.h"
#include "model.h"
#include "openglview.h"
#include "arrow.h"
#include "alignment.h"
#include "handle.h"
#include "dimensionview.h"
#include "listviewitem.h"
#include "units.h"
#include "rectangle.h"
#include "lccolorchooser.h"
#include "lcdefaultlengthspinbox.h"
#include "lcdefaultcombobox.h"
#include "lcconstraintchooser.h"
#include "lcdefaultfilechooser.h"
#include "rectangleinfodialog.h"
#include "rectangleview.h"

namespace Space2D {
  /*!
   * This is the Rectangle's (and its view's) memento. A rectangle
   * (and its view) can be reconstructed from just this description.
   */
  class RectangleViewCreate : public CreateObject {
    //! When CommandHistory undoes or redoes the creation of this object,
    //! the Design Book view is the only handle which is required. Everything
    //! else can be looked up.
    DesignBookView* design_book_view_;
    //! The details of the rectangle and view are stored as XML.
    QDomDocument xml_doc_;
  public:
    /*!
     * Create a memento of the current state of the rectangle and its view.
     * \param rectangle_view view object of the rectangle.
     */
    RectangleViewCreate ( RectangleView* rectangle_view )
    {
      design_book_view_ = rectangle_view->parent()->parent();

      QDomElement root = xml_doc_.createElement( lC::STR::MEMENTO );

      root.setAttribute( lC::STR::NAME, rectangle_view->dbURL().toString(true) );

      rectangle_view->rectangle()->write( root );
      rectangle_view->write( root );

      xml_doc_.appendChild( root );
    }
    //! Destructor doesn't do much.
    ~RectangleViewCreate ( void ) {}
    /*!
     * (Re)create the rectangle object and its view as specified in
     * the XML representation.
     */
    void create ( void )
    {
      QDomNodeList memento_list = xml_doc_.elementsByTagName( lC::STR::MEMENTO );

      if ( memento_list.length() > 0 ) {
	DBURL url = memento_list.item(0).toElement().attribute( lC::STR::NAME );

	// First, we have to find the parent page and view for the rectangle.

	DBURL parent = url.parent();

	Page* page =
	  dynamic_cast<Page*>( design_book_view_->model()->lookup( parent ) );
	PageView* page_view =
	  dynamic_cast<PageView*>( design_book_view_->lookup( parent ) );

	// Now, we can extract the XML representations for the rectangle
	// and its view
	QDomNodeList rectangle_list =
	  xml_doc_.elementsByTagName( lC::STR::RECTANGLE );

	if ( rectangle_list.length() > 0 ) {

	  QDomNodeList rectangle_view_list =
	    xml_doc_.elementsByTagName( lC::STR::RECTANGLE_VIEW );

	  if ( rectangle_view_list.length() > 0 ) {
	    uint rectangle_id =
	      rectangle_list.item(0).toElement().attribute( lC::STR::ID ).toUInt();

	    new Rectangle( rectangle_id, rectangle_list.item(0).toElement(), page );

	    page_view->
	      addFigureView( new RectangleView( rectangle_view_list.
						item(0).toElement(), page_view ) );
	  }
	}

	// See if there are any constraints which should be put back
	// in place when this rectangle is restored. Evidently
	// these have to be recovered in two phases: first the
	// constraints themselves and then any views we need
	// to restore.

	QDomNodeList unconstraint_list =
	  xml_doc_.elementsByTagName( lC::STR::UNCONSTRAINTS );

	if ( unconstraint_list.length() > 0 ) {
	  QDomNode n = unconstraint_list.item( 0 ).firstChild();

	  while ( !n.isNull() ) {
	    QDomElement e = n.toElement();
	    if ( e.tagName() == lC::STR::CONSTRAINED_LINE ) {
	      DBURL db_url = e.attribute( lC::STR::URL );

	      ConstrainedLine* line =
		dynamic_cast<ConstrainedLine*>( page->model()->lookup( db_url ));

	      if ( line != 0 ) {
		QDomElement new_constraint = e.firstChild().toElement();
		line->setConstraint( new_constraint );
	      }
	      else
		cerr << "Yikes, line did not exist!" << endl;
	    }
	    n = n.nextSibling();
	  }

	  n = unconstraint_list.item( 0 ).firstChild();

	  while ( !n.isNull() ) {
	    QDomElement e = n.toElement();
	    if ( e.tagName() == lC::STR::DIMENSION_VIEW ) {
	      DBURL db_url = e.attribute( lC::STR::URL );
	      
	      FigureView* figure_view =
		dynamic_cast<FigureView*>( design_book_view_->lookup( db_url ) );

	      if ( figure_view != 0 ) {
		QDomElement dimension_view = e.firstChild().toElement();
		figure_view->setDimensionView( dimension_view );
	      }
	      else
		cerr << "Yikes, figure view did not exist!" << endl;
	    }
	    n = n.nextSibling();
	  }
	}
      }
    }
    /*!
     * Delete the rectangle object and its view as specified in the
     * XML representation.
     */
    void remove ( void )
    {
      QDomNodeList memento_list = xml_doc_.elementsByTagName( lC::STR::MEMENTO );

      if ( memento_list.length() > 0 ) {
	DBURL url = memento_list.item(0).toElement().attribute( lC::STR::NAME );

	RectangleView* rectangle_view =
	  dynamic_cast<RectangleView*>( design_book_view_->lookup( url ) );

	DBURL parent = url.parent();

	PageView* page_view =
	  dynamic_cast<PageView*>( design_book_view_->lookup( parent ) );

	page_view->removeFigureView( rectangle_view );
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

      QDomNodeList rectangle_list =
	xml_doc_.elementsByTagName( lC::STR::RECTANGLE );

      if ( rectangle_list.length() > 0 ) {
	QDomNode element = xml_rep->importNode( rectangle_list.item(0), true );

	rep_root.appendChild( element );
      }

      QDomNodeList rectangle_view_list =
	xml_doc_.elementsByTagName( lC::STR::RECTANGLE_VIEW );

      if ( rectangle_view_list.length() > 0 ) {
	QDomNode element = xml_rep->importNode( rectangle_view_list.item(0),
						true );
	rep_root.appendChild( element );
      }

      QDomNodeList constraint_list =
	xml_doc_.elementsByTagName( lC::STR::CONSTRAINTS );

      if ( constraint_list.length() > 0 ) {
	QDomNode element = xml_rep->importNode( constraint_list.item(0), true );

	rep_root.appendChild( element );
      }

      QDomNodeList unconstraint_list =
	xml_doc_.elementsByTagName( lC::STR::UNCONSTRAINTS );

      if ( unconstraint_list.length() > 0 ) {
	QDomNode element = xml_rep->importNode( unconstraint_list.item(0), true );

	rep_root.appendChild( element );
      }
    }
    /*!
     * If the creation of a rectangle is immediately followed by a rename,
     * merge this into the representation rather than having a separate
     * undo step.
     * \param rename the rename command.
     * \return true if the rename command applied to this object.
     */
    bool mergeRename ( RenameCommand* rename )
    {
      // First, make sure the rename refers to this object. Start by extracting
      // the old path.
      QDomNodeList memento_list = xml_doc_.elementsByTagName( lC::STR::MEMENTO );

      if ( memento_list.length() > 0 ) {
	QDomElement root = memento_list.item(0).toElement();

	QString path = root.attribute( lC::STR::NAME );

	if ( path != rename->oldDBURL().toString(true) )
	  return false;

	// If this memento has reconstaint commands (i.e., other objects
	// were affected by the creation of this rectangle), then it's
	// too difficult to update all the reconstraints and we abandon
	// the rename merge. Normally, this can't happen when a rectangle
	// is created through the UI, only when it's deleted (and then
	// you can't change the name, of course).
	QDomNodeList reconstaint_list;
	reconstaint_list = xml_doc_.elementsByTagName( lC::STR::CONSTRAINTS );
	if ( reconstaint_list.count() > 0 ) return false;
	reconstaint_list = xml_doc_.elementsByTagName( lC::STR::UNCONSTRAINTS );
	if ( reconstaint_list.count() > 0 ) return false;

	// Additional sanity check: make sure the object and its view have elements.

	QDomNodeList rectangle_list =
	  xml_doc_.elementsByTagName( lC::STR::RECTANGLE );
	QDomNodeList rectangle_view_list =
	  xml_doc_.elementsByTagName( lC::STR::RECTANGLE_VIEW );

	if ( rectangle_list.length() == 0 ||
	     rectangle_view_list.length() == 0 )
	  return false;

	// Update the name elements in the object and it's view.

	root.setAttribute( lC::STR::NAME, rename->newDBURL().toString(true) );

	// Just rewrite the XML representations since the
	// line references may have been renamed too!

	root.removeChild( rectangle_list.item(0) );
	root.removeChild( rectangle_view_list.item(0) );

	RectangleView* rectangle_view =
	  dynamic_cast<RectangleView*>( design_book_view_->
					lookup( rename->newDBURL() ) );

	rectangle_view->rectangle()->write( root );
	rectangle_view->write( root );

	return true;
      }

      return false;
    }
  };

  /*!
   * Another thing to (un)do is restyling rectangles. Not sure how
   * general this will end up being.
   */
  class RestyleRectangleCommand : public Command {
    //! Used to lookup the view.
    DesignBookView* design_book_view_;
    //! Name of the rectangle (and its view, too).
    DBURL db_url_;
    //! There are a number of attributes which can optionally be
    //! changed, so they're just encapsulated by their XML representation.
    QDomDocument xml_doc_;
  public:
    RestyleRectangleCommand ( const QString& name, RectangleView* rectangle_view )
      : Command( name ),
	design_book_view_( rectangle_view->parent()->parent() ),
	db_url_( rectangle_view->dbURL() )
    {}
    ~RestyleRectangleCommand ( void )
    {}

    Type type ( void ) const { return RestyleRectangle; }

    bool merge ( Command* /*command*/ ) { return false; }

    void execute ( void )
    {
      RectangleView* rv =
	dynamic_cast<RectangleView*>( design_book_view_->lookup( db_url_ ) );

      QDomNodeList width_side_list =
	xml_doc_.elementsByTagName( lC::STR::WIDTH_ANNOTATION_SIDE );

      if ( width_side_list.count() > 0 ) {
	lC::AnnotationSide width_side =
	  lC::annotationSide( width_side_list.item(0).toElement().
			      attribute( lC::STR::NEW_VALUE ) );

	rv->setWidthAnnotationSide( width_side );
      }

      QDomNodeList height_side_list =
	xml_doc_.elementsByTagName( lC::STR::HEIGHT_ANNOTATION_SIDE );

      if ( height_side_list.count() > 0 ) {
	lC::AnnotationSide height_side =
	  lC::annotationSide( height_side_list.item(0).toElement().
			      attribute( lC::STR::NEW_VALUE ) );

	rv->setHeightAnnotationSide( height_side );
      }

      QDomNodeList style_list =
	xml_doc_.elementsByTagName( lC::STR::NEW_STYLE );

      if ( style_list.count() > 0 ) {
	Style* style = new Style( style_list.item(0).firstChild().toElement() );

	rv->setStyle( style );
      }
      else
	rv->setStyle( 0 );
    }

    void unexecute ( void )
    {
      RectangleView* rv =
	dynamic_cast<RectangleView*>( design_book_view_->lookup( db_url_ ) );

      QDomNodeList width_side_list =
	xml_doc_.elementsByTagName( lC::STR::WIDTH_ANNOTATION_SIDE );

      if ( width_side_list.count() > 0 ) {
	lC::AnnotationSide width_side =
	  lC::annotationSide( width_side_list.item(0).toElement().
			      attribute( lC::STR::OLD_VALUE ) );

	rv->setWidthAnnotationSide( width_side );
      }

      QDomNodeList height_side_list =
	xml_doc_.elementsByTagName( lC::STR::HEIGHT_ANNOTATION_SIDE );

      if ( height_side_list.count() > 0 ) {
	lC::AnnotationSide height_side =
	  lC::annotationSide( height_side_list.item(0).toElement().
			      attribute( lC::STR::OLD_VALUE ) );

	rv->setHeightAnnotationSide( height_side );
      }

      QDomNodeList style_list =
	xml_doc_.elementsByTagName( lC::STR::OLD_STYLE );

      if ( style_list.count() > 0 ) {
	Style* style = new Style( style_list.item(0).firstChild().toElement() );

	rv->setStyle( style );
      }
      else
	rv->setStyle( 0 );
    }

    void write ( QDomDocument* document ) const
    {
      QDomElement restyle_element =
	document->createElement( lC::STR::RESTYLE_RECTANGLE );

      restyle_element.setAttribute( lC::STR::NAME, db_url_.toString(true) );

      QDomNode root_node = document->firstChild();
      if ( !root_node.isNull() )
	document->replaceChild( restyle_element, root_node );
      else
	document->appendChild( restyle_element );

      QDomNodeList width_side_list =
	xml_doc_.elementsByTagName( lC::STR::WIDTH_ANNOTATION_SIDE );

      if ( width_side_list.count() > 0 ) {
	QDomNode width_side_node =
	  document->importNode( width_side_list.item(0), false );

	restyle_element.appendChild( width_side_node );
      }

      QDomNodeList height_side_list =
	xml_doc_.elementsByTagName( lC::STR::HEIGHT_ANNOTATION_SIDE );

      if ( height_side_list.count() > 0 ) {
	QDomNode height_side_node =
	  document->importNode( height_side_list.item(0), false );

	restyle_element.appendChild( height_side_node );
      }

      QDomNodeList old_style_list =
	xml_doc_.elementsByTagName( lC::STR::OLD_STYLE );

      if ( old_style_list.count() > 0 ) {
	QDomNode old_style_node =
	  document->importNode( old_style_list.item(0), true );

	restyle_element.appendChild( old_style_node );
      }

      QDomNodeList new_style_list =
	xml_doc_.elementsByTagName( lC::STR::NEW_STYLE );

      if ( new_style_list.count() > 0 ) {
	QDomNode new_style_node =
	  document->importNode( new_style_list.item(0), true );

	restyle_element.appendChild( new_style_node );
      }
    }

    void changeWidthAnnotationSide ( lC::AnnotationSide old_side,
				     lC::AnnotationSide new_side )
    {
      QDomElement width_side_element =
	xml_doc_.createElement( lC::STR::WIDTH_ANNOTATION_SIDE );

      width_side_element.setAttribute( lC::STR::OLD_VALUE,
				       annotationSideText( old_side ) );
      width_side_element.setAttribute( lC::STR::NEW_VALUE,
				       annotationSideText( new_side ) );

      xml_doc_.appendChild( width_side_element );
    }

    void changeHeightAnnotationSide ( lC::AnnotationSide old_side,
				      lC::AnnotationSide new_side )
    {
      QDomElement height_side_element =
	xml_doc_.createElement( lC::STR::HEIGHT_ANNOTATION_SIDE );

      height_side_element.setAttribute( lC::STR::OLD_VALUE,
				       annotationSideText( old_side ) );
      height_side_element.setAttribute( lC::STR::NEW_VALUE,
				       annotationSideText( new_side ) );

      xml_doc_.appendChild( height_side_element );
    }

    void changeStyle ( const Style* old_style, const Style* new_style )
    {
      if ( old_style != 0 ) {
	QDomElement old_style_element = xml_doc_.createElement( lC::STR::OLD_STYLE);
	old_style->write( old_style_element );
	xml_doc_.appendChild( old_style_element );
      }
      if ( new_style != 0 ) {
	QDomElement new_style_element = xml_doc_.createElement( lC::STR::NEW_STYLE);
	new_style->write( new_style_element );
	xml_doc_.appendChild( new_style_element );
      }
    }
  };

  RectangleCreateInput::RectangleCreateInput ( Rectangle* rectangle,
					       RectangleView* rectangle_view )
    : rectangle_( rectangle ), rectangle_view_( rectangle_view )
  {}

  void RectangleCreateInput::startDisplay ( QPopupMenu* context_menu )
  {
    context_menu_ = context_menu;

    QAction* cancel_action =
      rectangle_view_->parent()->lCMW()->cancelRectangleAction;

    separator_id_ = context_menu_->insertSeparator();
    cancel_action->addTo( context_menu_ );
    connect( cancel_action, SIGNAL( activated() ), SLOT( cancelOperation() )  );

    rectangle_view_->view()->
      setCursor( CursorFactory::instance().cursor( CursorFactory::RECTANGLE ) );
  }

  void RectangleCreateInput::mousePrepress ( QMouseEvent* /*me*/,
					     const SelectedNames& /*selected*/ )
  {}

  void RectangleCreateInput::mousePress ( QMouseEvent* me,
					  const SelectedNames& selected )
  {
    Point origin = rectangle_view_->view()->unproject2D( me->pos() );
    UnitsBasis::instance()->round( origin );

    // OK, this is really the parametric behavior: If appropriate items
    // are highlighted, then base this rectangle on those items.

    // I can already see problems with this if the highlighted geometry
    // is not a horizontal or vertical line. For now, we'll have to assume
    // they're lines and think more deeply about other geometries later :-(

    // Also, two already coincident lines appear together if you happen
    // to be where they overlap. The right thing to do, of course, depends
    // on the design intent! So, we'll have to punt this back to user.
    // But when? Before the rectangle can continue or after it's done?
    // *After* seems easier on the hand and mouse.
    // Well, it affects any automatic Dimensions created, though. To a certain
    // extent, you can clean this up later without too much difficulty.

    Line* x_reference = 0;
    Line* y_reference = 0;

    SelectedNames::const_iterator f;

    for ( f = selected.begin(); f != selected.end(); ++f ) {
      FigureView* fv =
	dynamic_cast< FigureView* >( rectangle_view_->parent()->
				     figureSelectionNames()[ (*f).second[0] ] );

      vector<GLuint>::const_iterator g;

      for ( g = (*f).second.begin(); g != (*f).second.end(); ++g ) {

	Line* line = dynamic_cast< Line* >( fv->geometry( *g ) );

	if ( line != 0 ) {
	  if ( line->isVertical() )
	    x_reference = line;
	  else if ( line->isHorizontal() )
	    y_reference = line;
	}
      }
    }
    // Note: this emits a signal which the view will receive and construct
    // the proper dimension views (in theory).
    rectangle_->initializeEdges( origin, x_reference, y_reference );
  }

  void RectangleCreateInput::mouseDrag ( QMouseEvent* me,
					 const SelectedNames& /*selected*/ )
  {
    Point current = rectangle_view_->view()->unproject2D( me->pos() );
    UnitsBasis::instance()->round( current );

    // Again, this emits a signal which view will receive and modify
    // the dimension views as necessary (in theory).
    rectangle_->moveEdges( current );
  }

  bool RectangleCreateInput::mouseRelease ( QMouseEvent* me,
					    const SelectedNames& selected )
  {
    Point current = rectangle_view_->view()->unproject2D( me->pos() );
    UnitsBasis::instance()->round( current );

    Line* x_reference = 0;
    Line* y_reference = 0;

    SelectedNames::const_iterator f;

    for ( f = selected.begin(); f != selected.end(); ++f ) {
      FigureView* fv =
	dynamic_cast< FigureView* >( rectangle_view_->parent()->
				     figureSelectionNames()[ (*f).second[0] ] );

      vector<GLuint>::const_iterator g;

      for ( g = (*f).second.begin(); g != (*f).second.end(); ++g ) {

	Line* line = dynamic_cast< Line* >( fv->geometry( *g ) );

	if ( line != 0 ) {
	  if ( line->isVertical() )
	    x_reference = line;
	  else if ( line->isHorizontal() )
	    y_reference = line;
	}
      }
    }
    // Finally, this should emit a signal which the view receives and
    // finalizes any necessary dimension views.
    rectangle_->finishEdges( current, x_reference, y_reference );

    PageView* page_view = rectangle_view_->parent();

    QAction* cancel_action = page_view->lCMW()->cancelRectangleAction;
    cancel_action->disconnect();
    cancel_action->removeFrom( context_menu_ );
    context_menu_->removeItem( separator_id_ );

    rectangle_view_->unsetCursor();

    rectangle_view_->parent()->figureComplete();

    rectangle_view_->parent()->activateFigure( rectangle_view_ );

    CommandHistory::instance().
      addCommand( new CreateCommand( "create rectangle",
				     rectangle_view_->memento() ) );

    return false;
  }

  void RectangleCreateInput::keyPress ( QKeyEvent* ke )
  {
    if ( ke->key() == Qt::Key_Escape )
      cancelOperation();
    else
      ke->ignore();
  }

  void RectangleCreateInput::cancelOperation ( void )
  {
    PageView* page_view = rectangle_view_->parent();

    QAction* cancel_action = page_view->lCMW()->cancelRectangleAction;

    cancel_action->disconnect();
    cancel_action->removeFrom( context_menu_ );
    context_menu_->removeItem( separator_id_ );

    page_view->removeFigureView( rectangle_view_ );

    page_view->cancelOperation();
  }

  RectangleModifyInput::RectangleModifyInput ( Rectangle* rectangle,
					       RectangleView* rectangle_view )
    : selection_type_( TRANSIENT, FIGURE ),
      rectangle_( rectangle ), rectangle_view_( rectangle_view ), xml_rep_( 0 )
  {
  }

  void RectangleModifyInput::mousePrepress ( QMouseEvent* me,
					     const SelectedNames& selected )
  {
    if ( selected.empty() ) {
      rectangle_view_->unsetCursor();
      return;
    }

    SelectedNames::const_iterator f;

    for ( f = selected.begin(); f != selected.end(); ++f ) {

      if ( (*f).second[0] == rectangle_view_->selectionName() ) {

	if ( (*f).second.size() > 1 ) {

	  // The order in which selection names are allocated means
	  // that the handle selections are at the end of the selection
	  // name submap. So, we take the geometry of interest from
	  // the end (using rbegin()).

	  GLuint g = (*f).second[1];

	  if ( rectangle_view_->isGeometry( g ) ) {

	    rectangle_view_->setCursor( g );

	    // Determine which edge(s) we're dragging......

	    edges_ = rectangle_view_->dragEdges( g );

	    me->accept();
	  }
	  else {
	    rectangle_view_->unsetCursor();
	  }

	  return; // Probably not the last word here
	}
      }
    }

    rectangle_view_->unsetCursor();
  }

  void RectangleModifyInput::mousePress ( QMouseEvent* me,
					  const SelectedNames& selected )
  {
    // Let the Page handle these events

    if ( me->button() != Qt::LeftButton || selected.empty() ) {
      me->ignore();
      return;
    }

    SelectedNames::const_iterator f;

    for ( f = selected.begin(); f != selected.end(); ++f ) {

      if ( (*f).second[0] == rectangle_view_->selectionName() ) {

	if ( (*f).second.size() > 1 ) {

	  // The order in which selection names are allocated means
	  // that the handle selections are at the end of the selection
	  // name submap. So, we take the geometry of interest from
	  // the end (using rbegin()).

	  GLuint g = (*f).second[1];

	  if ( rectangle_view_->isGeometry( g ) ) {

	    old_x0_offset_ = rectangle_->x0()->offset();
	    old_y0_offset_ = rectangle_->y0()->offset();
	    old_x1_offset_ = rectangle_->x1()->offset();
	    old_y1_offset_ = rectangle_->y1()->offset();

	    last_pnt_ = rectangle_view_->view()->unproject2D( me->pos());
	    UnitsBasis::instance()->round( last_pnt_ );
	    start_pnt_ = last_pnt_;

	    drag_handle_ = g;

	    // Search for other geometry's to align with.
	    selection_type_ = SelectionType( TRANSIENT, EDGE );

	    // Prevent highlighting of this figure's geometry.
	    rectangle_->setComplete( false );

	    return;
	  }
	}
      }
    }

    me->ignore();
  }

  void RectangleModifyInput::mouseDrag ( QMouseEvent* me,
					 const SelectedNames& /*selected*/ )
  {
    if ( drag_handle_ == 0 ) return;

    current_pnt_ = rectangle_view_->view()->unproject2D( me->pos() );
    UnitsBasis::instance()->round( current_pnt_ );

    // To avoid some mouse pointer hysteresis when resizing fails, don't
    // move the previous point if resizing doesn't succeed.

    // [It suddenly occurs to me that this is really the domain of this
    // class, not really the view. However, we may be changing a cosmetic
    // aspect of the view, too (in particular, the position of a
    // dimension view; not really a property of the rectangle)...]

    // Anyway, the rectangle view adjust routine will modify either
    // the rectangle or the dimension view positions, based on whichever
    // is activated.
    lC::ValidDelta ok = rectangle_view_->adjust( drag_handle_,
						 last_pnt_, current_pnt_);
	  
    if ( ok.dx_ok_ )
      last_pnt_[X] = current_pnt_[X];
    if ( ok.dy_ok_ )
      last_pnt_[Y] = current_pnt_[Y];

    rectangle_view_->parent()->figureModified();
  }

  bool RectangleModifyInput::mouseRelease ( QMouseEvent* /*me*/,
					    const SelectedNames& selected )
  {
    MoveLinesCommand* command = 0;

    if ( last_pnt_ != start_pnt_ ) {
      command = new MoveLinesCommand( "move/resize rectangle",
				      rectangle_view_->model() );

      if ( rectangle_->x0()->offset() != old_x0_offset_ )
	command->add( rectangle_->x0(), old_x0_offset_ );
      if ( rectangle_->y0()->offset() != old_y0_offset_ )
	command->add( rectangle_->y0(), old_y0_offset_ );
      if ( rectangle_->x1()->offset() != old_x1_offset_ )
	command->add( rectangle_->x1(), old_x1_offset_ );
      if ( rectangle_->y1()->offset() != old_y1_offset_ )
	command->add( rectangle_->y1(), old_y1_offset_ );
    }

    Line* x_reference = 0;
    Line* y_reference = 0;

    SelectedNames::const_iterator f;

    for ( f = selected.begin(); f != selected.end(); ++f ) {
      FigureView* fv =
	dynamic_cast< FigureView* >( rectangle_view_->parent()->
				     figureSelectionNames()[ (*f).second[0] ] );

      if ( fv == rectangle_view_ ) continue;

      vector<GLuint>::const_iterator g;

      for ( g = (*f).second.begin(); g != (*f).second.end(); ++g ) {

	Line* line = dynamic_cast< Line* >( fv->geometry( *g ) );

	if ( line != 0 ) {
	  if ( line->isVertical() )
	    x_reference = line;
	  else if ( line->isHorizontal() )
	    y_reference = line;
	}
      }
    }

    if ( x_reference != 0 || y_reference != 0 ) {
      xml_rep_ = new QDomDocument;

      QDomElement reconstraints = xml_rep_->createElement( lC::STR::RECONSTRAINTS );
      xml_rep_->appendChild( reconstraints );

      QDomElement old_constraints = xml_rep_->createElement( lC::STR::OLD_CONSTRAINTS );
      reconstraints.appendChild( old_constraints );

      QDomElement new_constraints = xml_rep_->createElement( lC::STR::NEW_CONSTRAINTS );
      reconstraints.appendChild( new_constraints );
    }

    for ( ConstrainedLine* edge = edges_.first(); edge != 0; edge = edges_.next() ){
      if ( x_reference != 0 && edge->isVertical() ) {
	// Of all the things that can go wrong, this is the only one
	// which isn't protected against as a result of preventing
	// rectangle from being selected itself
	if ( !x_reference->dependsOn( edge ) ) {
	  QDomElement old_xml = xml_rep_->createElement( lC::STR::CONSTRAINED_LINE );
	  old_xml.setAttribute( lC::STR::URL, edge->dbURL().toString(true) );
	  edge->constraint()->write( old_xml );
	  xml_rep_->firstChild().firstChild().appendChild( old_xml );

	  // If from_reference's modifiedConstraint signal is emitted,
	  // then the old XML representation of its constraint is appended
	  // to this reconstrain command (done in the SLOT).
	  ConstrainedLine* from_reference =
	    dynamic_cast<ConstrainedLine*>( edge->reference() );

	  if ( from_reference != 0 ) {
	    from_ref_xml_ = xml_rep_->createElement( lC::STR::CONSTRAINED_LINE );
	    from_ref_xml_.setAttribute( lC::STR::URL, from_reference->dbURL().toString(true) );
	    from_reference->constraint()->write( from_ref_xml_ );

	    connect( from_reference, SIGNAL( modifiedConstraint() ),
		     SLOT( cascadeConstraints() ) );
	  }

	  edge->setConstraint( new Coincident( x_reference ) );

	  if ( from_reference != 0 )
	    disconnect( from_reference, SIGNAL( modifiedConstraint() ),
			this, SLOT( cascadeConstraints() ) );

	  QDomElement new_xml = xml_rep_->createElement( lC::STR::CONSTRAINED_LINE );
	  new_xml.setAttribute( lC::STR::URL, edge->dbURL().toString(true) );
	  edge->constraint()->write( new_xml );
	  xml_rep_->firstChild().firstChild().nextSibling().appendChild( new_xml );
	}
	else
	  cout << "x edge connection rejected for circular dependencies" << endl;
      }
      
      if ( y_reference != 0 && edge->isHorizontal() ) {
	if ( !y_reference->dependsOn( edge ) ) {
	  QDomElement old_xml = xml_rep_->createElement( lC::STR::CONSTRAINED_LINE );
	  old_xml.setAttribute( lC::STR::URL, edge->dbURL().toString(true) );
	  edge->constraint()->write( old_xml );
	  xml_rep_->firstChild().firstChild().appendChild( old_xml );


	  // If from_reference's modifiedConstraint signal is emitted,
	  // then the old XML representation of its constraint is appended
	  // to this reconstrain command (done in the SLOT).
	  ConstrainedLine* from_reference =
	    dynamic_cast<ConstrainedLine*>( edge->reference() );

	  if ( from_reference != 0 ) {
	    from_ref_xml_ = xml_rep_->createElement( lC::STR::CONSTRAINED_LINE );
	    from_ref_xml_.setAttribute( lC::STR::URL, from_reference->dbURL().toString(true) );
	    from_reference->constraint()->write( from_ref_xml_ );

	    connect( from_reference, SIGNAL( modifiedConstraint() ),
		     SLOT( cascadeConstraints() ) );
	  }

	  edge->setConstraint( new Coincident( y_reference ) );

	  if ( from_reference != 0 )
	    disconnect( from_reference, SIGNAL( modifiedConstraint() ),
			this, SLOT( cascadeConstraints() ) );

	  QDomElement new_xml = xml_rep_->createElement( lC::STR::CONSTRAINED_LINE );
	  new_xml.setAttribute( lC::STR::URL, edge->dbURL().toString(true) );
	  edge->constraint()->write( new_xml );
	  xml_rep_->firstChild().firstChild().nextSibling().appendChild( new_xml );
	}
	else
	  cout << "y edge connection rejected for circular dependencies" << endl;
      }
    }

    if ( xml_rep_ != 0 ) {
      if ( command == 0 )
	command =
	  new MoveLinesCommand( "move/resize rectangle", rectangle_view_->model() );

      command->add( xml_rep_ );

      xml_rep_ = 0;
    }

    if ( command != 0 )
      CommandHistory::instance().addCommand( command );

    selection_type_ = SelectionType( TRANSIENT, FIGURE );

    rectangle_->setComplete( true );

    drag_handle_ = 0;
    // Might do a sanity check comparing to the original point (?)
    //  rectangle_view_->unsetCursor(); //
    // Continue in this mode until unselected
    return false;
  }

  void RectangleModifyInput::mouseDoubleClick ( QMouseEvent* /*me*/ )
  {
    rectangle_view_->editInformation();
  }

  void RectangleModifyInput::setRectangle ( Rectangle* rectangle )
  {
    rectangle_ = rectangle;
  }

  void RectangleModifyInput::cascadeConstraints ( void )
  {
    // The primary reconstraint target is buried deep in this representation.
    // This change has to go before it so it can be undone properly.
    xml_rep_->firstChild().firstChild().insertBefore( from_ref_xml_,
			      xml_rep_->firstChild().firstChild().firstChild() );
  }

  RectangleInfoDialog* RectangleView::rectangle_info_dialog_ = 0;

  RectangleView::RectangleView ( Rectangle* rectangle, PageView* parent )
    : FigureView( parent ), rectangle_( rectangle ),
      create_input_( rectangle, this ), modify_input_( rectangle, this ),
      x0_dimensionview_( 0 ), y0_dimensionview_( 0 ),
      x1_dimensionview_( 0 ), y1_dimensionview_( 0 ),
      width_annotation_side_( lC::ABOVE ), height_annotation_side_( lC::ABOVE ),
      style_( 0 )
  {
    init();
  }

  RectangleView::RectangleView ( const QDomElement& xml_rep, PageView* parent )
    : FigureView( parent ), rectangle_( 0 ),
      create_input_( 0, this ), modify_input_( 0, this ),
      x0_dimensionview_( 0 ), y0_dimensionview_( 0 ),
      x1_dimensionview_( 0 ), y1_dimensionview_( 0 ),
      width_annotation_side_( lC::ABOVE ), height_annotation_side_( lC::ABOVE ),
      style_( 0 )
  {
    DBURL db_url = xml_rep.attribute( lC::STR::RECTANGLE );

    rectangle_ = dynamic_cast< Rectangle* >( model()->lookup( db_url ) );

    init();

    // The view has to be alerted to any changes in constraints
    // on the edges of the rectangle since they can potentially gain,
    // lose or alter offset constraints at any time.
    connect( rectangle_->x0(), SIGNAL( modifiedConstraint() ),
	     SLOT( x0ModifiedConstraint() ) );
    connect( rectangle_->y0(), SIGNAL( modifiedConstraint() ),
	     SLOT( y0ModifiedConstraint() ) );
    connect( rectangle_->x1(), SIGNAL( modifiedConstraint() ),
	     SLOT( x1ModifiedConstraint() ) );
    connect( rectangle_->y1(), SIGNAL( modifiedConstraint() ),
	     SLOT( y1ModifiedConstraint() ) );

    connect( rectangle_->x0(), SIGNAL( modifiedPosition() ),
	     SLOT( x0ModifiedPosition() ) );
    connect( rectangle_->y0(), SIGNAL( modifiedPosition() ),
	     SLOT( y0ModifiedPosition() ) );
    connect( rectangle_->x1(), SIGNAL( modifiedPosition() ),
	     SLOT( x1ModifiedPosition() ) );
    connect( rectangle_->y1(), SIGNAL( modifiedPosition() ),
	     SLOT( y1ModifiedPosition() ) );

    // Next, there should be legitimate constraints on all the edges now
    list_view_item_->setText( lC::DETAIL,
			     trC( lC::STR::RECTANGLE_FMT ).
			     arg( rectangle_->origin()[X] ).
			     arg( rectangle_->origin()[Y] ).
			     arg( rectangle_->width() ).
			     arg( rectangle_->height() ) );
    x0_list_view_->setText( lC::DETAIL, rectangle_->x0()->detail() );
    y0_list_view_->setText( lC::DETAIL, rectangle_->y0()->detail() );
    x1_list_view_->setText( lC::DETAIL, rectangle_->x1()->detail() );
    y1_list_view_->setText( lC::DETAIL, rectangle_->y1()->detail() );

    width_annotation_side_ =
      lC::annotationSide( xml_rep.attribute( lC::STR::WIDTH_ANNOTATION_SIDE ) );
    height_annotation_side_ =
      lC::annotationSide( xml_rep.attribute( lC::STR::HEIGHT_ANNOTATION_SIDE ) );

    QDomNode n = xml_rep.firstChild();

    while ( !n.isNull() ) {
      QDomElement e = n.toElement();
      if ( !e.isNull() ) {
	if ( e.tagName() == lC::STR::STYLE ) {
	  style_ = new Style( e );
	}
	else if ( e.tagName() == lC::STR::ROLE ) {
	  QString role = e.attribute( lC::STR::NAME );

	  if ( role == lC::STR::X0 ) {
	    QDomNode dimension_view_node = e.firstChild();

	    if ( !dimension_view_node.isNull() ) {
	      QDomElement dimension_view_element = dimension_view_node.toElement();

	      if ( !dimension_view_element.isNull() &&
		   dimension_view_element.tagName() == lC::STR::DIMENSION_VIEW ) {
		x0_dimensionview_ = new DimensionView( dimension_view_element,
						      view()->genSelectionName(),
						      this );
		dimensionview_objects_.insert( x0_dimensionview_->selectionName(),
					       x0_dimensionview_ );

		connect( rectangle_->x0(), SIGNAL( modifiedPosition() ),
			 SLOT( x0DmVDimModified() ) );
		connect( x0_dimensionview_->fromReference(),
			 SIGNAL( modifiedPosition() ), SLOT( x0DmVRefModified() ) );
		connect( x0_dimensionview_->fromReference(),
			 SIGNAL( destroyedGeometry() ), SLOT( x0DestroyedDmVRef()));
		connect( x0_dimensionview_->toReference(),
			 SIGNAL( modifiedPosition() ), SLOT( x0DmVRefModified() ) );
		connect( x0_dimensionview_->toReference(),
			 SIGNAL( destroyedGeometry() ), SLOT( x0DestroyedDmVRef()));
	      }
	    }
	  }
	  else if ( role == lC::STR::Y0 ) {
	    QDomNode dimension_view_node = e.firstChild();

	    if ( !dimension_view_node.isNull() ) {
	      QDomElement dimension_view_element = dimension_view_node.toElement();

	      if ( !dimension_view_element.isNull() &&
		   dimension_view_element.tagName() == lC::STR::DIMENSION_VIEW ) {
		y0_dimensionview_ = new DimensionView( dimension_view_element,
						      view()->genSelectionName(),
						      this );
		dimensionview_objects_.insert( y0_dimensionview_->selectionName(),
					       y0_dimensionview_ );

		connect( rectangle_->y0(), SIGNAL( modifiedPosition() ),
			 SLOT( y0DmVDimModified() ) );
		connect( y0_dimensionview_->fromReference(),
			 SIGNAL( modifiedPosition() ), SLOT( y0DmVRefModified() ) );
		connect( y0_dimensionview_->fromReference(),
			 SIGNAL( destroyedGeometry() ), SLOT( y0DestroyedDmVRef()));
		connect( y0_dimensionview_->toReference(),
			 SIGNAL( modifiedPosition() ), SLOT( y0DmVRefModified() ) );
		connect( y0_dimensionview_->toReference(),
			 SIGNAL( destroyedGeometry() ), SLOT( y0DestroyedDmVRef()));
	      }
	    }
	  }
	  else if ( role == lC::STR::X1 ) {
	    QDomNode dimension_view_node = e.firstChild();

	    if ( !dimension_view_node.isNull() ) {
	      QDomElement dimension_view_element = dimension_view_node.toElement();

	      if ( !dimension_view_element.isNull() &&
		   dimension_view_element.tagName() == lC::STR::DIMENSION_VIEW ) {
		x1_dimensionview_ = new DimensionView( dimension_view_element,
						       view()->genSelectionName(),
							  this );
		dimensionview_objects_.insert( x1_dimensionview_->
					       selectionName(),
					       x1_dimensionview_ );

		connect( rectangle_->x1(), SIGNAL( modifiedPosition() ),
			 SLOT( x1DmVDimModified() ) );
		connect( x1_dimensionview_->fromReference(),
			 SIGNAL( modifiedPosition() ), SLOT( x1DmVRefModified() ) );
		connect( x1_dimensionview_->fromReference(),
			 SIGNAL( destroyedGeometry() ), SLOT( x1DestroyedDmVRef()));
		connect( x1_dimensionview_->toReference(),
			 SIGNAL( modifiedPosition() ), SLOT( x1DmVRefModified() ) );
		connect( x1_dimensionview_->toReference(),
			 SIGNAL( destroyedGeometry() ), SLOT( x1DestroyedDmVRef()));
	      }
	    }
	  }
	  else if ( role == lC::STR::Y1 ) {
	    QDomNode dimension_view_node = e.firstChild();

	    if ( !dimension_view_node.isNull() ) {
	      QDomElement dimension_view_element = dimension_view_node.toElement();

	      if ( !dimension_view_element.isNull() &&
		   dimension_view_element.tagName() == lC::STR::DIMENSION_VIEW ) {
		y1_dimensionview_ = new DimensionView( dimension_view_element,
						       view()->genSelectionName(),
							   this );
		dimensionview_objects_.insert( y1_dimensionview_->
					       selectionName(),
					       y1_dimensionview_ );

		connect( rectangle_->y1(), SIGNAL( modifiedPosition() ),
			 SLOT( y1DmVDimModified() ) );
		connect( y1_dimensionview_->fromReference(),
			 SIGNAL( modifiedPosition() ), SLOT( y1DmVRefModified() ) );
		connect( y1_dimensionview_->fromReference(),
			 SIGNAL( destroyedGeometry() ), SLOT( y1DestroyedDmVRef()));
		connect( y1_dimensionview_->toReference(),
			 SIGNAL( modifiedPosition() ), SLOT( y1DmVRefModified() ) );
		connect( y1_dimensionview_->toReference(),
			 SIGNAL( destroyedGeometry() ), SLOT( y1DestroyedDmVRef()));
	      }
	    }
	  }
	}
      }
      n = n.nextSibling();
    }

    modify_input_.setRectangle( rectangle_ );
  }

  RectangleView::RectangleView ( const QString& name, const QDomElement& xml_rep,
				 Page* page, PageView* parent )
    : FigureView( parent ), rectangle_( 0 ),
      create_input_( 0, this ), modify_input_( 0, this ),
      x0_dimensionview_( 0 ), y0_dimensionview_( 0 ),
      x1_dimensionview_( 0 ), y1_dimensionview_( 0 ),
      width_annotation_side_( lC::ABOVE ), height_annotation_side_( lC::ABOVE ),
      style_( 0 )
  {
    QDomDocument doc = xml_rep.ownerDocument();
    QDomNodeList rectangle_list = doc.elementsByTagName( lC::STR::RECTANGLE );
    // Well, failure is not really an option...but it could happen if
    // somebody put a partial XML document into the clipboard...
    // In general, the failure modes of lignumCAD need substantially more
    // consideration......
    if ( rectangle_list.count() > 0 )
      rectangle_ = new Rectangle( page->newID(), name,
				  rectangle_list.item(0).toElement(), page );

    init();

    // The view has to be alerted to any changes in constraints
    // on the edges of the rectangle since they can potentially gain,
    // lose or alter offset constraints at any time.
    connect( rectangle_->x0(), SIGNAL( modifiedConstraint() ),
	     SLOT( x0ModifiedConstraint() ) );
    connect( rectangle_->y0(), SIGNAL( modifiedConstraint() ),
	     SLOT( y0ModifiedConstraint() ) );
    connect( rectangle_->x1(), SIGNAL( modifiedConstraint() ),
	     SLOT( x1ModifiedConstraint() ) );
    connect( rectangle_->y1(), SIGNAL( modifiedConstraint() ),
	     SLOT( y1ModifiedConstraint() ) );

    connect( rectangle_->x0(), SIGNAL( modifiedPosition() ),
	     SLOT( x0ModifiedPosition() ) );
    connect( rectangle_->y0(), SIGNAL( modifiedPosition() ),
	     SLOT( y0ModifiedPosition() ) );
    connect( rectangle_->x1(), SIGNAL( modifiedPosition() ),
	     SLOT( x1ModifiedPosition() ) );
    connect( rectangle_->y1(), SIGNAL( modifiedPosition() ),
	     SLOT( y1ModifiedPosition() ) );

    // Next, there should be legitimate constraints on all the edges now
    list_view_item_->setText( lC::DETAIL,
			     trC( lC::STR::RECTANGLE_FMT ).
			     arg( rectangle_->origin()[X] ).
			     arg( rectangle_->origin()[Y] ).
			     arg( rectangle_->width() ).
			     arg( rectangle_->height() ) );
    x0_list_view_->setText( lC::DETAIL, rectangle_->x0()->detail() );
    y0_list_view_->setText( lC::DETAIL, rectangle_->y0()->detail() );
    x1_list_view_->setText( lC::DETAIL, rectangle_->x1()->detail() );
    y1_list_view_->setText( lC::DETAIL, rectangle_->y1()->detail() );

    width_annotation_side_ =
      lC::annotationSide( xml_rep.attribute( lC::STR::WIDTH_ANNOTATION_SIDE ) );
    height_annotation_side_ =
      lC::annotationSide( xml_rep.attribute( lC::STR::HEIGHT_ANNOTATION_SIDE ) );

    QDomNode n = xml_rep.firstChild();

    while ( !n.isNull() ) {
      QDomElement e = n.toElement();
      if ( !e.isNull() ) {
	if ( e.tagName() == lC::STR::STYLE ) {
	  style_ = new Style;
	  style_->setStyle( lC::Render::style( e.attribute( lC::STR::STYLE ) ) );
	  style_->setEdge( lC::Edge::style( e.attribute( lC::STR::EDGE ) ) );
	  style_->setWireframeColor( e.attribute( lC::STR::WIREFRAME ) );
	  style_->setSolidColor( e.attribute( lC::STR::SOLID ) );
	  style_->setTextureFile( e.attribute( lC::STR::TEXTURE ) );
	}
      }
      n = n.nextSibling();
    }

    modify_input_.setRectangle( rectangle_ );

    rectangleInitialized();
  }


  RectangleView::~RectangleView ( void )
  {
    if ( style_ != 0 ) delete style_;
    // Yeah, we don't want to hear from these any more.

    if ( x0_dimensionview_ ) {
      disconnect( x0_dimensionview_->fromReference(), SIGNAL( modifiedPosition() ),
		  this, SLOT( x0DmVRefModified() ) );
      disconnect( x0_dimensionview_->fromReference(), SIGNAL( destroyedGeometry() ),
		  this, SLOT( x0DestroyedDmVRef() ) );
      disconnect( x0_dimensionview_->toReference(), SIGNAL( modifiedPosition() ),
		  this, SLOT( x0DmVRefModified() ) );
      disconnect( x0_dimensionview_->toReference(), SIGNAL( destroyedGeometry() ),
		  this, SLOT( x0DestroyedDmVRef() ) );

      delete x0_dimensionview_;
    }
    if ( y0_dimensionview_ ) {
      disconnect( y0_dimensionview_->fromReference(), SIGNAL( modifiedPosition() ),
		  this, SLOT( y0DmVRefModified() ) );
      disconnect( y0_dimensionview_->fromReference(), SIGNAL( destroyedGeometry() ),
		  this, SLOT( y0DestroyedDmVRef() ) );
      disconnect( y0_dimensionview_->toReference(), SIGNAL( modifiedPosition() ),
		  this, SLOT( y0DmVRefModified() ) );
      disconnect( y0_dimensionview_->toReference(), SIGNAL( destroyedGeometry() ),
		  this, SLOT( y0DestroyedDmVRef() ) );

      delete y0_dimensionview_;
    }
    if ( x1_dimensionview_ ) {
      disconnect( x1_dimensionview_->fromReference(), SIGNAL( modifiedPosition() ),
		  this, SLOT( x1DmVRefModified() ) );
      disconnect( x1_dimensionview_->fromReference(), SIGNAL( destroyedGeometry() ),
		  this, SLOT( x1DestroyedDmVRef() ) );
      disconnect( x1_dimensionview_->toReference(), SIGNAL( modifiedPosition() ),
		  this, SLOT( x1DmVRefModified() ) );
      disconnect( x1_dimensionview_->toReference(), SIGNAL( destroyedGeometry() ),
		  this, SLOT( x1DestroyedDmVRef() ) );

      delete x1_dimensionview_;
    }
    if ( y1_dimensionview_ ) {
      disconnect( y1_dimensionview_->toReference(), SIGNAL( modifiedPosition() ),
		  this, SLOT( y1DmVRefModified() ) );
      disconnect( y1_dimensionview_->fromReference(), SIGNAL( destroyedGeometry() ),
		  this, SLOT( y1DestroyedDmVRef() ) );
      disconnect( y1_dimensionview_->fromReference(), SIGNAL( modifiedPosition() ),
		  this, SLOT( y1DmVRefModified() ) );
      disconnect( y1_dimensionview_->toReference(), SIGNAL( destroyedGeometry() ),
		  this, SLOT( y1DestroyedDmVRef() ) );

      delete y1_dimensionview_;
    }

    if ( rectangle_ ) {
      disconnect( rectangle_->x0(), SIGNAL( modifiedPosition() ),
		  this, SLOT( x0DmVDimModified() ) );
      disconnect( rectangle_->y0(), SIGNAL( modifiedPosition() ),
		  this, SLOT( y0DmVDimModified() ) );
      disconnect( rectangle_->x1(), SIGNAL( modifiedPosition() ),
		  this, SLOT( x1DmVDimModified() ) );
      disconnect( rectangle_->y1(), SIGNAL( modifiedPosition() ),
		  this, SLOT( y1DmVDimModified() ) );

      disconnect( rectangle_->x0(), SIGNAL( modifiedConstraint() ),
		  this, SLOT( x0ModifiedConstraint() ) );
      disconnect( rectangle_->y0(), SIGNAL( modifiedConstraint() ),
		  this, SLOT( y0ModifiedConstraint() ) );
      disconnect( rectangle_->x1(), SIGNAL( modifiedConstraint() ),
		  this, SLOT( x1ModifiedConstraint() ) );
      disconnect( rectangle_->y1(), SIGNAL( modifiedConstraint() ),
		  this, SLOT( y1ModifiedConstraint() ) );

      disconnect( rectangle_->x0(), SIGNAL( modifiedPosition() ),
		  this, SLOT( x0ModifiedPosition() ) );
      disconnect( rectangle_->y0(), SIGNAL( modifiedPosition() ),
		  this, SLOT( y0ModifiedPosition() ) );
      disconnect( rectangle_->x1(), SIGNAL( modifiedPosition() ),
		  this, SLOT( x1ModifiedPosition() ) );
      disconnect( rectangle_->y1(), SIGNAL( modifiedPosition() ),
		  this, SLOT( y1ModifiedPosition() ) );

      rectangle_->parent()->removeFigure( rectangle_ );
    }

    delete list_view_item_;
  }

  /*
   * Decide whether the rectangle or the dimension is being
   * manipulated and return the responsible controller
   */
  InputObject* RectangleView::modifyInput ( void )
  {
    if ( isActivated() )
      return &modify_input_;

    QIntDictIterator< DimensionView > idmv( dimensionview_objects_ );

    for (; idmv.current(); ++idmv ) {
      if ( idmv.current()->isActivated() ) {
	return idmv.current()->modifyInput();
      }
    }

    return 0;
  }

  void RectangleView::init ( void )
  {
    QObject::setName( rectangle_->name() + lC::STR::VIEW_EXT );

    left_edge_.setSelectionName( view()->genSelectionName() );
    right_edge_.setSelectionName( view()->genSelectionName() );
    bottom_edge_.setSelectionName( view()->genSelectionName() );
    top_edge_.setSelectionName( view()->genSelectionName() );

    left_edge_.setCursorShape( Qt::SizeAllCursor );
    right_edge_.setCursorShape( Qt::SizeAllCursor );
    bottom_edge_.setCursorShape( Qt::SizeAllCursor );
    top_edge_.setCursorShape( Qt::SizeAllCursor );

    solid_rectangle_.setSelectionName( view()->genSelectionName() );

    solid_rectangle_.setCursorShape( Qt::SizeAllCursor );

    lb_handle_.setSelectionName( view()->genSelectionName() );
    mb_handle_.setSelectionName( view()->genSelectionName() );
    rb_handle_.setSelectionName( view()->genSelectionName() );
    mr_handle_.setSelectionName( view()->genSelectionName() );
    rt_handle_.setSelectionName( view()->genSelectionName() );
    mt_handle_.setSelectionName( view()->genSelectionName() );
    lt_handle_.setSelectionName( view()->genSelectionName() );
    ml_handle_.setSelectionName( view()->genSelectionName() );

    lb_handle_.setCursorShape( Qt::SizeBDiagCursor );
    mb_handle_.setCursorShape( Qt::SizeVerCursor );
    rb_handle_.setCursorShape( Qt::SizeFDiagCursor );
    mr_handle_.setCursorShape( Qt::SizeHorCursor );
    rt_handle_.setCursorShape( Qt::SizeBDiagCursor );
    mt_handle_.setCursorShape( Qt::SizeVerCursor );
    lt_handle_.setCursorShape( Qt::SizeFDiagCursor );
    ml_handle_.setCursorShape( Qt::SizeHorCursor );

    // Reverse lookup of GL selection name to object
    rectangle_objects_.insert( left_edge_.selectionName(), &left_edge_ );
    rectangle_objects_.insert( right_edge_.selectionName(), &right_edge_ );
    rectangle_objects_.insert( bottom_edge_.selectionName(), &bottom_edge_ );
    rectangle_objects_.insert( top_edge_.selectionName(), &top_edge_ );
    rectangle_objects_.insert( solid_rectangle_.selectionName(),
			     &solid_rectangle_ );
    rectangle_objects_.insert( lb_handle_.selectionName(), &lb_handle_ );
    rectangle_objects_.insert( mb_handle_.selectionName(), &mb_handle_ );
    rectangle_objects_.insert( rb_handle_.selectionName(), &rb_handle_ );
    rectangle_objects_.insert( mr_handle_.selectionName(), &mr_handle_ );
    rectangle_objects_.insert( rt_handle_.selectionName(), &rt_handle_ );
    rectangle_objects_.insert( mt_handle_.selectionName(), &mt_handle_ );
    rectangle_objects_.insert( lt_handle_.selectionName(), &lt_handle_ );
    rectangle_objects_.insert( ml_handle_.selectionName(), &ml_handle_ );

    adjustments_[left_edge_.selectionName()] = &Rectangle::move;
    adjustments_[right_edge_.selectionName()] = &Rectangle::move;
    adjustments_[bottom_edge_.selectionName()] = &Rectangle::move;
    adjustments_[top_edge_.selectionName()] = &Rectangle::move;

    adjustments_[solid_rectangle_.selectionName()] = &Rectangle::move;

    adjustments_[ml_handle_.selectionName()] = &Rectangle::resizeLeft;
    adjustments_[mr_handle_.selectionName()] = &Rectangle::resizeRight;
    adjustments_[mb_handle_.selectionName()] = &Rectangle::resizeBottom;
    adjustments_[mt_handle_.selectionName()] = &Rectangle::resizeTop;

    adjustments_[lb_handle_.selectionName()] = &Rectangle::resizeLeftBottom;
    adjustments_[rb_handle_.selectionName()] = &Rectangle::resizeRightBottom;
    adjustments_[rt_handle_.selectionName()] = &Rectangle::resizeRightTop;
    adjustments_[lt_handle_.selectionName()] = &Rectangle::resizeLeftTop;

    if ( rectangle_info_dialog_ == 0 )
      rectangle_info_dialog_ = new RectangleInfoDialog( parent()->lCMW() );

    QListViewItem* previous_item = parent()->previousItem( parent()->listViewItem(),
							   rectangle_->id() );

    list_view_item_ = new ListViewItem( parent()->listViewItem(), previous_item );

#if 0
    list_view_item_->setText( lC::NAME, lC::STR::NAME_ID.
			     arg( rectangle_->name() ).arg( rectangle_->id() ) );
#else
  list_view_item_->setText( lC::NAME, lC::formatName( rectangle_->name() )
			    + QString( " <%1>" ).arg( rectangle_->id() ) );
#endif
    list_view_item_->setText( lC::TYPE, trC( lC::STR::RECTANGLE ) );
    list_view_item_->setText( lC::DETAIL, trC( lC::STR::UNDEFINED ) );
    list_view_item_->setOpen( true );
    list_view_item_->setRenameEnabled( lC::NAME, true );

    x0_list_view_ = new ListViewItem( list_view_item_, 0 );
    x0_list_view_->setText( lC::NAME, lC::formatName( rectangle_->x0()->name() ) );
    x0_list_view_->setText( lC::TYPE, trC( lC::STR::CONSTRAINED_LINE ) );
    x0_list_view_->setText( lC::DETAIL, trC( lC::STR::CONSTRAINT_UNDEFINED ) );
    x0_list_view_->setOpen( true );
    x0_list_view_->setRenameEnabled( lC::NAME, true );

    y0_list_view_ = new ListViewItem( list_view_item_, x0_list_view_ );
    y0_list_view_->setText( lC::NAME, lC::formatName( rectangle_->y0()->name() ) );
    y0_list_view_->setText( lC::TYPE, trC( lC::STR::CONSTRAINED_LINE ) );
    y0_list_view_->setText( lC::DETAIL, trC( lC::STR::CONSTRAINT_UNDEFINED ) );
    y0_list_view_->setOpen( true );
    y0_list_view_->setRenameEnabled( lC::NAME, true );

    x1_list_view_ = new ListViewItem( list_view_item_, y0_list_view_ );
    x1_list_view_->setText( lC::NAME, lC::formatName( rectangle_->x1()->name() ) );
    x1_list_view_->setText( lC::TYPE, trC( lC::STR::CONSTRAINED_LINE ) );
    x1_list_view_->setText( lC::DETAIL, trC( lC::STR::CONSTRAINT_UNDEFINED ) );
    x1_list_view_->setOpen( true );
    x1_list_view_->setRenameEnabled( lC::NAME, true );

    y1_list_view_ = new ListViewItem( list_view_item_, x1_list_view_ );
    y1_list_view_->setText( lC::NAME, lC::formatName( rectangle_->y1()->name() ) );
    y1_list_view_->setText( lC::TYPE, trC( lC::STR::CONSTRAINED_LINE ) );
    y1_list_view_->setText( lC::DETAIL, trC( lC::STR::CONSTRAINT_UNDEFINED ) );
    y1_list_view_->setOpen( true );
    y1_list_view_->setRenameEnabled( lC::NAME, true );

    y1_list_view_->listView()->ensureItemVisible( y1_list_view_ );

    connect( list_view_item_, SIGNAL( nameChanged( const QString& ) ),
	     SLOT( listNameChanged( const QString& ) ) );

    connect( x0_list_view_, SIGNAL( nameChanged( const QString& ) ),
	     SLOT( x0SetName( const QString& ) ) );
    connect( y0_list_view_, SIGNAL( nameChanged( const QString& ) ),
	     SLOT( y0SetName( const QString& ) ) );
    connect( x1_list_view_, SIGNAL( nameChanged( const QString& ) ),
	     SLOT( x1SetName( const QString& ) ) );
    connect( y1_list_view_, SIGNAL( nameChanged( const QString& ) ),
	     SLOT( y1SetName( const QString& ) ) );

    connect( rectangle_, SIGNAL( initialized() ), SLOT( rectangleInitialized()));
    connect( rectangle_, SIGNAL( moved() ), SLOT( rectangleMoved() ) );
    connect( rectangle_, SIGNAL( resized() ), SLOT( rectangleResized() ) );

    connect( rectangle_, SIGNAL( nameChanged( const QString& ) ),
	     SLOT( updateName( const QString& ) ) );

    connect( rectangle_->x0(), SIGNAL( nameChanged( const QString& ) ),
	     SLOT( x0UpdateName( const QString& ) ) );
    connect( rectangle_->y0(), SIGNAL( nameChanged( const QString& ) ),
	     SLOT( y0UpdateName( const QString& ) ) );
    connect( rectangle_->x1(), SIGNAL( nameChanged( const QString& ) ),
	     SLOT( x1UpdateName( const QString& ) ) );
    connect( rectangle_->y1(), SIGNAL( nameChanged( const QString& ) ),
	     SLOT( y1UpdateName( const QString& ) ) );
  }

  CreateObject* RectangleView::memento ( void )
  {
    return new RectangleViewCreate( this );
  }

  /*!
   * Update the dimension view to have the given properties.
   */
  void RectangleView::setDimensionView ( const QDomElement& xml_rep )
  {
    DimensionView* dmv = new DimensionView( xml_rep, view()->genSelectionName(),
					    this );

    if ( dmv->dimension() == rectangle_->x0() ) {
      if ( x0_dimensionview_ != 0 ) {

	disconnect( x0_dimensionview_->dimension(), SIGNAL( modifiedPosition() ),
		    this, SLOT( x0DmVDimModified() ) );
	disconnect( x0_dimensionview_->fromReference(), SIGNAL( modifiedPosition()),
		    this, SLOT( x0DmVRefModified() ) );
	disconnect( x0_dimensionview_->toReference(), SIGNAL( modifiedPosition() ),
		    this, SLOT( x0DmVRefModified() ) );
	disconnect( x0_dimensionview_->fromReference(), SIGNAL(destroyedGeometry()),
		    this, SLOT( x0DestroyedDmVRef() ) );
	disconnect( x0_dimensionview_->toReference(), SIGNAL(destroyedGeometry()),
		    this, SLOT( x0DestroyedDmVRef() ) );

	dimensionview_objects_.remove( x0_dimensionview_->selectionName() );

	delete x0_dimensionview_;
      }

      x0_dimensionview_ = dmv;

      dimensionview_objects_.insert( x0_dimensionview_->selectionName(),
				     x0_dimensionview_ );

      connect( rectangle_->x0(), SIGNAL( modifiedPosition() ),
	       SLOT( x0DmVDimModified() ) );
      connect( x0_dimensionview_->fromReference(), SIGNAL( modifiedPosition() ),
	       SLOT( x0DmVRefModified() ) );
      connect( x0_dimensionview_->toReference(), SIGNAL( modifiedPosition() ),
	       SLOT( x0DmVRefModified() ) );
      connect( x0_dimensionview_->fromReference(), SIGNAL( destroyedGeometry() ),
	       SLOT( x0DestroyedDmVRef() ) );
      connect( x0_dimensionview_->toReference(), SIGNAL( destroyedGeometry() ),
	       SLOT( x0DestroyedDmVRef() ) );
    }
    else if ( dmv->dimension() == rectangle_->y0() ) {
      if ( y0_dimensionview_ != 0 ) {

	disconnect( y0_dimensionview_->dimension(), SIGNAL( modifiedPosition() ),
		    this, SLOT( y0DmVDimModified() ) );
	disconnect( y0_dimensionview_->fromReference(), SIGNAL( modifiedPosition()),
		    this, SLOT( y0DmVRefModified() ) );
	disconnect( y0_dimensionview_->toReference(), SIGNAL( modifiedPosition() ),
		    this, SLOT( y0DmVRefModified() ) );
	disconnect( y0_dimensionview_->fromReference(), SIGNAL(destroyedGeometry()),
		    this, SLOT( y0DestroyedDmVRef() ) );
	disconnect( y0_dimensionview_->toReference(), SIGNAL(destroyedGeometry()),
		    this, SLOT( y0DestroyedDmVRef() ) );

	dimensionview_objects_.remove( y0_dimensionview_->selectionName() );

	delete y0_dimensionview_;
      }

      y0_dimensionview_ = dmv;

      dimensionview_objects_.insert( y0_dimensionview_->selectionName(),
				     y0_dimensionview_ );

      connect( rectangle_->y0(), SIGNAL( modifiedPosition() ),
	       SLOT( y0DmVDimModified() ) );
      connect( y0_dimensionview_->fromReference(), SIGNAL( modifiedPosition() ),
	       SLOT( y0DmVRefModified() ) );
      connect( y0_dimensionview_->toReference(), SIGNAL( modifiedPosition() ),
	       SLOT( y0DmVRefModified() ) );
      connect( y0_dimensionview_->fromReference(), SIGNAL( destroyedGeometry() ),
	       SLOT( y0DestroyedDmVRef() ) );
      connect( y0_dimensionview_->toReference(), SIGNAL( destroyedGeometry() ),
	       SLOT( y0DestroyedDmVRef() ) );
    }
    else if ( dmv->dimension() == rectangle_->x1() ) {
      if ( x1_dimensionview_ != 0 ) {

	disconnect( x1_dimensionview_->dimension(), SIGNAL( modifiedPosition() ),
		    this, SLOT( x1DmVDimModified() ) );
	disconnect( x1_dimensionview_->fromReference(), SIGNAL( modifiedPosition()),
		    this, SLOT( x1DmVRefModified() ) );
	disconnect( x1_dimensionview_->toReference(), SIGNAL( modifiedPosition() ),
		    this, SLOT( x1DmVRefModified() ) );
	disconnect( x1_dimensionview_->fromReference(), SIGNAL(destroyedGeometry()),
		    this, SLOT( x1DestroyedDmVRef() ) );
	disconnect( x1_dimensionview_->toReference(), SIGNAL(destroyedGeometry()),
		    this, SLOT( x1DestroyedDmVRef() ) );

	dimensionview_objects_.remove( x1_dimensionview_->selectionName() );

	delete x1_dimensionview_;
      }

      x1_dimensionview_ = dmv;

      dimensionview_objects_.insert( x1_dimensionview_->selectionName(),
				     x1_dimensionview_ );

      connect( rectangle_->x1(), SIGNAL( modifiedPosition() ),
	       SLOT( x1DmVDimModified() ) );
      connect( x1_dimensionview_->fromReference(), SIGNAL( modifiedPosition() ),
	       SLOT( x1DmVRefModified() ) );
      connect( x1_dimensionview_->toReference(), SIGNAL( modifiedPosition() ),
	       SLOT( x1DmVRefModified() ) );
      connect( x1_dimensionview_->fromReference(), SIGNAL( destroyedGeometry() ),
	       SLOT( x1DestroyedDmVRef() ) );
      connect( x1_dimensionview_->toReference(), SIGNAL( destroyedGeometry() ),
	       SLOT( x1DestroyedDmVRef() ) );
    }
    else if ( dmv->dimension() == rectangle_->y1() ) {
      if ( y1_dimensionview_ != 0 ) {

	disconnect( y1_dimensionview_->dimension(), SIGNAL( modifiedPosition() ),
		    this, SLOT( y1DmVDimModified() ) );
	disconnect( y1_dimensionview_->fromReference(), SIGNAL( modifiedPosition()),
		    this, SLOT( y1DmVRefModified() ) );
	disconnect( y1_dimensionview_->toReference(), SIGNAL( modifiedPosition() ),
		    this, SLOT( y1DmVRefModified() ) );
	disconnect( y1_dimensionview_->fromReference(), SIGNAL(destroyedGeometry()),
		    this, SLOT( y1DestroyedDmVRef() ) );
	disconnect( y1_dimensionview_->toReference(), SIGNAL(destroyedGeometry()),
		    this, SLOT( y1DestroyedDmVRef() ) );

	dimensionview_objects_.remove( y1_dimensionview_->selectionName() );

	delete y1_dimensionview_;
      }

      y1_dimensionview_ = dmv;

      dimensionview_objects_.insert( y1_dimensionview_->selectionName(),
				     y1_dimensionview_ );

      connect( rectangle_->y1(), SIGNAL( modifiedPosition() ),
	       SLOT( y1DmVDimModified() ) );
      connect( y1_dimensionview_->fromReference(), SIGNAL( modifiedPosition() ),
	       SLOT( y1DmVRefModified() ) );
      connect( y1_dimensionview_->toReference(), SIGNAL( modifiedPosition() ),
	       SLOT( y1DmVRefModified() ) );
      connect( y1_dimensionview_->fromReference(), SIGNAL( destroyedGeometry() ),
	       SLOT( y1DestroyedDmVRef() ) );
      connect( y1_dimensionview_->toReference(), SIGNAL( destroyedGeometry() ),
	       SLOT( y1DestroyedDmVRef() ) );
    }
    else {
      cout << "Yikes! doesn't belong to anybody!!!" << endl;
      delete dmv;
    }
  }

  /*!
   * Canvas the graphics objects and try to identify this selection
   */
  QString RectangleView::nameOf ( GLuint selection_name ) const
  {
    if ( selection_name == left_edge_.selectionName() )
      return rectangle_->left()->name();
    else if ( selection_name == right_edge_.selectionName() )
      return rectangle_->right()->name();
    else if ( selection_name == bottom_edge_.selectionName() )
      return rectangle_->bottom()->name();
    else if ( selection_name == top_edge_.selectionName() )
      return rectangle_->top()->name();
    else if ( selection_name == solid_rectangle_.selectionName() )
      return QString::null;
    else if ( selection_name == lb_handle_.selectionName() )
      return QString( "the left bottom handle" );
    else if ( selection_name == mb_handle_.selectionName() )
      return QString( "the middle bottom handle" );
    else if ( selection_name == rb_handle_.selectionName() )
      return QString( "the right bottom handle" );
    else if ( selection_name == mr_handle_.selectionName() )
      return QString( "the middle right handle" );
    else if ( selection_name == rt_handle_.selectionName() )
      return QString( "the right top handle" );
    else if ( selection_name == mt_handle_.selectionName() )
      return QString( "the middle top handle" );
    else if ( selection_name == lt_handle_.selectionName() )
      return QString( "the left top handle" );
    else if ( selection_name == ml_handle_.selectionName() )
      return QString( "the middle left handle" );
    return QString( "unknow geometry in " ) + rectangle_->name();
  }
  // Implementation of FigureView interface
  uint RectangleView::id ( void ) const
  {
    return rectangle_->id();
  }

  QString RectangleView::name ( void ) const
  {
    return rectangle_->name();
  }

  DBURL RectangleView::dbURL ( void ) const
  {
    return rectangle_->dbURL();
  }

  QString RectangleView::selectionText ( const vector<GLuint>& selection_name,
					 SelectionEntity entity ) const
  {
    QString text;

    switch ( entity ) {
    case FIGURE:
      text = rectangle_->name(); break;
    case EDGE:
      text = lC::formatName( rectangle_->name() ) + '/' +
	lC::formatName( nameOf( selection_name[1] ) );
    }

    return text;
  }

  QString RectangleView::type ( void ) const
  {
    return lC::STR::RECTANGLE;
  }
  // This would seem to be an implicit look up of the dimension view.
  // Is there anything else this could be asked about?
  View* RectangleView::lookup ( QStringList& path_components ) const
  {
    int dot_pos = path_components.front().findRev( '.' );
    QString name = path_components.front().left( dot_pos );
    QString type = path_components.front().right( path_components.front().length()
						  - dot_pos - 1 );

    if ( x0_dimensionview_ != 0  && name == rectangle_->x0()->name() &&
	 type == rectangle_->x0()->type() )
      return x0_dimensionview_;

    else if ( y0_dimensionview_ != 0 && name == rectangle_->y0()->name() &&
	      type == rectangle_->y0()->type() )
      return y0_dimensionview_;

    else if ( x1_dimensionview_ != 0 && name == rectangle_->x1()->name() &&
	      type == rectangle_->x1()->type() )
      return x1_dimensionview_;

    else if ( y1_dimensionview_ != 0 && name == rectangle_->y1()->name() &&
	      type == rectangle_->y1()->type() )
      return y1_dimensionview_;

    return 0;
  }


  void RectangleView::setHighlighted ( bool highlight, SelectionEntity entity,
				       const vector<GLuint>& items )
  {
    if ( entity == FIGURE ) {
      DimensionView* dmv = 0;

      if ( items.size() > 1 )
	dmv = dimensionview_objects_[ items[1] ];

      if ( dmv != 0 )
	dmv->setHighlighted( highlight );
      else
	FigureViewBase::setHighlighted( highlight );
    }
    else if ( entity == EDGE ) {
      if ( items.size() > 1 && rectangle_objects_[items[1]] != 0 )
	rectangle_objects_[items[1]]->setHighlighted( highlight );
    }
  }

  void RectangleView::setActivated ( bool activate, SelectionEntity entity,
				     const vector<GLuint>& items )
  {
    if ( entity == FIGURE ) {
      DimensionView* dmv = 0;

      if ( items.size() > 1 )
	dmv = dimensionview_objects_[ items[1] ];

      if ( dmv != 0 )
	dmv->setActivated( activate );
      else
	FigureViewBase::setActivated( activate );
    }
    else if ( entity == EDGE ) {
      if ( items.size() > 1 && rectangle_objects_[items[1]] != 0 )
	rectangle_objects_[items[1]]->setActivated( activate );
    }
  }

  bool RectangleView::isActivated ( GLuint selection_name ) const
  {
    DimensionView* dmv;

    dmv = dimensionview_objects_[ selection_name ];

    if ( dmv != 0 )
      return dmv->isActivated();

    return FigureViewBase::isActivated();
  }
  /*!
   * Determine if this selection is part of the geometry representation;
   * this includes the handles if they happen to be highlighted, too.
   * The purpose here is really just to distinguish between the
   * lines and the dimension views.
   */
  bool RectangleView::isGeometry ( GLuint selection_name ) const
  {
    QIntDictIterator< GraphicsView > igv( rectangle_objects_ );

    for (; igv.current(); ++igv )
      if ( (GLuint)igv.currentKey() == selection_name ) return true;

    return false;
  }

  Curve* RectangleView::geometry ( GLuint selection_name ) const
  {
    // Now here's a problem: the graphical representation of the
    // rectangle is just a bunch of rendered lines. The actual edges
    // (OffsetLines) of the rectangle don't really have a one-to-one
    // correspondence with the graphics.  So, we have to perform some
    // kind of scan...  rectangle_objects_ relates the OpenGL selection
    // name to an edge and maybe Rectangle will tell us which line is
    // associated with a particular edge.

    if ( selection_name == left_edge_.selectionName() )
      return rectangle_->left();
    else if ( selection_name == right_edge_.selectionName() )
      return rectangle_->right();
    else if ( selection_name == bottom_edge_.selectionName() )
      return rectangle_->bottom();
    else if ( selection_name == top_edge_.selectionName() )
      return rectangle_->top();

    return 0;
  }

  FigureBase* RectangleView::figure ( void ) const
  {
    return rectangle_;
  }

  /*!
   * Which edges are involved with the drag resizing?
   */
  QPtrList< ConstrainedLine > RectangleView::dragEdges ( GLuint selection_name ) const
  {
    QPtrList< ConstrainedLine > edges;

    if ( selection_name == lb_handle_.selectionName() ) {
      edges.append( rectangle_->left() );
      edges.append( rectangle_->bottom() );
    }
    else if ( selection_name == mb_handle_.selectionName() ) {
      edges.append( rectangle_->bottom() );
    }
    else if ( selection_name == rb_handle_.selectionName() ) {
      edges.append( rectangle_->right() );
      edges.append( rectangle_->bottom() );
    }
    else if ( selection_name == mr_handle_.selectionName() ) {
      edges.append( rectangle_->right() );
    }
    else if ( selection_name == rt_handle_.selectionName() ) {
      edges.append( rectangle_->right() );
      edges.append( rectangle_->top() );
    }
    else if ( selection_name == mt_handle_.selectionName() ) {
      edges.append( rectangle_->top() );
    }
    else if ( selection_name == lt_handle_.selectionName() ) {
      edges.append( rectangle_->left() );
      edges.append( rectangle_->top() );
    }
    else if ( selection_name == ml_handle_.selectionName() ) {
      edges.append( rectangle_->left() );
    }
    else if ( selection_name == left_edge_.selectionName() ) {
      edges.append( rectangle_->left() );
    }
    else if ( selection_name == right_edge_.selectionName() ) {
      edges.append( rectangle_->right() );
    }
    else if ( selection_name == bottom_edge_.selectionName() ) {
      edges.append( rectangle_->bottom() );
    }
    else if ( selection_name == top_edge_.selectionName() ) {
      edges.append( rectangle_->top() );
    }
    return edges;
  }

  void RectangleView::editInformation ( void )
  {
    if ( FigureViewBase::isActivated() ) {
      editRectangleInformation();
    }
    else {
      QIntDictIterator< DimensionView >	idmv( dimensionview_objects_ );

      for ( ; idmv.current(); ++idmv ) {
	if ( idmv.current()->isActivated() ) {
	  idmv.current()->editInformation();
	  return;
	}
      }
    }
  }

  void RectangleView::editRectangleInformation ( void )
  {
    rectangle_info_dialog_->nameEdit->
      setText( lC::formatName( rectangle_->name() ) );

    // If width is a meaningful dimension for this rectangle, allow it
    // to be adjusted. Here, "meaningful" means that either x0's or x1's
    // offset describes the width of the rectangle.
    double width = 0;
    ConstrainedLine* width_constraint = 0;

    if ( rectangle_->x1()->isAdjustable() &&
	 rectangle_->x1()->reference() == rectangle_->x0() ) {
      width_constraint = rectangle_->x1();
      width = fabs( rectangle_->x1()->offset() );
    }

    else if ( rectangle_->x0()->isAdjustable() &&
	      rectangle_->x0()->reference() == rectangle_->x1() ) {
      width_constraint = rectangle_->x0();
      width = fabs( rectangle_->x0()->offset() );
    }

    else {
      width = fabs( rectangle_->width() );
    }

    if ( width_constraint != 0 ) {
      rectangle_info_dialog_->widthButtonGroup->setEnabled( true );
      rectangle_info_dialog_->widthSpecifiedRadioButton->setChecked( true );
    }
    else {
      rectangle_info_dialog_->widthButtonGroup->setEnabled( false );
    }

    rectangle_info_dialog_->widthLengthSpinBox->
      setLengthLimits( UnitsBasis::instance()->lengthUnit(),
		       UnitsBasis::instance()->format(),
		       UnitsBasis::instance()->precision(),
		       lC::MINIMUM_DIMESION, lC::MAXIMUM_DIMENSION, width );
    rectangle_info_dialog_->widthLengthSpinBox->setLength( width );

    // For now, this operation is not allowed...
    rectangle_info_dialog_->widthImportedRadioButton->setEnabled( false );
    rectangle_info_dialog_->widthConstraintChooser->setEnabled( false );

    // If height is a meaningful dimension for this rectangle, allow it
    // to be adjusted. Here, "meaningful" means that either y0's or y1's
    // offset describes the height of the rectangle.
    double height = 0;
    ConstrainedLine* height_constraint = 0;

    if ( rectangle_->y1()->isAdjustable() &&
	 rectangle_->y1()->reference() == rectangle_->y0() ) {
      height_constraint = rectangle_->y1();
      height = fabs( rectangle_->y1()->offset() );
    }

    else if ( rectangle_->y0()->isAdjustable() &&
	      rectangle_->y0()->reference() == rectangle_->y1() ) {
      height_constraint = rectangle_->y0();
      height = fabs( rectangle_->y0()->offset() );
    }

    else {
      height = fabs( rectangle_->height() );
    }

    if ( height_constraint != 0 ) {
      rectangle_info_dialog_->heightButtonGroup->setEnabled( true );
      rectangle_info_dialog_->heightSpecifiedRadioButton->setChecked( true );
    }
    else {
      rectangle_info_dialog_->heightButtonGroup->setEnabled( false );
    }

    rectangle_info_dialog_->heightLengthSpinBox->
      setLengthLimits( UnitsBasis::instance()->lengthUnit(),
		       UnitsBasis::instance()->format(),
		       UnitsBasis::instance()->precision(),
		       lC::MINIMUM_DIMESION, lC::MAXIMUM_DIMENSION, height );
    rectangle_info_dialog_->heightLengthSpinBox->setLength( height );

    // For now, this operation is not allowed...
    rectangle_info_dialog_->heightImportedRadioButton->setEnabled( false );
    rectangle_info_dialog_->heightConstraintChooser->setEnabled( false );

    // If the x origin is a meaningful dimension for this rectangle, allow it
    // to be adjusted. Here, "meaningful" means that x0 is adjustable and
    // it is not the width constraint.
    double x = 0;
    ConstrainedLine* x_constraint = 0;

    if ( rectangle_->x0()->isAdjustable() && rectangle_->x0() != width_constraint ){
      x_constraint = rectangle_->x0();
      x = fabs( rectangle_->x0()->offset() );
    }
    else {
      x = fabs( rectangle_->origin()[X] );
    }

    if ( x_constraint != 0 ) {
      rectangle_info_dialog_->xOriginButtonGroup->setEnabled( true );
      rectangle_info_dialog_->xOriginSpecifiedRadioButton->setChecked( true );
    }
    else {
      rectangle_info_dialog_->xOriginButtonGroup->setEnabled( false );
    }

    rectangle_info_dialog_->xOriginReferenceLineEdit->
      setText( rectangle_->x0()->reference()->path() );
    rectangle_info_dialog_->xOriginOffsetLengthSpinBox->
      setLengthLimits( UnitsBasis::instance()->lengthUnit(),
		       UnitsBasis::instance()->format(),
		       UnitsBasis::instance()->precision(),
		       lC::MINIMUM_DIMESION, lC::MAXIMUM_DIMENSION, x );
    rectangle_info_dialog_->xOriginOffsetLengthSpinBox->setLength( x );

    // For now, this operation is not allowed...
    rectangle_info_dialog_->xOriginImportedRadioButton->setEnabled( false );
    rectangle_info_dialog_->xOriginConstraintChooser->setEnabled( false );

    // If the y origin is a meaningful dimension for this rectangle, allow it
    // to be adjusted. Here, "meaningful" means y0 is adjustable and it is
    // not the height constraint.
    double y = 0;
    ConstrainedLine* y_constraint = 0;

    if ( rectangle_->y0()->isAdjustable() && rectangle_->y0() != height_constraint){
      y_constraint = rectangle_->y0();
      y = fabs( rectangle_->y0()->offset() );
    }
    else {
      y = fabs( rectangle_->origin()[Y] );
    }

    if ( y_constraint != 0 ) {
      rectangle_info_dialog_->yOriginButtonGroup->setEnabled( true );
      rectangle_info_dialog_->yOriginSpecifiedRadioButton->setChecked( true );
    }
    else {
      rectangle_info_dialog_->yOriginButtonGroup->setEnabled( false );
    }

    rectangle_info_dialog_->yOriginReferenceLineEdit->
      setText( rectangle_->y0()->reference()->path() );
    rectangle_info_dialog_->yOriginOffsetLengthSpinBox->
      setLengthLimits( UnitsBasis::instance()->lengthUnit(),
		       UnitsBasis::instance()->format(),
		       UnitsBasis::instance()->precision(),
		       lC::MINIMUM_DIMESION, lC::MAXIMUM_DIMENSION, y );
    rectangle_info_dialog_->yOriginOffsetLengthSpinBox->setLength( y );

    // For now, this operation is not allowed...
    rectangle_info_dialog_->yOriginImportedRadioButton->setEnabled( false );
    rectangle_info_dialog_->yOriginConstraintChooser->setEnabled( false );

    switch ( width_annotation_side_  ) {
    case lC::ABOVE:
      rectangle_info_dialog_->aboveButton->setChecked( true );
      break;
    case lC::BELOW:
      rectangle_info_dialog_->belowButton->setChecked( true );
      break;
    }

    switch ( height_annotation_side_  ) {
    case lC::ABOVE:
      rectangle_info_dialog_->rightButton->setChecked( true );
      break;
    case lC::BELOW:
      rectangle_info_dialog_->leftButton->setChecked( true );
      break;
    }

    if ( style_ != 0 )
      rectangle_info_dialog_->rectangleStyleSlider->setValue( style_->style() );
    else
      rectangle_info_dialog_->rectangleStyleSlider->setValue( 0 );

    switch ( rectangle_info_dialog_->rectangleStyleSlider->value() ) {
    case lC::Render::PARENT:
      rectangle_info_dialog_->defaultStyleLabel->setEnabled( true );
      rectangle_info_dialog_->edgeStyleLabel->setEnabled( false );
      rectangle_info_dialog_->edgeStyleComboBox->setEnabled( false );
      rectangle_info_dialog_->wireframeStyleLabel->setEnabled( false );
      rectangle_info_dialog_->wireframeStyleColorChooser->setEnabled( false );
      rectangle_info_dialog_->solidStyleLabel->setEnabled( false );
      rectangle_info_dialog_->solidStyleColorChooser->setEnabled( false );
      rectangle_info_dialog_->textureStyleLabel->setEnabled( false );
      rectangle_info_dialog_->textureStyleFileChooser->setEnabled( false );

      rectangle_info_dialog_->edgeStyleComboBox->
	setDefaultValue( lC::Render::SOLID );
      rectangle_info_dialog_->edgeStyleComboBox->
	setValue( lC::Edge::SOLID );
      rectangle_info_dialog_->wireframeStyleColorChooser->
	setDefaultColor( OpenGLGlobals::instance()->geometryColor() );
      rectangle_info_dialog_->wireframeStyleColorChooser->
	setColor( OpenGLGlobals::instance()->geometryColor() );
      rectangle_info_dialog_->solidStyleColorChooser->
	setDefaultColor( OpenGLGlobals::instance()->geometryColor() );
      rectangle_info_dialog_->solidStyleColorChooser->
	setColor( OpenGLGlobals::instance()->geometryColor() );
      rectangle_info_dialog_->textureStyleFileChooser->
	setDefaultFileName( OpenGLGlobals::instance()->textureFile() );
      rectangle_info_dialog_->textureStyleFileChooser->
	setFileName( OpenGLGlobals::instance()->textureFile() );
      break;
    case lC::Render::STIPPLE:
      rectangle_info_dialog_->defaultStyleLabel->setEnabled( false );
      rectangle_info_dialog_->edgeStyleLabel->setEnabled( true );
      rectangle_info_dialog_->edgeStyleComboBox->setEnabled( true );
      rectangle_info_dialog_->wireframeStyleLabel->setEnabled( false );
      rectangle_info_dialog_->wireframeStyleColorChooser->setEnabled( false );
      rectangle_info_dialog_->solidStyleLabel->setEnabled( false );
      rectangle_info_dialog_->solidStyleColorChooser->setEnabled( false );
      rectangle_info_dialog_->textureStyleLabel->setEnabled( false );
      rectangle_info_dialog_->textureStyleFileChooser->setEnabled( false );

      rectangle_info_dialog_->edgeStyleComboBox->
	setDefaultValue( style_->edge() );
      rectangle_info_dialog_->edgeStyleComboBox->
	setValue( style_->edge() );
      rectangle_info_dialog_->wireframeStyleColorChooser->
	setDefaultColor( OpenGLGlobals::instance()->geometryColor() );
      rectangle_info_dialog_->wireframeStyleColorChooser->
	setColor( style_->wireframeColor() );
      rectangle_info_dialog_->solidStyleColorChooser->
	setDefaultColor( OpenGLGlobals::instance()->geometryColor() );
      rectangle_info_dialog_->solidStyleColorChooser->
	setColor( style_->solidColor() );
      rectangle_info_dialog_->textureStyleFileChooser->
	setDefaultFileName( OpenGLGlobals::instance()->textureFile() );
      rectangle_info_dialog_->textureStyleFileChooser->
	setFileName( style_->textureFile() );
      break;
    case lC::Render::WIREFRAME:
      rectangle_info_dialog_->defaultStyleLabel->setEnabled( false );
      rectangle_info_dialog_->edgeStyleLabel->setEnabled( true );
      rectangle_info_dialog_->edgeStyleComboBox->setEnabled( true );
      rectangle_info_dialog_->wireframeStyleLabel->setEnabled( true );
      rectangle_info_dialog_->wireframeStyleColorChooser->setEnabled( true );
      rectangle_info_dialog_->solidStyleLabel->setEnabled( false );
      rectangle_info_dialog_->solidStyleColorChooser->setEnabled( false );
      rectangle_info_dialog_->textureStyleLabel->setEnabled( false );
      rectangle_info_dialog_->textureStyleFileChooser->setEnabled( false );

      rectangle_info_dialog_->edgeStyleComboBox->
	setDefaultValue( style_->edge() );
      rectangle_info_dialog_->edgeStyleComboBox->
	setValue( style_->edge() );
      rectangle_info_dialog_->wireframeStyleColorChooser->
	setDefaultColor( style_->wireframeColor() );
      rectangle_info_dialog_->wireframeStyleColorChooser->
	setColor( style_->wireframeColor() );
      rectangle_info_dialog_->solidStyleColorChooser->
	setDefaultColor( OpenGLGlobals::instance()->geometryColor() );
      rectangle_info_dialog_->solidStyleColorChooser->
	setColor( style_->solidColor() );
      rectangle_info_dialog_->textureStyleFileChooser->
	setDefaultFileName( OpenGLGlobals::instance()->textureFile() );
      rectangle_info_dialog_->textureStyleFileChooser->
	setFileName( style_->textureFile() );
      break;
    case lC::Render::SOLID:
      rectangle_info_dialog_->defaultStyleLabel->setEnabled( false );
      rectangle_info_dialog_->edgeStyleLabel->setEnabled( true );
      rectangle_info_dialog_->edgeStyleComboBox->setEnabled( true );
      rectangle_info_dialog_->wireframeStyleLabel->setEnabled( true );
      rectangle_info_dialog_->wireframeStyleColorChooser->setEnabled( true );
      rectangle_info_dialog_->solidStyleLabel->setEnabled( true );
      rectangle_info_dialog_->solidStyleColorChooser->setEnabled( true );
      rectangle_info_dialog_->textureStyleLabel->setEnabled( false );
      rectangle_info_dialog_->textureStyleFileChooser->setEnabled( false );

      rectangle_info_dialog_->edgeStyleComboBox->
	setDefaultValue( style_->edge() );
      rectangle_info_dialog_->edgeStyleComboBox->
	setValue( style_->edge() );
      rectangle_info_dialog_->wireframeStyleColorChooser->
	setDefaultColor( style_->wireframeColor() );
      rectangle_info_dialog_->wireframeStyleColorChooser->
	setColor( style_->wireframeColor() );
      rectangle_info_dialog_->solidStyleColorChooser->
	setDefaultColor( style_->solidColor() );
      rectangle_info_dialog_->solidStyleColorChooser->
	setColor( style_->solidColor() );
      rectangle_info_dialog_->textureStyleFileChooser->
	setDefaultFileName( OpenGLGlobals::instance()->textureFile() );
      rectangle_info_dialog_->textureStyleFileChooser->
	setFileName( style_->textureFile() );
      break;
    case lC::Render::TEXTURED:
      rectangle_info_dialog_->defaultStyleLabel->setEnabled( false );
      rectangle_info_dialog_->edgeStyleLabel->setEnabled( true );
      rectangle_info_dialog_->edgeStyleComboBox->setEnabled( true );
      rectangle_info_dialog_->wireframeStyleLabel->setEnabled( true );
      rectangle_info_dialog_->wireframeStyleColorChooser->setEnabled( true );
      rectangle_info_dialog_->solidStyleLabel->setEnabled( true );
      rectangle_info_dialog_->solidStyleColorChooser->setEnabled( true );
      rectangle_info_dialog_->textureStyleLabel->setEnabled( true );
      rectangle_info_dialog_->textureStyleFileChooser->setEnabled( true );

      rectangle_info_dialog_->edgeStyleComboBox->
	setDefaultValue( style_->edge() );
      rectangle_info_dialog_->edgeStyleComboBox->
	setValue( style_->edge() );
      rectangle_info_dialog_->wireframeStyleColorChooser->
	setDefaultColor( style_->wireframeColor() );
      rectangle_info_dialog_->wireframeStyleColorChooser->
	setColor( style_->wireframeColor() );
      rectangle_info_dialog_->solidStyleColorChooser->
	setDefaultColor( style_->solidColor() );
      rectangle_info_dialog_->solidStyleColorChooser->
	setColor( style_->solidColor() );
      rectangle_info_dialog_->textureStyleFileChooser->
	setDefaultFileName( style_->textureFile() );
      rectangle_info_dialog_->textureStyleFileChooser->
	setFileName( style_->textureFile() );
      break;
    }

    // Kind of gets lost here in the noise
  redo:
    int ret = rectangle_info_dialog_->exec();

    if ( ret == QDialog::Rejected ) return;

    bool modified = false;
    bool dimension_modified = false;

    if ( rectangle_info_dialog_->nameEdit->edited() ) {
      // Pageview handles checking the name and putting up the error dialog
      // if necessary.
      int ret = parent()->uniqueFigureName( this,
					  rectangle_info_dialog_->nameEdit->text(),
					    rectangle_->type() );

      switch ( ret ) {
      case lC::Rejected:
	return;
      case lC::Redo:
	rectangle_info_dialog_->nameEdit->
	  setText( lC::formatName( rectangle_->name() ) );
	goto redo;
      }

      setName( rectangle_info_dialog_->nameEdit->text() );
      modified = true;
    }

    MoveLinesCommand* move_command = 0;

    if ( rectangle_info_dialog_->widthLengthSpinBox->edited() ) {
      double old_offset = width_constraint->offset();

      if ( move_command == 0 )
	move_command = new MoveLinesCommand( "move/resize rectangle", model() );

      if ( width_constraint->offset() >= 0 )
	width_constraint->
	  setOffset( rectangle_info_dialog_->widthLengthSpinBox->length() );
      else
	width_constraint->
	  setOffset( -rectangle_info_dialog_->widthLengthSpinBox->length() );

      move_command->add( width_constraint, old_offset );

      dimension_modified = true;
    }

    if ( rectangle_info_dialog_->heightLengthSpinBox->edited() ) {
      double old_offset = height_constraint->offset();

      if ( move_command == 0 )
	move_command = new MoveLinesCommand( "move/resize rectangle", model() );

      if ( height_constraint->offset() >= 0 )
	height_constraint->
	  setOffset( rectangle_info_dialog_->heightLengthSpinBox->length() );
      else
	height_constraint->
	  setOffset( -rectangle_info_dialog_->heightLengthSpinBox->length() );

      move_command->add( height_constraint, old_offset );

      dimension_modified = true;
    }

    if ( rectangle_info_dialog_->xOriginOffsetLengthSpinBox->edited() ) {
      double old_offset = rectangle_->x0()->offset();

      if ( move_command == 0 )
	move_command = new MoveLinesCommand( "move/resize rectangle", model() );

      if ( rectangle_->x0()->offset() >= 0 )
	rectangle_->x0()->
	  setOffset( rectangle_info_dialog_->xOriginOffsetLengthSpinBox->length() );
      else
	rectangle_->x0()->
	  setOffset( -rectangle_info_dialog_->xOriginOffsetLengthSpinBox->length() );

      move_command->add( rectangle_->x0(), old_offset );

      dimension_modified = true;
    }

    if ( rectangle_info_dialog_->yOriginOffsetLengthSpinBox->edited() ) {
      double old_offset = rectangle_->y0()->offset();

      if ( move_command == 0 )
	move_command = new MoveLinesCommand( "move/resize rectangle", model() );

      if ( rectangle_->y0()->offset() >= 0 )
	rectangle_->y0()->
	  setOffset( rectangle_info_dialog_->yOriginOffsetLengthSpinBox->length() );
      else
	rectangle_->y0()->
	  setOffset( -rectangle_info_dialog_->yOriginOffsetLengthSpinBox->length() );

      move_command->add( rectangle_->y0(), old_offset );

      dimension_modified = true;
    }

    if ( move_command != 0 )
      CommandHistory::instance().addCommand( move_command );

    RestyleRectangleCommand* restyle_command = 0;

    if ( rectangle_info_dialog_->aboveButton->isOn() &&
	 width_annotation_side_ == lC::BELOW ) {
      if ( restyle_command == 0 )
	restyle_command = new RestyleRectangleCommand( "restyle rectangle", this );

      restyle_command->changeWidthAnnotationSide( lC::BELOW, lC::ABOVE );

      setWidthAnnotationSide( lC::ABOVE );

      dimension_modified = true;
    }
    else if ( rectangle_info_dialog_->belowButton->isOn() &&
	      width_annotation_side_ == lC::ABOVE ) {
      if ( restyle_command == 0 )
	restyle_command = new RestyleRectangleCommand( "restyle rectangle", this );

      restyle_command->changeWidthAnnotationSide( lC::ABOVE, lC::BELOW );

      setWidthAnnotationSide( lC::BELOW );

      dimension_modified = true;
    }
       
    if ( rectangle_info_dialog_->rightButton->isOn() &&
	 height_annotation_side_ == lC::BELOW ) {
      if ( restyle_command == 0 )
	restyle_command = new RestyleRectangleCommand( "restyle rectangle", this );

      restyle_command->changeHeightAnnotationSide( lC::BELOW, lC::ABOVE );

      setHeightAnnotationSide( lC::ABOVE );

      dimension_modified = true;
    }
    else if ( rectangle_info_dialog_->leftButton->isOn() &&
	      height_annotation_side_ == lC::ABOVE ) {
      if ( restyle_command == 0 )
	restyle_command = new RestyleRectangleCommand( "restyle rectangle", this );

      restyle_command->changeHeightAnnotationSide( lC::ABOVE, lC::BELOW );

      setHeightAnnotationSide( lC::BELOW );

      dimension_modified = true;
    }

    if ( rectangle_info_dialog_->rectangleStyleSlider->value() == 0 ) {
      if ( style_ != 0 ) {

	if ( restyle_command == 0 )
	  restyle_command = new RestyleRectangleCommand( "restyle rectangle", this);

	restyle_command->changeStyle( style_, 0 );

	delete style_;
	style_ = 0;
      }
    }
    else {
      Style* style = new Style;

      style->setStyle( (lC::Render::Style)rectangle_info_dialog_->
		       rectangleStyleSlider->value() );
      style->setEdge( (lC::Edge::Style)rectangle_info_dialog_->
		      edgeStyleComboBox->value() );
      style->setWireframeColor( rectangle_info_dialog_->
				wireframeStyleColorChooser->color() );
      style->setSolidColor( rectangle_info_dialog_->
			    solidStyleColorChooser->color() );
      style->setTextureFile( rectangle_info_dialog_->
			     textureStyleFileChooser->fileName() );

      if ( restyle_command == 0 )
	restyle_command = new RestyleRectangleCommand( "restyle rectangle", this);

      restyle_command->changeStyle( style_, style );

      if ( style_ != 0 )
	delete style_;

      style_ = style;
    }

    if ( restyle_command != 0 )
      CommandHistory::instance().addCommand( restyle_command );

    if ( dimension_modified ) updateDimensions();

    if ( dimension_modified || modified ) parent()->figureModified();
  }

  void RectangleView::draw ( void ) const
  {
    if ( !rectangle_->isValid() ) return;

    // If highlighted, draw an "X" at the origin.

    if ( isHighlighted() || isActivated() ) {
      glColor3ubv( lC::qCubv( view()->constraintPrimaryColor() ) );

      glBegin( GL_LINES );

      double size = 1./16. * view()->scale();
      double gap = size / 2.;

      Point p;
      p = rectangle_->origin() + Vector( gap, gap );
      glVertex2dv( p );
      p += Vector( size, size );
      glVertex2dv( p );

      p = rectangle_->origin() + Vector( -gap, gap );
      glVertex2dv( p );
      p += Vector( -size, size );
      glVertex2dv( p );

      p = rectangle_->origin() + Vector( -gap, -gap );
      glVertex2dv( p );
      p += Vector( -size, -size );
      glVertex2dv( p );

      p = rectangle_->origin() + Vector( gap, -gap );
      glVertex2dv( p );
      p += Vector( size, -size );
      glVertex2dv( p );

      glEnd();
    }

    // Draw the rectangle itself. Start by picking the appropriate color.

    QColor regular_color = view()->geometryColor();

    if ( style_ != 0 && style_->style() >= lC::Render::WIREFRAME )
      regular_color = style_->wireframeColor();

    QColor highlight_color = regular_color.light();

    // Determine if we need to draw a solid rectangle and its color.

    glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

    if ( style_ == 0 ) {
      if ( parent()->renderStyle() == lC::Render::SOLID ) {
	if ( isActivated() )
	  glColor3ubv( lC::qCubv( highlight_color ) );
	else
	  glColor3ubv( lC::qCubv( regular_color ) );

	glRectdv( rectangle_->lbVertex(), rectangle_->rtVertex() );
      }
    }
    else if ( style_->style() == lC::Render::SOLID ) {
      QColor solid_color = style_->solidColor();

      if ( isActivated() )
	solid_color = solid_color.light();

      glColor3ubv( lC::qCubv( solid_color ) );

      glRectdv( rectangle_->lbVertex(), rectangle_->rtVertex() );
    }

    // Maybe it should be textured.
    if ( style_ == 0 ) {
      if ( parent()->renderStyle() == lC::Render::TEXTURED ) {
	QString texture_file = OpenGLGlobals::instance()->textureFile();
	GLuint gl_texture_name = view()->texture( texture_file );
	QImage image = lC::lookupImage( texture_file );

	drawTextured( gl_texture_name, image );
      }
    }
    else if ( style_->style() == lC::Render::TEXTURED ) {
      GLuint gl_texture_name = view()->texture( style_->textureFile() );
      QImage image = lC::lookupImage( style_->textureFile() );

      drawTextured( gl_texture_name, image );
    }

    // Use a special stipple for the edges?

    bool stippled = false;

    if ( style_ != 0 && style_->edge() != lC::Edge::SOLID ) {
      glEnable( GL_LINE_STIPPLE );
      switch ( style_->edge() ) {
      case lC::Edge::DASH:
	glLineStipple( 1, 0xf0f0 ); break;
      case lC::Edge::DOT:
	glLineStipple( 1, 0xaaaa ); break;
      case lC::Edge::DASHDOT:
	glLineStipple( 1, 0xf6f6 ); break;
      case lC::Edge::DASHDOTDOT:
	glLineStipple( 1, 0xeaea ); break;
      }
      stippled = true;
    }

    glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

    // Each edge may be highlighted separately depending on the seletion
    // mode.

    glBegin( GL_QUADS );

    if ( isHighlighted() || isActivated() ||
	 left_edge_.isHighlighted() || left_edge_.isActivated() )
      glColor3ubv( lC::qCubv( highlight_color ) );
    else
      glColor3ubv( lC::qCubv( regular_color ) );

    glVertex2dv( rectangle_->lbVertex() );

    if ( isHighlighted() || isActivated() ||
	 bottom_edge_.isHighlighted() || bottom_edge_.isActivated() )
      glColor3ubv( lC::qCubv( highlight_color ) );
    else
      glColor3ubv( lC::qCubv( regular_color ) );

    glVertex2dv( rectangle_->rbVertex() );

    if ( isHighlighted() || isActivated() ||
	 right_edge_.isHighlighted() || right_edge_.isActivated() )
      glColor3ubv( lC::qCubv( highlight_color ) );
    else
      glColor3ubv( lC::qCubv( regular_color ) );

    glVertex2dv( rectangle_->rtVertex() );

    if ( isHighlighted() || isActivated() ||
	 top_edge_.isHighlighted() || top_edge_.isActivated() )
      glColor3ubv( lC::qCubv( highlight_color ) );
    else
      glColor3ubv( lC::qCubv( regular_color ) );

    glVertex2dv( rectangle_->ltVertex() );

    glEnd();

    if ( stippled ) glDisable( GL_LINE_STIPPLE );

    // Determine if we need to draw handles or coincident constraints.

    lC::Render::Mode mode = lC::Render::INVISIBLE;
    if ( isHighlighted() )
      mode = lC::Render::HIGHLIGHTED;
    if ( isActivated() )
      mode = lC::Render::ACTIVATED;

    if ( mode != lC::Render::INVISIBLE ) {

      // Draw an alignment indication on any edges which have the
      // coincident constraint.

      if ( !rectangle_->left()->isAdjustable() ) {
	Segment edge_segment = rectangle_->closestVertices( rectangle_->left() );
	Segment reference_segment = rectangle_->left()->reference()->parent()->
	  closestVertices( rectangle_->left()->reference() );
	Alignment::draw( view(), edge_segment, reference_segment );
      }

      if ( !rectangle_->right()->isAdjustable() ) {
	Segment edge_segment = rectangle_->closestVertices( rectangle_->right());
	Segment reference_segment = rectangle_->right()->reference()->parent()->
	  closestVertices( rectangle_->right()->reference() );
	Alignment::draw( view(), edge_segment, reference_segment );
      }

      if ( !rectangle_->bottom()->isAdjustable() ) {
	Segment edge_segment = rectangle_->closestVertices(rectangle_->bottom());
	Segment reference_segment = rectangle_->bottom()->reference()->parent()->
	  closestVertices( rectangle_->bottom()->reference() );
	Alignment::draw( view(), edge_segment, reference_segment );
      }

      if ( !rectangle_->top()->isAdjustable() ) {
	Segment edge_segment = rectangle_->closestVertices( rectangle_->top() );
	Segment reference_segment = rectangle_->top()->reference()->parent()->
	  closestVertices( rectangle_->top()->reference() );
	Alignment::draw( view(), edge_segment, reference_segment );
      }

      // Don't draw the resize handle if the edge cannot be moved...

      bool l_resize = rectangle_->left()->isAdjustable();
      bool r_resize = rectangle_->right()->isAdjustable();
      bool b_resize = rectangle_->bottom()->isAdjustable();
      bool t_resize = rectangle_->top()->isAdjustable();

      if ( l_resize && b_resize )
	ResizeHandle::draw( view(), mode, rectangle_->lbVertex() );

      if ( b_resize )
	ResizeHandle::draw( view(), mode, rectangle_->lbVertex() +
		      0.5 * ( rectangle_->rbVertex() - rectangle_->lbVertex() ) );
      if ( r_resize && b_resize )
	ResizeHandle::draw( view(), mode, rectangle_->rbVertex() );

      if ( r_resize )
	ResizeHandle::draw( view(), mode, rectangle_->rbVertex() +
		      0.5 * ( rectangle_->rtVertex() - rectangle_->rbVertex() ) );

      if ( r_resize && t_resize )
	ResizeHandle::draw( view(), mode, rectangle_->rtVertex() );

      if ( t_resize )
	ResizeHandle::draw( view(), mode, rectangle_->rtVertex() +
		      0.5 * ( rectangle_->ltVertex() - rectangle_->rtVertex() ) );

      if ( t_resize && l_resize )
	ResizeHandle::draw( view(), mode, rectangle_->ltVertex() );

      if ( l_resize )
	ResizeHandle::draw( view(), mode, rectangle_->ltVertex() +
		      0.5 * ( rectangle_->lbVertex() - rectangle_->ltVertex() ) );
    }

    // The previous handles are drawn if the entire figure is highlighted
    // or activated. Additionally, a single edge may be highlighted or
    // activated.

    if ( left_edge_.isHighlighted() ) {
      ResizeHandle::draw( view(), lC::Render::HIGHLIGHTED, rectangle_->lbVertex() );
      ResizeHandle::draw( view(), lC::Render::HIGHLIGHTED, rectangle_->ltVertex() );
    }
    else if ( left_edge_.isActivated() ) {
      ResizeHandle::draw( view(), lC::Render::ACTIVATED, rectangle_->lbVertex() );
      ResizeHandle::draw( view(), lC::Render::ACTIVATED, rectangle_->ltVertex() );
    }
    if ( right_edge_.isHighlighted() ) {
      ResizeHandle::draw( view(), lC::Render::HIGHLIGHTED, rectangle_->rbVertex() );
      ResizeHandle::draw( view(), lC::Render::HIGHLIGHTED, rectangle_->rtVertex() );
    }
    else if ( right_edge_.isActivated() ) {
      ResizeHandle::draw( view(), lC::Render::ACTIVATED, rectangle_->rbVertex() );
      ResizeHandle::draw( view(), lC::Render::ACTIVATED, rectangle_->rtVertex() );
    }
    if ( bottom_edge_.isHighlighted() ) {
      ResizeHandle::draw( view(), lC::Render::HIGHLIGHTED, rectangle_->lbVertex() );
      ResizeHandle::draw( view(), lC::Render::HIGHLIGHTED, rectangle_->rbVertex() );
    }
    else if ( bottom_edge_.isActivated() ) {
      ResizeHandle::draw( view(), lC::Render::ACTIVATED, rectangle_->lbVertex() );
      ResizeHandle::draw( view(), lC::Render::ACTIVATED, rectangle_->rbVertex() );
    }
    if ( top_edge_.isHighlighted() ) {
      ResizeHandle::draw( view(), lC::Render::HIGHLIGHTED, rectangle_->ltVertex() );
      ResizeHandle::draw( view(), lC::Render::HIGHLIGHTED, rectangle_->rtVertex() );
    }
    else if ( top_edge_.isActivated() ) {
      ResizeHandle::draw( view(), lC::Render::ACTIVATED, rectangle_->ltVertex() );
      ResizeHandle::draw( view(), lC::Render::ACTIVATED, rectangle_->rtVertex() );
    }

    // Draw any dimension views we have

    if ( x0_dimensionview_ != 0 ) x0_dimensionview_->draw();
    if ( y0_dimensionview_ != 0 ) y0_dimensionview_->draw();
    if ( x1_dimensionview_ != 0 ) x1_dimensionview_->draw();
    if ( y1_dimensionview_ != 0 ) y1_dimensionview_->draw();
  }

  void RectangleView::drawTextured ( GLuint gl_texture_name, const QImage& image )
    const
  {
    glEnable( GL_TEXTURE_2D );
    glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glBindTexture( GL_TEXTURE_2D, gl_texture_name );

    GLfloat x_resolution = 0.0254 * image.dotsPerMeterX() / image.width();
    GLfloat y_resolution = 0.0254 * image.dotsPerMeterY() / image.height();

    if ( x_resolution == 0. ) x_resolution = 20. / image.width();
    if ( y_resolution == 0. ) y_resolution = 20. / image.height();

    GLfloat s_coefficients[] = { x_resolution, 0., 0.,
				 -x_resolution * rectangle_->lbVertex()[X] };
    GLfloat t_coefficients[] = { 0., y_resolution, 0.,
				 -y_resolution * rectangle_->lbVertex()[Y] };

    glEnable( GL_TEXTURE_GEN_S );
    glEnable( GL_TEXTURE_GEN_T );

    glTexGeni( GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR );
    glTexGenfv( GL_S, GL_OBJECT_PLANE, s_coefficients );
    glTexGeni( GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR );
    glTexGenfv( GL_T, GL_OBJECT_PLANE, t_coefficients );

    glRectdv( rectangle_->lbVertex(), rectangle_->rtVertex() );

    glDisable( GL_TEXTURE_2D );
  }

  void RectangleView::select ( SelectionType selection_type ) const
  {
    if ( !rectangle_->isComplete() ) return;

    glLoadName( selectionName() );

    // The handles override any edge selection

    if ( isHighlighted() || isActivated() ) {
      // Don't *select* the resize handle if the edge cannot be moved.
      bool l_resize = rectangle_->left()->isAdjustable();
      bool r_resize = rectangle_->right()->isAdjustable();
      bool b_resize = rectangle_->bottom()->isAdjustable();
      bool t_resize = rectangle_->top()->isAdjustable();

      if ( l_resize && b_resize ) {
	glPushName( lb_handle_.selectionName() );
	ResizeHandle::select( view(), rectangle_->lbVertex() );
	glPopName();
      }

      if ( b_resize ) {
	glPushName( mb_handle_.selectionName() );
	ResizeHandle::select( view(), rectangle_->lbVertex() +
			0.5 * ( rectangle_->rbVertex() - rectangle_->lbVertex() ) );
	glPopName();
      }

      if ( r_resize && b_resize ) {
	glPushName( rb_handle_.selectionName() );
	ResizeHandle::select( view(), rectangle_->rbVertex() );
	glPopName();
      }

      if ( r_resize ) {
	glPushName( mr_handle_.selectionName() );
	ResizeHandle::select( view(), rectangle_->rbVertex() +
			0.5 * ( rectangle_->rtVertex() - rectangle_->rbVertex() ) );
	glPopName();
      }

      if ( r_resize && t_resize ) {
	glPushName( rt_handle_.selectionName() );
	ResizeHandle::select( view(), rectangle_->rtVertex() );
	glPopName();
      }

      if ( t_resize ) {
	glPushName( mt_handle_.selectionName() );
	ResizeHandle::select( view(), rectangle_->rtVertex() +
		      0.5 * ( rectangle_->ltVertex() - rectangle_->rtVertex() ) );
	glPopName();
      }

      if ( t_resize && l_resize ) {
	glPushName( lt_handle_.selectionName() );
	ResizeHandle::select( view(), rectangle_->ltVertex() );
	glPopName();
      }

      if ( l_resize ) {
	glPushName( ml_handle_.selectionName() );
	ResizeHandle::select( view(), rectangle_->ltVertex() +
			0.5 * ( rectangle_->lbVertex() - rectangle_->ltVertex() ) );
	glPopName();
      }
    }

    if ( ( style_ != 0 && style_->style() >= lC::Render::SOLID ) ||
	 ( style_ == 0 && parent()->renderStyle() >= lC::Render::SOLID ) ) {
      glPushName( solid_rectangle_.selectionName() );

      glRectdv( rectangle_->lbVertex(), rectangle_->rtVertex() );

      glPopName();
    }

    glPushName( bottom_edge_.selectionName() );

    glBegin( GL_LINES );

    glVertex2dv( rectangle_->lbVertex() );
    glVertex2dv( rectangle_->rbVertex() );

    glEnd();

    glPopName();

    glPushName( right_edge_.selectionName() );

    glBegin( GL_LINES );

    glVertex2dv( rectangle_->rbVertex() );
    glVertex2dv( rectangle_->rtVertex() );

    glEnd();

    glPopName();

    glPushName( top_edge_.selectionName() );

    glBegin( GL_LINES );

    glVertex2dv( rectangle_->rtVertex() );
    glVertex2dv( rectangle_->ltVertex() );

    glEnd();

    glPopName();

    glPushName( left_edge_.selectionName() );

    glBegin( GL_LINES );

    glVertex2dv( rectangle_->ltVertex() );
    glVertex2dv( rectangle_->lbVertex() );

    glEnd();

    glPopName();

    // So, what happens if we just include the dimensions here?
    if ( x0_dimensionview_ != 0 ) x0_dimensionview_->select( selection_type );
    if ( y0_dimensionview_ != 0 ) y0_dimensionview_->select( selection_type );
    if ( x1_dimensionview_ != 0 ) x1_dimensionview_->select( selection_type );
    if ( y1_dimensionview_ != 0 ) y1_dimensionview_->select( selection_type );
  }

  /*!
   * Perhaps this should really be the purview of ModifyInput since
   * ultimately, it's adjusting the rectangle, not the view of it per se...
   */
  lC::ValidDelta RectangleView::adjust ( int selection_name,
					 const Point& last_pnt, Point& current_pnt )
  {
    return (rectangle_->*adjustments_[selection_name])( last_pnt, current_pnt);
  }

  void RectangleView::updateDimensions ( void ) const
  {
    if ( x0_dimensionview_ != 0 )
      x0_dimensionview_->computeLayout();

    if ( y0_dimensionview_ != 0 )
      y0_dimensionview_->computeLayout();

    if ( x1_dimensionview_ != 0 )
      x1_dimensionview_->computeLayout();

    if ( y1_dimensionview_ != 0 )
      y1_dimensionview_->computeLayout();
  }

  void RectangleView::setCursor ( GLuint selection_name )
  {
    GraphicsView* graphics_object = rectangle_objects_[ selection_name ];

    if ( graphics_object == 0 )
      graphics_object = dimensionview_objects_[ selection_name ];

    if ( graphics_object != 0 )
      view()->setCursor( QCursor( graphics_object->cursorShape() ) );
  }

  void RectangleView::setCursor ( const QCursor* cursor )
  {
    view()->setCursor( *cursor );
  }

  void RectangleView::unsetCursor ( void )
  {
    view()->unsetCursor();
  }

  void RectangleView::setWidthAnnotationSide ( lC::AnnotationSide width_annotation_side )
  {
    width_annotation_side_ = width_annotation_side;

    Line* reference_edge;

    switch ( width_annotation_side ) {
    case lC::ABOVE:
      reference_edge = rectangle_->top(); break;
    case lC::BELOW:
      reference_edge = rectangle_->bottom(); break;
    }

    disconnect( x1_dimensionview_->fromReference(),SIGNAL(modifiedPosition()),
		this, SLOT( x1DmVRefModified() ) );
    disconnect( x1_dimensionview_->toReference(), SIGNAL( modifiedPosition()),
		this, SLOT( x1DmVRefModified() ) );
    disconnect( x1_dimensionview_->fromReference(),SIGNAL(destroyedGeometry()),
		this, SLOT( x1DestroyedDmVRef() ) );
    disconnect( x1_dimensionview_->toReference(), SIGNAL(destroyedGeometry()),
		this, SLOT( x1DestroyedDmVRef() ) );

    x1_dimensionview_->setReferences( reference_edge, reference_edge );
    x1_dimensionview_->setAnnotationSide( width_annotation_side );

    connect( reference_edge, SIGNAL( modifiedPosition() ),
	     SLOT( x1DmVRefModified() ) );
    connect( reference_edge, SIGNAL( destroyedGeometry() ),
	     SLOT( x1DestroyedDmVRef() ) );
  }

  void RectangleView::setHeightAnnotationSide ( lC::AnnotationSide height_annotation_side )
  {
    height_annotation_side_ = height_annotation_side;

    Line* reference_edge;

    switch ( height_annotation_side ) {
    case lC::ABOVE:
      reference_edge = rectangle_->right(); break;
    case lC::BELOW:
      reference_edge = rectangle_->left(); break;
    }

    disconnect( y1_dimensionview_->fromReference(),SIGNAL(modifiedPosition()),
		this, SLOT( y1DmVRefModified() ) );
    disconnect( y1_dimensionview_->toReference(), SIGNAL( modifiedPosition()),
		this, SLOT( y1DmVRefModified() ) );
    disconnect( y1_dimensionview_->fromReference(),SIGNAL(destroyedGeometry()),
	        this, SLOT( y1DestroyedDmVRef() ) );
    disconnect( y1_dimensionview_->toReference(), SIGNAL(destroyedGeometry()),
		this, SLOT( y1DestroyedDmVRef() ) );

    y1_dimensionview_->setReferences( reference_edge, reference_edge );
    y1_dimensionview_->setAnnotationSide( height_annotation_side );

    connect( reference_edge, SIGNAL( modifiedPosition() ),
	     SLOT( y1DmVRefModified() ) );
    connect( reference_edge, SIGNAL( destroyedGeometry() ),
	     SLOT( y1DestroyedDmVRef() ) );
  }

  void RectangleView::setStyle ( Style* style )
  {
    if ( style_ != 0 )
      delete style_;

    style_ = style;
  }

  void RectangleView::write ( QDomElement& xml_rep ) const
  {
    if ( !rectangle_->isValid() ) return;

    QDomDocument document = xml_rep.ownerDocument();
    QDomElement rectangle_view_element = document.createElement( lC::STR::RECTANGLE_VIEW );
    rectangle_view_element.setAttribute( lC::STR::RECTANGLE,
					 rectangle_->dbURL().toString( true ) );
    rectangle_view_element.setAttribute( lC::STR::WIDTH_ANNOTATION_SIDE,
				 annotationSideText( width_annotation_side_ ) );
    rectangle_view_element.setAttribute( lC::STR::HEIGHT_ANNOTATION_SIDE,
				 annotationSideText( height_annotation_side_) );

    if ( style_ != 0 )
      style_->write( rectangle_view_element );

    if ( x0_dimensionview_ != 0 ) {
      QDomElement x_role_element = document.createElement( lC::STR::ROLE );
      x_role_element.setAttribute( lC::STR::NAME, lC::STR::X0 );
      x0_dimensionview_->write( x_role_element );
      rectangle_view_element.appendChild( x_role_element );
    }

    if ( y0_dimensionview_ != 0 ) {
      QDomElement y_role_element = document.createElement( lC::STR::ROLE );
      y_role_element.setAttribute( lC::STR::NAME, lC::STR::Y0 );
      y0_dimensionview_->write( y_role_element );
      rectangle_view_element.appendChild( y_role_element );
    }

    if ( x1_dimensionview_ != 0 ) {
      QDomElement width_role_element = document.createElement( lC::STR::ROLE );
      width_role_element.setAttribute( lC::STR::NAME, lC::STR::X1 );
      x1_dimensionview_->write( width_role_element );
      rectangle_view_element.appendChild( width_role_element );
    }

    if ( y1_dimensionview_ != 0 ) {
      QDomElement height_role_element = document.createElement( lC::STR::ROLE );
      height_role_element.setAttribute( lC::STR::NAME, lC::STR::Y1 );
      y1_dimensionview_->write( height_role_element );
      rectangle_view_element.appendChild( height_role_element );
    }

    xml_rep.appendChild( rectangle_view_element );
  }
  /*
   * Create an XML representation of a simplified version of this view
   * and its rectangle.
   */
  void RectangleView::copyWrite ( QDomElement& xml_rep ) const
  {
    QDomDocument document = xml_rep.ownerDocument();
    QDomElement rectangle_view_element = document.createElement( lC::STR::RECTANGLE_VIEW );
    rectangle_view_element.setAttribute( lC::STR::WIDTH_ANNOTATION_SIDE,
				 annotationSideText( width_annotation_side_ ) );
    rectangle_view_element.setAttribute( lC::STR::HEIGHT_ANNOTATION_SIDE,
				 annotationSideText( height_annotation_side_) );

    if ( style_ != 0 ) {
      QDomElement style_element = document.createElement( lC::STR::STYLE );
      style_element.setAttribute( lC::STR::STYLE, lC::Render::styleText( style_->style()));
      style_element.setAttribute( lC::STR::EDGE, lC::Edge::styleText( style_->edge() ) );
      style_element.setAttribute( lC::STR::WIREFRAME, style_->wireframeColor().name() );
      style_element.setAttribute( lC::STR::SOLID, style_->solidColor().name() );
      style_element.setAttribute( lC::STR::TEXTURE, style_->textureFile() );
      rectangle_view_element.appendChild( style_element );
    }

    rectangle_->copyWrite( rectangle_view_element );

    xml_rep.appendChild( rectangle_view_element );
  }

  // The rectangle was initialized
  void RectangleView::rectangleInitialized ( void )
  {
    // Construct a set of DimensionViews for the rectangle. (we're
    // going to write this all out for each step and then fold
    // redundancies later...)

    // First, the view has to be alerted to any changes in constraints
    // on the edges of the rectangle since they can potentially gain,
    // lose or alter offset constraints at any time.
    connect( rectangle_->x0(), SIGNAL( modifiedConstraint() ),
	     SLOT( x0ModifiedConstraint() ) );
    connect( rectangle_->y0(), SIGNAL( modifiedConstraint() ),
	     SLOT( y0ModifiedConstraint() ) );
    connect( rectangle_->x1(), SIGNAL( modifiedConstraint() ),
	     SLOT( x1ModifiedConstraint() ) );
    connect( rectangle_->y1(), SIGNAL( modifiedConstraint() ),
	     SLOT( y1ModifiedConstraint() ) );

    connect( rectangle_->x0(), SIGNAL( modifiedPosition() ),
	     SLOT( x0ModifiedPosition() ) );
    connect( rectangle_->y0(), SIGNAL( modifiedPosition() ),
	     SLOT( y0ModifiedPosition() ) );
    connect( rectangle_->x1(), SIGNAL( modifiedPosition() ),
	     SLOT( x1ModifiedPosition() ) );
    connect( rectangle_->y1(), SIGNAL( modifiedPosition() ),
	     SLOT( y1ModifiedPosition() ) );

    // Next, there should be legitimate constraints on all the edges now
    list_view_item_->setText( lC::DETAIL,
			     trC( lC::STR::RECTANGLE_FMT ).
			     arg( rectangle_->origin()[X] ).
			     arg( rectangle_->origin()[Y] ).
			     arg( rectangle_->width() ).
			     arg( rectangle_->height() ) );

    x0_list_view_->setText( lC::DETAIL, rectangle_->x0()->detail() );
    y0_list_view_->setText( lC::DETAIL, rectangle_->y0()->detail() );
    x1_list_view_->setText( lC::DETAIL, rectangle_->x1()->detail() );
    y1_list_view_->setText( lC::DETAIL, rectangle_->y1()->detail() );

    if ( rectangle_->x0()->isAdjustable() && !rectangle_->x0()->isFree() ) {
      // This implies that y0 has a coincident constraint, so we'll use
      // that as the references for the dimension positioning.
      x0_dimensionview_ = new DimensionView( rectangle_->x0(),
#if 1
					     rectangle_->y0(),
					     rectangle_->y0()->reference(),
#else
					     rectangle_->top(),
					     rectangle_->top()->reference(),
#endif
					     lC::HORIZONTAL, lC::ABOVE,
					     view()->genSelectionName(),
					     this );

      dimensionview_objects_.insert( x0_dimensionview_->selectionName(),
				     x0_dimensionview_ );

      connect( rectangle_->x0(), SIGNAL( modifiedPosition() ),
	       SLOT( x0DmVDimModified() ) );
      // Note that if y0->reference is modified, y0 will be modified as well
#if 1
      connect( rectangle_->y0(), SIGNAL( modifiedPosition() ),
	       SLOT( x0DmVRefModified() ));
#else
      connect( rectangle_->top(), SIGNAL( modifiedPosition() ),
	       SLOT( x0DmVRefModified() ));
#endif
      connect( x0_dimensionview_->fromReference(), SIGNAL( destroyedGeometry() ),
	       SLOT( x0DestroyedDmVRef() ) );
      connect( x0_dimensionview_->toReference(), SIGNAL( destroyedGeometry() ),
	       SLOT( x0DestroyedDmVRef() ) );
    }

    if ( rectangle_->y0()->isAdjustable() && !rectangle_->y0()->isFree() ) {
      // This implies that x0 has a coincident constraint, so we'll use
      // that as the references for the dimension positioning.
      y0_dimensionview_ = new DimensionView( rectangle_->y0(),
#if 1
					     rectangle_->x0(),
					     rectangle_->x0()->reference(),
#else
					     rectangle_->right(),
					     rectangle_->right()->reference(),
#endif
					     lC::VERTICAL, lC::ABOVE,
					     view()->genSelectionName(),
					     this );

      dimensionview_objects_.insert( y0_dimensionview_->selectionName(),
				     y0_dimensionview_ );

      connect( rectangle_->y0(), SIGNAL( modifiedPosition() ),
	       SLOT( y0DmVDimModified() ) );
      // Note that if x0->reference is modified, x0 will be modified as well
#if 1
      connect( rectangle_->x0(), SIGNAL( modifiedPosition() ),
	       SLOT( y0DmVRefModified() ) );
#else
      connect( rectangle_->right(), SIGNAL( modifiedPosition() ),
	       SLOT( y0DmVRefModified() ) );
#endif
      connect( y0_dimensionview_->fromReference(), SIGNAL( destroyedGeometry() ),
	       SLOT( y0DestroyedDmVRef() ) );
      connect( y0_dimensionview_->toReference(), SIGNAL( destroyedGeometry() ),
	       SLOT( y0DestroyedDmVRef() ) );
    }

    // It's always the case that x1 has an offset constaint at this
    // point.  The choice of y0 as the reference is arbitrary since
    // the size of rectangle is zero. Ahh, not so arbitrary after all:
    // we really want the dimension view to adjust itself so that it's
    // always ABOVE the rectangle as it is created. The rectangle
    // creation routine moves y1!

    // Actually, obey the annotation side designation.
    Line* width_reference;
    switch ( width_annotation_side_ ) {
    case lC::ABOVE:
      width_reference = rectangle_->top(); break;
    case lC::BELOW:
      width_reference = rectangle_->bottom(); break;
    }


    x1_dimensionview_ = new DimensionView( rectangle_->x1(),
#if 0
					   rectangle_->y1(),
					   rectangle_->y1(),
#else
					   width_reference, width_reference,
#endif
					   lC::HORIZONTAL, width_annotation_side_,
					   view()->genSelectionName(),
					   this );

    dimensionview_objects_.insert( x1_dimensionview_->selectionName(),
				   x1_dimensionview_ );

    connect( rectangle_->x1(), SIGNAL( modifiedPosition() ),
	     SLOT( x1DmVDimModified() ) );
#if 0
    connect( rectangle_->y1(), SIGNAL( modifiedPosition() ),
	     SLOT( x1DmVRefModified() ));
    // Slight optimization since both references are the same
    connect( rectangle_->y1(), SIGNAL( destroyedGeometry() ),
	     SLOT( x1DestroyedDmVRef() ) );
#else
    connect( width_reference, SIGNAL( modifiedPosition() ),
	     SLOT( x1DmVRefModified() ));
    // Slight optimization since both references are the same
    connect( width_reference, SIGNAL( destroyedGeometry() ),
	     SLOT( x1DestroyedDmVRef() ) );
#endif
    // It's also always the case that y1 has an offset constaint at
    // this point.  The choice of x0 as the reference is arbitrary
    // since the size of rectangle is zero. Ahh, not so arbitrary
    // after all: we really want the dimension view to adjust itself
    // so that it's always to the right (ABOVE) the rectanlge as it is
    // created. The rectanlge creation routine moves x1!

    Line* height_reference;
    switch ( height_annotation_side_ ) {
    case lC::ABOVE:
      height_reference = rectangle_->right(); break;
    case lC::BELOW:
      height_reference = rectangle_->left(); break;
    }

    y1_dimensionview_ = new DimensionView( rectangle_->y1(),
#if 0
					   rectangle_->x1(),
					   rectangle_->x1(),
#else
					   height_reference, height_reference,
#endif
					   lC::VERTICAL, height_annotation_side_,
					   view()->genSelectionName(),
					   this );

    dimensionview_objects_.insert( y1_dimensionview_->selectionName(),
				   y1_dimensionview_ );

    connect( rectangle_->y1(), SIGNAL( modifiedPosition() ),
	     SLOT( y1DmVDimModified() ) );
#if 0
    connect( rectangle_->x1(), SIGNAL( modifiedPosition() ),
	     SLOT( y1DmVRefModified() ));
    // Slight optimization since both references are the same
    connect( rectangle_->x1(), SIGNAL( destroyedGeometry() ),
	     SLOT( y1DestroyedDmVRef() ) );
#else
    connect( height_reference, SIGNAL( modifiedPosition() ),
	     SLOT( y1DmVRefModified() ));
    // Slight optimization since both references are the same
    connect( height_reference, SIGNAL( destroyedGeometry() ),
	     SLOT( y1DestroyedDmVRef() ) );
#endif
  }

  // The rectangle was moved
  void RectangleView::rectangleMoved ( void )
  {
    //    cout << "rectangle moved" << endl;
  }

  // The rectangle was resized
  void RectangleView::rectangleResized ( void )
  {
    //    cout << "rectangle resized" << endl;
  }

  /*!
   * x0's constraint has been modified. Make the dimension view consistent
   * with the new constraint. (Might want to figure out if we can reduce
   * some of the code here, but it's all slightly different from case to
   * case...)
   */
  void RectangleView::x0ModifiedConstraint ( void )
  {
    // First, determine if we even need a dimension view
    if ( rectangle_->x0()->needsDimensionView() ) {
      // If we didn't have one before, create one
      if ( x0_dimensionview_ == 0 ) {
	// If x0 is relative to x1, create a width-like dimension. Use the
	// width annotation side setting to determine the reference edge.
	if ( rectangle_->x0()->reference() == rectangle_->x1() ) {
	  Line* width_reference;
	  switch ( width_annotation_side_ ) {
	  case lC::ABOVE:
	    width_reference = rectangle_->top(); break;
	  case lC::BELOW:
	    width_reference = rectangle_->bottom(); break;
	  }

	  x0_dimensionview_ = new DimensionView( rectangle_->x0(),
						 width_reference,
						 width_reference,
						 lC::HORIZONTAL, lC::ABOVE,
						 view()->genSelectionName(),
						 this );

	  dimensionview_objects_.insert( x0_dimensionview_->selectionName(),
					 x0_dimensionview_ );

	  connect( rectangle_->x0(), SIGNAL( modifiedPosition() ),
		   SLOT( x0DmVDimModified() ) );
	  // Slight optimization since both references are the same
	  connect( width_reference, SIGNAL( modifiedPosition() ),
		   SLOT( x0DmVRefModified() ) );
	  connect( width_reference, SIGNAL( destroyedGeometry() ),
		   SLOT( x0DestroyedDmVRef() ) );
	}
	// x0 is not related to x1 and we have to do some convoluted things
	// to figure out decent references. (and more is required...)
	else {
	  Curve* to_reference = rectangle_->top();
	  // So, ask the parent (Figure) of x0's reference for a
	  // orthogonal edge which is close to the rectangle's origin
	  // (recall that the rectangle's origin is at the intersection
	  // of x0 and y0). Conceivably a reference line or centerline
	  // might not be able to supply such a thing...
	  Vertex v = rectangle_->x0()->reference()->parent()->
	    closestVertex( rectangle_->origin(), rectangle_->x0() );
	  Curve* from_reference = v.curve2_;

	  x0_dimensionview_ = new DimensionView( rectangle_->x0(),
						 from_reference,
						 to_reference,
						 lC::HORIZONTAL, lC::ABOVE,
						 view()->genSelectionName(),
						 this );

	  dimensionview_objects_.insert( x0_dimensionview_->selectionName(),
					 x0_dimensionview_ );

	  connect( rectangle_->x0(), SIGNAL( modifiedPosition() ),
		   SLOT( x0DmVDimModified() ) );
	  connect( from_reference, SIGNAL( modifiedPosition() ),
		   SLOT( x0DmVRefModified() ) );
	  connect( to_reference, SIGNAL( modifiedPosition() ),
		   SLOT( x0DmVRefModified() ) );
	  connect( from_reference, SIGNAL( destroyedGeometry() ),
		   SLOT( x0DestroyedDmVRef() ) );
	  connect( to_reference, SIGNAL( destroyedGeometry() ),
		   SLOT( x0DestroyedDmVRef() ) );
	}
      }
      else { // A dimension view already exists, just recycle it.
	// If x0 is relative to x1, create a width-like dimension. Use the
	// width annotation side setting to determine the reference edge.
	if ( rectangle_->x0()->reference() == rectangle_->x1() ) {
	  Line* width_reference;
	  switch ( width_annotation_side_ ) {
	  case lC::ABOVE:
	    width_reference = rectangle_->top(); break;
	  case lC::BELOW:
	    width_reference = rectangle_->bottom(); break;
	  }

	  disconnect( x0_dimensionview_->fromReference(),SIGNAL(modifiedPosition()),
		      this, SLOT( x0DmVRefModified() ) );
	  disconnect( x0_dimensionview_->toReference(), SIGNAL( modifiedPosition()),
		      this, SLOT( x0DmVRefModified() ) );
	  disconnect(x0_dimensionview_->fromReference(),SIGNAL(destroyedGeometry()),
		     this, SLOT( x0DestroyedDmVRef() ) );
	  disconnect( x0_dimensionview_->toReference(), SIGNAL(destroyedGeometry()),
		      this, SLOT( x0DestroyedDmVRef() ) );

	  x0_dimensionview_->setReferences( width_reference, width_reference );

	  connect( width_reference, SIGNAL( modifiedPosition() ),
		   SLOT( x0DmVRefModified() ) );
	  connect( width_reference, SIGNAL( destroyedGeometry() ),
		   SLOT( x0DestroyedDmVRef() ) );
	}
	// x0 is not related to x1 and we have to do some convoluted things
	// to figure out decent references. (and more is required...)
	else {
	  Curve* to_reference = rectangle_->top();
	  // So, ask the parent (Figure) of x0's reference for a
	  // orthogonal edge which is close to the rectangle's origin
	  // (recall that the rectangle's origin is at the intersection
	  // of x0 and y0). Conceivably a reference line or centerline
	  // might not be able to supply such a thing...
	  Vertex v = rectangle_->x0()->reference()->parent()->
	    closestVertex( rectangle_->origin(), rectangle_->x0() );
	  Curve* from_reference = v.curve2_;

	  disconnect( x0_dimensionview_->fromReference(),SIGNAL(modifiedPosition()),
		      this, SLOT( x0DmVRefModified() ) );
	  disconnect( x0_dimensionview_->toReference(), SIGNAL( modifiedPosition()),
		      this, SLOT( x0DmVRefModified() ) );
	  disconnect(x0_dimensionview_->fromReference(),SIGNAL(destroyedGeometry()),
		     this, SLOT( x0DestroyedDmVRef() ) );
	  disconnect( x0_dimensionview_->toReference(), SIGNAL(destroyedGeometry()),
		      this, SLOT( x0DestroyedDmVRef() ) );

	  x0_dimensionview_->setReferences( from_reference, to_reference );

	  connect( from_reference, SIGNAL( modifiedPosition() ),
		   SLOT( x0DmVRefModified() ) );
	  connect( to_reference, SIGNAL( modifiedPosition() ),
		   SLOT( x0DmVRefModified() ) );
	  connect( from_reference, SIGNAL( destroyedGeometry() ),
		   SLOT( x0DestroyedDmVRef() ) );
	  connect( to_reference, SIGNAL( destroyedGeometry() ),
		   SLOT( x0DestroyedDmVRef() ) );
	}
      }
    }
    else { // we don't need a dimension view
      if ( x0_dimensionview_ != 0 ) { // remove it if we have one
	QDomDocument* xml_doc = ConstraintHistory::instance().history();
	if ( xml_doc != 0 ) {
	  QDomElement xml_rep = xml_doc->createElement( lC::STR::DIMENSION_VIEW );
	  xml_rep.setAttribute( lC::STR::URL, rectangle_->dbURL().toString( true ));
	  x0_dimensionview_->write( xml_rep );
	  ConstraintHistory::instance().appendUnconstraint( xml_rep );
	}

	disconnect( x0_dimensionview_->dimension(), SIGNAL( modifiedPosition() ),
		    this, SLOT( x0DmVDimModified() ) );
	disconnect( x0_dimensionview_->fromReference(), SIGNAL( modifiedPosition()),
		    this, SLOT( x0DmVRefModified() ) );
	disconnect( x0_dimensionview_->toReference(), SIGNAL( modifiedPosition() ),
		    this, SLOT( x0DmVRefModified() ) );
	disconnect( x0_dimensionview_->fromReference(), SIGNAL(destroyedGeometry()),
		    this, SLOT( x0DestroyedDmVRef() ) );
	disconnect( x0_dimensionview_->toReference(), SIGNAL(destroyedGeometry()),
		    this, SLOT( x0DestroyedDmVRef() ) );

	if ( x0_dimensionview_->isActivated() )
	  parent()->deactivateFigure( selectionName(),
				      x0_dimensionview_->selectionName() );

	dimensionview_objects_.remove( x0_dimensionview_->selectionName() );

	delete x0_dimensionview_;

	x0_dimensionview_ = 0;
      }
    }
  }
  /*!
   * This routine should only respond when x0 moves AND has an offset
   * constraint (i.e., this SLOT is only connected when the dimension view
   * exists). We might have to change the references of the dimension view
   * if x0 has the width-like offset and the reference is no longer
   * the correct edge for the width annotation side.
   */
  void RectangleView::x0DmVDimModified ( void )
  {
    if ( rectangle_->x0()->reference() == rectangle_->x1() ) {
      Line* width_reference;
      switch ( width_annotation_side_ ) {
      case lC::ABOVE:
	width_reference = rectangle_->top(); break;
      case lC::BELOW:
	width_reference = rectangle_->bottom(); break;
      }
      // If the reference edges don't already correspond to the
      // annotation side, replace it.
      if ( width_reference != x0_dimensionview_->fromReference() ||
	   width_reference != x0_dimensionview_->toReference() ) {

	disconnect( x0_dimensionview_->fromReference(), SIGNAL( modifiedPosition()),
		    this, SLOT( x0DmVRefModified() ) );
	disconnect( x0_dimensionview_->toReference(), SIGNAL( modifiedPosition() ),
		    this, SLOT( x0DmVRefModified() ) );
	disconnect( x0_dimensionview_->fromReference(), SIGNAL(destroyedGeometry()),
		    this, SLOT( x0DestroyedDmVRef() ) );
	disconnect( x0_dimensionview_->toReference(), SIGNAL(destroyedGeometry()),
		    this, SLOT( x0DestroyedDmVRef() ) );

	x0_dimensionview_->setReferences( width_reference, width_reference );

	connect( width_reference, SIGNAL( modifiedPosition() ),
		 SLOT( x0DmVRefModified() ) );
	connect( width_reference, SIGNAL( destroyedGeometry() ),
		 SLOT( x0DestroyedDmVRef() ) );
      }
    }

    x0_dimensionview_->computeLayout();
  }
  /*!
   * This routine should only respond when the references of x0's dimension
   * view move.
   */
  void RectangleView::x0DmVRefModified ( void )
  {
    x0_dimensionview_->computeLayout();
  }

  /*!
   * y0's constraint has been modified. Make the dimension view consistent
   * with the new constraint. (Might want to figure out if we can reduce
   * some of the code here, but it's all slightly different from case to
   * case...)
   */
  void RectangleView::y0ModifiedConstraint ( void )
  {
    // First, determine if we even need a dimension view
    if ( rectangle_->y0()->needsDimensionView() ) {
      // If we didn't have one before, create one
      if ( y0_dimensionview_ == 0 ) {
	// If y0 is relative to y1, create a height-like dimension. Use the
	// annotation side setting to determine the reference edge.
	if ( rectangle_->y0()->reference() == rectangle_->y1() ) {
	  Line* height_reference;
	  switch ( height_annotation_side_ ) {
	  case lC::ABOVE:
	    height_reference = rectangle_->right(); break;
	  case lC::BELOW:
	    height_reference = rectangle_->left(); break;
	  }

	  y0_dimensionview_ = new DimensionView( rectangle_->y0(),
						 height_reference,
						 height_reference,
						 lC::VERTICAL, lC::ABOVE,
						 view()->genSelectionName(),
						 this );

	  dimensionview_objects_.insert( y0_dimensionview_->selectionName(),
					 y0_dimensionview_ );

	  connect( rectangle_->y0(), SIGNAL( modifiedPosition() ),
		   SLOT( y0DmVDimModified() ) );
	  // Slight optimization since both references are the same
	  connect( height_reference, SIGNAL( modifiedPosition() ),
		   SLOT( y0DmVRefModified() ) );
	  connect( height_reference, SIGNAL( destroyedGeometry() ),
		   SLOT( y0DestroyedDmVRef() ) );
	}
	// y0 is not related to y1 and we have to do some convoluted things
	// to figure out decent references. (and more is required...)
	else {
	  Curve* to_reference = rectangle_->right();
	  // So, ask the parent (Figure) of y0's reference for a
	  // orthogonal edge which is close to the rectangle's origin
	  // (recall that the rectangle's origin is at the intersection
	  // of x0 and y0). Conceivably a reference line or centerline
	  // might not be able to supply such a thing...
	  Vertex v = rectangle_->y0()->reference()->parent()->
	    closestVertex( rectangle_->origin(), rectangle_->y0() );
	  Curve* from_reference = v.curve2_;

	  y0_dimensionview_ = new DimensionView( rectangle_->y0(),
						 from_reference,
						 to_reference,
						 lC::VERTICAL, lC::ABOVE,
						 view()->genSelectionName(),
						 this );

	  dimensionview_objects_.insert( y0_dimensionview_->selectionName(),
					 y0_dimensionview_ );

	  connect( rectangle_->y0(), SIGNAL( modifiedPosition() ),
		   SLOT( y0DmVDimModified() ) );
	  connect( from_reference, SIGNAL( modifiedPosition() ),
		   SLOT( y0DmVRefModified() ) );
	  connect( to_reference, SIGNAL( modifiedPosition() ),
		   SLOT( y0DmVRefModified() ) );
	  connect( from_reference, SIGNAL( destroyedGeometry() ),
		   SLOT( y0DestroyedDmVRef() ) );
	  connect( to_reference, SIGNAL( destroyedGeometry() ),
		   SLOT( y0DestroyedDmVRef() ) );
	}
      }
      else { // A dimension view already exists, just recycle it.
	// If y0 is relative to y1, create a height-like dimension. Use the
	// height annotation side setting to determine the reference edge.
	if ( rectangle_->y0()->reference() == rectangle_->y1() ) {
	  Line* height_reference;
	  switch ( height_annotation_side_ ) {
	  case lC::ABOVE:
	    height_reference = rectangle_->right(); break;
	  case lC::BELOW:
	    height_reference = rectangle_->left(); break;
	  }

	  disconnect( y0_dimensionview_->fromReference(),SIGNAL(modifiedPosition()),
		      this, SLOT( y0DmVRefModified() ) );
	  disconnect( y0_dimensionview_->toReference(), SIGNAL( modifiedPosition()),
		      this, SLOT( y0DmVRefModified() ) );
	  disconnect(y0_dimensionview_->fromReference(),SIGNAL(destroyedGeometry()),
		     this, SLOT( y0DestroyedDmVRef() ) );
	  disconnect( y0_dimensionview_->toReference(), SIGNAL(destroyedGeometry()),
		      this, SLOT( y0DestroyedDmVRef() ) );

	  y0_dimensionview_->setReferences( height_reference, height_reference );

	  connect( height_reference, SIGNAL( modifiedPosition() ),
		   SLOT( y0DmVRefModified() ) );
	  connect( height_reference, SIGNAL( destroyedGeometry() ),
		   SLOT( y0DestroyedDmVRef() ) );
	}
	// y0 is not related to y1 and we have to do some convoluted things
	// to figure out decent references. (and more is required...)
	else {
	  Curve* to_reference = rectangle_->right();
	  // So, ask the parent (Figure) of y0's reference for a
	  // orthogonal edge which is close to the rectangle's origin
	  // (recall that the rectangle's origin is at the intersection
	  // of x0 and y0). Conceivably a reference line or centerline
	  // might not be able to supply such a thing...
	  Vertex v = rectangle_->y0()->reference()->parent()->
	    closestVertex( rectangle_->origin(), rectangle_->y0() );
	  Curve* from_reference = v.curve2_;

	  disconnect( y0_dimensionview_->fromReference(),SIGNAL(modifiedPosition()),
		      this, SLOT( y0DmVRefModified() ) );
	  disconnect( y0_dimensionview_->toReference(), SIGNAL( modifiedPosition()),
		      this, SLOT( y0DmVRefModified() ) );
	  disconnect(y0_dimensionview_->fromReference(),SIGNAL(destroyedGeometry()),
		     this, SLOT( y0DestroyedDmVRef() ) );
	  disconnect( y0_dimensionview_->toReference(), SIGNAL(destroyedGeometry()),
		      this, SLOT( y0DestroyedDmVRef() ) );

	  y0_dimensionview_->setReferences( from_reference, to_reference );

	  connect( from_reference, SIGNAL( modifiedPosition() ),
		   SLOT( y0DmVRefModified() ) );
	  connect( to_reference, SIGNAL( modifiedPosition() ),
		   SLOT( y0DmVRefModified() ) );
	  connect( from_reference, SIGNAL( destroyedGeometry() ),
		   SLOT( y0DestroyedDmVRef() ) );
	  connect( to_reference, SIGNAL( destroyedGeometry() ),
		   SLOT( y0DestroyedDmVRef() ) );
	}
      }
    }
    else { // we don't need a dimension view
      if ( y0_dimensionview_ != 0 ) { // remove it if we have one
	QDomDocument* xml_doc = ConstraintHistory::instance().history();
	if ( xml_doc != 0 ) {
	  QDomElement xml_rep = xml_doc->createElement( lC::STR::DIMENSION_VIEW );
	  xml_rep.setAttribute( lC::STR::URL, rectangle_->dbURL().toString(true) );
	  y0_dimensionview_->write( xml_rep );
	  ConstraintHistory::instance().appendUnconstraint( xml_rep );
	}

	disconnect( y0_dimensionview_->dimension(), SIGNAL( modifiedPosition() ),
		    this, SLOT( y0DmVDimModified() ) );
	disconnect( y0_dimensionview_->fromReference(), SIGNAL( modifiedPosition()),
		    this, SLOT( y0DmVRefModified() ) );
	disconnect( y0_dimensionview_->toReference(), SIGNAL( modifiedPosition() ),
		    this, SLOT( y0DmVRefModified() ) );
	disconnect( y0_dimensionview_->fromReference(), SIGNAL(destroyedGeometry()),
		    this, SLOT( y0DestroyedDmVRef() ) );
	disconnect( y0_dimensionview_->toReference(), SIGNAL(destroyedGeometry()),
		    this, SLOT( y0DestroyedDmVRef() ) );

	if ( y0_dimensionview_->isActivated() )
	  parent()->deactivateFigure( selectionName(),
				      y0_dimensionview_->selectionName() );

	dimensionview_objects_.remove( y0_dimensionview_->selectionName() );

	delete y0_dimensionview_;

	y0_dimensionview_ = 0;
      }
    }
  }
  /*!
   * This routine should only respond when y0 moves AND has an offset
   * constraint (i.e., this SLOT is only connected when the dimension view
   * exists). We might have to change the references of the dimension view
   * if y0 is the height-like offset and the reference is no longer
   * the correct edge for the width annotation side.
   */
  void RectangleView::y0DmVDimModified ( void )
  {
    if ( rectangle_->y0()->reference() == rectangle_->y1() ) {
      Line* height_reference;
      switch ( height_annotation_side_ ) {
      case lC::ABOVE:
	height_reference = rectangle_->right(); break;
      case lC::BELOW:
	height_reference = rectangle_->left(); break;
      }
      // If the reference edges don't already correspond to the
      // annotation side, replace it.
      if ( height_reference != y0_dimensionview_->fromReference() ||
	   height_reference != y0_dimensionview_->toReference() ) {

	disconnect( y0_dimensionview_->fromReference(), SIGNAL( modifiedPosition()),
		    this, SLOT( y0DmVRefModified() ) );
	disconnect( y0_dimensionview_->toReference(), SIGNAL( modifiedPosition() ),
		    this, SLOT( y0DmVRefModified() ) );
	disconnect( y0_dimensionview_->fromReference(), SIGNAL(destroyedGeometry()),
		    this, SLOT( y0DestroyedDmVRef() ) );
	disconnect( y0_dimensionview_->toReference(), SIGNAL(destroyedGeometry()),
		    this, SLOT( y0DestroyedDmVRef() ) );

	y0_dimensionview_->setReferences( height_reference, height_reference );

	connect( height_reference, SIGNAL( modifiedPosition() ),
		 SLOT( y0DmVRefModified() ) );
	connect( height_reference, SIGNAL( destroyedGeometry() ),
		 SLOT( y0DestroyedDmVRef() ) );
      }
    }

    y0_dimensionview_->computeLayout();
  }
  /*!
   * This routine should only respond when the references of y0's dimension
   * view move.
   */
  void RectangleView::y0DmVRefModified ( void )
  {
    y0_dimensionview_->computeLayout();
  }
  /*!
   * x1's constraint has been modified. Make the dimension view consistent
   * with the new constraint. (Might want to figure out if we can reduce
   * some of the code here, but it's all slightly different from case to
   * case...)
   */
  void RectangleView::x1ModifiedConstraint ( void )
  {
    // First, determine if we even need a dimension view
    if ( rectangle_->x1()->needsDimensionView() ) {
      // If we didn't have one before, create one
      if ( x1_dimensionview_ == 0 ) {
	// If x1 is relative to x0, create a width-like dimension. Use the
	// width annotation side setting to determine the reference edge.
	if ( rectangle_->x1()->reference() == rectangle_->x0() ) {
	  Line* width_reference;
	  switch ( width_annotation_side_ ) {
	  case lC::ABOVE:
	    width_reference = rectangle_->top(); break;
	  case lC::BELOW:
	    width_reference = rectangle_->bottom(); break;
	  }

	  x1_dimensionview_ = new DimensionView( rectangle_->x1(),
						 width_reference,
						 width_reference,
						 lC::HORIZONTAL, lC::ABOVE,
						 view()->genSelectionName(),
						 this );

	  dimensionview_objects_.insert( x1_dimensionview_->selectionName(),
					 x1_dimensionview_ );

	  connect( rectangle_->x1(), SIGNAL( modifiedPosition() ),
		   SLOT( x1DmVDimModified() ) );
	  // Slight optimization since both references are the same
	  connect( width_reference, SIGNAL( modifiedPosition() ),
		   SLOT( x1DmVRefModified() ) );
	  connect( width_reference, SIGNAL( destroyedGeometry() ),
		   SLOT( x1DestroyedDmVRef() ) );
	}
	// x1 is not related to x0 and we have to do some convoluted things
	// to figure out decent references. (and more is required...)
	else {
	  Curve* to_reference = rectangle_->top();
	  // So, ask the parent (Figure) of x1's reference for a
	  // orthogonal edge which is close something (use the origin
	  // for now)...
	  // Conceivably a reference line or centerline
	  // might not be able to supply such a thing...
	  Vertex v = rectangle_->x1()->reference()->parent()->
	    closestVertex( rectangle_->origin(), rectangle_->x1() );
	  Curve* from_reference = v.curve2_;

	  x1_dimensionview_ = new DimensionView( rectangle_->x1(),
						 from_reference,
						 to_reference,
						 lC::HORIZONTAL, lC::ABOVE,
						 view()->genSelectionName(),
						 this );

	  dimensionview_objects_.insert( x1_dimensionview_->selectionName(),
					 x1_dimensionview_ );

	  connect( rectangle_->x1(), SIGNAL( modifiedPosition() ),
		   SLOT( x1DmVDimModified() ) );
	  connect( to_reference, SIGNAL( modifiedPosition() ),
		   SLOT( x1DmVRefModified() ) );
	  connect( from_reference, SIGNAL( modifiedPosition() ),
		   SLOT( x1DmVRefModified() ) );
	  connect( from_reference, SIGNAL( destroyedGeometry() ),
		   SLOT( x1DestroyedDmVRef() ) );
	  connect( to_reference, SIGNAL( destroyedGeometry() ),
		   SLOT( x1DestroyedDmVRef() ) );
	}
      }
      else { // A dimension view already exists, just recycle it.
	// If x1 is relative to x0, create a width-like dimension. Use the
	// width annotation side setting to determine the reference edge.
	if ( rectangle_->x1()->reference() == rectangle_->x0() ) {
	  Line* width_reference;
	  switch ( width_annotation_side_ ) {
	  case lC::ABOVE:
	    width_reference = rectangle_->top(); break;
	  case lC::BELOW:
	    width_reference = rectangle_->bottom(); break;
	  }

	  disconnect( x1_dimensionview_->fromReference(),SIGNAL(modifiedPosition()),
		      this, SLOT( x1DmVRefModified() ) );
	  disconnect( x1_dimensionview_->toReference(), SIGNAL( modifiedPosition()),
		      this, SLOT( x1DmVRefModified() ) );
	  disconnect(x1_dimensionview_->fromReference(),SIGNAL(destroyedGeometry()),
		     this, SLOT( x1DestroyedDmVRef() ) );
	  disconnect( x1_dimensionview_->toReference(), SIGNAL(destroyedGeometry()),
		      this, SLOT( x1DestroyedDmVRef() ) );

	  x1_dimensionview_->setReferences( width_reference, width_reference );

	  connect( width_reference, SIGNAL( modifiedPosition() ),
		   SLOT( x1DmVRefModified() ) );
	  connect( width_reference, SIGNAL( destroyedGeometry() ),
		   SLOT( x1DestroyedDmVRef() ) );
	}
	// x1 is not related to x0 and we have to do some convoluted things
	// to figure out decent references. (and more is required...)
	else {
	  Curve* to_reference = rectangle_->top();
	  // So, ask the parent (Figure) of x0's reference for a
	  // orthogonal edge which is close to something (current the
	  // rectangle's origin)...
	  // Conceivably a reference line or centerline
	  // might not be able to supply such a thing...
	  Vertex v = rectangle_->x1()->reference()->parent()->
	    closestVertex( rectangle_->origin(), rectangle_->x1() );
	  Curve* from_reference = v.curve2_;

	  disconnect( x1_dimensionview_->fromReference(),SIGNAL(modifiedPosition()),
		      this, SLOT( x1DmVRefModified() ) );
	  disconnect( x1_dimensionview_->toReference(), SIGNAL( modifiedPosition()),
		      this, SLOT( x1DmVRefModified() ) );
	  disconnect(x1_dimensionview_->fromReference(),SIGNAL(destroyedGeometry()),
		     this, SLOT( x1DestroyedDmVRef() ) );
	  disconnect( x1_dimensionview_->toReference(), SIGNAL(destroyedGeometry()),
		      this, SLOT( x1DestroyedDmVRef() ) );

	  x1_dimensionview_->setReferences( from_reference, to_reference );

	  connect( from_reference, SIGNAL( modifiedPosition() ),
		   SLOT( x1DmVRefModified() ) );
	  connect( to_reference, SIGNAL( modifiedPosition() ),
		   SLOT( x1DmVRefModified() ) );
	  connect( from_reference, SIGNAL( destroyedGeometry() ),
		   SLOT( x1DestroyedDmVRef() ) );
	  connect( to_reference, SIGNAL( destroyedGeometry() ),
		   SLOT( x1DestroyedDmVRef() ) );
	}
      }
    }
    else { // we don't need a dimension view
      if ( x1_dimensionview_ != 0 ) { // remove it if we have one
	QDomDocument* xml_doc = ConstraintHistory::instance().history();
	if ( xml_doc != 0 ) {
	  QDomElement xml_rep = xml_doc->createElement( lC::STR::DIMENSION_VIEW );
	  xml_rep.setAttribute( lC::STR::URL, rectangle_->dbURL().toString(true) );
	  x1_dimensionview_->write( xml_rep );
	  ConstraintHistory::instance().appendUnconstraint( xml_rep );
	}

	disconnect( x1_dimensionview_->dimension(), SIGNAL( modifiedPosition() ),
		    this, SLOT( x1DmVDimModified() ) );
	disconnect( x1_dimensionview_->fromReference(), SIGNAL( modifiedPosition()),
		    this, SLOT( x1DmVRefModified() ) );
	disconnect( x1_dimensionview_->toReference(), SIGNAL( modifiedPosition() ),
		    this, SLOT( x1DmVRefModified() ) );
	disconnect( x1_dimensionview_->fromReference(), SIGNAL(destroyedGeometry()),
		    this, SLOT( x1DestroyedDmVRef() ) );
	disconnect( x1_dimensionview_->toReference(), SIGNAL(destroyedGeometry()),
		    this, SLOT( x1DestroyedDmVRef() ) );

	if ( x1_dimensionview_->isActivated() )
	  parent()->deactivateFigure( selectionName(),
				      x1_dimensionview_->selectionName() );

	dimensionview_objects_.remove( x1_dimensionview_->selectionName() );

	delete x1_dimensionview_;

	x1_dimensionview_ = 0;
      }
    }
  }
  /*!
   * This routine should only respond when x1 moves AND has an offset
   * constraint (i.e., this SLOT is only connected when the dimension view
   * exists). We might have to change the references of the dimension view
   * if x1 is the width-like offset and the reference is no longer
   * the correct edge for the width annotation side. (Hmm...)
   */
  void RectangleView::x1DmVDimModified ( void )
  {
    if ( rectangle_->x1()->reference() == rectangle_->x0() ) {
      Line* width_reference;
      switch ( width_annotation_side_ ) {
      case lC::ABOVE:
	width_reference = rectangle_->top(); break;
      case lC::BELOW:
	width_reference = rectangle_->bottom(); break;
      }
      // If the reference edges don't already correspond to the
      // annotation side, replace it.
      if ( width_reference != x1_dimensionview_->fromReference() ||
	   width_reference != x1_dimensionview_->toReference() ) {

	disconnect( x1_dimensionview_->fromReference(), SIGNAL( modifiedPosition()),
		    this, SLOT( x1DmVRefModified() ) );
	disconnect( x1_dimensionview_->toReference(), SIGNAL( modifiedPosition() ),
		    this, SLOT( x1DmVRefModified() ) );
	disconnect( x1_dimensionview_->fromReference(), SIGNAL(destroyedGeometry()),
		    this, SLOT( x1DestroyedDmVRef() ) );
	disconnect( x1_dimensionview_->toReference(), SIGNAL(destroyedGeometry()),
		    this, SLOT( x1DestroyedDmVRef() ) );

	x1_dimensionview_->setReferences( width_reference, width_reference );

	connect( width_reference, SIGNAL( modifiedPosition() ),
		 SLOT( x1DmVRefModified() ) );
	connect( width_reference, SIGNAL( destroyedGeometry() ),
		 SLOT( x1DestroyedDmVRef() ) );
      }
    }

    x1_dimensionview_->computeLayout();
  }
  /*!
   * This routine should only respond when the references of x1's dimension
   * view move.
   */
  void RectangleView::x1DmVRefModified ( void )
  {
    x1_dimensionview_->computeLayout();
  }
  /*!
   * y1's constraint has been modified. Make the dimension view consistent
   * with the new constraint. (Might want to figure out if we can reduce
   * some of the code here, but it's all slightly different from case to
   * case...)
   */
  void RectangleView::y1ModifiedConstraint ( void )
  {
    // First, determine if we even need a dimension view
    if ( rectangle_->y1()->needsDimensionView() ) {
      // If we didn't have one before, create one
      if ( y1_dimensionview_ == 0 ) {
	// If y1 is relative to y0, create a height-like dimension. Use the
	// annotation side setting to determine the reference edge.
	if ( rectangle_->y1()->reference() == rectangle_->y0() ) {
	  Line* height_reference;
	  switch ( height_annotation_side_ ) {
	  case lC::ABOVE:
	    height_reference = rectangle_->right(); break;
	  case lC::BELOW:
	    height_reference = rectangle_->left(); break;
	  }

	  y1_dimensionview_ = new DimensionView( rectangle_->y1(),
						 height_reference,
						 height_reference,
						 lC::VERTICAL, lC::ABOVE,
						 view()->genSelectionName(),
						 this );

	  dimensionview_objects_.insert( y1_dimensionview_->selectionName(),
					 y1_dimensionview_ );

	  connect( rectangle_->y1(), SIGNAL( modifiedPosition() ),
		   this, SLOT( y1DmVDimModified() ) );
	  // Slight optimization since both references are the same
	  connect( height_reference, SIGNAL( modifiedPosition() ),
		   this, SLOT( y1DmVRefModified() ) );
	  connect( height_reference, SIGNAL( destroyedGeometry() ),
		   SLOT( y1DestroyedDmVRef() ) );
	}
	// y0 is not related to y1 and we have to do some convoluted things
	// to figure out decent references. (and more is required...)
	else {
	  Curve* to_reference = rectangle_->right();
	  // So, ask the parent (Figure) of y1's reference for a
	  // orthogonal edge which is close to someting (currently the
	  // rectangle's origin)...
	  // (recall that the rectangle's origin is at the intersection
	  // of x0 and y0). Conceivably a reference line or centerline
	  // might not be able to supply such a thing...
	  Vertex v = rectangle_->y1()->reference()->parent()->
	    closestVertex( rectangle_->origin(), rectangle_->y1() );
	  Curve* from_reference = v.curve2_;

	  y1_dimensionview_ = new DimensionView( rectangle_->y1(),
						 from_reference,
						 to_reference,
						 lC::HORIZONTAL, lC::ABOVE,
						 view()->genSelectionName(),
						 this );

	  dimensionview_objects_.insert( y1_dimensionview_->selectionName(),
					 y1_dimensionview_ );

	  connect( rectangle_->y1(), SIGNAL( modifiedPosition() ),
		   SLOT( y1DmVDimModified() ) );
	  connect( from_reference, SIGNAL( modifiedPosition() ),
		   SLOT( y1DmVRefModified() ) );
	  connect( to_reference, SIGNAL( modifiedPosition() ),
		   SLOT( y1DmVRefModified() ) );
	  connect( from_reference, SIGNAL( destroyedGeometry() ),
		   SLOT( y1DestroyedDmVRef() ) );
	  connect( to_reference, SIGNAL( destroyedGeometry() ),
		   SLOT( y1DestroyedDmVRef() ) );
	}
      }
      else { // A dimension view already exists, just recycle it.
	// If y1 is relative to y0, create a height-like dimension. Use the
	// height annotation side setting to determine the reference edge.
	if ( rectangle_->y1()->reference() == rectangle_->y0() ) {
	  Line* height_reference;
	  switch ( height_annotation_side_ ) {
	  case lC::ABOVE:
	    height_reference = rectangle_->right(); break;
	  case lC::BELOW:
	    height_reference = rectangle_->left(); break;
	  }

	  disconnect( y1_dimensionview_->fromReference(),SIGNAL(modifiedPosition()),
		      this, SLOT( y1DmVRefModified() ) );
	  disconnect( y1_dimensionview_->toReference(), SIGNAL( modifiedPosition()),
		      this, SLOT( y1DmVRefModified() ) );
	  disconnect(y1_dimensionview_->fromReference(),SIGNAL(destroyedGeometry()),
		     this, SLOT( y1DestroyedDmVRef() ) );
	  disconnect( y1_dimensionview_->toReference(), SIGNAL(destroyedGeometry()),
		      this, SLOT( y1DestroyedDmVRef() ) );

	  y1_dimensionview_->setReferences( height_reference, height_reference );

	  connect( height_reference, SIGNAL( modifiedPosition() ),
		   SLOT( y1DmVRefModified() ) );
	  connect( height_reference, SIGNAL( destroyedGeometry() ),
		   SLOT( y1DestroyedDmVRef() ) );
	}
	// y0 is not related to y1 and we have to do some convoluted things
	// to figure out decent references. (and more is required...)
	else {
	  Curve* to_reference = rectangle_->right();
	  // So, ask the parent (Figure) of y1's reference for a
	  // orthogonal edge which is close to something (currently the
	  // rectangle's origin)...
	  // (recall that the rectangle's origin is at the intersection
	  // of x0 and y0). Conceivably a reference line or centerline
	  // might not be able to supply such a thing...
	  Vertex v = rectangle_->y1()->reference()->parent()->
	    closestVertex( rectangle_->origin(), rectangle_->y1() );
	  Curve* from_reference = v.curve2_;

	  disconnect( y1_dimensionview_->fromReference(),SIGNAL(modifiedPosition()),
		      this, SLOT( y1DmVRefModified() ) );
	  disconnect( y1_dimensionview_->toReference(), SIGNAL( modifiedPosition()),
		      this, SLOT( y1DmVRefModified() ) );
	  disconnect(y1_dimensionview_->fromReference(),SIGNAL(destroyedGeometry()),
		     this, SLOT( y1DestroyedDmVRef() ) );
	  disconnect( y1_dimensionview_->toReference(), SIGNAL(destroyedGeometry()),
		      this, SLOT( y1DestroyedDmVRef() ) );

	  y1_dimensionview_->setReferences( from_reference, to_reference );

	  connect( from_reference, SIGNAL( modifiedPosition() ),
		   SLOT( y1DmVRefModified() ) );
	  connect( to_reference, SIGNAL( modifiedPosition() ),
		   SLOT( y1DmVRefModified() ) );
	  connect( from_reference, SIGNAL( destroyedGeometry() ),
		   SLOT( y1DestroyedDmVRef() ) );
	  connect( to_reference, SIGNAL( destroyedGeometry() ),
		   SLOT( y1DestroyedDmVRef() ) );
	}
      }
    }
    else { // we don't need a dimension view
      if ( y1_dimensionview_ != 0 ) { // remove it if we have one
	QDomDocument* xml_doc = ConstraintHistory::instance().history();
	if ( xml_doc != 0 ) {
	  QDomElement xml_rep = xml_doc->createElement( lC::STR::DIMENSION_VIEW );
	  xml_rep.setAttribute( lC::STR::URL, rectangle_->dbURL().toString(true) );
	  y1_dimensionview_->write( xml_rep );
	  ConstraintHistory::instance().appendUnconstraint( xml_rep );
	}

	disconnect( y1_dimensionview_->dimension(), SIGNAL( modifiedPosition() ),
		    this, SLOT( y1DmVDimModified() ) );
	disconnect( y1_dimensionview_->fromReference(), SIGNAL( modifiedPosition()),
		    this, SLOT( y1DmVRefModified() ) );
	disconnect( y1_dimensionview_->toReference(), SIGNAL( modifiedPosition() ),
		    this, SLOT( y1DmVRefModified() ) );
	disconnect( y1_dimensionview_->fromReference(), SIGNAL(destroyedGeometry()),
		    this, SLOT( y1DestroyedDmVRef() ) );
	disconnect( y1_dimensionview_->toReference(), SIGNAL(destroyedGeometry()),
		    this, SLOT( y1DestroyedDmVRef() ) );

	if ( y1_dimensionview_->isActivated() )
	  parent()->deactivateFigure( selectionName(),
				      y1_dimensionview_->selectionName() );

	dimensionview_objects_.remove( y1_dimensionview_->selectionName() );

	delete y1_dimensionview_;

	y1_dimensionview_ = 0;
      }
    }
  }
  /*!
   * This routine should only respond when y1 moves AND has an offset
   * constraint (i.e., this SLOT is only connected when the dimension view
   * exists). We might have to change the references of the dimension view
   * if y1 is the height-like offset and the reference is no longer
   * the correct edge for the height annotation side. (Hmm...)
   */
  void RectangleView::y1DmVDimModified ( void )
  {
    if ( rectangle_->y1()->reference() == rectangle_->y0() ) {
      Line* height_reference;
      switch ( height_annotation_side_ ) {
      case lC::ABOVE:
	height_reference = rectangle_->right(); break;
      case lC::BELOW:
	height_reference = rectangle_->left(); break;
      }
      // If the reference edges don't already correspond to the
      // annotation side, replace it.
      if ( height_reference != y1_dimensionview_->fromReference() ||
	   height_reference != y1_dimensionview_->toReference() ) {

	disconnect( y1_dimensionview_->fromReference(), SIGNAL( modifiedPosition()),
		    this, SLOT( y1DmVRefModified() ) );
	disconnect( y1_dimensionview_->toReference(), SIGNAL( modifiedPosition() ),
		    this, SLOT( y1DmVRefModified() ) );
	disconnect( y1_dimensionview_->fromReference(), SIGNAL(destroyedGeometry()),
		    this, SLOT( y1DestroyedDmVRef() ) );
	disconnect( y1_dimensionview_->toReference(), SIGNAL(destroyedGeometry()),
		    this, SLOT( y1DestroyedDmVRef() ) );

	y1_dimensionview_->setReferences( height_reference, height_reference );

	connect( height_reference, SIGNAL( modifiedPosition() ),
		 SLOT( y1DmVRefModified() ) );
	connect( height_reference, SIGNAL( destroyedGeometry() ),
		 SLOT( y1DestroyedDmVRef() ) );
      }
    }

    y1_dimensionview_->computeLayout();
  }
  /*!
   * This routine should only respond when the references of y1's dimension
   * view move.
   */
  void RectangleView::y1DmVRefModified ( void )
  {
    y1_dimensionview_->computeLayout();
  }
  /*!
   * If the dimension view's reference is deleted, then save the attributes
   * of this dimension in case the action is undone later.
   */
  void RectangleView::x0DestroyedDmVRef ( void )
  {
    QDomDocument* xml_doc = ConstraintHistory::instance().history();
    if ( xml_doc != 0 ) {
      QDomElement xml_rep = xml_doc->createElement( lC::STR::DIMENSION_VIEW );
      xml_rep.setAttribute( lC::STR::URL, rectangle_->dbURL().toString(true) );
      x0_dimensionview_->write( xml_rep );
      ConstraintHistory::instance().appendUnconstraint( xml_rep );
    }

    disconnect( rectangle_->x0(), SIGNAL( modifiedPosition() ),
		this, SLOT( x0DmVDimModified() ) );
    disconnect( x0_dimensionview_->fromReference(), SIGNAL( modifiedPosition() ),
		this, SLOT( x0DmVRefModified() ) );
    disconnect( x0_dimensionview_->fromReference(), SIGNAL( destroyedGeometry() ),
		this, SLOT( x0DestroyedDmVRef() ) );
    disconnect( x0_dimensionview_->toReference(), SIGNAL( modifiedPosition() ),
		this, SLOT( x0DmVRefModified() ) );
    disconnect( x0_dimensionview_->toReference(), SIGNAL( destroyedGeometry() ),
		this, SLOT( x0DestroyedDmVRef() ) );

    delete x0_dimensionview_;

    x0_dimensionview_ = 0;
  }

  void RectangleView::y0DestroyedDmVRef ( void )
  {
    QDomDocument* xml_doc = ConstraintHistory::instance().history();
    if ( xml_doc != 0 ) {
      QDomElement xml_rep = xml_doc->createElement( lC::STR::DIMENSION_VIEW );
      xml_rep.setAttribute( lC::STR::URL, rectangle_->dbURL().toString(true) );
      y0_dimensionview_->write( xml_rep );
      ConstraintHistory::instance().appendUnconstraint( xml_rep );
    }

    disconnect( rectangle_->y0(), SIGNAL( modifiedPosition() ),
		this, SLOT( y0DmVDimModified() ) );
    disconnect( y0_dimensionview_->fromReference(), SIGNAL( modifiedPosition() ),
		this, SLOT( y0DmVRefModified() ) );
    disconnect( y0_dimensionview_->fromReference(), SIGNAL( destroyedGeometry() ),
		this, SLOT( y0DestroyedDmVRef() ) );
    disconnect( y0_dimensionview_->toReference(), SIGNAL( modifiedPosition() ),
		this, SLOT( y0DmVRefModified() ) );
    disconnect( y0_dimensionview_->toReference(), SIGNAL( destroyedGeometry() ),
		this, SLOT( y0DestroyedDmVRef() ) );

    delete y0_dimensionview_;

    y0_dimensionview_ = 0;
  }

  void RectangleView::x1DestroyedDmVRef ( void )
  {
    QDomDocument* xml_doc = ConstraintHistory::instance().history();
    if ( xml_doc != 0 ) {
      QDomElement xml_rep = xml_doc->createElement( lC::STR::DIMENSION_VIEW );
      xml_rep.setAttribute( lC::STR::URL, rectangle_->dbURL().toString(true) );
      x1_dimensionview_->write( xml_rep );
      ConstraintHistory::instance().appendUnconstraint( xml_rep );
    }

    disconnect( rectangle_->x1(), SIGNAL( modifiedPosition() ),
		this, SLOT( x1DmVDimModified() ) );
    disconnect( x1_dimensionview_->fromReference(), SIGNAL( modifiedPosition() ),
		this, SLOT( x1DmVRefModified() ) );
    disconnect( x1_dimensionview_->fromReference(), SIGNAL( destroyedGeometry() ),
		this, SLOT( x1DestroyedDmVRef() ) );
    disconnect( x1_dimensionview_->toReference(), SIGNAL( modifiedPosition() ),
		this, SLOT( x1DmVRefModified() ) );
    disconnect( x1_dimensionview_->toReference(), SIGNAL( destroyedGeometry() ),
		this, SLOT( x1DestroyedDmVRef() ) );

    delete x1_dimensionview_;

    x1_dimensionview_ = 0;
  }

  void RectangleView::y1DestroyedDmVRef ( void )
  {
    QDomDocument* xml_doc = ConstraintHistory::instance().history();
    if ( xml_doc != 0 ) {
      QDomElement xml_rep = xml_doc->createElement( lC::STR::DIMENSION_VIEW );
      xml_rep.setAttribute( lC::STR::URL, rectangle_->dbURL().toString(true) );
      y1_dimensionview_->write( xml_rep );
      ConstraintHistory::instance().appendUnconstraint( xml_rep );
    }

    disconnect( rectangle_->y1(), SIGNAL( modifiedPosition() ),
		this, SLOT( y1DmVDimModified() ) );
    disconnect( y1_dimensionview_->fromReference(), SIGNAL( modifiedPosition() ),
		this, SLOT( y1DmVRefModified() ) );
    disconnect( y1_dimensionview_->fromReference(), SIGNAL( destroyedGeometry() ),
		this, SLOT( y1DestroyedDmVRef() ) );
    disconnect( y1_dimensionview_->toReference(), SIGNAL( modifiedPosition() ),
		this, SLOT( y1DmVRefModified() ) );
    disconnect( y1_dimensionview_->toReference(), SIGNAL( destroyedGeometry() ),
		this, SLOT( y1DestroyedDmVRef() ) );

    delete y1_dimensionview_;

    y1_dimensionview_ = 0;
  }

  // Recompute any layout due to changes in the view.

  void RectangleView::viewAttributeChanged ( void )
  {
    updateDimensionViews();
  }

   // Change the name of the rectangle.

  void RectangleView::setName ( const QString& name )
  {
    DBURL old_db_url = rectangle_->dbURL();
    
    rectangle_->setName( name );

    CommandHistory::instance().
      addCommand( new RenameCommand( "rename rectangle", model(), old_db_url,
				     rectangle_->dbURL() ) );
  }

  // Check a list view rename for uniqueness.

  void RectangleView::listNameChanged ( const QString& name )
  {
    int ret = parent()->uniqueFigureName( this, name, rectangle_->type() );

    switch ( ret ) {
    case lC::OK:
      setName( name );
      break;
    case lC::Redo:
      list_view_item_->startRename( lC::NAME );
    case lC::Rejected:
      updateName( rectangle_->name() ); // Repaint list item with old name.
    }
  }

  // Check to see if the edge name is unique.

  lC::RenameStatus RectangleView::uniqueEdgeName ( const QString& name,
						   const QString& role ) const
  {
    // role lets us skip the edge being renamed.
    if ( ( name == lC::formatName( rectangle_->x0()->name() )
	   && role != lC::STR::X0 ) ||
	 ( name == lC::formatName( rectangle_->y0()->name() )
	   && role != lC::STR::Y0 ) ||
	 ( name == lC::formatName( rectangle_->x1()->name() )
	   && role != lC::STR::X1 ) ||
	 ( name == lC::formatName( rectangle_->y1()->name() )
	   && role != lC::STR::Y1 ) ) {
      QMessageBox mb( trC( lC::STR::LIGNUMCAD ),
		      tr( "The name \"%1\" for an edge already exists." ).
		      arg( name ),
		      QMessageBox::Warning,
		      QMessageBox::Yes | QMessageBox::Default,
		      QMessageBox::Cancel,
		      QMessageBox::NoButton );
      mb.setButtonText( QMessageBox::Yes, tr( "Enter another name" ) );
      mb.setButtonText( QMessageBox::Cancel, tr( "Cancel edge edit" ) );

      switch ( mb.exec() ) {
      case QMessageBox::Yes:
	return lC::Redo;
      case QMessageBox::Cancel:
	return lC::Rejected;
      }
    }
    return lC::OK;
  }

  // Change the name of the x0 edge of rectangle.

  void RectangleView::x0SetName ( const QString& name )
  {
    switch ( uniqueEdgeName( name, lC::STR::X0 ) ) {
    case lC::Redo:
      x0_list_view_->startRename( lC::NAME );
    case lC::Rejected:
      x0UpdateName( rectangle_->x0()->name() );
      return;
    }

    DBURL old_db_url = rectangle_->x0()->dbURL();

    rectangle_->x0()->setName( name );

    CommandHistory::instance().
      addCommand( new RenameCommand( "rename edge", model(), old_db_url,
				     rectangle_->x0()->dbURL() ) );
  }

   // Change the name of the y0 edge of rectangle.

  void RectangleView::y0SetName ( const QString& name )
  {
    switch ( uniqueEdgeName( name, lC::STR::Y0 ) ) {
    case lC::Redo:
      y0_list_view_->startRename( lC::NAME );
    case lC::Rejected:
      y0UpdateName( rectangle_->y0()->name() );
      return;
    }

    DBURL old_db_url = rectangle_->y0()->dbURL();
    
    rectangle_->y0()->setName( name );

    CommandHistory::instance().
      addCommand( new RenameCommand( "rename edge", model(), old_db_url,
				     rectangle_->y0()->dbURL() ) );
  }
  /*!
   * Change the name of the x1 edge of rectangle.
   */
  void RectangleView::x1SetName ( const QString& name )
  {
    switch ( uniqueEdgeName( name, lC::STR::X1 ) ) {
    case lC::Redo:
      x1_list_view_->startRename( lC::NAME );
    case lC::Rejected:
      x1UpdateName( rectangle_->x1()->name() );
      return;
    }

    DBURL old_db_url = rectangle_->x1()->dbURL();

    rectangle_->x1()->setName( name );

    CommandHistory::instance().
      addCommand( new RenameCommand( "rename edge", model(), old_db_url,
				     rectangle_->x1()->dbURL() ) );
  }
  /*!
   * Change the name of the y1 edge of rectangle.
   */
  void RectangleView::y1SetName ( const QString& name )
  {
    switch ( uniqueEdgeName( name, lC::STR::Y1 ) ) {
    case lC::Redo:
      y1_list_view_->startRename( lC::NAME );
    case lC::Rejected:
      y1UpdateName( rectangle_->y1()->name() );
      return;
    }

    DBURL old_db_url = rectangle_->y1()->dbURL();

    rectangle_->y1()->setName( name );

    CommandHistory::instance().
      addCommand( new RenameCommand( "rename edge", model(), old_db_url,
				     rectangle_->y1()->dbURL() ) );
  }
  /*!
   * Update the static displays of the rectangle's name
   */
  void RectangleView::updateName ( const QString& /*name*/ )
  {
    list_view_item_->setText( lC::NAME, lC::formatName( rectangle_->name() )
			      + QString( " <%1>" ).arg( rectangle_->id() ) );
  }

  /*!
   * Detect changes to the name of x0 initiated elsewhere
   */
  void RectangleView::x0UpdateName ( const QString& /*name*/ )
  {
    x0_list_view_->setText( lC::NAME, lC::formatName( rectangle_->x0()->name() ) );
    x0_list_view_->setText( lC::DETAIL, rectangle_->x0()->detail() );
  }

  /*!
   * Detect changes to the name of y0 initiated elsewhere
   */
  void RectangleView::y0UpdateName ( const QString& /*name*/ )
  {
    y0_list_view_->setText( lC::NAME, lC::formatName( rectangle_->y0()->name() ) );
    y0_list_view_->setText( lC::DETAIL, rectangle_->y0()->detail() );
  }

  /*!
   * Detect changes to the name of x1 initiated elsewhere
   */
  void RectangleView::x1UpdateName ( const QString& /*name*/ )
  {
    x1_list_view_->setText( lC::NAME, lC::formatName( rectangle_->x1()->name() ) );
    x1_list_view_->setText( lC::DETAIL, rectangle_->x1()->detail() );
  }

  /*!
   * Detect changes to the name of y1 initiated elsewhere
   */
  void RectangleView::y1UpdateName ( const QString& /*name*/ )
  {
    y1_list_view_->setText( lC::NAME, lC::formatName( rectangle_->y1()->name() ) );
    y1_list_view_->setText( lC::DETAIL, rectangle_->y1()->detail() );
  }

  /*!
   * Detects movement of x0
   */
  void RectangleView::x0ModifiedPosition ( void )
  {
    x0_list_view_->setText( lC::DETAIL, rectangle_->x0()->detail() );

    list_view_item_->setText( lC::DETAIL,
			     trC( lC::STR::RECTANGLE_FMT ).
			     arg( rectangle_->origin()[X] ).
			     arg( rectangle_->origin()[Y] ).
			     arg( rectangle_->width() ).
			     arg( rectangle_->height() ) );
  }

  /*!
   * Detects movement of y0
   */
  void RectangleView::y0ModifiedPosition ( void )
  {
    y0_list_view_->setText( lC::DETAIL, rectangle_->y0()->detail() );

    list_view_item_->setText( lC::DETAIL,
			     trC( lC::STR::RECTANGLE_FMT ).
			     arg( rectangle_->origin()[X] ).
			     arg( rectangle_->origin()[Y] ).
			     arg( rectangle_->width() ).
			     arg( rectangle_->height() ) );
  }

  /*!
   * Detects movement of x1
   */
  void RectangleView::x1ModifiedPosition ( void )
  {
    x1_list_view_->setText( lC::DETAIL, rectangle_->x1()->detail() );

    list_view_item_->setText( lC::DETAIL,
			     trC( lC::STR::RECTANGLE_FMT ).
			     arg( rectangle_->origin()[X] ).
			     arg( rectangle_->origin()[Y] ).
			     arg( rectangle_->width() ).
			     arg( rectangle_->height() ) );
  }

  /*!
   * Detects movement of y1
   */
  void RectangleView::y1ModifiedPosition ( void )
  {
    y1_list_view_->setText( lC::DETAIL, rectangle_->y1()->detail() );

    list_view_item_->setText( lC::DETAIL,
			     trC( lC::STR::RECTANGLE_FMT ).
			     arg( rectangle_->origin()[X] ).
			     arg( rectangle_->origin()[Y] ).
			     arg( rectangle_->width() ).
			     arg( rectangle_->height() ) );
  }
  /*
   * The least little change requires the dimension views to be recomputed
   * since they depend on the size of the paper units, and not so much
   * on the geometry (which scales automatically with the view, of course).
   */
  void RectangleView::updateDimensionViews ( void )
  {
    if ( x0_dimensionview_ != 0 ) x0_dimensionview_->computeLayout();
    if ( y0_dimensionview_ != 0 ) y0_dimensionview_->computeLayout();
    if ( x1_dimensionview_ != 0 ) x1_dimensionview_->computeLayout();
    if ( y1_dimensionview_ != 0 ) y1_dimensionview_->computeLayout();
  }
} // End of Space2D namespace
