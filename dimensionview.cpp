/*
 * dimensionview.cpp
 *
 * DimensionView classes: The View and Controllers for Dimensions
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
#include <qaction.h>
#include <qpopupmenu.h>
#include <qwhatsthis.h>
#include <qlineedit.h>
#include <qradiobutton.h>
#include <qbuttongroup.h>
#include <qcursor.h>
#include <qmessagebox.h>
#include <qpainter.h>

#include "configuration.h"
#include "units.h"
#include "command.h"
#include "openglview.h"
#include "lignumcadmainwindow.h"
#include "cursorfactory.h"
#include "line.h"
#include "pageview.h"
#include "model.h"
#include "figure.h"
#include "lcdefaultlengthspinbox.h"
#include "designbookview.h"
#include "dimensioninfodialog.h"
#include "dimensionview.h"

namespace Space2D {

  DimensionCreateInput::DimensionCreateInput ( PageView* parent )
    : parent_( parent ), n_geometries_( 0 ), xml_rep_( 0 )
  {}

  DimensionCreateInput::~DimensionCreateInput ( void )
  {
    if ( xml_rep_ != 0 ) delete xml_rep_;
  }

  void DimensionCreateInput::startDisplay ( QPopupMenu* context_menu )
  {
    n_geometries_ = 0;
    points_.clear();
    lines_.clear();

    xml_rep_ = new QDomDocument;

    context_menu_ = context_menu;

    QAction* cancel_action = parent_->lCMW()->cancelDimensionAction;

    separator_id_ = context_menu_->insertSeparator();
    cancel_action->addTo( context_menu_ );
    connect( cancel_action, SIGNAL( activated() ), SLOT( cancelOperation() ) );

    parent_->view()->
      setCursor( CursorFactory::instance().cursor( CursorFactory::DIMENSION ) );
  }

  void DimensionCreateInput::mousePrepress ( QMouseEvent* me,
					     const SelectedNames& selected )
  {
    (void)me;
    (void)selected;
  }

  void DimensionCreateInput::mousePress ( QMouseEvent* me,
					  const SelectedNames& selected )
  {
    // This is in pick mode, so we only consider the first geometry
    // selected (rather than all of them like we had...)
    if ( selected.empty() )
      return;

    SelectedNames::const_iterator f = selected.begin();

    if ( (*f).second.size() < 2 )
      return;

    FigureView* fv =
      dynamic_cast< FigureView* >( parent_->figureSelectionNames()[ (*f).second[0] ]);

    // Let the first selected figure decide how many selections we need.
    // (This is entirely for the Centerline, which wants 2 additional
    // edges instead of the usual 1.)
    if ( lines_.count() == 0 )
      n_geometries_ = fv->dimensionPickCount();

    GLuint g = (*f).second[1]; // (*(*f).second->begin()).first;
    ConstrainedLine* line = dynamic_cast< ConstrainedLine* >( fv->geometry( g ) );

    if ( line != 0 ) {

      Point point = parent_->view()->unproject2D( me->pos() );
      UnitsBasis::instance()->round( point );

      QPtrListIterator< ConstrainedLine > l( lines_ );

      for ( ; l.current(); ++l ) {
	if ( line == l.current() ) {
	  QWhatsThis::display( "<table cellpadding=10><tr>"
			       "<td><img source=\"not_allowed.png\"></td>"
			       "<td width=\"70%\">Cannot dimension to self.</td>"
			       "</tr></table>" );
	  return;
	}
	if ( fabs( fabs( line->e() * l.current()->e() ) - 1. ) > lC::EPSILON ) {
	  QWhatsThis::display( "<table cellpadding=10><tr>"
			       "<td><img source=\"not_allowed.png\"></td>"
			       "<td width=\"70%\">Cannot dimension non-parallel lines.</td>"
			       "</tr></table>" );
	  return;
	}
	if ( line->dependsOn( l.current() ) ) {
	  QWhatsThis::display( "<table cellpadding=10><tr>"
			       "<td><img source=\"not_allowed.png\"></td>"
			       "<td width=\"70%\">Dimension would create circular reference.</td>"
			       "</tr></table>" );
	  return;
	}
      }

      points_.push_back( point );
      lines_.append( line );
    }

    if ( lines_.count() == 1 )
      parent_->view()->
	setCursor( CursorFactory::instance().cursor( CursorFactory::DIMENSIONPLUS ) );

  }

  void DimensionCreateInput::mouseDrag ( QMouseEvent* me,
					 const SelectedNames& selected )
  {
    (void)me;
    (void)selected;
  }

  bool DimensionCreateInput::mouseRelease ( QMouseEvent* me,
					    const SelectedNames& selected )
  {
    (void)me;
    (void)selected;

    if ( n_geometries_ == 0 || lines_.count() < n_geometries_ )
      return false;
    // This should probably be passed off to the FigureView to do
    // the right thing with...Indeed...
    if ( lines_.count() == 2 ) {
      ConstrainedLine* from = lines_.first();
      ConstrainedLine* to = lines_.next();

      QDomElement reconstraints = xml_rep_->createElement( lC::STR::RECONSTRAINTS );
      xml_rep_->appendChild( reconstraints );

      QDomElement old_constraints = xml_rep_->createElement(lC::STR::OLD_CONSTRAINTS);
      reconstraints.appendChild( old_constraints );

      QDomElement old_xml = xml_rep_->createElement( lC::STR::CONSTRAINED_LINE );
      old_xml.setAttribute( lC::STR::URL, from->dbURL().toString(true) );
      from->constraint()->write( old_xml );
      old_constraints.appendChild( old_xml );

      // Making this contraint can, in some cases, cause the target
      // line's former reference to be reconstrained. This happens
      // at least in Rectangle in order to keep a width- or height-like
      // dimension. In undoing this operation, that cascade change
      // must be done first in order to avoid a circular reference.
      // How do we know it's going to happen? Catch the signal emitted
      // when the target's former reference is reconstrained itself.

      from_reference_ = dynamic_cast<ConstrainedLine*>( from->reference() );

      if ( from_reference_ != 0 ) {
	from_ref_xml_ = xml_rep_->createElement( lC::STR::CONSTRAINED_LINE );
	from_ref_xml_.setAttribute( lC::STR::URL, from_reference_->dbURL().toString(true) );
	from_reference_->constraint()->write( from_ref_xml_ );

	// If from_reference_'s modifiedConstraint signal is emitted,
	// then the old XML representation of its constraint is
	// appended to this reconstrain command (done in the SLOT).

	connect( from_reference_, SIGNAL( modifiedConstraint() ),
		 SLOT( cascadeConstraints() ) );
      }

      Offset* offset = new Offset( from->o(), to );

      QDomElement new_constraints = xml_rep_->createElement( lC::STR::NEW_CONSTRAINTS );
      reconstraints.appendChild( new_constraints );

      QDomElement new_xml = xml_rep_->createElement( lC::STR::CONSTRAINED_LINE );
      new_xml.setAttribute( lC::STR::URL, from->dbURL().toString(true) );
      offset->write( new_xml );
      new_constraints.appendChild( new_xml );

      from->setConstraint( offset );

      if ( from_reference_ != 0 )
	disconnect( from_reference_, SIGNAL( modifiedConstraint() ),
		    this, SLOT( cascadeConstraints() ) );

      CommandHistory::instance().
	addCommand( new ReconstrainCommand( "reconstrain line",
					    parent_->model(),
					    xml_rep_ ) );

      xml_rep_ = 0;
    }
    else if ( lines_.count() == 3 ) {
      ConstrainedLine* to = lines_.first();
      ConstrainedLine* from = lines_.next();

      QDomElement reconstraints = xml_rep_->createElement( lC::STR::RECONSTRAINTS );
      xml_rep_->appendChild( reconstraints );

      QDomElement old_constraints = xml_rep_->createElement( lC::STR::OLD_CONSTRAINTS );
      reconstraints.appendChild( old_constraints );

      QDomElement old_xml = xml_rep_->createElement( lC::STR::CONSTRAINED_LINE );
      old_xml.setAttribute( lC::STR::URL, from->dbURL().toString(true) );
      from->constraint()->write( old_xml );
      old_constraints.appendChild( old_xml );

      // Making this contraint can, in some cases, cause the target
      // line's former reference to be reconstrained. This happens
      // at least in Rectangle in order to keep a width- or height-like
      // dimension. In undoing this operation, that cascade change
      // must be done first in order to avoid a circular reference.
      // How do we know it's going to happen? Catch the signal emitted
      // when the target's former reference is reconstrained itself.

      from_reference_ = dynamic_cast<ConstrainedLine*>( from->reference() );

      if ( from_reference_ != 0 ) {
	from_ref_xml_ = xml_rep_->createElement( lC::STR::CONSTRAINED_LINE );
	from_ref_xml_.setAttribute( lC::STR::URL, from_reference_->dbURL().toString(true) );
	from_reference_->constraint()->write( from_ref_xml_ );

	// If from_reference_'s modifiedConstraint signal is emitted,
	// then the old XML representation of its constraint is
	// appended to this reconstrain command (done in the SLOT).

	connect( from_reference_, SIGNAL( modifiedConstraint() ),
		 SLOT( cascadeConstraints() ) );
      }

      from->setConstraint( new Offset( from->o(), to ) );

      if ( from_reference_ != 0 )
	disconnect( from_reference_, SIGNAL( modifiedConstraint() ),
		    this, SLOT( cascadeConstraints() ) );

      QDomElement new_constraints = xml_rep_->createElement( lC::STR::NEW_CONSTRAINTS );
      reconstraints.appendChild( new_constraints );

      QDomElement new_xml = xml_rep_->createElement( lC::STR::CONSTRAINED_LINE );
      new_xml.setAttribute( lC::STR::URL, from->dbURL().toString(true) );
      from->constraint()->write( new_xml );
      new_constraints.appendChild( new_xml );

      to = from;
      from = lines_.next();

      // Note: Obviously, must append these subsequent changes in the
      // REVERSE order of their creation!

      QDomElement old_xml2 = xml_rep_->createElement( lC::STR::CONSTRAINED_LINE );
      old_xml2.setAttribute( lC::STR::URL, from->dbURL().toString(true) );
      from->constraint()->write( old_xml2 );
      old_constraints.insertBefore( old_xml2, old_constraints.firstChild() );

      // Making this contraint can, in some cases, cause the target
      // line's former reference to be reconstrained. This happens
      // at least in Rectangle in order to keep a width- or height-like
      // dimension. In undoing this operation, that cascade change
      // must be done first in order to avoid a circular reference.
      // How do we know it's going to happen? Catch the signal emitted
      // when the target's former reference is reconstrained itself.

      from_reference_ = dynamic_cast<ConstrainedLine*>( from->reference() );

      if ( from_reference_ != 0 ) {
	from_ref_xml_ = xml_rep_->createElement( lC::STR::CONSTRAINED_LINE );
	from_ref_xml_.setAttribute( lC::STR::URL, from_reference_->dbURL().toString(true) );
	from_reference_->constraint()->write( from_ref_xml_ );

	// If from_reference_'s modifiedConstraint signal is emitted,
	// then the old XML representation of its constraint is
	// appended to this reconstrain command (done in the SLOT).

	connect( from_reference_, SIGNAL( modifiedConstraint() ),
		 SLOT( cascadeConstraints() ) );
      }

      from->setConstraint( new Centered( from->o(), to ) );

      if ( from_reference_ != 0 )
	disconnect( from_reference_, SIGNAL( modifiedConstraint() ),
		    this, SLOT( cascadeConstraints() ) );


      QDomElement new_xml2 = xml_rep_->createElement( lC::STR::CONSTRAINED_LINE );
      new_xml2.setAttribute( lC::STR::URL, from->dbURL().toString(true) );
      from->constraint()->write( new_xml2 );
      new_constraints.appendChild( new_xml2 );

      CommandHistory::instance().
	addCommand( new ReconstrainCommand( "reconstrain line",
					    parent_->model(),
					    xml_rep_ ) );
      xml_rep_ = 0;
    }

    QAction* cancel_action = parent_->lCMW()->cancelDimensionAction;
    cancel_action->disconnect();
    cancel_action->removeFrom( context_menu_ );
    context_menu_->removeItem( separator_id_ );

    parent_->view()->unsetCursor();

    parent_->figureComplete();

    return true;
  }

  void DimensionCreateInput::keyPress ( QKeyEvent* ke )
  {
    if ( ke->key() == Qt::Key_Escape )
      cancelOperation();
    else
      ke->ignore();
  }

  void DimensionCreateInput::cancelOperation ( void )
  {
    // Normally, the Command would take responsibility for this
    // object, but if the dimension creation is cancelled, it's never
    // passed along.
    delete xml_rep_;
    xml_rep_ = 0;

    QAction* cancel_action = parent_->lCMW()->cancelDimensionAction;

    cancel_action->disconnect();
    cancel_action->removeFrom( context_menu_ );
    context_menu_->removeItem( separator_id_ );

    parent_->cancelOperation();
  }

  void DimensionCreateInput::cascadeConstraints ( void )
  {
    // The primary reconstraint target is buried deep in this representation.
    // This change has to go before it so it can be undone properly.
    xml_rep_->firstChild().firstChild().insertBefore( from_ref_xml_,
						      xml_rep_->firstChild().firstChild().firstChild() );
  }

  // Since these operations are unique to the dimension view, the
  // undo/redo command is defined here rather than in command.cpp.
  // We'll see if this is reasonable...
  class MoveDimensionCommand : public Command {
    DimensionView* dimension_view_;
    DesignBookView* design_book_view_;
    DBURL db_url_;
    double old_extension_offset_;
    double new_extension_offset_;
    double old_dimension_offset_;
    double new_dimension_offset_;
    enum Dimension::Placement old_placement_;
    enum Dimension::Placement new_placement_;
    lC::AnnotationSide old_annotation_side_;
    lC::AnnotationSide new_annotation_side_;

  public:
    MoveDimensionCommand ( const QString& name, DimensionView* dimension_view,
			   double old_extension_offset, double new_extension_offset,
			   double old_dimension_offset, double new_dimension_offset,
			   enum Dimension::Placement old_placement,
			   enum Dimension::Placement new_placement,
			   lC::AnnotationSide old_annotation_side,
			   lC::AnnotationSide new_annotation_side )
      : Command( name ),
	dimension_view_( dimension_view ),
	design_book_view_( dimension_view_->parent()->parent()->parent() ),
	db_url_( dimension_view_->dimension()->dbURL() ),
	old_extension_offset_( old_extension_offset ),
	new_extension_offset_( new_extension_offset ),
	old_dimension_offset_( old_dimension_offset ),
	new_dimension_offset_( new_dimension_offset ),
	old_placement_( old_placement ), new_placement_( new_placement ),
	old_annotation_side_( old_annotation_side ),
	new_annotation_side_( new_annotation_side )
    {}

    Type type ( void ) const { return MoveDimension; }

    bool merge ( Command* /*command*/ ) { return false; }

    void execute ( void )
    {
      dimension_view_ =
	dynamic_cast<DimensionView*>( design_book_view_->lookup( db_url_ ) );

      dimension_view_->setDimensionAttributes( new_extension_offset_,
					       new_dimension_offset_,
					       new_placement_ );
      dimension_view_->setAnnotationSide( new_annotation_side_ );
    }
    void unexecute ( void )
    {
      dimension_view_ =
	dynamic_cast<DimensionView*>( design_book_view_->lookup( db_url_ ) );

      dimension_view_->setDimensionAttributes( old_extension_offset_,
					       old_dimension_offset_,
					       old_placement_ );
      dimension_view_->setAnnotationSide( old_annotation_side_ );
    }
    void write ( QDomDocument* document ) const
    {
      QDomElement move_element = document->createElement( lC::STR::MOVE_DIMENSION );
      QDomNode root_node = document->firstChild();
      if ( !root_node.isNull() )
	document->replaceChild( move_element, root_node );
      else
	document->appendChild( move_element );

      move_element.setAttribute( lC::STR::URL, db_url_ );
      move_element.setAttribute( lC::STR::OLD_EXTENSION_OFFSET,
				 lC::format( old_extension_offset_ ) );
      move_element.setAttribute( lC::STR::NEW_EXTENSION_OFFSET,
				 lC::format( new_extension_offset_ ) );

      move_element.setAttribute( lC::STR::OLD_DIMENSION_OFFSET,
				 lC::format( old_dimension_offset_ ) );
      move_element.setAttribute( lC::STR::NEW_DIMENSION_OFFSET,
				 lC::format( new_dimension_offset_ ) );

      move_element.setAttribute( lC::STR::OLD_PLACEMENT,
				 placementText( old_placement_ ) );
      move_element.setAttribute( lC::STR::NEW_PLACEMENT,
				 placementText( new_placement_ ) );

      move_element.setAttribute( lC::STR::OLD_ANNOTATION_SIDE,
				 annotationSideText( old_annotation_side_ ) );
      move_element.setAttribute( lC::STR::NEW_ANNOTATION_SIDE,
				 annotationSideText( new_annotation_side_ ) );
    }
  };

  DimensionModifyInput::DimensionModifyInput ( ConstrainedLine* dimension,
					       DimensionView* dimension_view )
    : selection_type_( TRANSIENT, FIGURE ),
      dimensioned_line_( dimension ), dimension_view_( dimension_view ),
      old_extension_offset_( 0 ), free_( false )
  {}

  void DimensionModifyInput::startDisplay ( QPopupMenu* /*context_menu*/ )
  {
    free_ = false;
  }

  void DimensionModifyInput::mousePrepress ( QMouseEvent* me,
					     const SelectedNames& selected )
  {
    SelectedNames::const_iterator f;

    if ( selected.empty() ) {
      dimension_view_->parent()->view()->unsetCursor();
      return;
    }

    // For each highlighted figure view
    for ( f = selected.begin(); f != selected.end(); ++f ) {
      // If the highlighted figure view is *this* view
      if ( (*f).second[0] == dimension_view_->parent()->selectionName() ) {
	// If this is referring to me
	if ( (*f).second.size() > 1 &&
	     (*f).second[1] == dimension_view_->selectionName() ) {

	  // If the mouse is over the text box, display the all-ways resize
	  // cursor (otherwise, the user is only allowed to change the extension
	  // line offset).
	  if ( (*f).second.size() > 2 ) {
	    dimension_view_->parent()->view()->setCursor( Qt::SizeAllCursor );
	    free_ = true;
	  }
	  else {
	    dimension_view_->parent()->view()->
	      setCursor( dimension_view_->cursorShape() );
	    free_ = false;
	  }

	  me->accept();
	}
	else {
	  dimension_view_->parent()->view()->unsetCursor();
	}
	return;
      }
    }
    // The dimension view's parent was not highlighted.
    dimension_view_->parent()->view()->unsetCursor();
  }

  void DimensionModifyInput::mousePress ( QMouseEvent* me,
					  const SelectedNames& selected )
  {
    // Let the Page handle these events

    if ( me->button() != Qt::LeftButton || selected.empty() ) {
      me->ignore();
      return;
    }

    SelectedNames::const_iterator f;

    for ( f = selected.begin(); f != selected.end(); ++f ) {
      // Make sure that it is our figure which is selected
      if ( (*f).second[0] == dimension_view_->parent()->selectionName() ) {
	// and that we are really selected
	if ( (*f).second.size() > 1 &&
	     (*f).second[1] == dimension_view_->selectionName() ) {

	  start_pnt_ = dimension_view_->parent()->view()->unproject2D( me->pos() );

	  last_pnt_ = start_pnt_;

	  old_extension_offset_ = dimension_view_->extensionOffset();
	  old_dimension_offset_ = dimension_view_->dimensionOffset();
	  old_placement_ = dimension_view_->placement();

	  selection_type_ = SelectionType( TRANSIENT, NONE );

	  return;
	}
      }
    }

    // If this wasn't highlighted, then also let the page handle it.
    me->ignore();
    return;
  }

  void DimensionModifyInput::mouseDrag ( QMouseEvent* me,
					 const SelectedNames& /*selected*/ )
  {
    current_pnt_ = dimension_view_->parent()->view()->unproject2D( me->pos() );
    UnitsBasis::instance()->round( current_pnt_ );

    bool ok = dimension_view_->adjust( free_, last_pnt_, current_pnt_ );

    if ( ok )
      last_pnt_ = current_pnt_;

    dimension_view_->parent()->parent()->figureModified();
  }

  bool DimensionModifyInput::mouseRelease ( QMouseEvent* me,
					    const SelectedNames& selected )
  {
    (void)me;
    (void)selected;

    // Only record an event if the dimension actually moved.
    if ( dimension_view_->extensionOffset() != old_extension_offset_ ) {
      CommandHistory::instance().
	addCommand( new MoveDimensionCommand( "move dimension",
					      dimension_view_,
					      old_extension_offset_,
					      dimension_view_->extensionOffset(),
					      old_dimension_offset_,
					      dimension_view_->dimensionOffset(),
					      old_placement_,
					      dimension_view_->placement(),
					      dimension_view_->annotationSide(),
					      dimension_view_->annotationSide() ) );
    }

    selection_type_ = SelectionType( TRANSIENT, FIGURE );

    return false;
  }

  void DimensionModifyInput::mouseDoubleClick ( QMouseEvent* me )
  {
    (void)me;
    dimension_view_->editInformation();
  }

  void DimensionModifyInput::setDimension ( ConstrainedLine* dimension )
  {
    dimensioned_line_ = dimension;
  }

  DimensionInfoDialog* DimensionView::dimension_info_dialog_ = 0;

  DimensionView::DimensionView ( ConstrainedLine* dimension,
				 Curve* from_reference, Curve* to_reference,
				 lC::Orientation orientation,
				 lC::AnnotationSide annotation_side,
				 GLuint selection_name,
				 FigureView* parent )
    : GraphicsView( selection_name ), parent_( parent ), dimensioned_line_( dimension ),
      from_reference_( from_reference ), to_reference_( to_reference ),
      dimension_( 0 ),
      modify_input_( dimension, this ),
      orientation_( orientation ), annotation_side_( annotation_side )
  {
    init();
  }

  DimensionView::DimensionView ( const QDomElement& xml_rep,
				 GLuint selection_name, FigureView* parent )
    : GraphicsView( selection_name ), parent_( parent ), dimensioned_line_( 0 ),
      from_reference_( 0 ), to_reference_( 0 ),
      dimension_( 0 ),
      modify_input_( 0, this )
  {
    annotation_side_ = lC::annotationSide( xml_rep.attribute( lC::STR::ANNOTATION_SIDE ) );
    orientation_ = lC::orientation( xml_rep.attribute( lC::STR::ORIENTATION ) );

    DBURL db_url( xml_rep.attribute( lC::STR::DIMENSION ) );

    dimensioned_line_ =
      dynamic_cast< ConstrainedLine* >( parent_->model()->lookup( db_url ) );

    db_url = xml_rep.attribute( lC::STR::FROM );

    from_reference_ = 
      dynamic_cast< Curve* >( parent_->model()->lookup( db_url ) );

    db_url = xml_rep.attribute( lC::STR::TO );

    to_reference_ = 
      dynamic_cast< Curve* >( parent_->model()->lookup( db_url ) );

    init();

    dimension_->
      setDimensionAttributes( xml_rep.attribute( lC::STR::EXTENSION_LINE_OFFSET ).toDouble(),
			      xml_rep.attribute( lC::STR::DIMENSION_OFFSET ).toDouble(),
			      Space2D::placement( xml_rep.attribute( lC::STR::PLACEMENT ) ) );

  }

  DimensionView::~DimensionView ( void )
  {}

  void DimensionView::init ( void )
  {

    if ( dimension_info_dialog_ == 0 ) {
      dimension_info_dialog_ =
	new DimensionInfoDialog( parent()->parent()->lCMW() );
      // Create invisible button groups connecting the Left/Right Above/Below
      // radio buttons, respectively.
      QButtonGroup* h = new QButtonGroup( dimension_info_dialog_->GroupBox1,"H");
      h->insert( dimension_info_dialog_->leftButton );
      h->insert( dimension_info_dialog_->rightButton );
      h->hide();
      QButtonGroup* v = new QButtonGroup( dimension_info_dialog_->GroupBox1,"V");
      v->insert( dimension_info_dialog_->aboveButton );
      v->insert( dimension_info_dialog_->belowButton );
      v->hide();

      dimension_info_dialog_->offsetLengthSpinBox->
	setLengthLimits( UnitsBasis::instance()->lengthUnit(),
			 UnitsBasis::instance()->format(),
			 UnitsBasis::instance()->precision(),
			 lC::MINIMUM_DIMESION, lC::MAXIMUM_DIMENSION, 0 );
    }

    computeLayout();
  }

  QString DimensionView::name ( void ) const
  {
    return dimensioned_line_->name();
  }

  DBURL DimensionView::dbURL ( void ) const
  {
    return dimensioned_line_->dbURL();
  }

  void DimensionView::setName ( const QString& name )
  {
    DBURL old_db_url = dimensioned_line_->dbURL();

    dimensioned_line_->setName( name );

    CommandHistory::instance().
      addCommand( new RenameCommand( "rename dimension", parent()->model(),
				     old_db_url, dimensioned_line_->dbURL() ) );
  }

  void DimensionView::setAnnotationSide ( lC::AnnotationSide annotation_side )
  {
    annotation_side_ = annotation_side;

    computeLayout();
  }

  void DimensionView::setReferences ( Curve* from_reference, Curve* to_reference )
  {
    from_reference_ = from_reference;
    to_reference_ = to_reference;

    computeLayout();
  }

  void DimensionView::editInformation ( void )
  {
    QString length_str = QString::number( fabs( dimensioned_line_->offset() ) );

    dimension_info_dialog_->nameEdit->
      setText( lC::formatName( dimensioned_line_->name() ) );
    dimension_info_dialog_->nameEdit->selectAll();

    dimension_info_dialog_->toReferenceLineEdit->
      setText( lC::formatName( dimensioned_line_->path() ) );

    dimension_info_dialog_->fromReferenceLineEdit->
      setText( lC::formatName( dimensioned_line_->reference()->path() ) );

    dimension_info_dialog_->offsetLengthSpinBox->
      setLengthLimits( UnitsBasis::instance()->lengthUnit(),
		       UnitsBasis::instance()->format(),
		       UnitsBasis::instance()->precision(),
		       lC::MINIMUM_DIMESION, lC::MAXIMUM_DIMENSION,
		       fabs( dimensioned_line_->offset() ) );
    dimension_info_dialog_->offsetLengthSpinBox->
      setLength( fabs( dimensioned_line_->offset() ) );

    switch ( orientation_ ) {
    case lC::HORIZONTAL:
      dimension_info_dialog_->horizontalButton->setChecked( true );

      dimension_info_dialog_->leftButton->setEnabled( false );
      dimension_info_dialog_->rightButton->setEnabled( false );
      dimension_info_dialog_->belowButton->setEnabled( true );
      dimension_info_dialog_->aboveButton->setEnabled( true );

      switch ( annotation_side_ ) {
      case lC::ABOVE:
	dimension_info_dialog_->aboveButton->setChecked( true );
	break;
      case lC::BELOW:
	dimension_info_dialog_->belowButton->setChecked( true );
      }
      break;
    case lC::VERTICAL:
      dimension_info_dialog_->verticalButton->setChecked( true );

      dimension_info_dialog_->leftButton->setEnabled( true );
      dimension_info_dialog_->rightButton->setEnabled( true );
      dimension_info_dialog_->belowButton->setEnabled( false );
      dimension_info_dialog_->aboveButton->setEnabled( false );

      switch ( annotation_side_ ) {
      case lC::ABOVE:
	dimension_info_dialog_->rightButton->setChecked( true );
	break;
      case lC::BELOW:
	dimension_info_dialog_->leftButton->setChecked( true );
      }
      break;
    case lC::OTHER:
      dimension_info_dialog_->otherButton->setChecked( true );

      dimension_info_dialog_->leftButton->setChecked( false );
      dimension_info_dialog_->rightButton->setChecked( false );
      dimension_info_dialog_->belowButton->setChecked( false );
      dimension_info_dialog_->aboveButton->setChecked( false );

      dimension_info_dialog_->leftButton->setEnabled( false );
      dimension_info_dialog_->rightButton->setEnabled( false );
      dimension_info_dialog_->belowButton->setEnabled( false );
      dimension_info_dialog_->aboveButton->setEnabled( false );
      break;
    }
  REDO:
    // Kind of gets lost in the noise here
    int ret = dimension_info_dialog_->exec();

    if ( ret == QDialog::Rejected ) return;

    bool modified = false;

    if ( dimension_info_dialog_->nameEdit->edited() ) {
#if 0
      if ( !setName( dimension_info_dialog_->nameEdit->text() ) )
	goto REDO;
#else
      if ( dimensioned_line_->parent()->uniqueGeometryName( dimensioned_line_,
							    dimension_info_dialog_->
							    nameEdit->text() ) )
	setName( dimension_info_dialog_->nameEdit->text() );
      else {
	QMessageBox mb( trC( lC::STR::LIGNUMCAD ),
			qApp->translate( "Space2D::DimensionView",
		 "The name \"%1\" for a geometry of type %2 already exists\n"
					 "as a child of %3." ).
			arg( dimension_info_dialog_->nameEdit->text() ).
			arg( trC( dimensioned_line_->type() ) ).
			arg( dimensioned_line_->parent()->name() ),
			QMessageBox::Information,
			QMessageBox::Yes | QMessageBox::Default,
			QMessageBox::Cancel,
			QMessageBox::NoButton );
	mb.setButtonText( QMessageBox::Yes,
			  qApp->translate( "Space2D::DimensionView",
					   "Enter another name" ) );
	mb.setButtonText( QMessageBox::Cancel,
			  qApp->translate( "Space2D::DimensionView",
					   "Cancel changes" ) );

	switch ( mb.exec() ) {
	case QMessageBox::Yes:
	  goto REDO;
	case QMessageBox::Cancel:
	  return;
	}
      }
#endif
      modified = true;
    }


    if ( dimension_info_dialog_->offsetLengthSpinBox->edited() ) {
      double old_offset = dimensioned_line_->offset();

      if ( dimensioned_line_->offset() >= 0 )
	dimensioned_line_->
	  setOffset( dimension_info_dialog_->offsetLengthSpinBox->length() );
      else
	dimensioned_line_->
	  setOffset( -dimension_info_dialog_->offsetLengthSpinBox->length() );

      MoveLinesCommand* move_command = new MoveLinesCommand( "resize dimension",
							     parent()->model() );
      move_command->add( dimensioned_line_, old_offset );

      CommandHistory::instance().addCommand( move_command );

      modified = true;
    }

    lC::AnnotationSide old_annotation_side = annotation_side_;
    bool annotation_side_modified = false;

    switch ( orientation_ ) {
    case lC::HORIZONTAL:
      if ( dimension_info_dialog_->aboveButton->isOn() &&
	   annotation_side_ == lC::BELOW ) {
	annotation_side_ = lC::ABOVE;
	computeLayout();
	annotation_side_modified = true;
      }
      else if ( dimension_info_dialog_->belowButton->isOn() &&
		annotation_side_ == lC::ABOVE ) {
	annotation_side_ =  lC::BELOW ;
	computeLayout();
	annotation_side_modified = true;
      }
      break;
    case lC::VERTICAL:
      if ( dimension_info_dialog_->rightButton->isOn() &&
	   annotation_side_ == lC::BELOW ) {
	annotation_side_ = lC::ABOVE;
	computeLayout();
	annotation_side_modified = true;
      }
      else if ( dimension_info_dialog_->leftButton->isOn() &&
		annotation_side_ == lC::ABOVE ) {
	annotation_side_ = lC::BELOW;
	computeLayout();
	annotation_side_modified = true;
      }
      break;
    case lC::OTHER:
      // Not sure what to do here. Indicates that the situation may not
      // be so complicated. Probably better to have some kind of graphical
      // representation of the annotation side, anyway.
      break;
    }

    if ( annotation_side_modified )
      CommandHistory::instance().
	addCommand( new MoveDimensionCommand( "move dimension",
					      this,
					      dimension_->extensionOffset(),
					      dimension_->extensionOffset(),
					      dimension_->dimensionOffset(),
					      dimension_->dimensionOffset(),
					      dimension_->placement(),
					      dimension_->placement(),
					      old_annotation_side,
					      annotation_side_ ) );

    if ( modified ) parent()->parent()->figureModified();
  }

  void DimensionView::draw ( void ) const
  {
    lC::Render::Mode mode = lC::Render::REGULAR;

    if ( isActivated() )
      mode = lC::Render::ACTIVATED;
    else if ( isHighlighted() )
      mode = lC::Render::HIGHLIGHTED;

    dimension_->draw( mode );
  }

  void DimensionView::select ( SelectionType selection_type ) const
  {
    if ( selection_type.entity_ != FIGURE ) return;

    glPushName( selectionName() );

    lC::Render::Mode mode = lC::Render::REGULAR;

    if ( isActivated() )
      mode = lC::Render::ACTIVATED;
    else if ( isHighlighted() )
      mode = lC::Render::HIGHLIGHTED;

    dimension_->select( mode );

    glPopName();
  }

  void DimensionView::setDimensionAttributes ( double extension_offset,
					       double dimension_offset,
					       enum Dimension::Placement placement )
  {
    dimension_->setDimensionAttributes( extension_offset, dimension_offset,
					placement );
  }

  double DimensionView::extensionOffset ( void ) const
  {
    return dimension_->extensionOffset();
  }

  double DimensionView::dimensionOffset ( void ) const
  {
    return dimension_->dimensionOffset();
  }

  enum Dimension::Placement DimensionView::placement ( void ) const
  {
    return dimension_->placement();
  }

  void DimensionView::computeLayout ( void )
  {
    Point end0 = dimensioned_line_->reference()->intersect( from_reference_ );
    Point end1 = dimensioned_line_->intersect( to_reference_ );

    Vector normal;

    switch ( orientation_ ) {
    case lC::HORIZONTAL:
      normal = Vector( 0, 1 ); break;
    case lC::VERTICAL:
      normal = Vector( 1, 0 ); break;
    }

    if ( annotation_side_ == lC::BELOW )
      normal *= -1;

    if ( dimension_ == 0 )
      dimension_ = new Dimension( end0, end1, normal, parent()->view(),
				  dimensioned_line_->note() );
    else
      dimension_->setEndsNormal( end0, end1, normal, parent()->view(),
				 dimensioned_line_->note() );

    // Note that some geometry does not have a parent, such as the X axis.
    bool use_extension_end0 = dimensioned_line_->reference()->parent() != 0 &&
      dimensioned_line_->reference()->parent()->needsExtensionLine();
    bool use_extension_end1 = dimensioned_line_->parent() != 0 &&
      dimensioned_line_->parent()->needsExtensionLine();

    dimension_->setUseExtensionLine( use_extension_end0, use_extension_end1 );

    // Finally, set the resize cursor
    switch ( orientation_ ) {
    case lC::HORIZONTAL:
      setCursorShape( Qt::SizeVerCursor );
      break;
    case lC::VERTICAL:
      setCursorShape( Qt::SizeHorCursor );
      break;
    }
  }

  bool DimensionView::adjust ( bool free, const Point& last_pnt, Point& current_pnt)
  {
    Vector delta = current_pnt - last_pnt;

    if ( free )
      dimension_->adjustDimension( delta );
    else
      dimension_->adjustExtension( delta );

    computeLayout();

    return true;
  }

  void DimensionView::write ( QDomElement& xml_rep )
  {
    QDomDocument document = xml_rep.ownerDocument();
    QDomElement dimension_view_element = document.createElement( lC::STR::DIMENSION_VIEW );
    dimension_view_element.setAttribute( lC::STR::DIMENSION,
					 dimensioned_line_->dbURL().toString(true));
    dimension_view_element.setAttribute( lC::STR::FROM,
					 from_reference_->dbURL().toString(true) );
    dimension_view_element.setAttribute( lC::STR::TO,
					 to_reference_->dbURL().toString(true) );
    dimension_view_element.setAttribute( lC::STR::ORIENTATION,
					 orientationText( orientation_ ) );
    dimension_view_element.setAttribute( lC::STR::ANNOTATION_SIDE,
					 annotationSideText( annotation_side_) );
    dimension_view_element.setAttribute( lC::STR::EXTENSION_LINE_OFFSET,
					 lC::format(dimension_->extensionOffset()));
    dimension_view_element.setAttribute( lC::STR::DIMENSION_OFFSET,
					 lC::format(dimension_->dimensionOffset()));
    dimension_view_element.setAttribute( lC::STR::PLACEMENT,
					 placementText( dimension_->placement() ) );
    xml_rep.appendChild( dimension_view_element );
  }
} // End of Space2D namespace
