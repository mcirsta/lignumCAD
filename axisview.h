/* -*- c++ -*-
 * axisview.h
 *
 * Header for the AxisView class
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
#ifndef AXISVIEW_H
#define AXISVIEW_H

#include "constructiondatumview.h"

namespace Space3D {
  class Axis;
  /*!
   * Abstract view of a construction datum. Mostly virtual.
   */
  class AxisView : public ConstructionDatumView {
  public:
    AxisView ( const Axis* axis ) : axis_( axis ) {}
    ~AxisView ( void ) {}
    void draw ( OpenGLBase* view ) const;
    void select (SelectionType select_type ) const;
  private:
    const Axis* axis_;
  };
} // end of Space3D namespace
#endif // AXISVIEW_H
