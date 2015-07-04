/*
 * page.cpp
 *
 * Implementation of the Page classes
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
#include <TopoDS_Shape.hxx>

#include "constants.h"
#include "model.h"
#include "page.h"

PageBase::PageBase ( uint id, const QString& name, const QString& type,
		     Model* parent )
  : ModelItem( id, name, type ), parent_( parent )
{}

QVector<uint> PageBase::ID( void ) const
{
  QVector<uint> ids;

  if ( parent_ )
    ids = parent_->ID();

  ids.push_back( id() );

  return ids;
}

QString PageBase::path ( void ) const
{
  if ( parent_ )
    //    return
    //      lC::STR::DB_URL_PATTERN.arg( parent_->path() ).arg( name() ).arg( type() );
    return parent_->path() + '/' + name() + '.' + type();
  else
    //    return lC::STR::PATH_PATTERN.arg( lC::STR::DB_PREFIX ).arg( name() );
    return name();
}

namespace Space2D {

  Page::Page ( uint id, const QString& name, const QString& type, Model* parent )
    : PageBase( id, name, type, parent ), unique_figure_id_( 0 )
  {
    parent_->addPage( this );

    connect( parent_, SIGNAL( nameChanged( const QString& ) ),
	     SLOT( modelRenamed( const QString& ) ) );
  }

  Page::~Page ( void )
  {
    QMap<uint,Figure*>::iterator f = figures_.begin();
    for ( ; f != figures_.end(); ++f )
      delete f.data();
  }

  uint Page::newID ( void ) { return ++unique_figure_id_; }

  void Page::addFigure ( Figure* figure )
  {
    figures_[ figure->id() ] = figure;

    if ( figure->id() > unique_figure_id_ )
      unique_figure_id_ = figure->id();
  }

  void Page::removeFigure ( Figure* figure )
  {
    QMap<uint,Figure*>::iterator f = figures_.find( figure->id() );

    if ( f != figures_.end() ) {
      delete f.data();
      figures_.erase( f );
    }
  }

  ModelItem* Page::lookup ( QStringList& path_components ) const
  {
    // The front path component is the name of a figure with ".type" appended
    // to it.
    int dot_pos = path_components.front().findRev( '.' );
    QString name = path_components.front().left( dot_pos );
    QString type = path_components.front().right( path_components.front().length()
						  - dot_pos - 1 );

    QMap<uint,Figure*>::const_iterator f = figures_.begin();

    for ( ; f != figures_.end(); ++f ) {
      if ( f.data()->name() == name && f.data()->type() == type ) {
	path_components.erase( path_components.begin() );
	if ( path_components.empty() )
	  return f.data();
	else
	  return f.data()->lookup( path_components );
      }
    }

    return 0;			// Really an error...
  }
#if 0
  QPtrListIterator< Figure > Page::figures ( void ) const
  {
    return QPtrListIterator< Figure > ( figures_ );
  }
#endif
  void Page::modelRenamed ( const QString& /*name*/ )
  {
    emit nameChanged( name() );
  }

  TopoDS_Shape Page::lookupShape ( QStringList& /*path_components*/ ) const
  {
    return TopoDS_Shape(); // Not needed (currently) in 2D context
  }

  TopoDS_Shape Page::lookupShape ( QVector<uint>& /*id_path*/ ) const
  {
    return TopoDS_Shape(); // Not needed (currently) in 2D context
  }

  QString Page::idPath (QVector<uint> /*id_path*/ ) const
  {
    return name(); // Not needed (currently) in 2D context.
  }

  void Page::pathID ( QStringList& /*path_components*/,
		      QVector<uint>& /*id_path*/ ) const
  {
    return; // Not needed (currently) in 2D context.
  }

  void Page::dumpInfo ( void ) const
  {
    QMap<uint,Figure*>::const_iterator f = figures_.begin();
    for ( ; f != figures_.end(); ++f ) {
      cout << endl;
      f.data()->dumpObjectInfo();
    }
  }
} // End of Space2D namespace

namespace Space3D {

  Page::Page ( uint id, const QString& name, const QString& type, Model* parent )
    : PageBase( id, name, type, parent ), unique_figure_id_( 0 )
  {
    parent->addPage( this );
  }

  Page::~Page ( void )
  {
    QMap<uint,Figure*>::iterator f = figures_.begin();

    for ( ; f != figures_.end(); ++f )
      delete f.data();
  }

  uint Page::newID ( void ) { return ++unique_figure_id_; }

  void Page::addFigure ( Figure* figure )
  {
    figures_[ figure->id() ] = figure;

    if ( figure->id() > unique_figure_id_ )
      unique_figure_id_ = figure->id();
  }

  void Page::removeFigure ( Figure* figure )
  {
    QMapIterator<uint,Figure*> f = figures_.find( figure->id() );

    if ( f != figures_.end() ) {
      delete f.data();
      figures_.erase( f );
    }
  }

  ModelItem* Page::lookup ( QStringList& path_components ) const
  {
    // The front path component is the name of a figure with ".type" appended
    // to it.
    int dot_pos = path_components.front().findRev( '.' );
    QString name = path_components.front().left( dot_pos );
    QString type = path_components.front().right( path_components.front().length()
						  - dot_pos - 1 );

    QMap<uint,Figure*>::const_iterator f = figures_.begin();

    for ( ; f != figures_.end(); ++f ) {
      if ( f.data()->name() == name && f.data()->type() == type ) {
	path_components.erase( path_components.begin() );
	if ( path_components.empty() )
	  return f.data();
	else
	  return f.data()->lookup( path_components );
      }
    }

    return 0;			// Really an error...
  }

  ModelItem* Page::lookup (QVector<uint> &id_path ) const
  {
    QMapConstIterator<uint,Figure*> figure = figures_.find( id_path[0] );

    if ( figure != figures_.end() ) {
      id_path.erase( id_path.begin() );
      if ( id_path.empty() )
	return figure.data();
      else
	return figure.data()->lookup( id_path );
    }

    return 0;			// Really an error...
  }
#if 0
  QPtrListIterator< Figure > Page::figures ( void ) const
  {
    return QPtrListIterator< Figure > ( figures_ );
  }
#endif

  void Page::dumpInfo ( void ) const
  {
    QMap<uint,Figure*>::const_iterator f = figures_.begin();
    for ( ; f != figures_.end(); ++f )
      f.data()->dumpObjectInfo();
  }
} // End of Space3D namespace
