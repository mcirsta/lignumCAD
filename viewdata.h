/* -*- c++ -*-
 * viewdata.h
 *
 * Interfaces for the ViewData object.
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
#ifndef VIEWDATA_H
#define VIEWDATA_H

#include "ratio.h"
#include "vectoralgebra.h"

class QDomElement;
enum Space;

/*!
 * Save the information which (minimally) describes the current
 * view.
 */
struct ViewData {
  //! The scale of the view.
  Ratio scale_;
  //! View point in model space. This is the lower left corner of the
  //! window in 2D and the center of the window in 3D.
  Space3D::Point view_point_;
  //! In 3D, there is also the possibility of rotating the view. See
  //! the notes for the derivation of the rotation angles.
  //! Initial Z axis rotation in degrees.
  double z_angle_0_;
  //! Y axis rotation in degrees.
  double y_angle_0_;
  //! Second Z axis rotation in degrees.
  double z_angle_1_;
  //! Display (or not) the the origin coordinate system.
  bool show_csys_;
  /*!
   * Default constructor merely initializes everything to zero.
   */
  ViewData ( void );

  /*!
   * Restore the view dataation from its XML representation.
   * \param xml_rep XML representation.
   */
  ViewData ( const QDomElement& xml_rep );

  /*!
   * Reset the view data to its default for the given space.
   * \param space select the 2D or 3D defaults.
   */
  void reset ( Space space );

  /*!
   * Save the current view dataation.
   * \param xml_rep parent DOM node.
   */
  void write ( QDomElement& xml_rep ) const;

  /*!
   * Compare two views. Includes a bit of hysteresis for the floating
   * point numbers.
   * \param b ViewData object to compare to this one.
   * \return true if ViewData objects are essentially equal.
   */
  bool operator== ( const ViewData& b ) const;
};
#endif // VIEWDATA_H
