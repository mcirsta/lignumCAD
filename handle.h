/* -*- c++ -*-
 * handle.h
 *
 * Header for the Handle class
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
#ifndef HANDLE_H
#define HANDLE_H

#include <GL/gl.h>

#include "constants.h"

class OpenGLBase;

namespace Space2D {
  class Point;

  /*!
   * ResizeHandle provides a couple of static routines to draw and select
   * resize handles. This happens to be a 2D handle.
   */
  class ResizeHandle {
  public:
    /*!
     * Draw the resize handle at the given position and in the given mode.
     * \param view the view to render the handle in.
     * \param mode the rendering mode.
     * \param position the point in space about which the handle is centered.
     */
    static void draw ( OpenGLBase* view, lC::Render::Mode mode,
		       const Point& position);
    /*!
     * Draw the handle in select mode. The caller needs to have pushed
     * the selection name on the stack already.
     * \param view the view to render the handle in.
     * \param position the point in space about which the handle is centered.
     */
    static void select ( OpenGLBase* view, const Point& position );
  };
} // End of Space2D namespace

namespace Space3D {
  class Point;

  /*!
   * ResizeHandle provides a couple of static routines to draw and select
   * resize handles. This happens to be a 3D handle.
   */
  class ResizeHandle {
  public:
    /*!
     * Draw the resize handle at the given position and in the given mode.
     * \param view the view to render the handle in.
     * \param mode the rendering mode.
     * \param position the point in space about which the handle is centered.
     */
    static void draw ( OpenGLBase* view, lC::Render::Mode mode,
		       const Point& position );
    /*!
     * Draw the handle in select mode. The caller needs to have pushed
     * the selection name on the stack already.
     * \param view the view to render the handle in.
     * \param position the point in space about which the handle is centered.
     */
    static void select ( OpenGLBase* view, const Point& position );
  private:
    //! The vertices of each chamfered face.
    static GLfloat vertices_[24][3];
    //! The list of quad faces indices.
    static const GLubyte quads_[18][4];
    //! The list of the corner triangles indices.
    static const GLubyte tris_[8][3];
    //! The list of normals for each face.
    static const GLfloat normals_[30][3];
    //! The complementary angles to make a complete coordinate system on
    //! each face (used to compute the texture generation equation coefficients).
    static const GLubyte comp_angles_[26][2];
  };
} // End of Space3D namespace

#endif // HANDLE_H
