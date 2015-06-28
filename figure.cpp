/*
 * figure.cpp
 *
 * Implementation of Figure classes
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
#include "page.h"
#include "figure.h"

FigureBase::FigureBase ( uint id, const QString& name, const QString& type )
  : ModelItem( id, name, type )
{}

namespace Space2D {

  Figure::Figure ( uint id, const QString& name, const QString& type, Page* parent )
    : FigureBase( id, name, type ),
      parent_( parent ), valid_( false ), complete_( false )
  {
    if ( parent_ ) {
      parent_->addFigure( this );
      connect( parent_, SIGNAL( nameChanged( const QString& ) ),
	       SLOT( pageRenamed( const QString& ) ) );
    }
  }

  Model* Figure::model ( void ) const
  {
    return parent_->model();
  }

  QValueVector<uint> Figure::ID ( void ) const
  {
    QValueVector<uint> ids;

    if ( parent_ )
      ids = parent_->ID();

    ids.push_back( id() );

    return ids;
  }

  QString Figure::path ( void ) const
  {
#if 0
    if ( parent_ )
      return
	lC::STR::DB_URL_PATTERN.arg( parent_->path() ).arg( name() ).arg( type() );
    else
      return lC::STR::PATH_PATTERN.arg( lC::STR::DB_PREFIX ).arg( name() );
#else
  if ( parent_ )
    return parent_->path() + '/' + name() + '.' + type();
  else
    return name();
#endif
  }

  void Figure::setValid ( bool valid )
  {
    valid_ = valid;
  }

  void Figure::setComplete ( bool complete )
  {
    complete_ = complete;
  }

  void Figure::pageRenamed ( const QString& /*name*/ )
  {
    emit nameChanged( name() );
  }
} // End of Space2D namespace

namespace Space3D {

  Figure::Figure ( uint id, const QString& name, const QString& type, Page* parent )
    : FigureBase( id, name, type ),
      parent_( parent ), valid_( false ), complete_( false )
  {
    if ( parent_ ) {
      parent_->addFigure( this );
      connect( parent_, SIGNAL( nameChanged( const QString& ) ),
	       SLOT( pageRenamed( const QString& ) ) );
    }
  }

  QValueVector<uint> Figure::ID ( void ) const
  {
    QValueVector<uint> ids;

    if ( parent_ )
      ids = parent_->ID();

    ids.push_back( id() );

    return ids;
  }

  Model* Figure::model ( void ) const
  {
    return parent_->model();
  }

  QString Figure::path ( void ) const
  {
#if 0
    if ( parent_ )
      return
	lC::STR::DB_URL_PATTERN.arg( parent_->path() ).arg( name() ).arg( type() );
    else
      return lC::STR::PATH_PATTERN.arg( lC::STR::DB_PREFIX ).arg( name() );
#else
  if ( parent_ )
    return parent_->path() + '/' + name() + '.' + type();
  else
    return name();
#endif
  }

  void Figure::setValid ( bool valid )
  {
    valid_ = valid;
  }

  void Figure::setComplete ( bool complete )
  {
    complete_ = complete;
  }

  void Figure::pageRenamed ( const QString& /*name*/ )
  {
    emit nameChanged( name() );
  }
} // End of Space3D namespace
