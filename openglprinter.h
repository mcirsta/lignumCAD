/* -*- c++ -*-
 * openglprinter.h
 *
 * Header for the OpenGLPrinter class.
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

#ifndef OPENGLPRINTER_H
#define OPENGLPRINTER_H

#include "openglview.h"

/*!
 * OpenGLPrinter prints pages from the Design Book (I hope).
 */
class OpenGLPrinter : public OpenGLView {
  Q_OBJECT

public:
  OpenGLPrinter ( DesignBookView* parent, const char* name,
		  QGLWidget* share_widget );
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

  void print ( PageView* page_view, QPainter& painter, int page_no, int pages );
  void exportPage ( PageView* page_view, OpenGLView* view,
		    const QString& exportFilename, int page_no, int pages );
private:
  /*!
   * Draw the box containing the page metadata. Someday, the user will
   * more control over the format and the contents...
   * \param page_no the number of the current page.
   * \param pages total number of pages.
   */
  void drawFrame ( int page_no, int pages );

  //! Save the original scale when a subwindow is created.
  Ratio old_scale_;
};

#endif // OPENGLPRINTER_H
