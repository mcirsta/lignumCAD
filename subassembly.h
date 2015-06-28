/* -*- c++ -*-
 * subassembly.h
 *
 * Header for the Subassembly class
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
#ifndef SUBASSEMBLY_H
#define SUBASSEMBLY_H
#include <qdom.h>

#include "figure.h"
#include "assemblyconstraint.h"

#include <TopLoc_Location.hxx>

class gp_Dir;
class gp_Vec;
class gp_Trsf;
class TopoDS_Compound;
class TopoDS_Shape;

class Subassembly : public Space3D::Figure {
  Q_OBJECT
public:
  /*!
   * Contruct a subassembly.
   * \param id id of assembly.
   * \param name identifier within the assembly.
   * \param subassembly reference to part or assembly being included in parent
   * \param parent should be an Assembly.
   */
  Subassembly ( uint id, const QString& name, Space3D::Page* subassembly,
		Space3D::Page* parent );
  /*!
   * Contruct a subassembly from its XML representation.
   * \param id id of assembly.
   * \param xml_rep XML representation.
   * \param parent should be an Assembly.
   */
  Subassembly ( uint id, const QDomElement& xml_rep, Space3D::Page* parent );
  //! Destructor does nothing (execpt propagate up the chain?)
  virtual ~Subassembly ( void ) {}
  //! \return the subassembly page.
  Space3D::Page* subassembly ( void ) const { return subassembly_; }
  //! \return transformation of this subassembly within the assembly.
  const TopLoc_Location& location ( void ) const { return location_; }
  //!  \return a reference to the constraints object.
  AssemblyConstraintManager& constraints ( void ) { return constraints_; }
  /*!
   * The constraint manager calls this method when the user adds a new constraint.
   * \param constraint the new (empty) constraint.
   */
  void newConstraint ( const AssemblyConstraint* constraint );
  /*!
   * The constraint manager calls this method when the current constraint is
   * modified.
   * \param old_constraint the old contents of the constraint.
   * \param new_constraint the new contents of the constraint.
   */
  void changeConstraint ( const AssemblyConstraint* old_constraint,
			  const AssemblyConstraint* new_constraint );
  /*!
   * Try this: distinguish the change in offset distance of an offset
   * constraint from a change in references.
   * \param constraint constraint whose offset has changed.
   */
  void changeConstraintOffset ( const AssemblyConstraint* constraint );
  /*!
   * The constraint manager call this method when its current constraint is
   * canceled before it is completed.
   */
  void cancelLast ( void );
  /*!
   * The constraint manager calls this method when it finds a valid face
   * on which the position of the subassembly now depends due to a constraint.
   * \param surface_id id path to the surface on which the position
   * of this subassembly depends.
   */
  void addDependency ( const QValueVector<uint>& surface_id );
  /*!
   * Rotate the subassembly about the given direction. The origin of the
   * subassembly is (effectively) translated to the origin, rotated, and then
   * translated back to its original position.
   * \param direction the direction around which the solid is rotated.
   * \param angle angle of rotation (radians).
   */
  void rotate ( gp_Dir direction, double angle );
  /*!
   * Translate the subassembly by the given displacement.
   * \param displacement distance to move the solid.
   */
  void translate ( gp_Vec displacement );
  /*!
   * Create a full XML representation of this object suitable for
   * saving in a file with the rest of the model information.
   * \param xml_rep the XML representation of the model to append
   * this representation to.
   */
  void write ( QDomElement& xml_rep ) const;
  /*!
   * Create an XML representation of the object suitable for use
   * as a clipboard selection. Generally, this will be simplified
   * compared to the full representation.
   * \param xml_rep the XML representation of the selection to append
   * this representation to.
   */
  void copyWrite ( QDomElement& xml_rep ) const;
#if 0
  Vertex closestVertex( Point p, Curve* curve ) const;
#endif
  ModelItem* lookup ( QStringList& path_components ) const;
  ModelItem* lookup ( QValueVector<uint>& id_path ) const;
  /*!
   * Find the OpenCASCADE type of the given surface (identified by
   * path).
   * \param path_components path to solid.
   * \return OpenCASCADE standard type identifier.
   */
  Handle(Standard_Type) lookupType ( QStringList& path_components ) const;
  /*!
   * Find the OpenCASCADE type of the given surface (identified by
   * path).
   * \param id_path id path to solid.
   * \return OpenCASCADE standard type identifier.
   */
  Handle(Standard_Type) lookupType ( QValueVector<uint>& id_path ) const;
  /*!
   * Look up the topology of an OpenCASCADE shape. Should return
   * a shape which has the proper Location based on its traversal
   * of the assembly hierarchy.
   * \param path_components string list of path components.
   */
  TopoDS_Shape lookupShape ( QStringList& path_components ) const;
  /*!
   * Look up the topology of an OpenCASCADE shape. Should return
   * a shape which has the proper Location based on its traversal
   * of the assembly hierarchy.
   * \param id_path id path to solid.
   */
  TopoDS_Shape lookupShape ( QValueVector<uint>& id_path ) const;
  /*!
   * Determine if any of the constraints in this subassembly reference a surface
   * (or other geometry) of the given subassembly.
   * \param target subassembly to check.
   * \return true if this subassembly references a surface of the given subassembly.
   */
  bool referenced ( const Subassembly* target ) const;
  /*!
   * Construct a string path to the specified item. May be either the
   * subassembly itself or one of its subassemblies.
   * \param id_path id path to item.
   * \return string encoded path to item.
   */
  QString idPath ( QValueVector<uint> id_path ) const;
  /*!
   * Construct the id path to the specified item. May be either the subassembly
   * itself or one of its subassemblies.
   * \param path_components broken down components of the orignal DBURL with
   * paths to current item removed.
   * \param id_path id path to update with id's.
   */
  void pathID ( QStringList& path_components, QValueVector<uint>& id_path ) const;
  /*!
   * Build up a single OC representation of the geometry of the assembly.
   */
  void compound ( TopoDS_Shape& assembly );
  /*!
   * If this subassembly references an unserialized page, then ask
   * the model to report it later when it is loaded.
   * \param db_path original url.
   * \param item reference's memory pointer.
   */
  void resolved ( const QString& db_path, ModelItem* item );
  
  //! \return a unique name for the subassembly.
  static QString newName ( void );

signals:
  void locationChanged ( void );
  void solidChanged ( void );
  void materialChanged ( void );
  void constraintCreated ( const AssemblyConstraint* constraint );
  void constraintChanged ( const AssemblyConstraint* old_constraint,
			   const AssemblyConstraint* new_constraint );
  void constraintOffsetChanged ( const AssemblyConstraint* constraint );
  void constraintCanceled ( void );

private slots:
  void updateLocation ( void );
  void updateSolid ( void );
  void updateMaterial ( void );

private:
  // Usual common initialization actions.
  void init ( void );

  //! Counter for unique subassembly names.
  static uint unique_index_;

  //! Reference to the Part or Assembly.
  Space3D::Page* subassembly_;
  //! If the part or assembly doesn't exist yet, we have to delay computing
  //! the constraints, too.
  QDomElement delayed_constraints_;
  //! Transformation of this subassembly within the assembly.
  TopLoc_Location location_;
  //! The constraints which locate this subassembly.
  AssemblyConstraintManager constraints_;
  //! List of other subassemblies this subassembly depends on via constraints.
  QPtrList<const ModelItem> dependencies_;
};

#endif // SUBASSEMBLY_H
