/* -*- c++ -*-
 * subassemlblyview.h
 *
 * Header for the SubassemblyView class
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
#ifndef SUBASSEMBLYVIEW_H
#define SUBASSEMBLYVIEW_H

#include "figureview.h"

#include <memory>

class PageView;
class Subassembly;
class SubassemblyModifyInput;
namespace Space3D {
  class OCSubassemblyDraw;
  class Dimension;
}
class AssemblyConstraint;
class OffsetInfoDialog;

class SubassemblyView : public Space3D::FigureView {
  Q_OBJECT
public:
  SubassemblyView ( Subassembly* subassembly, PageView* parent );
  SubassemblyView ( const QDomElement& xml_rep, PageView* parent );
  ~SubassemblyView ( void );

  Subassembly* subassembly ( void ) const { return subassembly_; }

  // Implementation of GraphicsObject interface
  void draw ( void ) const;
  void select ( SelectionType select_type ) const;

  // Implementation of FigureView interface

  //! \return the list view item for this page view.
  ListViewItem* listViewItem ( void ) const { return list_view_item_; }

  uint id ( void ) const;
  QString name ( void ) const;
  QString type ( void ) const;
  DBURL dbURL ( void ) const;
  /*!
   * Return a string which represents the selection for the entity type.
   * \param selection_name item's selection name.
   * \param entity entity selected for in view.
   * \return string representing selection.
   */
  QString selectionText ( const std::vector<GLuint>& selection_name,
			  SelectionEntity entity ) const;
  View* lookup ( QStringList& path_components ) const;
  void lookup ( QVector<uint>& id_path, std::vector<GLuint>& name_path ) const;
  QString geometry ( const std::vector<GLuint>& selection_name ) const;
  QVector<uint> geomPath ( const std::vector<GLuint>& selection_name ) const;

  void setHighlighted ( bool highlight, SelectionEntity entity,
			const std::vector<GLuint>& items );
  void setActivated( bool activate, SelectionEntity entity,
		     const std::vector<GLuint>& items );

  InputObject* createInput ( void ) { return 0; }
  InputObject* modifyInput ( void );

  CreateObject* memento ( void );

  uint dimensionPickCount ( void ) const { return 0; }
  void setDimensionView ( const QDomElement& /*xml_rep*/ ) {}

  /*!
   * React to changes in the view attributes. Experience shows that this
   * mostly has to do with rendering text.
   */
  void viewAttributeChanged ( void );
  /*!
     * Create an XML representation of this view (only) suitable for
     * saving in a file with the rest of the model information.
     * \param xml_rep the XML representation of the model to append
     * this representation to.
     */
  void write ( QDomElement& xml_rep ) const;
  /*!
     * Create an XML representation for this view *AND* its object suitable
     * for use as a clipboard selection.
     * \param xml_rep the XML representation of the selection to append
     * this representation to.
     */
  void copyWrite ( QDomElement& /*xml_rep*/ ) const {}

  void editInformation ( void );

signals:
  void destroyedSubassembly();

private:
  void init ( void );
  /*!
   * Extract the items which can be renamed from this id path.
   * \param surface_id id path of a constraint dependency.
   */
  void addDependency ( const QVector<uint>& surface_id );

private slots:
  void updateModelName ( const QString& );
  void updateConstraintName ( const QString& );
  void updateLocation ( void );
  void updateMaterial ( void );
  void updateTessellation ( void );
  void updateViewNormal ( const GLdouble* );
  void updateNewConstraint ( const AssemblyConstraint* );
  void updateChangedConstraint ( const AssemblyConstraint*,
				 const AssemblyConstraint* );
  void updateChangedOffset ( const AssemblyConstraint* );
  void updateCanceledConstraint ( void );

private:
  Subassembly* subassembly_;

  //! Model hierarchy view.
  ListViewItem* list_view_item_;
  //! (Recursively) draws the subassembly.
  Space3D::OCSubassemblyDraw* drawer_;
  //! (for now) The dimension views for locating this subassembly with offsets
  Space3D::Dimension* dimensions_[3];
  //! OpenGL selection name for dimensions.
  GLuint dimension_name_;
  //! Modify input object.
  SubassemblyModifyInput* modify_input_;
  //! List of other subassemblies this subassembly depends on via constraints.
  QList<const std::shared_ptr<ModelItem>> dependencies_;
  //! Offset constraint size editor.
  static OffsetInfoDialog* offset_info_dialog_;
};

#endif // SUBASSEMBLYVIEW_H
