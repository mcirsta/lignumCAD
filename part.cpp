/*
 * part.cpp
 *
 * Implementation of Part class
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

#include <qdom.h>
#include <qregexp.h>
#include <qlistview.h>

#include <BRepTools.hxx>

#include "constants.h"
#include "dburl.h"
#include "model.h"
#include "material.h"
#include "ocsolid.h"
#include "part.h"

using namespace Space3D;

uint Part::unique_index_ = 0;

QString Part::newName ( void )
{
  return tr( "Part[%1]" ).arg( ++unique_index_ );
}

Part::Part ( uint id, const QString& name, Model* parent )
  : Page( id, name, lC::STR::PART, parent ), solid_( 0 )
{
  init();
}

Part::Part ( uint id, const QDomElement& xml_rep, Model* parent )
  : Page( id, QString::null, lC::STR::PART, parent ), solid_( 0 )
{
  setName( xml_rep.attribute( lC::STR::NAME ) );

  QRegExp regexp( tr( "Part\\[([0-9]+)\\]" ) );
  int position = regexp.indexIn( name() );
  if ( position >= 0 ) {
    Part::unique_index_ = qMax( regexp.cap(1).toUInt(), Part::unique_index_ );
  }

  QDomNode n = xml_rep.firstChild();

  while ( !n.isNull() ) {
    QDomElement e = n.toElement();
    if ( !e.isNull() ) {
      if ( e.tagName() == lC::STR::SOLID ) {
	solid_ = PartFactory::instance()->create( this, e );
      }
    }
    n = n.nextSibling();
  }

  init();
}

void Part::init ( void )
{
  if ( solid_ != 0 ) {
    connect( this, SIGNAL( nameChanged(const QString&) ),
	     solid_, SLOT( setName(const QString&) ) );
  }
}

const TopoDS_Shape& Part::shape ( void ) const
{
  return solid_->shape();
}

void Part::write ( QDomElement& xml_rep ) const
{
  QDomDocument document = xml_rep.ownerDocument();
  QDomElement part_element = document.createElement( lC::STR::PART );
  part_element.setAttribute( lC::STR::ID, id() );
  part_element.setAttribute( lC::STR::NAME, name() );

  QMap<uint,Figure*>::const_iterator figure = figures_.begin();

  for ( ; figure != figures_.end(); ++figure )
    figure.value()->write( part_element );

  xml_rep.appendChild( part_element );
}

// Use the PartFactory interface to create the initial solid geometry

void Part::makeSolidParameters (const PartMetadata* part,
                 const QHash<int, lCDefaultLengthConstraint> &parameters )
{
  solid_ = part->create( name(), parameters, this );

  connect( this, SIGNAL( nameChanged(const QString&) ),
	   solid_, SLOT( setName(const QString&) ) );
}

// Apply this material to the solid

void Part::setSolidMaterial ( const Material* material )
{
  if ( solid_ != 0 )
    solid_->setMaterial( material );
}

Handle(Standard_Type) Part::lookupType ( QStringList& path_components ) const
{
  // The front path component is the name of a figure with ".type" appended
  // to it.
  int dot_pos = path_components.front().lastIndexOf( '.' );
  QString name = path_components.front().left( dot_pos );
  QString type = path_components.front().right( path_components.front().length()
						- dot_pos - 1 );

  QMap<uint,Figure*>::const_iterator figure = figures_.begin();

  for ( ; figure != figures_.end(); ++figure ) {
    if ( figure.value()->name() == name && figure.value()->type() == type ) {
      path_components.erase( path_components.begin() );
      if ( !path_components.empty() )
    return figure.value()->lookupType( path_components );
      else
	break;
    }
  }

  return Handle(Standard_Type)(); // Really an error...
}

Handle(Standard_Type) Part::lookupType ( QVector<uint>& id_path ) const
{
  QMap<uint,Figure*>::const_iterator figure = figures_.find( id_path[0] );

  if ( figure != figures_.end() ) {
    id_path.erase( id_path.begin() );
    if ( id_path.empty() )
      return Handle(Standard_Type)();
    else
      return figure.value()->lookupType( id_path );
  }

  return Handle(Standard_Type)();
}

TopoDS_Shape Part::lookupShape ( QStringList& path_components ) const
{
  // The front path component is the name of a figure with ".type" appended
  // to it.
  int dot_pos = path_components.front().indexOf( '.' );
  QString name = path_components.front().left( dot_pos );
  QString type = path_components.front().right( path_components.front().length()
						- dot_pos - 1 );
  QMap<uint,Figure*>::const_iterator figure = figures_.begin();

  for ( ; figure != figures_.end(); ++figure ) {
    if ( figure.value()->name() == name && figure.value()->type() == type ) {
      path_components.erase( path_components.begin() );
      if ( path_components.empty() )
	return TopoDS_Shape();
      else
    return figure.value()->lookupShape( path_components );
    }
  }

  return TopoDS_Shape();
}

TopoDS_Shape Part::lookupShape ( QVector<uint>& id_path ) const
{
  QMap<uint,Figure*>::const_iterator figure = figures_.find( id_path[0] );

  if ( figure != figures_.end() ) {
    id_path.erase( id_path.begin() );
    if ( id_path.empty() )
      return TopoDS_Shape();
    else
      return figure.value()->lookupShape( id_path );
  }

  return TopoDS_Shape();
}

QString Part::idPath ( QVector<uint> id_path ) const
{
  // The argument path must not have zero length, i.e., it must
  // refer to a figure (the Model has already taken care of
  // including *this* page in the path string).
  QMap<uint,Figure*>::const_iterator f = figures_.find( id_path[0] );

  if ( f == figures_.end() )
    return QString::null;	// Really an error...

  id_path.erase( id_path.begin() );

  if ( id_path.empty() )
    return f.value()->name() + '.' + f.value()->type();

  return f.value()->name() + '.' + f.value()->type() + '/' +
    f.value()->idPath( id_path );
}

void Part::pathID (QStringList& path_components, QVector<uint> &id_path )const
{
  // The front path component is the name of a figure with ".type" appended
  // to it.
  int dot_pos = path_components.front().indexOf( '.' );
  QString name = path_components.front().left( dot_pos );
  QString type = path_components.front().right( path_components.front().length()
						- dot_pos - 1 );

  QMap<uint,Figure*>::const_iterator figure = figures_.begin();

  for ( ; figure != figures_.end(); ++figure ) {
    if ( figure.value()->name() == name && figure.value()->type() == type ) {
      id_path.push_back( figure.value()->id() );

      path_components.erase( path_components.begin() );

      if ( !path_components.empty() ) {
    figure.value()->pathID( path_components, id_path );
      }
    }
  }
}

// The part metadata constructor adds this template to the library

PartMetadata::PartMetadata ( const QString& group, const QString& name,
			     const QString& icon, const QString& solid )
  : group_( group ), name_( name ), icon_( icon ), solid_( solid )
{
  PartFactory::instance()->addPartMetadata( this );
}

// Add the name of a parameter to the template

void PartMetadata::addParameter ( const QString& parameter_name )
{
  parameter_names_.append( parameter_name );
}

// The part library instance.

PartFactory* PartFactory::instance_ = 0;

// Setup the part library.

PartFactory::PartFactory ( void )
  : unique_index_( 0 ), unique_id_( 0 )
{}

// Lazily create the part library.

PartFactory* PartFactory::instance ( void )
{
  if ( instance_ == 0 )
    instance_ = new PartFactory;

  return instance_;
}

// Add this part template to the library.

void PartFactory::addPartMetadata ( PartMetadata* part_data )
{
  part_data_.push_back( std::shared_ptr<PartMetadata> (part_data) );
}

// Look up a part template's metadata

PartMetadata* PartFactory::part ( const QString& group, const QString& name )
{
  // Perhaps something more clever is in order...
  QListIterator< std::shared_ptr<PartMetadata> > part( part_data_ );
  while( part.hasNext() ) {
     PartMetadata* tmpPm = part.next().get();
    if ( tmpPm->group() == group &&
     tmpPm->name() == name )
      return tmpPm;
  }
  return 0;
}

Space3D::OCSolid* PartFactory::create ( Part* parent,
					const QDomElement& xml_rep ) const
{
  QString name = xml_rep.attribute( lC::STR::NAME );
  QString material = xml_rep.attribute( lC::STR::MATERIAL );

  QDomNode n = xml_rep.firstChild();

  if ( !n.isNull() ) {
    QDomElement e = n.toElement();
    if ( !e.isNull() ) {
      QString solid = e.tagName();

      QListIterator< std::shared_ptr<PartMetadata> > part( part_data_ );
      while ( part.hasNext() ) {
          PartMetadata* tmpPm = part.next().get();
    if ( tmpPm->solid() == solid ) {
      Space3D::OCSolid* ocsolid =  tmpPm->create( name, e, parent );
	  if ( !material.isEmpty() )
	    ocsolid->setMaterial( MaterialDatabase::instance().
				  materialCommon( material ) );
	  return ocsolid;
	}
      }
    }
  }

  return 0; // But should be an error...
}

// Return a name and id for a geometry item.

std::pair<QString,uint> PartFactory::name ( const QString& suggestion )
{
  // If there's no suggestion, make something completely up.
  if ( suggestion.isEmpty() ) {
    QString name = QString( "n[%1]" ).arg( ++unique_index_ );
    uint id = ++unique_id_;
    std::pair<QString,uint> name_id( name, id );
    names_.insert( name_id );
    return name_id;
  }

  // If it's already there, return that.
  std::map<QString,uint>::const_iterator entry = names_.find( suggestion );
  if ( entry != names_.end() ) {
    return *entry;
  }
  // Otherwise, use the suggested name but with a unique id.
  std::pair<QString,uint> name_id( suggestion, ++unique_id_ );
  names_.insert( name_id );

  return name_id;
}

// Return the name corresponding to the the id.

QString PartFactory::name ( uint id )
{
  std::map<QString,uint>::const_iterator entry = names_.begin();
  for ( ; entry != names_.end(); ++entry )
    if ( (*entry).second == id )
      return (*entry).first;

  return QString::null;
}
