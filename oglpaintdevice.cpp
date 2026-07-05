/*
 * oglpaintdevice.cpp
 *
 * OGLPaintDevice class
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
#include <qpen.h>

#include <QLineF>
#include <QPaintEngine>
#include <QPaintEngineState>
#include <QPainterPath>
#include <QPixmap>
#include <QTextItem>
#include <QTransform>

#include "constants.h"
#include "OGLFT.h"
#include "oglpaintdevice.h"

namespace {

QPointF transformedPoint ( const QTransform& transform, const QPointF& point )
{
  return transform.map( point );
}

void glVertexPoint ( const QPointF& point )
{
  glVertex2d( point.x(), point.y() );
}

} // end of anonymous namespace

class OGLPaintEngine : public QPaintEngine
{
  OGLPaintDevice* device_;
  QPen pen_;
  QFont font_;
  QTransform transform_;

public:
  OGLPaintEngine ( OGLPaintDevice* device )
    : QPaintEngine( QPaintEngine::PaintOutsidePaintEvent ), device_( device )
  {}

  bool begin ( QPaintDevice* paint_device ) override
  {
    setPaintDevice( paint_device );
    setActive( true );
    return true;
  }

  bool end ( void ) override
  {
    setActive( false );
    return true;
  }

  void updateState ( const QPaintEngineState& state ) override
  {
    if ( state.state() & QPaintEngine::DirtyPen ) {
      pen_ = state.pen();
      device_->faceData().color_ = pen_.color().rgb();
    }

    if ( state.state() & QPaintEngine::DirtyFont ) {
      font_ = state.font();
      device_->faceData().font_ = font_.toString();
    }

    if ( state.state() & QPaintEngine::DirtyTransform )
      transform_ = state.transform();
  }

  void drawLines ( const QLineF* lines, int line_count ) override
  {
    if ( pen_.style() == Qt::NoPen )
      return;

    glColor3ubv( lC::qCubv( device_->faceData().color_ ) );
    glBegin( GL_LINES );
    for ( int i = 0; i < line_count; ++i ) {
      glVertexPoint( transformedPoint( transform_, lines[i].p1() ) );
      glVertexPoint( transformedPoint( transform_, lines[i].p2() ) );
    }
    glEnd();
  }

  void drawLines ( const QLine* lines, int line_count ) override
  {
    if ( pen_.style() == Qt::NoPen )
      return;

    glColor3ubv( lC::qCubv( device_->faceData().color_ ) );
    glBegin( GL_LINES );
    for ( int i = 0; i < line_count; ++i ) {
      glVertexPoint( transformedPoint( transform_, lines[i].p1() ) );
      glVertexPoint( transformedPoint( transform_, lines[i].p2() ) );
    }
    glEnd();
  }

  void drawPolygon ( const QPointF* points, int point_count,
		     PolygonDrawMode mode ) override
  {
    if ( point_count <= 0 || pen_.style() == Qt::NoPen )
      return;

    GLenum gl_mode = mode == PolylineMode ? GL_LINE_STRIP : GL_LINE_LOOP;

    glColor3ubv( lC::qCubv( device_->faceData().color_ ) );
    glBegin( gl_mode );
    for ( int i = 0; i < point_count; ++i )
      glVertexPoint( transformedPoint( transform_, points[i] ) );
    glEnd();
  }

  void drawPolygon ( const QPoint* points, int point_count,
		     PolygonDrawMode mode ) override
  {
    if ( point_count <= 0 || pen_.style() == Qt::NoPen )
      return;

    GLenum gl_mode = mode == PolylineMode ? GL_LINE_STRIP : GL_LINE_LOOP;

    glColor3ubv( lC::qCubv( device_->faceData().color_ ) );
    glBegin( gl_mode );
    for ( int i = 0; i < point_count; ++i )
      glVertexPoint( transformedPoint( transform_, points[i] ) );
    glEnd();
  }

  void drawPath ( const QPainterPath& path ) override
  {
    QPolygonF polygon = path.toFillPolygon( transform_ );

    if ( polygon.isEmpty() || pen_.style() == Qt::NoPen )
      return;

    glColor3ubv( lC::qCubv( device_->faceData().color_ ) );
    glBegin( GL_LINE_STRIP );
    for ( const QPointF& point : polygon )
      glVertex2d( point.x(), point.y() );
    glEnd();
  }

  void drawTextItem ( const QPointF& point, const QTextItem& text_item ) override
  {
    device_->faceData().font_ = text_item.font().toString();
    QPointF draw_point = transformedPoint( transform_, point );

    device_->view()->font( device_->faceData() )->draw( draw_point.x(),
							-draw_point.y(),
							text_item.text() );
  }

  void drawPixmap ( const QRectF&, const QPixmap&, const QRectF& ) override
  {}

  Type type ( void ) const override
  {
    return QPaintEngine::User;
  }
};

OGLPaintDevice::OGLPaintDevice ( OpenGLBase* view )
  : QPaintDevice(), view_( view ),
    paint_engine_( std::make_unique<OGLPaintEngine>( this ) )
{}

OGLPaintDevice::~OGLPaintDevice ( void ) = default;

void OGLPaintDevice::setView ( OpenGLBase* view )
{
  view_ = view;
}

QPaintEngine* OGLPaintDevice::paintEngine ( void ) const
{
  return paint_engine_.get();
}

int OGLPaintDevice::metric ( PaintDeviceMetric n ) const
{
  // All of these attributes are taken directly from the OpenGL widget.
  switch ( n ) {
  case QPaintDevice::PdmWidth:
    return view_->width();
  case QPaintDevice::PdmHeight:
    return view_->height();
  case QPaintDevice::PdmWidthMM:
    return view_->widthMM();
  case QPaintDevice::PdmHeightMM:
    return view_->heightMM();
  case QPaintDevice::PdmDpiX:
    return view_->logicalDpiX();
  case QPaintDevice::PdmPhysicalDpiX:
    return view_->physicalDpiX();
  case QPaintDevice::PdmDpiY:
    return view_->logicalDpiY();
  case QPaintDevice::PdmPhysicalDpiY:
    return view_->physicalDpiY();
  case QPaintDevice::PdmNumColors:
    return view_->colorCount();
  case QPaintDevice::PdmDepth:
    return view_->depth();
  case QPaintDevice::PdmDevicePixelRatio:
    return view_->devicePixelRatio();
  case QPaintDevice::PdmDevicePixelRatioScaled:
    return view_->devicePixelRatio() * QPaintDevice::devicePixelRatioFScale();
  default:
    return 0;
  }
}
