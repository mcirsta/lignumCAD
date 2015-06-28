/*
 * referencelineview.cpp
 *
 * ReferenceLineView classes
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
#include <qlineedit.h>
#include <qradiobutton.h>
#include <qlabel.h>
#include <qcursor.h>

#include "constants.h"
#include "dburl.h"
#include "units.h"
#include "command.h"
#include "lignumcadmainwindow.h"
#include "cursorfactory.h"
#include "line.h"
#include "page.h"
#include "pageview.h"
#include "designbookview.h"
#include "model.h"
#include "openglview.h"
#include "alignment.h"
#include "handle.h"
#include "gl2ps.h"
#include "listviewitem.h"
#include "dimensionview.h"
#include "lcdefaultlengthspinbox.h"
#include "lcconstraintchooser.h"
#include "referenceline.h"
#include "referencelineinfodialog.h"
#include "referencelineview.h"

namespace Space2D {
  /*!
   * (Find a better name; maybe ReferenceLineViewMemento). Create
   * a memento of the current state of the reference line and its
   * view.
   */
  class ReferenceLineViewCreate : public CreateObject {
    //! When CommandHistory undoes or redoes the creation of this object,
    //! the Design Book view is the only handle which is required. Everything
    //! else can be looked up.
    DesignBookView* design_book_view_;
    //! The details of the reference line and view are stored as XML.
    QDomDocument xml_doc_;
  public:
  /*!
   * Create a memento of the current state of the reference line and its view.
   * \param reference_line_view view object of the reference line.
   */
    ReferenceLineViewCreate ( ReferenceLineView* reference_line_view )
    {
      design_book_view_ = reference_line_view->parent()->parent();

      QDomElement root = xml_doc_.createElement( lC::STR::MEMENTO );

      root.setAttribute( lC::STR::NAME,reference_line_view->dbURL().toString(true));

      reference_line_view->referenceLine()->write( root );
      reference_line_view->write( root );

      xml_doc_.appendChild( root );
    }
    //! Destructor doesn't do much.
    ~ReferenceLineViewCreate ( void ) {}
    /*!
     * (Re)create the reference line object and its view as specified in
     * the XML representation.
     */
    void create ( void )
    {
      QDomNodeList memento_list = xml_doc_.elementsByTagName( lC::STR::MEMENTO );

      if ( memento_list.length() > 0 ) {
	DBURL url = memento_list.item(0).toElement().attribute( lC::STR::NAME );

	// First, we have to find the parent page and view for the reference line.

	DBURL parent = url.parent();

	Page* page =
	  dynamic_cast<Page*>( design_book_view_->model()->lookup( parent ) );
	PageView* page_view =
	  dynamic_cast<PageView*>( design_book_view_->lookup( parent ) );

	// Now, we can extract the XML representations for the reference line
	// and its view
	QDomNodeList reference_line_list =
	  xml_doc_.elementsByTagName( lC::STR::REFERENCE_LINE );

	if ( reference_line_list.length() > 0 ) {

	  QDomNodeList reference_line_view_list =
	    xml_doc_.elementsByTagName( lC::STR::REFERENCE_LINE_VIEW );

	  if ( reference_line_view_list.length() > 0 ) {
	    uint reference_line_id =
	      reference_line_list.item(0).toElement().attribute( lC::STR::ID ).
	      toUInt();

	    new ReferenceLine( reference_line_id,
			       reference_line_list.item(0).toElement(), page );

	    page_view->
	      addFigureView( new ReferenceLineView( reference_line_view_list.
						    item(0).toElement(),
						    page_view ) );
	  }
	}

	// See if there are any constraints which should be put back
	// in place when this reference line is restored.
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
	    else if ( e.tagName() == lC::STR::DIMENSION_VIEW ) {
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
     * Delete the reference line object and its view as specified in the
     * XML representation.
     */
    void remove ( void )
    {
      QDomNodeList memento_list = xml_doc_.elementsByTagName( lC::STR::MEMENTO );

      if ( memento_list.length() > 0 ) {
	DBURL url = memento_list.item(0).toElement().attribute( lC::STR::NAME );

	ReferenceLineView* reference_line_view =
	  dynamic_cast<ReferenceLineView*>( design_book_view_->lookup( url ) );

	DBURL parent = url.parent();

	PageView* page_view =
	  dynamic_cast<PageView*>( design_book_view_->lookup( parent ) );

	page_view->removeFigureView( reference_line_view );
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

      QDomNodeList reference_line_list =
	xml_doc_.elementsByTagName( lC::STR::REFERENCE_LINE );

      if ( reference_line_list.length() > 0 ) {
	QDomNode element = xml_rep->importNode( reference_line_list.item(0), true );

	rep_root.appendChild( element );
      }

      QDomNodeList reference_line_view_list =
	xml_doc_.elementsByTagName( lC::STR::REFERENCE_LINE_VIEW );

      if ( reference_line_view_list.length() > 0 ) {
	QDomNode element = xml_rep->importNode( reference_line_view_list.item(0),
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
     * If the creation of a reference line is immediately followed by a rename,
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
	QString path = memento_list.item(0).toElement().attribute( lC::STR::NAME );

	if ( path != rename->oldDBURL().toString(true) )
	  return false;

	// Additional sanity check: make sure the object and its view have elements.

	QDomNodeList reference_line_list =
	  xml_doc_.elementsByTagName( lC::STR::REFERENCE_LINE );
	QDomNodeList reference_line_view_list =
	  xml_doc_.elementsByTagName( lC::STR::REFERENCE_LINE_VIEW );

	if ( reference_line_list.length() == 0 ||
	     reference_line_view_list.length() == 0 )
	  return false;

	// Update the name elements in the object and it's view.

	memento_list.item(0).toElement().setAttribute( lC::STR::NAME,
						       rename->newDBURL().toString(true) );

	reference_line_list.item(0).toElement().
	  setAttribute( lC::STR::NAME, rename->newDBURL().name() );

	reference_line_view_list.item(0).toElement().
	  setAttribute( lC::STR::REFERENCE_LINE, rename->newDBURL().toString(true) );

	return true;
      }

      return false;
    }
  };

  ReferenceLineCreateInput::ReferenceLineCreateInput( ReferenceLine* reference_line,
					    ReferenceLineView* reference_line_view )
    : context_menu_( 0 ), reference_line_( reference_line ),
      reference_line_view_( reference_line_view )
  {}

  void ReferenceLineCreateInput::startDisplay ( QPopupMenu* context_menu )
  {
    context_menu_ = context_menu;

    QAction* cancel_action =
      reference_line_view_->parent()->lCMW()->cancelReferenceLineAction;

    separator_id_ = context_menu_->insertSeparator();
    cancel_action->addTo( context_menu_ );
    connect( cancel_action, SIGNAL( activated() ), SLOT( cancelOperation() )  );

    reference_line_view_->view()->
      setCursor( CursorFactory::instance().cursor( CursorFactory::REFERENCELINE ) );
  }

  void ReferenceLineCreateInput::mousePrepress ( QMouseEvent* /*me*/,
						 const SelectedNames& /*selected*/ )
  {}

  void ReferenceLineCreateInput::mousePress ( QMouseEvent* me,
					      const SelectedNames& selected )
  {
    start_ = reference_line_view_->view()->unproject2D( me->pos() );

    UnitsBasis::instance()->round( start_ );

    Line* reference_line = 0;

    SelectedNames::const_iterator f;

    for ( f = selected.begin(); f != selected.end(); ++f ) {
      FigureView* fv =
	dynamic_cast< FigureView* >( reference_line_view_->parent()->
				     figureSelectionNames()[ (*f).second[0] ] );

      std::vector<GLuint>::const_iterator g;

      for ( g = (*f).second.begin(); g != (*f).second.end(); ++g ) {

	Line* line = dynamic_cast< Line* >( fv->geometry( *g ) );

	if ( line != 0 ) {
	  reference_line = line;
	}
      }
    }

    if ( reference_line != 0 ) {
      reference_line_->line()->setConstraint( new Coincident( reference_line ) );
    }
    else {
      reference_line_->line()->setConstraint( new Offset( start_, &X_AXIS ) );

      reference_line_view_->view()->
	setCursor( CursorFactory::instance().cursor( CursorFactory::LINE_ROTATE ) );
    }
  }

  void ReferenceLineCreateInput::mouseDrag ( QMouseEvent* me,
					     const SelectedNames& /*selected*/ )
  {
    ConstrainedLine* line = reference_line_->line();

    if ( line->isAdjustable() ) {
      Point current = reference_line_view_->view()->unproject2D( me->pos() );

      UnitsBasis::instance()->round( current );

      double orientation = (current - start_).orientation();

      // Currently, there're only horizontal and vertical reference lines
      if ( fabs( orientation ) <= 45. || fabs( orientation ) > 135. ) {
	if ( !line->isHorizontal() )
	  line->setReference( start_, &X_AXIS );
      }
      else {
	if ( !line->isVertical() )
	  line->setReference( start_, &Y_AXIS );
      }
    }
  }

  bool ReferenceLineCreateInput::mouseRelease ( QMouseEvent* /*me*/,
						const SelectedNames& /*selected*/)
  {
    PageView* page_view = reference_line_view_->parent();

    QAction* cancel_action = page_view->lCMW()->cancelReferenceLineAction;
    cancel_action->disconnect();
    cancel_action->removeFrom( context_menu_ );
    context_menu_->removeItem( separator_id_ );

    reference_line_->setComplete( true );

    reference_line_view_->unsetCursor();

    page_view->figureComplete();

    page_view->activateFigure( reference_line_view_ );

    CommandHistory::instance().
      addCommand( new CreateCommand( "create reference line",
				     reference_line_view_->memento() ) );

    return false;
  }

  void ReferenceLineCreateInput::keyPress ( QKeyEvent* ke )
  {
    if ( ke->key() == Qt::Key_Escape )
      cancelOperation();
    else
      ke->ignore();
  }

  void ReferenceLineCreateInput::cancelOperation ( void )
  {
    PageView* page_view = reference_line_view_->parent();

    QAction* cancel_action = page_view->lCMW()->cancelReferenceLineAction;

    cancel_action->disconnect();
    cancel_action->removeFrom( context_menu_ );
    context_menu_->removeItem( separator_id_ );

    page_view->removeFigureView( reference_line_view_ );

    page_view->cancelOperation();
  }

  ReferenceLineModifyInput::ReferenceLineModifyInput( ReferenceLine* reference_line,
					    ReferenceLineView* reference_line_view )
    : selection_type_( TRANSIENT, FIGURE ),
      reference_line_( reference_line ), reference_line_view_( reference_line_view),
      xml_rep_( 0 )
  {}

  /*
   * Tries to set the mouse cursor to a reasonable shape based on what
   * is highlighted. For reference lines, they can only move in one direction.
   * If *this* view is no longer highlighted, turn off any special cursor
   * shape imposed by this view.
   */
  void ReferenceLineModifyInput::mousePrepress ( QMouseEvent* me,
						 const SelectedNames& selected )
  {
    if ( selected.empty() ) {
      reference_line_view_->unsetCursor();
      return;
    }

    SelectedNames::const_iterator f;

    // For each highlighted figure view...

    for ( f = selected.begin(); f != selected.end(); ++f ) {

      // If the highlighted figure view is *this* view...

      if ( (*f).second[0] == reference_line_view_->selectionName() ) {

	// If the geometry of this view is highlighted...

	if ( (*f).second.size() > 1 ) {

	  GLuint g = (*f).second[1]; // (*(*f).second->rbegin()).first;

	  if ( reference_line_view_->isGeometry( g ) ) {

	    reference_line_view_->setCursor( g );
	    // This may have been called as a second chance action which
	    // another FigureView has already rejected.
	    me->accept();
	  }
	  else {
	    reference_line_view_->unsetCursor();
	  }

	  return; // Probably not the last word here
	}
      }
    }

    // If none of the highlighted figure views was this one, then
    // reset the cursor.

    reference_line_view_->unsetCursor();
  }

  void ReferenceLineModifyInput::mousePress ( QMouseEvent* me,
					      const SelectedNames& selected )
  {
    // Let the Page handle these events

    if ( me->button() != Qt::LeftButton || selected.empty() ) {
      me->ignore();
      return;
    }

    SelectedNames::const_iterator f;

    for ( f = selected.begin(); f != selected.end(); ++f ) {

      if ( (*f).second[0] == reference_line_view_->selectionName() ) {

	if ( (*f).second.size() > 1 &&
	     reference_line_view_->isGeometry( (*f).second[1] ) ) {

	  old_offset_ = reference_line_->line()->offset();

	  last_pnt_ = reference_line_view_->view()->unproject2D( me->pos() );

	  UnitsBasis::instance()->round( last_pnt_ );

	  start_pnt_ = last_pnt_;

	  // Search for other geometry's to align with.
	  selection_type_ = SelectionType( TRANSIENT, EDGE );

	  // I think this controls some state...
	  drag_handle_ = (*f).second[1]; //(*(*f).second->rbegin()).first;
	  return;
	}
      }
    }
    
    // If this view wasn't highlighted, then let also let the page handle it.

    me->ignore();
  }

  void ReferenceLineModifyInput::mouseDrag ( QMouseEvent* me,
					     const SelectedNames& /*selected*/ )
  {
    if ( drag_handle_ == 0 ) return;

    current_pnt_ = reference_line_view_->view()->unproject2D( me->pos() );

    UnitsBasis::instance()->round( current_pnt_ );

    reference_line_->line()->setPosition( current_pnt_ );

    last_pnt_ = current_pnt_;
	  
    reference_line_view_->parent()->figureModified();
  }

  bool ReferenceLineModifyInput::mouseRelease ( QMouseEvent* /*me*/,
						const SelectedNames& selected )
  {
    MoveLinesCommand* command = 0;

    if ( reference_line_->line()->offset() != old_offset_ ) {
      command = new MoveLinesCommand( "move reference line",
				      reference_line_view_->model() );

      command->add( reference_line_->line(), old_offset_ );
    }

    // See if we picked up a coincident constraint.

    Line* reference = 0;

    SelectedNames::const_iterator f;

    for ( f = selected.begin(); f != selected.end(); ++f ) {
      FigureView* fv =
	dynamic_cast< FigureView* >( reference_line_view_->parent()->
				     figureSelectionNames()[ (*f).second[0] ] );

      if ( fv == reference_line_view_ ) continue;

      std::vector<GLuint>::const_iterator g;

      for ( g = (*f).second.begin(); g != (*f).second.end(); ++g ) {

	Line* line = dynamic_cast< Line* >( fv->geometry( *g ) );

	if ( line != 0 ) {
	  if ( ( reference_line_->line()->isVertical() && line->isVertical() ) ||
	       ( reference_line_->line()->isHorizontal() && line->isHorizontal() ) )
	    reference = line;
	}
      }
    }

    if ( reference != 0 && !reference->dependsOn( reference_line_->line() ) ) {
      xml_rep_ = new QDomDocument;

      QDomElement reconstraints = xml_rep_->createElement( lC::STR::RECONSTRAINTS );
      xml_rep_->appendChild( reconstraints );

      QDomElement old_constraints = xml_rep_->createElement( lC::STR::OLD_CONSTRAINTS );
      reconstraints.appendChild( old_constraints );

      QDomElement new_constraints = xml_rep_->createElement( lC::STR::NEW_CONSTRAINTS );
      reconstraints.appendChild( new_constraints );

      QDomElement old_xml = xml_rep_->createElement( lC::STR::CONSTRAINED_LINE );

      old_xml.setAttribute( lC::STR::URL,
			    reference_line_->line()->dbURL().toString(true) );
      reference_line_->line()->constraint()->write( old_xml );
      old_constraints.appendChild( old_xml );

      // If from_reference's modifiedConstraint signal is emitted,
      // then the old XML representation of its constraint is appended
      // to this reconstrain command (done in the SLOT).

      // Actually, I don't think this can happen here...
      ConstrainedLine* from_reference =
	dynamic_cast<ConstrainedLine*>( reference_line_->line()->reference() );

      if ( from_reference != 0 ) {
	from_ref_xml_ = xml_rep_->createElement( lC::STR::CONSTRAINED_LINE );
	from_ref_xml_.setAttribute( lC::STR::URL,
				    from_reference->dbURL().toString(true) );
	from_reference->constraint()->write( from_ref_xml_ );

	connect( from_reference, SIGNAL( modifiedConstraint() ),
		 SLOT( cascadeConstraints() ) );
      }

      reference_line_->line()->setConstraint( new Coincident( reference ) );

      if ( from_reference != 0 )
	disconnect( from_reference, SIGNAL( modifiedConstraint() ),
		    this, SLOT( cascadeConstraints() ) );

      QDomElement new_xml = xml_rep_->createElement( lC::STR::CONSTRAINED_LINE );

      new_xml.setAttribute( lC::STR::URL,
			    reference_line_->line()->dbURL().toString(true) );
      reference_line_->line()->constraint()->write( new_xml );
      new_constraints.appendChild( new_xml );
    }

    if ( xml_rep_ != 0 ) {
      if ( command == 0 )
	command = new MoveLinesCommand( "move reference line",
					reference_line_view_->model() );

      command->add( xml_rep_ );

      xml_rep_ = 0;
    }

    if ( command != 0 )
      CommandHistory::instance().addCommand( command );

    selection_type_ = SelectionType( TRANSIENT, FIGURE );

    drag_handle_ = 0;
    // Continue in this mode until unselected
    return false;
  }

  void ReferenceLineModifyInput::mouseDoubleClick ( QMouseEvent* )
  {
    reference_line_view_->editInformation();
  }

  void ReferenceLineModifyInput::setReferenceLine ( ReferenceLine* reference_line )
  {
    reference_line_ = reference_line;
  }

  ReferenceLineInfoDialog* ReferenceLineView::reference_line_info_dialog_;

  ReferenceLineView::ReferenceLineView ( ReferenceLine* reference_line,
					 PageView* parent )
    : FigureView( parent ), reference_line_( reference_line ), dimension_view_( 0 ),
      create_input_( reference_line, this ), modify_input_( reference_line, this )
  {
    init();
  }

  ReferenceLineView::ReferenceLineView ( const QDomElement& xml_rep,
					 PageView* parent )
    : FigureView( parent ), reference_line_( 0 ), dimension_view_( 0 ),
      create_input_( 0, this ), modify_input_( 0, this )
  {
    DBURL db_url( xml_rep.attribute( lC::STR::REFERENCE_LINE ) );

    reference_line_ = dynamic_cast< ReferenceLine* >( model()->lookup( db_url ) );

    QDomNode n = xml_rep.firstChild();

    while ( !n.isNull() ) {
      QDomElement e = n.toElement();
      if ( !e.isNull() ) {
	if ( e.tagName() == lC::STR::DIMENSION_VIEW ) {
	  dimension_view_ = new DimensionView( e, view()->genSelectionName(),
					       this );

	  dimensionview_objects_.insert( dimension_view_->selectionName(),
					 dimension_view_ );

	  connect( dimension_view_->fromReference(), SIGNAL( modifiedPosition() ),
		   SLOT( dmvRefModified() ));
	  connect( dimension_view_->fromReference(), SIGNAL( destroyedGeometry() ),
		   SLOT( destroyedDmVRef() ) );
	  connect( dimension_view_->toReference(), SIGNAL( modifiedPosition() ),
		   SLOT( dmvRefModified() ));
	  connect( dimension_view_->toReference(), SIGNAL( destroyedGeometry() ),
		   SLOT( destroyedDmVRef() ) );
	}
      }

      n = n.nextSibling();
    }

    modify_input_.setReferenceLine( reference_line_ );

    init();

    modifiedPosition();
  }

  ReferenceLineView::ReferenceLineView ( const QString& name,
					 const QDomElement& xml_rep,
					 Page* page, PageView* parent )
    : FigureView( parent ), reference_line_( 0 ), dimension_view_( 0 ),
      create_input_( 0, this ), modify_input_( 0, this )
  {
    QDomDocument doc = xml_rep.ownerDocument();
    QDomNodeList reference_line_list =
      doc.elementsByTagName( lC::STR::REFERENCE_LINE );
    // Well, failure is not really an option...but it could happen if
    // somebody put a partial XML document into the clipboard...
    // In general, the failure modes of lignumCAD need substantially more
    // consideration......
    if ( reference_line_list.count() > 0 )
      reference_line_ = new ReferenceLine( page->newID(), name,
					   reference_line_list.item(0).toElement(),
					   page );


    modify_input_.setReferenceLine( reference_line_ );

    init();

    modifiedPosition();
  }

  void ReferenceLineView::init ( void )
  {
    QObject::setName( reference_line_->name().latin1() );

    // Create unique GL selection names for each of the components
    // of the reference line view (i.e., the line itself, and two handles).

    line_.setSelectionName( view()->genSelectionName() );
    handle0_.setSelectionName( view()->genSelectionName() );
    handle1_.setSelectionName( view()->genSelectionName() );

    // Normally, set the selected cursor shape here, but it really depends
    // on the orientation of the reference line.

    // Allow for reverse lookup of GL selection name to object

    reference_line_objects_.insert( line_.selectionName(), &line_ );
    reference_line_objects_.insert( handle0_.selectionName(), &handle0_ );
    reference_line_objects_.insert( handle1_.selectionName(), &handle1_ );

    QListViewItem* previous_item = parent()->previousItem( parent()->listViewItem(),
							   reference_line_->id() );

    list_view_item_ = new ListViewItem( parent()->listViewItem(), previous_item );

#if 0
    list_view_item_->setText( lC::NAME, lC::STR::NAME_ID.
			     arg( reference_line_->name() ).
			     arg( reference_line_->id() ) );
#else
  list_view_item_->setText( lC::NAME, lC::formatName( reference_line_->name() )
			    + QString( " <%1>" ).arg( reference_line_->id() ) );
#endif
    list_view_item_->setText( lC::TYPE, trC( lC::STR::REFERENCE_LINE ) );
    list_view_item_->setText( lC::DETAIL, trC( lC::STR::UNDEFINED ) );
    list_view_item_->setOpen( true );
    list_view_item_->setRenameEnabled( lC::NAME, true );

    line_list_view_ = new ListViewItem( list_view_item_, 0 );
    line_list_view_->setText( lC::NAME, lC::formatName( reference_line_->name() ) );
    line_list_view_->setText( lC::TYPE, trC( lC::STR::CONSTRAINED_LINE ) );
    line_list_view_->setText( lC::DETAIL, trC( lC::STR::CONSTRAINT_UNDEFINED ) );
    line_list_view_->setOpen( true );

    line_list_view_->listView()->ensureItemVisible( line_list_view_ );

    connect( reference_line_->line(), SIGNAL( modifiedConstraint() ),
	     SLOT( modifiedConstraint() ) );
    connect( reference_line_->line(), SIGNAL( modifiedPosition() ),
	     SLOT( modifiedPosition() ) );

    connect( list_view_item_, SIGNAL( nameChanged( const QString& ) ),
	     SLOT( listNameChanged( const QString& ) ) );

    connect( reference_line_->line(), SIGNAL( nameChanged( const QString& ) ),
	     SLOT( updateName( const QString& ) ) );

    if ( reference_line_info_dialog_ == 0 )
      reference_line_info_dialog_ = new ReferenceLineInfoDialog( parent()->lCMW() );
  }

  ReferenceLineView::~ReferenceLineView ( void )
  {
    if ( dimension_view_ ) {
      disconnect( dimension_view_->fromReference(), SIGNAL( modifiedPosition() ),
		  this, SLOT( dmvRefModified() ) );
      disconnect( dimension_view_->fromReference(), SIGNAL( destroyedGeometry() ),
		  this, SLOT( destroyedDmVRef() ) );
      disconnect( dimension_view_->toReference(), SIGNAL( modifiedPosition() ),
		  this, SLOT( dmvRefModified() ) );
      disconnect( dimension_view_->toReference(), SIGNAL( destroyedGeometry() ),
		  this, SLOT( destroyedDmVRef() ) );

      delete dimension_view_;
    }

    if ( reference_line_ ) {
      disconnect( reference_line_->line(), SIGNAL( modifiedConstraint() ),
		  this, SLOT( modifiedConstraint() ) );
      disconnect( reference_line_->line(), SIGNAL( modifiedPosition() ),
		  this, SLOT( modifiedPosition() ) );
      reference_line_->parent()->removeFigure( reference_line_ );
    }

    delete list_view_item_;
  }

  /*
   * Decide whether the reference line or the dimension is being
   * manipulated and return the responsible controller
   */
  InputObject* ReferenceLineView::modifyInput ( void )
  {
    if ( isActivated() )
      return &modify_input_;

    QIntDictIterator< DimensionView > idmv( dimensionview_objects_ );

    for (; idmv.current(); ++idmv ) {
      if ( idmv.current()->isActivated() )
	return dimension_view_->modifyInput();
    }

    return 0;
  }

  CreateObject* ReferenceLineView::memento ( void )
  {
    return new ReferenceLineViewCreate( this );
  }

  /*
   * Update the dimension view to have the given properties.
   */
  void ReferenceLineView::setDimensionView ( const QDomElement& xml_rep )
  {
    if ( dimension_view_ != 0 ) {
      disconnect( dimension_view_->fromReference(), SIGNAL( modifiedPosition() ),
		  this, SLOT( dmvRefModified() ) );
      disconnect( dimension_view_->fromReference(), SIGNAL( destroyedGeometry() ),
		  this, SLOT( destroyedDmVRef() ) );
      disconnect( dimension_view_->toReference(), SIGNAL( modifiedPosition() ),
		  this, SLOT( dmvRefModified() ) );
      disconnect( dimension_view_->toReference(), SIGNAL( destroyedGeometry() ),
		  this, SLOT( destroyedDmVRef() ) );

      dimensionview_objects_.remove( dimension_view_->selectionName() );

      delete dimension_view_;
    }

    dimension_view_ = new DimensionView( xml_rep, view()->genSelectionName(),
					 this );

    dimensionview_objects_.insert( dimension_view_->selectionName(),
				   dimension_view_ );

    connect( dimension_view_->fromReference(), SIGNAL( modifiedPosition() ),
	     SLOT( dmvRefModified() ));
    connect( dimension_view_->fromReference(), SIGNAL( destroyedGeometry() ),
	     SLOT( destroyedDmVRef() ) );
    connect( dimension_view_->toReference(), SIGNAL( modifiedPosition() ),
	     SLOT( dmvRefModified() ));
    connect( dimension_view_->toReference(), SIGNAL( destroyedGeometry() ),
	     SLOT( destroyedDmVRef() ) );
  }
  /*
   * Recompute any layout due to changes in the view.
   */
  void ReferenceLineView::viewAttributeChanged ( void )
  {
    updateDimensionViews();
  }

  // Change the name of the reference line.

  void ReferenceLineView::setName ( const QString& name )
  {
    DBURL old_db_url = reference_line_->dbURL();
    
    reference_line_->setName( name );

    CommandHistory::instance().
      addCommand( new RenameCommand( "rename reference line",
				     model(),old_db_url,reference_line_->dbURL()));
  }

  // Check a list view rename for uniqueness.

  void ReferenceLineView::listNameChanged ( const QString& name )
  {
    int ret = parent()->uniqueFigureName( this, name, reference_line_->type() );

    switch ( ret ) {
    case lC::OK:
      setName( name );
      break;
    case lC::Redo:
      list_view_item_->startRename( lC::NAME );
    case lC::Rejected:
      updateName( reference_line_->name() ); // Repaint list item with old name.
    }
  }

  void ReferenceLineView::draw ( void ) const
  {
    if ( !reference_line_->isValid() ) return;

    Point ll_corner( view()->llCorner()[X], view()->llCorner()[Y] );
    Point ur_corner( view()->urCorner()[X], view()->urCorner()[Y] );

    Point end0;
    Point end1;

    if ( reference_line_->line()->isHorizontal() ) {
      end0 = Point( ll_corner[X], reference_line_->line()->o()[Y] );
      end1 = Point( ur_corner[X], reference_line_->line()->o()[Y] );
    }
    else {
      end0 = Point( reference_line_->line()->o()[X], ll_corner[Y] );
      end1 = Point( reference_line_->line()->o()[X], ur_corner[Y] );
    }

    glEnable( GL_LINE_STIPPLE );
    glLineStipple( 1, 0x00ff );

    gl2psEnable( GL2PS_LINE_STIPPLE );
    gl2psLineStipple( GL2PS_LINE_STIPPLE_DASH );

    if ( isHighlighted() || isActivated() ||
	 line_.isHighlighted() || line_.isActivated() )
      view()->qglColor( view()->annotationColor().light() );
    else
      glColor3ubv( lC::qCubv( view()->annotationColor() ) );

    glBegin( GL_LINES );
    glVertex2dv( end0 );
    glVertex2dv( end1 );
    glEnd();

    glDisable( GL_LINE_STIPPLE );

    gl2psDisable( GL2PS_LINE_STIPPLE );

    if ( !reference_line_->line()->isAdjustable() &&
	 ( isHighlighted() || isActivated() ) ) {
      Segment edge_segment = reference_line_->
	closestVertices( reference_line_->line() );
      Segment reference_segment = reference_line_->line()->reference()->
	parent()->closestVertices( reference_line_->line()->reference() );
      Alignment::draw( view(), edge_segment, reference_segment );
    }

    lC::Render::Mode mode = lC::Render::INVISIBLE;
    if ( isHighlighted() || line_.isHighlighted() )
      mode = lC::Render::HIGHLIGHTED;
    if ( isActivated() || line_.isActivated() )
      mode = lC::Render::ACTIVATED;

    if ( mode != lC::Render::INVISIBLE ) {
      Vector spacing = (1./3.) * ( end1 - end0 );
      ResizeHandle::draw( view(), mode, end0 + spacing );
      ResizeHandle::draw( view(), mode, end1 - spacing );
    }

    if ( dimension_view_ != 0 ) dimension_view_->draw();
  }

  void ReferenceLineView::select ( SelectionType selection_type ) const
  {
    if ( !reference_line_->isComplete() ) return;

    glLoadName( selectionName() );

    Point ll_corner( view()->llCorner()[X], view()->llCorner()[Y] );
    Point ur_corner( view()->urCorner()[X], view()->urCorner()[Y] );

    Point end0;
    Point end1;

    if ( reference_line_->line()->isHorizontal() ) {
      end0 = Point( ll_corner[X], reference_line_->line()->o()[Y] );
      end1 = Point( ur_corner[X], reference_line_->line()->o()[Y] );
    }
    else {
      end0 = Point( reference_line_->line()->o()[X], ll_corner[Y] );
      end1 = Point( reference_line_->line()->o()[X], ur_corner[Y] );
    }

    if ( isHighlighted() || isActivated() ) {
      Vector spacing = (1./3.) * ( end1 - end0 );

      glPushName( handle0_.selectionName() );
      ResizeHandle::select( view(), end0 + spacing );
      glPopName();

      glPushName( handle1_.selectionName() );
      ResizeHandle::select( view(), end1 - spacing );
      glPopName();
    }

    glPushName( line_.selectionName() );
    glBegin( GL_LINES );
    glVertex2dv( end0 );
    glVertex2dv( end1 );
    glEnd();
    glPopName();

    if ( dimension_view_ != 0 ) dimension_view_->select( selection_type );
  }

  void ReferenceLineView::setCursor ( GLuint selection_name )
  {
    GraphicsView* graphics_object = reference_line_objects_[ selection_name ];

    if ( graphics_object != 0 )
      view()->setCursor( QCursor( graphics_object->cursorShape() ) );
  }

  void ReferenceLineView::setCursor ( const QCursor* cursor )
  {
    view()->setCursor( *cursor );
  }

  void ReferenceLineView::unsetCursor ( void )
  {
    view()->unsetCursor();
  }

  // Implementation of FigureView interface
  uint ReferenceLineView::id ( void ) const
  {
    return reference_line_->id();
  }

  QString ReferenceLineView::name ( void ) const
  {
    return reference_line_->name();
  }

  DBURL ReferenceLineView::dbURL ( void ) const
  {
    return reference_line_->dbURL();
  }

  QString ReferenceLineView::selectionText ( const std::vector<GLuint>&
					     /*selection_name*/,
					     SelectionEntity /*entity*/ ) const
  {
    return reference_line_->name();
  }

  QString ReferenceLineView::type ( void ) const
  {
    return lC::STR::REFERENCE_LINE;
  }

  // This would seem to be an implicit look up of the dimension view.
  // Is there anything else this could be asked about?
  View* ReferenceLineView::lookup ( QStringList& path_components ) const
  {
    if ( dimension_view_ != 0 ) {
      int dot_pos = path_components.front().findRev( '.' );
      QString name = path_components.front().left( dot_pos );
      QString type = path_components.front().right( path_components.front().length()
						    - dot_pos - 1 );

      if ( name == reference_line_->line()->name() &&
	   type == reference_line_->line()->type() )
	return dimension_view_;
    }

    return 0;
  }

  void ReferenceLineView::setHighlighted( bool highlight, SelectionEntity entity,
					  const std::vector<GLuint>& items )
  {
    if (entity == FIGURE ) {
      DimensionView* dmv = 0;

      if ( items.size() > 1 )
	dmv = dimensionview_objects_[ items[1] ];

      if ( dmv != 0 )
	dmv->setHighlighted( highlight );
      else
	FigureViewBase::setHighlighted( highlight );
    }
    else if ( entity == EDGE ) {
      if ( items.size() > 1 && reference_line_objects_[items[1]] != 0 )
	reference_line_objects_[items[1]]->setHighlighted( highlight );
    }
  }

  void ReferenceLineView::setActivated( bool activate, SelectionEntity entity,
					const std::vector<GLuint>& items )
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
      if ( items.size() > 1 && reference_line_objects_[items[1]] != 0 )
	reference_line_objects_[items[1]]->setActivated( activate );
    }
  }
  /*
   * Determine if this selection is part of the geometry representation;
   * this includes the handles if they happen to be highlighted, too.
   * The purpose here is really just to distinguish between the
   * line and the dimension views.
   */
  bool ReferenceLineView::isGeometry ( GLuint selection_name ) const
  {
    QIntDictIterator< GraphicsView > igv( reference_line_objects_ );

    for (; igv.current(); ++igv )
      if ( (GLuint)igv.currentKey() == selection_name ) return true;

    return false;
  }
  /*
   * The line is the only geometry for the reference line.
   */
  Curve* ReferenceLineView::geometry ( GLuint /*selection_name*/ ) const
  {
    return reference_line_->line();
  }
  /*
   * (This is only here to avoid a dependency on figure.h in the
   * header file which would otherwise be required in the moc
   * compilation.) (Would a cast work?)
   */
  FigureBase* ReferenceLineView::figure ( void ) const
  {
    return reference_line_;
  }

  /*
   * If the reference line itself is modified by a class other than this,
   * the ConstrainedLine will report it here. Currently, this mostly has
   * to do with which resize cursors to pick for the orientation of the
   * reference line
   */
  void ReferenceLineView::modifiedPosition ( void )
  {
    if ( reference_line_->line()->isHorizontal() ) {
      line_.setCursorShape( Qt::SizeVerCursor );
      handle0_.setCursorShape( Qt::SizeVerCursor );
      handle1_.setCursorShape( Qt::SizeVerCursor );
    }
    else {
      line_.setCursorShape( Qt::SizeHorCursor );
      handle0_.setCursorShape( Qt::SizeHorCursor );
      handle1_.setCursorShape( Qt::SizeHorCursor );
    }

    if ( dimension_view_ ) dimension_view_->computeLayout();

    list_view_item_->setText( lC::DETAIL,
			     trC( lC::STR::REFERENCE_LINE_FMT ).
			     arg( reference_line_->line()->o()[X] ).
			     arg( reference_line_->line()->o()[Y] ).
			     arg( reference_line_->line()->e()[X] ).
			     arg( reference_line_->line()->e()[Y] ) );
			     
    line_list_view_->setText( lC::DETAIL, reference_line_->line()->detail() );
  }

  /*
   * We are now responsible for our own dimension line view (if necessary)
   */
  void ReferenceLineView::modifiedConstraint ( void )
  {
    if ( reference_line_->line()->needsDimensionView() ) {
      if ( dimension_view_ == 0 ) {
	Vertex v = reference_line_->line()->reference()->parent()->
	  closestVertex( Point(), reference_line_->line() );
	Curve* reference = v.curve2_;

	lC::Orientation orientation;
	if ( reference_line_->line()->isHorizontal() )
	  orientation = lC::VERTICAL;
	else
	  orientation = lC::HORIZONTAL;

	dimension_view_ = new DimensionView( reference_line_->line(),
					     reference, reference,
					     orientation, lC::ABOVE,
					     view()->genSelectionName(),
					     this );

	dimensionview_objects_.insert( dimension_view_->selectionName(),
				       dimension_view_ );

	connect( reference, SIGNAL( modifiedPosition() ), SLOT( dmvRefModified() ));

	connect( reference, SIGNAL( destroyedGeometry() ),
		 SLOT( destroyedDmVRef() ) );
      }
      else {
	Vertex v = reference_line_->line()->reference()->parent()->
	  closestVertex( Point(), reference_line_->line() );
	Curve* reference = v.curve2_;

	disconnect( dimension_view_->fromReference(), SIGNAL( modifiedPosition() ),
		    this, SLOT( dmvRefModified() ) );
	disconnect( dimension_view_->fromReference(), SIGNAL( destroyedGeometry() ),
		    this, SLOT( destroyedDmVRef() ) );
	disconnect( dimension_view_->toReference(), SIGNAL( modifiedPosition() ),
		    this, SLOT( dmvRefModified() ) );
	disconnect( dimension_view_->toReference(), SIGNAL( destroyedGeometry() ),
		    this, SLOT( destroyedDmVRef() ) );

	dimension_view_->setReferences( reference, reference );

	connect( reference, SIGNAL( modifiedPosition() ), SLOT( dmvRefModified() ));

	connect( reference, SIGNAL( destroyedGeometry() ),
		 SLOT( destroyedDmVRef() ) );
      }

      dimension_view_->computeLayout();
    }
    else {
      if ( dimension_view_ != 0 ) {
	// I hope these references really persist until this evolution is
	// complete?
	QDomDocument* xml_doc = ConstraintHistory::instance().history();
	if ( xml_doc != 0 ) {
	  QDomElement xml_rep = xml_doc->createElement( lC::STR::DIMENSION_VIEW );
	  xml_rep.setAttribute( lC::STR::URL,
				reference_line_->dbURL().toString(true) );
	  dimension_view_->write( xml_rep );
	  ConstraintHistory::instance().appendUnconstraint( xml_rep );
	}

	disconnect( dimension_view_->fromReference(), SIGNAL( modifiedPosition() ),
		    this, SLOT( dmvRefModified() ) );
	disconnect( dimension_view_->fromReference(), SIGNAL( destroyedGeometry() ),
		    this, SLOT( destroyedDmVRef() ) );
	disconnect( dimension_view_->toReference(), SIGNAL( modifiedPosition() ),
		    this, SLOT( dmvRefModified() ) );
	disconnect( dimension_view_->toReference(), SIGNAL( destroyedGeometry() ),
		    this, SLOT( destroyedDmVRef() ) );

	if ( dimension_view_->isActivated() )
	  parent()->deactivateFigure( selectionName(),
				      dimension_view_->selectionName() );

	dimensionview_objects_.remove( dimension_view_->selectionName() );

	delete dimension_view_;

	dimension_view_ = 0;
      }
    }

    list_view_item_->setText( lC::DETAIL,
			     trC( lC::STR::REFERENCE_LINE_FMT ).
			     arg( reference_line_->line()->o()[X] ).
			     arg( reference_line_->line()->o()[Y] ).
			     arg( reference_line_->line()->e()[X] ).
			     arg( reference_line_->line()->e()[Y] ) );

    line_list_view_->setText( lC::DETAIL, reference_line_->line()->detail() );
  }

  void ReferenceLineView::dmvRefModified ( void )
  {
    dimension_view_->computeLayout();
  }

  void ReferenceLineView::destroyedDmVRef ( void )
  {
    QDomDocument* xml_doc = ConstraintHistory::instance().history();
    if ( xml_doc != 0 ) {
      QDomElement xml_rep = xml_doc->createElement( lC::STR::DIMENSION_VIEW );
      xml_rep.setAttribute( lC::STR::URL, reference_line_->dbURL().toString(true) );
      dimension_view_->write( xml_rep );
      ConstraintHistory::instance().appendUnconstraint( xml_rep );
    }

    disconnect( dimension_view_->fromReference(), SIGNAL( modifiedPosition() ),
		this, SLOT( dmvRefModified() ) );
    disconnect( dimension_view_->fromReference(), SIGNAL( destroyedGeometry() ),
		this, SLOT( destroyedDmVRef() ) );
    disconnect( dimension_view_->toReference(), SIGNAL( modifiedPosition() ),
		this, SLOT( dmvRefModified() ) );
    disconnect( dimension_view_->toReference(), SIGNAL( destroyedGeometry() ),
		this, SLOT( destroyedDmVRef() ) );

    delete dimension_view_;

    dimension_view_ = 0;
  }

  void ReferenceLineView::editInformation ( void )
  {
    if ( FigureViewBase::isActivated() ) {
      editReferenceLineInformation();
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

  void ReferenceLineView::editReferenceLineInformation ( void )
  {
    reference_line_info_dialog_->nameEdit->
      setText( lC::formatName( reference_line_->name() ) );

    if ( reference_line_->line()->isHorizontal() ) {
      reference_line_info_dialog_->horizontalButton->setChecked( true );
      reference_line_info_dialog_->verticalButton->setChecked( false );
    }
    else {
      reference_line_info_dialog_->horizontalButton->setChecked( false );
      reference_line_info_dialog_->verticalButton->setChecked( true );
    }

    reference_line_info_dialog_->referenceLineEdit->
      setText( lC::formatName( reference_line_->line()->reference()->path() ) );

    reference_line_info_dialog_->specifiedRadioButton->setChecked( true );
    reference_line_info_dialog_->offsetLengthSpinBox->setEnabled( true );

    // Can't do this yet...
    reference_line_info_dialog_->importedRadioButton->setEnabled( false );
    reference_line_info_dialog_->constraintChooser->setEnabled( false );

    reference_line_info_dialog_->offsetLengthSpinBox->
      setLengthLimits( UnitsBasis::instance()->lengthUnit(),
		       UnitsBasis::instance()->format(),
		       UnitsBasis::instance()->precision(),
		       lC::MINIMUM_DIMESION, lC::MAXIMUM_DIMENSION,
		       fabs( reference_line_->line()->offset() ) );

    reference_line_info_dialog_->offsetLengthSpinBox->
      setLength( fabs( reference_line_->line()->offset() ) );

  redo:
    int ret = reference_line_info_dialog_->exec();

    if ( ret == QDialog::Rejected ) return;

    bool modified = false;

    if ( reference_line_info_dialog_->nameEdit->edited() ) {
      // Pageview handles checking the name and putting up the error dialog
      // if necessary.
      int ret = parent()->uniqueFigureName( this,
				  reference_line_info_dialog_->nameEdit->text(),
					    reference_line_->type() );

      switch ( ret ) {
      case lC::Rejected:
	return;
      case lC::Redo:
	reference_line_info_dialog_->nameEdit->
	  setText( lC::formatName( reference_line_->name() ) );
	goto redo;
      }

      setName( reference_line_info_dialog_->nameEdit->text() );
      modified = true;
    }

    if ( reference_line_info_dialog_->offsetLengthSpinBox->edited() ) {
      double old_offset = reference_line_->line()->offset();

      if ( reference_line_->line()->offset() >= 0 )
	reference_line_->line()->
	  setOffset( reference_line_info_dialog_->offsetLengthSpinBox->length() );
      else
	reference_line_->line()->
	  setOffset( -reference_line_info_dialog_->offsetLengthSpinBox->length() );

      MoveLinesCommand* command =
	new MoveLinesCommand( "move reference line", model() );

      command->add( reference_line_->line(), old_offset );

      CommandHistory::instance().addCommand( command );

      modified = true;
    }

    if ( modified ) parent()->figureModified();
  }

  void ReferenceLineView::write ( QDomElement& xml_rep ) const
  {
    if ( !reference_line_->isValid() ) return;

    QDomDocument document = xml_rep.ownerDocument();
    QDomElement reference_line_view_element =
      document.createElement( lC::STR::REFERENCE_LINE_VIEW );
    reference_line_view_element.setAttribute( lC::STR::REFERENCE_LINE,
				      reference_line_->dbURL().toString( true) );
    if ( dimension_view_ != 0 )
      dimension_view_->write( reference_line_view_element );

    xml_rep.appendChild( reference_line_view_element );
  }
  /*
   * Create an XML representation of a simplified version of this view
   * and its reference line.
   */
  void ReferenceLineView::copyWrite ( QDomElement& xml_rep ) const
  {
    QDomDocument document = xml_rep.ownerDocument();
    QDomElement reference_line_view_element =
      document.createElement( lC::STR::REFERENCE_LINE_VIEW );

    reference_line_->copyWrite( reference_line_view_element );

    xml_rep.appendChild( reference_line_view_element );
  }
  /*
   * Detect changes to the name initiated elsewhere
   */
  void ReferenceLineView::updateName ( const QString& /*name*/ )
  {
    list_view_item_->setText( lC::NAME, lC::formatName( reference_line_->name() )
			      + QString( " <%1>" ).arg( reference_line_->id() ) );
    line_list_view_->setText( lC::NAME,
			      lC::formatName( reference_line_->line()->name() ) );
    line_list_view_->setText( lC::DETAIL, reference_line_->line()->detail() );
  }

  // The least little change to user preferences requires that any
  // dimension views be recomputed.
  void ReferenceLineView::updateDimensionViews ( void )
  {
    if ( dimension_view_ != 0 ) dimension_view_->computeLayout();
  }
} // End of Space2D namespace
