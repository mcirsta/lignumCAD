/* -*- c++ -*-
 * sketch.h
 *
 * Header for the Sketch class
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
#ifndef SKETCH_H
#define SKETCH_H

#include "page.h"

class DBURL;

/*!
 * This is the sketch. This is a 2D (generally abstract) representation
 * of the model.
 */
class Sketch : public Space2D::Page {
  Q_OBJECT
public:
  //! This is the unique ID for a sketch. No other Page should have
  //! this value. Also, this value should be less than the IDs for pages which
  //! the user would logically create after this page.
  static const uint PAGE_ID = 10;
  /*!
   * Construct an empty sketch.
   * \param id id of new sketch.
   * \param name name of new sketch.
   * \param parent parent model of the new sketch.
   */
  Sketch ( uint id, const QString& name, Model* parent );
  /*!
   * Construct a sketch from its XML representation.
   * \param id id of new sketch.
   * \param xml_rep XML representation of sketch (and its child figures).
   * \param parent parent model of the new sketch.
   */
  Sketch ( uint id, const QDomElement& xml_rep, Model* parent );
  //! Destructor doesn't do anything. (The superclass, Page, frees
  //! any figures on this sketch.)
  ~Sketch ( void ) {}

  // Implementation of ModelItem interface

  /*!
   * Append the sketch's XML representation into the argument XML element.
   * \param xml_rep XML representation to append sketch's XML representation to.
   */
  void write ( QDomElement& xml_rep ) const;

  // Global class methods

  /*!
   * \return a globally unique name for the sketch.
   */
  static QString newName ( void );

private:
  //! Next index from which newName() forms a name.
  static uint unique_index_;
};

#endif // SKETCH_H
