/*
 * alignment.cpp
 *
 * Alignment class
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
#include "openglbase.h"
#include "vectoralgebra.h"
#include "arrow.h"
#include "figure.h"
#include "alignment.h"

namespace Space2D {

  // Draw the constraint.

  void Alignment::draw ( OpenGLBase* view,
			 const Segment& edge, const Segment& reference )
  {
    Arrow arrow( lC::OPEN, 3./32. * view->scale() );
    const double SPACING = 0.25;
    const double WIDTH = 0.125;
    double width = WIDTH * view->scale();

    Point from, to;
    if ( edge.valid_from_ && edge.valid_to_ ) {
      from = edge.from_;
      to = edge.to_;
    }
    else {
      Vector u = edge.to_ - edge.from_;
      from = edge.from_ + ( ( view->llCorner() - edge.from_ ) * u ) * u;
      to = edge.from_ + ( ( view->urCorner() - edge.from_ ) * u ) * u;
    }

    // The spine is vector between the two end points.
    Vector spine = to - from;
    double l = spine.length() / view->scale();

    // Compute the perpendicular to the spine and normalize it. This is Used
    // to find the endpoints of each of the little arrows.
    Vector offset = spine;
    offset.rotate(90.).normalize();

    // Compute the number of arrows which fit along the spine.

    uint n_arrows = (uint)::floor( l / SPACING + .5 );
    n_arrows = n_arrows > 0 ? n_arrows : 1;
    double t = 0.;
    double d = 1. / n_arrows;

    glColor3ubv( lC::qCubv( view->constraintPrimaryColor() ) );

    from += d / 2. * spine;

    for ( uint i = 0; i < n_arrows; i++ ) {
      Point p_to = from + t * spine;
      Point p_from = p_to + width * offset;
      arrow.draw( p_from, p_to );

      t += d;
    }

    // The spine is vector between the two end points. Have to make
    // sure that they are valid, though. If not, use the screen
    // boundaries projected onto the curve instead.
    if ( reference.valid_from_ && reference.valid_to_ ) {
      from = reference.from_;
      to = reference.to_;
    }
    else {
      Vector u = reference.to_ - reference.from_;
      from = reference.from_ +
	( ( view->llCorner() - reference.from_ ) * u ) * u;
      to = reference.from_ +
	( ( view->urCorner() - reference.from_ ) * u ) * u;
    }

    spine = to - from;
    l = spine.length() / view->scale();

    // Compute the number of arrows which fit along the spine.

    n_arrows = (uint)::floor(l / SPACING + .5);
    n_arrows = n_arrows > 0 ? n_arrows : 1;
    t = 0.;
    d = 1. / n_arrows;

    glColor3ubv( lC::qCubv( view->constraintSecondaryColor() ) );

    from += d / 2. * spine;

    for ( uint i = 0; i < n_arrows; i++ ) {
      Point p_to = from + t * spine;
      Point p_from = p_to - width * offset;
      arrow.draw( p_from, p_to );

      t += d;
    }
  }
} // End of Space2D namespace
