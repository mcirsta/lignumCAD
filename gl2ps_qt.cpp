/*
 * gl2ps_qt.cpp
 *
 * Translate OpenGL drawing commands to Qt drawing commands for printing
 * Copyright (C) 2002 lignum Computing, Inc. <lignumcad@lignumcomputing.com>
 * $Id$
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include <math.h>
#include <iostream>

#include <qpen.h>
#include <qcolor.h>
#include <qpainter.h>
#include <qpaintdevicemetrics.h>
#include <qprinter.h>

#include "gl2ps.h"

extern GL2PScontext gl2ps;
static QPainter* painter;

static float SCALE_X;
static float SCALE_Y;

inline int F2CI ( double x ) { return (int)( 255. * x ); }

inline int F2SIX ( double x ) { return (int)rint( SCALE_X * x ); }
inline int F2SIY ( double y ) { return (int)rint( SCALE_Y * y ); }

inline void PNT ( int i, GL2PSvertex* verts, QPointArray& triangle )
{ triangle.setPoint( i, F2SIX( verts[i].xyz[0] ), F2SIY( verts[i].xyz[1] ) ); }

extern "C" {
  /*!
   * Initialize the callbacks for the GL2PS code to render using
   * Qt commands. The chief thing here is to grab a copy of the
   * Qt QPainter from the GL2PScontext and to compute the conversion
   * factor from OpenGL coordinates to the printer coordinate system.
   *
   * In this round, lignumCAD prints by rendering to a 1x1 pixel pixmap.
   * Actually, nothing is ever really rendered, since GL2PS puts
   * OpenGL into GL_FEEDBACK mode and captures the transformed primitives.
   *
   * The major drawback to this approach is that the aspect ratio of
   * the printed page is probably not going to be the same as the
   * aspect ratio of the desktop window. But, some elements of the
   * view depend on the knowing the boundaries of the drawing surface,
   * such as reference lines and centerlines. Also, there might be
   * figures which are just off-screen but which you'd like to have
   * appear in the printout.
   *
   * So, printing proceeds like this:
   * 1. Create a 1x1 pixel pixmap and a new [Q]GLcontext (done once
   *    when an OpenGLView object is created).
   * 2. When printing, make this context the Current context.
   * 3. Set the projection matrix to be the size of the printed page
   *    times the scale factor of the view. (This is the same way
   *    the projection matrix is computed for the desktop window.)
   *    Set the printer attibutes into the ViewTransform so the elements
   *    of the display can compute proper layouts.
   * 4. Render the picture.
   * 5. As GL2PS produces primitives, they have to be converted from the
   *    OpenGL coordinate system to the printer coordinate system. So,
   *    here's the rub:
   *    Feedback coordinates are in screen coordinates. Since the "screen"
   *    is a 1x1 pixmap, they all lie between 0 and 1. Note that the
   *    aspect ratio of these coordinates is not unity, it is
   *    the printer height/width ratio.
   *    These coordinates can be converted to printer coordinates with
   *    the following equation:
   *      Screen Coord[DOTS] / Screen Resolution[DPI] * Printer Dimension[Inch]
   *        / Projection Dimension[Inch] * Printer Resolution[DPI]
   *         = Printer Coord[DOTS]
   *    However, the Screen Resolution[DPI] = 1 DOT / Projection Dimension[Inch]
   *    So, we can eliminate the screen resolution and the projection
   *    resolution:
   *      Screen Coord[DOTS] * Printer Dimension[Inch] * Printer Resolution[DPI]
   *         = Printer Coord[DOTS]
   *     Further, Printer Dimension * Printer Resolution is just the total
   *     number of dots in the printer's painter's window. So, the conversion
   *     factor is just:
   *       Screen Coord[DOTS] * Printer window[DOTS] = Printer Coord[DOTS]
   *     This also takes care of the aspect ratio of the printer window.
   */
  GLvoid gl2psPrintQtHeader ( void )
  {
    painter = reinterpret_cast<QPainter*>( gl2ps.stream );

    painter->save();

    GLint max_viewport_dims[2];
    glGetIntegerv( GL_MAX_VIEWPORT_DIMS, max_viewport_dims );

    SCALE_X = ( (float)painter->viewport().width() ) / (max_viewport_dims[0]);
    SCALE_Y = ( (float)painter->viewport().height() ) / (max_viewport_dims[1]);
  }

  GLvoid gl2psPrintQtPrimitive ( GLvoid* a, GLvoid* b )
  {
    GL2PSprimitive *prim;
    (void)b;

    prim = *(GL2PSprimitive**) a;

    if(gl2ps.options & GL2PS_OCCLUSION_CULL && prim->depth >= 0.) return;

    switch(prim->type){
    case GL2PS_TEXT :
      break;
    case GL2PS_POINT :
      break;
    case GL2PS_LINE :
      if(gl2ps.shade){
	std::cout << "somehow, we got a shaded line anyway" << endl;
      }
      else{
	QColor color( F2CI( prim->verts[0].rgba[0] ),
		      F2CI( prim->verts[0].rgba[1] ),
		      F2CI( prim->verts[0].rgba[2] ) );
	QPen pen( color );

	switch ( prim->dash ) {
	case GL2PS_LINE_STIPPLE_DASH:
	  pen.setStyle( Qt::DashLine ); break;
	case GL2PS_LINE_STIPPLE_DOT:
	  pen.setStyle( Qt::DotLine ); break;
	case GL2PS_LINE_STIPPLE_DASHDOT:
	  pen.setStyle( Qt::DashDotLine ); break;
	case GL2PS_LINE_STIPPLE_DASHDOTDOT:
	  pen.setStyle( Qt::DashDotDotLine ); break;
	default:
	  pen.setStyle( Qt::SolidLine ); break;
	}

	painter->setPen( pen );
#if 0
	cout << " painter draws raw line: ["
	     << prim->verts[0].xyz[0] << ", " << prim->verts[0].xyz[1] << "], ["
	     << prim->verts[1].xyz[0] << ", " << prim->verts[1].xyz[1] << "]. But scaled its: ["
	     << F2SIX(prim->verts[0].xyz[0]) << ", " << F2SIY(prim->verts[0].xyz[1])
	     << "], ["
	     << F2SIX(prim->verts[1].xyz[0]) << ", " << F2SIY(prim->verts[1].xyz[1])
	     << "]" << endl;
#endif

	painter->drawLine( F2SIX( prim->verts[0].xyz[0] ),
			   F2SIY( prim->verts[0].xyz[1] ),
			   F2SIX( prim->verts[1].xyz[0] ),
			   F2SIY( prim->verts[1].xyz[1] ) );
      }
      break;
    case GL2PS_TRIANGLE :
      if(gl2ps.shade){
	std::cout << "somehow, we got a shaded triangle anyway" << endl;
      }
      else{
#if 0
	cout << " painter draws raw triangle: ["
	     << prim->verts[0].xyz[0] << ", " << prim->verts[0].xyz[1] << "], ["
	     << prim->verts[1].xyz[0] << ", " << prim->verts[1].xyz[1] << "], ["
	     << prim->verts[2].xyz[0] << ", " << prim->verts[2].xyz[1] << "]. But scaled its: ["
	     << F2SIX(prim->verts[0].xyz[0]) << ", " << F2SIY(prim->verts[0].xyz[1])
	     << "], ["
	     << F2SIX(prim->verts[1].xyz[0]) << ", " << F2SIY(prim->verts[1].xyz[1])
	     << "], ["
	     << F2SIX(prim->verts[2].xyz[0]) << ", " << F2SIY(prim->verts[2].xyz[1])
	     << "]" << endl;
#endif

	painter->setPen( Qt::NoPen );
	painter->setBrush( QColor( F2CI( prim->verts[0].rgba[0] ),
				   F2CI( prim->verts[0].rgba[1] ),
				   F2CI( prim->verts[0].rgba[2] ) ) );

	QPointArray triangle(3);

	PNT( 0, prim->verts, triangle );
	PNT( 1, prim->verts, triangle );
	PNT( 2, prim->verts, triangle );

	painter->drawPolygon( triangle );
      }
      break;
    case GL2PS_QUADRANGLE :
      //      gl2psMsg(GL2PS_WARNING, "There should not be any quad left to print");
      break;
    default :
      //      gl2psMsg(GL2PS_ERROR, "Unknown type of primitive to print");
      break;
    }
  }

  GLvoid gl2psPrintQtFooter ( void )
  {
    painter->restore();
  }
}
