/*
 * annotation.cpp
 *
 * Annotation classes
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
#include "page.h"
#include "annotation.h"

namespace Space2D {
  // This is the next unique annotation name number to dispense.
  uint Annotation::unique_index_ = 0;

  /*
   * Dispense a new, globally unique name.
   */
  QString Annotation::newName ( void )
  {
    return tr( "Annotation[%1]" ).arg( ++unique_index_ );
  }

  /*
   * Constructor for interactive creation.
   */
  Annotation::Annotation ( uint id, const QString& name, Page* parent )
    : Figure( id, name, lC::STR::ANNOTATION, parent )
  {}

  /*
   * Constructor for creation from a file.
   */
  Annotation::Annotation ( uint id, const QDomElement& xml_rep, Page* parent )
    : Figure( id, QString::null, lC::STR::ANNOTATION, parent )
  {
    setName( xml_rep.attribute( lC::STR::NAME ) );

    QRegExp regexp( tr( "Annotation\\[([0-9]+)\\]" ) );
    int position = regexp.search( name() );
    if ( position >= 0 ) {
      unique_index_ = QMAX( regexp.cap(1).toUInt(), unique_index_ );
    }

    origin_[X] = xml_rep.attribute( lC::STR::X ).toDouble();
    origin_[Y] = xml_rep.attribute( lC::STR::Y ).toDouble();

    text_ = xml_rep.text();

    setComplete( true );
    setValid( true );
  }

  /*
   * Constructor for a pasted annotation.
   */
  Annotation::Annotation ( uint id, const QString& name, const QDomElement& xml_rep,
			   Page* parent )
    : Figure( id, name, lC::STR::ANNOTATION, parent )
  {
    QRegExp regexp( tr( "Annotation\\[([0-9]+)\\]" ) );
    int position = regexp.search( name );
    if ( position >= 0 ) {
      unique_index_ = QMAX( regexp.cap(1).toUInt(), unique_index_ );
    }

    origin_[X] = xml_rep.attribute( lC::STR::X ).toDouble();
    origin_[Y] = xml_rep.attribute( lC::STR::Y ).toDouble();

    text_ = xml_rep.text();

    setComplete( true );
    setValid( true );
  }

  /*
   * Rename the annotation.
   */
  void Annotation::setName ( const QString name )
  {
    ModelItem::setName( name );
  }

  /*
   * Create an XML representation of the annotation.
   */
  void Annotation::write ( QDomElement& xml_rep ) const
  {
    if ( !isValid() ) return;

    QDomDocument document = xml_rep.ownerDocument();
    QDomElement annotation_element = document.createElement( lC::STR::ANNOTATION );

    annotation_element.setAttribute( lC::STR::ID, id() );
    annotation_element.setAttribute( lC::STR::NAME, name() );

    annotation_element.setAttribute( lC::STR::X, lC::format( origin_[X] ) );
    annotation_element.setAttribute( lC::STR::Y, lC::format( origin_[Y] ) );

    QDomText text_node = document.createTextNode( text_ );
    annotation_element.appendChild( text_node );
    xml_rep.appendChild( annotation_element );
  }

  /*
   * Append a (un)simplified representation of the annotation to the
   * argument XML document.
   */
  void Annotation::copyWrite ( QDomElement& xml_rep ) const
  {
    // Prety simple already.
    write( xml_rep );
  }

  /*
   * Find a point on the annotation which is close to the given point.
   */
  Vertex Annotation::closestVertex( Point /*p*/, Curve* curve ) const
  {
    // The origin is the only point which Annotation knows about.
    Vertex vertex( curve );
    vertex.p_ = origin_;
    return vertex;
  }

  /*
   * Compute the segment of the curve used by the annotation (but,
   * annotations don't have any curves, so this should never be
   * called).
   */
  Segment Annotation::closestVertices ( Curve* curve ) const
  {
    // Return the origin, anyway.
    Segment segment( curve, true, true );
    segment.from_ = origin_;
    segment.to_ = origin_;
    return segment;
  }

  /*
   * Change the text of the annotation.
   */
  void Annotation::setText ( QString text )
  {
    text_ = text;

    emit textChanged();
  }

  /*
   * Change the position of the annotation.
   */
  void Annotation::setOrigin ( const Point& origin )
  {
    origin_ = origin;

    emit moved();
  }

  /*
   * Start constructing an annotation interactively.
   */
  void Annotation::initialize ( const Point& origin )
  {
    origin_ = origin;

    setValid( true );

    emit initialized();
  }

  /*
   * Complete interactive construction.
   */
  void Annotation::finish ( void )
  {
    setComplete( true );
  }

  /*
   * Move the annotation to a new position.
   */
  void Annotation::move ( const Vector& delta )
  {
    origin_ += delta;

    emit moved();
  }
} // End of Space2D namespace
