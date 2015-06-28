/*
 * openglprinter.cpp
 *
 * OpenGLPrinter class: the OpenGL widget.
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

#include <fstream>

#include <qpainter.h>
#include <qpicture.h>
#include <qfileinfo.h>

#include "OGLFT.h"

#include "vectoralgebra.h"
#include "gl2ps.h"
#include "pageview.h"
#include "model.h"
#include "designbookview.h"
#include "businessinfo.h"
#include "systemdependencies.h"
#include "oglpaintdevice.h"
#include "openglprinter.h"

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

/*!
 * Construct an OpenGL Printer.
 * \param parent The parent widget (a layout, most likely).
 * \param name Widget name.
 */
OpenGLPrinter::OpenGLPrinter ( DesignBookView* parent, const char* name,
			       QGLWidget* share_widget )
  : OpenGLView( parent, name, 0, share_widget )
{
  hide(); // Very shy
  resize( 1, 1 );
}

OpenGLPrinter::~OpenGLPrinter ( void )
{}

// Lazily construct and cache a font corresponding to the given
// attributes (since the OGLFT display list caches are cleared on
// almost all property changes).

OGLFT::Face* OpenGLPrinter::font ( const FaceData& requested_face )
{
  FaceData actual_face = requested_face;
  actual_face.scale_ = view_data_.scale_;

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

  // The printer uses an outline font, thus it has to be adjusted for
  // the scale of the view. Note that resolution == 1 (last argument
  // to the face constructor) implies that the point size is
  // effectively in scale inches.

  OGLFT::Face* base_face = new OGLFT::Filled( file, point_size * scale_ * 72 / pdm_.logicalDpiY(), 1 );

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
/*
 * Temporarily set the MODELVIEW (only) to this subwindow. In the printer
 * view, we use clipping planes instead of the scissor box.
 */
QRect OpenGLPrinter::newWindow ( const Space2D::Point& origin,
			      const Space2D::Vector& size )
{
  // Compute the size in (OpenGL) screen coordinates.
  int w = (int)fabs( rint( size[X] * pdm_.logicalDpiX() ) );
  int h = (int)fabs( rint( size[Y] * pdm_.logicalDpiY() ) );

  // This is a bit of gloss: don't render outside (below) the bounding box.
  glEnable( GL_CLIP_PLANE0 );

  // Create the local window coordinate system such that the origin
  // is positioned at the upper left corner in model coordinates
  // but the size of the window is in paper inches (equivalent to
  // a number of pixels)
  glPushMatrix();
  glLoadIdentity();
  glTranslated( origin[X], origin[Y], 0. );

  GLdouble plane[] = { 0, 1, 0, scale_ * fabs(size[Y]) };
  glClipPlane( GL_CLIP_PLANE0, plane );

  glScaled( view_data_.scale_/(double)pdm_.logicalDpiX(),
	    view_data_.scale_/(double)pdm_.logicalDpiY(), 1. );

  scale_ = pdm_.logicalDpiX();
  old_scale_ = view_data_.scale_;
  view_data_.scale_ = scale_;

  return QRect( 0, 0, w, h );
}
/*
 * Restore the previous model view.
 */
void OpenGLPrinter::resetWindow ( void )
{
  glDisable( GL_CLIP_PLANE0 );
  glPopMatrix();
  scale_ = view_data_.scale_ = old_scale_;
}

/*
 * Printing is done using OpenGL feedback (and a substantial additional
 * infrastructure). Eventually, the OpenGL primitives are drawn using
 * Qt's drawing routines which themselves are converted into whatever
 * graphics language the QPrinter device uses.
 */
void OpenGLPrinter::print ( PageView* page_view, QPainter& painter,
			    int page_no, int pages )
{
  page_view_ = page_view;
  page_view_->viewData( view_data_ );
  scale_ = view_data_.scale_;
  page_view_->viewAttributeChanged();
  page_view_->hideHighlights();

  // This really only needs to happen when the scale is changed...
  clearFontCache();

  QPaintDeviceMetrics p_pdm( painter.device() );

  // Use the maximum sized viewport on the theory that the OpenGL
  // implementation may use fixed point for the screen coordinates.

  GLint max_viewport_dims[2];
  glGetIntegerv( GL_MAX_VIEWPORT_DIMS, max_viewport_dims );

  glViewport( 0, 0, max_viewport_dims[0], max_viewport_dims[1] );

  glGetIntegerv( GL_VIEWPORT, viewport_ );

  // Unfortunately, we can't know a priori how much space the OpenGL
  // feedback buffer will require, so we have to keep iterating
  // until we capture the entire page.

  GLint buffsize = 0;
  GLint state = GL2PS_OVERFLOW;

  while ( state == GL2PS_OVERFLOW ) {
    buffsize += 1024 * 1024;

    gl2psBeginPage( GL2PS_QT, parent()->name(), "lignumCAD",
		    page_view_->space() == SPACE2D? GL2PS_NO_SORT : GL2PS_BSP_SORT,
		    GL2PS_SIMPLE_LINE_OFFSET | GL2PS_SILENT,
		    GL_RGBA, 0, NULL, buffsize, &painter );

    // The first thing to do is to set up the paper size viewport for
    // 2D drawing so that we can draw the frame and the metadata box.

    widthIN_ = view_data_.scale_ * painter.viewport().width()
      / p_pdm.logicalDpiX();
    heightIN_ = view_data_.scale_ * painter.viewport().height()
      / p_pdm.logicalDpiY();

    ll_corner_ = view_data_.view_point_;
    ur_corner_ = ll_corner_ + Vector( widthIN_, heightIN_ );

    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();

    gluOrtho2D( ll_corner_[X], ur_corner_[X], ll_corner_[Y], ur_corner_[Y] );

    // There is a slight mismatch between Qt's and OpenGL's idea of
    // the last pixel on a drawing surface, so reduce the size of the
    // bounding box by the equivalent of one pixel.
    ll_corner_ += Vector( view_data_.scale_ / p_pdm.logicalDpiX(),
			  view_data_.scale_ / p_pdm.logicalDpiY() );
    ur_corner_ -= Vector( view_data_.scale_ / p_pdm.logicalDpiX(),
			  view_data_.scale_ / p_pdm.logicalDpiY() );

    glGetDoublev( GL_PROJECTION_MATRIX, projection_ );

    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();

    // (All that for just this)
    glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
    qglColor( Qt::black );
    glRectdv( ll_corner_, ur_corner_ );

    drawFrame( page_no, pages );

    // Set the viewing transform for the page (which is already set for 2D)

    if ( page_view_->space() == SPACE3D ) {
      ll_corner_ = view_data_.view_point_ +
	Vector( -widthIN_/2., -heightIN_/2., -widthIN_/2. );
      ur_corner_ = view_data_.view_point_ +
	Vector( widthIN_/2., heightIN_/2., widthIN_/2. );

      glMatrixMode( GL_PROJECTION );
      glLoadIdentity();

      glOrtho( ll_corner_[X], ur_corner_[X],
	       ll_corner_[Y], ur_corner_[Y],
	       ll_corner_[Z], ur_corner_[Z] );

      glGetDoublev( GL_PROJECTION_MATRIX, projection_ );

      glMatrixMode( GL_MODELVIEW );
      glLoadIdentity();

      glRotated( view_data_.z_angle_1_, 0, 0, 1 );
      glRotated( view_data_.y_angle_0_, 0, 1, 0 );
      glRotated( view_data_.z_angle_0_, 0, 0, 1 );

      glGetDoublev( GL_MODELVIEW_MATRIX, modelview_ );
    }

    page_view_->draw();

    state = gl2psEndPage();
    if ( state == GL2PS_OVERFLOW ) cout << "hmm. Gl2PS overflow" << endl;
  }

  page_view_->restoreHighlights();
}

/*
 * Draw the border and metadata using OpenGL as well.
 */
void OpenGLPrinter::drawFrame ( int page_no, int pages )
{
  Model* model = dynamic_cast<DesignBookView*>( parent() )->model();

  QString model_str = tr( "Model: %1" ).
    arg( model->name() ).prepend(' ').append(' ');
  QString page_str = tr( "%1: %2" ).
    arg( tr( page_view_->type() ) ).
    arg( page_view_->name() ).prepend(' ').append(' ');
  QString date_str = tr( "Date: %1" ).
    arg( model->modified().date().toString(Qt::ISODate) ).prepend(' ').append(' ');
  QString scale_str = tr( "Scale: %1:%2" ).
    arg( view_data_.scale_.numerator() ).
    arg( view_data_.scale_.denominator() ).prepend(' ').append(' ');
  QString page_no_str = tr( "Page %1 of %2" ).
    arg( page_no ).
    arg( pages ).prepend(' ').append(' ');
  QString approval_str = tr( "Ap'd by:" ).prepend(' ').append(' ');
  // I tried a QSimpleRichText table for the layout, but proved to be
  // intractable. So, we have to compute the layout by hand.
  QFont large_font;
  if ( !OpenGLGlobals::instance()->annotationFont().isEmpty() )
    large_font.fromString( OpenGLGlobals::instance()->annotationFont() );
  large_font.setPointSizeFloat( 1.5 * large_font.pointSizeFloat() );
  large_font.setBold( false );

  FaceData large_face_data( large_font.toString(), 0, Qt::black.rgb(),
			    lC::CENTER );

  FaceData mid_face_data( OpenGLGlobals::instance()->annotationFont(),
			  0, Qt::black.rgb(), lC::CENTER );

  FaceData regular_face_data( OpenGLGlobals::instance()->annotationFont(),
			      0, Qt::black.rgb(), lC::LEFT );

  OGLFT::Face* large_face = font( large_face_data );
  OGLFT::Face* medium_face = font( mid_face_data );
  OGLFT::Face* regular_face = font( regular_face_data );

  OGLFT::BBox approval_bbox = regular_face->measure( approval_str );
  OGLFT::BBox page_no_bbox = regular_face->measure( page_no_str );
  OGLFT::BBox scale_bbox = regular_face->measure( scale_str );
  OGLFT::BBox date_bbox = regular_face->measure( date_str );
  OGLFT::BBox model_bbox = regular_face->measure( model_str );
  OGLFT::BBox page_bbox = regular_face->measure( page_str );
  OGLFT::BBox business_bbox = large_face->measure( BusinessInfo::instance().name());
  OGLFT::BBox location_bbox = medium_face->measure(BusinessInfo::instance().location());

  QPicture logo;
  double logo_width = 0, logo_height = 0;
  double logo_scale_x = 1, logo_scale_y = 1;

  if ( !BusinessInfo::instance().logo().isEmpty() ) {
    if ( QFileInfo( BusinessInfo::instance().logo() ).extension().lower() == "svg"){
      logo.load( BusinessInfo::instance().logo(), "svg" );
      QPaintDeviceMetrics pcpdm( &logo );
      // Convert the size of the logo from paper inches to scale inches (like
      // the font sizes).
      logo_width = scale_ * pcpdm.width() / pcpdm.logicalDpiX();
      logo_height = scale_ * pcpdm.height() / pcpdm.logicalDpiY();

      // Scale the logo down so that it is not higher than the business info
      // text.
      double logo_scale = ( medium_face->height() + large_face->height() ) /
	logo_height;
      logo_width *= logo_scale;
      logo_scale_x = scale_ * logo_scale / pcpdm.logicalDpiX();
      logo_scale_y = scale_ * logo_scale / pcpdm.logicalDpiY();
    }
    // A Pixmap logo is ignored for now...
  }

  double box_width = 2. * approval_bbox.advance_.dx_;
  box_width = QMAX( box_width, 2. * page_no_bbox.advance_.dx_ );
  box_width = QMAX( box_width, 2. * scale_bbox.advance_.dx_ );
  box_width = QMAX( box_width, 2. * date_bbox.advance_.dx_ );
  box_width = QMAX( box_width, 2. * model_bbox.advance_.dx_ );
  box_width = QMAX( box_width, 2. * page_bbox.advance_.dx_ );
  box_width = QMAX( box_width, business_bbox.advance_.dx_ + logo_width );
  box_width = QMAX( box_width, location_bbox.advance_.dx_ + logo_width );

  double row_height = regular_face->height();
  double row_cell_width = box_width / 2.;
  double row_x = urCorner()[X] - box_width;
  double row_y = llCorner()[Y];
  double text_y = row_y +
    ( row_height -
      QMAX( approval_bbox.y_max_ - approval_bbox.y_min_,
	    page_no_bbox.y_max_ - page_no_bbox.y_min_ ) ) / 2 -
    QMIN( approval_bbox.y_min_, page_no_bbox.y_min_ );

  glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
  glRectd( row_x, row_y, row_x + row_cell_width, row_y + row_height );
  regular_face->draw( row_x, text_y, approval_str );

  glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
  glRectd( row_x + row_cell_width, row_y, urCorner()[X], row_y + row_height );
  regular_face->draw( row_x + row_cell_width, text_y, page_no_str );

  row_y += row_height;
  text_y = row_y +
    ( row_height -
      QMAX( scale_bbox.y_max_ - scale_bbox.y_min_,
	    date_bbox.y_max_ - date_bbox.y_min_ ) ) / 2 -
    QMIN( scale_bbox.y_min_, date_bbox.y_min_ );

  glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
  glRectd( row_x, row_y, row_x + row_cell_width, row_y + row_height );
  regular_face->draw( row_x, text_y, scale_str );

  glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
  glRectd( row_x + row_cell_width, row_y, urCorner()[X], row_y + row_height );
  regular_face->draw( row_x + row_cell_width, text_y, date_str );

  row_y += row_height;
  text_y = row_y +
    ( row_height -
      QMAX( model_bbox.y_max_ - model_bbox.y_min_,
	    page_bbox.y_max_ - page_bbox.y_min_ ) ) / 2 -
    QMIN( model_bbox.y_min_, page_bbox.y_min_ );

  glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
  glRectd( row_x, row_y, row_x + row_cell_width, row_y + row_height );
  regular_face->draw( urCorner()[X] - box_width, text_y, model_str );

  glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
  glRectd( row_x + row_cell_width, row_y, urCorner()[X], row_y + row_height );
  regular_face->draw( row_x + row_cell_width, text_y, page_str );

  row_y += row_height;
  row_height = large_face->height() + medium_face->height();

  glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
  glRectd( row_x, row_y, urCorner()[X], row_y + row_height );

  if ( !logo.isNull() ) {
    // Highly experimental...
    glPushMatrix(); // Oglpaintdevice should do this...

    OGLPaintDevice ogl_paintdevice( this );
    QPainter ogl_painter( &ogl_paintdevice );

    // Render at the proper scale.
    ogl_painter.translate( row_x,
			   -row_y - medium_face->height() - large_face->height() );
    ogl_painter.scale( logo_scale_x, -logo_scale_y );

    logo.play( &ogl_painter );
    glPopMatrix(); // Oglpaintdevice should do this too...
  }

  text_y = row_y +
    ( medium_face->height() -
      ( location_bbox.y_max_ - location_bbox.y_min_ ) ) / 2. -
    location_bbox.y_min_;

  medium_face->draw( row_x + row_cell_width + logo_width/2, text_y,
		     BusinessInfo::instance().location() );

  row_y += medium_face->height();
  text_y = row_y +
    ( large_face->height() -
      ( business_bbox.y_max_ - business_bbox.y_min_ ) ) / 2. -
    business_bbox.y_min_;

  large_face->draw( row_x + row_cell_width + logo_width/2, text_y,
		    BusinessInfo::instance().name());
}

void OpenGLPrinter::exportPage ( PageView* page_view, OpenGLView* view,
				 const QString& exportFilename,
				 int page_no, int pages )
{
  GL2PSEMF emf;
  emf.stream = ::fopen( exportFilename, "w" );

  page_view_ = page_view;
  page_view_->viewData( view_data_ );
  scale_ = view_data_.scale_;
  page_view_->viewAttributeChanged();
  page_view_->hideHighlights();

  // This really only needs to happen when the scale is changed...
  clearFontCache();

  QPaintDeviceMetrics p_pdm( view );

  // Use the maximum sized viewport on the theory that the OpenGL
  // implementation may use fixed point for the screen coordinates.

  GLint max_viewport_dims[2];
  glGetIntegerv( GL_MAX_VIEWPORT_DIMS, max_viewport_dims );

  glViewport( 0, 0, max_viewport_dims[0], max_viewport_dims[1] );

  glGetIntegerv( GL_VIEWPORT, viewport_ );

  // The first thing to do is to set up the paper size viewport for
  // 2D drawing so that we can draw the frame and the metadata box.

  widthIN_ = view_data_.scale_ * p_pdm.width() / p_pdm.logicalDpiX();
  heightIN_ = view_data_.scale_ * p_pdm.height() / p_pdm.logicalDpiY();

  emf.width = (GLdouble)p_pdm.width() / p_pdm.logicalDpiX();
  emf.height = (GLdouble)p_pdm.height() / p_pdm.logicalDpiY();
  emf.scale_x = 2540 * emf.width / max_viewport_dims[0];
  emf.scale_y = 2540 * emf.height / max_viewport_dims[1];

  // Unfortunately, we can't know a priori how much space the OpenGL
  // feedback buffer will require, so we have to keep iterating
  // until we capture the entire page.

  GLint buffsize = 0;
  GLint state = GL2PS_OVERFLOW;

  while ( state == GL2PS_OVERFLOW ) {
    buffsize += 1024 * 1024;

    gl2psBeginPage( GL2PS_EMF, parent()->name(), "lignumCAD",
		    page_view_->space() == SPACE2D? GL2PS_NO_SORT : GL2PS_BSP_SORT,
		    GL2PS_SIMPLE_LINE_OFFSET | GL2PS_SILENT,
		    GL_RGBA, 0, NULL, buffsize, &emf );

    ll_corner_ = view_data_.view_point_;
    ur_corner_ = ll_corner_ + Vector( widthIN_, heightIN_ );

    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();

    gluOrtho2D( ll_corner_[X], ur_corner_[X], ll_corner_[Y], ur_corner_[Y] );

    // There is a slight mismatch between Qt's and OpenGL's idea of
    // the last pixel on a drawing surface, so reduce the size of the
    // bounding box by the equivalent of one pixel.
    ll_corner_ += Vector( view_data_.scale_ / p_pdm.logicalDpiX(),
			  view_data_.scale_ / p_pdm.logicalDpiY() );
    ur_corner_ -= Vector( view_data_.scale_ / p_pdm.logicalDpiX(),
			  view_data_.scale_ / p_pdm.logicalDpiY() );

    glGetDoublev( GL_PROJECTION_MATRIX, projection_ );

    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();

    // (All that for just this)

    glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
    qglColor( Qt::black );
    glRectdv( ll_corner_, ur_corner_ );

    drawFrame( page_no, pages );

    // Set the viewing transform for the page (which is already set for 2D)

    if ( page_view_->space() == SPACE3D ) {
      ll_corner_ = view_data_.view_point_ +
	Vector( -widthIN_/2., -heightIN_/2., -widthIN_/2. );
      ur_corner_ = view_data_.view_point_ +
	Vector( widthIN_/2., heightIN_/2., widthIN_/2. );

      glMatrixMode( GL_PROJECTION );
      glLoadIdentity();

      glOrtho( ll_corner_[X], ur_corner_[X],
	       ll_corner_[Y], ur_corner_[Y],
	       ll_corner_[Z], ur_corner_[Z] );

      glGetDoublev( GL_PROJECTION_MATRIX, projection_ );

      glMatrixMode( GL_MODELVIEW );
      glLoadIdentity();

      glRotated( view_data_.z_angle_1_, 0, 0, 1 );
      glRotated( view_data_.y_angle_0_, 0, 1, 0 );
      glRotated( view_data_.z_angle_0_, 0, 0, 1 );
      
      glGetDoublev( GL_MODELVIEW_MATRIX, modelview_ );
    }

    page_view_->draw();

    state = gl2psEndPage();
    if ( state == GL2PS_OVERFLOW ) cout << "hmm. Gl2PS overflow" << endl;
  }

  ::fclose( emf.stream );

  page_view_->restoreHighlights();
}
