/*
 * dburl.cpp
 *
 * DBURL class: A URL-like representation object names.
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
#include "dburl.h"

#include <QFileInfo>
#include <QDir>

QString DBURL::name ( void ) const
{
  QString name = fileName();
  return name.left( name.lastIndexOf( '.' ) );
}

QString DBURL::type ( void ) const
{
  QString name = fileName();
  return name.right( name.length() - name.lastIndexOf( '.' ) - 1 );
}

DBURL DBURL::parent ( void ) const
{
  DBURL parent( *this );
  parent.setPath( QFileInfo(this->toString()).dir().path() );
  return parent;
}
