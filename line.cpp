/*
 * line.cpp
 *
 * Line class: straight lines
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
#include <qapplication.h>

#include "constants.h"
#include "figure.h"
#include "dburl.h"
#include "model.h"
#include "line.h"

#include <QString>

// 2D lines

namespace Space2D {

  Axis X_AXIS( 0, lC::STR::PATH_PATTERN.arg( lC::STR::SPACE2D).arg(lC::STR::X_AXIS),
	       Point(0,0), Vector(1,0) );
  Axis Y_AXIS( 0, lC::STR::PATH_PATTERN.arg( lC::STR::SPACE2D).arg(lC::STR::Y_AXIS),
	       Point(0,0), Vector(0,1) );

  Point Line::intersect ( const Curve* c ) const
  {
    // Unfortunately, you do have to know the exact type of the curve
    // in order to compute the intersection (unless we want to use splines
    // for everything)
    const Line* b = dynamic_cast< const Line* >( c );
    if ( b != 0 ) {
      // See the derivation of this formula in the Technical Reference.
      double sint2 = 1 - ( e_ * b->e() ) * ( e_ * b->e() );
      double l = ( e_ - b->e() * ( e_ * b->e() ) ) * ( b->o() - o_ ) / sint2;
      return o_ + e_ * l;
    }
    return Point();
  }

  bool Line::isHorizontal ( void ) const
  {
    return fabs( fabs( e_ * Vector(1,0) ) - 1. ) < lC::EPSILON;
  }

  bool Line::isVertical ( void ) const
  {
    return fabs( fabs( e_ * Vector(0,1) ) - 1. ) < lC::EPSILON;
  }

  Vector Line::normal ( void ) const
  {
    // We want to insure that offsets are usually positive, particularly
    // when the line is one of the coordinate axes.
    if ( e_[X] > 0 )
      return Vector( -e_[Y], e_[X] );
    else if ( e_[X] < 0 )
      return Vector( e_[Y], -e_[X] );
    else if ( e_[Y] > 0 )
      return Vector( e_[Y], -e_[X] );

    return Vector( -e_[Y], e_[X] );
  }

  ostream& operator<< ( ostream& o, Line* line )
  {
    return line->edit( o );
  }

  ostream& Axis::edit ( ostream& o ) const
  {
    return o << "axis: " << name().toStdString() << " o:" << o_ << ", e: " << e_ << endl;
  }

  void Axis::write ( QDomElement& /*xml_rep*/ ) const
  {
  }

  ConstrainedLine::ConstrainedLine ( uint id, const QString& name, Figure* parent,
				     Constraint* constraint )
    : Line( id, name, parent ), constraint_( constraint )
  {
    if ( constraint_ != 0 ) {

      o_ = constraint_->o();
      e_ = constraint_->e();

      connect( reference(), SIGNAL( modifiedPosition() ),
	       SLOT( referenceModified() ) );
      connect( reference(), SIGNAL( nameChanged( const QString& ) ),
	       SIGNAL( nameChanged( const QString& ) ) );
      connect( reference(), SIGNAL( destroyedLine() ),
	       SLOT( referenceDestroyed() ) );
    }
  }

  ConstrainedLine::ConstrainedLine ( uint id, const QDomElement& xml_rep,
				     Figure* parent )
    : Line( id, QString::null, parent ), constraint_( 0 )
  {
    setName( xml_rep.attribute( lC::STR::NAME ) );

    QDomNode n = xml_rep.firstChild();

    while ( !n.isNull() ) {
      QDomElement e = n.toElement();
      if ( !e.isNull() ) {
	if ( e.tagName() == lC::STR::COINCIDENT_CONSTRAINT ) {
	  constraint_ = new Coincident( e, this );
	}
	else if ( e.tagName() == lC::STR::OFFSET_CONSTRAINT ) {
	  constraint_ = new Offset( e, this );
	}
	else if ( e.tagName() == lC::STR::CENTERED_CONSTRAINT ) {
	  constraint_ = new Centered( e, this );
	}
      }
      n = n.nextSibling();
    }

    o_ = constraint_->o();
    e_ = constraint_->e();

    if ( reference() != 0 ) {
      connect( reference(), SIGNAL( modifiedPosition() ),
	       SLOT( referenceModified() ) );
      connect( reference(), SIGNAL( nameChanged( const QString& ) ),
	       SIGNAL( nameChanged( const QString& ) ) );
      connect( reference(), SIGNAL( destroyedLine() ),
	       SLOT( referenceDestroyed() ) );
    }
  }

  ConstrainedLine::~ConstrainedLine ( void )
  {
    emit destroyedLine();
    if ( constraint_ ) delete constraint_;
  }

  void ConstrainedLine::setConstraint ( Constraint* constraint )
  {
    // Can we do this with impunity now? No. Remember to remove the
    // old connection.
    if ( constraint_ != 0 ) {
      if ( reference() != 0 ) {
	disconnect( reference(), SIGNAL( modifiedPosition() ),
		    this, SLOT( referenceModified() ) );
	disconnect( reference(), SIGNAL( nameChanged( const QString& ) ),
		    this, SIGNAL( nameChanged( const QString& ) ) );
	disconnect( reference(), SIGNAL( destroyedLine() ),
		    this, SLOT( referenceDestroyed() ) );
      }
      delete constraint_;
    }

    constraint_ = constraint;

    o_ = constraint_->o();
    e_ = constraint_->e();

    connect( reference(), SIGNAL( modifiedPosition() ),
	     SLOT( referenceModified() ) );
    connect( reference(), SIGNAL( nameChanged( const QString& ) ),
	     SIGNAL( nameChanged( const QString& ) ) );
    connect( reference(), SIGNAL( destroyedLine() ),
	     SLOT( referenceDestroyed() ) );

    emit modifiedConstraint();
    emit modifiedPosition();
  }

  void ConstrainedLine::setConstraint ( const QDomElement& e )
  {
    Constraint* constraint;

    if ( e.tagName() == lC::STR::COINCIDENT_CONSTRAINT ) {
      constraint = new Coincident( e, this );
    }
    else if ( e.tagName() == lC::STR::OFFSET_CONSTRAINT ) {
      constraint = new Offset( e, this );
    }
    else if ( e.tagName() == lC::STR::CENTERED_CONSTRAINT ) {
      constraint = new Centered( e, this );
    }

    setConstraint( constraint );
  }

  void ConstrainedLine::resolved ( const QString& /*db_path*/, ModelItem* item )
  {
    Line* _reference = dynamic_cast< Line* >( item );

    if ( _reference != 0 ) {
      constraint_->resolvedReference( _reference );

      o_ = constraint_->o();
      e_ = constraint_->e();

      connect( reference(), SIGNAL( modifiedPosition() ),
	       SLOT( referenceModified() ) );
      connect( reference(), SIGNAL( nameChanged( const QString& ) ),
	       SIGNAL( nameChanged( const QString& ) ) );
      connect( reference(), SIGNAL( destroyedLine() ),
	       SLOT( referenceDestroyed() ) );


      emit modifiedPosition();
    }
  }

  void ConstrainedLine::setOffset ( double offset )
  {
    o_ = constraint_->setOffset( offset );

    emit modifiedPosition();
  }

  void ConstrainedLine::setReference ( const Point& position, Line* _reference )
  {
    if ( reference() != 0 ) {
      disconnect( reference(), SIGNAL( modifiedPosition() ),
		  this, SLOT( referenceModified() ) );
      disconnect( reference(), SIGNAL( nameChanged( const QString&) ),
		  this, SIGNAL( nameChanged( const QString& ) ) );
      disconnect( reference(), SIGNAL( destroyedLine() ),
		  this, SLOT( referenceDestroyed() ) );
    }

    constraint_->setReference( position, _reference );

    o_ = constraint_->o();
    e_ = constraint_->e();

    connect( reference(), SIGNAL( modifiedPosition() ),
	     this, SLOT( referenceModified() ) );
    connect( reference(), SIGNAL( nameChanged( const QString& ) ),
	     this, SIGNAL( nameChanged( const QString& ) ) );
    connect( reference(), SIGNAL( destroyedLine() ),
	     SLOT( referenceDestroyed() ) );


    emit modifiedPosition();
  }

  void ConstrainedLine::setPosition ( const Point& position )
  {
    o_ = constraint_->setPosition( position );

    emit modifiedPosition();
  }

  QString ConstrainedLine::detail ( void ) const
  {
    if ( constraint_ != 0 )
      return constraint_->detail();
    else
      return tr( "constraint undefined" );
  }

  // This is an (optional) annotation of the dimension string.
  QString ConstrainedLine::note ( void ) const
  {
    if ( constraint_ != 0 )
      return constraint_->note();
    else
      return QString::null;
  }

  void ConstrainedLine::write ( QDomElement& xml_rep ) const
  {
    QDomDocument document = xml_rep.ownerDocument();
    QDomElement line_element = document.createElement( lC::STR::CONSTRAINED_LINE );
    line_element.setAttribute( lC::STR::NAME, name() );
    // Add the information about the constraint
    constraint_->write( line_element );
    xml_rep.appendChild( line_element );
  }

  void ConstrainedLine::referenceModified ( void )
  {
    constraint_->referenceModified();

    o_ = constraint_->o();
    e_ = constraint_->e();

    emit modifiedPosition();
  }

  void ConstrainedLine::referenceDestroyed ( void )
  {
    QDomDocument* xml_doc = ConstraintHistory::instance().history();
    if ( xml_doc != 0 ) {
      QDomElement old_xml_rep = xml_doc->createElement( lC::STR::CONSTRAINED_LINE );
      old_xml_rep.setAttribute( lC::STR::URL, dbURL().toString() );
      constraint_->write( old_xml_rep );
      ConstraintHistory::instance().appendUnconstraint( old_xml_rep );
    }

    if ( isVertical() )
      constraint_ = new Offset( o_, &Y_AXIS );
    else if ( isHorizontal() )
      constraint_ = new Offset( o_, &X_AXIS );

    o_ = constraint_->o();
    e_ = constraint_->e();

    if ( xml_doc != 0 ) {
      QDomElement new_xml_rep = xml_doc->createElement( lC::STR::CONSTRAINED_LINE );
      new_xml_rep.setAttribute( lC::STR::URL, dbURL().toString() );
      constraint_->write( new_xml_rep );
      ConstraintHistory::instance().appendContraint( new_xml_rep );
    }

    connect( reference(), SIGNAL( modifiedPosition() ),
	     SLOT( referenceModified() ) );
    connect( reference(), SIGNAL( nameChanged( const QString& ) ),
	     SIGNAL( nameChanged( const QString& ) ) );
    connect( reference(), SIGNAL( destroyedLine() ),
	     SLOT( referenceDestroyed() ) );

    emit modifiedConstraint();
    emit modifiedPosition();
  }

  Coincident::Coincident ( Line* reference )
    : from_( reference )
  {}

  Coincident::Coincident ( const QDomElement& xml_rep, ConstrainedLine* parent)
  {
    DBURL db_url = xml_rep.attribute( lC::STR::FROM );

    from_ = dynamic_cast< Line* >( parent->model()->lookup( db_url ) );

    if ( from_ == 0 )
      parent->model()->addDelayedResolution( parent, db_url.toString() );
  }

  Point Coincident::o ( void ) const
  {
    if ( from_ != 0 )
      return from_->o();
    else
      return Point();
  }

  Vector Coincident::e ( void ) const
  {
    if ( from_ != 0 )
      return from_->e();
    else
      return Vector();
  }

  void Coincident::setReference ( const Point& /*position*/, Line* reference )
  {
    from_ = reference;
  }

  void Coincident::resolvedReference ( Line* reference )
  {
    from_ = reference;
  }

  bool Coincident::dependsOn ( const Line* line ) const
  {
    if ( line == from_ )
      return true;
    else
      return from_->dependsOn( line );
  }

  ostream& Coincident::edit ( ostream& o ) const
  {
    return o << "coincident line";
  }

  QString Coincident::detail ( void ) const
  {
    if ( from_ != 0 )
      return qApp->translate( "Space2D::Constraint",
			      "coincident with %1" ).
	arg( lC::formatName( from_->path() ) );
    else
      return qApp->translate( "Space2D::Constraint",
			      "invalid coincident constraint (reference == 0)" );
  }

  void Coincident::write ( QDomElement& xml_rep ) const
  {
    QDomDocument document = xml_rep.ownerDocument();
    QDomElement coincident_element = document.createElement( lC::STR::COINCIDENT_CONSTRAINT );
    coincident_element.setAttribute( lC::STR::FROM, from_->dbURL().toString() );
    xml_rep.appendChild( coincident_element );
  }

  Offset::Offset ( const Point& position, Line* reference )
    : from_( reference )
  {
    if ( from_ != 0 )
      //      normal_ = Vector( -from_->e().v_[Y], from_->e().v_[X] );
      normal_ = from_->normal();

    setPosition( position );
  }

  Offset::Offset ( Line* reference )
    : offset_( 0 ), from_( reference )
  {
    if ( from_ != 0 ) {
      //      normal_ = Vector( -from_->e().v_[Y], from_->e().v_[X] );
      normal_ = from_->normal();
    }
  }

  Offset::Offset ( const QDomElement& xml_rep, ConstrainedLine* parent )
  {
    offset_ = xml_rep.attribute( lC::STR::OFFSET ).toDouble();

    DBURL db_url = xml_rep.attribute( lC::STR::FROM );

    from_ = dynamic_cast< Line* >( parent->model()->lookup( db_url ) );

    if ( from_ != 0 )
      //      normal_ = Vector( -from_->e().v_[Y], from_->e().v_[X] );
      normal_ = from_->normal();
    else
      parent->model()->addDelayedResolution( parent, db_url.toString() );
  }

  Point Offset::o ( void ) const
  {
    if ( from_ != 0 )
      return from_->o() + offset_ * normal_;
    else
      return Point();
  }

  Vector Offset::e ( void ) const
  {
    if ( from_ != 0 )
      return from_->e();
    else
      return Vector();
  }

  bool Offset::dependsOn ( const Line* line ) const
  {
    if ( line == from_ )
      return true;
    else
      return from_->dependsOn( line );
  }

  ostream& Offset::edit ( ostream& o ) const
  {
    return o << "offset line";
  }

  QString Offset::detail ( void ) const
  {
    if ( from_ != 0 )
      return qApp->translate( "Space2D::Constraint", "offset %1 from %2" )
	.arg( offset_).arg( lC::formatName( from_->path() ) );
    else
      return qApp->translate( "Space2D::Constraint",
			      "invalid offset constraint (reference == 0)" );
  }

  Point Offset::setPosition ( const Point& position )
  {
    if ( from_ != 0 ) {
      offset_ = ( position - from_->o() ) * normal_;

      return from_->o() + offset_ * normal_;
    }
    return Point();
  }

  Point Offset::setOffset ( double offset )
  {
    offset_ = offset;

    return from_->o() + offset_ * normal_;
  }

  void Offset::setReference ( const Point& position, Line* reference )
  {
    from_ = reference;

    if ( from_ != 0 ) {
      //      normal_ = Vector( -from_->e().v_[Y], from_->e().v_[X] );
      normal_ = from_->normal();

      setPosition( position );
    }
  }

  void Offset::resolvedReference ( Line* reference )
  {
    from_ = reference;

    //    normal_ = Vector( -from_->e().v_[Y], from_->e().v_[X] );
    normal_ = from_->normal();
  }

  void Offset::referenceModified ( void )
  {
    //    normal_ = Vector( -from_->e().v_[Y], from_->e().v_[X] );
    normal_ = from_->normal();
  }

  void Offset::write ( QDomElement& xml_rep ) const
  {
    QDomDocument document = xml_rep.ownerDocument();
    QDomElement offset_element = document.createElement( lC::STR::OFFSET_CONSTRAINT );
    offset_element.setAttribute( lC::STR::OFFSET, lC::format( offset_ ) );
    offset_element.setAttribute( lC::STR::FROM, from_->dbURL().toString() );
    xml_rep.appendChild( offset_element );
  }

  QString Centered::centerline_symbol_( QChar( 0x2104 ) );

  Centered::Centered ( const Point& /*position*/, Line* reference )
  {
    from_ = dynamic_cast< ConstrainedLine* >( reference );
    if ( from_ != 0 ) {
      //      normal_ = Vector( -from_->e().v_[Y], from_->e().v_[X] );
      normal_ = from_->normal();
      offset_ = -2. * from_->offset();
    }
  }

  Centered::Centered ( Line* reference )
  {
    from_ = dynamic_cast< ConstrainedLine* >( reference );
    if ( from_ != 0 ) {
      //      normal_ = Vector( -from_->e().v_[Y], from_->e().v_[X] );
      normal_ = from_->normal();
      offset_ = -2. * from_->offset();
    }
  }

  Centered::Centered ( const QDomElement& xml_rep, ConstrainedLine* parent )
  {
    offset_ = xml_rep.attribute( lC::STR::OFFSET ).toDouble();

    DBURL db_url = xml_rep.attribute( lC::STR::FROM );

    from_ = dynamic_cast< ConstrainedLine* >( parent->model()->lookup( db_url ) );

    if ( from_ != 0 ) {
      //      normal_ = Vector( -from_->e().v_[Y], from_->e().v_[X] );
      normal_ = from_->normal();
      offset_ = -2. * from_->offset();
    }
    else
      parent->model()->addDelayedResolution( parent, db_url.toString() );
  }

  Point Centered::o ( void ) const
  {
    if ( from_ != 0 )
      return from_->o() + offset_ * normal_;
    else
      return Point();
  }

  Vector Centered::e ( void ) const
  {
    if ( from_ != 0 )
      return from_->e();
    else
      return Vector();
  }

  bool Centered::dependsOn ( const Line* line ) const
  {
    if ( line == from_ )
      return true;
    else
      return from_->dependsOn( line );
  }

  ostream& Centered::edit ( ostream& o ) const
  {
    return o << "centered line";
  }

  QString Centered::detail ( void ) const
  {
    if ( from_ != 0 ) {
      if ( from_->reference() != 0 )
	return qApp->translate( "Space2D::Constraint",
				"centered about %1, offset %2 from %3" )
	  .arg( lC::formatName( from_->reference()->path() ) ).
	  arg( offset_).arg( lC::formatName( from_->path() ) );
      else
	return qApp->translate( "Space2D::Constraint",
			"invalid center constraint (center reference == 0)");
    }
    else
      return qApp->translate( "Space2D::Constraint",
			      "invalid center constraint (reference == 0)" );
  }

  QString Centered::note ( void ) const
  {
    return centerline_symbol_;
  }

  Point Centered::setPosition ( const Point& position )
  {
    if ( from_ != 0 ) {
      offset_ = ( position - from_->o() ) * normal_;
      offset_ = -2. * from_->offset();

      return from_->o() + offset_ * normal_;
    }
    return Point();
  }

  Point Centered::setOffset ( double offset )
  {
    offset_ = offset;

    from_->setOffset( offset_ / 2. );

    return from_->o() + offset_ * normal_;
  }

  void Centered::setReference ( const Point& /*position*/, Line* reference )
  {
    from_ = dynamic_cast< ConstrainedLine* >( reference );

    if ( from_ != 0 ) {
      //      normal_ = Vector( -from_->e().v_[Y], from_->e().v_[X] );
      normal_ = from_->normal();
      offset_ = -2. * from_->offset();
    }
  }

  void Centered::resolvedReference ( Line* reference )
  {
    from_ = dynamic_cast< ConstrainedLine* >( reference );
    if ( from_ != 0 ) {
      //      normal_ = Vector( -from_->e().v_[Y], from_->e().v_[X] );
      normal_ = from_->normal();
      offset_ = -2. * from_->offset();
    }
  }

  void Centered::referenceModified ( void )
  {
    if ( from_ != 0 ) {
      //      normal_ = Vector( -from_->e().v_[Y], from_->e().v_[X] );
      normal_ = from_->normal();
      offset_ = -2. * from_->offset();
    }
  }

  void Centered::write ( QDomElement& xml_rep ) const
  {
    QDomDocument document = xml_rep.ownerDocument();
    QDomElement offset_element = document.createElement( lC::STR::CENTERED_CONSTRAINT );
    offset_element.setAttribute( lC::STR::OFFSET, lC::format( offset_ ) );
    offset_element.setAttribute( lC::STR::FROM, from_->dbURL().toString() );
    xml_rep.appendChild( offset_element );
  }
} // End of Space2D namespace

namespace Space3D {

  Point Line::intersect ( const Curve* c ) const
  {
    // Unfortunately, you do have to know the exact type of the curve
    // in order to compute the intersection (unless we want to use splines
    // for everything)
    const Line* b = dynamic_cast< const Line* >( c );
    if ( b != 0 ) {
      // See the derivation of this formula in the Technical Reference.
      double sint2 = 1 - ( e_ * b->e() ) * ( e_ * b->e() );
      double l = ( e_ - b->e() * ( e_ * b->e() ) ) * ( b->o() - o_ ) / sint2;
      return o_ + e_ * l;
    }
    return Point();
  }

  Point Line::evaluate ( double t ) const
  {
    return o_ + t * e_;
  }

  double Line::t ( const Point& p ) const
  {
    return e_ * ( p - o_ );
  }
} // End of Space3D namespace
