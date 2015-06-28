/*
 * arrow.cpp
 *
 * Arrow class
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
#include <GL/gl.h>

#include "arrow.h"

namespace Space2D {
  Arrow::Arrow ( lC::ArrowHeadStyle style, double length, double width_ratio )
    : style_( style ), length_( length ), width_ratio_( width_ratio )
  {}

  void Arrow::setStyle ( enum lC::ArrowHeadStyle style )
  {
    style_ = style;
  }

  void Arrow::setLength ( double length )
  {
    length_ = length;
  }

  void Arrow::setWidthRatio ( double width_ratio )
  {
    width_ratio_ = width_ratio;
  }

  void Arrow::draw ( const Point& from, const Point& to ) const
  {
    Vector v = to - from;
    double theta = v.orientation();
    double length = v.length();

    glPushMatrix();

    glTranslated( from.p_[X], from.p_[Y], 0. );
    glRotated( theta, 0., 0., 1. );

    switch ( style_ ) {
    case lC::OPEN:
      drawOpen( length ); break;
    case lC::HOLLOW:
      drawHollow( length ); break;
    case lC::FILLED:
      drawFilled( length ); break;
    }

    glPopMatrix();
  }

  void Arrow::select ( const Point& from, const Point& to ) const
  {
    // Nothing special here, I guess
    draw( from, to );
  }

  void Arrow::drawOpen ( double length ) const
  {
    glBegin( GL_LINES );

    // Stem
    glVertex2d( 0., 0. );
    glVertex2d( length, 0. );

    // Upper edge
    glVertex2d( length, 0. );
    glVertex2d( length - length_, length_ * width_ratio_ );

    // Lower edge
    glVertex2d( length, 0. );
    glVertex2d( length - length_, -length_ * width_ratio_ );
  
    glEnd();
  }

  void Arrow::drawHollow ( double length ) const
  {
    // Stem
    glBegin( GL_LINES );

    glVertex2d( 0., 0. );
    glVertex2d( length - length_, 0. );

    glEnd();

    // Arrow head
    glBegin( GL_LINE_LOOP );

    glVertex2d( length, 0. );
    glVertex2d( length - length_, -length_ * width_ratio_ );
    glVertex2d( length - length_, length_ * width_ratio_ );
  
    glEnd();
  }

  void Arrow::drawFilled ( double length ) const
  {
    // Stem
    glBegin( GL_LINES );

    glVertex2d( 0., 0. );
    glVertex2d( length - length_, 0. );

    glEnd();

    // Arrow head
    glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

    glBegin( GL_TRIANGLES );

    glVertex2d( length, 0. );
    glVertex2d( length - length_, -length_ * width_ratio_ );
    glVertex2d( length - length_, length_ * width_ratio_ );
  
    glEnd();
  }
} // End of Space2D namespace

namespace Space3D {
  Arrow::Arrow ( lC::ArrowHeadStyle style, double length, double width_ratio )
    : style_( style ), length_( length ), width_ratio_( width_ratio )
  {
    view_normal_ = Vector( 0, 0, 1 );
  }

  void Arrow::setStyle ( enum lC::ArrowHeadStyle style )
  {
    style_ = style;
  }

  void Arrow::setLength ( double length )
  {
    length_ = length;
  }

  void Arrow::setWidthRatio ( double width_ratio )
  {
    width_ratio_ = width_ratio;
  }

  void Arrow::setViewNormal ( const Vector& view_normal )
  {
    view_normal_ = view_normal;
  }

  void Arrow::draw ( const Point& from, const Point& to ) const
  {
    Vector v = to - from;
    double length = v.length();
    Vector n = v;
    n.normalize();
    Vector x( 1, 0, 0 );	// Canonical direction for drawing the arrow
    double theta = n * x;
    Vector r = x.cross( v ); // Rotation vector
    double rl = r.length();
    if ( fabs( rl ) > 1e-6 )
      r.normalize();
    else
      r = Vector( 0, 0, 1 );	// theta == 0 anyway

    // Also, figure out how to rotate the arrow head so that it is
    // normal to the view normal, i.e., so you can always see the side
    // of the arrow head. Start by applying the orienting rotation to
    // the canonical arrow head normal.
    Matrix m( Matrix::ROTATION, r[X], r[Y], r[Z], -acos( theta ) );
    Vector z( 0, 0, 1 );// Canonical arrow head normal
    Vector zr = m * z;
    Vector y( 0, 1, 0 );//The "other" direction (in the plane of n)
    Vector yr = m * y;
    double dz = zr * view_normal_;
    double dy = yr * view_normal_;
    double phi = atan2( dy, dz );

    glPushMatrix();

    glTranslated( from[X], from[Y], from[Z] );
    glRotated( -180 * phi / M_PI, n[X], n[Y], n[Z] );
    glRotated( 180 * acos( theta ) / M_PI, r[X], r[Y], r[Z] );

    switch ( style_ ) {
    case lC::OPEN:
      drawOpen( length ); break;
    case lC::HOLLOW:
      drawHollow( length ); break;
    case lC::FILLED:
      drawFilled( length ); break;
    }

    glPopMatrix();
  }

  void Arrow::select ( const Point& from, const Point& to ) const
  {
    // Nothing special here, I guess
    draw( from, to );
  }

  void Arrow::drawOpen ( double length ) const
  {
    glBegin( GL_LINES );

    // Stem
    glVertex2d( 0., 0. );
    glVertex2d( length, 0. );

    // Upper edge
    glVertex2d( length, 0. );
    glVertex2d( length - length_, length_ * width_ratio_ );

    // Lower edge
    glVertex2d( length, 0. );
    glVertex2d( length - length_, -length_ * width_ratio_ );
  
    glEnd();
  }

  void Arrow::drawHollow ( double length ) const
  {
    // Stem
    glBegin( GL_LINES );

    glVertex2d( 0., 0. );
    glVertex2d( length - length_, 0. );

    glEnd();

    // Arrow head
    glBegin( GL_LINE_LOOP );

    glVertex2d( length, 0. );
    glVertex2d( length - length_, -length_ * width_ratio_ );
    glVertex2d( length - length_, length_ * width_ratio_ );
  
    glEnd();
  }

  void Arrow::drawFilled ( double length ) const
  {
    // Stem
    glBegin( GL_LINES );

    glVertex2d( 0., 0. );
    glVertex2d( length - length_, 0. );

    glEnd();

    // Arrow head
    glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

    glBegin( GL_TRIANGLES );

    glVertex2d( length, 0. );
    glVertex2d( length - length_, -length_ * width_ratio_ );
    glVertex2d( length - length_, length_ * width_ratio_ );
  
    glEnd();
  }
} // End of Space3D namespace
