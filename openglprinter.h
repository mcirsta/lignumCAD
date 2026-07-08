/* -*- c++ -*-
 * openglprinter.h
 *
 * Header for the OpenGLPrinter class.
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

#ifndef OPENGLPRINTER_H
#define OPENGLPRINTER_H

#include <QString>

#include "openglview.h"

class QOffscreenSurface;
class QOpenGLContext;

struct GL2PSPageSettings {
  double paperWidthPts = 0.0;
  double paperHeightPts = 0.0;
  double marginPts = 36.0;
  int outputDpi = 72;
  bool color = true;
  bool drawFrame = false;
};

/*!
 * OpenGLPrinter prints pages from the Design Book (I hope).
 */
class OpenGLPrinter : public OpenGLView {
  Q_OBJECT

public:
  OpenGLPrinter ( DesignBookView* parent, const char* name,
		  QOpenGLWidget* share_widget );
  ~OpenGLPrinter ( void );

  /*!
   * Get the OGLFT Face corresponding to the supplied key. If there
   * is no such face, one is created. Normally, if the face or
   * point size elements of face_data are empty, the default from
   * OpenGLGlobals is used. In particular, OpenGL Printer always
   * asks for an outline Face.
   * \param face_data key to look up Face object.
   * \return selected face object.
   */
  OGLFT::Face* font ( const FaceData& face_data );
  /*!
   * Change the model view temporarily to the subwindow described
   * by the parameters. Used primarily (exclusively?) to establish
   * a custom window for annotation text. Returns a QRect containing
   * the window in screen coordinates.
   * \param origin upper left corner of window.
   * \param size width and height of the window.
   * \return the position and size of the window in screen (pixel)
   * coordinates.
   */
  QRect newWindow ( const Space2D::Point& origin, const Space2D::Vector& size );
  /*!
   * Set the model view back to its default state.
   */
  void resetWindow ( void );

  bool printing ( void ) const { return true; }

  //! Paper is white: draw geometry in black regardless of the screen
  //! color scheme.
  QColor geometryColor ( void ) const { return QColor( Qt::black ); }
  //! Paper is white: draw annotations in black regardless of the screen
  //! color scheme.
  QColor annotationColor ( void ) const { return QColor( Qt::black ); }

  bool printToPdf ( PageView* page_view, const GL2PSPageSettings& settings,
		    const QString& pdf_path, int page_no, int pages,
		    QString* error = 0 );
  bool exportPage ( PageView* page_view, OpenGLView* source_view,
		    const GL2PSPageSettings& settings,
		    const QString& pdf_path, int page_no, int pages,
		    QString* error = 0 );
private:
  /*!
   * Lazily create the offscreen OpenGL context used for gl2ps capture.
   * A hidden QOpenGLWidget never initializes its own context under Qt 6,
   * so rendering happens in a QOffscreenSurface context shared with the
   * on-screen view instead.
   * \param error receives a description of the failure, if any.
   * \return true if the offscreen context is current.
   */
  bool makeRenderContextCurrent ( QString* error );

  /*!
   * Determine the extent of the page's drawn content by rendering it
   * once in OpenGL feedback mode over a very large projection and
   * reading back the captured vertices. Requires the render context to
   * be current and page_view_ to be set. 2D pages only.
   * \param ll receives the lower left corner of the content in model units.
   * \param ur receives the upper right corner of the content in model units.
   * \return true if any content was captured.
   */
  bool measureContentBounds2D ( Space2D::Point& ll, Space2D::Point& ur );

  bool renderPagePdf ( PageView* page_view, OpenGLView* source_view,
		       const GL2PSPageSettings& settings,
		       const QString& pdf_path, int page_no, int pages,
		       QString* error );
  /*!
   * Draw the box containing the page metadata. Someday, the user will
   * more control over the format and the contents...
   * \param page_no the number of the current page.
   * \param pages total number of pages.
   */
  void drawFrame ( int page_no, int pages );

  //! Save the original scale when a subwindow is created.
  Ratio old_scale_;
  //! PDF/gl2ps output grid, in points per inch.
  int output_dpi_ = 72;
  //! The on-screen view whose context the offscreen context shares
  //! (for display lists and textures).
  QOpenGLWidget* share_widget_;
  //! Offscreen context in which gl2ps feedback capture runs.
  QOpenGLContext* render_context_ = nullptr;
  //! Surface backing render_context_.
  QOffscreenSurface* render_surface_ = nullptr;
};

#endif // OPENGLPRINTER_H
