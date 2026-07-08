/*
 * openglprinter.cpp
 *
 * OpenGLPrinter class: the OpenGL widget.
 * Copyright (C) 2002 lignum Computing, Inc. <lignumcad@lignumcomputing.com>
 * Copyright (C) 2026 Marius Cirsta <mforce2@gmail.com>
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

#include <iostream>
#include <cstdio>
#include <clocale>
#include <limits>
#include <vector>

#include <qpainter.h>
#include <qfileinfo.h>
#include <QColor>
#include <QFile>
#include <QOffscreenSurface>
#include <QOpenGLContext>
#include <QOpenGLFramebufferObject>
#include <QSvgRenderer>

#include <gl2ps.h>

#if !defined(GL2PS_MAJOR_VERSION) || GL2PS_MAJOR_VERSION != 1 || \
  !defined(GL2PS_MINOR_VERSION) || GL2PS_MINOR_VERSION < 4
#error "lignumCAD requires system gl2ps 1.4.x or newer"
#endif

#ifndef GL2PS_PDF
#error "This gl2ps header does not support PDF output"
#endif

#include "OGLFT.h"

#include "vectoralgebra.h"
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

namespace {
  class NumericLocaleGuard {
  public:
    NumericLocaleGuard()
      : old_locale_( std::setlocale( LC_NUMERIC, 0 ) )
    {
      std::setlocale( LC_NUMERIC, "C" );
    }

    ~NumericLocaleGuard()
    {
      if ( !old_locale_.isEmpty() )
	std::setlocale( LC_NUMERIC, old_locale_.constData() );
    }

  private:
    QByteArray old_locale_;
  };

  //! Release the offscreen context when PDF rendering leaves scope.
  class CurrentContextGuard {
  public:
    explicit CurrentContextGuard( QOpenGLContext* context )
      : context_( context )
    {}

    ~CurrentContextGuard()
    {
      if ( context_ != nullptr )
	context_->doneCurrent();
    }

  private:
    QOpenGLContext* context_;
  };

  QString gl2psStatusName( GLint status )
  {
    switch ( status ) {
    case GL2PS_SUCCESS:
      return QStringLiteral( "GL2PS_SUCCESS" );
    case GL2PS_INFO:
      return QStringLiteral( "GL2PS_INFO" );
    case GL2PS_WARNING:
      return QStringLiteral( "GL2PS_WARNING" );
    case GL2PS_ERROR:
      return QStringLiteral( "GL2PS_ERROR" );
    case GL2PS_NO_FEEDBACK:
      return QStringLiteral( "GL2PS_NO_FEEDBACK" );
    case GL2PS_OVERFLOW:
      return QStringLiteral( "GL2PS_OVERFLOW" );
    case GL2PS_UNINITIALIZED:
      return QStringLiteral( "GL2PS_UNINITIALIZED" );
    default:
      return QStringLiteral( "GL2PS_UNKNOWN" );
    }
  }
}

/*!
 * Construct an OpenGL Printer.
 * \param parent The parent widget (a layout, most likely).
 * \param name Widget name.
 */
OpenGLPrinter::OpenGLPrinter ( DesignBookView* parent, const char* name,
			       QOpenGLWidget* share_widget )
  : OpenGLView( parent, name, 0, share_widget ),
    share_widget_( share_widget )
{
  hide(); // Very shy
  resize( 1, 1 );
}

OpenGLPrinter::~OpenGLPrinter ( void )
{
  delete render_context_;
  delete render_surface_;
}

bool OpenGLPrinter::makeRenderContextCurrent ( QString* error )
{
  if ( render_context_ == nullptr ) {
    // The global share context (Qt::AA_ShareOpenGLContexts) lives as long
    // as the application; a QOpenGLWidget's own context can be torn down
    // on hide/reparent, so it is only the fallback.
    QOpenGLContext* share_context = QOpenGLContext::globalShareContext();

    if ( share_context == nullptr && share_widget_ != nullptr )
      share_context = share_widget_->context();

    if ( share_context == nullptr )
      // Geometry still exports without sharing; only display lists and
      // textures cached by the on-screen view would be missing.
      qWarning( "OpenGLPrinter: no share context available; "
		"PDF rendering uses an unshared OpenGL context" );

    render_context_ = new QOpenGLContext;
    render_context_->setFormat( share_context != nullptr ?
				share_context->format() :
				QSurfaceFormat::defaultFormat() );
    render_context_->setShareContext( share_context );

    if ( !render_context_->create() ) {
      delete render_context_;
      render_context_ = nullptr;
      if ( error )
	*error = tr( "Could not create an OpenGL context for PDF rendering." );
      return false;
    }

    render_surface_ = new QOffscreenSurface;
    render_surface_->setFormat( render_context_->format() );
    render_surface_->create();

    if ( !render_surface_->isValid() ) {
      delete render_surface_;
      render_surface_ = nullptr;
      delete render_context_;
      render_context_ = nullptr;
      if ( error )
	*error = tr( "Could not create an OpenGL surface for PDF rendering." );
      return false;
    }
  }

  if ( !render_context_->makeCurrent( render_surface_ ) ) {
    if ( error )
      *error = tr( "Could not activate the OpenGL context for PDF rendering." );
    return false;
  }

  return true;
}

// Lazily construct and cache a font corresponding to the given
// attributes (since the OGLFT display list caches are cleared on
// almost all property changes).

OGLFT::Face* OpenGLPrinter::font ( const FaceData& requested_face )
{
  FaceData actual_face = requested_face;
  actual_face.scale_ = view_data_.scale_;

  if ( actual_face.font_.isEmpty() )
    actual_face.font_ = QWidget::font().toString();

  QMap< FaceData, OGLFT::Face* >::const_iterator face =
    faces_.constFind( actual_face );

  if ( face != faces_.constEnd() )
    return face.value();

  QString file;
  double point_size;

  // While this could fail, I think eventually I'll have to make it always
  // succeed...
  System::findFontFile( actual_face.font_, file, point_size );

  // The printer uses an outline font, thus it has to be adjusted for
  // the scale of the view. Note that resolution == 1 (last argument
  // to the face constructor) implies that the point size is
  // effectively in scale inches.

  QByteArray file_name = file.toUtf8();
  OGLFT::Face* base_face = new OGLFT::Filled( file_name.constData(),
					      point_size * scale_ * 72 / output_dpi_, 1 );

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
  int w = (int)fabs( rint( size[X] * output_dpi_ ) );
  int h = (int)fabs( rint( size[Y] * output_dpi_ ) );

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

  glScaled( view_data_.scale_/(double)output_dpi_,
	    view_data_.scale_/(double)output_dpi_, 1. );

  scale_ = output_dpi_;
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

bool OpenGLPrinter::measureContentBounds2D ( Space2D::Point& ll,
					     Space2D::Point& ur )
{
  // Render the page over a region large enough to contain any plausible
  // drawing and let the feedback buffer report where the content actually
  // is, in window coordinates.
  const double world_ll = -1000.;
  const double world_ur = 1000.;
  const int measure_viewport = 4096;

  std::vector<GLfloat> buffer( 1 << 20 );
  GLint captured = -1;

  while ( true ) {
    glViewport( 0, 0, measure_viewport, measure_viewport );

    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    gluOrtho2D( world_ll, world_ur, world_ll, world_ur );

    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();

    glFeedbackBuffer( (GLsizei)buffer.size(), GL_2D, buffer.data() );
    glRenderMode( GL_FEEDBACK );

    page_view_->draw();

    captured = glRenderMode( GL_RENDER );

    if ( captured >= 0 )
      break;

    // A negative count means the buffer overflowed.
    if ( buffer.size() >= ( 1 << 26 ) )
      return false;
    buffer.resize( buffer.size() * 4 );
  }

  if ( captured == 0 )
    return false;

  double min_x = std::numeric_limits<double>::max();
  double min_y = std::numeric_limits<double>::max();
  double max_x = -std::numeric_limits<double>::max();
  double max_y = -std::numeric_limits<double>::max();
  bool any_vertex = false;

  GLint i = 0;
  while ( i < captured ) {
    const GLfloat token = buffer[i++];
    int vertices = 0;

    if ( token == GL_POINT_TOKEN ||
	 token == GL_BITMAP_TOKEN ||
	 token == GL_DRAW_PIXEL_TOKEN ||
	 token == GL_COPY_PIXEL_TOKEN )
      vertices = 1;
    else if ( token == GL_LINE_TOKEN || token == GL_LINE_RESET_TOKEN )
      vertices = 2;
    else if ( token == GL_POLYGON_TOKEN )
      vertices = (int)buffer[i++];
    else if ( token == GL_PASS_THROUGH_TOKEN ) {
      i++;
      continue;
    }
    else
      break; // Unknown token: stop rather than misparse.

    for ( int v = 0; v < vertices && i + 1 < captured; v++ ) {
      const double x = buffer[i++];
      const double y = buffer[i++];
      min_x = qMin( min_x, x );
      min_y = qMin( min_y, y );
      max_x = qMax( max_x, x );
      max_y = qMax( max_y, y );
      any_vertex = true;
    }
  }

  if ( !any_vertex )
    return false;

  // Map the window coordinates back into model units.
  const double world_extent = world_ur - world_ll;
  ll = Space2D::Point( world_ll + min_x / measure_viewport * world_extent,
		       world_ll + min_y / measure_viewport * world_extent );
  ur = Space2D::Point( world_ll + max_x / measure_viewport * world_extent,
		       world_ll + max_y / measure_viewport * world_extent );

  return true;
}

bool OpenGLPrinter::printToPdf ( PageView* page_view,
				 const GL2PSPageSettings& settings,
				 const QString& pdf_path,
				 int page_no, int pages,
				 QString* error )
{
  return renderPagePdf( page_view, 0, settings, pdf_path, page_no, pages, error );
}

bool OpenGLPrinter::exportPage ( PageView* page_view,
				 OpenGLView* source_view,
				 const GL2PSPageSettings& settings,
				 const QString& pdf_path,
				 int page_no, int pages,
				 QString* error )
{
  return renderPagePdf( page_view, source_view, settings, pdf_path, page_no, pages, error );
}

bool OpenGLPrinter::renderPagePdf ( PageView* page_view,
				    OpenGLView* source_view,
				    const GL2PSPageSettings& settings,
				    const QString& pdf_path,
				    int page_no, int pages,
				    QString* error )
{
  if ( page_view == 0 ) {
    if ( error ) *error = tr( "No page is selected for export." );
    return false;
  }

  const int page_width = qRound( settings.paperWidthPts );
  const int page_height = qRound( settings.paperHeightPts );
  const int margin = qMax( 0, qRound( settings.marginPts ) );

  if ( page_width <= 0 || page_height <= 0 ||
       page_width <= 2 * margin || page_height <= 2 * margin ) {
    if ( error ) *error = tr( "The selected paper size or margin is invalid." );
    return false;
  }

  QByteArray file_name = QFile::encodeName( pdf_path );
  if ( file_name.isEmpty() ) {
    if ( error ) *error = tr( "The PDF file name is empty." );
    return false;
  }

  output_dpi_ = settings.outputDpi > 0 ? settings.outputDpi : 72;
  NumericLocaleGuard numeric_locale_guard;

  // gl2ps captures through the OpenGL feedback buffer, so a real, current
  // context is required for the whole begin/draw/end sequence.
  if ( !makeRenderContextCurrent( error ) )
    return false;

  CurrentContextGuard context_guard( render_context_ );

  // The offscreen surface has no default framebuffer on some platforms
  // (e.g. Wayland). Feedback capture never rasterizes, but an incomplete
  // framebuffer still makes GL discard every draw call with
  // GL_INVALID_FRAMEBUFFER_OPERATION, so bind a small FBO for the capture.
  QOpenGLFramebufferObject capture_fbo( 16, 16 );
  if ( !capture_fbo.bind() ) {
    if ( error )
      *error = tr( "Could not bind a framebuffer for PDF rendering." );
    return false;
  }

  page_view_ = page_view;
  page_view_->viewData( view_data_ );
  scale_ = view_data_.scale_;
  page_view_->viewAttributeChanged();
  page_view_->hideHighlights();

  clearFontCache();

  // Frame the page's actual content: measure it with a plain feedback
  // pass now, before gl2ps enters its own feedback mode. The screen
  // viewport is not a usable frame; it may be panned anywhere.
  Space2D::Point content_ll, content_ur;
  bool have_content = false;
  if ( page_view_->space() == SPACE2D )
    have_content = measureContentBounds2D( content_ll, content_ur );

  GLint page_viewport[4] = { 0, 0, page_width, page_height };

  GLint buffsize = 0;
  GLint state = GL2PS_OVERFLOW;

  while ( state == GL2PS_OVERFLOW ) {
    buffsize += 1024 * 1024;

    FILE* fp = ::fopen( file_name.constData(), "wb" );
    if ( fp == 0 ) {
      page_view_->restoreHighlights();
      if ( error )
	*error = tr( "Could not open '%1' for writing." ).arg( pdf_path );
      return false;
    }

    glViewport( page_viewport[0], page_viewport[1],
		page_viewport[2], page_viewport[3] );
    glGetIntegerv( GL_VIEWPORT, viewport_ );

    QByteArray page_name = parent()->objectName().toUtf8();
    if ( page_name.isEmpty() )
      page_name = "lignumCAD";

    const GLint sort =
      page_view_->space() == SPACE2D ? GL2PS_NO_SORT : GL2PS_BSP_SORT;
    const GLint options = GL2PS_SIMPLE_LINE_OFFSET | GL2PS_SILENT;

    GLint begin = gl2psBeginPage( page_name.constData(), "lignumCAD",
				  page_viewport,
				  GL2PS_PDF, sort, options,
				  GL_RGBA,
				  0, 0,
				  0, 0, 0,
				  buffsize,
				  fp,
				  file_name.constData() );

    if ( begin != GL2PS_SUCCESS ) {
      ::fclose( fp );
      page_view_->restoreHighlights();
      if ( error )
	*error = tr( "gl2ps could not start PDF export: %1 (%2)." ).
	  arg( gl2psStatusName( begin ) ).arg( begin );
      return false;
    }

    if ( page_view_->space() == SPACE2D &&
	 ( have_content || source_view != 0 ) ) {
      // Fit the measured content to the printable area; fall back to the
      // on-screen framing when the page is empty.
      double region_ll_x, region_ll_y, region_width, region_height;

      if ( have_content ) {
	// Pad the bounds a little so strokes at the very edge survive.
	const double pad = .05 * qMax( content_ur[X] - content_ll[X],
				       content_ur[Y] - content_ll[Y] );
	region_ll_x = content_ll[X] - pad;
	region_ll_y = content_ll[Y] - pad;
	region_width = content_ur[X] - content_ll[X] + 2. * pad;
	region_height = content_ur[Y] - content_ll[Y] + 2. * pad;
      }
      else {
	const Point source_ll = source_view->llCorner();
	const Point source_ur = source_view->urCorner();
	region_ll_x = source_ll[X];
	region_ll_y = source_ll[Y];
	region_width = source_ur[X] - source_ll[X];
	region_height = source_ur[Y] - source_ll[Y];
      }

      const double draw_width_pts = qMax( 1, page_width - 2 * margin );
      const double draw_height_pts = qMax( 1, page_height - 2 * margin );

      if ( region_width > 0. && region_height > 0. ) {
	const double world_per_point =
	  qMax( region_width / draw_width_pts, region_height / draw_height_pts );
	widthIN_ = page_width * world_per_point;
	heightIN_ = page_height * world_per_point;

	const double center_x = region_ll_x + .5 * region_width;
	const double center_y = region_ll_y + .5 * region_height;

	ll_corner_ = Point( center_x - .5 * widthIN_,
			    center_y - .5 * heightIN_ );
	ur_corner_ = Point( center_x + .5 * widthIN_,
			    center_y + .5 * heightIN_ );
      }
      else {
	ll_corner_ = Point( region_ll_x, region_ll_y );
	ur_corner_ = Point( region_ll_x + region_width,
			    region_ll_y + region_height );
	widthIN_ = region_width;
	heightIN_ = region_height;
      }
    }
    else {
      widthIN_ = view_data_.scale_ *
	static_cast<double>( page_width - 2 * margin ) / output_dpi_;
      heightIN_ = view_data_.scale_ *
	static_cast<double>( page_height - 2 * margin ) / output_dpi_;

      ll_corner_ = view_data_.view_point_;
      ur_corner_ = ll_corner_ + Vector( widthIN_, heightIN_ );
    }

    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();

    gluOrtho2D( ll_corner_[X], ur_corner_[X], ll_corner_[Y], ur_corner_[Y] );

    ll_corner_ += Vector( view_data_.scale_ / output_dpi_,
			  view_data_.scale_ / output_dpi_ );
    ur_corner_ -= Vector( view_data_.scale_ / output_dpi_,
			  view_data_.scale_ / output_dpi_ );

    glGetDoublev( GL_PROJECTION_MATRIX, projection_ );

    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();

    if ( settings.drawFrame ) {
      glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
      qglColor( Qt::black );
      glRectdv( ll_corner_, ur_corner_ );

      drawFrame( page_no, pages );
    }

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

    // Phase 3 diagnostics (docs/gl2ps-pdf-print-export-plan.md): report the
    // capture framing and optionally draw a known-good primitive.
    // LIGNUMCAD_GL2PS_SMOKE=before draws it in addition to the page;
    // LIGNUMCAD_GL2PS_SMOKE=only draws it instead of the page.
    const QByteArray smoke = qgetenv( "LIGNUMCAD_GL2PS_SMOKE" );

    std::cerr << "gl2ps: space=" << ( page_view_->space() == SPACE2D ? "2D" : "3D" )
	      << " page=" << page_width << "x" << page_height
	      << " scale=" << (double)view_data_.scale_
	      << " figures=" << page_view_->figureViews().size()
	      << " ortho ll=(" << ll_corner_[X] << "," << ll_corner_[Y]
	      << ") ur=(" << ur_corner_[X] << "," << ur_corner_[Y] << ")"
	      << " glGetError(setup)=0x" << std::hex << glGetError() << std::dec
	      << ( smoke.isEmpty() ? "" : " smoke=" ) << smoke.constData()
	      << std::endl;

    if ( !smoke.isEmpty() ) {
      glColor3d( 0., 0., 0. );
      glBegin( GL_LINES );
      glVertex3d( ll_corner_[X], ll_corner_[Y], 0. );
      glVertex3d( ur_corner_[X], ur_corner_[Y], 0. );
      glEnd();
    }

    if ( smoke != "only" )
      page_view_->draw();

    const GLenum draw_error = glGetError();

    state = gl2psEndPage();
    ::fclose( fp );

    std::cerr << "gl2ps: end=" << gl2psStatusName( state ).toStdString()
	      << " glGetError(draw)=0x" << std::hex << draw_error << std::dec
	      << std::endl;

    if ( state == GL2PS_OVERFLOW )
      std::cout << "hmm. Gl2PS overflow" << std::endl;
  }

  page_view_->restoreHighlights();

  if ( state != GL2PS_SUCCESS ) {
    if ( error )
      *error = tr( "gl2ps PDF export failed: %1 (%2)." ).
	arg( gl2psStatusName( state ) ).arg( state );
    return false;
  }

  return true;
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
    arg( tr( page_view_->type().toUtf8().constData() ),
	 page_view_->name() ).prepend(' ').append(' ');
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
  large_font.setPointSizeF( 1.5 * large_font.pointSizeF() );
  large_font.setBold( false );

  QRgb black = QColor( Qt::black ).rgb();

  FaceData large_face_data( large_font.toString(), 0, black,
			    lC::CENTER );

  FaceData mid_face_data( OpenGLGlobals::instance()->annotationFont(),
			  0, black, lC::CENTER );

  FaceData regular_face_data( OpenGLGlobals::instance()->annotationFont(),
			      0, black, lC::LEFT );

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

  QSvgRenderer logo;
  bool logo_loaded = false;
  double logo_width = 0, logo_height = 0;
  double logo_scale_x = 1, logo_scale_y = 1;

  if ( !BusinessInfo::instance().logo().isEmpty() ) {
    if ( QFileInfo( BusinessInfo::instance().logo() ).suffix().toLower() == "svg" ) {
      logo_loaded = logo.load( BusinessInfo::instance().logo() );
      // Convert the size of the logo from paper inches to scale inches (like
      // the font sizes).
      QSize logo_size = logo.defaultSize();
      if ( logo_loaded && logo_size.isValid() && logo_size.height() > 0 ) {
	logo_width = scale_ * logo_size.width() / output_dpi_;
	logo_height = scale_ * logo_size.height() / output_dpi_;

	// Scale the logo down so that it is not higher than the business info
	// text.
	double logo_scale = ( medium_face->height() + large_face->height() ) /
	  logo_height;
	logo_width *= logo_scale;
	logo_scale_x = scale_ * logo_scale / output_dpi_;
	logo_scale_y = scale_ * logo_scale / output_dpi_;
      }
      else
	logo_loaded = false;
    }
    // A Pixmap logo is ignored for now...
  }

  double box_width = 2. * approval_bbox.advance_.dx_;
  box_width = qMax(  box_width, 2. * page_no_bbox.advance_.dx_ );
  box_width = qMax(  box_width, 2. * scale_bbox.advance_.dx_ );
  box_width = qMax(  box_width, 2. * date_bbox.advance_.dx_ );
  box_width = qMax(  box_width, 2. * model_bbox.advance_.dx_ );
  box_width = qMax(  box_width, 2. * page_bbox.advance_.dx_ );
  box_width = qMax(  box_width, business_bbox.advance_.dx_ + logo_width );
  box_width = qMax(  box_width, location_bbox.advance_.dx_ + logo_width );

  double row_height = regular_face->height();
  double row_cell_width = box_width / 2.;
  double row_x = urCorner()[X] - box_width;
  double row_y = llCorner()[Y];
  double text_y = row_y +
    ( row_height -
      qMax(  approval_bbox.y_max_ - approval_bbox.y_min_,
	    page_no_bbox.y_max_ - page_no_bbox.y_min_ ) ) / 2 -
    qMin(  approval_bbox.y_min_, page_no_bbox.y_min_ );

  glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
  glRectd( row_x, row_y, row_x + row_cell_width, row_y + row_height );
  regular_face->draw( row_x, text_y, approval_str );

  glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
  glRectd( row_x + row_cell_width, row_y, urCorner()[X], row_y + row_height );
  regular_face->draw( row_x + row_cell_width, text_y, page_no_str );

  row_y += row_height;
  text_y = row_y +
    ( row_height -
      qMax(  scale_bbox.y_max_ - scale_bbox.y_min_,
	    date_bbox.y_max_ - date_bbox.y_min_ ) ) / 2 -
    qMin(  scale_bbox.y_min_, date_bbox.y_min_ );

  glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
  glRectd( row_x, row_y, row_x + row_cell_width, row_y + row_height );
  regular_face->draw( row_x, text_y, scale_str );

  glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
  glRectd( row_x + row_cell_width, row_y, urCorner()[X], row_y + row_height );
  regular_face->draw( row_x + row_cell_width, text_y, date_str );

  row_y += row_height;
  text_y = row_y +
    ( row_height -
      qMax(  model_bbox.y_max_ - model_bbox.y_min_,
	    page_bbox.y_max_ - page_bbox.y_min_ ) ) / 2 -
    qMin(  model_bbox.y_min_, page_bbox.y_min_ );

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

  if ( logo_loaded ) {
    // Highly experimental...
    glPushMatrix(); // Oglpaintdevice should do this...

    OGLPaintDevice ogl_paintdevice( this );
    QPainter ogl_painter( &ogl_paintdevice );

    // Render at the proper scale.
    ogl_painter.translate( row_x,
			   -row_y - medium_face->height() - large_face->height() );
    ogl_painter.scale( logo_scale_x, -logo_scale_y );

    logo.render( &ogl_painter );
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
