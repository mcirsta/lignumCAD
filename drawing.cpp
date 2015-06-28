/*
 * drawing.cpp
 *
 * Implementation of Drawing class
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
#include "drawing.h"

uint Drawing::unique_index_ = 0;

QString Drawing::newName ( void )
{
  return tr( "Drawing[%1]" ).arg( ++unique_index_ );
}

Drawing::Drawing ( uint id, const QString name, Model* parent )
  : Page( id, name, lC::STR::DRAWING, parent )
{
}

Drawing::Drawing ( uint id, const QDomElement& xml_rep, Model* parent )
  : Page( id, QString::null, lC::STR::DRAWING, parent )
{
  setName( xml_rep.attribute( lC::STR::NAME ) );

  QRegExp regexp( tr( "Drawing\\[([0-9]+)\\]" ) );
  int position = regexp.search( name() );
  if ( position >= 0 ) {
    Drawing::unique_index_ = QMAX( regexp.cap(1).toUInt(),
				   Drawing::unique_index_ );
  }
}

void Drawing::write ( QDomElement& xml_rep ) const
{
  QDomDocument document = xml_rep.ownerDocument();
  QDomElement drawing_element = document.createElement( lC::STR::DRAWING );
  drawing_element.setAttribute( lC::STR::ID, id() );
  drawing_element.setAttribute( lC::STR::NAME, name() );

  xml_rep.appendChild( drawing_element );
}
