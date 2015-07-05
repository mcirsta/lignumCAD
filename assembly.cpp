/*
 * assembly.cpp
 *
 * Implementation of Assembly class
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

//#include <BRepTools.hxx>
#include <BRep_Builder.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>
#include <TopExp_Explorer.hxx>

#include "constants.h"
#include "dburl.h"
#include "model.h"
#include "part.h"
//#include "figure.h"
#include "ocsolid.h"
#include "subassembly.h"
#include "assembly.h"

#include "lcdebug.h"

using namespace Space3D;

uint Assembly::unique_index_ = 0;

QString Assembly::newName ( void )
{
  return tr( "Assembly[%1]" ).arg( ++unique_index_ );
}

Assembly::Assembly ( uint id, const QString& name, Model* parent )
  : Page( id, name, lC::STR::ASSEMBLY, parent )
{}

Assembly::Assembly ( uint id, const QDomElement& xml_rep, Model* parent )
  : Page( id, QString::null, lC::STR::ASSEMBLY, parent )
{
  setName( xml_rep.attribute( lC::STR::NAME ) );

  QRegExp regexp( tr( "Assembly\\[([0-9]+)\\]" ) );
  int position = regexp.indexIn( name() );
  if ( position >= 0 ) {
    Assembly::unique_index_ = qMax( regexp.cap(1).toUInt(),
				    Assembly::unique_index_ );
  }

  QDomNode n = xml_rep.firstChild();

  while ( !n.isNull() ) {
    QDomElement e = n.toElement();
    if ( !e.isNull() ) {
      if ( e.tagName() == lC::STR::SUBASSEMBLY ) {
	uint id = e.attribute( lC::STR::ID ).toUInt();
	Subassembly* subassembly = new Subassembly ( id, e, this );

	// If this is not the first figure and there are no constraints on it,
	// move it away from the origin. Still not the best, since its existing
	// constraints could still leave it at the origin... Still not
	// guaranteed to look the same, though...
	if ( figures_.count() > 1 &&
	     subassembly->constraints().status() != PlacementComplete ) {
	  subassembly->translate( gp_Vec( 20, 0, 0 ) );
	  subassembly->constraints().recompute();
	}

	connect( subassembly, SIGNAL( locationChanged()),SLOT(updateAssemblyLocation()));
	connect( subassembly, SIGNAL( solidChanged() ), SLOT( updateAssemblySolid()));
	//	connect( subassembly, SIGNAL( destroyed() ), SLOT( subassemblyDestroyed() ) );
      }
    }
    n = n.nextSibling();
  }
}

void Assembly::updateAssemblySolid ( void )
{
  //  cout << now() << path() << " changed due to solid change [before emit]" << endl;
  emit assemblyChanged();
  //  cout << now() << path() << " changed due to solid change [after emit]" << endl;
}

void Assembly::updateAssemblyLocation ( void )
{
  //  cout << now() << path() << " changed due to location change [before emit]" << endl;
  emit assemblyChanged();
  //  cout << now() << path() << " changed due to location change [after emit]" << endl;
}

Subassembly* Assembly::addModel ( Space3D::Page* model )
{
  Subassembly* subassembly = new Subassembly( newID(), Subassembly::newName(),
					      model, this );
  // If this is not the first model, then shuffle this one around so
  // that is not on top of the assembly.
  if ( figures_.count() > 1 ) {
    //    subassembly->rotate( gp::DY(), M_PI / 4  );
    //    subassembly->rotate( gp::DZ(), M_PI / 4  );
    subassembly->translate( gp_Vec( 20, 0, 0 ) );
  }

  connect( subassembly, SIGNAL( locationChanged() ), SIGNAL( assemblyChanged() ) );
  connect( subassembly, SIGNAL( solidChanged() ), SIGNAL( assemblyChanged() ) );
  //  connect( subassembly, SIGNAL( destroyed() ), SLOT( subassemblyDestroyed() ) );

  //  cout << path() << " changed due to new model [before emit]" << endl;
  emit assemblyChanged();
  //  cout << path() << " changed due to new model [after emit]" << endl;

  return subassembly;
}

void Assembly::subassemblyDestroyed ( void )
{
  //  cout << "Got signal for destruction of subassembly" << endl;
  emit assemblyChanged();
}

Handle(Standard_Type) Assembly::lookupType ( QStringList& path_components ) const
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

Handle(Standard_Type) Assembly::lookupType ( QVector<uint>& id_path ) const
{
  QMap<uint,Figure*>::const_iterator figure = figures_.find( id_path[0] );

  if ( figure != figures_.end() ) {
    id_path.erase( id_path.begin() );
    if ( !id_path.empty() )
      return figure.value()->lookupType( id_path );
  }

  return Handle(Standard_Type)(); // Really an error...
}

TopoDS_Shape Assembly::lookupShape ( QStringList& path_components ) const
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
    return figure.value()->lookupShape( path_components );
      else
	break;
    }
  }

  return TopoDS_Shape();	// Really an error...
}

TopoDS_Shape Assembly::lookupShape ( QVector<uint>& id_path ) const
{
  QMap<uint,Figure*>::const_iterator figure = figures_.find( id_path[0] );

  if ( figure != figures_.end() ) {
    id_path.erase( id_path.begin() );
    if ( !id_path.empty() )
      return figure.value()->lookupShape( id_path );
  }

  return TopoDS_Shape();	// Really an error...
}

bool Assembly::used ( const PageBase* page ) const
{
  QMap<uint,Figure*>::const_iterator figure = figures_.begin();

  for ( ; figure != figures_.end(); ++figure ) {
    Subassembly* subassembly = dynamic_cast<Subassembly*>( figure.value() );
    if ( subassembly != 0 )
      if ( subassembly->subassembly() == page ) {
	return true;
      }
  }

  return false;
}

bool Assembly::referenced ( const Subassembly* target ) const
{
  QMap<uint,Figure*>::const_iterator figure = figures_.begin();

  for ( ; figure != figures_.end(); ++figure ) {
    Subassembly* subassembly = dynamic_cast<Subassembly*>( figure.value() );
    if ( subassembly != 0 ) {
      if ( subassembly == target )
	continue;		// Skip self, of course.
      else if ( subassembly->referenced( target ) ) { // Otherwise, ask each sa.
	return true;
      }
    }
  }

  return false;
}

QString Assembly::idPath ( QVector<uint> id_path ) const
{
  // The argument path must not have zero length, i.e., it must
  // refer to a figure (the Model has already taken care of
  // including *this* page in the path string).
  QMap<uint,Figure*>::const_iterator f = figures_.find( id_path[0] );

  if ( f == figures_.end() )
    return QString::null;	// Really an error...

  id_path.erase( id_path.begin() );

  if ( id_path.empty() )
    return f.value()->name() + '.' +  f.value()->type();

  return f.value()->name() + '.' +  f.value()->type() + '/' +
    f.value()->idPath( id_path );
}

void Assembly::pathID ( QStringList& path_components, QVector<uint>& id_path )
  const
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
      id_path.push_back( figure.value()->id() );

      path_components.erase( path_components.begin() );

      if ( !path_components.empty() ) {
    figure.value()->pathID( path_components, id_path );
      }
    }
  }
}

void Assembly::write ( QDomElement& xml_rep ) const
{
  QDomDocument document = xml_rep.ownerDocument();
  QDomElement assembly_element = document.createElement( lC::STR::ASSEMBLY );
  assembly_element.setAttribute( lC::STR::ID, id() );
  assembly_element.setAttribute( lC::STR::NAME, name() );

  QMap<uint,Figure*>::const_iterator figure = figures_.begin();

  for ( ; figure != figures_.end(); ++figure )
    figure.value()->write( assembly_element );

  xml_rep.appendChild( assembly_element );
}

TopoDS_Compound Assembly::compound ( void )
{
  TopoDS_Compound assembly;
  BRep_Builder builder;
  builder.MakeCompound( assembly );

  QMap<uint,Figure*>::const_iterator figure = figures_.begin();

  for ( ; figure != figures_.end(); ++figure ) {
    Subassembly* subassembly = dynamic_cast<Subassembly*>( figure.value() );
    if ( subassembly != 0 ) {
      subassembly->compound( assembly );
    }
  }

  return assembly;
}
