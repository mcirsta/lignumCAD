/*
 * modelitem.cpp
 *
 * ModelItem class: Abstract base class of the elements of the design.
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
#include <qapplication.h>

#include "constants.h"
#include "dburl.h"
#include "modelitem.h"

// Change the name of the item.

void ModelItem::setName ( const QString& name )
{
  // It was pointed out that "/" can't really be in a name, so encode it.
  // Evidently, neither can "\".
  name_ = QString::null;

  for ( uint i = 0; i < name.length(); i++ )
    if ( name[i] == '/' || name[i] == '\\' )
      name_ += QString( "%%1" ).arg( name[i].unicode(), 0, 16 );
    else
      name_ += name[i];

  //  name_ = name;

  QObject::setName( name_.latin1() );

  emit nameChanged( name_ );
}

// Return the DB URL identifier of the object.

DBURL ModelItem::dbURL ( void ) const
{
#if 1
  QString _path = path();
  DBURL::encode( _path );
  return DBURL( lC::STR::DB_PREFIX, _path );
#else
  return DBURL( lC::STR::DB_PREFIX, path() );
#endif
}

// Translate a string in the Constant context.

const QString ModelItem::trC ( const QString& string ) const
{
  return qApp->translate( "Constants", string );
}
