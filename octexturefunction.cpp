/*
 * octexturefunction.cpp
 *
 * OCTextureFunction classes
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
#include <gp_Circ.hxx>
#include <gp_Cone.hxx>
#include <gp_Cylinder.hxx>
#include <gp_Pln.hxx>
#include <Bnd_Box.hxx>
#include <BndLib_AddSurface.hxx>
#include <Geom_Circle.hxx>
#include <Geom_ConicalSurface.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_Plane.hxx>
#include <Geom_SurfaceOfRevolution.hxx>
#include <GeomAPI_ProjectPointOnSurf.hxx>
#include <GeomLProp_SLProps.hxx>

#include "constants.h"
#include "material.h"
#include "openglbase.h"
#include "octexturefunction.h"

/*!
 * On the off chance we get an unfamiliar surface, use this texture function.
 */
class OCUnknownTexture : public OCTextureFunction {
public:
  OCUnknownTexture ( void ) { /* Nothing to do here*/ }
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
  void setProperties ( OpenGLBase* view, const Handle( Geom_Surface) surface,
		       double umin, double umax, double vmin, double vmax,
		       const gp_Ax2& grain, const Material* material )
  {
    surface_ = surface;
    grain_ = grain;
    material_ = material;

    // Basically, we're just going to take the normal at the mid-point
    // and treat the surface like a plane.
    double umid, vmid;

    if ( Precision::IsInfinite( umin ) || Precision::IsInfinite( umax ) )
      umid = umin;
    else
      umid = ( umin + umax ) / 2;

    if ( Precision::IsInfinite( vmin ) || Precision::IsInfinite( vmax ) )
      vmid = vmin;
    else
      vmid = ( vmin + vmax ) / 2;

    GeomLProp_SLProps properties( surface_, umid, vmid, 1, gp::Resolution() );
    gp_Dir normal = properties.Normal();

    Standard_Real x_dot_n =  fabs( grain_.XDirection() * normal );
    Standard_Real y_dot_n =  fabs( grain_.YDirection() * normal );
    Standard_Real z_dot_n =  fabs( grain_.Direction() * normal );

    texture_ = 0;
    s_resolution_ = t_resolution_ = 20. / 256.;

    if ( x_dot_n > y_dot_n && x_dot_n > z_dot_n ) {
      if ( material_ != 0 ) {
	QImage image = lC::lookupImage( material_->faceGrainFile() );
	if ( !image.isNull() ) {
	  s_resolution_ = 0.0254 * image.dotsPerMeterX() / image.width();
	  t_resolution_ = 0.0254 * image.dotsPerMeterY() / image.height();

	  texture_ = view->texture( material_->faceGrainFile() );
	}
	if ( s_resolution_ == 0. ) s_resolution_ = 20. / image.width();
	if ( t_resolution_ == 0. ) s_resolution_ = 20. / image.height();
      }
      reference_plane_ = new Geom_Plane( gp_Ax3( grain_.Location(),
						 grain_.XDirection(),
						 grain_.YDirection() ) );
    }
    else if ( y_dot_n > z_dot_n ) {
      if ( material_ != 0 ) {
	QImage image = lC::lookupImage( material_->edgeGrainFile() );
	if ( !image.isNull() ) {
	  s_resolution_ = 0.0254 * image.dotsPerMeterX() / image.width();
	  t_resolution_ = 0.0254 * image.dotsPerMeterY() / image.height();

	  texture_ = view->texture( material_->edgeGrainFile() );
	}
	if ( s_resolution_ == 0. ) s_resolution_ = 20. / image.width();
	if ( t_resolution_ == 0. ) s_resolution_ = 20. / image.height();
      }
      reference_plane_ = new Geom_Plane( gp_Ax3( grain_.Location(),
						 grain_.YDirection(),
						 grain_.XDirection() ) );
    }
    else {
      if ( material_ != 0 ) {
	QImage image = lC::lookupImage( material_->endGrainFile() );
	if ( !image.isNull() ) {
	  s_resolution_ = 0.0254 * image.dotsPerMeterX() / image.width();
	  t_resolution_ = 0.0254 * image.dotsPerMeterY() / image.height();

	  texture_ = view->texture( material_->endGrainFile() );
	}
	if ( s_resolution_ == 0. ) s_resolution_ = 20. / image.width();
	if ( t_resolution_ == 0. ) s_resolution_ = 20. / image.height();
      }
      reference_plane_ = new Geom_Plane( gp_Ax3( grain_.Location(),
						 grain_.Direction(),
						 grain_.YDirection() ) );
    }
  }
  //! \return the OpenGL texture name.
  GLuint texture ( void ) const { return texture_; }
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
  GLfloat* texCoords ( const gp_Pnt2d& uv, const gp_Pnt& p ) const
  {
    static GLfloat coords[2];

    if ( material_ != 0 ) {
      GeomAPI_ProjectPointOnSurf project( p, reference_plane_ );
      Quantity_Parameter u, v;
      project.LowerDistanceParameters( u, v );
      coords[0] = s_resolution_ * u;
      coords[1] = t_resolution_ * v;
    }
    else {
      coords[0] = s_resolution_ * uv.X();
      coords[1] = t_resolution_ * uv.Y();
    }

    return coords;
  }

private:
  //! Handle to the generic surface.
  Handle( Geom_Surface ) surface_;
  //! Grain orientation.
  gp_Ax2 grain_;
  //! Reference to the material.
  const Material* material_;
  //! OpenGL texture name.
  GLuint texture_;
  //! Conversion factor from s texture coordinate to texture image resolution.
  GLfloat s_resolution_;
  //! Conversion factor from t texture coordinate to texture image resolution.
  GLfloat t_resolution_;
  //! The surface coordinates are projected on this plane to provide parametric
  //! coordinates.
  Handle( Geom_Plane ) reference_plane_;
};

/*!
 * The planar texture function.
 */
class OCPlaneTexture : public OCTextureFunction {
public:
  OCPlaneTexture ( void ) {
    OCTextureFactory::instance()->addTextureFunction( HashCode(STANDARD_TYPE( Geom_Plane ), IntegerLast() ), this );
  }
  /*!
   * Update the planar surface properties used to in the computation of the
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
  void setProperties ( OpenGLBase* view, const Handle( Geom_Surface) surface,
		       double /*umin*/, double /*umax*/, double /*vmin*/, double /*vmax*/,
		       const gp_Ax2& grain, const Material* material )
  {
    plane_ = Handle(Geom_Plane)::DownCast( surface );
    grain_ = grain;
    material_ = material;

    gp_Dir normal = plane_->Pln().Axis().Direction();

    Standard_Real x_dot_n =  fabs( grain_.XDirection() * normal );
    Standard_Real y_dot_n =  fabs( grain_.YDirection() * normal );
    Standard_Real z_dot_n =  fabs( grain_.Direction() * normal );

    texture_ = 0;
    s_resolution_ = t_resolution_ = 20. / 256.;

    if ( x_dot_n > y_dot_n && x_dot_n > z_dot_n ) {
      if ( material_ != 0 ) {
	QImage image = lC::lookupImage( material_->faceGrainFile() );
	if ( !image.isNull() ) {
	  s_resolution_ = 0.0254 * image.dotsPerMeterX() / image.width();
	  t_resolution_ = 0.0254 * image.dotsPerMeterY() / image.height();

	  texture_ = view->texture( material_->faceGrainFile() );
	}
	if ( s_resolution_ == 0. ) s_resolution_ = 20. / image.width();
	if ( t_resolution_ == 0. ) s_resolution_ = 20. / image.height();
      }
      reference_plane_ = new Geom_Plane( gp_Ax3( grain_.Location(),
						 grain_.XDirection(),
						 grain_.YDirection() ) );
    }
    else if ( y_dot_n > z_dot_n ) {
      if ( material_ != 0 ) {
	QImage image = lC::lookupImage( material_->edgeGrainFile() );
	if ( !image.isNull() ) {
	  s_resolution_ = 0.0254 * image.dotsPerMeterX() / image.width();
	  t_resolution_ = 0.0254 * image.dotsPerMeterY() / image.height();

	  texture_ = view->texture( material_->edgeGrainFile() );
	}
	if ( s_resolution_ == 0. ) s_resolution_ = 20. / image.width();
	if ( t_resolution_ == 0. ) s_resolution_ = 20. / image.height();
      }
      reference_plane_ = new Geom_Plane( gp_Ax3( grain_.Location(),
						 grain_.YDirection(),
						 grain_.XDirection() ) );
    }
    else {
      if ( material_ != 0 ) {
	QImage image = lC::lookupImage( material_->endGrainFile() );
	if ( !image.isNull() ) {
	  s_resolution_ = 0.0254 * image.dotsPerMeterX() / image.width();
	  t_resolution_ = 0.0254 * image.dotsPerMeterY() / image.height();

	  texture_ = view->texture( material_->endGrainFile() );
	}
	if ( s_resolution_ == 0. ) s_resolution_ = 20. / image.width();
	if ( t_resolution_ == 0. ) s_resolution_ = 20. / image.height();
      }
      reference_plane_ = new Geom_Plane( gp_Ax3( grain_.Location(),
						 grain_.Direction(),
						 grain_.YDirection() ) );
    }
  }
  //! \return the OpenGL texture name.
  GLuint texture ( void ) const { return texture_; }
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
  GLfloat* texCoords ( const gp_Pnt2d& uv, const gp_Pnt& p ) const
  {
    static GLfloat coords[2];

    if ( material_ != 0 ) {
      GeomAPI_ProjectPointOnSurf project( p, reference_plane_ );
      Quantity_Parameter u, v;
      project.LowerDistanceParameters( u, v );
      coords[0] = s_resolution_ * u;
      coords[1] = t_resolution_ * v;
    }
    else {
      coords[0] = s_resolution_ * uv.X();
      coords[1] = t_resolution_ * uv.Y();
    }

    return coords;
  }

private:
  //! Handle to the plane surface.
  Handle( Geom_Plane ) plane_;
  //! Grain orientation.
  gp_Ax2 grain_;
  //! Reference to the material.
  const Material* material_;
  //! OpenGL texture name.
  GLuint texture_;
  //! Conversion factor from s texture coordinate to texture image resolution.
  GLfloat s_resolution_;
  //! Conversion factor from t texture coordinate to texture image resolution.
  GLfloat t_resolution_;
  //! The surface coordinates are projected on this plane to provide parametric
  //! coordinates.
  Handle( Geom_Plane ) reference_plane_;
};

/*!
 * The cylindrical texture function.
 */
class OCCylindricalTexture : public OCTextureFunction {
public:
  OCCylindricalTexture ( void ) {
    OCTextureFactory::instance()->addTextureFunction( HashCode(STANDARD_TYPE( Geom_CylindricalSurface ), IntegerLast() ), this );
  }
  /*!
   * Update the cylindrical surface properties used to in the computation of the
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
  void setProperties ( OpenGLBase* view, const Handle( Geom_Surface) surface,
		       double /*umin*/, double /*umax*/, double /*vmin*/, double /*vmax*/,
		       const gp_Ax2& grain, const Material* material )
  {
    cylinder_ = Handle(Geom_CylindricalSurface)::DownCast( surface );
    grain_ = grain;
    material_ = material;

    gp_Dir axis = cylinder_->Cylinder().Axis().Direction();

    Standard_Real x_dot_n =  fabs( grain_.XDirection() * axis );
    Standard_Real y_dot_n =  fabs( grain_.YDirection() * axis );
    Standard_Real z_dot_n =  fabs( grain_.Direction() * axis );

    texture_ = 0;
    s_resolution_ = t_resolution_ = 20. / 256.;

    if ( z_dot_n > x_dot_n && z_dot_n > y_dot_n ) {
      if ( material_ != 0 ) {
	QImage image = lC::lookupImage( material_->faceGrainFile() );
	if ( !image.isNull() ) {
	  s_resolution_ = 0.0254 * image.dotsPerMeterX() / image.width();
	  t_resolution_ = 0.0254 * image.dotsPerMeterY() / image.height();

	  texture_ = view->texture( material_->faceGrainFile() );
	}
	if ( s_resolution_ == 0. ) s_resolution_ = 20. / image.width();
	if ( t_resolution_ == 0. ) s_resolution_ = 20. / image.height();
      }
    }
    else {
      if ( material_ != 0 ) {
	QImage image = lC::lookupImage( material_->edgeGrainFile() );
	if ( !image.isNull() ) {
	  s_resolution_ = 0.0254 * image.dotsPerMeterX() / image.width();
	  t_resolution_ = 0.0254 * image.dotsPerMeterY() / image.height();

	  texture_ = view->texture( material_->edgeGrainFile() );
	}
	if ( s_resolution_ == 0. ) s_resolution_ = 20. / image.width();
	if ( t_resolution_ == 0. ) s_resolution_ = 20. / image.height();
      }
    }
  }
  //! \return the OpenGL texture name.
  GLuint texture ( void ) const { return texture_; }
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
  GLfloat* texCoords ( const gp_Pnt2d& uv, const gp_Pnt& /*p*/ ) const
  {
    static GLfloat coords[2];
    // Only need to turn the angle parameter into an arc length.
    coords[0] = s_resolution_ * uv.X() * cylinder_->Radius();
    coords[1] = t_resolution_ * uv.Y();
    return coords;
  }

private:
  //! Handle to the cylindrical surface.
  Handle( Geom_CylindricalSurface ) cylinder_;
  //! Grain orientation.
  gp_Ax2 grain_;
  //! Reference to the material.
  const Material* material_;
  //! OpenGL texture name.
  GLuint texture_;
  //! Conversion factor from s texture coordinate to texture image resolution.
  GLfloat s_resolution_;
  //! Conversion factor from t texture coordinate to texture image resolution.
  GLfloat t_resolution_;
};

/*!
 * The conical texture function.
 */
class OCConicalTexture : public OCTextureFunction {
public:
  OCConicalTexture ( void ) {
    OCTextureFactory::instance()->addTextureFunction( HashCode(STANDARD_TYPE( Geom_ConicalSurface ), IntegerLast() ), this );
  }
  /*!
   * Update the cylindrical surface properties used to in the computation of the
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
  void setProperties ( OpenGLBase* view, const Handle( Geom_Surface) surface,
		       double /*umin*/, double /*umax*/, double vmin, double vmax,
		       const gp_Ax2& grain, const Material* material )
  {
    cone_ = Handle(Geom_ConicalSurface)::DownCast( surface );
    grain_ = grain;
    material_ = material;

    // Compute the average radius over the used portion of the cone.
    Handle(Geom_Circle) circle = Handle(Geom_Circle)::DownCast( cone_->VIso( vmin));
    double radius0 = circle->Circ().Radius();
    circle = Handle(Geom_Circle)::DownCast( cone_->VIso( vmax ) );
    double radius1 = circle->Circ().Radius();

    radius_ = ( radius0 + radius1 ) / 2;

    gp_Dir axis = cone_->Cone().Axis().Direction();

    Standard_Real x_dot_n =  fabs( grain_.XDirection() * axis );
    Standard_Real y_dot_n =  fabs( grain_.YDirection() * axis );
    Standard_Real z_dot_n =  fabs( grain_.Direction() * axis );

    texture_ = 0;
    s_resolution_ = t_resolution_ = 20. / 256.;

    if ( z_dot_n > x_dot_n && z_dot_n > y_dot_n ) {
      if ( material_ != 0 ) {
	QImage image = lC::lookupImage( material_->faceGrainFile() );
	if ( !image.isNull() ) {
	  s_resolution_ = 0.0254 * image.dotsPerMeterX() / image.width();
	  t_resolution_ = 0.0254 * image.dotsPerMeterY() / image.height();

	  texture_ = view->texture( material_->faceGrainFile() );
	}
	if ( s_resolution_ == 0. ) s_resolution_ = 20. / image.width();
	if ( t_resolution_ == 0. ) s_resolution_ = 20. / image.height();
      }
    }
    else {
      if ( material_ != 0 ) {
	QImage image = lC::lookupImage( material_->edgeGrainFile() );
	if ( !image.isNull() ) {
	  s_resolution_ = 0.0254 * image.dotsPerMeterX() / image.width();
	  t_resolution_ = 0.0254 * image.dotsPerMeterY() / image.height();

	  texture_ = view->texture( material_->edgeGrainFile() );
	}
	if ( s_resolution_ == 0. ) s_resolution_ = 20. / image.width();
	if ( t_resolution_ == 0. ) s_resolution_ = 20. / image.height();
      }
    }
  }
  //! \return the OpenGL texture name.
  GLuint texture ( void ) const { return texture_; }
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
  GLfloat* texCoords ( const gp_Pnt2d& uv, const gp_Pnt& /*p*/ ) const
  {
    static GLfloat coords[2];
    // Only need to turn the angle parameter into an arc length.
    coords[0] = s_resolution_ * uv.X() * radius_;
    coords[1] = t_resolution_ * uv.Y();
    return coords;
  }

private:
  //! Handle to the cylindrical surface.
  Handle( Geom_ConicalSurface ) cone_;
  //! Grain orientation.
  gp_Ax2 grain_;
  //! Reference to the material.
  const Material* material_;
  //! OpenGL texture name.
  GLuint texture_;
  //! Average radius of the cone.
  double radius_;
  //! Conversion factor from s texture coordinate to texture image resolution.
  GLfloat s_resolution_;
  //! Conversion factor from t texture coordinate to texture image resolution.
  GLfloat t_resolution_;
};

/*!
 * The surface of revolution texture function.
 */
class OCSORTexture : public OCTextureFunction {
public:
  OCSORTexture ( void ) {
    OCTextureFactory::instance()->addTextureFunction( HashCode(STANDARD_TYPE( Geom_SurfaceOfRevolution ), IntegerLast() ), this );
  }
  /*!
   * Update the cylindrical surface properties used to in the computation of the
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
  void setProperties ( OpenGLBase* view, const Handle( Geom_Surface) surface,
		       double /*umin*/, double /*umax*/, double /*vmin*/, double /*vmax*/,
		       const gp_Ax2& grain, const Material* material )
  {
    sor_ = Handle(Geom_SurfaceOfRevolution)::DownCast( surface );
    grain_ = grain;
    material_ = material;

    // Use the bounding box to convert the parametric coordinates (u,v)
    // into the coordinates of an "equivalent" bounding cylinder.
    Bnd_Box bbox;
    GeomAdaptor_Surface Asor ( sor_ );
    BndLib_AddSurface::Add( Asor, gp::Resolution(), bbox );
    double x_min, y_min, z_min, x_max, y_max, z_max;
    bbox.Get( x_min, y_min, z_min, x_max, y_max, z_max );
    radius_ = x_max;
    length_ = z_max - z_min;

    gp_Dir axis = sor_->Axis().Direction();

    Standard_Real x_dot_n =  fabs( grain_.XDirection() * axis );
    Standard_Real y_dot_n =  fabs( grain_.YDirection() * axis );
    Standard_Real z_dot_n =  fabs( grain_.Direction() * axis );

    texture_ = 0;
    s_resolution_ = t_resolution_ = 20. / 256.;

    if ( z_dot_n > x_dot_n && z_dot_n > y_dot_n ) {
      if ( material_ != 0 ) {
	QImage image = lC::lookupImage( material_->faceGrainFile() );
	if ( !image.isNull() ) {
	  s_resolution_ = 0.0254 * image.dotsPerMeterX() / image.width();
	  t_resolution_ = 0.0254 * image.dotsPerMeterY() / image.height();

	  texture_ = view->texture( material_->faceGrainFile() );
	}
	if ( s_resolution_ == 0. ) s_resolution_ = 20. / image.width();
	if ( t_resolution_ == 0. ) s_resolution_ = 20. / image.height();
      }
    }
    else {
      if ( material_ != 0 ) {
	QImage image = lC::lookupImage( material_->edgeGrainFile() );
	if ( !image.isNull() ) {
	  s_resolution_ = 0.0254 * image.dotsPerMeterX() / image.width();
	  t_resolution_ = 0.0254 * image.dotsPerMeterY() / image.height();

	  texture_ = view->texture( material_->edgeGrainFile() );
	}
	if ( s_resolution_ == 0. ) s_resolution_ = 20. / image.width();
	if ( t_resolution_ == 0. ) s_resolution_ = 20. / image.height();
      }
    }
  }
  //! \return the OpenGL texture name.
  GLuint texture ( void ) const { return texture_; }
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
  GLfloat* texCoords ( const gp_Pnt2d& uv, const gp_Pnt& p ) const
  {
    static GLfloat coords[2];
    // Convert the parametric coordinates to a position on the
    // equivalent cylinder.
    coords[0] = s_resolution_ * uv.X() * radius_;
#if 0
    coords[1] = t_resolution_ * uv.Y() * length_;
#else
    // Maybe this will even out the texture coordinates since the
    // SOR's parameters aren't evenly spaced.
    double t = gp_Vec( grain_.Location(), p ) * grain_.Direction();
    coords[1] = t_resolution_ * t;
#endif
    return coords;
  }

private:
  //! Handle to the surface of revolution.
  Handle( Geom_SurfaceOfRevolution ) sor_;
  //! Grain orientation.
  gp_Ax2 grain_;
  //! Reference to the material.
  const Material* material_;
  //! OpenGL texture name.
  GLuint texture_;
  //! Effective length of the SOR.
  double length_;
  //! Average radius of the SOR.
  double radius_;
  //! Conversion factor from s texture coordinate to texture image resolution.
  GLfloat s_resolution_;
  //! Conversion factor from t texture coordinate to texture image resolution.
  GLfloat t_resolution_;
};

namespace {
  OCUnknownTexture unknown_texture;
  OCPlaneTexture plane_texture;
  OCCylindricalTexture cylindrical_texture;
  OCConicalTexture conical_texture;
  OCSORTexture sor_texture;
}

OCTextureFactory* OCTextureFactory::instance_ = 0;

OCTextureFactory::OCTextureFactory ( void )
{
  //  cout << "Constructing a texture factory" << endl;
}

OCTextureFactory* OCTextureFactory::instance ( void )
{
  if ( instance_ == 0 )
    instance_ = new OCTextureFactory;

  return instance_;
}

void OCTextureFactory::addTextureFunction( Standard_Integer type_hash_code,
					   OCTextureFunction* texture_function )
{
  texture_functions_[type_hash_code] = texture_function;
}

OCTextureFunction* OCTextureFactory::function ( OpenGLBase* view,
						const TopoDS_Face& face,
						const gp_Ax2& grain,
						const Material* material )
{
  // Some surfaces need to know the limits of the surface used by the
  // face.
  double umin, umax, vmin, vmax;
  BRepTools::UVBounds( face, umin, umax, vmin, vmax );

  Handle( Geom_Surface ) surface = BRep_Tool::Surface( face );

  std::map<Standard_Integer, OCTextureFunction*>::iterator tf =
    texture_functions_.find( HashCode(surface->DynamicType(), IntegerLast() ) );

  OCTextureFunction* texture_function;

  if ( tf != texture_functions_.end() )
    texture_function = (*tf).second;
  else
    texture_function = &unknown_texture;

  texture_function->setProperties( view, surface, umin, umax, vmin, vmax,
				   grain, material );

  return texture_function;
}

