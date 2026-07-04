/*
 * subassembly.cpp
 *
 * Implementation of Subassembly class
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

#include <BRep_Builder.hxx>
#include <BRepTools.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Face.hxx>
#include <TopExp_Explorer.hxx>

#include "constants.h"
#include "dburl.h"
#include "model.h"
#include "assembly.h"
#include "part.h"
#include "ocsolid.h"
#include "subassembly.h"

#include "lcdebug.h"

using namespace Space3D;

uint Subassembly::unique_index_ = 0;

QString Subassembly::newName ( void )
{
  return tr( "Subassembly[%1]" ).arg( ++unique_index_ );
}

Subassembly::Subassembly ( uint id, const QString& name, Space3D::Page* subassembly,
			   Space3D::Page* parent )
  : Space3D::Figure( id, name, lC::STR::SUBASSEMBLY, parent ),
	     subassembly_( subassembly ), constraints_( this )
{
  init();
}

Subassembly::Subassembly ( uint id, const QDomElement& xml_rep,
			   Space3D::Page* parent )
  : Space3D::Figure( id, QString::null, lC::STR::SUBASSEMBLY, parent ),
	     constraints_( this )
{
  setName( xml_rep.attribute( lC::STR::NAME ) );

  QRegExp regexp( tr("Subassembly\\[([0-9]+)\\]" ) );
  int position = regexp.search( name() );
  if ( position >= 0 ) {
    Subassembly::unique_index_ = QMAX( regexp.cap(1).toUInt(),
				       Subassembly::unique_index_ );
  }

  DBURL url = xml_rep.attribute( lC::STR::SUBASSEMBLY );
  subassembly_ = dynamic_cast<Space3D::Page*>( parent->model()->lookup( url ) );

  if ( subassembly_ == 0 ) {
    parent->model()->addDelayedResolution( this, url );
    delayed_constraints_ = xml_rep;
    return;
  }

  init();

  QDomNode n = xml_rep.firstChild();

  while ( !n.isNull() ) {
    QDomElement e = n.toElement();
    if ( !e.isNull() ) {
      if ( e.tagName() == lC::STR::CONSTRAINTS ) {
	constraints_.addConstraints( e );
      }
    }
    n = n.nextSibling();
  }
}

void Subassembly::init ( void )
{
  if ( dynamic_cast<Part*>( subassembly_ ) != 0 ) {
    Part* part = dynamic_cast<Part*>( subassembly_ );
    connect( part->solid(), SIGNAL( modified() ), SLOT( updateSolid() ) );
    connect( part->solid(), SIGNAL( materialChanged() ), SIGNAL(materialChanged()));
  }
  else if ( dynamic_cast<Assembly*>( subassembly_ ) != 0 ) {
    Assembly* assembly = dynamic_cast<Assembly*>( subassembly_ );
    connect( assembly, SIGNAL( assemblyChanged() ), SLOT( updateSolid() ) );
  }
}

void Subassembly::resolved ( const QString& /*db_path*/, ModelItem* item )
{
  subassembly_ = dynamic_cast<Space3D::Page*>( item );

  init();

  QDomNode n = delayed_constraints_.firstChild();

  while ( !n.isNull() ) {
    QDomElement e = n.toElement();
    if ( !e.isNull() ) {
      if ( e.tagName() == lC::STR::CONSTRAINTS ) {
	constraints_.addConstraints( e );
      }
    }
    n = n.nextSibling();
  }
}

void Subassembly::newConstraint ( const AssemblyConstraint* constraint )
{
  emit constraintCreated( constraint );
}

void Subassembly::changeConstraint ( const AssemblyConstraint* old_constraint,
				     const AssemblyConstraint* new_constraint )
{
  emit constraintChanged( old_constraint, new_constraint );
}

void Subassembly::changeConstraintOffset ( const AssemblyConstraint* constraint )
{
  emit constraintOffsetChanged( constraint );
}

void Subassembly::cancelLast ( void )
{
  emit constraintCanceled();
}

void Subassembly::addDependency ( const QVector<uint>& surface_id )
{
#if 0
  // Basically, the idea here is to connect to each component of the surface_id
  // path whose alteration could affect this subassembly. There are two classes
  // of changes which are important:
  // 1. Modification of a solid (or, more generally, modification of a part
  // but the only interesting thing in a part currently is the solid).
  // 2. Transformation of a subassembly to a new location.
  //
  // So, exhustively examine each element of the path and see what it means to
  // us. Note, since part faces are not real ModelItems, we skip the last
  // element of the id_path.
  QVector<uint> id_path;
  id_path.reserve( surface_id.size()-1 );

  for ( uint i = 0; i < surface_id.size()-1; ++i ) {
    id_path.push_back( surface_id[i] );
    ModelItem* item = model()->lookup( id_path );
    // The first order check is that we don't already depend on this item.
    if ( dependencies_.findRef( item ) != -1 ) {
      continue;
    }
    // If it's a subassembly, then our location may depend on its location
    // (unless it is ourself)
    else if ( dynamic_cast<Subassembly*>( item ) != 0 && item != this ) {
      dependencies_.append( item );
      connect( item, SIGNAL( locationChanged() ), SLOT( updateLocation() ) );
    }
    // If it's a solid, then we probably depend on the exact geometry
    // of its faces.
    else if ( dynamic_cast<Space3D::OCSolid*>( item ) != 0 ) {
      // Unless our own subassembly model *is* this part.
      if ( dynamic_cast<Part*>( subassembly_ ) != 0 &&
	   dynamic_cast<Part*>( subassembly_ )->solid() == item )
	continue;

      dependencies_.append( item );
      connect( dynamic_cast<Space3D::OCSolid*>(item), SIGNAL( modified() ),
	       SLOT( updateLocation() ) );
    }
  }
#else
  // I think the above was too much. We should depend on a signal from the
  // top level subassembly to indicate that one of its children has been
  // modified. At the subassembly level, it doesn't matter if the reference
  // has changed due a solid resize or just a sub-subassembly having been
  // moved. I think?...
  QVector<uint> id_path;
  id_path.reserve( surface_id.size()-1 );

  for ( uint i = 0; i < surface_id.size()-1; ++i ) {
    id_path.push_back( surface_id[i] );
    ModelItem* item = model()->lookup( id_path );
    // The first order check is that we don't already depend on this item.
    if ( dependencies_.findRef( item ) != -1 ) {
      continue;
    }
    // If it's the top level subassembly of the reference, then our location
    // may depend on its location (unless it is ourself)
    else if ( dynamic_cast<Subassembly*>( item ) != 0 && item != this ) {
      dependencies_.append( item );
      connect( item, SIGNAL( locationChanged() ), SLOT( updateLocation() ) );
      break;			// And that's it. Don't look any more.
    }
#if 0
    // If it's a solid, then we probably depend on the exact geometry
    // of its faces.
    else if ( dynamic_cast<Space3D::OCSolid*>( item ) != 0 ) {
      // Unless our own subassembly model *is* this part.
      if ( dynamic_cast<Part*>( subassembly_ ) != 0 &&
	   dynamic_cast<Part*>( subassembly_ )->solid() == item )
	continue;

      dependencies_.append( item );
      connect( dynamic_cast<Space3D::OCSolid*>(item), SIGNAL( modified() ),
	       SLOT( updateLocation() ) );
    }
#endif
 }
#endif
}

void Subassembly::rotate ( gp_Dir direction, double angle )
{
  //  cout << path() << " rotate" << endl;
  // Rotate the subassembly about its current origin in the global
  // coordinate system. This is equivalent to translating the solid
  // to the origin performing the rotation and translating back
  // to the original displacement.

  gp_Trsf transform = location_;
  gp_Trsf rotation;
  rotation.SetRotation( gp_Ax1( transform.TranslationPart(), direction ), angle );
  transform.PreMultiply( rotation );
  location_ = TopLoc_Location( transform );
#if 0
  cout << path() << " rotated [before emit]" << endl;
  emit locationChanged();
  cout << path() << " rotated [after emit]" << endl;
#endif
}

void Subassembly::translate ( gp_Vec displacement )
{
  //  cout << path() << " translate" << endl;

  gp_Trsf transform = location_;

  gp_Trsf translation;
  translation.SetTranslation( displacement );
  transform.PreMultiply( translation );
  location_ = TopLoc_Location( transform );
#if 0
  cout << path() << " translated [before emit]" << endl;
  emit locationChanged();
  cout << path() << " translated [after emit]" << endl;
#endif
}

void Subassembly::updateSolid ( void )
{
  constraints_.recompute();
  //  cout << now() << path() << " changed due to solid change [before emit]" << endl;
  emit locationChanged();
  //  cout << now() << path() << " changed due to solid change [after emit]" << endl;
}

void Subassembly::updateLocation ( void )
{
  constraints_.recompute();
  // Not ordinarily necessary, since the OpenGL transform matrices are
  // computed on the fly; but may be necessary for hidden line mode...
  //  cout << now() << path() << " changed due to location change [before emit]" << endl;
  emit locationChanged();
  //  cout << now() << path() << " changed due to location change [after emit]" << endl;
}

void Subassembly::updateMaterial ( void )
{
  emit materialChanged();
}

void Subassembly::write ( QDomElement& xml_rep ) const
{
  QDomDocument document = xml_rep.ownerDocument();
  QDomElement subassembly_element = document.createElement( lC::STR::SUBASSEMBLY );
  subassembly_element.setAttribute( lC::STR::ID, id() );
  subassembly_element.setAttribute( lC::STR::NAME, name() );
  subassembly_element.setAttribute( lC::STR::SUBASSEMBLY,
				    subassembly_->dbURL().toString(true) );

  constraints_.write( subassembly_element );

  xml_rep.appendChild( subassembly_element );
}

void Subassembly::copyWrite ( QDomElement& /*xml_rep*/ ) const {}
ModelItem* Subassembly::lookup ( QStringList& /*path_components*/ ) const { return 0; }

// Find the ModelItem given by the path.

ModelItem* Subassembly::lookup ( QVector<uint>& id_path ) const
{
  if ( subassembly_->id() == id_path[0] ) {
    id_path.erase( id_path.begin() );
    if ( id_path.empty() )
      return subassembly_;
    else
      return subassembly_->lookup( id_path );
  }

  return 0;			// Really an error...
}

// Find the OpenCASCADE type associated with the path (presumably some kind
// of geometry)

Handle(Standard_Type) Subassembly::lookupType ( QStringList& path_components ) const
{
  // The front path component is the name of a figure with ".type" appended
  // to it.
  int dot_pos = path_components.front().findRev( '.' );
  QString name = path_components.front().left( dot_pos );
  QString type = path_components.front().right( path_components.front().length()
						- dot_pos - 1 );

  if ( subassembly_->name() == name && subassembly_->type() == type ) {
    path_components.erase( path_components.begin() );
    return subassembly_->lookupType( path_components );
  }

  return Handle(Standard_Type)();
}

// Find the OpenCASCADE type associated with the path (presumably some kind
// of geometry)

Handle(Standard_Type) Subassembly::lookupType ( QVector<uint>& id_path ) const
{
  if ( subassembly_->id() == id_path[0] ) {
    id_path.erase( id_path.begin() );
    return subassembly_->lookupType( id_path );
  }

  return Handle(Standard_Type)();
}

// Find the OpenCASCADE shape associated with the path (presumably some kind
// of geometry)

TopoDS_Shape Subassembly::lookupShape ( QStringList& path_components ) const
{
  // The front path component is the name of a figure with ".type" appended
  // to it.
  int dot_pos = path_components.front().findRev( '.' );
  QString name = path_components.front().left( dot_pos );
  QString type = path_components.front().right( path_components.front().length()
						- dot_pos - 1 );
  TopoDS_Shape shape;

  if ( subassembly_->name() == name && subassembly_->type() == type ) {
    path_components.erase( path_components.begin() );
    // The shape returned here must be modified by the transformation
    // of the subassembly.
    shape = subassembly_->lookupShape( path_components );
    shape.Location( location_ * shape.Location() );
  }

  return shape;
}

// Find the OpenCASCADE shape associated with the path (presumably some kind
// of geometry)

TopoDS_Shape Subassembly::lookupShape ( QVector<uint>& id_path ) const
{
  TopoDS_Shape shape;

  if ( subassembly_->id() == id_path[0] ) {
    id_path.erase( id_path.begin() );
    shape = subassembly_->lookupShape( id_path );
    shape.Location( location_ * shape.Location() );
  }

  return shape;
}

bool Subassembly::referenced ( const Subassembly* target ) const
{
  // What to do? Look at the ID of each constraint's reference1 and see
  // if target is a prefix match of the reference?
  QVector<uint> target_id = target->ID();

  QPtrListIterator<AssemblyConstraint> constraint = constraints_.constraints();
  for ( ; constraint.current() != 0; ++constraint ) {
    QVector<uint> reference1 = constraint.current()->reference1();
    if ( !reference1.empty() && reference1.size() >= target_id.size() ) {
      bool equal = true;
      for ( uint i = 0; i < target_id.size(); i++ ) {
	if ( reference1[i] != target_id[i] ) {
	  equal = false;
	  break;
	}
      }
      if ( equal )
	return true;
    }
  }

  return false;
}

QString Subassembly::idPath ( QVector<uint> id_path ) const
{
  if ( subassembly_->id() != id_path[0] )
    return QString::null;	// Really an error...

  id_path.erase( id_path.begin() );

  if ( id_path.empty() )
    return subassembly_->name() + '.' + subassembly_->type();

  return subassembly_->name() + '.' + subassembly_->type() + '/' +
    subassembly_->idPath( id_path );
}

void Subassembly::pathID ( QStringList& path_components,
			   QVector<uint>& id_path ) const
{
  // The front path component is the name of a figure with ".type" appended
  // to it.
  int dot_pos = path_components.front().findRev( '.' );
  QString name = path_components.front().left( dot_pos );
  QString type = path_components.front().right( path_components.front().length()
						- dot_pos - 1 );

  if ( subassembly_->name() == name && subassembly_->type() == type ) {

    id_path.push_back( subassembly_->id() );

    path_components.erase( path_components.begin() );

    if ( !path_components.empty() ) {
      subassembly_->pathID( path_components, id_path );
    }
  }
}

void Subassembly::compound ( TopoDS_Shape& assembly )
{
  BRep_Builder builder;
  if ( dynamic_cast<Part*>( subassembly_ ) != 0 ) {
    TopoDS_Shape placed_solid =
      dynamic_cast<Part*>( subassembly_ )->shape().Located( location_ );

    builder.Add( assembly, placed_solid );
  }
  else if ( dynamic_cast<Assembly*>( subassembly_ ) != 0 ) {
    TopoDS_Shape placed_solid =
      dynamic_cast<Assembly*>( subassembly_ )->compound().Located( location_ );

    builder.Add( assembly, placed_solid );
  }
}
