/* -*- c++ -*-
 * part.h
 *
 * Header for the Part class
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
#ifndef PART_H
#define PART_H

#include <map>

#include <qdict.h>

#include "page.h"

class DBURL;
class Material;
class lCDefaultLengthConstraint;
namespace Space3D {
  class OCSolid;
}
class PartMetadata;

/*!
 * This is a part: A 3D dimensional representation of a piece of
 * wood (generally) which has been milled to the desired shape.
 */

class Part : public Space3D::Page {
  Q_OBJECT
public:
  //! This is the unique ID for a part. No other Page should have
  //! this value. Also, this value should be less than the IDs for pages which
  //! the user would logically create after this page.
  static const uint PAGE_ID = 20;
  /*!
   * Construct an empty part.
   * \param id id of new part.
   * \param name name of new part.
   * \param parent parent model of the new part.
   */
  Part ( uint id, const QString& name, Model* parent );
  /*!
   * Construct a part from its XML representation.
   * \param id id of new part.
   * \param xml_rep XML representation of part.
   * \param parent parent model of the new part.
   */
  Part ( uint id, const QDomElement& xml_rep, Model* parent );
  //! Destructor doesn't do anything.
  ~Part ( void ) {}
  //! \return the solid.
  Space3D::OCSolid* solid ( void ) const { return solid_; }
  //! \return the representation of this solid as an OpenCASCADE Compound
  // object.
  const TopoDS_Shape& shape ( void ) const;

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
   * \param id_path id path.
   */
  TopoDS_Shape lookupShape ( QValueVector<uint>& id_path ) const;
  /*!
   * Is the given page referenced by this part. Always false so far.
   * (When the argument is more general, then we'll have to do something.)
   * \param page Page to check for usage.
   * \return false. A page can't reference any page (except itself, of course).
   */
  bool used ( const PageBase* /*page*/ ) const { return false; }

  /*!
   * Construct a string path to the specified item. May be either the
   * part itself or one of its figures.
   * \param id_path id path to item.
   * \return string encoded path to item.
   */
  QString idPath ( QValueVector<uint> id_path ) const;
  /*!
   * Construct the id path to the specified item. May be either the part
   * itself or one of its figures.
   * \param path_components broken down components of the orignal DBURL with
   * paths to current item removed.
   * \param id_path id path to update with id's.
   */
  void pathID ( QStringList& path_components, QValueVector<uint>& id_path ) const;

  void init ( void );

  // Implementation of ModelItem interface

  /*!
   * Append the part's XML representation into the argument XML element.
   * \param xml_rep XML representation to append part's XML representation to.
   */
  void write ( QDomElement& xml_rep ) const;

  // General functions
  /*!
   * Pass the parameters collect from the New Part Wizard to the part
   * template library constructor to create the initial solid geometry
   * of the part.
   * \param part reference to part template's metadata.
   * \param parameters a QDict containing the lCDefaultLengthConstraint
   * widgets which collected the initial parameter data from the user.
   */
  void makeSolidParameters ( const PartMetadata* part,
			     const QDict<lCDefaultLengthConstraint>& parameters );

  /*!
   * Set the (optional) material out of which this part is made.
   * \param material (new) material of part.
   */
  void setSolidMaterial ( const Material* material );

  // Global class methods
  /*!
   * \return a globally unique name for the part.
   */
  static QString newName ( void );

private:
  //! Next index from which newName() forms a name.
  static uint unique_index_;
  //! The solid.
  Space3D::OCSolid* solid_;
};

/*!
 * This structure defines the information presented to the
 * user when a template part is available. Like PageMetadata,
 * this is something like a QAction.
 */
class PartMetadata {
public:
  /*!
   * Construct part metadata.
   * \param group Parts are grouped into (totally arbitrary) sets.
   * \param name name of template part.
   * \param icon name of icon file.
   * \param solid name of the base solid.
   */
  PartMetadata ( const QString& group, const QString& name, const QString& icon,
		 const QString& solid );
  //! Simple virtual destructor.
  virtual ~PartMetadata ( void ) {}
  //! Add a parameter (name).
  void addParameter ( const QString& parameter_name );
  //! \return the part's group.
  QString group ( void ) const { return group_; }
  //! \return the part's name.
  QString name ( void ) const { return name_; }
  //! \return the part's icon file name.
  QString icon ( void ) const { return icon_; }
  //! \return the part's base solid.
  QString solid ( void ) const { return solid_; }
  //! \return the number of parameters.
  int parameterCount ( void ) const { return parameter_names_.size(); }
  //! \return an iterator over the parameter names.
  QStringList::const_iterator parameters ( void ) const { return parameter_names_.begin(); }
  /*!
   * Validate that the values given for this part (so far) are sensical.
   * \param initial_values array of the length constraint widgets which
   * the user is filling out.
   * \return true if everything is OK with the input.
   */
  virtual bool valid ( const QDict<lCDefaultLengthConstraint>& initial_values ) const = 0;
  /*!
   * Create the starting solid part requested by the user.
   * \param name new solid name.
   * \param initial_values an array of the length constraint widgets which
   * the user filled out.
   * \param parent parent page of solid.
   * \return the solid.
   */
  virtual Space3D::OCSolid* create ( const QString& name,
				     const QDict<lCDefaultLengthConstraint>& initial_values,
				     Part* parent ) const = 0;
  /*!
   * Create the solid from its XML representation.
   * \param name solid name.
   * \param xml_rep XML representation of the solid.
   * \param parent parent page of solid.
   * \return the solid.
   */
  virtual Space3D::OCSolid* create ( const QString& name,
				     const QDomElement& xml_rep, Part* parent )
    const = 0;

private:
  //! Group of template part.
  QString group_;
  //! Name of template part.
  QString name_;
  //! Icon file name of template part.
  QString icon_;
  //! Name of base solid.
  QString solid_;
  //! List of template parameter names.
  QStringList parameter_names_;
};

/*!
 * This class implements a Part library: predefined parts which can be
 * customized with a few parameters. Eventually, I hope we're able
 * to load parts dynamically...
 *
 * For now, we'll also use it to dispense <Name,Id> pairs for geometry
 * naming. For example, every part with a face named Front will also
 * use the same integer Id for that face. If you need a non-standard
 * name, we'll dispense that too.
 */
class PartFactory {
public:
  //! \return the singleton instance of the part factory.
  static PartFactory* instance ( void );

  /*!
   * On construction of a Part template, it should add itself to
   * the library.
   */
  void addPartMetadata ( PartMetadata* part_data );

  //! \return an iterator to the parts in the library.
  QPtrListIterator< PartMetadata > parts ( void ) const
  { return QPtrListIterator<PartMetadata>( part_data_ ); }

  /*!
   * Return a specific part's metadata
   * \param group part's group.
   * \param name part's name.
   * \return reference to part template's metadata.
   */
  PartMetadata* part ( const QString& group, const QString& name );

  /*!
   * Create a solid based on it's XML representation.
   * \param parent parent page of the solid.
   * \param xml_rep XML representation to the solid.
   * \return the constructed solid.
   */
  Space3D::OCSolid* create ( Part* parent, const QDomElement& xml_rep ) const;

  /*!
   * Generate a name and id for this geometry. If a suggestion is passed in,
   * we'll use that (if we have one already), or create it if not. If
   * no name is passed in, construct a unique name.
   * \param suggestion requested name (or suggestion). If equal to QString::null,
   * invent a unique name.
   * \return the matched set of name and integer id.
   */
  std::pair< QString, uint > name ( const QString& suggestion );

  /*!
   * Look up the name corresponding to this id.
   * \param id id of name.
   * \return corresponding name.
   */
  QString name ( uint id );

protected:
  //! Per "Design Patterns": It can only construct itself.
  PartFactory ( void );

private:
  //! Singleton instance of the part factory. Constructed lazily.
  static PartFactory* instance_;
  //! List of parts in the library.
  QPtrList< PartMetadata > part_data_;
  //! Global list of geometry names and ids.
  std::map<QString, uint> names_;
  //! Last unique name index used.
  uint unique_index_;
  //! Last unique id used.
  uint unique_id_;
};
#endif // PART_H
