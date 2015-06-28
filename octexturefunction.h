/* -*- c++ -*-
 * octexturefunction.h
 *
 * Header for the OCTextureFunction classes
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
#ifndef OCTEXTUREFUNCTION_H
#define OCTEXTUREFUNCTION_H

#include <map>

#include <GL/gl.h>

#include <BRep_Tool.hxx>
#include <BRepTools.hxx>
#include <Standard_Integer.hxx>
#include <Geom_Surface.hxx>

class gp_Ax2;
class gp_Pnt2d;
class gp_Pnt;
class TopoDS_Face;
class Material;
class OpenGLBase;

class OCTextureFunction {
public:
  //! Destructor does nothing (except make it an abstract class).
  virtual ~OCTextureFunction ( void ) {}
  /*!
   * Update the generic properties used to in the computation of the
   * texture coordinates.
   * \param view the OpenGL view.
   * \param surface the specific surface.
   * \param umin the beginning value of the face's u parameter.
   * \param umax the ending value of the face's u parameter.
   * \param vmin the beginning value of the face's v parameter.
   * \param vmax the ending value of the face's v parameter.
   * \param grain the grain coordinate system.
   * \param material optional material of the solid.
   */
  virtual void setProperties ( OpenGLBase* view, const Handle(Geom_Surface) surface,
			       double umin, double umax, double vmin, double vmax,
			       const gp_Ax2& grain, const Material* material ) = 0;
  //! \return the OpenGL texture name.
  virtual GLuint texture ( void ) const = 0;
  /*!
   * Return the texture coordinates based on the position
   * of the vertex on the surface. Depending on the surface,
   * either one or the other definitions of the position will be
   * used.
   * \param uv parametric position of vertex.
   * \param p real position of vertex.
   * \return a pointer to a static array containing the two texture
   * coordinates, s and t.
   */
  virtual GLfloat* texCoords ( const gp_Pnt2d& uv, const gp_Pnt& p ) const = 0;
};

class OCTextureFactory {
public:
  //! \return the singleton instance of the texture factory.
  static OCTextureFactory* instance ( void );

  /*!
   * Add a texture function for this kind of surface.
   * \param type_hash_code the OC hash code for the surface type.
   * \param texture_function the texture function object.
   */
  void addTextureFunction ( Standard_Integer type_hash_code,
			    OCTextureFunction* texture_function );

  /*!
   * Lookup the texture function for this face and squirrel away
   * the relevant information from the solid itself (i.e., the grain
   * direction and material).
   * \param view the OpenGL view.
   * \param face the face to texture map.
   * \param grain the grain coordinate system.
   * \param material optional material of the solid.
   */
  OCTextureFunction* function ( OpenGLBase* view,
				const TopoDS_Face& face,
				const gp_Ax2& grain,
				const Material* material );

protected:
  //! Per "Design Patterns": It can only construct itself.
  OCTextureFactory();

private:
  //! The singleton instance of the texture factory.
  static OCTextureFactory* instance_;

  //! The available texture function objects.
  std::map<Standard_Integer, OCTextureFunction*> texture_functions_;
};

#endif // OCTEXTUREFUNCTION_H
