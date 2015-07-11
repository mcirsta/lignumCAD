/* -*- c++ -*-
 * assemlblyview.h
 *
 * Header for the AssemblyView class
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
#ifndef ASSEMBLYVIEW_H
#define ASSEMBLYVIEW_H

#include "dburl.h"
#include "pageview.h"
#include "assembly.h"
#include "assemblyconfigdialog.h"
#include "assemblyadddialog.h"
#include "assemblyconstraint.h"
#include "assemblyconstraintform.h"
#include "offsetinfodialog.h"

#include <QDockWidget>

class SubassemblyView;

namespace Space3D {
  class FigureView;
  class OCHiddenDraw;
}

class ConstraintInput;
class OffsetConstraintInput;
class ModelDeleteInput;

class AssemblyView : public PageView {
  Q_OBJECT

  Assembly* assembly_;

  QString tabText;
  QPixmap tabIcon;
  ListViewItem* list_view_item_;

  SubassemblyView* current_view_;

  QMenu* context_menu_;
  QAction* wireframe_id_;
  QAction* hidden_id_;
  QAction* solid_id_;
  QAction* texture_id_;
  QAction* separator_id_;

public:
  AssemblyView ( Assembly* assembly, DesignBookView* parent );
  AssemblyView ( DesignBookView* parent, const QDomElement& xml_rep );
  ~AssemblyView ( void );

  /*!
   * For the assembly view, there isn't much to configure; just the name
   * of the page.
   * \return false if the user cancels the rename dialog; return true
   * otherwise.
   */
  bool configure ( void );

  /*!
   * The user may decide to cancel a constraint half-way through.
   */
  void cancelConstraint ( void );

  //! By parent(), we mean the lignumCAD class hierarchy parent (not the
  //! QObject's parent)(I suppose there's no reason to waste perfectly
  //! good parent and child members in QObject, though.)
  DesignBookView* parent ( void ) const { return PageView::parent(); }

  //! \return assembly refered to by this view.
  Assembly* assembly ( void ) const { return assembly_; }
 
  // Implementation of PageView interface

  //! \return the tab for this page view.
  int tabIdx ( void ) const { return tabIndex; }
  //! \return the list view item for this page view.
  ListViewItem* listViewItem ( void ) const { return list_view_item_; }

  uint id ( void ) const { return assembly_->id(); }
  QString name ( void ) const { return assembly_->name(); }
  QString type ( void ) const { return assembly_->type(); }

  DBURL dbURL ( void ) const { return assembly_->dbURL(); }
  QVector<uint> ID ( void ) const { return assembly_->ID(); }

  CreateObject* memento ( void );

  void show ( void ) const;
  void write ( QDomElement& xml_rep ) const;

  void pasteFigure ( const QDomElement& xml_rep );

  // Implementation of remaining InputObject interface
  SelectionType defaultSelectionType ( void ) const;
  void startDisplay ( QMenu* context_menu );
  void stopDisplay ( QMenu* context_menu );

  // Implementation of GraphicsObject interface

  Space space ( void ) const { return SPACE3D; }
  void draw ( void ) const;
  void select ( SelectionType select_type ) const;

  void constraintComplete ( void );
  void constraintCanceled ( void );
  void placementComplete ( void );

  int editOffset ( const QString& type, double& offset ) const;
  void editConstraint ( const AssemblyConstraint* constraint );
  void editConstraints ( SubassemblyView* subassembly_view );

  void cancelDeleteOperation ( void );
protected:
  //! \return true if page supports cut and paste (which assemblies don't currently)
  bool canCutPaste ( void ) const { return false; }
private:
  void init ( void );
  void updateConstraintLabel ( void );

public slots:
  void updateHiddenModel ( void );
  void constraintAdded ( const AssemblyConstraint* constraint );
  void constraintModified ( const AssemblyConstraint* old_constraint,
			    const AssemblyConstraint* new_constraint );

signals:
  void orientationChanged ( const GLdouble* modelview );
  //  void orientationChangedHidden ( const GLdouble* modelview );

private slots:
  void setName ( const QString& name );
  /*!
   * The name in the list view has changed. This has to be checked for
   * redundancy before it can be allowed.
   * \param name new name for list view edit.
   */
  void listNameChanged ( const QString& name );
  void updateName ( const QString& name );
  void toggleRenderStyle (QAction *id );
  void cancelOperation ( void );
  void addModel ( void );
  void deleteModel ( void );
  void mate ( void );
  void align ( void );
  void mateOffset ( void );
  void alignOffset ( void );
  void subassemblyDestroyed( void );
  void reeditSubassembly ( const AssemblyConstraint* old_constraint,
			   const AssemblyConstraint* new_constraint );
  void updateHiddenOrientation ( const GLdouble* modelview );
  void constraintOffsetChanged ( const AssemblyConstraint* constraint );

private:
  bool constraint_complete_;

  ConstraintInput* constraint_input_;
  OffsetConstraintInput* offset_constraint_input_;
  ModelDeleteInput* model_delete_input_;

  Space3D::OCHiddenDraw* hidden_drawer_;

  QVector<QStringList> constraints_text_;

  static AssemblyConfigDialog* config_dialog_;
  static QDockWidget* constraint_dock_;
  static AssemblyAddDialog* add_dialog_;
  static AssemblyConstraintForm* constraint_form_;
  static OffsetInfoDialog* offset_info_dialog_;

  int tabIndex;
};

#endif // ASSEMBLYVIEW_H
