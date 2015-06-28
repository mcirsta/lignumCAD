/*
 * openglexample.cpp
 *
 * OpenGLExample class: the OpenGL widget.
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
#include <qpaintdevicemetrics.h>
#include <qtimer.h>

#include "OGLFT.h"

#include "systemdependencies.h"
#include "handle.h"
#include "dimension.h"
#include "openglexample.h"

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

OpenGLExample::OpenGLExample ( LengthUnit* length_unit, const QGLFormat& format,
			       const char* name, QWidget* parent )
  : OpenGLBase( format, parent, name ), mode_( lC::Render::REGULAR ),
    length_unit_( length_unit )
{
  format_ = length_unit_->defaultFormat();
  precision_ = length_unit_->defaultPrecision( format_ );

  attributes_.setColorScheme( OpenGLGlobals::instance()->scheme() );
  attributes_.setDimension( OpenGLGlobals::instance()->dimension() );
  attributes_.setAnnotation( OpenGLGlobals::instance()->annotation() );
  attributes_.setHandle( OpenGLGlobals::instance()->handle() );

  dimension_ = new Space2D::Dimension( this );

  computeLayout();
}

OpenGLExample::~OpenGLExample ( void )
{
  delete dimension_;
}

double OpenGLExample::scale ( void ) const
{
  return scale_;
}

Point OpenGLExample::llCorner ( void ) const
{
  return ll_corner_;
}

Point OpenGLExample::urCorner ( void ) const
{
  return ur_corner_;
}

void OpenGLExample::setLengthUnit ( LengthUnit* length_unit, UnitFormat format,
				    int precision )
{
  length_unit_ = length_unit;
  format_ = format;
  precision_ = precision;

  makeCurrent();

  QPaintDeviceMetrics pdm( this );

  glViewport( 0, 0, pdm.width(), pdm.height() );

  computeLayout();

  glMatrixMode( GL_PROJECTION );
  glLoadIdentity();
  glOrtho( ll_corner_[X], ur_corner_[X], ll_corner_[Y], ur_corner_[Y],
	   ll_corner_[Z], ur_corner_[Z] );

  glMatrixMode( GL_MODELVIEW );

  dimension_->setEndsNormal( Space2D::Point( 0, 0 ),
			     Space2D::Point( dmn_length_, 0 ),
			     Space2D::Vector( 0, 1 ), this );

  mode_ = lC::Render::REGULAR;

  updateGL();
}

void OpenGLExample::setColorScheme ( const PageColorScheme& scheme )
{
  attributes_.setColorScheme( scheme );

  makeCurrent();

  qglClearColor( attributes_.scheme().backgroundColor() );

  if ( attributes_.scheme().backgroundStyle() == lC::Background::PATTERN )
    createBackgroundTexture( attributes_.scheme().patternFile(),
			     attributes_.scheme().backgroundColor(),
			     attributes_.scheme().gradientColor() );

  mode_ = lC::Render::REGULAR;

  updateGL();
}

void OpenGLExample::setGeometryColor ( const QColor& color )
{
  attributes_.scheme().setGeometryColor( color );

  makeCurrent();

  mode_ = lC::Render::REGULAR;

  updateGL();
}

void OpenGLExample::setAnnotationColor ( const QColor& color )
{
  attributes_.scheme().setAnnotationColor( color );

  makeCurrent();

  mode_ = lC::Render::REGULAR;

  updateGL();
}

void OpenGLExample::setGridColor ( const QColor& color )
{
  attributes_.scheme().setGridColor( color );

  makeCurrent();

  mode_ = lC::Render::REGULAR;

  updateGL();
}

void OpenGLExample::setConstraintPrimaryColor ( const QColor& color )
{
  attributes_.scheme().setConstraintPrimaryColor( color );

  makeCurrent();

  mode_ = lC::Render::REGULAR;

  updateGL();
}

void OpenGLExample::setConstraintSecondaryColor ( const QColor& color )
{
  attributes_.scheme().setConstraintSecondaryColor( color );

  makeCurrent();

  mode_ = lC::Render::REGULAR;

  updateGL();
}

void OpenGLExample::setBackgroundStyle ( enum lC::Background::Style style )
{
  attributes_.scheme().setBackgroundStyle( style );

  makeCurrent();

  if ( style == lC::Background::PATTERN )
    createBackgroundTexture( attributes_.scheme().patternFile(),
			     attributes_.scheme().backgroundColor(),
			     attributes_.scheme().gradientColor() );

  mode_ = lC::Render::REGULAR;

  updateGL();
}

void OpenGLExample::setBackgroundColor ( const QColor& color )
{
  attributes_.scheme().setBackgroundColor( color );

  makeCurrent();

  qglClearColor( attributes_.scheme().backgroundColor() );

  if ( attributes_.scheme().backgroundStyle() == lC::Background::PATTERN )
    createBackgroundTexture( attributes_.scheme().patternFile(),
			     attributes_.scheme().backgroundColor(),
			     attributes_.scheme().gradientColor() );

  mode_ = lC::Render::REGULAR;

  updateGL();
}

void OpenGLExample::setGradientColor ( const QColor& color )
{
  attributes_.scheme().setGradientColor( color );

  makeCurrent();

  if ( attributes_.scheme().backgroundStyle() == lC::Background::PATTERN )
    createBackgroundTexture( attributes_.scheme().patternFile(),
			     attributes_.scheme().backgroundColor(),
			     attributes_.scheme().gradientColor() );

  mode_ = lC::Render::REGULAR;

  updateGL();
}

void OpenGLExample::setPatternFile ( const QString& file )
{
  attributes_.scheme().setPatternFile( file );

  makeCurrent();

  if ( attributes_.scheme().backgroundStyle() == lC::Background::PATTERN )
    createBackgroundTexture( attributes_.scheme().patternFile(),
			     attributes_.scheme().backgroundColor(),
			     attributes_.scheme().gradientColor() );

  mode_ = lC::Render::REGULAR;

  updateGL();
}

void OpenGLExample::setDimensionFont ( const QString& font )
{
  attributes_.dimension().setFont( font );

  makeCurrent();

  dimension_->setEndsNormal( Space2D::Point( 0, 0 ),
			     Space2D::Point( dmn_length_, 0 ),
			     Space2D::Vector( 0, 1 ), this );

  mode_ = lC::Render::REGULAR;

  updateGL();
}

void OpenGLExample::setArrowHeadLength ( double length )
{
  attributes_.dimension().setArrowHeadLength( length );

  makeCurrent();

  dimension_->setEndsNormal( Space2D::Point( 0, 0 ),
			     Space2D::Point( dmn_length_, 0 ),
			     Space2D::Vector( 0, 1 ), this );

  mode_ = lC::Render::REGULAR;

  updateGL();
}

void OpenGLExample::setArrowHeadWidthRatio ( const Ratio& ratio )
{
  attributes_.dimension().setArrowHeadWidthRatio( ratio );

  makeCurrent();

  dimension_->setEndsNormal( Space2D::Point( 0, 0 ),
			     Space2D::Point( dmn_length_, 0 ),
			     Space2D::Vector( 0, 1 ), this );

  mode_ = lC::Render::REGULAR;

  updateGL();
}

void OpenGLExample::setArrowHeadStyle ( enum lC::ArrowHeadStyle style )
{
  attributes_.dimension().setArrowHeadStyle( style );

  makeCurrent();

  dimension_->setEndsNormal( Space2D::Point( 0, 0 ),
			     Space2D::Point( dmn_length_, 0 ),
			     Space2D::Vector( 0, 1 ), this );

  mode_ = lC::Render::REGULAR;

  updateGL();
}

void OpenGLExample::setClearance ( double length )
{
  attributes_.dimension().setClearance( length );

  makeCurrent();

  dimension_->setEndsNormal( Space2D::Point( 0, 0 ),
			     Space2D::Point( dmn_length_, 0 ),
			     Space2D::Vector( 0, 1 ), this );

  mode_ = lC::Render::REGULAR;

  updateGL();
}

void OpenGLExample::setExtensionLineOffset ( double length )
{
  attributes_.dimension().setExtensionLineOffset( length );

  makeCurrent();

  dimension_->setEndsNormal( Space2D::Point( 0, 0 ),
			     Space2D::Point( dmn_length_, 0 ),
			     Space2D::Vector( 0, 1 ), this );

  mode_ = lC::Render::REGULAR;

  updateGL();
}

void OpenGLExample::setAnnotationFont ( const QString& font )
{
  attributes_.annotation().setFont( font );

  makeCurrent();

  mode_ = lC::Render::REGULAR;

  updateGL();
}

void OpenGLExample::setHandleSize ( double length )
{
  attributes_.handle().setSize( length );

  makeCurrent();

  mode_ = lC::Render::ACTIVATED;

  updateGL();
}

OGLFT::Face* OpenGLExample::font ( const FaceData& requested_face )
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

void OpenGLExample::computeLayout ( void )
{
  // Try to construct a meaningful example which illustrates all of
  // attributes of the length specification.

  switch ( length_unit_->system() ) {
  case ENGLISH:
    switch ( format_ ) {
    case FRACTIONAL:
      if ( length_unit_->in() <= 1. )
	dmn_length_ = 5 + pow( .5, precision_ );
      else
	dmn_length_ = 2.5 * length_unit_->in() + pow( .5, precision_ );
      break;
    case DECIMAL: 
      dmn_length_ = 5.6789 * length_unit_->in();
      break;
    }
    break;
  case METRIC:
    dmn_length_ = 123.456 / length_unit_->in();
    break;
  }

  // Re-round this value as the program would do.
  dmn_length_ = length_unit_->round( format_, precision_, dmn_length_ );

  // The factor of 1.2 is give extra space around the dimension. The
  // factor of 2 is to make room for other items.
  double width = 2 * 1.2 * dmn_length_;

  QPaintDeviceMetrics pdm( this );
  scale_ = pdm.logicalDpiX() / ( pdm.width() / width );

  double height = width * pdm.height() / pdm.width();

  ll_corner_ = Point( -0.1 * dmn_length_, -.25 * height, -1. );
  ur_corner_ = Point(  width - 0.1 * dmn_length_,  .75 * height, 1. );
}

QString OpenGLExample::format ( double value ) const
{
  // If we don't have a specific unit representation yet, just make
  // something up. It's just an example, after all.
  if ( length_unit_ == 0 )
    return QString::number( value, 'f', 3 );
  else
    return length_unit_->format( format_, precision_, value );
}

void OpenGLExample::initializeGL ( void )
{
  qglClearColor( attributes_.scheme().backgroundColor() );

  glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
  glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
  glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
#if defined(GL_RASTER_POSITION_UNCLIPPED_IBM)
  glEnable( GL_RASTER_POSITION_UNCLIPPED_IBM );
#endif
  glShadeModel( GL_FLAT );
}

void OpenGLExample::resizeGL ( int w, int h )
{
  glViewport( 0, 0, w, h );

  computeLayout();

  glMatrixMode( GL_PROJECTION );
  glLoadIdentity();
  glOrtho( ll_corner_[X], ur_corner_[X], ll_corner_[Y], ur_corner_[Y],
	   ll_corner_[Z], ur_corner_[Z] );

  glMatrixMode( GL_MODELVIEW );
  glLoadIdentity();

  dimension_->setEndsNormal( Space2D::Point( 0, 0 ),
			     Space2D::Point( dmn_length_, 0 ),
			     Space2D::Vector( 0, 1 ), this );

  // For nVidia OpenGL 28.80 the exposure of this window is not correct.
  // Trying to draw again later (hopefully after the window is mapped),
  // seems to help.
  QTimer::singleShot( 10, this, SLOT( redraw(void) ) );
}

void OpenGLExample::paintGL ( void )
{
  attributes_.clear( this, false );

  Vector size = ur_corner_ - ll_corner_;

  double d = dmn_length_ / 6.;
  int n_x = (int)( size[X] / d );
  int n_y = (int)( size[Y] / d );

  qglColor( attributes_.scheme().gridColor() );
  glEnable( GL_LINE_STIPPLE );
  glLineStipple( 1, 0xaaaa );

  glBegin( GL_LINES );

  double x = ll_corner_[X];

  for ( int i_x = 0; i_x <= n_x; i_x++ ) {
    glVertex2d( x, ll_corner_[Y] );
    glVertex2d( x, ur_corner_[Y] );
    x += d;
  }

  double y = ll_corner_[Y];

  for ( int j_y = 0; j_y <= n_y; j_y++ ) {
    glVertex2d( ll_corner_[X], y );
    glVertex2d( ur_corner_[X], y );
    y += d;
  }
  
  glEnd();

  glDisable( GL_LINE_STIPPLE );

  qglColor( attributes_.scheme().geometryColor() );

  glBegin( GL_LINE_STRIP );

  glVertex2f( 0, ll_corner_[Y] );
  glVertex2f( 0, 0 );
  glVertex2f( dmn_length_, 0 );
  glVertex2f( dmn_length_, ll_corner_[Y] );

  glEnd();

  if ( mode_ == lC::Render::ACTIVATED ) {
    Space2D::ResizeHandle::draw( this, lC::Render::ACTIVATED, Point( 0, 0 ) );
    Space2D::ResizeHandle::draw( this, lC::Render::ACTIVATED, Point( dmn_length_, 0 ) );
  }

  dimension_->draw( lC::Render::REGULAR );

  FaceData face( annotationFont(), 0, annotationColor().rgb() );
  font( face )->draw( 1.1 * dmn_length_, 0., tr( "An Example annotation" ) );
}

void OpenGLExample::redraw ( void )
{
  updateGL();
}
