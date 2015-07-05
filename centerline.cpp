/*
 * centerline.cpp
 *
 * Centerline classes
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
#include "page.h"
#include "line.h"
#include "centerline.h"

namespace Space2D {
  uint Centerline::unique_index_ = 0;

  QString Centerline::newName ( void )
  {
    return tr( "Centerline[%1]" ).arg( ++unique_index_ );
  }

  Centerline::Centerline ( uint id, const QString& name, Page* parent )
    : Figure( id, name, lC::STR::CENTERLINE, parent),
      line_( new ConstrainedLine( 0, name, this, 0 ) )
  {
    connect( line_, SIGNAL( modifiedPosition() ), SLOT( lineModified() ) );
  }

  Centerline::Centerline ( uint id, const QDomElement& xml_rep, Page* parent )
    : Figure( id, QString::null, lC::STR::CENTERLINE, parent ), line_( 0 )
  {
    QDomNode n = xml_rep.firstChild();

    while ( !n.isNull() ) {
      QDomElement e = n.toElement();

      if ( !e.isNull() )
	line_ = new ConstrainedLine( 0, e, this );

      n = n.nextSibling();
    }

    setName( xml_rep.attribute( lC::STR::NAME ) );

    QRegExp regexp( tr( "Centerline\\[([0-9]+)\\]" ) );

    int position = regexp.indexIn( name() );
    if ( position >= 0 ) {
      Centerline::unique_index_ = qMax( regexp.cap(1).toUInt(),
				       Centerline::unique_index_ );
    }

    connect( line_, SIGNAL( modifiedPosition() ), SLOT( lineModified() ) );

    setComplete( true );
    setValid( true );
  }

  Centerline::Centerline ( uint id, const QString& name, const QDomElement& xml_rep,
			   Page* parent )
    : Figure( id, name, lC::STR::CENTERLINE, parent),
      line_( new ConstrainedLine( 0, name, this, 0 ) )
  {
    QRegExp regexp( tr( "Centerline\\[([0-9]+)\\]" ) );

    int position = regexp.indexIn( name );
    if ( position >= 0 ) {
      Centerline::unique_index_ = qMax( regexp.cap(1).toUInt(),
					Centerline::unique_index_ );
    }

    Point o( xml_rep.attribute( lC::STR::X0 ).toDouble(),
	     xml_rep.attribute( lC::STR::Y0 ).toDouble() );
    Vector e( xml_rep.attribute( lC::STR::X1 ).toDouble(),
	      xml_rep.attribute( lC::STR::Y1 ).toDouble() );

    if ( fabs( fabs( e * X_AXIS.e() ) - 1. ) < lC::EPSILON ) {
      line_->setConstraint( new Offset( o, &X_AXIS ) );
    }
    else if ( fabs( fabs( e * Y_AXIS.e() ) - 1. ) < lC::EPSILON ) {
      line_->setConstraint( new Offset( o, &Y_AXIS ) );
    }

    connect( line_, SIGNAL( modifiedPosition() ), SLOT( lineModified() ) );

    setComplete( true );
    setValid( true );
  }

  Centerline::~Centerline ( void )
  {
    if ( line_ != 0 && line_->constraint() != 0 ) {
      disconnect( line_->reference(), SIGNAL( destroyedLine() ),
		  line_, SLOT( referenceDestroyed() ) );
      delete line_;
    }
  }

  void Centerline::lineModified ( void )
  {
    setValid( true );
    emit modified();
  }

  void Centerline::move ( const Vector& delta )
  {
    line_->setPosition( line_->o() + delta );
  }

  void Centerline::setOffset ( double offset )
  {
    if ( line_->isAdjustable() ) {
      line_->setOffset( offset );
      emit modified();
    }
  }

  void Centerline::setName ( const QString& name )
  {
    ModelItem::setName( name );

    if ( line_ )
      line_->setName( name );
  }

  void Centerline::lineRenamed ( QString /*name*/ )
  {
    emit nameChanged( name() );
  }

  void Centerline::write ( QDomElement& xml_rep ) const
  {
    if ( !isValid() ) return;

    QDomDocument document = xml_rep.ownerDocument();
    QDomElement centerline_element = document.createElement( lC::STR::CENTERLINE );
    centerline_element.setAttribute( lC::STR::ID, id() );
    centerline_element.setAttribute( lC::STR::NAME, name() );
    if ( line_ != 0 ) {
      line_->write( centerline_element );
    }
    xml_rep.appendChild( centerline_element );
  }
  /*
   * Append a simplified representation of the centerline to the
   * argument XML document.
   */
  void Centerline::copyWrite ( QDomElement& xml_rep ) const
  {
    QDomDocument document = xml_rep.ownerDocument();
    QDomElement centerline_element = document.createElement( lC::STR::CENTERLINE );
    centerline_element.setAttribute( lC::STR::NAME, name() );
    if ( line_ != 0 ) {
      centerline_element.setAttribute( lC::STR::X0, lC::format( line_->o()[X] ) );
      centerline_element.setAttribute( lC::STR::Y0, lC::format( line_->o()[Y] ) );
      centerline_element.setAttribute( lC::STR::X1, lC::format( line_->e()[X] ) );
      centerline_element.setAttribute( lC::STR::Y1, lC::format( line_->e()[Y] ) );
    }
    xml_rep.appendChild( centerline_element );
  }

  Vertex Centerline::closestVertex( Point p, Curve* curve ) const
  {
    // There is no closest vertex in a centerline. Some more thought is
    // required here...
    Vertex v( curve );

    if ( line_->isHorizontal() )
      v.curve2_ = &Y_AXIS;
    else
      v.curve2_ = &X_AXIS;

    v.p_ = p;
    return v;
  }

  // Since the reference line doesn't really have vertices, we return
  // the origin and some other point on the line and indicate that
  // they are not really valid vertices. The caller can decide what to
  // do with them. However, we make the general assumption that
  // to - from is a unit vector.
  
  Segment Centerline::closestVertices ( Curve* curve ) const
  {
    Segment segment( curve );
    segment.from_ = line_->o();
    segment.to_ = line_->o() + line_->e();
    return segment;
  }

  ModelItem* Centerline::lookup ( QStringList& path_components ) const
  {
    // The front path component is the name of a figure with ".type" appended
    // to it. So, use a regular expression to split out the trailing type.
    int dot_pos = path_components.front().lastIndexOf( '.' );
    QString name = path_components.front().left( dot_pos );
    QString type = path_components.front().right( path_components.front().length()
						  - dot_pos - 1 );

    if ( name == line_->name() && type == line_->type() )
      return line_;

    return 0;
  }
} // End of Space2D namespace
