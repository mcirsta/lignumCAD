/*
 * openglbase.cpp
 *
 * OpenGLBase class: the base class of all the OpenGL widgets.
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
#include <qapplication.h>
#include <qimage.h>
#include <qpaintdevicemetrics.h>
#include <qregexp.h>

#include "OGLFT.h"

#include "systemdependencies.h"
#include "vectoralgebra.h"
#include "openglbase.h"

using namespace Space3D;

extern "C" {
  // This is an internally compiled font which contains some symbols
  // which are out of the ordinary (UNICODE - CENTERLINE SYMBOL) and
  // the characters used in making fractions (in the UNICODE private
  // range 0xE000). The font is encoded in TrueType format and stored
  // in this array.
  extern uchar lCSymbols_ttf[];
  // The size of the symbol font array.
  extern int lCSymbols_ttf_size;
};

OpenGLBase::OpenGLBase ( QWidget* parent, const char* name,
			 QGLWidget* share_widget )
  : QGLWidget( parent, name, share_widget ), background_texture_( 0 )
{}

OpenGLBase::OpenGLBase ( const QGLFormat& format, QWidget* parent, const char* name)
  : QGLWidget( format, parent, name )
{}

OpenGLBase::~OpenGLBase ( void )
{
  makeCurrent();
  clearFontCache();
}

Space3D::Vector OpenGLBase::viewNormal ( void ) const
{
  return Space3D::Vector(0,0,1);
}
// Lazily construct and cache a font corresponding to the given
// attributes (since the OGLFT display list caches are cleared on
// almost all property changes).

OGLFT::Face* OpenGLBase::font ( const FaceData& requested_face )
{
  FaceData actual_face = requested_face;

  if ( actual_face.font_.isEmpty() )
    actual_face.font_ = QWidget::font().toString();

  QMap< FaceData, OGLFT::Face* >::const_iterator face = faces_.find( actual_face );

  if ( face != faces_.end() )
      return face.data();

  QString file;
  double point_size;

  // While this could fail, I think eventually I'll have to make it always
  // succeed...
  System::findFontFile( actual_face.font_, file, point_size );

  // The resolution is passed as 0 to agree with what Qt's font requestor
  // passes to Xft so the OpenGL font appears to be same size as that
  // shown in the requestor.

  OGLFT::Face* base_face = new OGLFT::Monochrome( file, point_size, 0 );

  faces_.insert( actual_face, base_face );

  // Augment the base face with some extra symbols. Sorry they won't match
  // the style of the user selection for now.

  FT_Library library = OGLFT::Library::instance();
  FT_Face symbol_face;
  FT_New_Memory_Face( library, lCSymbols_ttf, lCSymbols_ttf_size, 0,
		      &symbol_face );

  faces_[ actual_face ]->addAuxiliaryFace( symbol_face );
  faces_[ actual_face ]->setStringRotation( actual_face.orientation_ );
  faces_[ actual_face ]->setForegroundColor( actual_face.color_ );

  switch ( actual_face.horizontal_justification_ ) {
  case lC::LEFT:
    faces_[ actual_face ]->setHorizontalJustification( OGLFT::Face::LEFT );
    break;
  case lC::ORIGIN:
    faces_[ actual_face ]->setHorizontalJustification(OGLFT::Face::ORIGIN);
    break;
  case lC::CENTER:
    faces_[ actual_face ]->setHorizontalJustification(OGLFT::Face::CENTER);
    break;
  case lC::RIGHT:
    faces_[ actual_face ]->setHorizontalJustification( OGLFT::Face::RIGHT);
    break;
  }

  switch ( actual_face.vertical_justification_ ) {
  case lC::BOTTOM:
    faces_[ actual_face ]->setVerticalJustification( OGLFT::Face::BOTTOM );
    break;
  case lC::BASELINE:
    faces_[ actual_face ]->setVerticalJustification(OGLFT::Face::BASELINE);
    break;
  case lC::MIDDLE:
    faces_[ actual_face ]->setVerticalJustification( OGLFT::Face::MIDDLE );
    break;
  case lC::TOP:
    faces_[ actual_face ]->setVerticalJustification( OGLFT::Face::TOP );
  }
    
  return faces_[ actual_face ];
}

// Project a point on the screen into 2D model coordinates.

Space2D::Point OpenGLBase::unproject2D ( const QPoint& p ) const
{
  // This is the default, relatively expensive, implmentation.
  GLint viewport[4];
  GLdouble projection[16];
  GLdouble modelview[16];
  GLdouble x, y, z;
  glGetIntegerv( GL_VIEWPORT, viewport );
  glGetDoublev( GL_PROJECTION_MATRIX, projection );
  glGetDoublev( GL_MODELVIEW_MATRIX, modelview );
  gluUnProject( p.x(), viewport[3] - p.y(), 0,
		modelview, projection, viewport, &x, &y, &z );

  return Space2D::Point( x, y );
}

// Project a point on the screen into 3D model coordinates.

Space3D::Point OpenGLBase::unproject3D ( const QPoint& p ) const
{
  // This is the default, relatively expensive, implmentation.
  GLint viewport[4];
  GLdouble projection[16];
  GLdouble modelview[16];
  GLdouble x, y, z;
  glGetIntegerv( GL_VIEWPORT, viewport );
  glGetDoublev( GL_PROJECTION_MATRIX, projection );
  glGetDoublev( GL_MODELVIEW_MATRIX, modelview );
  gluUnProject( p.x(), viewport[3] - p.y(), 0,
		modelview, projection, viewport, &x, &y, &z );

  return Space3D::Point( x, y, z );
}

// Project a point in (sub)pixel coordinates into 3D model coordinates.

Space3D::Point OpenGLBase::unproject3D ( const Space2D::Point& p ) const
{
  // This is the default, relatively expensive, implmentation.
  GLint viewport[4];
  GLdouble projection[16];
  GLdouble modelview[16];
  GLdouble x, y, z;
  glGetIntegerv( GL_VIEWPORT, viewport );
  glGetDoublev( GL_PROJECTION_MATRIX, projection );
  glGetDoublev( GL_MODELVIEW_MATRIX, modelview );
  gluUnProject( p[X], p[Y], 0,
		modelview, projection, viewport, &x, &y, &z );

  return Space3D::Point( x, y, z );
}

// Lazily construct a texture object for this name.

GLuint OpenGLBase::texture ( const QString& texture_name )
{
  QMap< QString, GLuint >::const_iterator t = textures_.find( texture_name );

  if ( t != textures_.end() )
      return t.data();

  // Note: texture images currently have to be compiled into
  // the program...
  // Compensate for the fact that OpenGL expects images defined
  // bottom-up instead of top-down (as is usually the case).
  QImage image = lC::lookupImage( texture_name ).mirror();

  if ( image.isNull() ) return 0;

  // Scale such that each dimension is a power of two.
  int width = nearestPowerCeil( image.width() );
  int height = nearestPowerCeil( image.height() );

  if ( width != image.width() || height != image.height() ) {
    image = image.smoothScale( width, height );
  }

  GLuint gl_texture_name;

  glGenTextures( 1, &gl_texture_name );

  glBindTexture( GL_TEXTURE_2D, gl_texture_name );

  glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, image.width(), image.height(),
		0, GL_BGRA, GL_UNSIGNED_BYTE, image.bits() );

  textures_.insert( texture_name, gl_texture_name );

  return gl_texture_name;
}

void OpenGLBase::updateBackgroundColor ( const QColor& color )
{
  qglClearColor( color );

  updateGL();
}

void OpenGLBase::updateFontFile ( const QString& /*font_file*/ )
{
  // Here we take the easy way out and just let the face cache be
  // refilled lazily rather than examining each face to see if it is
  // affected by this change (e.g., the fonts used in Annotations are
  // not affected by the global OpenGL view face).

  clearFontCache();

  updateGL();
}

void OpenGLBase::updateFontPointSize ( double /*point_size*/ )
{
  // Here we take the easy way out and just let the face cache be
  // refilled lazily rather than examining each face to see if
  // it is affected by this change.

  clearFontCache();

  updateGL();
}

// Erase all the OGLFT Face object's we've cached.

void OpenGLBase::clearFontCache ( void )
{
  QMap< FaceData, OGLFT::Face* >::iterator f;

  for ( f = faces_.begin(); f != faces_.end(); ++f )
    delete f.data();

  faces_.clear();
}

// Find the smallest power of 2 which is greater than the input number.

uint OpenGLBase::nearestPowerCeil ( unsigned int a )
{
  uint b = a;
  uint c = 1;

  if ( a == 0 ) return 1;

  // Take the log-2 of a
  for ( ; ; ) {
    if ( b == 1 )
      break;
      
    else if ( b == 3 ) {
      c *= 4;
      break;
    }

    b >>= 1;
    c *= 2;
  }
  // If it's too small, raise it another power
  if ( c < a ) c *= 2;

  return c;
}

// Generate texture for the background.

GLuint OpenGLBase::createBackgroundTexture ( const QString& file,
					     const QColor& color0,
					     const QColor& color1 )
{
  // The supplied image is scaled to an image size which is a power of
  // 2, reduced to grayscale and then color0 and color1 are blended in
  // the proportion given by each pixel. Uses the standard OpenGL
  // pixel transfer functions to do the blending.
  QImage image = lC::lookupImage( file ).mirror();

  // Scale such that each dimension is a power of two.
  int width = nearestPowerCeil( image.width() );
  int height = nearestPowerCeil( image.height() );

  if ( width != image.width() || height != image.height() ) {
    image = image.smoothScale( width, height );
  }

  // Reduce the image to gray scale (conversion factors from ppmtopgm)
  background_image_.create( width, height, 8, 256 );

  for ( int j = 0; j < height; j++ )
    for ( int i = 0; i < width; i++ ) {
      QRgb pixel = image.pixel( i, j );
      background_image_.setPixel( i, j, (uint)( .299 * qRed( pixel ) +
						.587 * qGreen( pixel ) +
						.114 * qBlue( pixel ) ) );
    }

  if ( background_texture_ == 0 )
    glGenTextures( 1, &background_texture_ );

  glBindTexture( GL_TEXTURE_2D, background_texture_ );

  glPushAttrib( GL_PIXEL_MODE_BIT );

  glPixelTransferf( GL_RED_SCALE, ( color1.red() - color0.red() ) / 255. );
  glPixelTransferf( GL_GREEN_SCALE, ( color1.green() - color0.green() ) / 255. );
  glPixelTransferf( GL_BLUE_SCALE, ( color1.blue() - color0.blue() ) / 255. );
  glPixelTransferf( GL_RED_BIAS, color0.red() / 255. );
  glPixelTransferf( GL_GREEN_BIAS, color0.green() / 255. );
  glPixelTransferf( GL_BLUE_BIAS,  color0.blue() / 255. );

  glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB,
		background_image_.width(), background_image_.height(),
		0, GL_LUMINANCE, GL_UNSIGNED_BYTE,
		background_image_.bits() );

  glPopAttrib();

  return background_texture_;
}

// The OpenGL Attribute structure
OpenGLAttributes::OpenGLAttributes ( void )
  : color_scheme_( qApp->translate( "", "Default (solid)" ) )
{}

void OpenGLAttributes::clear ( OpenGLBase* view, bool clear_depth_buffer ) const
{
  switch ( color_scheme_.backgroundStyle() ) {
  case lC::Background::SOLID:
    {
      // Can we depend on the glClearColor having been set properly?...Not now.
      
      view->qglClearColor( color_scheme_.backgroundColor() );

      GLuint clear_bits = GL_COLOR_BUFFER_BIT;

      if ( clear_depth_buffer ) clear_bits |= GL_DEPTH_BUFFER_BIT;

      glClear( clear_bits );
    }
  break;
  case lC::Background::GRADIENT:
    {
      // Draw a big quadrilateral with smooth shading.

      if ( clear_depth_buffer )
	glClear( GL_DEPTH_BUFFER_BIT );

      QPaintDeviceMetrics pdm( view );

      // Set up a projection/modelview in which vertex coordinates
      // correspond to pixels

      glMatrixMode( GL_PROJECTION );
      glPushMatrix();
      glLoadIdentity();
      glOrtho( 0, pdm.width(), 0, pdm.height(), 1., -1. );
      glMatrixMode( GL_MODELVIEW );
      glPushMatrix();
      glLoadIdentity();

      glPushAttrib( GL_LIGHTING_BIT | GL_DEPTH_BUFFER_BIT );

      glShadeModel( GL_SMOOTH );
      glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

      // Disable writing to the depth buffer. This is just going to
      // render in the color buffer.

      glDisable( GL_DEPTH_TEST );
      glDepthMask( GL_FALSE );

      glBegin( GL_QUADS );

      view->qglColor( color_scheme_.gradientColor() );
      glVertex2i( 0, 0 );
      glVertex2i( pdm.width(), 0 );

      view->qglColor( color_scheme_.backgroundColor() );
      glVertex2i( pdm.width(), pdm.height() );
      glVertex2i( 0, pdm.height() );
  
      glEnd();

      glPopAttrib();

      glPopMatrix();		// restore the modelview matrix
      glMatrixMode( GL_PROJECTION );
      glPopMatrix();
      glMatrixMode( GL_MODELVIEW );
    }
  break;
  case lC::Background::PATTERN:
    {
      // Draw a big, texture mapped quadrilateral.

      if ( clear_depth_buffer )
	glClear( GL_DEPTH_BUFFER_BIT );

      GLuint gl_texture_name = view->backgroundTexture();
#if 1
      if ( gl_texture_name == 0 )
	gl_texture_name =
	  view->createBackgroundTexture( color_scheme_.patternFile(),
					 color_scheme_.backgroundColor(),
					 color_scheme_.gradientColor() );
#endif
      glEnable( GL_TEXTURE_2D );

      glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );
      glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
      glBindTexture( GL_TEXTURE_2D, gl_texture_name );

      // Compute the texture coordinates based on the size of the image
      // and the size of the viewport.

      QImage image = view->backgroundImage();

      QPaintDeviceMetrics pdm( view );
      double s = (double)pdm.width() / image.width();
      double t = (double)pdm.height() / image.height();

      // Change the projection matrix so that the texture is the correct size,
      // i.e., there is a one-to-one correspondence between pixels and texels.
      glMatrixMode( GL_PROJECTION );
      glPushMatrix();
      glLoadIdentity();
      glOrtho( 0, pdm.width(), 0, pdm.height(), 1., -1. );
      glMatrixMode( GL_MODELVIEW );
      glPushMatrix();
      glLoadIdentity();

      glPushAttrib( GL_DEPTH_BUFFER_BIT );

      // Disable writing to the depth buffer. This is just going to
      // render in the color buffer.

      glDisable( GL_DEPTH_TEST );
      glDepthMask( GL_FALSE );

      glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

      glBegin( GL_QUADS );

      glTexCoord2f( 0, 0 );
      glVertex2i( 0, 0 );
      glTexCoord2f( s, 0 );
      glVertex2i( pdm.width(), 0 );
      glTexCoord2f( s, t );
      glVertex2i( pdm.width(), pdm.height() );
      glTexCoord2f( 0, t );
      glVertex2i( 0, pdm.height() );
      
      glEnd();

      glDisable( GL_TEXTURE_2D );

      glPopAttrib();

      glPopMatrix();		// restore the modelview matrix
      glMatrixMode( GL_PROJECTION );
      glPopMatrix();
      glMatrixMode( GL_MODELVIEW );
    }
  break;
  }
}

// Single instance of the OpenGL global database
OpenGLGlobals* OpenGLGlobals::opengl_globals_ = 0;

OpenGLGlobals::OpenGLGlobals ( void )
  : QObject( 0, "openGLGlobals" ),
    dimension_minimums_( lC::DEFAULT_FONT, lC::OPEN,
			 lC::MINIMUM_ARROW_HEAD_LENGTH,
			 lC::MINIMUM_ARROW_HEAD_WIDTH_RATIO,
			 lC::MINIMUM_DIMENSION_CLEARANCE,
			 lC::MINIMUM_DIMENSION_LINE_THICKNESS,
			 lC::MINIMUM_EXTENSION_LINE_OFFSET ),
    dimension_maximums_( lC::DEFAULT_FONT, lC::FILLED,
			 lC::MAXIMUM_ARROW_HEAD_LENGTH,
			 lC::MAXIMUM_ARROW_HEAD_WIDTH_RATIO,
			 lC::MAXIMUM_DIMENSION_CLEARANCE,
			 lC::MAXIMUM_DIMENSION_LINE_THICKNESS,
			 lC::MAXIMUM_EXTENSION_LINE_OFFSET ),
    handle_minimums_( lC::MINIMUM_HANDLE_SIZE ),
    handle_maximums_( lC::MAXIMUM_HANDLE_SIZE )
{
  predefined_scheme_ = true;

  // Make a list of our default display schemata.
  color_schemes_.
    append( new PageColorScheme( qApp->translate( "", "Default (solid)" ) ) );

  color_schemes_.
    append( new PageColorScheme( qApp->translate( "", "Default (gradient)" ),
				 lC::DEFAULT_GEOMETRY_COLOR,
				 lC::DEFAULT_ANNOTATION_COLOR,
				 lC::DEFAULT_GRID_COLOR,
				 lC::DEFAULT_CONSTRAINT_PRIMARY_COLOR,
				 lC::DEFAULT_CONSTRAINT_SECONDARY_COLOR,
				 lC::Background::GRADIENT ) );

  color_schemes_.
    append( new PageColorScheme( qApp->translate( "", "Default (paper)" ),
				 lC::DEFAULT_GEOMETRY_COLOR,
				 lC::DEFAULT_ANNOTATION_COLOR,
				 lC::DEFAULT_GRID_COLOR,
				 lC::DEFAULT_CONSTRAINT_PRIMARY_COLOR,
				 lC::DEFAULT_CONSTRAINT_SECONDARY_COLOR,
				 lC::Background::PATTERN,
				 Qt::white,
				 Qt::gray ) );

  color_schemes_.
    append( new PageColorScheme( qApp->translate( "", "Monochrome" ),
				 Qt::black,
				 Qt::black,
				 Qt::gray,
				 Qt::black,
				 Qt::gray,
				 lC::Background::SOLID,
				 Qt::white ) );
  color_schemes_.
    append( new PageColorScheme( qApp->translate( "", "Monochrome (reverse)" ),
				 Qt::white,
				 Qt::white,
				 Qt::gray,
				 Qt::white,
				 Qt::gray,
				 lC::Background::SOLID,
				 Qt::black ) );


  default_.setColorScheme( *color_schemes_.at( 0 ) );
  current_.setColorScheme( *color_schemes_.at( 0 ) );
}

OpenGLGlobals::~OpenGLGlobals ( void )
{}

OpenGLGlobals* OpenGLGlobals::instance ( void )
{
  if ( opengl_globals_ == 0 )
    opengl_globals_ = new OpenGLGlobals;

  return opengl_globals_;
}

QStringList OpenGLGlobals::schemeStrings ( void ) const
{
  QStringList strings;

  QList< PageColorScheme > cs( color_schemes_ );

  for ( ; cs.current() != 0; ++cs )
    strings.append( cs.current()->name() );

  return strings;
}

void OpenGLGlobals::setPredefinedScheme ( bool predefined )
{
  predefined_scheme_ = predefined;
}

PageColorScheme& OpenGLGlobals::scheme ( int index ) const
{
  // For some reason, QPtrList doesn't have a direct access method
  // which does not change the current item, but we can do this
  // with an iterator:
  QList< PageColorScheme > pcs( color_schemes_ );

  pcs += index;

  return *pcs.current();
}

// Set the attributes of the given scheme as the default scheme.

void OpenGLGlobals::setDefaultScheme ( int index )
{
  // The side-effect of using QPtrList::at(index) is to make index the
  // current entry.
  default_.setColorScheme( *color_schemes_.at( index ) );
}

// Set the attributes of the named scheme as the default scheme.

void OpenGLGlobals::setDefaultScheme ( const QString& name )
{
  // Advance the current entry to the given point
  PageColorScheme* pcs = color_schemes_.first();
  for ( ; pcs != 0; pcs = color_schemes_.next() )
    if ( pcs->name() == name )
      break;

  if ( pcs != 0 )
    default_.setColorScheme( *pcs );
}

// Set the attributes of the given scheme into current scheme. For now,
// this does not update the default scheme.
void OpenGLGlobals::setScheme ( int index )
{
  // For now, this does not update the default scheme
  current_.setColorScheme( scheme( index ) );
}

void OpenGLGlobals::setGeometryColor ( const QColor& color )
{
  current_.scheme().setGeometryColor( color );

  emit attributeChanged();
}

void OpenGLGlobals::setAnnotationColor ( const QColor& color )
{
  current_.scheme().setAnnotationColor( color );

  emit attributeChanged();
}

void OpenGLGlobals::setGridColor ( const QColor& color )
{
  current_.scheme().setGridColor( color );

  emit attributeChanged();
}

void OpenGLGlobals::setConstraintPrimaryColor ( const QColor& color )
{
  current_.scheme().setConstraintPrimaryColor( color );

  emit attributeChanged();
}

void OpenGLGlobals::setConstraintSecondaryColor ( const QColor& color )
{
  current_.scheme().setConstraintSecondaryColor( color );

  emit attributeChanged();
}

void OpenGLGlobals::setBackgroundStyle ( enum lC::Background::Style style )
{
  current_.scheme().setBackgroundStyle( style );

  emit backgroundAttributeChanged();
}

void OpenGLGlobals::setBackgroundColor ( const QColor& color )
{
  current_.scheme().setBackgroundColor( color );

  emit backgroundAttributeChanged();
}

void OpenGLGlobals::setGradientColor ( const QColor& color )
{
  current_.scheme().setGradientColor( color );

  emit backgroundAttributeChanged();
}

void OpenGLGlobals::setPatternFile ( const QString& file )
{
  current_.scheme().setPatternFile( file );

  emit backgroundAttributeChanged();
}

void OpenGLGlobals::setDimensionFont ( const QString& font )
{
  current_.dimension().setFont( font );

  emit attributeChanged();
}

void OpenGLGlobals::setArrowHeadLength ( double length )
{
  current_.dimension().setArrowHeadLength( length );

  emit attributeChanged();
}

void OpenGLGlobals::setArrowHeadWidthRatio ( const Ratio& ratio )
{
  current_.dimension().setArrowHeadWidthRatio( ratio );

  emit attributeChanged();
}

void OpenGLGlobals::setArrowHeadStyle ( enum lC::ArrowHeadStyle style )
{
  current_.dimension().setArrowHeadStyle( style );

  emit attributeChanged();
}

void OpenGLGlobals::setClearance ( double clearance )
{
  current_.dimension().setClearance( clearance );

  emit attributeChanged();
}

void OpenGLGlobals::setLineThickness ( double thickness )
{
  current_.dimension().setLineThickness( thickness );

  emit attributeChanged();
}

void OpenGLGlobals::setExtensionLineOffset ( double offset )
{
  current_.dimension().setExtensionLineOffset( offset );

  emit attributeChanged();
}

void OpenGLGlobals::setAnnotationFont ( const QString& font )
{
  current_.annotation().setFont( font );

  emit attributeChanged();
}

void OpenGLGlobals::setHandleSize ( double size )
{
  current_.handle().setSize( size );

  emit attributeChanged();
}
