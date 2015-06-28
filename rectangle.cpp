/*
 * rectangle.cpp
 *
 * Rectangle classes
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

#include <qstringlist.h>
#include <qregexp.h>
#include <qlistview.h>
#include <qmessagebox.h>
#include <qdom.h>

#include "constants.h"
#include "units.h"
#include "line.h"
#include "model.h"
#include "page.h"
#include "rectangle.h"

namespace Space2D {
  uint Rectangle::unique_index_ = 0;

  QString Rectangle::newName ( void )
  {
    return tr( "Rectangle[%1]" ).arg( ++unique_index_ );
  }

  Rectangle::Rectangle ( uint id, const QString& name, Page* parent )
    : Figure( id, name, lC::STR::RECTANGLE, parent ),
      x0_( 0 ), y0_( 0 ), x1_( 0 ), y1_( 0 )
  {
    x0_ = new ConstrainedLine( 0, lC::STR::X0, this, 0 );
    y0_ = new ConstrainedLine( 0, lC::STR::Y0, this, 0 );
    x1_ = new ConstrainedLine( 0, lC::STR::X1, this, 0 );
    y1_ = new ConstrainedLine( 0, lC::STR::Y1, this, 0 );
  }

  Rectangle::Rectangle ( uint id, const QDomElement& xml_rep, Page* parent )
    : Figure( id, QString::null, lC::STR::RECTANGLE, parent ),
      x0_( 0 ), y0_( 0 ), x1_( 0 ), y1_( 0 )
  {
    setName( xml_rep.attribute( lC::STR::NAME ) );

    QRegExp regexp( tr("Rectangle\\[([0-9]+)\\]" ) );
    int position = regexp.search( name() );
    if ( position >= 0 ) {
      Rectangle::unique_index_ = QMAX( regexp.cap(1).toUInt(),
				       Rectangle::unique_index_ );
    }

    QDomNode n = xml_rep.firstChild();

    while ( !n.isNull() ) {
      QDomElement e = n.toElement();
      if ( !e.isNull() ) {
	if ( e.tagName() == lC::STR::ROLE ) {
	  QString role = e.attribute( lC::STR::NAME );

	  if ( role == lC::STR::X0 ) {
	    QDomNode line_node = e.firstChild();

	    if ( !line_node.isNull() ) {
	      QDomElement line_element = line_node.toElement();

	      if ( !line_element.isNull() )
		x0_ = new ConstrainedLine( 0, line_element, this );
	    }
	  }
	  else if ( role == lC::STR::Y0 ) {
	    QDomNode line_node = e.firstChild();

	    if ( !line_node.isNull() ) {
	      QDomElement line_element = line_node.toElement();

	      if ( !line_element.isNull() )
		y0_ = new ConstrainedLine( 0, line_element, this );
	    }
	  }
	  else if ( role == lC::STR::X1 ) {
	    QDomNode line_node = e.firstChild();

	    if ( !line_node.isNull() ) {
	      QDomElement line_element = line_node.toElement();

	      if ( !line_element.isNull() )
		x1_ = new ConstrainedLine( 0, line_element, this );
	    }
	  }
	  else if ( role == lC::STR::Y1 ) {
	    QDomNode line_node = e.firstChild();

	    if ( !line_node.isNull() ) {
	      QDomElement line_element = line_node.toElement();

	      if ( !line_element.isNull() )
		y1_ = new ConstrainedLine( 0, line_element, this );
	    }
	  }
	}
      }
      n = n.nextSibling();
    }

    // If the rectangle is being recreated as part of an undo, it may
    // have internal self-references which need to resolved.
    parent->model()->resolveNow();

    connect( x0_, SIGNAL( modifiedConstraint() ), SLOT( x0ModifiedConstraint()));
    connect( y0_, SIGNAL( modifiedConstraint() ), SLOT( y0ModifiedConstraint()));
    connect( x1_, SIGNAL( modifiedConstraint() ), SLOT( x1ModifiedConstraint()));
    connect( y1_, SIGNAL( modifiedConstraint() ), SLOT( y1ModifiedConstraint()));

    setValid( true );
    setComplete( true );
  }

  // This constructor uses the simple XML representation (and ignores
  // the name element)

  Rectangle::Rectangle ( uint id, const QString& name, const QDomElement& xml_rep,
			 Page* parent )
    : Figure( id, name, lC::STR::RECTANGLE, parent )
  {
    QRegExp regexp( tr("Rectangle\\[([0-9]+)\\]" ) );
    int position = regexp.search( name );
    if ( position >= 0 ) {
      Rectangle::unique_index_ = QMAX( regexp.cap(1).toUInt(),
				       Rectangle::unique_index_ );
    }

    Point origin( xml_rep.attribute( lC::STR::X0 ).toDouble(),
		  xml_rep.attribute( lC::STR::Y0 ).toDouble() );
    Point corner( xml_rep.attribute( lC::STR::X1 ).toDouble(),
		  xml_rep.attribute( lC::STR::Y1 ).toDouble() );

    x0_ = new ConstrainedLine( 0, lC::STR::X0, this, new Offset( origin, &Y_AXIS ) );
    y0_ = new ConstrainedLine( 0, lC::STR::Y0, this, new Offset( origin, &X_AXIS ) );
    x1_ = new ConstrainedLine( 0, lC::STR::X1, this, new Offset( corner, x0_ ) );
    y1_ = new ConstrainedLine( 0, lC::STR::Y1, this, new Offset( corner, y0_ ) );

    connect( x0_, SIGNAL( modifiedConstraint() ), SLOT( x0ModifiedConstraint()));
    connect( y0_, SIGNAL( modifiedConstraint() ), SLOT( y0ModifiedConstraint()));
    connect( x1_, SIGNAL( modifiedConstraint() ), SLOT( x1ModifiedConstraint()));
    connect( y1_, SIGNAL( modifiedConstraint() ), SLOT( y1ModifiedConstraint()));

    setValid( true );
    setComplete( true );
  }

  Rectangle::~Rectangle ( void )
  {
    // Yeah. Don't respond to these any more. We don't need to change the
    // width and height arrangements because they're all going away.
    if ( x0_ != 0 ) {
      disconnect( x0_, SIGNAL( modifiedConstraint() ),
		  this, SLOT( x0ModifiedConstraint()));
      if ( x0_->constraint() != 0 )
	disconnect( x0_->reference(), SIGNAL( destroyedLine() ),
		    x0_, SLOT( referenceDestroyed() ) );
    }
    if ( y0_ != 0 ) {
      disconnect( y0_, SIGNAL( modifiedConstraint() ),
		  this, SLOT( y0ModifiedConstraint()));
      if ( y0_->constraint() != 0 )
	disconnect( y0_->reference(), SIGNAL( destroyedLine() ),
		    y0_, SLOT( referenceDestroyed() ) );
    }
    if ( x1_ != 0 ) {
      disconnect( x1_, SIGNAL( modifiedConstraint() ),
		  this, SLOT( x1ModifiedConstraint()));
      if ( x1_->constraint() != 0 )
	disconnect( x1_->reference(), SIGNAL( destroyedLine() ),
		    x1_, SLOT( referenceDestroyed() ) );
    }
    if ( y1_ != 0 ) {
      disconnect( y1_, SIGNAL( modifiedConstraint() ),
		  this, SLOT( y1ModifiedConstraint()));
      if ( y1_->constraint() != 0 )
	disconnect( y1_->reference(), SIGNAL( destroyedLine() ),
		    y1_, SLOT( referenceDestroyed() ) );
    }

    if ( x0_ != 0 ) delete x0_;
    if ( y0_ != 0 ) delete y0_;
    if ( x1_ != 0 ) delete x1_;
    if ( y1_ != 0 ) delete y1_;
  }

  ModelItem* Rectangle::lookup ( QStringList& path_components ) const
  {
    // The front path component is the name of a figure with ".type" appended
    // to it.
    int dot_pos = path_components.front().findRev( '.' );
    QString name = path_components.front().left( dot_pos );
    QString type = path_components.front().right( path_components.front().length()
						  - dot_pos - 1 );

    if ( x0_ != 0 && x0_->name() == name && type == x0_->type() )
      return x0_;
    else if ( y0_ != 0 && y0_->name() == name && type == y0_->type() )
      return y0_;
    else if ( x1_ != 0 && x1_->name() == name && type == x1_->type() )
      return x1_;
    else if ( y1_ != 0 && y1_->name() == name && type == y1_->type() )
      return y1_;

    return 0;
  }

  ConstrainedLine* Rectangle::left ( void ) const
  {
    // Return which ever edge is currently leftmost. (Might best be inlined?)
    if ( x0_->o()[X] < x1_->o()[X] )
      return x0_;
    else
      return x1_;
  }

  ConstrainedLine* Rectangle::right ( void ) const
  {
    // Return which ever edge is currently rightmost. (Might best be inlined?)
    if ( x0_->o()[X] >= x1_->o()[X] )
      return x0_;
    else
      return x1_;
  }

  ConstrainedLine* Rectangle::bottom ( void ) const
  {
    // Return which ever edge is currently bottommost. (Might best be inlined?)
    if ( y0_->o()[Y] < y1_->o()[Y] )
      return y0_;
    else
      return y1_;
  }

  ConstrainedLine* Rectangle::top ( void ) const
  {
    // Return which ever edge is currently topmost. (Might best be inlined?)
    if ( y0_->o()[Y] >= y1_->o()[Y] )
      return y0_;
    else
      return y1_;
  }

  // By hook or by crook, determine which end of this edge is closest
  // to the given point and return the edges which intersect there

  Vertex Rectangle::closestVertex ( Point p, Curve* curve ) const
  {
    Vertex vertex( curve );

    // Suggests keeping the vertices around for just this reason

    Line* line = dynamic_cast< Line* >( curve );

    if ( line != x0_ &&
	 ( fabs( fabs( line->e() * x0_->e() ) - 1. ) > lC::EPSILON ) ) {
      Point q = line->intersect( x0_ );
      vertex.curve2_ = x0_;
      vertex.p_ = q;
    }

    if ( line != x1_ &&
	 ( fabs( fabs( line->e() * x1_->e() ) - 1. ) > lC::EPSILON ) ) {
      Point q = line->intersect( x1_ );
      if ( vertex.curve2_ == 0 ||
	   ( q.distance( p ) < vertex.p_.distance( p ) ) ) {
	vertex.curve2_ = x1_;
	vertex.p_ = q;
      }
    }

    if ( line != y0_ &&
	 ( fabs( fabs( line->e() * y0_->e() ) - 1. ) > lC::EPSILON ) ) {
      Point q = line->intersect( y0_ );
      if ( vertex.curve2_ == 0 ||
	   ( q.distance( p ) < vertex.p_.distance( p ) ) ) {
	vertex.curve2_ = y0_;
	vertex.p_ = q;
      }
    }

    if ( line != y1_ &&
	 ( fabs( fabs( line->e() * y1_->e() ) - 1. ) > lC::EPSILON ) ) {
      Point q = line->intersect( y1_ );
      if ( vertex.curve2_ == 0 ||
	   ( q.distance( p ) < vertex.p_.distance( p ) ) ) {
	vertex.curve2_ = y1_;
	vertex.p_ = q;
      }
    }

    return vertex;
  }

  // Basically finds the end-points of this edge. Note: you should be
  // able to infer which side is the interior of the rectangle from
  // the order of the vertices. Even though it has nothing to do with
  // how the rectangle is actually stored, this representation insures
  // a counter-clockwise (right-hand-rule) traverse of the figure.

  Segment Rectangle::closestVertices ( Curve* curve ) const
  {
    Segment segment( curve, true, true );

    if ( curve == left() ) {
      segment.from_ = ltVertex();
      segment.to_ = lbVertex();
    }
    else if ( curve == bottom() ) {
      segment.from_ = lbVertex();
      segment.to_ = rbVertex();
    }
    else if ( curve == right() ) {
      segment.from_ = rbVertex();
      segment.to_ = rtVertex();
    }
    else if ( curve == top() ) {
      segment.from_ = rtVertex();
      segment.to_ = ltVertex();
    }
    return segment;
  }

  // Here is a complicated routine which tries to build the data structures
  // similar to what the user has intended to enter.

  void Rectangle::initializeEdges ( Point p, Line* x_reference, Line* y_reference )
  {
    // First case: the user just clicks out in the middle of nowhere. The
    // Point becomes the offset from the coordinate system for each edge.
    if ( x_reference == 0 && y_reference == 0 ) {
      x0_->setConstraint( new Offset( p, &Y_AXIS ) );
      y0_->setConstraint( new Offset( p, &X_AXIS ) );
    }
    // Second case: only the initial horizontal edge has a reference;
    // causes two things:
    // 1. y0 is coincident with that reference
    // 2. we seek a close-by edge from the reference's parent from
    //    which to dimension x0
    else if ( x_reference == 0 && y_reference != 0 ) {
      p[Y] = y_reference->o()[Y];

      Vertex v = y_reference->parent()->closestVertex( p, y_reference );

      Line* x_reference = dynamic_cast< Line* >( v.curve2_ );

      if ( x_reference == 0 )
	x_reference = &Y_AXIS;

      x0_->setConstraint( new Offset( p, x_reference ) );
      y0_->setConstraint( new Coincident( y_reference ) );
    }

    // Third case: only the initial vertical edge has a *coincidence* reference
    else if ( x_reference != 0 && y_reference == 0 ) {
      p[X] = x_reference->o()[X];

      Vertex v = x_reference->parent()->closestVertex( p, x_reference );

      Line* y_reference = dynamic_cast< Line* >( v.curve2_ );

      if ( y_reference == 0 )
	y_reference = &X_AXIS;

      x0_->setConstraint( new Coincident( x_reference ) );
      y0_->setConstraint( new Offset( p, y_reference ) );
    }

    // Fourth case: both edges have *coincidence* references
    else {
      x0_->setConstraint( new Coincident( x_reference ) );
      y0_->setConstraint( new Coincident( y_reference ) );
    }
    // The other edges are offset from the first two (until their constraints
    // change, anyway)
    x1_->setConstraint( new Offset( x0_ ) );
    y1_->setConstraint( new Offset( y0_ ) );

    setValid( true );

    connect( x0_, SIGNAL( modifiedConstraint() ), SLOT( x0ModifiedConstraint()));
    connect( y0_, SIGNAL( modifiedConstraint() ), SLOT( y0ModifiedConstraint()));
    connect( x1_, SIGNAL( modifiedConstraint() ), SLOT( x1ModifiedConstraint()));
    connect( y1_, SIGNAL( modifiedConstraint() ), SLOT( y1ModifiedConstraint()));

    emit initialized();
  }

  void Rectangle::moveEdges ( Point p )
  {
    x1_->setPosition( p );
    y1_->setPosition( p );

    emit resized();
  }

  void Rectangle::finishEdges ( Point p, Line* x_reference, Line* y_reference )
  {
    // If there is no x reference, then x1 is free and its offset
    // essentially represents the width of the rectangle.
    if ( x_reference == 0 )
      x1_->setPosition( p );
    else
      x1_->setConstraint( new Coincident( x_reference ) );

    // Similarly for the y reference: if none, then y1's offset represents
    // the height of the rectangle.
    if ( y_reference == 0 )
      y1_->setPosition( p );
    else
      y1_->setConstraint( new Coincident( y_reference ) );

    setComplete( true );

    emit resized();
  }
  /*!
   * React to x0's constraint changing. Basically, try to maintain
   * a width-like offset dimension.
   */
  void Rectangle::x0ModifiedConstraint ( void )
  {
    // If x0 comes to depend on something other than x1 (actually the
    // normal state of affairs) and x1 is free, then set x1 to depend
    // on x0, trying to keep a width-like dimension for the rectangle.
    if ( !x0_->dependsOn( x1_ ) && x1_->isFree() )
      x1_->setConstraint( new Offset( x1_->o(), x0_ ) );
  }
  /*!
   * React to y0's constraint changing. Basically, try to maintain
   * a height-like offset dimension.
   */
  void Rectangle::y0ModifiedConstraint ( void )
  {
    // If y0 comes to depend on something other than y1 (actually the
    // normal state of affairs) and y1 is free, then set y1 to depend
    // on y0, trying to keep a height-like dimension for the rectangle.
    if ( !y0_->dependsOn( y1_ ) && y1_->isFree() )
      y1_->setConstraint( new Offset( y1_->o(), y0_ ) );
  }
  /*!
   * React to x1's constraint changing. Basically, try to maintain
   * a width-like offset dimension.
   */
  void Rectangle::x1ModifiedConstraint ( void )
  {
    // If x1 comes to depend on something other than x0 (unusual) and
    // x0 is free, then set x0 to depend on x1, trying to keep a
    // width-like dimension for the rectangle.
    if ( !x1_->dependsOn( x0_ ) && x0_->isFree() )
      x0_->setConstraint( new Offset( x0_->o(), x1_ ) );
  }
  /*!
   * React to y1's constraint changing. Basically, try to maintain
   * a height-like offset dimension.
   */
  void Rectangle::y1ModifiedConstraint ( void )
  {
    // If y1 comes to depend on something other than y0 (unusual) and
    // y0 is free, then set y0 to depend on y1, trying to keep a
    // height-like dimension for the rectangle.
    if ( !y1_->dependsOn( y0_ ) && y0_->isFree() )
      y0_->setConstraint( new Offset( y0_->o(), y1_ ) );
  }

  Point Rectangle::lbVertex ( void ) const
  {
    double x, y;

    x = left()->o()[X];
    y = bottom()->o()[Y];

    return Point( x, y );
  }

  Point Rectangle::rbVertex ( void ) const
  {
    double x, y;

    x = right()->o()[X];
    y = bottom()->o()[Y];

    return Point( x, y );
  }

  Point Rectangle::rtVertex ( void ) const
  {
    double x, y;

    x = right()->o()[X];
    y = top()->o()[Y]; 

    return Point( x, y );
  }

  Point Rectangle::ltVertex ( void ) const
  {
    double x, y;

    x = left()->o()[X];
    y = top()->o()[Y]; 

    return Point( x, y );
  }

  lC::ValidDelta Rectangle::move ( const Point& last_pnt, Point& current_pnt )
  {
    Vector delta = current_pnt - last_pnt;

    return move( delta );
  }

  lC::ValidDelta Rectangle::move ( const Vector& delta )
  {
    // Evidently, we need to be more careful about which edges
    // can be manipulated by the user interface. It's possible to
    // call this routine when one (or both) of the origin edges
    // are coincident, a property you cannot change by dragging
    // the rectangle.

    bool x_ok = true;
    bool y_ok = true;

    if ( x0_->isAdjustable() && x1_->dependsOn( x0_ ) )
      x0_->setPosition( x0_->o() + delta );
    else
      x_ok = false;

    if ( y0_->isAdjustable() && y1_->dependsOn( y0_ ) )
      y0_->setPosition( y0_->o() + delta );
    else
      y_ok = false;

    emit moved();

    return lC::ValidDelta( x_ok, y_ok );
  }

  lC::ValidDelta Rectangle::resizeLeft ( const Point& last_pnt, Point& current_pnt )
  {
    Vector delta = current_pnt - last_pnt;

    LengthUnit* length_unit = UnitsBasis::instance()->lengthUnit();
    lCPrecision* precision = length_unit->precision( UnitsBasis::instance()->format(),
					   UnitsBasis::instance()->precision() );
    double minimum_size = 1. / precision->unitsPerIn();

    if ( ( fabs( width() ) - delta[X] ) <= minimum_size ) {
      if ( fabs( width() ) <= minimum_size )
	return lC::ValidDelta( false );
      else {
	current_pnt[X] = last_pnt[X] + fabs( width() ) - minimum_size;
	delta[X] = fabs( width() ) - minimum_size;
      }
    }

    if ( x0_->o()[X] <= x1_->o()[X] ) {
      if ( x1_->isAdjustable() && x1_->dependsOn( x0_ ) )
	x1_->setPosition( x1_->o() - delta );

      x0_->setPosition( x0_->o() + delta );
    }
    else {
      x1_->setPosition( x1_->o() + delta );
    }

    emit resized();

    return lC::ValidDelta();
  }

  lC::ValidDelta Rectangle::resizeRight ( const Point& last_pnt, Point& current_pnt )
  {
    Vector delta = current_pnt - last_pnt;

    LengthUnit* length_unit = UnitsBasis::instance()->lengthUnit();
    lCPrecision* precision = length_unit->precision( UnitsBasis::instance()->format(),
					   UnitsBasis::instance()->precision() );
    double minimum_size = 1. / precision->unitsPerIn();

    if ( ( fabs( width() ) + delta[X] ) <= minimum_size ) {
      if ( fabs( width() ) <= minimum_size )
	return lC::ValidDelta( false );
      else {
	current_pnt[X] = last_pnt[X] - fabs( width() ) + minimum_size;
	delta[X] = -fabs( width() ) + minimum_size;
      }
    }

    if ( x0_->o()[X] > x1_->o()[X] ) {
      if ( x1_->isAdjustable() && x1_->dependsOn( x0_ ) )
	x1_->setPosition( x1_->o() - delta );

      x0_->setPosition( x0_->o() + delta );
    }
    else {
      x1_->setPosition( x1_->o() + delta );
    }

    emit resized();

    return lC::ValidDelta();
  }

  lC::ValidDelta Rectangle::resizeBottom ( const Point& last_pnt, Point& current_pnt )
  {
    Vector delta = current_pnt - last_pnt;

    LengthUnit* length_unit = UnitsBasis::instance()->lengthUnit();
    lCPrecision* precision = length_unit->precision( UnitsBasis::instance()->format(),
					   UnitsBasis::instance()->precision() );
    double minimum_size = 1. / precision->unitsPerIn();

    if ( ( fabs( height() ) - delta[Y] ) <= minimum_size ) {
      if ( fabs( height() ) <= minimum_size )
	return lC::ValidDelta( true, false );
      else {
	current_pnt[Y] = last_pnt[Y] + fabs( height() ) - minimum_size;
	delta[Y] = fabs( height() ) - minimum_size;
      }
    }

    if ( y0_->o()[Y] <= y1_->o()[Y] ) {
      if ( y1_->isAdjustable() && y1_->dependsOn( y0_ ) )
	y1_->setPosition( y1_->o() - delta );

      y0_->setPosition( y0_->o() + delta );
    }
    else {
      y1_->setPosition( y1_->o() + delta );
    }

    emit resized();

    return lC::ValidDelta();
  }

  lC::ValidDelta Rectangle::resizeTop ( const Point& last_pnt, Point& current_pnt )
  {
    Vector delta = current_pnt - last_pnt;

    LengthUnit* length_unit = UnitsBasis::instance()->lengthUnit();
    lCPrecision* precision = length_unit->precision( UnitsBasis::instance()->format(),
					   UnitsBasis::instance()->precision() );
    double minimum_size = 1. / precision->unitsPerIn();

    if ( ( fabs( height() ) + delta[Y] ) <= minimum_size ) {
      if ( fabs( height() ) <= minimum_size )
	return lC::ValidDelta( true, false );
      else {
	current_pnt[Y] = last_pnt[Y] - fabs( height() ) + minimum_size;
	delta[Y] = - fabs( height() ) + minimum_size;
      }
    }
    if ( y0_->o()[Y] > y1_->o()[Y] ) {
      if ( y1_->isAdjustable() && y1_->dependsOn( y0_ ) )
	y1_->setPosition( y1_->o() - delta );

      y0_->setPosition( y0_->o() + delta );
    }
    else {
      y1_->setPosition( y1_->o() + delta );
    }

    emit resized();

    return lC::ValidDelta();
  }

  lC::ValidDelta Rectangle::resizeLeftBottom ( const Point& last_pnt,
					   Point& current_pnt )
  {
    lC::ValidDelta ok_left = resizeLeft( last_pnt, current_pnt );
    lC::ValidDelta ok_bottom = resizeBottom( last_pnt, current_pnt );

    return lC::ValidDelta( ok_left.dx_ok_, ok_bottom.dy_ok_ );
  }

  lC::ValidDelta Rectangle::resizeRightBottom ( const Point& last_pnt,
					    Point& current_pnt )
  {
    lC::ValidDelta ok_right = resizeRight( last_pnt, current_pnt );
    lC::ValidDelta ok_bottom = resizeBottom( last_pnt, current_pnt );

    return lC::ValidDelta( ok_right.dx_ok_, ok_bottom.dy_ok_ );
  }

  lC::ValidDelta Rectangle::resizeLeftTop ( const Point& last_pnt, Point& current_pnt )
  {
    lC::ValidDelta ok_left = resizeLeft( last_pnt, current_pnt );
    lC::ValidDelta ok_top = resizeTop( last_pnt, current_pnt );

    return lC::ValidDelta( ok_left.dx_ok_, ok_top.dy_ok_ );
  }

  lC::ValidDelta Rectangle::resizeRightTop ( const Point& last_pnt, Point& current_pnt )
  {
    lC::ValidDelta ok_right = resizeRight( last_pnt, current_pnt );
    lC::ValidDelta ok_top = resizeTop( last_pnt, current_pnt );

    return lC::ValidDelta( ok_right.dx_ok_, ok_top.dy_ok_ );
  }

  Point Rectangle::origin ( void ) const
  {
    return Point( x0_->o()[X], y0_->o()[Y] );
  }

  Point Rectangle::corner ( void ) const
  {
    return Point( x1_->o()[X], y1_->o()[Y] );
  }

  double Rectangle::width ( void ) const
  {
    return ( x1_->o() - x0_->o() ) * Vector( x0_->e()[Y], -x0_->e()[X] );
  }

  double Rectangle::height ( void ) const
  {
    return ( y1_->o() - y0_->o() ) * Vector( y0_->e()[Y], -y0_->e()[X] );
  }

  void Rectangle::setOrigin ( const Point& origin )
  {
    x0_->setPosition( Point( origin[X], 0. ) );
    y0_->setPosition( Point( 0., origin[Y] ) );
  }

  void Rectangle::setWidth ( double width )
  {
    (void)width;
  }

  void Rectangle::setHeight ( double height )
  {
    (void)height;
  }

  void Rectangle::write ( QDomElement& xml_rep ) const
  {
    if ( !isValid() ) return;

    QDomDocument document = xml_rep.ownerDocument();
    QDomElement rectangle_element = document.createElement( lC::STR::RECTANGLE );
    rectangle_element.setAttribute( lC::STR::ID, id() );
    rectangle_element.setAttribute( lC::STR::NAME, name() );

    QDomElement role_x0_element = document.createElement( lC::STR::ROLE );
    role_x0_element.setAttribute( lC::STR::NAME, lC::STR::X0 );

    x0_->write( role_x0_element );

    QDomElement role_y0_element = document.createElement( lC::STR::ROLE );
    role_y0_element.setAttribute( lC::STR::NAME, lC::STR::Y0 );

    y0_->write( role_y0_element );

    QDomElement role_x1_element = document.createElement( lC::STR::ROLE );
    role_x1_element.setAttribute( lC::STR::NAME, lC::STR::X1 );

    x1_->write( role_x1_element );

    QDomElement role_y1_element = document.createElement( lC::STR::ROLE );
    role_y1_element.setAttribute( lC::STR::NAME, lC::STR::Y1 );

    y1_->write( role_y1_element );

    rectangle_element.appendChild( role_x0_element );
    rectangle_element.appendChild( role_y0_element );
    rectangle_element.appendChild( role_x1_element );
    rectangle_element.appendChild( role_y1_element );

    xml_rep.appendChild( rectangle_element );
  }

  /*
   * Append a simplified representation of the rectangle to the
   * argument XML document.
   */
  void Rectangle::copyWrite ( QDomElement& xml_rep ) const
  {
    QDomDocument document = xml_rep.ownerDocument();
    QDomElement rectangle_element = document.createElement( lC::STR::RECTANGLE );
    rectangle_element.setAttribute( lC::STR::NAME, name() );

    rectangle_element.setAttribute( lC::STR::X0, lC::format( origin()[X] ) );
    rectangle_element.setAttribute( lC::STR::Y0, lC::format( origin()[Y] ) );
    rectangle_element.setAttribute( lC::STR::X1, lC::format( corner()[X] ) );
    rectangle_element.setAttribute( lC::STR::Y1, lC::format( corner()[Y] ) );

    xml_rep.appendChild( rectangle_element );
  }

  // Determine if the new name is OK.

  bool Rectangle::uniqueGeometryName ( const Geometry* geometry,
				       const QString& name ) const
  {
    if ( ( name == lC::formatName( x0_->name() ) && geometry != x0_ ) ||
	 ( name == lC::formatName( y0_->name() ) && geometry != y0_ ) ||
	 ( name == lC::formatName( x1_->name() ) && geometry != x1_ ) ||
	 ( name == lC::formatName( y1_->name() ) && geometry != y1_ ) )
      return false;

    return true;
  }
} // End of Space2D namespace
