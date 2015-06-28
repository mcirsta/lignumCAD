/* -*- c++ -*-
 * assembly.h
 *
 * Header for the Assembly class
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
#ifndef ASSEMBLY_H
#define ASSEMBLY_H

#include "page.h"

class Subassembly;
class TopoDS_Compound;

/*!
 * This is a assembly: A 3D dimensional representation of a piece of
 * furniture. It is collection of parts and (sub)assemblies.
 */
class Assembly : public Space3D::Page {
  Q_OBJECT

public:
  //! This is the unique ID for a assembly. No other Page should have
  //! this value. Also, this value should be less than the IDs for pages which
  //! the user would logically create after this page.
  static const uint PAGE_ID = 30;
  /*!
   * Construct an empty assembly.
   * \param id id of new assembly.
   * \param name name of new assembly.
   * \param parent parent model of the new assembly.
   */
  Assembly ( uint id, const QString& name, Model* parent );
  /*!
   * Construct an assembly from its XML representation.
   * \param id id of new assembly.
   * \param xml_rep XML representation of assembly.
   * \param parent parent model of the new assembly.
   */
  Assembly ( uint id, const QDomElement& xml_rep, Model* parent );
  //! Destructor doesn't do anything.
  ~Assembly ( void ) {}

  /*!
   * Add a model model to the assembly.
   * \param model model to add.
   * \return the pointer to the new subassembly.
   */
  Subassembly* addModel( Space3D::Page* model );

  /*!
   * Look up the geometry type of an OpenCASCADE shape.
   * \param path_components string list of path components.
   */
  Handle(Standard_Type) lookupType ( QStringList& path_components ) const;

  /*!
   * Look up the geometry type of an OpenCASCADE shape.
   * \param id_path id list of path components.
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
   * \param id_path id path of components.
   */
  TopoDS_Shape lookupShape ( QValueVector<uint>& id_path ) const;
  /*!
   * Is the given page referenced by this assembly. Have to traverse
   * the subassembly tree to find out.
   * \param page Page to check for usage.
   * \return true if the page is immediately referenced by a subassembly of this
   * assembly.
   */
  bool used ( const PageBase* page ) const;
  /*!
   * Even more difficult than used() is referenced(). Determine if any
   * other subassemblies reference a surface (or other geometry) of
   * the given subassembly.
   * \param target subassembly to check.
   * \return true if another subassembly in this assembly references
   * a surface of the given subassembly.
   */
  bool referenced ( const Subassembly* target ) const;
  /*!
   * Construct a string path to the specified item. May be either the
   * assembly itself or one of its figures.
   * \param id_path id path to item.
   * \return string encoded path to item.
   */
  QString idPath ( QValueVector<uint> id_path ) const;
  /*!
   * Construct the id path to the specified item. May be either the assembly
   * itself or one of its figures.
   * \param path_components broken down components of the orignal DBURL with
   * paths to current item removed.
   * \param id_path id path to update with id's.
   */
  void pathID ( QStringList& path_components, QValueVector<uint>& id_path ) const;

  // Implementation of ModelItem interface

  /*!
   * Append the assembly's XML representation into the argument XML element.
   * \param xml_rep XML representation to append assembly's XML representation to.
   */
  void write ( QDomElement& xml_rep ) const;

  /*!
   * On demand (i.e., we don't keep a copy around), create a TopoDS_Compound
   * shape which collects all the subassemblies into a representation of
   * the assembly. Currently, this is needed for the correct computation
   * of hidden lines.
   */
  TopoDS_Compound compound ( void );

  // Global class methods

  /*!
   * \return a globally unique name for the assembly.
   */
  static QString newName ( void );

signals:
  //! Emitted when a model is added (or removed?) from the assembly
  void assemblyChanged ( void );

private:
  //! Next index from which newName() forms a name.
  static uint unique_index_;
private slots:
  void updateAssemblySolid ( void );
  void updateAssemblyLocation ( void );
  void subassemblyDestroyed ( void );
};

#endif // ASSEMBLY_H
