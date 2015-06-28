/*
 * oglpaintdevice.cpp
 *
 * OGLPaintDevice class
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
#include <qpainter.h>
#include <qpen.h>

#include "OGLFT.h"
#include "oglpaintdevice.h"

std::ostream& operator<< ( std::ostream& o, const QPoint& p )
{
  return o << "[" << p.x() << ", " << p.y() << "]";
}

std::ostream& operator<< ( std::ostream& o, const QColor& c )
{
  return o << "[" << c.red() << ", " << c.green() << ", " << c.blue() << "]";
}

std::ostream& operator<< ( std::ostream& o, const QBrush& b )
{
  o << "style ";
  switch ( b.style() ) {
  case Qt::NoBrush:
    o << "no brush"; break;
  case Qt::SolidPattern:
    o << "solid"; break;
  default:
    o << "some pattern";
  }
  return o << ", color " << b.color();
}

std::ostream& operator<< ( std::ostream& o, const QPen& p )
{
  o << "style ";
  switch( p.style() ) {
  case Qt::NoPen:
    o << "no pen"; break;
  case Qt::SolidLine:
    o << "solid"; break;
  case Qt::DashLine:
    o << "dash"; break;
  case Qt::DotLine:
    o << "dot"; break;
  default:
    o << "UNKNOWN";
  }
  return o << ", color " << p.color() << ", width " << p.width();
}

std::ostream& operator<< ( std::ostream& o, const QFont& f )
{
  return o << f.rawName().latin1();
}

std::ostream& operator<< ( std::ostream& o, const QRect& r )
{
  return o << r.left() << ", " << r.top() << "; " << r.right() << ", " << r.bottom();
}

std::ostream& operator<< ( std::ostream& o, const QRegion& r )
{
  QMemArray<QRect> rects( r.rects() );
  for ( uint i = 0; i < rects.size(); i++ )
    o << "\t" << i << ": " << rects[i] << endl;
  return o;
}

std::ostream& operator<< ( std::ostream& o, const QWMatrix& m )
{
  o << "\t" << m.m11() << ", " << m.m12() << ": " << m.dx() << endl;
  o << "\t" << m.m21() << ", " << m.m22() << ": " << m.dy() << endl;
  return o;
}

void OGLPaintDevice::setView ( OpenGLBase* view )
{
  view_ = view;
}

int OGLPaintDevice::metric ( int n ) const
{
  // All of these attributes are taken directly from the OpenGL widget.
  QPaintDeviceMetrics pdm( view_ );
  switch ( n ) {
  case QPaintDeviceMetrics::PdmWidth:
    return pdm.width();
  case QPaintDeviceMetrics::PdmHeight:
    return pdm.height();
  case QPaintDeviceMetrics::PdmWidthMM:
    return pdm.widthMM();
  case QPaintDeviceMetrics::PdmHeightMM:
    return pdm.heightMM();
  case QPaintDeviceMetrics::PdmDpiX:
  case QPaintDeviceMetrics::PdmPhysicalDpiX:
    return pdm.logicalDpiX();
  case QPaintDeviceMetrics::PdmDpiY:
  case QPaintDeviceMetrics::PdmPhysicalDpiY:
    return pdm.logicalDpiY();
  case QPaintDeviceMetrics::PdmNumColors:
    return pdm.numColors();
  case QPaintDeviceMetrics::PdmDepth:
    return pdm.depth();
  default:
    return 0;
  }
}

bool OGLPaintDevice::cmd ( int command, QPainter* /*painter*/, QPDevCmdParam* params )
{
  switch ( command ) {
#if 0
  case PdcNOP:
    cout << "NOP" << endl; break;
  case PdcDrawPoint:
    cout << "Draw point" << endl; break;
  case PdcMoveTo:
    cout << "Move to" << endl; break;
  case PdcLineTo:
    cout << "Line to" << endl; break;
#endif
  case PdcDrawLine:
    //    cout << "Draw line" << *params[0].point << "; " << *params[1].point << endl;
    glColor3ubv( lC::qCubv( face_data_.color_ ) );
    glBegin( GL_LINES );
    glVertex2i( params[0].point->x(), params[0].point->y() );
    glVertex2i( params[1].point->x(), params[1].point->y() );
    glEnd();
    break;
#if 0
  case PdcDrawRect:
    cout << "Draw rect" << endl; break;
  case PdcDrawRoundRect:
    cout << "Draw round rect" << endl; break;
  case PdcDrawEllipse:
    cout << "Draw ellipse" << endl; break;
  case PdcDrawArc:
    cout << "Draw round arc" << endl; break;
  case PdcDrawPie:
    cout << "Draw pie" << endl; break;
  case PdcDrawChord:
    cout << "Draw chord" << endl; break;
  case PdcDrawLineSegments:
    cout << "Draw line segments" << endl; break;
#endif
  case PdcDrawPolyline:
    //    cout << "Draw polyline with " << params[0].ptarr->size() << " points" << endl;
    {
      glColor3ubv( lC::qCubv( face_data_.color_ ) );
      glBegin( GL_LINE_STRIP );
      for ( uint i = 0; i < params[0].ptarr->size(); i++ ) {
	int x, y;
	params[0].ptarr->point( i, &x, &y );
	//	cout << i << ": [ " << x << ", " << y << " ]" << endl;
	glVertex2i( x, y );
      }
      glEnd();
    }
    break;
#if 0
  case PdcDrawPolygon:
    cout << "Draw polygon" << endl; break;
  case PdcDrawCubicBezier:
    cout << "Draw cubic bezier" << endl; break;
  case PdcDrawText:
    cout << "Draw text" << endl; break;
  case PdcDrawTextFormatted:
    cout << "Draw text formatted" << endl; break;
  case PdcDrawPixmap:
    cout << "Draw pixmap" << endl; break;
#endif
  case PdcDrawText2:
    //    cout << "Draw text2" << *params[0].point << ", "
    //    	 << (const char*)*params[1].str
    //    	 << endl;
    view_->font( face_data_ )->draw( params[0].point->x(), -params[0].point->y(),
				     *params[1].str );
    break;
#if 0
  case PdcDrawText2Formatted:
    cout << "Draw text2 formatted" << endl; break;
  case PdcBegin:
    cout << "Begin" << endl; break;
  case PdcEnd:
    cout << "End" << endl; break;
  case PdcSave:
    cout << "Save" << endl; break;
  case PdcRestore:
    cout << "Restore" << endl; break;
  case PdcSetdev:
    cout << "Setdev" << endl; break;
  case PdcSetBkColor:
    cout << "Set background color " << *params[0].color << endl; break;
  case PdcSetBkMode:
    cout << "Set background mode ";
    switch( params[0].ival ) {
    case Qt::TransparentMode:
      cout << "transparent"; break;
    case Qt::OpaqueMode:
      cout << "opaque"; break;
    default:
      cout << "UNKNOWN";
    }
    cout << endl;
    break;
  case PdcSetROP:
    cout << "Set raster op ";
    switch( params[0].ival ) {
    case Qt::CopyROP:
      cout << "copy (dst=src)"; break;
    case Qt::OrROP:
      cout << "or (dst=src|dst)"; break;
    case Qt::XorROP:
      cout << "xor (dst=src^dst)"; break;
    default:
      cout << "other rop: " << params[0].ival; break;
    }
    cout << endl;
    break;
  case PdcSetBrushOrigin:
    cout << "Set brush origin ";
    if ( params[0].point != 0 )
      cout << *params[0].point;
    else
      cout << "[none]";
    cout << endl;
    break;
#endif
  case PdcSetFont:
    //    cout << "Set font" << *params[0].font << endl;
    face_data_.font_ = params[0].font->toString();
    break;

  case PdcSetPen:
    //    cout << "Set pen" << *params[0].pen << endl;
    face_data_.color_ = params[0].pen->color().rgb();
    break;
#if 0
  case PdcSetBrush:
    cout << "Set brush" << *params[0].brush << endl; break;
  case PdcSetTabStops:
    cout << "Set tab stops" << endl; break;
  case PdcSetTabArray:
    cout << "Set tab array" << endl; break;
  case PdcSetUnit:
    cout << "Set unit" << endl; break;
  case PdcSetVXform:
    cout << "Set view transform(?)" << endl; break;
  case PdcSetWindow:
    cout << "Set window" << endl; break;
  case PdcSetViewport:
    cout << "Set viewport" << endl; break;
  case PdcSetWXform:
    cout << "Set window transform:" << params[0].ival << endl; break;
#endif
  case PdcSetWMatrix:
    //    cout << "Set window matrix" << endl << *params[0].matrix;
    glTranslated( params[0].matrix->dx(), -params[0].matrix->dy(), 0 );
    glScaled( params[0].matrix->m11(), params[0].matrix->m22(), 1. );
    break;
#if 0
  case PdcSaveWMatrix:
    cout << "Save window matrix(?)" << endl; break;
  case PdcRestoreWMatrix:
    cout << "Restore window matrix(?)" << endl; break;
  case PdcSetClip:
    cout << "Set clip: " << params[0].ival << endl; break;
  case PdcSetClipRegion:
    cout << "Set clip region" << endl << *params[0].rgn; break;
  default:
    cout << "command unknown: " << command << endl;
#endif
  }
  return true;
}
