/*
 * alignmentview.cpp
 *
 * (Coincident) Alignment Input class
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
#include <qevent.h>
#include <qcursor.h>
#include <qwhatsthis.h>

#include "command.h"
#include "ui_lignumcadmainwindow.h"
#include "cursorfactory.h"
#include "openglview.h"
#include "pageview.h"
#include "figureview.h"
#include "line.h"
#include "alignmentview.h"

namespace Space2D {

  /*
   * Simple constructor.
   */
  AlignmentCreateInput::AlignmentCreateInput ( PageView* parent )
    : parent_( parent ), from_( 0 ), to_( 0 ), xml_rep_( 0 )
  {}

  /*
   *Destructor.
   */
  AlignmentCreateInput::~AlignmentCreateInput ( void )
  {
    // Clean up the undo/redo command representation if necessary.

    if ( xml_rep_ != 0 ) delete xml_rep_;
  }

  /*
   * Prep for operation.
   */
  void AlignmentCreateInput::startDisplay ( QPopupMenu* /*context_menu*/ )
  {
    from_ = to_ = 0;

    xml_rep_ = new QDomDocument;

    QAction* cancel_action = parent_->lCMW()->cancelAlignmentAction;

    separator_id_ = parent_->view()->contextMenu()->insertSeparator();
    cancel_action->addTo( parent_->view()->contextMenu() );
    connect( cancel_action, SIGNAL( activated() ), SLOT( cancelOperation() )  );

    parent_->view()->
      setCursor( CursorFactory::instance().cursor( CursorFactory::ALIGNMENT ) );
  }

  /*
   * Actions for each geometry pick.
   */
  void AlignmentCreateInput::mousePress ( QMouseEvent* /*me*/,
					  const SelectedNames& selected )
  {
    // This is in pick mode, so we only consider the first geometry
    // selected (rather than all of them like we had...)

    // If the user just clicks randomly, ignore it, but stay in pick mode.
    if ( selected.empty() )
      return;

    SelectedNames::const_iterator f = selected.begin();

    // This shouldn't happen, but we can ignore it and stay in pick mode.
    if ( (*f).second.size() < 2 )
      return;

    FigureView* fv =
	dynamic_cast< FigureView* >( parent_->figureSelectionNames()[ (*f).second[0] ]);

    GLuint g = (*f).second[1]; // (*(*f).second->begin()).first;
    ConstrainedLine* line = dynamic_cast< ConstrainedLine* >( fv->geometry( g ) );

    if ( line != 0 ) {
      if ( from_ == 0 ) {
	from_ = line;

	parent_->view()->
	  setCursor( CursorFactory::instance().cursor( CursorFactory::ALIGNMENTPLUS ) );
      }
      else {
	// Make a few basic checks to see if this comensurate with from_
	if ( line == from_ ) {
	  QWhatsThis::display( "<table cellpadding=10><tr>"
			       "<td><img source=\"not_allowed.png\"></td>"
			       "<td width=\"80%\">Cannot align to self.</td>"
			       "</tr></table>" );
	  return;
	}
	if ( fabs( fabs( line->e() * from_->e() ) - 1. ) > lC::EPSILON ) {
	  QWhatsThis::display( "<table cellpadding=10><tr>"
			       "<td><img source=\"not_allowed.png\"></td>"
			       "<td width=\"80%\">Cannot align non-parallel lines.</td>"
			       "</tr></table>" );
	  return;
	}
	if ( from_->parent() == line->parent() ) {
	  QWhatsThis::display( "<table cellpadding=10><tr>"
			       "<td><img source=\"not_allowed.png\"></td>"
			       "<td width=\"80%\">Cannot align two edges in the same figure.</td>"
			       "</tr></table>" );
	  return;
	}
	if ( line->dependsOn( from_ ) ) {
	  QWhatsThis::display( "<table cellpadding=10><tr>"
			       "<td><img source=\"not_allowed.png\"></td>"
			       "<td width=\"80%\">Alignment would create circular reference.</td>"
			       "</tr></table>" );
	  return;
	}
	to_ = line;
      }
    }
  }

  /*
   * If the user is done, perform the reconstaint.
   */
  bool AlignmentCreateInput::mouseRelease ( QMouseEvent* /*me*/,
					    const SelectedNames& /*selected*/ )
  {
    if ( to_ == 0 )
      return false;


    QDomElement reconstraints = xml_rep_->createElement( lC::STR::RECONSTRAINTS );
    xml_rep_->appendChild( reconstraints );

    QDomElement old_constraints = xml_rep_->createElement(lC::STR::OLD_CONSTRAINTS);
    reconstraints.appendChild( old_constraints );

    QDomElement old_xml = xml_rep_->createElement( lC::STR::CONSTRAINED_LINE );
    old_xml.setAttribute( lC::STR::URL, from_->dbURL().toString(true) );
    from_->constraint()->write( old_xml );
    old_constraints.appendChild( old_xml );

    // Making this contraint can, in some cases, cause the target
    // line's former reference to be reconstrained. This happens at
    // least in Rectangle in order to keep a width- or height-like
    // dimension. In undoing this operation, that cascade change must
    // be done first in order to avoid a circular reference.  How do
    // we know it's going to happen? I guess we have to look at it
    // before and after...
    ConstrainedLine* from_reference =
      dynamic_cast<ConstrainedLine*>( from_->reference() );

    if ( from_reference != 0 ) {
      from_ref_xml_ = xml_rep_->createElement( lC::STR::CONSTRAINED_LINE );
      from_ref_xml_.setAttribute( lC::STR::URL, from_reference->dbURL().toString(true) );
      from_reference->constraint()->write( from_ref_xml_ );

      // If from_reference_'s modifiedConstraint signal is emitted,
      // then the old XML representation of its constraint is appended
      // to this reconstrain command (done in the SLOT).

      connect( from_reference, SIGNAL( modifiedConstraint() ),
	       SLOT( cascadeConstraints() ) );
    }

    // It is now possible to just create this contraint and the affected
    // figures will react to signals from the modified line.

    from_->setConstraint( new Coincident( to_ ) );

    if ( from_reference != 0 )
      disconnect( from_reference, SIGNAL( modifiedConstraint() ),
		  this, SLOT( cascadeConstraints() ) );

    QDomElement new_constraints = xml_rep_->createElement( lC::STR::NEW_CONSTRAINTS );
    reconstraints.appendChild( new_constraints );

    QDomElement new_xml = xml_rep_->createElement( lC::STR::CONSTRAINED_LINE );
    new_xml.setAttribute( lC::STR::URL, from_->dbURL().toString(true) );
    from_->constraint()->write( new_xml );
    new_constraints.appendChild( new_xml );

    CommandHistory::instance().
      addCommand( new ReconstrainCommand( "reconstrain line",
					  parent_->model(),
					  xml_rep_ ) );

    xml_rep_ = 0;

    QAction* cancel_action = parent_->lCMW()->cancelAlignmentAction;
    cancel_action->disconnect();
    cancel_action->removeFrom( parent_->view()->contextMenu() );
    parent_->view()->contextMenu()->removeItem( separator_id_ );

    parent_->view()->unsetCursor();

    parent_->figureComplete();

    return true;
  }

  /*
   * Cancel the operation if the user presses the Escape key.
   */
  void AlignmentCreateInput::keyPress ( QKeyEvent* ke )
  {
    if ( ke->key() == Qt::Key_Escape )
      cancelOperation();
    else
      ke->ignore();
  }

  /*
   * Cancel the coincident constraint creation.
   */
  void AlignmentCreateInput::cancelOperation ( void )
  {
    // Normally, the Command would take responsibility for the XML
    // representation, but if the aligment creation is cancelled, it's never
    // passed along.
    delete xml_rep_;
    xml_rep_ = 0;

    QAction* cancel_action = parent_->lCMW()->cancelAlignmentAction;

    cancel_action->disconnect();
    cancel_action->removeFrom( parent_->view()->contextMenu() );
    parent_->view()->contextMenu()->removeItem( separator_id_ );

    parent_->cancelOperation();
  }

  /*
   * Pick up any additional constraints which are changed as result of this
   * operation.
   */
  void AlignmentCreateInput::cascadeConstraints ( void )
  {
    // The primary reconstraint target is buried deep in this representation.
    // This change has to go before it so it can be undone properly.
    xml_rep_->firstChild().firstChild().insertBefore( from_ref_xml_,
			      xml_rep_->firstChild().firstChild().firstChild() );
  }

} // End of Space2D namespace
