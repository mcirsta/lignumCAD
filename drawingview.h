/* -*- c++ -*-
 * drawingview.h
 *
 * Header for the DrawingView class
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
#ifndef DRAWINGVIEW_H
#define DRAWINGVIEW_H

#include "dburl.h"
#include "pageview.h"
#include "drawing.h"

class PageInfoDialog;

class DrawingView : public PageView {
  Q_OBJECT

  Drawing* drawing_;

  QTab* tab_;
  ListViewItem* list_view_item_;

public:
  DrawingView ( Drawing* drawing, DesignBookView* parent );
  DrawingView ( DesignBookView* parent, const QDomElement& xml_rep );
  ~DrawingView ( void );

  /*!
   * For the drawing view, there isn't much to configure; just the name
   * of the page.
   * \return false if the user cancels the rename dialog; return true
   * otherwise.
   */
  bool configure ( void );

  //! By parent(), we mean the lignumCAD class hierarchy parent (not the
  //! QObject's parent)(I suppose there's no reason to waste perfectly
  //! good parent and child members in QObject, though.)
  DesignBookView* parent ( void ) const { return PageView::parent(); }

  //! \return drawing refered to by this view.
  Drawing* drawing ( void ) const { return drawing_; }

  // Implementation of PageView interface

  //! \return the tab for this page view.
  QTab* tab ( void ) const { return tab_; }
  //! \return the list view item for this page view.
  ListViewItem* listViewItem ( void ) const { return list_view_item_; }

  uint id ( void ) const { return drawing_->id(); }
  QString name ( void ) const { return drawing_->name(); }
  QString type ( void ) const { return drawing_->type(); }

  DBURL dbURL ( void ) const { return drawing_->dbURL(); }
  QValueVector<uint> ID ( void ) const { return drawing_->ID(); }

  void cancelOperation ( void );

  CreateObject* memento ( void );

  void show ( void ) const;
  void write ( QDomElement& xml_rep ) const;

  void pasteFigure ( const QDomElement& xml_rep );

  // Implementation of remaining InputObject interface
  void startDisplay ( QPopupMenu* context_menu );
  void stopDisplay ( QPopupMenu* context_menu );

  // Implementation of GraphicsObject interface

  Space space ( void ) const { return SPACE2D; }
  void draw ( void ) const;
  void select ( SelectionType select_type ) const;
protected:
  //! \return true if page supports cut and paste (which drawings don't currently)
  bool canCutPaste ( void ) const { return false; }

private slots:
  void setName ( const QString& name );
  /*!
   * The name in the list view has changed. This has to be checked for
   * redundancy before it can be allowed.
   * \param name new name for list view edit.
   */
  void listNameChanged ( const QString& name );
  void updateName ( const QString& name );

private:
  static PageInfoDialog* config_dialog_;

  //! Canonical init routine.
  void init ( void );
};

#endif // DRAWINGVIEW_H
