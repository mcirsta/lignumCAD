/*
 * constrainthistory.cpp
 *
 * ConstraintHistory classes: Remember the constraints which were changed.
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
#include <qdom.h>

#include "constants.h"
#include "constrainthistory.h"

ConstraintHistory ConstraintHistory::constraint_history_;

ConstraintHistory& ConstraintHistory::instance ( void )
{
  return constraint_history_;
}

ConstraintHistory::ConstraintHistory ( void )
  : reconstraints_( 0 )
{
}

void ConstraintHistory::init ( void )
{
  if ( reconstraints_ != 0 )
    delete reconstraints_;

  reconstraints_ = new QDomDocument;

  QDomElement constraints = reconstraints_->createElement( lC::STR::CONSTRAINTS );
  QDomElement unconstraints = reconstraints_->createElement( lC::STR::UNCONSTRAINTS);
  reconstraints_->appendChild( constraints );
  reconstraints_->appendChild( unconstraints );
}

ConstraintHistory::~ConstraintHistory ( void )
{
  delete reconstraints_;
}

void ConstraintHistory::appendContraint ( const QDomElement& xml_rep )
{
  if ( reconstraints_ == 0 ) return;

  reconstraints_->firstChild().appendChild( xml_rep );
}

void ConstraintHistory::appendUnconstraint ( const QDomElement& xml_rep )
{
  if ( reconstraints_ == 0 ) return;
#if 1
  // Prepend so that these are executed in reverse order (IS IT SO?...)
  reconstraints_->firstChild().nextSibling().
    insertBefore( xml_rep, reconstraints_->firstChild().nextSibling().firstChild() );
#else
  reconstraints_->firstChild().nextSibling().appendChild( xml_rep );
#endif
}

void ConstraintHistory::takeConstraints ( QDomDocument* xml_doc )
{
  if ( reconstraints_ == 0 ) return;

  QDomNode node;

  node = xml_doc->importNode( reconstraints_->firstChild(), true );
  xml_doc->firstChild().appendChild( node );

  node = xml_doc->importNode( reconstraints_->firstChild().nextSibling(), true );
  xml_doc->firstChild().appendChild( node );

  // Reset for next time (but now must call init() manually when you are
  // ready to collect reconstraints).

  delete reconstraints_;

  reconstraints_ = 0;
}
