/* -*- c++ -*-
 * drawing.h
 *
 * Header for the Drawing class
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
#ifndef DRAWING_H
#define DRAWING_H

#include "page.h"

/*!
 * This is the drawing. This is a 2D detailed representation of the
 * parts of the model.
 */
class Drawing : public Space2D::Page {
  Q_OBJECT
public:
  //! This is the unique ID for a drawing. No other Page should have
  //! this value. Also, this value should be less than the IDs for pages which
  //! the user would logically create after this page.
  static const uint PAGE_ID = 40;
  /*!
   * Construct an empty drawing.
   * \param id id of new drawing.
   * \param name name of new drawing.
   * \param parent parent model of the new drawing.
   */
  Drawing ( uint id, const QString name, Model* parent );
  /*!
   * Construct an drawing from its XML representation.
   * \param id id of new drawing.
   * \param xml_rep XML representation of drawing.
   * \param parent parent model of the new drawing.
   */
  Drawing ( uint id, const QDomElement& xml_rep, Model* parent );
  //! Destructor doesn't do anything.
  ~Drawing ( void ) {}

  // Implementation of ModelItem interface

  /*!
   * Append the drawing's XML representation into the argument XML element.
   * \param xml_rep XML representation to append drawing's XML representation to.
   */
  void write ( QDomElement& xml_rep ) const;

  // Global class methods

  /*!
   * \return a globally unique name for the drawing.
   */
  static QString newName ( void );

private:
  //! Next index from which newName() forms a name.
  static uint unique_index_;
};

#endif // DRAWING_H
