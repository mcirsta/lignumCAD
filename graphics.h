/* -*- c++ -*-
 * graphics.h
 *
 * Interfaces for graphics and input objects.
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
#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <GL/gl.h>

#include <qobject.h>

#include <QMenu>

//#include "constants.h"

typedef enum { TRANSIENT, PICK } SelectionMode;
typedef enum { NONE, FIGURE, FACE, EDGE, VERTEX } SelectionEntity;
typedef enum { OFF, INCLUSIVE, EXCLUSIVE } SelectionFilter;
/*!
 * When an InputObject starts to take input, it can indicate to PageView
 * how it wants the raw mouse events filtered. There are two selection
 * modes: transient and pick. There are several geometry enities which can be
 * selected. A specific figure can serve as a filter: either as the only
 * figure to select from (INCLUSIVE) or it can be ignored (EXCLUSIVE).
 */
struct SelectionType {
  /*!
   * \param mode the current selection mode.
   * \param entity the current selection entity.
   * \param filter the current filter.
   * \param figure the figure to filter.
   */
  SelectionType ( SelectionMode mode = TRANSIENT,
		  SelectionEntity entity = FIGURE,
		  SelectionFilter filter = OFF,
		  GLuint figure = 0 )
    : mode_( mode ), entity_( entity ), filter_( filter ), figure_( figure )
  {}

  //! The current mode.
  SelectionMode mode_ : 1;
  //! The current entity.
  SelectionEntity entity_ : 3;
  //! The current filter state.
  SelectionFilter filter_ : 2;
  //! The current figure filter.
  GLuint figure_;
};

// The interaction of the view can be either in 2D or 3D space

typedef enum { SPACE2D, SPACE3D } Space;

class QMouseEvent;
class ViewData;
class SelectedNames;

/*!
 * GraphicsObject is the interface that is presented to the OpenGL view.
 */
class GraphicsObject {
public:
  virtual void draw ( void ) const = 0;
  virtual void select ( SelectionType select_type ) const = 0;
};

/*!
 * A FigureView (may) have subcomponents which can be selected (highlighted)
 * independently (such as the edges of a rectangle or resize handles).
 * They have their own OpenGL selection names and possibly a special
 * cursor shape.
 */
class GraphicsView {
  GLuint selection_name_;
  Qt::CursorShape cursor_shape_;
  bool highlighted_;
  bool activated_;
public:

  GraphicsView ( GLuint selection_name = 0 )
    : selection_name_( selection_name ), cursor_shape_( Qt::ArrowCursor ),
      highlighted_( false ), activated_( false )
  {}

  GLuint selectionName ( void ) const { return selection_name_; }
  void setSelectionName ( GLuint selection_name )
  {
    selection_name_ = selection_name;
  }
  Qt::CursorShape cursorShape ( void ) const { return cursor_shape_; }
  void setCursorShape ( Qt::CursorShape cursor_shape )
  {
    cursor_shape_ = cursor_shape;
  }
  bool isHighlighted ( void ) const { return highlighted_; }
  void setHighlighted ( bool highlight )
  {
    highlighted_ = highlight;
  }
  bool isActivated ( void ) const { return activated_; }
  void setActivated ( bool activate )
  {
    activated_ = activate;
  }
};

/*!
 * InputObject is the interface what is presented to the OpenGL view
 * for accepting user input. It is used at two levels within lC:
 * The page views (sketch view, part view, etc.) generally have an
 * implementation of this interface which is directly invoked by
 * the OpenGL view. The individual Figure views which appear on a
 * page also implement this interface; their methods are invoked
 * by the page view. Generally, if the figure view doesn't want the
 * input, the page will handle it.
 *
 * IO is now promoted to QObject so that it can add actions to the
 * context menu.
 */
class InputObject : public QObject {
  Q_OBJECT
public:
  virtual ~InputObject ( void ) {}
  virtual SelectionType selectionType ( void ) const = 0;
  virtual bool needsPrepressMouseCoordinates ( void ) const = 0;
  virtual void startDisplay ( QMenu* context_menu ) = 0;
  virtual void stopDisplay ( QMenu* context_menu ) = 0;
  virtual void mousePrepress ( QMouseEvent* me, const SelectedNames& selected )=0;
  virtual void mousePress ( QMouseEvent* me, const SelectedNames& selected ) = 0;
  virtual void mouseDrag ( QMouseEvent* me, const SelectedNames& selected ) = 0;
  virtual bool mouseRelease ( QMouseEvent* me, const SelectedNames& selected ) =0;
  virtual void mouseDoubleClick ( QMouseEvent* me ) = 0;
  virtual void keyPress ( QKeyEvent* ke ) = 0;
  virtual void cancelOperation ( void ) = 0;
};
#endif // GRAPHICS_H
