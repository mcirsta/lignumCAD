/*
 * sketch.cpp
 *
 * Implementation of Sketch class
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
#include <qregexp.h>
#include <qlistview.h>

#include "constants.h"
#include "dburl.h"
#include "model.h"
#include "rectangle.h"
#include "referenceline.h"
#include "centerline.h"
#include "annotation.h"
#include "sketch.h"

using namespace Space2D;

uint Sketch::unique_index_ = 0;

QString Sketch::newName ( void )
{
  return tr( "Sketch[%1]" ).arg( ++unique_index_ );
}

Sketch::Sketch ( uint id, const QString& name, Model* parent )
  : Page( id, name, lC::STR::SKETCH, parent )
{
}

Sketch::Sketch ( uint id, const QDomElement& xml_rep, Model* parent )
  : Page( id, QString::null, lC::STR::SKETCH, parent )
{
  setName( xml_rep.attribute( lC::STR::NAME ) );

  QRegExp regexp( tr( "Sketch\\[([0-9]+)\\]" ) );
  int position = regexp.search( name() );
  if ( position >= 0 ) {
    Sketch::unique_index_ = QMAX( regexp.cap(1).toUInt(), Sketch::unique_index_ );
  }

  QDomNode n = xml_rep.firstChild();

  while ( !n.isNull() ) {
    QDomElement e = n.toElement();
    if ( !e.isNull() ) {
      if ( e.tagName() == lC::STR::REFERENCE_LINE ) {
	uint id = e.attribute( lC::STR::ID ).toUInt();
	new ReferenceLine ( id, e, this );
      }
      else if ( e.tagName() == lC::STR::CENTERLINE ) {
	uint id = e.attribute( lC::STR::ID ).toUInt();
	new Centerline ( id, e, this );
      }
      else if ( e.tagName() == lC::STR::RECTANGLE ) {
	uint id = e.attribute( lC::STR::ID ).toUInt();
	new Rectangle ( id, e, this );
      }
      else if ( e.tagName() == lC::STR::ANNOTATION ) {
	uint id = e.attribute( lC::STR::ID ).toUInt();
	new Annotation ( id, e, this );
      }
    }
    n = n.nextSibling();
  }
}

void Sketch::write ( QDomElement& xml_rep ) const
{
  QDomDocument document = xml_rep.ownerDocument();
  QDomElement sketch_element = document.createElement( lC::STR::SKETCH );
  sketch_element.setAttribute( lC::STR::ID, id() );
  sketch_element.setAttribute( lC::STR::NAME, name() );

  QMap<uint,Figure*>::const_iterator figure = figures_.begin();

  for ( ; figure != figures_.end(); ++figure )
    figure.data()->write( sketch_element );

  xml_rep.appendChild( sketch_element );
}
