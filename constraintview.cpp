/*
 * constraintview.cpp
 *
 * Constraint Deletion class
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
#include "lignumcadmainwindow.h"
#include "cursorfactory.h"
#include "openglview.h"
#include "pageview.h"
#include "figureview.h"
#include "line.h"
#include "constraintview.h"

namespace Space2D {

  ConstraintDeleteInput::ConstraintDeleteInput ( PageView* parent )
    : parent_( parent )
  {}

  ConstraintDeleteInput::~ConstraintDeleteInput ( void )
  {
    if ( xml_rep_ != 0 ) delete xml_rep_;
  }

  void ConstraintDeleteInput::startDisplay ( QPopupMenu* context_menu )
  {
    target_ = 0;

    xml_rep_ = new QDomDocument;

    context_menu_ = context_menu;

    QAction* cancel_action = parent_->lCMW()->cancelConstraintDeleteAction;

    separator_id_ = context_menu_->insertSeparator();
    cancel_action->addTo( context_menu_ );
    connect( cancel_action, SIGNAL( activated() ), SLOT( cancelOperation() )  );

    parent_->view()->
      setCursor( CursorFactory::instance().cursor( CursorFactory::DELETE_CONSTRAINT ) );
  }

  void ConstraintDeleteInput::mousePrepress ( QMouseEvent* /*me*/,
					     const SelectedNames& /*selected*/ )
  {}

  void ConstraintDeleteInput::mousePress ( QMouseEvent* /*me*/,
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

    GLuint g = (*f).second[1]; /// (*(*f).second->begin()).first;
    ConstrainedLine* line = dynamic_cast< ConstrainedLine* >( fv->geometry( g ) );

    if ( line != 0 )
      target_ = line;
  }

  void ConstraintDeleteInput::mouseDrag ( QMouseEvent* /*me*/,
					 const SelectedNames& /*selected*/ )
  {}

  bool ConstraintDeleteInput::mouseRelease ( QMouseEvent* /*me*/,
					     const SelectedNames& /*selected*/ )
  {
    if ( target_ == 0 )
      return false;

    QDomElement reconstraints = xml_rep_->createElement( lC::STR::RECONSTRAINTS );
    xml_rep_->appendChild( reconstraints );

    QDomElement old_constraints = xml_rep_->createElement(lC::STR::OLD_CONSTRAINTS);
    reconstraints.appendChild( old_constraints );

    QDomElement old_xml = xml_rep_->createElement( lC::STR::CONSTRAINED_LINE );
    old_xml.setAttribute( lC::STR::URL, target_->dbURL() );
    target_->constraint()->write( old_xml );
    old_constraints.appendChild( old_xml );

    // Making this contraint can, in some cases, cause the target
    // line's former reference to be reconstrained. This happens at
    // least in Rectangle in order to keep a width- or height-like
    // dimension. In undoing this operation, that cascade change must
    // be done first in order to avoid a circular reference.  How do
    // we know it's going to happen? I guess we have to look at it
    // before and after...
    ConstrainedLine* from_reference =
      dynamic_cast<ConstrainedLine*>( target_->reference() );

    if ( from_reference != 0 ) {
      from_ref_xml_ = xml_rep_->createElement( lC::STR::CONSTRAINED_LINE );
      from_ref_xml_.setAttribute( lC::STR::URL, from_reference->dbURL() );
      from_reference->constraint()->write( from_ref_xml_ );

      // If from_reference_'s modifiedConstraint signal is emitted,
      // then the old XML representation of its constraint is appended
      // to this reconstrain command (done in the SLOT).

      connect( from_reference, SIGNAL( modifiedConstraint() ),
	       SLOT( cascadeConstraints() ) );
    }

    // It is now possible to just create this contraint and the affected
    // figures will react to signals from the modified line.

    if ( target_->isVertical() )
      target_->setConstraint( new Offset( target_->o(), &Y_AXIS ) );
    else if ( target_->isHorizontal() )
      target_->setConstraint( new Offset( target_->o(), &X_AXIS ) );

    if ( from_reference != 0 )
      disconnect( from_reference, SIGNAL( modifiedConstraint() ),
		  this, SLOT( cascadeConstraints() ) );

    QDomElement new_constraints = xml_rep_->createElement( lC::STR::NEW_CONSTRAINTS );
    reconstraints.appendChild( new_constraints );

    QDomElement new_xml = xml_rep_->createElement( lC::STR::CONSTRAINED_LINE );
    new_xml.setAttribute( lC::STR::URL, target_->dbURL() );
    target_->constraint()->write( new_xml );
    new_constraints.appendChild( new_xml );

    CommandHistory::instance().
      addCommand( new ReconstrainCommand( "reconstrain line",
					  parent_->model(),
					  xml_rep_ ) );

    xml_rep_ = 0;

    QAction* cancel_action = parent_->lCMW()->cancelConstraintDeleteAction;
    cancel_action->disconnect();
    cancel_action->removeFrom( context_menu_ );
    context_menu_->removeItem( separator_id_ );

    parent_->view()->unsetCursor();

    parent_->figureComplete();

    return true;
  }

  void ConstraintDeleteInput::keyPress ( QKeyEvent* ke )
  {
    if ( ke->key() == Qt::Key_Escape )
      cancelOperation();
    else
      ke->ignore();
  }

  void ConstraintDeleteInput::cancelOperation ( void )
  {
    // Normally, the Command would take responsibility for this
    // object, but if the dimension creation is cancelled, it's never
    // passed along.
    delete xml_rep_;
    xml_rep_ = 0;

    QAction* cancel_action = parent_->lCMW()->cancelConstraintDeleteAction;

    cancel_action->disconnect();
    cancel_action->removeFrom( context_menu_ );
    context_menu_->removeItem( separator_id_ );

    parent_->cancelOperation();
  }

  void ConstraintDeleteInput::cascadeConstraints ( void )
  {
    // The primary reconstraint target is buried deep in this representation.
    // This change has to go before it so it can be undone properly.
    xml_rep_->firstChild().firstChild().insertBefore( from_ref_xml_,
			      xml_rep_->firstChild().firstChild().firstChild() );
  }
} // End of Space2D namespace
