/*
 * axis.cpp
 *
 * Axis classes
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
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Edge.hxx>

#include "axis.h"

namespace Space3D {
  gp_Pnt Axis::minimum ( void ) const
  {
    gp_Pnt origin = axis_.Location();
    gp_Vec direction = axis_.Direction();
    direction.Scale( min_ );
    return origin.Translated( direction );
  }

  gp_Pnt Axis::maximum ( void ) const
  {
    gp_Pnt origin = axis_.Location();
    gp_Vec direction = axis_.Direction();
    direction.Scale( max_ );
    return origin.Translated( direction );
  }

  TopoDS_Shape Axis::lookupShape ( QStringList& /*path_components*/ ) const
  {
    return BRepBuilderAPI_MakeEdge( minimum(), maximum() ).Edge();
  }

  TopoDS_Shape Axis::lookupShape ( QVector<uint>& /*id_path*/ ) const
  {
    return BRepBuilderAPI_MakeEdge( minimum(), maximum() ).Edge();
  }
  
} // End of Space3D namespace
