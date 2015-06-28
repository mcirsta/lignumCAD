/* -*- c++ -*-
 * ochiddendraw.h
 *
 * Header for the OC Hidden Draw classes using OpenCASCADE
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
#ifndef OCHIDDENDRAW_H
#define OCHIDDENDRAW_H

#include <GL/gl.h>

#include <TopoDS_Compound.hxx>

class OpenGLView;

namespace Space3D {
  /*!
   * Drawing an assembly in hidden line mode is complicated by the fact
   * that the face of one part may hide an edge in another part. So, in
   * order to render the correct image, all the surfaces of an assembly
   * have to used to compute the hidden lines. And each of the solids
   * needs to have its correct location. Thus, the hidden line view
   * must be computed at a higher level than the part (as in OCSolidDraw).
   */
  class OCHiddenDraw {
  public:
    OCHiddenDraw ( const TopoDS_Compound& assembly, OpenGLView* view );
    /*!
     * The assembly has changed, so update the edges.
     * \param assembly assembly (as TopoDS_Compound) to draw.
     */
    void setAssembly ( const TopoDS_Compound& assembly );
    /*!
     * The critical operation for hidden line removal: the orientation of
     * the view has changed, so we need another recomputation.
     * \param modelview the modelview transformation matrix.
     */
    void setOrientation ( const GLdouble* modelview );
    /*!
     * Draw in hidden line mode. Highlights and handles are drawn
     * by the regular rendering routines (as is selection).
     */
    void draw ( void ) const;

  private:
    //! Recompute the hidden lines.
    void update ( void );

    //! The current compound
    TopoDS_Compound assembly_;
    //! The OpenGL view.
    OpenGLView* view_;
    //! Display list for hidden line view foreground edges
    GLuint hlr_fg_name_;
    //! Display list for hidden line view background edges
    GLuint hlr_bg_name_;
  };
} // End of Space3D namespace

#endif // OCHIDDENDRAW_H
