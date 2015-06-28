/* -*- c++ -*-
 * curve.h
 *
 * Header for the generic curve classes
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
#ifndef CURVE_H
#define CURVE_H

#ifndef LC_NO_DEBUG
#include <iostream>
#endif

#include "geometry.h"

namespace Space2D {
  class Point;
  /*!
   * This is a general curve (i.e., 1D object) in 2D space. It exists
   * to refine the types of intersections which can be computed between
   * 2D geometries.
   */
  class Curve : public Geometry {
    Q_OBJECT
  public:
    /*!
     * Constructor just passes arguments to Geometry superclass.
     * \param id id of curve.
     * \param name name of curve.
     * \param type type of curve.
     * \param parent figure parent of curve.
     */
    Curve ( uint id, const QString& name, const QString& type, Figure* parent )
      : Geometry( id, name, type, parent )
    {}
    /*!
     * The intersection of two curves is a point. The concrete geometry
     * must define this function.
     * Later, we may have to return a list of points, since the real
     * intersection may consist of 0, 1 or more points...
     * \param c the curve to intersect with this.
     * \return the point of their intersection.
     */
    virtual Point intersect ( const Curve* c ) const = 0;
  };

} // End of Space2D namespace

namespace Space3D {
  class Point;
  /*!
   * This is a general curve (i.e., 1D object) in 3D space. It exists
   * to refine the types of intersections which can be computed between
   * 3D geometries.
   */
  class Curve : public Geometry {
    Q_OBJECT
  public:
    /*!
     * Constructor just passes arguments to Geometry superclass.
     * \param id id of curve.
     * \param name name of curve.
     * \param type type of curve.
     * \param parent figure parent of curve.
     */
    Curve ( uint id, const QString& name, const QString& type, Figure* parent )
      : Geometry( id, name, type, parent )
    {}
    /*!
     * The intersection of two curves is a point. The concrete geometry
     * must define this function.
     * Later, we may have to return a list of points, since the real
     * intersection may consist of 0, 1 or more points...
     * \param c the curve to intersect with this.
     * \return the point of their intersection.
     */
    virtual Point intersect ( const Curve* c ) const = 0;
    /*!
     * Find the point which corresponds to this parameter.
     * \param t curve parameter.
     * \return point in space corresponding to t.
     */
    virtual Point evaluate ( double t ) const = 0;
    /*!
     * Find the parameter which corresponds to this point in space.
     * \param p point on curve.
     * \return parametric coordinate of p.
     */
    virtual double t ( const Point& p ) const = 0;
#ifndef LC_NO_DEBUG
    /*!
     * Produce a representation of yourself on the output stream.
     * \param o C++ output stream.
     * \return reference to output stream.
     */
    virtual std::ostream& edit ( std::ostream& o ) const = 0;
#endif
  };
#ifndef LC_NO_DEBUG
  inline std::ostream& operator<< ( std::ostream& o, const Curve& c )
  {
    return c.edit( o );
  }
#endif

} // End of Space3D namespace

#endif // CURVE_H
