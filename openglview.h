/* -*- c++ -*-
 * openglview.h
 *
 * Header for the OpenGLView class.
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

#ifndef OPENGLVIEW_H
#define OPENGLVIEW_H

#include <qcursor.h>

#include <QOpenGLPaintDevice>

#include "graphics.h"
#include "viewdata.h"
#include "arrow.h"
#include "openglbase.h"

class QMenu;
class QAction;
class QPixmap;
class DesignBookView;
class lignumCADMainWindow;
class Ratio;
class PageView;

/*!
 * OpenGLView shows pages from the Design Book.
 */
class OpenGLView : public OpenGLBase {
Q_OBJECT

private:
  static GLuint selection_name_;

  static const GLdouble PICK_APERTURE;
  static const int WHEEL_SCALE = ( 4 * 120 );
  enum MouseMode { MODEL, VIEW };

  // These data items can be used by subclasses.
protected:
  //! The page view to display and deliver input events to.
  PageView* page_view_;
  //! All the info necessary to reconstruct a page's view.
  ViewData view_data_;
  //! Actual scale (may temporarily be different from that specified
  //! in view_data_)
  double scale_;
  //! The width of the view in scale inches.
  double widthIN_;
  //! The height of the view in scale inches.
  double heightIN_;
  //! The viewport (duplicated in pdm_, but we need the array
  //! to call glu[Un]Project.)
  GLint viewport_[4];
  //! The OpenGL PROJECTION matrix. (Actually, this is redundant to
  //! the width and height, but it's faster for restoring the view.)
  GLdouble projection_[16];
  //! The OpenGL MODELVIEW matrix.
  GLdouble modelview_[16];
  //! The inverse of the viewport transformation.
  Space3D::Matrix v_inverse_;
  //! The inverse of the projection matrix.
  Space3D::Matrix p_inverse_;
  //! The inverse of the modelview matrix.
  Space3D::Matrix m_inverse_;
  //! The un-projection matrix.
  Space3D::Matrix mpv_inverse_;
  //! The lower left znear corner of the viewing volume.
  Space3D::Point ll_corner_;
  //! The upper right zfar corner of the viewing volume.
  Space3D::Point ur_corner_;
  //! The circular position (phase) of stipples for current view translation.
  QPoint stipple_phase_;
  //! General purpose arrow
  Space3D::Arrow arrow_;

private:
  QMenu* context_menu_;
  QMenu* view_menu_;
  //  QAction* view_restore_action_;

  SelectionType selection_type_;

  enum MouseMode mouse_mode_;
  QPoint mouse_position_;
  QCursor old_cursor_;

public:
  OpenGLView ( DesignBookView* parent, const char* name, lignumCADMainWindow* lCMW,
	       QGLWidget* share_widget );
  ~OpenGLView ( void );

  //! \return the context menu for this view.
  QMenu* contextMenu ( void ) const { return context_menu_; }

  double scale ( void ) const { return view_data_.scale_; }
  Ratio scaleRatio ( void ) const { return view_data_.scale_; }
  Space3D::Point llCorner ( void ) const { return ll_corner_; }
  Space3D::Point urCorner ( void ) const { return ur_corner_; }
  Space3D::Vector viewNormal ( void ) const { return Space3D::Vector( modelview_[2],
								      modelview_[6],
								      modelview_[10] ); }
  const GLdouble* viewOrientation ( void ) const { return modelview_; }

  /*!
   * Set the view parameters explicitly. Used by Undo/Redo.
   */
  void setViewData ( const ViewData& view_data );

  /*!
   * I guess every attribute which might be used by a drawing routine
   * will have to be reported through here, too. This is the geometry
   * outline color.
   * \return the geometry outline color.
   */
  QColor geometryColor ( void ) const;

  /*!
   * \return the annotation color.
   */
  QColor annotationColor ( void ) const;

  /*!
   * \return the sketch view grid.
   */
  QColor gridColor ( void ) const;

  /*!
   * The constraint primary color.
   * \return the constraint primary color.
   */
  QColor constraintPrimaryColor ( void ) const;

  /*!
   * The constraint secondary color.
   * \return the constraint secondary color.
   */
  QColor constraintSecondaryColor ( void ) const;

  /*!
   * \return the dimension font.
   */
  QString dimensionFont ( void ) const;

  /*!
   * \return the arrow head length.
   */
  double arrowHeadLength ( void ) const;

  /*!
   * \return the arrow head length.
   */
  Ratio arrowHeadWidthRatio ( void ) const;

  /*!
   * \return the arrow head style.
   */
  lC::ArrowHeadStyle arrowHeadStyle ( void ) const;

  /*!
   * \return the dimension clearance length.
   */
  double clearance ( void ) const;

  /*!
   * \return the dimension extension line offset.
   */
  double extensionLineOffset ( void ) const;

  /*!
   * \return the annotation font.
   */
  QString annotationFont ( void ) const;

  /*!
   * \return the handle size.
   */
  double handleSize ( void ) const;
  /*!
   * Format the given value using the current length units representation.
   * \param value length value to format.
   * \return string representation of length.
   */
  QString format ( double value ) const;
  /*!
   * \return the circular position (phase) of stipples.
   */
  QPoint stipplePhase ( void ) const { return stipple_phase_; }
  /*!
   * Take a QPoint in screen coordinates (usually from a QMouseEvent)
   * and determine where it is in a 2D model view.
   * \param p a point in screen coordinates.
   * \return the coordinates in the 2D model view.
   */
  Space2D::Point unproject2D ( const QPoint& p ) const;
  /*!
   * Take a QPoint in screen coordinates (usually from a QMouseEvent)
   * and determine where it is in a 3D model view.
   * \param p a point in screen coordinates.
   * \return the coordinates in the 3D model view.
   */
  Space3D::Point unproject3D ( const QPoint& p ) const;
  /*!
   * Take a Point in (sub)pixel coordinates and determine where it
   * is in a 3D model view.
   * \param p a point in (sub)pixel coordinates.
   * \return the coordinates in the 3D model view.
   */
  Space3D::Point unproject3D ( const Space2D::Point& p ) const;
  /*!
   * Convert a 2D point in model view coordinates into a screen coordinate.
   * \param p point in 2D model view coordinates.
   * \return point in screen coordinates.
   */
  QPoint project2D ( const Space2D::Point& p ) const;

  /*!
   * Here is a slightly different kind of thing: resize handles are
   * now a method in the view. The 3D handle is so complicated that
   * we need to limit computing it to only when the view scale changes.
   * \param position handle center in 2D coordinates.
   * \param mode the rendering mode for the handle.
   */
  void handleDraw ( const Space2D::Point& position, lC::Render::Mode mode ) const;
  /*!
   * Here is a slightly different kind of thing: resize handles are
   * now a method in the view. The 3D handle is so complicated that
   * we need to limit computing it to only when the view scale changes.
   * \param position handle center in 3D coordinates.
   * \param mode the rendering mode for the handle.
   */
  void handleDraw ( const Space3D::Point& position, lC::Render::Mode mode ) const;

  /*!
   * Render the handle in selection mode.
   * \param position handle center in 2D coordinates.
   */
  void handleSelect ( const Space2D::Point& position ) const;
  /*!
   * Render the handle in selection mode.
   * \param position handle center in 3D coordinates.
   */
  void handleSelect ( const Space3D::Point& position ) const;
  /*!
   * Set the page to display and to deliver input to.
   * \param page_view page to display.
   */
  virtual void setPageView ( PageView* page_view );

  /*!
   * \return the currently displayed Page view.
   */
  PageView* pageView ( void ) const { return page_view_; }

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
  virtual QRect newWindow ( const Space2D::Point& origin, const Space2D::Vector& size );
  /*!
   * Set the model view back to its default state.
   */
  virtual void resetWindow ( void );

  /*!
   * Generally used to signal when a sub-object of the page view has
   * changed and the input attributes (selection mode, wants prepress
   * coordinates) may have changed.
   */
  void inputObjectChanged ( void );

  void select( QMouseEvent* me, SelectedNames& selected_names );

  void redisplay ( void );

  virtual bool printing ( void ) const { return false; }

  static GLuint genSelectionName ( void );

signals:
  void scale ( const Ratio& );
  void rotation ( const GLdouble* modelview );

public slots:

private slots:
  void restoreView ( void );
  void zoomIn ( void );
  void zoomOut ( void );
  void toggleCSys ( void );
  void viewLeft ( void );
  void viewRight ( void );
  void viewFront ( void );
  void viewBack ( void );
  void viewBottom ( void );
  void viewTop ( void );
  void updateAttributes ( void );
  void updateBackground ( void );

protected:
  virtual void initializeGL ( void );
  virtual void resizeGL ( int w, int h );
  virtual void paintGL ( void );
  void mousePressEvent ( QMouseEvent* me );
  void mouseReleaseEvent ( QMouseEvent* me );
  void mouseMoveEvent ( QMouseEvent* me );
  void mouseDoubleClickEvent ( QMouseEvent* me );
  void wheelEvent ( QWheelEvent* we );
  void keyPressEvent ( QKeyEvent* ke );
  void contextMenuEvent ( QContextMenuEvent* cme );
private:
  static GLushort phase ( GLdouble c );
  static GLushort phase ( int c );
  static QPoint toPhase ( const QPoint& p );
  void set2DView ( void );
  void set3DView ( void );
  void pan2D ( const QPoint& mouse_delta );
  void pan3D ( const QPoint& mouse_delta );
  void spin3D ( const QPoint& mouse_delta );
  void scale2D ( void );
  void scale3D ( void );
  void setViewportInverse ( void );
  void setProjectionInverse ( void );
  void setModelviewInverse ( void );
  /*!
   * Draw a coordinate system at the origin of the view.
   */
  void csys ( void );
};

#endif // OPENGLVIEW_H
