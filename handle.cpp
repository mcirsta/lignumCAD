/*
 * handle.cpp
 *
 * Handle classes
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

#include "vectoralgebra.h"
#include "openglbase.h"
#include "handle.h"

namespace {
  // Some resources for drawing handle's

  const QString TEXTURENAME = "handle.png";

  const GLfloat light_position[] = { -1., 1., .25, 0. };
  const GLfloat light_ambiance[] = { .3, .3, .3, .25 };
  const GLfloat translucent_material[] = { 1., 0., 0., .5 };
  const GLfloat opaque_material[] = { 1., 1., 1., 1. };
};

namespace Space2D {

  // Well, this is definitely over-complicated. Should just use a pixmap

  void ResizeHandle::draw ( OpenGLBase* view, lC::Render::Mode mode,
		       const Point& position )
  {
    if ( mode == lC::Render::INVISIBLE ) return;

    // Compute the edge length in the current window scale.
    GLfloat side = view->handleSize() * view->scale();
    GLfloat bevel = 2. * side / 3.;

    GLuint gl_texture_name = view->texture( TEXTURENAME );

    // The handle appears at a fixed size regardless of scale. Since
    // it is drawn in the scale coordinate system, it has to change
    // physical size as the scale changes. We let OpenGL compute the
    // texture coordinates (which is the same computation we'd have to
    // do ourselves anyway). So, it's a trade-off between lots of
    // calls to glTexCoord versus recomputing the texture coordinates
    // over and over.

    GLfloat s_coefficients[] = { 0.5/side, 0., 0., 1./2. };
    GLfloat t_coefficients[] = { 0., 0.5/side, 0., 1./2. };

    glEnable( GL_LIGHTING );
    glEnable( GL_LIGHT0 );
    glLightfv( GL_LIGHT0, GL_POSITION, light_position );
    glLightfv( GL_LIGHT0, GL_AMBIENT, light_ambiance );

    if ( mode == lC::Render::HIGHLIGHTED ) {
      glEnable( GL_BLEND );
      glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE,translucent_material);
    }
    else {
      glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, opaque_material );
    }

    glEnable( GL_TEXTURE_2D );
    glEnable( GL_TEXTURE_GEN_S );
    glEnable( GL_TEXTURE_GEN_T );

    glTexGeni( GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR );
    glTexGenfv( GL_S, GL_OBJECT_PLANE, s_coefficients );
    glTexGeni( GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR );
    glTexGenfv( GL_T, GL_OBJECT_PLANE, t_coefficients );

    glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );

    glBindTexture( GL_TEXTURE_2D, gl_texture_name );

    glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

    glPushMatrix();

    glTranslated( position.p_[X], position.p_[Y], 0. );

    // Note: OpenGL lighting doesn't depend on the facet's real surface
    // normal. It only cares about what it's told the normal is. Therefore,
    // a handle is rendered entirely in the XY plane and we just set
    // the OpenGL normal to point in the appropriate direction.

    glBegin( GL_QUADS );

    // Bottom bevel
    glNormal3d( 0., -M_SQRT1_2, M_SQRT1_2 );

    glVertex2f( -side, -side );
    glVertex2f( side, -side );
    glVertex2f( bevel, -bevel );
    glVertex2f( -bevel, -bevel );

    // Right bevel
    glNormal3d( M_SQRT1_2, 0., M_SQRT1_2 );

    glVertex2f( side, -side );
    glVertex2f( side, side );
    glVertex2f( bevel, bevel );
    glVertex2f( bevel, -bevel );

    // Top bevel
    glNormal3d( 0., M_SQRT1_2, M_SQRT1_2 );

    glVertex2f( side, side );
    glVertex2f( -side, side );
    glVertex2f( -bevel, bevel );
    glVertex2f( bevel, bevel );

    // Left bevel
    glNormal3d( -M_SQRT1_2, 0., M_SQRT1_2 );

    glVertex2f( -side, side );
    glVertex2f( -side, -side );
    glVertex2f( -bevel, -bevel );
    glVertex2f( -bevel, bevel );

    // Center facet
    glNormal3d( 0., 0., 1. );

    glVertex2f( -bevel, -bevel );
    glVertex2f( bevel, -bevel );
    glVertex2f( bevel, bevel );
    glVertex2f( -bevel, bevel );

    glEnd();

    glPopMatrix();

    glDisable( GL_TEXTURE_GEN_S );
    glDisable( GL_TEXTURE_GEN_T );
  
    glDisable( GL_LIGHTING );
    glDisable( GL_TEXTURE_2D );

    if ( mode == lC::Render::HIGHLIGHTED ) {
      glDisable( GL_BLEND );
    }
  }

  void ResizeHandle::select ( OpenGLBase* view, const Point& position )
  {
    GLfloat side = OpenGLGlobals::instance()->handleSize() * view->scale();

    glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

    glPushMatrix();

    glTranslated( position.p_[X], position.p_[Y], 0. );

    glBegin( GL_QUADS );

    glVertex2f( -side, -side );
    glVertex2f( side, -side );
    glVertex2f( side, side );
    glVertex2f( -side, side );

    glEnd();

    glPopMatrix();
  }
} // End of Space2D namespace

namespace Space3D {

  GLfloat ResizeHandle::vertices_[24][3];

  // These are indices into the vertices_ array which pick out the
  // vertices for each quadrilateral face.

  const GLubyte ResizeHandle::quads_[18][4] = {
    {  0,  1,  2,  3 }, // Left face
    { 11, 10,  1,  0 }, // Left-Front bevel
    {  8,  9, 10, 11 }, // Front face
    {  7,  6,  9,  8 }, // Front-Right bevel
    {  4,  5,  6,  7 }, // Right face
    { 15, 14,  5,  4 }, // Right-Back bevel
    { 12, 13, 14, 15 }, // Back face
    {  3,  2, 13, 12 }, // Back-Left bevel
    { 16, 17, 18, 19 }, // Bottom face
    { 16, 19,  0,  3 }, // Bottom-Left bevel
    { 19, 18,  8, 11 }, // Bottom-Front bevel
    { 18, 17,  4,  7 }, // Bottom-Right bevel
    { 17, 16, 12, 15 }, // Bottom-Back bevel
    { 20, 21, 22, 23 }, // Top face
    { 22, 21,  2,  1 }, // Top-Left bevel
    { 23, 22, 10,  9 }, // Top-Front bevel
    { 20, 23,  6,  5 }, // Top-Right bevel
    { 21, 20, 14, 13 }, // Top-Back bevel
  };

  const GLubyte ResizeHandle::tris_[8][3] = {
    {  0, 19, 11 }, // Left-Front-Bottom bevel
    {  8, 18,  7 }, // Front-Right-Bottom bevel
    {  4, 17, 15 }, // Right-Back-Bottom bevel
    { 12, 16,  3 }, // Back-Left-Bottom bevel
    {  1, 10, 22 }, // Left-Front-Top bevel
    {  9,  6, 23 }, // Front-Right-Top bevel
    {  5, 14, 20 }, // Right-Back-Top bevel
    { 13,  2, 21 }, // Back-Left-Top bevel
  };

  // The normals for each face

  const GLfloat M_SQRT1_3 = 0.577350269;
  const GLfloat M_SQRT1_6 = 0.40824829;
  const GLfloat M_SQRT2_3 = 0.81649658;

  const GLfloat ResizeHandle::normals_[30][3] = {
    { -1,          0,          0 },         // Left face
    { -M_SQRT1_2, -M_SQRT1_2,  0 },         // Left-Front bevel
    {  0,         -1,          0 },         // Front face
    {  M_SQRT1_2, -M_SQRT1_2,  0 },         // Front-Right bevel
    {  1,          0,          0 },         // Right face
    {  M_SQRT1_2,  M_SQRT1_2,  0 },         // Right-Back bevel
    {  0,          1,          0 },         // Back face
    { -M_SQRT1_2,  M_SQRT1_2,  0 },         // Back-Left bevel
    {  0,          0,         -1 },         // Bottom face
    { -M_SQRT1_2,  0,         -M_SQRT1_2 }, // Bottom-Left bevel
    {  0,         -M_SQRT1_2, -M_SQRT1_2 }, // Bottom-Front bevel
    {  M_SQRT1_2,  0,         -M_SQRT1_2 }, // Bottom-Right bevel
    {  0,          M_SQRT1_2, -M_SQRT1_2 }, // Bottom-Back bevel
    {  0,          0,          1 },	    // Top face
    { -M_SQRT1_2,  0,          M_SQRT1_2 }, // Top-Left bevel
    {  0,         -M_SQRT1_2,  M_SQRT1_2 }, // Top-Front bevel
    {  M_SQRT1_2,  0,          M_SQRT1_2 }, // Top-Right bevel
    {  0,          M_SQRT1_2,  M_SQRT1_2 }, // Top-Back bevel
    { -M_SQRT1_3, -M_SQRT1_3, -M_SQRT1_3 }, // Left-Front-Bottom bevel
    {  M_SQRT1_3, -M_SQRT1_3, -M_SQRT1_3 }, // Front-Right-Bottom bevel
    {  M_SQRT1_3,  M_SQRT1_3, -M_SQRT1_3 }, // Right-Back-Bottom bevel
    { -M_SQRT1_3,  M_SQRT1_3, -M_SQRT1_3 }, // Back-Left-Bottom bevel
    { -M_SQRT1_3, -M_SQRT1_3,  M_SQRT1_3 }, // Left-Front-Top bevel
    {  M_SQRT1_3, -M_SQRT1_3,  M_SQRT1_3 }, // Front-Right-Top bevel
    {  M_SQRT1_3,  M_SQRT1_3,  M_SQRT1_3 }, // Right-Back-Top bevel
    { -M_SQRT1_3,  M_SQRT1_3,  M_SQRT1_3 }, // Back-Left-Top bevel
    { -M_SQRT1_6, -M_SQRT1_6,  M_SQRT2_3 }, // Extra Left-Front-Bottom direction
    {  M_SQRT1_6, -M_SQRT1_6,  M_SQRT2_3 }, // Extra Front-Right-Bottom direction
    {  M_SQRT1_6,  M_SQRT1_6,  M_SQRT2_3 }, // Extra Right-Back-Bottom direction
    { -M_SQRT1_6,  M_SQRT1_6,  M_SQRT2_3 }, // Extra Back-Left-Bottom direction
  };

  const GLubyte ResizeHandle::comp_angles_[26][2] = {
    {  2, 13 }, // Left face
    {  3, 13 }, // Left-Front bevel
    {  4, 13 }, // Front face
    {  5, 13 }, // Front-Right bevel
    {  6, 13 }, // Right face
    {  7, 13 }, // Right-Back bevel
    {  0, 13 }, // Back face
    {  1, 13 }, // Back-Left bevel
    {  4,  2 }, // Bottom face
    {  2, 14 }, // Bottom-Left bevel
    {  4, 15 }, // Bottom-Front bevel
    {  6, 16 }, // Bottom-Right bevel
    {  0, 17 }, // Bottom-Back bevel
    {  4,  6 }, // Top face
    {  2, 16 }, // Top-Left bevel
    {  4, 17 }, // Top-Front bevel
    {  6, 14 }, // Top-Right bevel
    {  0, 15 }, // Top-Back bevel
    {  3, 26 }, // Left-Front-Bottom bevel
    {  5, 27 }, // Front-Right-Bottom bevel
    {  7, 28 }, // Right-Back-Bottom bevel
    {  1, 29 }, // Back-Left-Bottom bevel
    {  3, 28 }, // Left-Front-Top bevel
    {  5, 29 }, // Front-Right-Top bevel
    {  7, 26 }, // Right-Back-Top bevel
    {  1, 27 }, // Back-Left-Top bevel
  };

  // Well, this is definitely over-complicated. Should just use a pixmap

  void ResizeHandle::draw ( OpenGLBase* view, lC::Render::Mode mode,
		       const Point& position )
  {
    if ( mode == lC::Render::INVISIBLE ) return;

    // Compute the edge length in the current window scale.
    GLfloat side = view->handleSize() * view->scale();
    GLfloat bevel = 2. * side / 3.;

    // Left and right face vertices

    vertices_[0][X] = -side;    vertices_[4][X] = side; 
    vertices_[0][Y] = -bevel;   vertices_[4][Y] = bevel;
    vertices_[0][Z] = -bevel;   vertices_[4][Z] = -bevel;
    vertices_[1][X] = -side;    vertices_[5][X] = side;
    vertices_[1][Y] = -bevel;   vertices_[5][Y] = bevel;
    vertices_[1][Z] = bevel;    vertices_[5][Z] = bevel;
    vertices_[2][X] = -side;    vertices_[6][X] = side;
    vertices_[2][Y] = bevel;    vertices_[6][Y] = -bevel;
    vertices_[2][Z] = bevel;    vertices_[6][Z] = bevel;
    vertices_[3][X] = -side;    vertices_[7][X] = side;
    vertices_[3][Y] = bevel;    vertices_[7][Y] = -bevel;
    vertices_[3][Z] = -bevel;   vertices_[7][Z] = -bevel;

    // Front and back face vertices

    vertices_[8][X] = bevel;    vertices_[12][X] = -bevel;
    vertices_[8][Y] = -side;    vertices_[12][Y] = side;
    vertices_[8][Z] = -bevel;   vertices_[12][Z] = -bevel;
    vertices_[9][X] = bevel;    vertices_[13][X] = -bevel;
    vertices_[9][Y] = -side;    vertices_[13][Y] = side;
    vertices_[9][Z] = bevel;    vertices_[13][Z] = bevel;
    vertices_[10][X] = -bevel;  vertices_[14][X] = bevel;
    vertices_[10][Y] = -side;   vertices_[14][Y] = side;
    vertices_[10][Z] = bevel;   vertices_[14][Z] = bevel;
    vertices_[11][X] = -bevel;  vertices_[15][X] = bevel;
    vertices_[11][Y] = -side;   vertices_[15][Y] = side;
    vertices_[11][Z] = -bevel;  vertices_[15][Z] = -bevel;

    // Bottom and top face vertices

    vertices_[16][X] = -bevel;  vertices_[20][X] = bevel;
    vertices_[16][Y] = bevel;   vertices_[20][Y] = bevel;
    vertices_[16][Z] = -side;   vertices_[20][Z] = side;
    vertices_[17][X] = bevel;   vertices_[21][X] = -bevel;
    vertices_[17][Y] = bevel;   vertices_[21][Y] = bevel;
    vertices_[17][Z] = -side;   vertices_[21][Z] = side;
    vertices_[18][X] = bevel;   vertices_[22][X] = -bevel;
    vertices_[18][Y] = -bevel;  vertices_[22][Y] = -bevel;
    vertices_[18][Z] = -side;   vertices_[22][Z] = side;
    vertices_[19][X] = -bevel;  vertices_[23][X] = bevel;
    vertices_[19][Y] = -bevel;  vertices_[23][Y] = -bevel;
    vertices_[19][Z] = -side;   vertices_[23][Z] = side;

    GLuint gl_texture_name = view->texture( TEXTURENAME );

    // The handle appears at a fixed size regardless of scale. Since
    // it is drawn in the scale coordinate system, it has to change
    // physical size as the scale changes. We let OpenGL compute the
    // texture coordinates (which is the same computation we'd have to
    // do ourselves anyway). So, it's a trade-off between lots of
    // calls to glTexCoord versus recomputing the texture coordinates
    // over and over.

    glEnable( GL_LIGHTING );
    glEnable( GL_LIGHT0 );

    if ( mode == lC::Render::HIGHLIGHTED ) {
      glEnable( GL_BLEND );
      glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE,translucent_material);
    }
    else {
      glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, opaque_material );
    }

    glEnable( GL_TEXTURE_2D );
    glEnable( GL_TEXTURE_GEN_S );
    glEnable( GL_TEXTURE_GEN_T );

    glTexGeni( GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR );
    glTexGeni( GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR );

    glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );

    glBindTexture( GL_TEXTURE_2D, gl_texture_name );

    glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

    glPushMatrix();

    glTranslated( position.p_[X], position.p_[Y], position.p_[Z] );

    glEnableClientState( GL_VERTEX_ARRAY );
    glVertexPointer( 3, GL_FLOAT, 0, vertices_ );

    for ( int i = 0; i < 18; i++ ) {
      GLfloat s_coefficients[4], t_coefficients[4];
      s_coefficients[X] = normals_[comp_angles_[i][0]][X];
      s_coefficients[Y] = normals_[comp_angles_[i][0]][Y];
      s_coefficients[Z] = normals_[comp_angles_[i][0]][Z];
      s_coefficients[W] = 0;
      glTexGenfv( GL_S, GL_OBJECT_PLANE, s_coefficients );

      t_coefficients[X] = normals_[comp_angles_[i][1]][X];
      t_coefficients[Y] = normals_[comp_angles_[i][1]][Y];
      t_coefficients[Z] = normals_[comp_angles_[i][1]][Z];
      t_coefficients[W] = 0;
      glTexGenfv( GL_T, GL_OBJECT_PLANE, t_coefficients );

      glNormal3fv( normals_[i] );
      glDrawElements( GL_QUADS, 4, GL_UNSIGNED_BYTE, quads_[i] );
    }

    for ( int i = 0; i < 8; i++ ) {
      GLfloat s_coefficients[4], t_coefficients[4];
      s_coefficients[X] = normals_[comp_angles_[i+18][0]][X];
      s_coefficients[Y] = normals_[comp_angles_[i+18][0]][Y];
      s_coefficients[Z] = normals_[comp_angles_[i+18][0]][Z];
      s_coefficients[W] = 0;
      glTexGenfv( GL_S, GL_OBJECT_PLANE, s_coefficients );

      t_coefficients[X] = normals_[comp_angles_[i+18][1]][X];
      t_coefficients[Y] = normals_[comp_angles_[i+18][1]][Y];
      t_coefficients[Z] = normals_[comp_angles_[i+18][1]][Z];
      t_coefficients[W] = 0;
      glTexGenfv( GL_T, GL_OBJECT_PLANE, t_coefficients );

      glNormal3fv( normals_[i+18] );
      glDrawElements( GL_TRIANGLES, 3, GL_UNSIGNED_BYTE, tris_[i] );
    }

    glDisableClientState( GL_VERTEX_ARRAY );

    glPopMatrix();

    glDisable( GL_TEXTURE_GEN_S );
    glDisable( GL_TEXTURE_GEN_T );
    glDisable( GL_TEXTURE_2D );
    glDisable( GL_LIGHTING );

    if ( mode == lC::Render::HIGHLIGHTED ) {
      glDisable( GL_BLEND );
    }
  }

  void ResizeHandle::select ( OpenGLBase* /*view*/, const Point& position )
  {
    glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

    glPushMatrix();

    glTranslated( position.p_[X], position.p_[Y], position.p_[Z] );

    glEnableClientState( GL_VERTEX_ARRAY );
    glVertexPointer( 3, GL_FLOAT, 0, vertices_ );

    for ( int i = 0; i < 18; i++ )
      glDrawElements( GL_QUADS, 4, GL_UNSIGNED_BYTE, quads_[i] );

    for ( int i = 0; i < 8; i++ )
      glDrawElements( GL_TRIANGLES, 3, GL_UNSIGNED_BYTE, tris_[i] );

    glDisableClientState( GL_VERTEX_ARRAY );

    glPopMatrix();
  }
} // End of Space3D namespace
