/*
 * model.cpp
 *
 * Model class: the primary representation of the design.
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

#include <qdom.h>
#include <qstringlist.h>
#include <qregexp.h>
#include <qlistview.h>
#include <qobjectlist.h>

#include "constants.h"
#include "globaldata.h"
#include "dburl.h"
#include "pagefactory.h"
#include "units.h"
#include "model.h"

uint Model::unique_id_ = 0;

uint Model::uniqueID ( void ) { return ++unique_id_; }

Model::Model ( const QString name, const QString description )
  : ModelItem( uniqueID(), name, lC::STR::MODEL ), unique_page_id_( 0 ),
    read_file_name_( 0 ),
    write_file_name_( name + lC::STR::LCAD_FILE_EXT ),
    description_( description ),
    created_( QDateTime::currentDateTime() ), modified_( created_ ),
    version_( INITIAL_VERSION ), revision_( INITIAL_REVISION ),
    changed_( false )
{
  delay_resolutions_.setAutoDelete( true );
}

Model::Model ( const QString file_name, const QDomElement& xml_rep )
  : ModelItem( uniqueID(), QString::null, lC::STR::MODEL ), unique_page_id_( 0 ),
    read_file_name_( file_name ), write_file_name_( file_name ), changed_( false )
{
  QDomNode n = xml_rep.firstChild();

  while ( !n.isNull() ) {
    QDomElement e = n.toElement();
    if ( !e.isNull() ) {
      if ( e.tagName() == "metadata" ) {
	restoreMetadata( e );
      }
      else if ( e.tagName() == lC::STR::PAGES ) {
	restorePages( e );
      }
    }
    n = n.nextSibling();
  }
}

Model::~Model ( void )
{
  // An inconvenience of the QMap is that we have to delete the pages ourselves.
  QMap<uint,PageBase*>::iterator p = pages_.begin();
  for ( ; p != pages_.end(); ++p )
    delete p.data();
}

void Model::restoreMetadata ( const QDomElement& xml_rep )
{
  QDomNode n = xml_rep.firstChild();

  while ( !n.isNull() ) {
    QDomElement e = n.toElement();
    if ( !e.isNull() ) {
      if ( e.tagName() == lC::STR::NAME ) {
	setName( e.text() );
      }
      else if ( e.tagName() == lC::STR::DESCRIPTION ) {
	description_ = e.text();
      }
      else if ( e.tagName() == lC::STR::CREATED ) {
	created_ = QDateTime::fromString( e.attribute( lC::STR::VALUE ), Qt::ISODate );
      }
      else if ( e.tagName() == lC::STR::MODIFIED ) {
	modified_ = QDateTime::fromString( e.attribute( lC::STR::VALUE ), Qt::ISODate );
      }
      else if ( e.tagName() == lC::STR::LVERSION ) {
	version_ = e.attribute( lC::STR::VALUE ).toUInt();
      }
      else if ( e.tagName() == lC::STR::REVISION ) {
	revision_ = e.attribute( lC::STR::VALUE ).toUInt();
      }
    }
    n = n.nextSibling();
  }
}

void Model::restorePages ( const QDomElement& xml_rep )
{
  QDomNode n = xml_rep.firstChild();

  while ( !n.isNull() ) {
    QDomElement e = n.toElement();
    if ( !e.isNull() ) {
      PageBase* page = PageFactory::instance()->create( this, e );
      if ( page->id() > unique_page_id_ )
	unique_page_id_ = page->id();
    }
    n = n.nextSibling();
  }

  // Need to consider whether one pass through is enough...
  delay_resolutions_.first();

  while ( delay_resolutions_.current() != 0 ) {
    ModelItem* resolvee = lookup( delay_resolutions_.current()->dbPath() );
    if ( resolvee != 0 ) {
      delay_resolutions_.current()->item()->
	resolved( delay_resolutions_.current()->dbPath(), resolvee );
      delay_resolutions_.remove();
    }
    else
      delay_resolutions_.next();
  }

  if ( delay_resolutions_.count() != 0 )
    cerr << "Did not resolve all delayed lookups. Model inconsistent." << endl;
}

/*!
 * Take a pass through the delayed resolution table right now.
 */
void Model::resolveNow ( void )
{
  delay_resolutions_.first();

  while ( delay_resolutions_.current() != 0 ) {
    ModelItem* resolvee = lookup( delay_resolutions_.current()->dbPath() );
    if ( resolvee != 0 ) {
      delay_resolutions_.current()->item()->
	resolved( delay_resolutions_.current()->dbPath(), resolvee);
      delay_resolutions_.remove();
    }
    else
      delay_resolutions_.next();
  }
}


uint Model::newID ( void ) { return ++unique_page_id_; }

QVector<uint> Model::ID( void ) const
{
  QValueVector<uint> ids( 1 );
  ids[0] = id();
  return ids;
}

QString Model::path ( void ) const
{
#if 0
  return lC::STR::PATH_PATTERN.arg( lC::STR::DB_PREFIX ).arg( name() );
#else
#if 0
  return lC::STR::DB_PREFIX + '/' + name();
#else
  return name();
#endif
#endif
}

QString Model::idPath (const QVector<uint> &id_path ) const
{
  if ( id_path.empty() )
    return QString::null;	// Really an error...

  if ( id_path[0] != id() )
    return QString::null;	// Really an error...

  QValueVector<uint> my_path = id_path;

  my_path.erase( my_path.begin() );

#if 0
  if ( my_path.empty() )
    return lC::STR::PATH_PATTERN.arg( lC::STR::DB_PREFIX ).arg( name() );
#else
  if ( my_path.empty() )
    return name();
#endif

  QMap<uint,PageBase*>::const_iterator p = pages_.find( my_path[0] );
  if ( p == pages_.end() )
    return QString::null;	// Really an error...

  my_path.erase( my_path.begin() );

#if 0
  if ( my_path.empty() )
    return QString( "%1/%2/%3.%4" ).arg( lC::STR::DB_PREFIX ).arg( name() ).
      arg( p.data()->name() ).arg( p.data()->type() );

  return QString( "%1/%2/%3.%4/%5" ).arg( lC::STR::DB_PREFIX ).arg( name() ).
    arg( p.data()->name() ).arg( p.data()->type() ).
    arg( p.data()->idPath( my_path ) );
#else
  if ( my_path.empty() )
    return name() + '/' + p.data()->name() + '.' +  p.data()->type();

  return name() + '/' +  p.data()->name() + '.' + p.data()->type() +
    '/' + p.data()->idPath( my_path );
#endif
}

QVector<uint> Model::pathID( const DBURL& db_url ) const
{
  QStringList path_components = QStringList::split( "/", db_url.path() );

  // The first path component is either the name of the model or a global
  // item (like the X axis in 2D space). Check for global items first.

  QValueVector<uint> id_path;

  ModelItem* item = GlobalData::lookup( path_components );

  if ( item != 0 ) {
    id_path.push_back( item->id() );
    return id_path;
  }

  if ( path_components.front() != name() )
    return id_path;			// Not this model! Really an error...

  id_path.push_back( id() );

  // Pop the model name off the list
  path_components.erase( path_components.begin() );

  // If that's all, then this was a reference to the model itself.
  if ( path_components.empty() ) {
    return id_path;
  }

  // The front path component is the name of a page with ".type" appended
  // to it.
  int dot_pos = path_components.front().findRev( '.' );
  QString name = path_components.front().left( dot_pos );
  QString type = path_components.front().right( path_components.front().length()
						- dot_pos - 1 );

  QMap<uint,PageBase*>::const_iterator p = pages_.begin();
  for ( ; p != pages_.end(); ++p ) {
    if ( p.data()->name() == name && p.data()->type() == type ) {
      id_path.push_back( p.data()->id() );

      path_components.erase( path_components.begin() );

      if ( !path_components.empty() ) {
	p.data()->pathID( path_components, id_path );
      }
    }
  }

  return id_path;		// Possibly an error, though...
}

void Model::addPage ( PageBase* page )
{
  pages_[ page->id() ] = page;
}

void Model::removePage ( PageBase* page )
{
  QMap<uint,PageBase*>::iterator p = pages_.find( page->id() );
  if ( p != pages_.end() ) {
    delete p.data();
    pages_.erase( p );
  }
}

/*!
 * Find the ModelItem which corresponds to this path. A path looks
 * like a URL:
 * protocol://name/item/...
 * where [protocol] is 'db' for the internal database, [name] is
 * the name of a page with the form name.type (where [type] is
 * sketch, part, assembly or drawing) and [item/...] is a hierarchical path
 * to any of the Figures on that Page.
 * \param db_url URL-style path to component.
 */
ModelItem* Model::lookup ( const DBURL& db_url ) const
{
  QStringList path_components = QStringList::split( "/", db_url.path() );

  // The first path component is either the name of the model or a global
  // item (like the X axis in 2D space). Check for global items first.

  ModelItem* item = GlobalData::lookup( path_components );

  if ( item != 0 )
    return item;

  if ( path_components.front() != name() )
    return 0;			// Not this model!

  // Pop the model name off the list
  path_components.erase( path_components.begin() );

  // If that's all, then this was a reference to the model itself.
  if ( path_components.empty() )
    return const_cast<Model*>( this );

  // The front path component is the name of a page with ".type" appended
  // to it.
  int dot_pos = path_components.front().findRev( '.' );
  QString name = path_components.front().left( dot_pos );
  QString type = path_components.front().right( path_components.front().length()
						- dot_pos - 1 );

  QMap<uint,PageBase*>::const_iterator p = pages_.begin();
  for ( ; p != pages_.end(); ++p ) {
    if ( p.data()->name() == name && p.data()->type() == type ) {
      path_components.erase( path_components.begin() );
      if ( path_components.empty() )
	return p.data();
      else
	return p.data()->lookup( path_components );
    }
  }

  return 0;
}

ModelItem* Model::lookup (const QVector<uint> &id_path ) const
{
  QValueVector<uint> my_path = id_path;

  if ( my_path.empty() || my_path[0] != id() )
    return 0;			// Not this model! Really an error...

  // Pop the model id off the list
  my_path.erase( my_path.begin() );

  if ( my_path.empty() )
    return const_cast<Model*>( this );

  QMap<uint,PageBase*>::const_iterator p = pages_.find( my_path[0] );

  if ( p != pages_.end() ) {
    my_path.erase( my_path.begin() );
    
    if ( my_path.empty() )
      return p.data();
    else
      return p.data()->lookup( my_path );
  }

  return 0;			// Really an error...
}

Handle(Standard_Type) Model::lookupType ( const DBURL& db_url ) const
{
  QStringList path_components = QStringList::split( "/", db_url.path() );

  if ( path_components.front() != name() )
    return Handle(Standard_Type)(); // Not this model!

  // Pop the model name off the list
  path_components.erase( path_components.begin() );

  // The front path component is the name of a page with ".type" appended
  // to it.
  int dot_pos = path_components.front().findRev( '.' );
  QString name = path_components.front().left( dot_pos );
  QString type = path_components.front().right( path_components.front().length()
						- dot_pos - 1 );

  path_components.erase( path_components.begin() );

  QMap<uint,PageBase*>::const_iterator p = pages_.begin();
  for ( ; p != pages_.end(); ++p ) {
    if ( p.data()->name() == name && p.data()->type() == type ) {
      if ( path_components.empty() )
	return Handle(Standard_Type)();
      else
	return p.data()->lookupType( path_components );
    }
  }

  return Handle(Standard_Type)();
}

Handle(Standard_Type) Model::lookupType ( const QValueVector<uint>& id_path ) const
{
  QValueVector<uint> my_path = id_path;

  if ( my_path.empty() || my_path[0] != id() )
    return Handle(Standard_Type)(); // Not this model!

  // Pop the model id off the list
  my_path.erase( my_path.begin() );

  if ( my_path.empty() )
    return Handle(Standard_Type());

  QMap<uint,PageBase*>::const_iterator p = pages_.find( my_path[0] );

  if ( p != pages_.end() ) {
    my_path.erase( my_path.begin() );
    
    if ( !my_path.empty() )
      return p.data()->lookupType( my_path );
  }

  return Handle(Standard_Type)();
}

TopoDS_Shape Model::lookupShape ( const DBURL& db_url ) const
{
  QStringList path_components = QStringList::split( "/", db_url.path() );

  if ( path_components.front() != name() )
    return TopoDS_Shape(); // Not this model!

  // Pop the model name off the list
  path_components.erase( path_components.begin() );

  // The front path component is the name of a page with ".type" appended
  // to it.
  int dot_pos = path_components.front().findRev( '.' );
  QString name = path_components.front().left( dot_pos );
  QString type = path_components.front().right( path_components.front().length()
						- dot_pos - 1 );

  path_components.erase( path_components.begin() );

  QMap<uint,PageBase*>::const_iterator p = pages_.begin();
  for ( ; p != pages_.end(); ++p ) {
    if ( p.data()->name() == name && p.data()->type() == type ) {
      if ( path_components.empty() )
	return TopoDS_Shape();
      else
	return p.data()->lookupShape( path_components );
    }
  }

  return TopoDS_Shape();
}

TopoDS_Shape Model::lookupShape (const QVector<uint> &id_path ) const
{
  QValueVector<uint> my_path = id_path;

  if ( my_path.empty() || my_path[0] != id() )
    return TopoDS_Shape(); // Not this model!

  // Pop the model id off the list
  my_path.erase( my_path.begin() );

  if ( my_path.empty() )
    return TopoDS_Shape();

  QMap<uint,PageBase*>::const_iterator p = pages_.find( my_path[0] );

  if ( p != pages_.end() ) {
    my_path.erase( my_path.begin() );
    
    if ( my_path.empty() )
      return TopoDS_Shape();
    else
      return p.data()->lookupShape( my_path );
  }

  return TopoDS_Shape();
}

QPtrList<PageBase> Model::whereUsed( const DBURL& db_url ) const
{
  QPtrList<PageBase> usages;

  PageBase* page = dynamic_cast<PageBase*>( lookup( db_url ) );

  if ( page != 0 ) {

    QMap<uint,PageBase*>::const_iterator p = pages_.begin();
    for ( ; p != pages_.end(); ++p ) {
      if ( p.data() != page && p.data()->used( page ) ) {
	usages.append( p.data() );
      }
    }
  }

  return usages;
}

// Depending on the order in which things are defined by the user,
// some references may be defined after they occur in the model file.
// Model will resolve these dependencies after the first pass through
// the <model> section.

void Model::addDelayedResolution ( ModelItem* item, const QString db_path )
{
  delay_resolutions_.append( new DelayedResolution( item, db_path ) );
}

void Model::write ( QDomElement& xml_rep ) const
{
  // Add the metadata
  QDomDocument document = xml_rep.ownerDocument();
  QDomElement model_element = document.createElement( lC::STR::MODEL );

  model_element.setAttribute( lC::STR::ID, id() );

  QDomElement metadata_element = document.createElement( "metadata" );
  model_element.appendChild( metadata_element );

  QDomElement name_element = document.createElement( lC::STR::NAME );
  QDomText name_text = document.createTextNode( name() );
  name_element.appendChild( name_text );
  metadata_element.appendChild( name_element );

  QDomElement description_element = document.createElement( lC::STR::DESCRIPTION );
  QDomText description_text = document.createTextNode( description_ );
  description_element.appendChild( description_text );
  metadata_element.appendChild( description_element );

  QDomElement created_element = document.createElement( lC::STR::CREATED );
  created_element.setAttribute( lC::STR::VALUE, created_.toString( Qt::ISODate ) );
  metadata_element.appendChild( created_element );

  QDomElement modified_element = document.createElement( lC::STR::MODIFIED );
  modified_element.setAttribute( lC::STR::VALUE, modified_.toString( Qt::ISODate ) );
  metadata_element.appendChild( modified_element );

  QDomElement version_element = document.createElement( lC::STR::LVERSION );
  version_element.setAttribute( lC::STR::VALUE, version_ );
  metadata_element.appendChild( version_element );

  QDomElement revision_element = document.createElement( lC::STR::REVISION );
  revision_element.setAttribute( lC::STR::VALUE, revision_ );
  metadata_element.appendChild( revision_element );

  // Now have the model items add themselves
  
  QDomElement pages_element = document.createElement( lC::STR::PAGES );
  model_element.appendChild( pages_element );

  QMap<uint, PageBase*>::const_iterator p = pages_.begin();
  for ( ; p != pages_.end(); ++p )
    p.data()->write( pages_element );

  xml_rep.appendChild( model_element );
}
