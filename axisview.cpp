/*
 * axisview.cpp
 *
 * Axis View classes
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
#include "axis.h"
#include "axisview.h"

namespace Space3D {
  // Draw the axis.
  void AxisView::draw ( OpenGLBase* view ) const
  {
    gp_Pnt min = axis_->minimum();
    gp_Pnt max = axis_->maximum();
    glColor3ubv( lC::qCubv( view->annotationColor() ) );
    glEnable( GL_LINE_STIPPLE );
    glLineStipple( 1, 0x18ff );
    glBegin( GL_LINES );
    glVertex3f( min.X(), min.Y(), min.Z() );
    glVertex3f( max.X(), max.Y(), max.Z() );
    glEnd();
    glDisable( GL_LINE_STIPPLE );
  }

  void AxisView::select (SelectionType /*select_type*/ ) const {}
  /*!
   * The metadata defines a couple of methods to invoke in order to
   * create construction datum views without knowing exactly what
   * kind of datum it is. (Too complicated?...)
   */
  class AxisViewMetadata : public ConstructionDatumViewMetadata {
  public:
    AxisViewMetadata ( void )
    {
      ConstructionDatumViewFactory::instance()->
	addConstructionDatumViewMetadata( lC::STR::AXIS, this );
    }

    //! Subclass may have something to do (but probably not).
    ~AxisViewMetadata ( void ) {}

    //! \return a view for this construction datum.
    AxisView* create ( const ConstructionDatum* datum ) const
    {
      const Axis* axis = dynamic_cast<const Axis*>( datum );
      return new AxisView( axis );
    }
  };

  static const AxisViewMetadata axis_view_metadata;
} // End of Space2D namespace
