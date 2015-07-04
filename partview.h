/* -*- c++ -*-
 * partview.h
 *
 * Header for the PartView class
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
#ifndef PARTVIEW_H
#define PARTVIEW_H

#include "dburl.h"
#include "part.h"
#include "pageview.h"

class PageInfoDialog;
class NewPartWizard;
class MaterialDialog;

class PartView : public PageView {
  Q_OBJECT

  //! Part this view references.
  Part* part_;

  int tabId;
  QString tabText;
  QPixmap tabIcon;
  ListViewItem* list_view_item_;

  QMenu* context_menu_;
  QAction* wireframe_id_;
  QAction* hidden_id_;
  QAction* solid_id_;
  QAction* texture_id_;

  //! Generic initial part form selection.
  static NewPartWizard* new_part_wizard_;

  //! The material dialog.
  static MaterialDialog* material_dialog_;

  static PageInfoDialog* config_dialog_;

public:
  PartView ( Part* part, DesignBookView* parent );
  PartView ( DesignBookView* parent, const QDomElement& xml_rep );
  ~PartView ( void );

  /*!
   * The part view has a very complicated configuration dialog (namely,
   * the new part wizard): the the user has to select a template part
   * and specify its dimensions.
   * \return false if the user cancels the new part wizard; return true
   * otherwise.
   */
  bool configure ( void );

  //! By parent(), we mean the lignumCAD class hierarchy parent (not the
  //! QObject's parent)(I suppose there's no reason to waste perfectly
  //! good parent and child members in QObject, though.)
  DesignBookView* parent ( void ) const { return PageView::parent(); } 

  //! \return part refered to by this view.
  Part* part ( void ) const { return part_; }

  // Implementation of PageView interface

  //! \return the tab for this page view.
  int tabIdx ( void ) const { return tabId; }
  //! \return the list view item for this page view.
  ListViewItem* listViewItem ( void ) const { return list_view_item_; }

  uint id ( void ) const { return part_->id(); }
  QString name ( void ) const { return part_->name(); }
  QString type ( void ) const { return part_->type(); }

  DBURL dbURL ( void ) const { return part_->dbURL(); }
  QVector<uint> ID ( void ) const { return part_->ID(); }

  void cancelOperation ( void );

  CreateObject* memento ( void );

  void show ( void ) const;
  void write ( QDomElement& xml_rep ) const;

  void pasteFigure ( const QDomElement& xml_rep );

  // Implementation of remaining InputObject interface
  void startDisplay ( QMenu* context_menu );
  void stopDisplay ( QMenu* context_menu );

  // Implementation of GraphicsObject interface

  Space space ( void ) const { return SPACE3D; }
  void draw ( void ) const;
  void select ( SelectionType select_type ) const;

signals:
  void orientationChanged ( const GLdouble* modelview );
  void orientationChangedHidden ( const GLdouble* modelview );

protected:
  //! \return true if page supports cut and paste (which parts don't currently)
  bool canCutPaste ( void ) const { return false; }

private:
  //! The usual init routine.
  void init ( void );

private slots:
  void setName ( const QString& name );
  /*!
   * The name in the list view has changed. This has to be checked for
   * redundancy before it can be allowed.
   * \param name new name for list view edit.
   */
  void listNameChanged ( const QString& name );
  void updateName ( const QString& name );
  void toggleRenderStyle ( QAction* id );
  void setMaterial ( void );
};

#endif // PARTVIEW_H
