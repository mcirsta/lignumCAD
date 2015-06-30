/*
 * centerlineview.cpp
 *
 * CenterlineView classes
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
#include <QMenu>
#include <qlineedit.h>
#include <qradiobutton.h>
#include <qlabel.h>
#include <qcursor.h>

#include "OGLFT.h"

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
#include "centerline.h"
#include "centerlineinfodialog.h"
#include "centerlineview.h"

namespace Space2D {

  class CenterlineViewCreate : public CreateObject {
    //! When CommandHistory undoes or redoes the creation of this object,
    //! the Design Book view is the only handle which is required. Everything
    //! else can be looked up.
    DesignBookView* design_book_view_;
    //! The details of the centerline and view are stored as XML.
    QDomDocument xml_doc_;
  public:
  /*!
   * Create a memento of the current state of the centerline and its view.
   * \param centerline_view view object of the centerline.
   */
    CenterlineViewCreate ( CenterlineView* centerline_view )
    {
      design_book_view_ = centerline_view->parent()->parent();

      QDomElement root = xml_doc_.createElement( lC::STR::MEMENTO );

      root.setAttribute( lC::STR::NAME, centerline_view->dbURL().toString(true) );

      centerline_view->centerline()->write( root );
      centerline_view->write( root );

      xml_doc_.appendChild( root );
    }
    //! Destructor doesn't do much.
    ~CenterlineViewCreate ( void ) {}
    /*!
     * (Re)create the centerline object and its view as specified in
     * the XML representation.
     */
    void create ( void )
    {
      QDomNodeList memento_list = xml_doc_.elementsByTagName( lC::STR::MEMENTO );

      if ( memento_list.length() > 0 ) {
	DBURL url = memento_list.item(0).toElement().attribute( lC::STR::NAME );

	// First, we have to find the parent page and view for the centerline.

	DBURL parent = url.parent();

	Page* page =
	  dynamic_cast<Page*>( design_book_view_->model()->lookup( parent ) );
	PageView* page_view =
	  dynamic_cast<PageView*>( design_book_view_->lookup( parent ) );

	// Now, we can extract the XML representations for the centerline
	// and its view
	QDomNodeList centerline_list =
	  xml_doc_.elementsByTagName( lC::STR::CENTERLINE );

	if ( centerline_list.length() > 0 ) {

	  QDomNodeList centerline_view_list =
	    xml_doc_.elementsByTagName( lC::STR::CENTERLINE_VIEW );

	  if ( centerline_view_list.length() > 0 ) {
	    uint centerline_id =
	      centerline_list.item(0).toElement().attribute( lC::STR::ID ).toUInt();

	    new Centerline( centerline_id, centerline_list.item(0).toElement(),
			    page );

	    page_view->
	      addFigureView( new CenterlineView( centerline_view_list.
						 item(0).toElement(),
						 page_view ) );
	  }
	}

	// See if there are any constraints which should be put back
	// in place when this centerline is restored.
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
     * Delete the centerline object and its view as specified in the
     * XML representation.
     */
    void remove ( void )
    {
      QDomNodeList memento_list = xml_doc_.elementsByTagName( lC::STR::MEMENTO );

      if ( memento_list.length() > 0 ) {
	DBURL url = memento_list.item(0).toElement().attribute( lC::STR::NAME );

	CenterlineView* centerline_view =
	  dynamic_cast<CenterlineView*>( design_book_view_->lookup( url ) );

	DBURL parent = url.parent();

	PageView* page_view =
	  dynamic_cast<PageView*>( design_book_view_->lookup( parent ) );

	page_view->removeFigureView( centerline_view );
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

      QDomNodeList centerline_list =
	xml_doc_.elementsByTagName( lC::STR::CENTERLINE );

      if ( centerline_list.length() > 0 ) {
	QDomNode element = xml_rep->importNode( centerline_list.item(0), true );

	rep_root.appendChild( element );
      }

      QDomNodeList centerline_view_list =
	xml_doc_.elementsByTagName( lC::STR::CENTERLINE_VIEW );

      if ( centerline_view_list.length() > 0 ) {
	QDomNode element = xml_rep->importNode( centerline_view_list.item(0),
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
     * If the creation of a centerline is immediately followed by a rename,
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

	QDomNodeList centerline_list =
	  xml_doc_.elementsByTagName( lC::STR::CENTERLINE );
	QDomNodeList centerline_view_list =
	  xml_doc_.elementsByTagName( lC::STR::CENTERLINE_VIEW );

	if ( centerline_list.length() == 0 ||
	     centerline_view_list.length() == 0 )
	  return false;

	// Update the name elements in the object and it's view.

	memento_list.item(0).toElement().setAttribute( lC::STR::NAME,
					       rename->newDBURL().toString(true) );

	centerline_list.item(0).toElement().
	  setAttribute( lC::STR::NAME, rename->newDBURL().name() );

	centerline_view_list.item(0).toElement().setAttribute( lC::STR::CENTERLINE,
					       rename->newDBURL().toString(true) );

	return true;
      }

      return false;
    }
  };


  QChar centerline_symbol_( 0x2104 );

  CenterlineCreateInput::CenterlineCreateInput( Centerline* centerline,
					    CenterlineView* centerline_view )
    : centerline_( centerline ), centerline_view_( centerline_view )
  {}

  void CenterlineCreateInput::startDisplay ( QPopupMenu* context_menu )
  {
    context_menu_ = context_menu;

    QAction* cancel_action =
      centerline_view_->parent()->lCMW()->cancelCenterlineAction;

    separator_id_ = context_menu_->insertSeparator();
    cancel_action->addTo( context_menu_ );
    connect( cancel_action, SIGNAL( activated() ), SLOT( cancelOperation() )  );

    centerline_view_->view()->
      setCursor( CursorFactory::instance().cursor( CursorFactory::CENTERLINE ) );
  }

  void CenterlineCreateInput::mousePrepress ( QMouseEvent* /*me*/,
					      const SelectedNames& /*selected*/ )
  {}

  void CenterlineCreateInput::mousePress ( QMouseEvent* me,
					   const SelectedNames& selected )
  {
    start_ = centerline_view_->view()->unproject2D( me->pos() );

    UnitsBasis::instance()->round( start_ );

    Line* centerline = 0;

    SelectedNames::const_iterator f;

    for ( f = selected.begin(); f != selected.end(); ++f ) {
      FigureView* fv =
	dynamic_cast< FigureView* >( centerline_view_->parent()->
				     figureSelectionNames()[ (*f).second[0] ] );

      std::vector<GLuint>::const_iterator g;

      for ( g = (*f).second.begin(); g != (*f).second.end(); ++g ) {

	Line* line = dynamic_cast< Line* >( fv->geometry( *g ) );

	if ( line != 0 ) {
	  centerline = line;
	}
      }
    }

    if ( centerline != 0 ) {
      centerline_->line()->setConstraint( new Coincident( centerline ) );
    }
    else {
      centerline_->line()->setConstraint( new Offset( start_, &Y_AXIS ) );

      centerline_view_->view()->
	setCursor( CursorFactory::instance().cursor( CursorFactory::LINE_ROTATE ) );
    }
  }

  void CenterlineCreateInput::mouseDrag ( QMouseEvent* me,
					  const SelectedNames& /*selected*/ )
  {
    ConstrainedLine* line = centerline_->line();

    if ( line->isAdjustable() ) {
      Point current = centerline_view_->view()->unproject2D( me->pos() );

      UnitsBasis::instance()->round( current );

      double orientation = (current - start_).orientation();

      // Currently, there're only horizontal and vertical centerlines
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

  bool CenterlineCreateInput::mouseRelease ( QMouseEvent* /*me*/,
					     const SelectedNames& /*selected*/ )
  {
    PageView* page_view = centerline_view_->parent();

    QAction* cancel_action = page_view->lCMW()->cancelCenterlineAction;
    cancel_action->disconnect();
    cancel_action->removeFrom( context_menu_ );
    context_menu_->removeItem( separator_id_ );

    centerline_->setComplete( true );

    centerline_view_->unsetCursor();

    page_view->figureComplete();

    page_view->activateFigure( centerline_view_ );

    CommandHistory::instance().
      addCommand( new CreateCommand( "create centerline",
				     centerline_view_->memento() ) );

    return false;
  }

  void CenterlineCreateInput::keyPress ( QKeyEvent* ke )
  {
    if ( ke->key() == Qt::Key_Escape )
      cancelOperation();
    else
      ke->ignore();
  }

  void CenterlineCreateInput::cancelOperation ( void )
  {
    PageView* page_view = centerline_view_->parent();

    QAction* cancel_action = page_view->lCMW()->cancelCenterlineAction;

    cancel_action->disconnect();
    cancel_action->removeFrom( context_menu_ );
    context_menu_->removeItem( separator_id_ );

    page_view->removeFigureView( centerline_view_ );

    page_view->cancelOperation();
  }

  CenterlineModifyInput::CenterlineModifyInput( Centerline* centerline,
						CenterlineView* centerline_view )
    : centerline_( centerline ), centerline_view_( centerline_view ),
      xml_rep_( 0 )
  {}

  /*
   * Tries to set the mouse cursor to a reasonable shape based on what
   * is highlighted. For centerlines, they can only move in one direction.
   * If *this* view is no longer highlighted, turn off any special cursor
   * shape imposed by this view.
   */
  void CenterlineModifyInput::mousePrepress ( QMouseEvent* me,
					      const SelectedNames& selected )
  {
    if ( selected.empty() ) {
      centerline_view_->unsetCursor();
      return;
    }

    SelectedNames::const_iterator f;

    // For each highlighted figure view...

    for ( f = selected.begin(); f != selected.end(); ++f ) {

      // If the highlighted figure view is *this* view...

      if ( (*f).second[0] == centerline_view_->selectionName() ) {

	// If the geometry of this view is highlighted...

	if ( (*f).second.size() > 1 ) {
	  GLuint g = (*f).second[1]; // (*(*f).second->rbegin()).first;

	  if ( centerline_view_->isGeometry( g ) ) {

	    centerline_view_->setCursor( g );
	    // This may have been called as a second chance action which
	    // another FigureView has already rejected.
	    me->accept();
	  }
	  else {
	    centerline_view_->unsetCursor();
	  }

	  return; // Probably not the last word here
	}
      }
    }

    // If none of the highlighted figure views was this one, then
    // reset the cursor.

    centerline_view_->unsetCursor();
  }

  void CenterlineModifyInput::mousePress ( QMouseEvent* me,
					   const SelectedNames& selected )
  {
    // Let the Page handle these events

    if ( me->button() != Qt::LeftButton || selected.empty() ) {
      me->ignore();
      return;
    }

    SelectedNames::const_iterator f;

    for ( f = selected.begin(); f != selected.end(); ++f ) {
      // If this view is selected, then accept this mouse press (me->accept
      // is true by default)
      if ( (*f).second[0] == centerline_view_->selectionName() ) {

	if ( (*f).second.size() > 1  &&
	     centerline_view_->isGeometry( (*f).second[1] ) ) {

	  old_offset_ = centerline_->line()->offset();

	  last_pnt_ = centerline_view_->view()->unproject2D( me->pos() );

	  UnitsBasis::instance()->round( last_pnt_ );

	  start_pnt_ = last_pnt_;

	  // Search for other geometry's to align with.
	  selection_type_ = SelectionType( TRANSIENT, EDGE );

	  // I think this controls some state...
	  drag_handle_ = (*f).second[1]; // (*(*f).second->rbegin()).first;
	  return;
	}
      }
    }
    
    // If this view wasn't highlighted, then let also let the page handle it.

    me->ignore();
  }

  void CenterlineModifyInput::mouseDrag ( QMouseEvent* me,
					  const SelectedNames& /*selected*/ )
  {
    if ( drag_handle_ == 0 ) return;

    current_pnt_ = centerline_view_->view()->unproject2D( me->pos() );

    UnitsBasis::instance()->round( current_pnt_ );

    centerline_->line()->setPosition( current_pnt_ );

    last_pnt_ = current_pnt_;
	  
    centerline_view_->parent()->figureModified();
  }

  bool CenterlineModifyInput::mouseRelease ( QMouseEvent* /*me*/,
					     const SelectedNames& selected )
  {
    MoveLinesCommand* command = 0;

    if ( centerline_->line()->offset() != old_offset_ ) {
      command = new MoveLinesCommand( "move centerline",
				      centerline_view_->model() );

      command->add( centerline_->line(), old_offset_ );
    }

    // See if we picked up a coincident constraint.

    Line* reference = 0;

    SelectedNames::const_iterator f;

    for ( f = selected.begin(); f != selected.end(); ++f ) {
      FigureView* fv =
	dynamic_cast< FigureView* >( centerline_view_->parent()->
				     figureSelectionNames()[ (*f).second[0] ] );

      if ( fv == centerline_view_ ) continue;

      std::vector<GLuint>::const_iterator g;

      for ( g = (*f).second.begin(); g != (*f).second.end(); ++g ) {

	Line* line = dynamic_cast< Line* >( fv->geometry( *g ) );

	if ( line != 0 ) {
	  if ( ( centerline_->line()->isVertical() && line->isVertical() ) ||
	       ( centerline_->line()->isHorizontal() && line->isHorizontal() ) )
	    reference = line;
	}
      }
    }

    if ( reference != 0 && !reference->dependsOn( centerline_->line() ) ) {
      xml_rep_ = new QDomDocument;

      QDomElement reconstraints = xml_rep_->createElement( lC::STR::RECONSTRAINTS );
      xml_rep_->appendChild( reconstraints );

      QDomElement old_constraints = xml_rep_->createElement( lC::STR::OLD_CONSTRAINTS );
      reconstraints.appendChild( old_constraints );

      QDomElement new_constraints = xml_rep_->createElement( lC::STR::NEW_CONSTRAINTS );
      reconstraints.appendChild( new_constraints );

      QDomElement old_xml = xml_rep_->createElement( lC::STR::CONSTRAINED_LINE );

      old_xml.setAttribute( lC::STR::URL,
			    centerline_->line()->dbURL().toString(true) );
      centerline_->line()->constraint()->write( old_xml );
      old_constraints.appendChild( old_xml );

      // If from_reference's modifiedConstraint signal is emitted,
      // then the old XML representation of its constraint is appended
      // to this reconstrain command (done in the SLOT).

      // Actually, I don't think this can happen here...
      ConstrainedLine* from_reference =
	dynamic_cast<ConstrainedLine*>( centerline_->line()->reference() );

      if ( from_reference != 0 ) {
	from_ref_xml_ = xml_rep_->createElement( lC::STR::CONSTRAINED_LINE );
	from_ref_xml_.setAttribute( lC::STR::URL,
				    from_reference->dbURL().toString(true) );
	from_reference->constraint()->write( from_ref_xml_ );

	connect( from_reference, SIGNAL( modifiedConstraint() ),
		 SLOT( cascadeConstraints() ) );
      }

      centerline_->line()->setConstraint( new Coincident( reference ) );

      if ( from_reference != 0 )
	disconnect( from_reference, SIGNAL( modifiedConstraint() ),
		    this, SLOT( cascadeConstraints() ) );

      QDomElement new_xml = xml_rep_->createElement( lC::STR::CONSTRAINED_LINE );

      new_xml.setAttribute( lC::STR::URL, centerline_->line()->dbURL().toString(true) );
      centerline_->line()->constraint()->write( new_xml );
      new_constraints.appendChild( new_xml );
    }

    if ( xml_rep_ != 0 ) {
      if ( command == 0 )
	command = new MoveLinesCommand( "move centerline",
					centerline_view_->model() );

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

  void CenterlineModifyInput::mouseDoubleClick ( QMouseEvent* )
  {
    centerline_view_->editInformation();
  }

  void CenterlineModifyInput::setCenterline ( Centerline* centerline )
  {
    centerline_ = centerline;
  }

  CenterlineInfoDialog* CenterlineView::centerline_info_dialog_;

  CenterlineView::CenterlineView ( Centerline* centerline,
				   PageView* parent )
    : FigureView( parent ), centerline_( centerline ), dimension_view_( 0 ),
      create_input_( centerline, this ), modify_input_( centerline, this )
  {
    init();
  }

  CenterlineView::CenterlineView ( const QDomElement& xml_rep,
				   PageView* parent )
    : FigureView( parent ), centerline_( 0 ), dimension_view_( 0 ),
      create_input_( 0, this ), modify_input_( 0, this )
  {
    DBURL db_url( xml_rep.attribute( lC::STR::CENTERLINE ) );

    centerline_ = dynamic_cast< Centerline* >( model()->lookup( db_url ) );

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

    modify_input_.setCenterline( centerline_ );

    init();

    modifiedPosition();
  }

  CenterlineView::CenterlineView ( const QString& name, const QDomElement& xml_rep,
				   Page* page, PageView* parent )
    : FigureView( parent ), centerline_( 0 ), dimension_view_( 0 ),
      create_input_( 0, this ), modify_input_( 0, this )
  {
    QDomDocument doc = xml_rep.ownerDocument();
    QDomNodeList centerline_list = doc.elementsByTagName( lC::STR::CENTERLINE );
    // Well, failure is not really an option...but it could happen if
    // somebody put a partial XML document into the clipboard...
    // In general, the failure modes of lignumCAD need substantially more
    // consideration......
    if ( centerline_list.count() > 0 )
      centerline_ = new Centerline( page->newID(), name,
				    centerline_list.item(0).toElement(), page );


    modify_input_.setCenterline( centerline_ );

    init();

    modifiedPosition();
  }

  void CenterlineView::init ( void )
  {
    QObject::setName( centerline_->name() + lC::STR::VIEW_EXT );

    // Create unique GL selection names for each of the components
    // of the centerline view (i.e., the line itself, and two handles).

    line_.setSelectionName( view()->genSelectionName() );
    handle0_.setSelectionName( view()->genSelectionName() );
    handle1_.setSelectionName( view()->genSelectionName() );

    // Normally, set the selected cursor shape here, but it really depends
    // on the orientation of the centerline.

    // Allow for reverse lookup of GL selection name to object

    centerline_objects_.insert( line_.selectionName(), &line_ );
    centerline_objects_.insert( handle0_.selectionName(), &handle0_ );
    centerline_objects_.insert( handle1_.selectionName(), &handle1_ );

    QListViewItem* previous_item = parent()->previousItem( parent()->listViewItem(),
							   centerline_->id() );

    list_view_item_ = new ListViewItem( parent()->listViewItem(), previous_item );

  list_view_item_->setText( lC::NAME, lC::formatName( centerline_->name() )
			    + QString( " <%1>" ).arg( centerline_->id() ) );
    list_view_item_->setText( lC::TYPE, trC( lC::STR::CENTERLINE ) );
    list_view_item_->setText( lC::DETAIL, trC( lC::STR::UNDEFINED ) );
    list_view_item_->setOpen( true );
    list_view_item_->setRenameEnabled( lC::NAME, true );

    line_list_view_ = new ListViewItem( list_view_item_, 0 );
    line_list_view_->setText( lC::NAME, lC::formatName( centerline_->name() ) );
    line_list_view_->setText( lC::TYPE, trC( lC::STR::CONSTRAINED_LINE ) );
    line_list_view_->setText( lC::DETAIL, trC( lC::STR::CONSTRAINT_UNDEFINED ) );
    line_list_view_->setOpen( true );

    line_list_view_->listView()->ensureItemVisible( line_list_view_ );

    connect( centerline_->line(), SIGNAL( modifiedConstraint() ),
	     SLOT( modifiedConstraint() ) );
    connect( centerline_->line(), SIGNAL( modifiedPosition() ),
	     SLOT( modifiedPosition() ) );

    connect( list_view_item_, SIGNAL( nameChanged( const QString& ) ),
	     SLOT( listNameChanged( const QString& ) ) );

    connect( centerline_->line(), SIGNAL( nameChanged( const QString& ) ),
	     SLOT( updateName( const QString& ) ) );

    if ( centerline_info_dialog_ == 0 )
      centerline_info_dialog_ = new CenterlineInfoDialog( parent()->lCMW() );
  }

  CenterlineView::~CenterlineView ( void )
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

    if ( centerline_ ) {
      disconnect( centerline_->line(), SIGNAL( modifiedConstraint() ),
		  this, SLOT( modifiedConstraint() ) );
      disconnect( centerline_->line(), SIGNAL( modifiedPosition() ),
		  this, SLOT( modifiedPosition() ) );
      centerline_->parent()->removeFigure( centerline_ );
    }

    delete list_view_item_;
  }

  /*
   * Decide whether the centerline or the dimension is being
   * manipulated and return the responsible controller
   */
  InputObject* CenterlineView::modifyInput ( void )
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

  CreateObject* CenterlineView::memento ( void )
  {
    return new CenterlineViewCreate( this );
  }

  /*
   * Update the dimension view to have the given properties.
   */
  void CenterlineView::setDimensionView ( const QDomElement& xml_rep )
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

  // Recompute any layout due to changes in the view.

  void CenterlineView::viewAttributeChanged ( void )
  {
    updateDimensionViews();
  }

  // Change the name of the centerline.

  void CenterlineView::setName ( const QString& name )
  {
    DBURL old_db_url = centerline_->dbURL();
    
    centerline_->setName( name );

    CommandHistory::instance().
      addCommand( new RenameCommand( "rename centerline",
				     model(), old_db_url, centerline_->dbURL() ));
  }

  // Check a list view rename for uniqueness.

  void CenterlineView::listNameChanged ( const QString& name )
  {
    int ret = parent()->uniqueFigureName( this, name, centerline_->type() );

    switch ( ret ) {
    case lC::OK:
      setName( name );
      break;
    case lC::Redo:
      list_view_item_->startRename( lC::NAME );
    case lC::Rejected:
      updateName( centerline_->name() ); // Repaint list item with old name.
    }
  }

  void CenterlineView::draw ( void ) const
  {
    if ( !centerline_->isValid() ) return;

    QColor color = view()->annotationColor();

    if ( isHighlighted() || isActivated() ||
	 line_.isHighlighted() || line_.isActivated() )
      color = color.light();

    view()->qglColor( color );

    FaceData face_data( view()->annotationFont(), 0, color.rgb() );
    OGLFT::BBox clbb = view()->font(face_data)->measure( centerline_symbol_ );

    // Determine where the ends are. The centerline is drawn
    // across the entire window with a gap for the CL symbol.

    Point ll_corner = view()->llCorner();
    Point ur_corner = view()->urCorner();

    Point end[4];

    if ( centerline_->line()->isHorizontal() ) {
      end[0] = Point( ll_corner[X], centerline_->line()->o()[Y] );
      end[1] = Point( ll_corner[X] +
		      ( clbb.x_max_ - clbb.x_min_ ) - clbb.advance_.dx_/2.,
		      centerline_->line()->o()[Y] );
      end[2] = Point( ll_corner[X] +
		      2 * (clbb.x_max_ - clbb.x_min_) + clbb.advance_.dx_/2.,
		      centerline_->line()->o()[Y] );
      end[3] = Point( ur_corner[X], centerline_->line()->o()[Y] );
    }
    else {
      end[0] = Point( centerline_->line()->o()[X], ur_corner[Y] );
      end[1] = Point( centerline_->line()->o()[X],
		      ur_corner[Y] -
		      (clbb.y_max_ - clbb.y_min_) + clbb.y_max_/2.);
      end[2] = Point( centerline_->line()->o()[X],
		      ur_corner[Y] -
		      2 * (clbb.y_max_ - clbb.y_min_) - clbb.y_max_/2. );
      end[3] = Point( centerline_->line()->o()[X], ll_corner[Y] );
    }

    if ( !centerline_->line()->isAdjustable() &&
	 ( isHighlighted() || isActivated() ) ) {
      Segment edge_segment = centerline_->closestVertices( centerline_->line() );
      Segment reference_segment = centerline_->line()->reference()->
	parent()->closestVertices( centerline_->line()->reference() );
      Alignment::draw( view(), edge_segment, reference_segment );
    }

    // Draw the centerline itself

    glEnable( GL_LINE_STIPPLE );
    glLineStipple( 1, 0x18ff );

    gl2psEnable( GL2PS_LINE_STIPPLE );
    gl2psLineStipple( GL2PS_LINE_STIPPLE_DASHDOT );

    glBegin( GL_LINES );
    glVertex2dv( end[0] );
    glVertex2dv( end[1] );
    glVertex2dv( end[2] );
    glVertex2dv( end[3] );
    glEnd();

    glDisable( GL_LINE_STIPPLE );

    gl2psDisable( GL2PS_LINE_STIPPLE );

    // Draw the CL symbol. Two notes: 1) we defined the glyph
    // ourselves and expect certain attributes of the bounding box. 2)
    // The single character OGLFT draw routine doesn't save the
    // current modelview matrix.

    glPushMatrix();
    if ( centerline_->line()->isHorizontal() )
      view()->font(face_data)->draw( end[0][X] + (clbb.x_max_ - clbb.x_min_),
				     end[0][Y] - ( clbb.y_max_ / 2. ),
				     centerline_symbol_ );
    else
      view()->font(face_data)->draw( end[0][X] - clbb.advance_.dx_ / 2.,
				     end[0][Y] - ( clbb.y_max_ - clbb.y_min_)
				     - clbb.y_max_,
				     centerline_symbol_ );
    glPopMatrix();

    // Draw the handles if required

    lC::Render::Mode mode = lC::Render::INVISIBLE;
    if ( isHighlighted() || line_.isHighlighted() )
      mode = lC::Render::HIGHLIGHTED;
    if ( isActivated() || line_.isActivated() )
      mode = lC::Render::ACTIVATED;

    if ( mode != lC::Render::INVISIBLE ) {
      Vector spacing = (1./3.) * ( end[3] - end[0] );
      ResizeHandle::draw( view(), mode, end[0] + spacing );
      ResizeHandle::draw( view(), mode, end[3] - spacing );
    }

    if ( dimension_view_ != 0 ) dimension_view_->draw();
  }

  void CenterlineView::select ( SelectionType selection_type ) const
  {
    if ( !centerline_->isComplete() ) return;

    glLoadName( selectionName() );

    Point ll_corner = view()->llCorner();
    Point ur_corner = view()->urCorner();

    Point end0;
    Point end1;

    if ( centerline_->line()->isHorizontal() ) {
      end0 = Point( ll_corner[X], centerline_->line()->o()[Y] );
      end1 = Point( ur_corner[X], centerline_->line()->o()[Y] );
    }
    else {
      end0 = Point( centerline_->line()->o()[X], ll_corner[Y] );
      end1 = Point( centerline_->line()->o()[X], ur_corner[Y] );
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

  void CenterlineView::setCursor ( GLuint selection_name )
  {
    GraphicsView* graphics_object = centerline_objects_[ selection_name ];

    if ( graphics_object != 0 )
      view()->setCursor( QCursor( graphics_object->cursorShape() ) );
  }

  void CenterlineView::setCursor ( const QCursor* cursor )
  {
    view()->setCursor( *cursor );
  }

  void CenterlineView::unsetCursor ( void )
  {
    view()->unsetCursor();
  }

  // Implementation of FigureView interface
  uint CenterlineView::id ( void ) const
  {
    return centerline_->id();
  }

  QString CenterlineView::name ( void ) const
  {
    return centerline_->name();
  }

  DBURL CenterlineView::dbURL ( void ) const
  {
    return centerline_->dbURL();
  }

  QString CenterlineView::selectionText ( const std::vector<GLuint>& /*selection_name*/,
					  SelectionEntity /*entity*/ ) const
  {
    return centerline_->name();
  }

  QString CenterlineView::type ( void ) const
  {
    return lC::STR::CENTERLINE;
  }
  // This would seem to be an implicit look up of the dimension view.
  // Is there anything else this could be asked about?
  View* CenterlineView::lookup ( QStringList& path_components ) const
  {
    if ( dimension_view_ != 0 ) {
      int dot_pos = path_components.front().findRev( '.' );
      QString name = path_components.front().left( dot_pos );
      QString type = path_components.front().right( path_components.front().length()
						    - dot_pos - 1 );

      if ( name == centerline_->line()->name() &&
	   type == centerline_->line()->type() )
	return dimension_view_;
    }

    return 0;
  }

  void CenterlineView::setHighlighted( bool highlight, SelectionEntity entity,
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
      if ( items.size() > 1 && centerline_objects_[items[1]] != 0 )
	centerline_objects_[items[1]]->setHighlighted( highlight );
    }
  }

  void CenterlineView::setActivated( bool activate, SelectionEntity entity,
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
      if ( items.size() > 1 && centerline_objects_[items[1]] != 0 )
	centerline_objects_[items[1]]->setActivated( activate );
    }
  }
  /*
   * Determine if this selection is part of the geometry representation;
   * this includes the handles if they happen to be highlighted, too.
   * The purpose here is really just to distinguish between the
   * line and the dimension views.
   */
  bool CenterlineView::isGeometry ( GLuint selection_name ) const
  {
    QIntDictIterator< GraphicsView > igv( centerline_objects_ );

    for (; igv.current(); ++igv )
      if ( (GLuint)igv.currentKey() == selection_name ) return true;

    return false;
  }

  Curve* CenterlineView::geometry ( GLuint /*selection_name*/ ) const
  {
    return centerline_->line();
  }

  FigureBase* CenterlineView::figure ( void ) const
  {
    return centerline_;
  }

  /*
   * If the centerline itself is modified by a class other than this,
   * the centerline will report it here. Currently, this mostly has
   * to do with which resize cursors to pick for the orientation of the
   * centerline.
   */
  void CenterlineView::modifiedPosition ( void )
  {
    if ( centerline_->line()->isHorizontal() ) {
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
			     arg( centerline_->line()->o()[X] ).
			     arg( centerline_->line()->o()[Y] ).
			     arg( centerline_->line()->e()[X] ).
			     arg( centerline_->line()->e()[Y] ) );

    line_list_view_->setText( lC::DETAIL, centerline_->line()->detail() );
  }

  /*
   * We are now responsible for our own dimension line view (if necessary)
   */
  void CenterlineView::modifiedConstraint ( void )
  {
    if ( centerline_->line()->needsDimensionView() ) {
      if ( dimension_view_ == 0 ) {
	Vertex v = centerline_->line()->reference()->parent()->
	  closestVertex( Point(), centerline_->line() );
	Curve* reference = v.curve2_;

	lC::Orientation orientation;
	if ( centerline_->line()->isHorizontal() )
	  orientation = lC::VERTICAL;
	else
	  orientation = lC::HORIZONTAL;

	dimension_view_ = new DimensionView( centerline_->line(),
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
	Vertex v = centerline_->line()->reference()->parent()->
	  closestVertex( Point(), centerline_->line() );
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
	  xml_rep.setAttribute( lC::STR::URL, centerline_->dbURL().toString(true) );
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
			      arg( centerline_->line()->o()[X] ).
			      arg( centerline_->line()->o()[Y] ).
			      arg( centerline_->line()->e()[X] ).
			      arg( centerline_->line()->e()[Y] ) );

    line_list_view_->setText( lC::DETAIL, centerline_->line()->detail() );
  }

  void CenterlineView::dmvRefModified ( void )
  {
    dimension_view_->computeLayout();
  }

  void CenterlineView::destroyedDmVRef ( void )
  {
    QDomDocument* xml_doc = ConstraintHistory::instance().history();
    if ( xml_doc != 0 ) {
      QDomElement xml_rep = xml_doc->createElement( lC::STR::DIMENSION_VIEW );
      xml_rep.setAttribute( lC::STR::URL, centerline_->dbURL().toString(true) );
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

  void CenterlineView::editInformation ( void )
  {
    if ( FigureViewBase::isActivated() ) {
      editCenterlineInformation();
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

  void CenterlineView::editCenterlineInformation ( void )
  {
    centerline_info_dialog_->nameEdit->
      setText( lC::formatName( centerline_->name() ) );

    if ( centerline_->line()->isHorizontal() ) {
      centerline_info_dialog_->horizontalButton->setChecked( true );
      centerline_info_dialog_->verticalButton->setChecked( false );
    }
    else {
      centerline_info_dialog_->horizontalButton->setChecked( false );
      centerline_info_dialog_->verticalButton->setChecked( true );
    }

    centerline_info_dialog_->referenceLineEdit->
      setText( lC::formatName( centerline_->line()->reference()->path() ) );

    centerline_info_dialog_->specifiedRadioButton->setChecked( true );
    centerline_info_dialog_->offsetLengthSpinBox->setEnabled( true );

    // Can't do this yet...
    centerline_info_dialog_->importedRadioButton->setEnabled( false );
    centerline_info_dialog_->constraintChooser->setEnabled( false );

    centerline_info_dialog_->offsetLengthSpinBox->
      setLengthLimits( UnitsBasis::instance()->lengthUnit(),
		       UnitsBasis::instance()->format(),
		       UnitsBasis::instance()->precision(),
		       lC::MINIMUM_DIMESION, lC::MAXIMUM_DIMENSION,
		       fabs( centerline_->line()->offset() ) );

    centerline_info_dialog_->offsetLengthSpinBox->
      setLength( fabs( centerline_->line()->offset() ) );
    
  redo:
    int ret = centerline_info_dialog_->exec();

    if ( ret == QDialog::Rejected ) return;

    bool modified = false;

    if ( centerline_info_dialog_->nameEdit->edited() ) {
      // Pageview handles checking the name and putting up the error dialog
      // if necessary.
      int ret = parent()->uniqueFigureName( this,
					  centerline_info_dialog_->nameEdit->text(),
					    centerline_->type() );

      switch ( ret ) {
      case lC::Rejected:
	return;
      case lC::Redo:
	centerline_info_dialog_->nameEdit->
	  setText( lC::formatName( centerline_->name() ) );
	goto redo;
      }

      setName( centerline_info_dialog_->nameEdit->text() );
      modified = true;
    }

    if ( centerline_info_dialog_->offsetLengthSpinBox->edited() ) {
      double old_offset = centerline_->line()->offset();

      if ( centerline_->line()->offset() >= 0 )
	centerline_->line()->
	  setOffset( centerline_info_dialog_->offsetLengthSpinBox->length() );
      else
	centerline_->line()->
	  setOffset( -centerline_info_dialog_->offsetLengthSpinBox->length() );

      MoveLinesCommand* command =
	new MoveLinesCommand( "move centerline", model() );

      command->add( centerline_->line(), old_offset );

      CommandHistory::instance().addCommand( command );

      modified = true;
    }

    if ( modified ) parent()->figureModified();
  }

  void CenterlineView::write ( QDomElement& xml_rep ) const
  {
    if ( !centerline_->isValid() ) return;

    QDomDocument document = xml_rep.ownerDocument();
    QDomElement centerline_view_element =
      document.createElement( lC::STR::CENTERLINE_VIEW );

    centerline_view_element.setAttribute( lC::STR::CENTERLINE,
					  centerline_->dbURL().toString( true ) );

    if ( dimension_view_ != 0 )
      dimension_view_->write( centerline_view_element );

    xml_rep.appendChild( centerline_view_element );
  }
  /*
   * Create an XML representation of a simplified version of this view
   * and its centerline.
   */
  void CenterlineView::copyWrite ( QDomElement& xml_rep ) const
  {
    QDomDocument document = xml_rep.ownerDocument();
    QDomElement centerline_view_element =
      document.createElement( lC::STR::CENTERLINE_VIEW );

    centerline_->copyWrite( centerline_view_element );

    xml_rep.appendChild( centerline_view_element );
  }
  /*
   * Detect changes to the name initiated elsewhere
   */
  void CenterlineView::updateName ( const QString& /*name*/ )
  {
  list_view_item_->setText( lC::NAME, lC::formatName( centerline_->name() )
			    + QString( " <%1>" ).arg( centerline_->id() ) );
    line_list_view_->setText( lC::NAME,
			      lC::formatName( centerline_->line()->name()  ) );
    line_list_view_->setText( lC::DETAIL, centerline_->line()->detail() );
  }

  // The least little change to user preferences requires that any
  // dimension views be recomputed.
  void CenterlineView::updateDimensionViews ( void )
  {
    if ( dimension_view_ != 0 ) dimension_view_->computeLayout();
  }
} // End of Space2D namespace
