/* -*- c++ -*-
 * alignment.h
 *
 * Header for the Alignment class
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
#ifndef ALIGNMENT_H
#define ALIGNMENT_H
class OpenGLBase;
class Segment;
/*!
 * A helper class for drawing alignment (aka coincident) constaints.
 */
namespace Space2D {
  class Alignment {
  public:
    /*!
     * Draw the alignment constraint. The alignment (aka coincident)
     * constraint visualization consists of a set of arrows which point
     * to the two curves which are contrained.
     * \param view the OpenGL window to draw in.
     * \param edge a structure representing the shape of the primary constraint.
     * \param reference a structure representing the shape of the
     * secondary constraint.
     */
    static void draw ( OpenGLBase* view,
		       const Segment& edge, const Segment& reference );
  };
} // End of Space2D namespace

#endif // ALIGNMENT_H
