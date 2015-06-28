/*
 * geometry.cpp
 *
 * Basic Geometry base classes
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

#include "constants.h"
#include "geometry.h"
#include "figure.h"

namespace Space2D {
  Geometry::Geometry ( uint id, const QString& name, const QString& type,
		       Figure* parent )
    : ModelItem( id, name, type ), parent_( parent )
  {
    // Guess I'm not sure under what circumstances this is necessary.
    // Seems to just create an infinite loop
#if 1
    if ( parent_ )
      connect( parent_, SIGNAL( nameChanged( const QString& ) ),
	       SLOT( figureRenamed( const QString& ) ) );
#endif
  }

  Model* Geometry::model ( void ) const
  {
    return parent_->model();
  }

  QValueVector<uint> Geometry::ID ( void ) const
  {
    QValueVector<uint> ids;

    if ( parent_ )
      ids = parent_->ID();

    ids.push_back( id() );

    return ids;
  }

  QString Geometry::path ( void ) const
  {
#if 0
    if ( parent_ )
      return
	lC::STR::DB_URL_PATTERN.arg( figure()->path() ).arg( name() ).arg( type() );
    else
      return lC::STR::PATH_PATTERN.arg( lC::STR::DB_PREFIX ).arg( name() );
#else
  if ( parent_ )
    return parent_->path() + '/' + name() + '.' + type();
  else
    return name();
#endif
  }

  void Geometry::figureRenamed ( const QString& /*name*/ )
  {
    emit nameChanged( name() );
  }
} // End of Space2D namespace

namespace Space3D {
  Geometry::Geometry ( uint id, const QString& name, const QString& type,
		       Figure* parent )
    : ModelItem( id, name, type ), parent_( parent )
  {
    // Guess I'm not sure under what circumstances this is necessary.
    // Seems to just create an infinite loop
#if 1
    if ( parent_ )
      connect( parent_, SIGNAL( nameChanged( const QString& ) ),
	       SLOT( figureRenamed( const QString& ) ) );
#endif
  }

  Model* Geometry::model ( void ) const
  {
    return parent_->model();
  }

  QValueVector<uint> Geometry::ID ( void ) const
  {
    QValueVector<uint> ids;

    if ( parent_ )
      ids = parent_->ID();

    ids.push_back( id() );

    return ids;
  }

  QString Geometry::path ( void ) const
  {
#if 0
    if ( parent_ )
      return
	lC::STR::DB_URL_PATTERN.arg( figure()->path() ).arg( name() ).arg( type() );
    else
      return lC::STR::PATH_PATTERN.arg( lC::STR::DB_PREFIX ).arg( name() );
#else
  if ( parent_ )
    return parent_->path() + '/' + name() + '.' + type();
  else
    return name();
#endif
  }

  void Geometry::figureRenamed ( const QString& /*name*/ )
  {
    emit nameChanged( name() );
  }
} // End of Space3D namespace
