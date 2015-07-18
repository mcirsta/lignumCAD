/* -*- c++ -*-
 * sketchview.h
 *
 * Header for the SketchView class
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
#ifndef SKETCHVIEW_H
#define SKETCHVIEW_H

#include "dburl.h"
#include "sketch.h"
#include "pageview.h"

class lignumCADMainWindow;
class PageInfoDialog;

namespace Space2D {
  class DimensionCreateInput;
  class AlignmentCreateInput;
  class ConstraintDeleteInput;
}

class SketchView : public PageView {
Q_OBJECT
  Sketch* sketch_;

  Space2D::DimensionCreateInput* dimension_create_input_;
  Space2D::AlignmentCreateInput* alignment_create_input_;
  Space2D::ConstraintDeleteInput* constraint_delete_input_;

  QPixmap tabIcon_;
  QString tabText_;
  ListViewItem* list_view_item_;

  QMenu* context_menu_;
  QAction* wireframe_id_;
  QAction* solid_id_;
  QAction* texture_id_;
  QAction* cancel_separator_id_;

  static GLushort* dashes[4];

  static PageInfoDialog* config_dialog_;

  int currentTab;

public:
  SketchView ( Sketch* sketch, DesignBookView* parent );
  SketchView ( DesignBookView* parent, const QDomElement& xml_rep );
  ~SketchView ( void );

  /*!
   * For the sketchview, there isn't much to configure; just the name
   * of the page.
   * \return false if the user cancels the rename dialog; return true
   * otherwise.
   */
  bool configure ( void );

  //! By parent(), we mean the lignumCAD class hierarchy parent (not the
  //! QObject's parent)(I suppose there's no reason to waste perfectly
  //! good parent and child members in QObject, though.)
  //! \return parent DesignBookView.
  DesignBookView* parent ( void ) const { return PageView::parent(); } 
  //! \return sketch refered to by this view.
  Sketch* sketch ( void ) const { return sketch_; }

  // Implementation of PageView interface

  //! \return the tab for this page view.
  int tabIdx ( void ) const { return currentTab; }
  //! \return the list view item for this page view.
  ListViewItem* listViewItem ( void ) const { return list_view_item_; }

  uint id ( void ) const { return sketch_->id(); }
  QString name ( void ) const { return sketch_->name(); }
  QString type ( void ) const { return sketch_->type(); }

  DBURL dbURL ( void ) const { return sketch_->dbURL(); }
  QVector<uint> ID ( void ) const { return sketch_->ID(); }


  CreateObject* memento ( void );

  void show ( void ) const;
  void write ( QDomElement& xml_rep ) const;

  void pasteFigure ( const QDomElement& xml_rep );

  // Implementation of remaining InputObject interface
  void startDisplay ( QMenu* context_menu );
  void stopDisplay ( QMenu* context_menu );

  // Implementation of GraphicsObject interface

  Space space ( void ) const { return SPACE2D; }
  void draw ( void ) const;
  void select ( SelectionType select_type ) const;

protected:
  //! \return true if page supports cut and paste (which a sketch can).
  bool canCutPaste ( void ) const { return true; }

private:
  void init ( void );

public slots:
  void setName ( const QString& name );
  void cancelOperation ( void );

private slots:
  /*!
   * The name in the list view has changed. This has to be checked for
   * redundancy before it can be allowed.
   * \param name new name for list view edit.
   */
  void listNameChanged ( const QString& name );
  void updateName ( const QString& name );
  void toggleRenderStyle (QAction *id );
  void createRectangle ();
  void createReferenceLine ();
  void createCenterline ();
  void createAnnotation ();
  void createDimension ();
  void createAlignment ();
  void deleteConstraint ();
};

#endif // SKETCHVIEW_H
