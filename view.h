/* -*- c++ -*-
 * view.h
 *
 * Header for the View class
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
#ifndef VIEW_H
#define VIEW_H

// I guess it turns out that you have to be able to look up *views* of
// modelitems by name (mostly so that undo/redo can operate after an
// object has been deleted and recreated, that is, after the original
// pointer to the view is no longer valid). Therefore, all the views
// have to belong to a common base class. QObject might do, but
// something like DimensionView really doesn't have to be a QObject
// (why?)

#include "dburl.h"
/*!
 * A view is a graphical representation of a ModelItem.
 */
class View {
public:
  // \return the name of the item being viewed.
  virtual QString name ( void ) const = 0;
  // \return the URL of the item being viewed.
  virtual DBURL dbURL ( void ) const = 0;
  /*!
   * Translate a string from the constant context. This is pretty common
   * functionality for views.
   * \param string constant string to translate.
   */
  const QString trC ( const QString& string ) const;
};
#endif /* VIEW_H */
