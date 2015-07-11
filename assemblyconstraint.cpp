/*
 * assemblyconstraint.cpp
 *
 * AssemblyConstraintManager class: model the constraints which locate a subassembly
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

#include <gp_Ax2.hxx>
#include <gp_Pln.hxx>
//#include <Bnd_Box.hxx>
//#include <BRepBndLib.hxx>
#include <BRep_Tool.hxx>
#include <BRepTools.hxx>
#include <Geom_Plane.hxx>
#include <Geom_Surface.hxx>
#include <GeomTools.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopExp_Explorer.hxx>

#include "constants.h"
#include "dburl.h"
#include "model.h"
#include "assembly.h"
#include "subassembly.h"
#include "assemblyconstraint.h"

#include "lcdebug.h"

/*!
 * Generic example of constraining two planes.
 */
struct PlnPln : public SurfacePair {

  PlnPln ( const AssemblyConstraint* parent )
    : SurfacePair( parent )
  {}

  PlnPln ( const QDomElement& xml_rep, const AssemblyConstraint* parent )
    : SurfacePair( parent )
  {
    QString surface0_db_url = xml_rep.attribute( lC::STR::SURFACE0 );

    if ( !surface0_db_url.isEmpty() ) {
      DBURL surface0_db( surface0_db_url );

      surface0_id_ = parent_->model()->pathID( DBURL( surface0_db_url ) );

      face0_ = TopoDS::Face( parent_->model()->lookupShape( surface0_id_ ) );
      plane0_ = Handle(Geom_Plane)::DownCast( BRep_Tool::Surface( face0_ ) );
    }

    QString surface1_db_url = xml_rep.attribute( lC::STR::SURFACE1 );

    if ( !surface1_db_url.isEmpty() ) {
      DBURL surface1_db( surface1_db_url );

      surface1_id_ = parent_->model()->pathID( DBURL( surface1_db_url ) );

      face1_ = TopoDS::Face( parent_->model()->lookupShape( surface1_id_ ) );
      plane1_ = Handle(Geom_Plane)::DownCast( BRep_Tool::Surface( face1_ ) );
    }
  }

  //! \return the id path to the first plane (and let the requestor
  //! parse it out as desired).
  QVector<uint> reference0 ( void ) const
  {
    return surface0_id_;
  }

  //! \return the id path to the second plane (and let the requestor
  //! parse it out as desired).
  QVector<uint> reference1 ( void ) const
  {
    return surface1_id_;
  }
  /*!
   * Remove the reference to the second surface. Should also un-transform
   * the subassembly back to where is was...
   */
  void removeLastReference ( void )
  {
    surface1_id_.clear();
    face1_.Nullify();
    plane1_.Nullify();
  }
  /*!
   * Recompute the constraint due to the subassembly having changed. But
   * only if this constraint is complete!
   */
  void recompute ( void )
  {
    if ( surface0_id_.empty() || surface1_id_.empty() ) return;

    face0_ = TopoDS::Face( parent_->model()->lookupShape( surface0_id_ ) );
    plane0_ = Handle(Geom_Plane)::DownCast( BRep_Tool::Surface( face0_ ) );

    face1_ = TopoDS::Face( parent_->model()->lookupShape( surface1_id_ ) );
    plane1_ = Handle(Geom_Plane)::DownCast( BRep_Tool::Surface( face1_ ) );

    transform();
  }

  //! \return the offset (0 by default, though).
  virtual double offset ( void ) const { return 0; }
  /*!
   * Store the plane's new offset (ignored by default, though).
   * \param offset new offset.
   */
  virtual void setOffset ( double /*offset*/ ) {}
  /*!
   * Update the plane's offset implies that the subassembly transformation
   * is recomputed (ignored by default, though).
   * \param offset new offset.
   */
  virtual void updateOffset ( double /*offset*/ ) {}

  //! \return the first end of a dimension representing the constraint's offset.
  Space3D::Point end0 ( void ) const { return end0_; }

  //! \return the second end of a dimension representing the constraint's offset.
  Space3D::Point end1 ( void ) const { return end1_; }

  //! \return the normal direction of a dimension representing the constraint's
  //! offset.
  Space3D::Vector normal ( void ) const { return normal_; }

  /*!
   * Serialize constraint to XML representation. (At least) the offset
   * versions also have to append some extra data to the representation;
   * so, this method is virtual.
   * \param xml_rep Manager XML representation to append to.
   */
  virtual void write ( QDomElement& xml_rep ) const
  {
    xml_rep.setAttribute( lC::STR::SURFACE, lC::STR::PLANE );
    QString surface0_path = parent_->model()->idPath( surface0_id_ );
    QUrl::toPercentEncoding( surface0_path );
    DBURL surface0_db_url( lC::STR::DB_PREFIX, surface0_path );
    xml_rep.setAttribute( lC::STR::SURFACE0, surface0_db_url.toString( ) );

    QString surface1_path = parent_->model()->idPath( surface1_id_ );
    QUrl::toPercentEncoding( surface1_path );
    DBURL surface1_db_url( lC::STR::DB_PREFIX, surface1_path );
    xml_rep.setAttribute( lC::STR::SURFACE1, surface1_db_url.toString( ) );
  }

  //! Compute the parameters of a dimension.

  void updateDimension ( void )
  {
    // Recompute face0 and plane0.
    face0_ = TopoDS::Face( parent_->model()->lookupShape( surface0_id_));
    plane0_ = Handle(Geom_Plane)::DownCast( BRep_Tool::Surface( face0_ ) );

    // 1. Arbitrarily take the first vertex from face0 for end0.
    TopExp_Explorer vertex( face0_, TopAbs_VERTEX );
    gp_Pnt end0 = BRep_Tool::Pnt( TopoDS::Vertex( vertex.Current() ) );
    end0_ = Space3D::Point( end0.X(), end0.Y(), end0.Z() );

    // 2. Find the closest vertex in face1 for end1.
    vertex.Init( face1_, TopAbs_VERTEX );
    gp_Pnt end1 = BRep_Tool::Pnt( TopoDS::Vertex( vertex.Current() ) );
    vertex.Next();
    for ( ; vertex.More(); vertex.Next() ) {
      gp_Pnt v = BRep_Tool::Pnt( TopoDS::Vertex( vertex.Current() ) );
      if ( end0.SquareDistance( v ) < end0.SquareDistance( end1 ) )
	end1 = v;
    }
    end1_ = Space3D::Point( end1.X(), end1.Y(), end1.Z() );

    // 3. Compute a direction for the dimension extension lines (aka, the
    // dimension normal). Start with the projection of the vector from
    // end1 to end0 onto the plane of face1. (Note that face0 and face1
    // are parallel at this point.)
    gp_Vec d_ends( end1, end0 );
    double nx = d_ends * plane1_->Pln().XAxis().Direction();
    double ny = d_ends * plane1_->Pln().YAxis().Direction();
    gp_Vec n = nx * plane1_->Pln().XAxis().Direction() +
      ny * plane1_->Pln().YAxis().Direction();
    // If this vector is not null (i.e., end0 and end1 don't line up along
    // the face normal), use it as the dimension normal.
    if ( n.SquareMagnitude() > Precision::Confusion() ) {
      normal_ = Space3D::Vector( n.X(), n.Y(), n.Z() );
      normal_.normalize();
    }
    // Otherwise, we just pick an arbitrary direction in the plane of face1.
    else {
      gp_Dir nnx = plane1_->Pln().XAxis().Direction();
      normal_ = Space3D::Vector( nnx.X(), nnx.Y(), nnx.Z() );
    }
  }

  QVector<uint> surface0_id_;
  TopoDS_Face face0_;
  Handle( Geom_Plane ) plane0_;

  QVector<uint> surface1_id_;
  TopoDS_Face face1_;
  Handle( Geom_Plane ) plane1_;
  // Optional, really...
  Space3D::Point end0_;
  Space3D::Point end1_;
  Space3D::Vector normal_;
};

struct MatePlnPln0 : public PlnPln {
  MatePlnPln0 ( const AssemblyConstraint* parent )
    : PlnPln( parent )
  {}

  MatePlnPln0 ( const QDomElement& xml_rep, const AssemblyConstraint* parent )
    : PlnPln( xml_rep, parent )
  {
    // I guess it's possible that the XML representation was not complete.
    if ( ! ( face0_.IsNull() || face1_.IsNull() ) )
      transform();
  }

  //! \return a (more or less) deep copy of this object.

  MatePlnPln0* clone ( void ) const
  {
    MatePlnPln0* new_plnpln = new MatePlnPln0( parent_ );

    new_plnpln->surface0_id_ = surface0_id_;
    new_plnpln->face0_ = face0_;
    new_plnpln->plane0_ = plane0_;
    new_plnpln->surface1_id_ = surface1_id_;
    new_plnpln->face1_ = face1_;
    new_plnpln->plane1_ = plane1_;

    return new_plnpln;
  }

  //! \return the coordinate system characteristic of this constraint.

  gp_Ax2 characteristic ( void ) const
  {
    if ( plane1_.IsNull() )
      return gp::XOY();		// Probably should be an error...

    // OK. Have to construct an appropriate coordinate system. The
    // Direction is the *face1's* outward normal, which may be reversed from
    // from the surface's normal.
    gp_Dir n1 = plane1_->Pln().Axis().Direction();

    if ( face1_.Orientation() == TopAbs_REVERSED ) n1.Reverse();
    return gp_Ax2( plane1_->Pln().Axis().Location(), n1,
		   plane1_->Pln().Position().XDirection() );
  }

  AssemblyConstraintStatus validate0 ( const QVector<uint>& surface0_id )
  {
    surface0_id_ = surface0_id;
    TopoDS_Shape shape0 = parent_->model()->lookupShape( surface0_id_ );

    // We already know this is a plane of some sort.
    face0_ = TopoDS::Face( shape0 );
    Handle(Geom_Surface) surface0 = BRep_Tool::Surface( face0_ );
    plane0_ = Handle(Geom_Plane)::DownCast( surface0 );
    // To the best of my knowledge, nothing can go wrong here.
    return OK;
  }

  AssemblyConstraintStatus validate1 ( const QVector<uint>& surface1_id )
  {
    Handle(Standard_Type) type = parent_->model()->lookupType(surface1_id);

    if ( type != STANDARD_TYPE( Geom_Plane ) )
      return Invalid;

    surface1_id_ = surface1_id;
    TopoDS_Shape shape1 = parent_->model()->lookupShape( surface1_id_ );

    // We already know this is a plane of some sort.
    face1_ = TopoDS::Face( shape1 );
    Handle(Geom_Surface) surface1 = BRep_Tool::Surface( face1_ );
    plane1_ = Handle(Geom_Plane)::DownCast( surface1 );

    // The subassembly is completely free, so any surface is acceptable.
    return ConstraintComplete;
  }

  void transform ( void )
  {
    gp_Dir n0 = plane0_->Pln().Axis().Direction();
    gp_Dir n1 = plane1_->Pln().Axis().Direction();

    if ( face0_.Orientation() == TopAbs_REVERSED ) n0.Reverse();
    if ( face1_.Orientation() == TopAbs_REVERSED ) n1.Reverse();

    // Twist the first subassembly around so that the normals point in
    // opposite directions.

    if ( !n0.IsOpposite( n1, Precision::Angular() ) ) {
      if ( n0.IsEqual( n1, Precision::Angular() ) ) {
	gp_Dir x_dir = plane0_->Pln().XAxis().Direction();
	parent_->subassembly()->rotate( x_dir, M_PI );
      }
      else {
	gp_Dir dir = n0 ^ n1;
	double angle = n0.Angle( n1 ) - M_PI;
	parent_->subassembly()->rotate( dir, angle );
      }

      // Need to reconstruct the transformation of the face.
      face0_ = TopoDS::Face( parent_->model()->lookupShape( surface0_id_));
      plane0_ = Handle(Geom_Plane)::DownCast( BRep_Tool::Surface( face0_ ) );
    }

    // Translate the origin of plane0 so that it lies on plane1.

    gp_Vec delta( plane0_->Pln().Axis().Location(),
		  plane1_->Pln().Axis().Location() );
    double distance = delta * n1;
    gp_Vec translation = distance * n1;

    parent_->subassembly()->translate( translation );
  }
};

struct MatePlnPln1 : public PlnPln {
  MatePlnPln1 ( const AssemblyConstraint* parent )
    : PlnPln( parent )
  {}

  MatePlnPln1 ( const QDomElement& xml_rep, const AssemblyConstraint* parent )
    : PlnPln( xml_rep, parent )
  {
    // I guess it's possible that the XML representation was not complete.
    if ( ! ( face0_.IsNull() || face1_.IsNull() ) )
      transform();
  }

  //! \return a (more or less) deep copy of this object.

  MatePlnPln1* clone ( void ) const
  {
    MatePlnPln1* new_plnpln = new MatePlnPln1( parent_ );

    new_plnpln->surface0_id_ = surface0_id_;
    new_plnpln->face0_ = face0_;
    new_plnpln->plane0_ = plane0_;
    new_plnpln->surface1_id_ = surface1_id_;
    new_plnpln->face1_ = face1_;
    new_plnpln->plane1_ = plane1_;

    return new_plnpln;
  }

  //! \return the coordinate system characteristic of this constraint.

  gp_Ax2 characteristic ( void ) const
  {
    if ( plane1_.IsNull() )
      return gp::XOY();		// Probably should be an error...

    // From this constraint, we report the remaining direction in which
    // the subassembly can be moved. That is, the subassembly is already
    // constrained in to two directions, so it can only move in the direction
    // of their cross product (so, Ax2 is overkill for this constraint).
    gp_Dir n1 = plane1_->Pln().Axis().Direction();

    if ( face1_.Orientation() == TopAbs_REVERSED ) n1.Reverse();

    gp_Dir n2 = n1.Crossed( parent_->characteristic().Direction() );

    return gp_Ax2( plane1_->Pln().Axis().Location(), n2 );
  }

  AssemblyConstraintStatus validate0 ( const QVector<uint>& surface0_id )
  {
    surface0_id_ = surface0_id;
    TopoDS_Shape shape0 = parent_->model()->lookupShape( surface0_id_ );

    // We already know this is a plane of some sort.
    face0_ = TopoDS::Face( shape0 );
    Handle(Geom_Surface) surface0 = BRep_Tool::Surface( face0_ );
    plane0_ = Handle(Geom_Plane)::DownCast( surface0 );

    if ( plane0_->Pln().Axis().Direction().
	 IsOpposite( parent_->characteristic().Direction(),
		     Precision::Angular() ) ||
	 plane0_->Pln().Axis().Direction().
	 IsEqual( parent_->characteristic().Direction(),
		  Precision::Angular() ) )
      return Invalid;

    return OK;
  }

  AssemblyConstraintStatus validate1 ( const QVector<uint>& surface1_id )
  {
    Handle(Standard_Type) type = parent_->model()->lookupType(surface1_id);

    if ( type != STANDARD_TYPE( Geom_Plane ) )
      return Invalid;

    surface1_id_ = surface1_id;
    TopoDS_Shape shape1 = parent_->model()->lookupShape( surface1_id_ );

    // We already know this is a plane of some sort.
    face1_ = TopoDS::Face( shape1 );
    Handle(Geom_Surface) surface1 = BRep_Tool::Surface( face1_ );
    plane1_ = Handle(Geom_Plane)::DownCast( surface1 );

    // In the second phase, the mate can only be made if the plane normals
    // make the same (polar) angle with the characteristic: That is, you must
    // be able to rotate the subassembly about the characteristic to make
    // the plane normals line up.

    gp_Dir n0 = plane0_->Pln().Axis().Direction();
    gp_Dir n1 = plane1_->Pln().Axis().Direction();

    if ( face0_.Orientation() == TopAbs_REVERSED ) n0.Reverse();
    if ( face1_.Orientation() == TopAbs_REVERSED ) n1.Reverse();

    if ( fabs( parent_->characteristic().Direction().Angle( n0 ) -
	       parent_->characteristic().Direction().Angle( n1 ) )
	 > Precision::Angular() )
      return Invalid;

    return ConstraintComplete;
  }

  void transform ( void )
  {
    // Twist the first subassembly around so that the normals point in
    // opposite directions.

    gp_Dir n0 = plane0_->Pln().Axis().Direction();
    gp_Dir n1 = plane1_->Pln().Axis().Direction();

    if ( face0_.Orientation() == TopAbs_REVERSED ) n0.Reverse();
    if ( face1_.Orientation() == TopAbs_REVERSED ) n1.Reverse();

    if ( !n0.IsOpposite( n1, Precision::Angular() ) ) {
      if ( n0.IsEqual( n1, Precision::Angular() ) ) {
	// Roll around on the same azimuth(?)
	gp_Dir meln = parent_->characteristic().Direction();
	parent_->subassembly()->rotate( meln, M_PI );
      }
      else {
	// If the two planes are not at least parallel, we have the
	// added convolution that the direction of rotation is the
	// characteristic. So, we have to find the azimuthal angle
	// which separates n0 and n1 with respect to the
	// characteristic.  Works like this:
	// 1. Project n0 onto the plane defined by the characteristic.
	double n0x = n0 * parent_->characteristic().XDirection();
	double n0y = n0 * parent_->characteristic().YDirection();
	gp_Vec n0p = n0x * parent_->characteristic().XDirection() +
	  n0y * parent_->characteristic().YDirection();
	// 2. Project n1 onto the plane defined by the characteristic.
	double n1x = n1 * parent_->characteristic().XDirection();
	double n1y = n1 * parent_->characteristic().YDirection();
	gp_Vec n1p = n1x * parent_->characteristic().XDirection() +
	  n1y * parent_->characteristic().YDirection();
	// 3. The angle between the projected directions is just the azimuthal
	// angle.
	double dphi = n0p.Angle( n1p );

	gp_Dir meln = n0p ^ n1p;
	parent_->subassembly()->rotate( meln, dphi - M_PI );
      }

      // Need to reconstruct the transformation of the face.
      face0_ = TopoDS::Face( parent_->model()->lookupShape( surface0_id_));
      plane0_ = Handle(Geom_Plane)::DownCast( BRep_Tool::Surface( face0_ ) );
    }

    // Translate the origin of plane0 so that it lies on plane1 with the
    // added requirement that the origin must move parallel to the
    // characteristic plane.

    // Compute a direction mostly normal to plane1 which also lies in the
    // plane of the characteristic.
    gp_Dir omega = parent_->characteristic().Direction().
      CrossCrossed( n1, parent_->characteristic().Direction() );

    // Compute the distance along omega from plane0's origin to plane1.
    double distance = ( gp_Vec( plane0_->Pln().Axis().Location(),
				plane1_->Pln().Axis().Location() ) * n1 ) /
      ( omega * n1 );

    gp_Vec translation = distance * omega;

    parent_->subassembly()->translate( translation );
  }
};

struct MatePlnPln2 : public PlnPln {
  MatePlnPln2 ( const AssemblyConstraint* parent )
    : PlnPln( parent )
  {}

  MatePlnPln2 ( const QDomElement& xml_rep, const AssemblyConstraint* parent )
    : PlnPln( xml_rep, parent )
  {
    // I guess it's possible that the XML representation was not complete.
    if ( ! ( face0_.IsNull() || face1_.IsNull() ) )
      transform();
  }

  //! \return a (more or less) deep copy of this object.

  MatePlnPln2* clone ( void ) const
  {
    MatePlnPln2* new_plnpln = new MatePlnPln2( parent_ );

    new_plnpln->surface0_id_ = surface0_id_;
    new_plnpln->face0_ = face0_;
    new_plnpln->plane0_ = plane0_;
    new_plnpln->surface1_id_ = surface1_id_;
    new_plnpln->face1_ = face1_;
    new_plnpln->plane1_ = plane1_;

    return new_plnpln;
  }

  //! \return the coordinate system characteristic of this constraint.

  gp_Ax2 characteristic ( void ) const
  {
    // Nothing comes after this so this method should never be invoked...
    return gp::XOY();
  }

  AssemblyConstraintStatus validate0 ( const QVector<uint>& surface0_id )
  {
    surface0_id_ = surface0_id;
    TopoDS_Shape shape0 = parent_->model()->lookupShape( surface0_id_ );

    // We already know this is a plane of some sort.
    face0_ = TopoDS::Face( shape0 );
    Handle(Geom_Surface) surface0 = BRep_Tool::Surface( face0_ );
    plane0_ = Handle(Geom_Plane)::DownCast( surface0 );

    // In the third phase, the selected subassembly surface must not be
    // normal to the phase two characteristic.

    if ( (plane0_->Pln().Axis().Direction().
	  IsNormal( parent_->characteristic().Direction(), Precision::Angular() ) ))
      return Invalid;

    return OK;
  }

  AssemblyConstraintStatus validate1 ( const QVector<uint>& surface1_id )
  {
    Handle(Standard_Type) type = parent_->model()->lookupType(surface1_id);

    if ( type != STANDARD_TYPE( Geom_Plane ) )
      return Invalid;

    surface1_id_ = surface1_id;
    TopoDS_Shape shape1 = parent_->model()->lookupShape( surface1_id_ );

    // We already know this is a plane of some sort.
    face1_ = TopoDS::Face( shape1 );
    Handle(Geom_Surface) surface1 = BRep_Tool::Surface( face1_ );
    plane1_ = Handle(Geom_Plane)::DownCast( surface1 );

    // Since only a translation is possible in phase three, the surfaces
    // must already have opposite normals.

    gp_Dir n0 = plane0_->Pln().Axis().Direction();
    gp_Dir n1 = plane1_->Pln().Axis().Direction();

    if ( face0_.Orientation() == TopAbs_REVERSED ) n0.Reverse();
    if ( face1_.Orientation() == TopAbs_REVERSED ) n1.Reverse();

    if ( !n0.IsOpposite( n1, Precision::Angular() ) )
      return Invalid;

    return ConstraintComplete;
  }

  void transform ( void )
  {
    gp_Dir n0 = plane0_->Pln().Axis().Direction();
    gp_Dir n1 = plane1_->Pln().Axis().Direction();

    if ( face0_.Orientation() == TopAbs_REVERSED ) n0.Reverse();
    if ( face1_.Orientation() == TopAbs_REVERSED ) n1.Reverse();

    // Only a translation in the characteristic direction is
    // possible at this point.

    gp_Vec delta( plane0_->Pln().Axis().Location(),
		  plane1_->Pln().Axis().Location() );
    double distance = ( delta * n1 ) / ( parent_->characteristic().Direction()*n1);
    gp_Vec translation = distance * parent_->characteristic().Direction();

    parent_->subassembly()->translate( translation );
  }
};

struct AlignPlnPln0 : public PlnPln {
  AlignPlnPln0 ( const AssemblyConstraint* parent )
    : PlnPln( parent )
  {}

  AlignPlnPln0 ( const QDomElement& xml_rep, const AssemblyConstraint* parent )
    : PlnPln( xml_rep, parent )
  {
    // I guess it's possible that the XML representation was not complete.
    if ( ! ( face0_.IsNull() || face1_.IsNull() ) )
      transform();
  }

  //! \return a (more or less) deep copy of this object.

  AlignPlnPln0* clone ( void ) const
  {
    AlignPlnPln0* new_plnpln = new AlignPlnPln0( parent_ );

    new_plnpln->surface0_id_ = surface0_id_;
    new_plnpln->face0_ = face0_;
    new_plnpln->plane0_ = plane0_;
    new_plnpln->surface1_id_ = surface1_id_;
    new_plnpln->face1_ = face1_;
    new_plnpln->plane1_ = plane1_;

    return new_plnpln;
  }

  //! \return the coordinate system characteristic of this constraint. For a
  //! plane, it's the normal of the SECOND plane.

  gp_Ax2 characteristic ( void ) const
  {
    if ( plane1_.IsNull() )
      return gp::XOY();
    // OK. Have to construct an appropriate coordinate system. The
    // Direction is the *face1's* outward normal, which may be reversed from
    // from the surface's normal.
    gp_Dir n1 = plane1_->Pln().Axis().Direction();

    if ( face1_.Orientation() == TopAbs_REVERSED ) n1.Reverse();

    return gp_Ax2( plane1_->Pln().Axis().Location(), n1,
		   plane1_->Pln().Position().XDirection() );
  }

  AssemblyConstraintStatus validate0 ( const QVector<uint>& surface0_id )
  {
    surface0_id_ = surface0_id;
    TopoDS_Shape shape0 = parent_->model()->lookupShape( surface0_id_ );

    // We already know this is a plane of some sort.
    face0_ = TopoDS::Face( shape0 );
    Handle(Geom_Surface) surface0 = BRep_Tool::Surface( face0_ );
    plane0_ = Handle(Geom_Plane)::DownCast( surface0 );
    // To the best of my knowledge, nothing can go wrong here.
    return OK;
  }

  AssemblyConstraintStatus validate1 ( const QVector<uint>& surface1_id )
  {
    Handle(Standard_Type) type = parent_->model()->lookupType(surface1_id);

    if ( type != STANDARD_TYPE( Geom_Plane ) )
      return Invalid;

    surface1_id_ = surface1_id;
    TopoDS_Shape shape1 = parent_->model()->lookupShape( surface1_id_ );

    // We already know this is a plane of some sort.
    face1_ = TopoDS::Face( shape1 );
    Handle(Geom_Surface) surface1 = BRep_Tool::Surface( face1_ );
    plane1_ = Handle(Geom_Plane)::DownCast( surface1 );

    // The subassembly is completely free, so any surface is acceptable.
    return ConstraintComplete;
  }

  void transform ( void )
  {
    gp_Dir n0 = plane0_->Pln().Axis().Direction();
    gp_Dir n1 = plane1_->Pln().Axis().Direction();

    if ( face0_.Orientation() == TopAbs_REVERSED ) n0.Reverse();
    if ( face1_.Orientation() == TopAbs_REVERSED ) n1.Reverse();

    // Twist the first subassembly around so that the normals point in
    // the same direction.

    if ( !n0.IsEqual( n1, Precision::Angular() ) ) {
      if ( n0.IsOpposite( n1, Precision::Angular() ) ) {
	gp_Dir x_dir = plane0_->Pln().XAxis().Direction();
	parent_->subassembly()->rotate( x_dir, M_PI );
      }
      else {
	gp_Dir dir = n0 ^ n1;
	double angle = n0.Angle( n1 );
	parent_->subassembly()->rotate( dir, angle );
      }

      // Need to reconstruct the transformation of the face.
      face0_ = TopoDS::Face( parent_->model()->lookupShape( surface0_id_));
      plane0_ = Handle(Geom_Plane)::DownCast( BRep_Tool::Surface( face0_ ) );
    }

    // Translate the origin of plane0 so that it lies on plane1. Note:

    gp_Vec delta( plane0_->Pln().Axis().Location(),
		  plane1_->Pln().Axis().Location() );
    double distance = delta * n1;
    gp_Vec translation = distance * n1;

    parent_->subassembly()->translate( translation );
  }
};

struct AlignPlnPln1 : public PlnPln {
  AlignPlnPln1 ( const AssemblyConstraint* parent )
    : PlnPln( parent )
  {}

  AlignPlnPln1 ( const QDomElement& xml_rep, const AssemblyConstraint* parent )
    : PlnPln( xml_rep, parent )
  {
    // I guess it's possible that the XML representation was not complete.
    if ( ! ( face0_.IsNull() || face1_.IsNull() ) )
      transform();
  }

  //! \return a (more or less) deep copy of this object.

  AlignPlnPln1* clone ( void ) const
  {
    AlignPlnPln1* new_plnpln = new AlignPlnPln1( parent_ );

    new_plnpln->surface0_id_ = surface0_id_;
    new_plnpln->face0_ = face0_;
    new_plnpln->plane0_ = plane0_;
    new_plnpln->surface1_id_ = surface1_id_;
    new_plnpln->face1_ = face1_;
    new_plnpln->plane1_ = plane1_;

    return new_plnpln;
  }

  //! \return the coordinate system characteristic of this constraint. For a
  //! plane, it's the normal of the SECOND plane.

  gp_Ax2 characteristic ( void ) const
  {
    if ( plane1_.IsNull() )
      return gp::XOY();		// Probably should be an error...

    // From this constraint, we report the remaining direction in which
    // the subassembly can be moved. That is, the subassembly is already
    // constrained in to two directions, so it can only move in the direction
    // of their cross product (so, Ax2 is overkill for this constraint).
    gp_Dir n1 = plane1_->Pln().Axis().Direction();

    if ( face1_.Orientation() == TopAbs_REVERSED ) n1.Reverse();

    gp_Dir n2 = n1.Crossed( parent_->characteristic().Direction() );

    return gp_Ax2( plane1_->Pln().Axis().Location(), n2 );
  }

  AssemblyConstraintStatus validate0 ( const QVector<uint>& surface0_id )
  {
    surface0_id_ = surface0_id;
    TopoDS_Shape shape0 = parent_->model()->lookupShape( surface0_id_ );

    // We already know this is a plane of some sort.
    face0_ = TopoDS::Face( shape0 );
    Handle(Geom_Surface) surface0 = BRep_Tool::Surface( face0_ );
    plane0_ = Handle(Geom_Plane)::DownCast( surface0 );

    // In phase two, the selected subassembly surface normal can point in
    // any direction except exactly in the same direction as the phase one
    // characteristic.

    if ( plane0_->Pln().Axis().Direction().
	 IsOpposite( parent_->characteristic().Direction(),
		     Precision::Angular() ) ||
	 plane0_->Pln().Axis().Direction().
	 IsEqual( parent_->characteristic().Direction(),
		  Precision::Angular() ) )
      return Invalid;

    return OK;
  }

  AssemblyConstraintStatus validate1 ( const QVector<uint>& surface1_id )
  {
    Handle(Standard_Type) type = parent_->model()->lookupType(surface1_id);

    if ( type != STANDARD_TYPE( Geom_Plane ) )
      return Invalid;

    surface1_id_ = surface1_id;
    TopoDS_Shape shape1 = parent_->model()->lookupShape( surface1_id_ );

    // We already know this is a plane of some sort.
    face1_ = TopoDS::Face( shape1 );
    Handle(Geom_Surface) surface1 = BRep_Tool::Surface( face1_ );
    plane1_ = Handle(Geom_Plane)::DownCast( surface1 );

    // In the second phase, the alignment can only be made if the plane normals
    // make the same (polar) angle with the characteristic: That is, you must
    // be able to rotate the subassembly about the characteristic to make
    // the plane normals line up.

    gp_Dir n0 = plane0_->Pln().Axis().Direction();
    gp_Dir n1 = plane1_->Pln().Axis().Direction();

    if ( face0_.Orientation() == TopAbs_REVERSED ) n0.Reverse();
    if ( face1_.Orientation() == TopAbs_REVERSED ) n1.Reverse();

    if ( fabs( parent_->characteristic().Direction().Angle( n0 ) -
	       parent_->characteristic().Direction().Angle( n1 ) )
	 > Precision::Angular() )
      return Invalid;

    return ConstraintComplete;
  }

  void transform ( void )
  {
    // Twist the first subassembly around so that the normals point in
    // the same direction.

    gp_Dir n0 = plane0_->Pln().Axis().Direction();
    gp_Dir n1 = plane1_->Pln().Axis().Direction();

    if ( face0_.Orientation() == TopAbs_REVERSED ) n0.Reverse();
    if ( face1_.Orientation() == TopAbs_REVERSED ) n1.Reverse();

    if ( !n0.IsEqual( n1, Precision::Angular() ) ) {
      if ( n0.IsOpposite( n1, Precision::Angular() ) ) {
	// Roll around on the same azimuth(?)
	gp_Dir meln = parent_->characteristic().Direction();
	parent_->subassembly()->rotate( meln, M_PI );
      }
      else {
	// If the normals are not already parallel, this has the added
	// convolution that the direction of rotation is the
	// characteristic. So, we have to find the azimuthal angle
	// which separates n0 and n1 with respect to the
	// characteristic.  Works like this:
	// 1. Project n0 onto the plane defined by the characteristic.
	double n0x = n0 * parent_->characteristic().XDirection();
	double n0y = n0 * parent_->characteristic().YDirection();
	gp_Vec n0p = n0x * parent_->characteristic().XDirection() +
	  n0y * parent_->characteristic().YDirection();
	// 2. Project n1 onto the plane defined by the characteristic.
	double n1x = n1 * parent_->characteristic().XDirection();
	double n1y = n1 * parent_->characteristic().YDirection();
	gp_Vec n1p = n1x * parent_->characteristic().XDirection() +
	  n1y * parent_->characteristic().YDirection();
	// 3. The angle between the projected directions is just the
	// azimuthal angle.
	double dphi = n0p.Angle( n1p );
	gp_Dir meln = n0p ^ n1p;
	parent_->subassembly()->rotate( meln, dphi );
      }

      // Need to reconstruct the transformation of the face.
      face0_ = TopoDS::Face( parent_->model()->lookupShape( surface0_id_));
      plane0_ = Handle(Geom_Plane)::DownCast( BRep_Tool::Surface( face0_ ) );
    }

    // Translate the origin of plane0 so that it lies on plane1 with the
    // added requirement that the origin must move parallel to the
    // characteristic plane.

    // Compute a direction mostly normal to plane1 which also lies in the
    // plane of the characteristic.
    gp_Dir omega = parent_->characteristic().Direction().
      CrossCrossed( n1, parent_->characteristic().Direction() );

    // Compute the distance along omega from plane0's origin to plane1.
    double distance = ( gp_Vec( plane0_->Pln().Axis().Location(),
				plane1_->Pln().Axis().Location() ) * n1 ) /
      ( omega * n1 );

    gp_Vec translation = distance * omega;

    parent_->subassembly()->translate( translation );
  }
};

struct AlignPlnPln2 : public PlnPln {
  AlignPlnPln2 ( const AssemblyConstraint* parent )
    : PlnPln( parent )
  {}

  AlignPlnPln2 ( const QDomElement& xml_rep, const AssemblyConstraint* parent )
    : PlnPln( xml_rep, parent )
  {
    // I guess it's possible that the XML representation was not complete.
    if ( ! ( face0_.IsNull() || face1_.IsNull() ) )
      transform();
  }

  //! \return a (more or less) deep copy of this object.

  AlignPlnPln2* clone ( void ) const
  {
    AlignPlnPln2* new_plnpln = new AlignPlnPln2( parent_ );

    new_plnpln->surface0_id_ = surface0_id_;
    new_plnpln->face0_ = face0_;
    new_plnpln->plane0_ = plane0_;
    new_plnpln->surface1_id_ = surface1_id_;
    new_plnpln->face1_ = face1_;
    new_plnpln->plane1_ = plane1_;

    return new_plnpln;
  }

  //! \return the coordinate system characteristic of this constraint. For a
  //! plane, it's the normal of the SECOND plane.

  gp_Ax2 characteristic ( void ) const
  {
    // Nothing comes after this so this method should never be invoked...
    return gp::XOY();
  }

  AssemblyConstraintStatus validate0 ( const QVector<uint>& surface0_id )
  {
    surface0_id_ = surface0_id;
    TopoDS_Shape shape0 = parent_->model()->lookupShape( surface0_id_ );

    // We already know this is a plane of some sort.
    face0_ = TopoDS::Face( shape0 );
    Handle(Geom_Surface) surface0 = BRep_Tool::Surface( face0_ );
    plane0_ = Handle(Geom_Plane)::DownCast( surface0 );

    // In the third phase, the selected subassembly surface has to already be
    // non-normal to the phase two characteristic.

    if ( (plane0_->Pln().Axis().Direction().
	  IsNormal( parent_->characteristic().Direction(),
		    Precision::Angular() ) ) )
      return Invalid;

    return OK;
  }

  AssemblyConstraintStatus validate1 ( const QVector<uint>& surface1_id )
  {
    Handle(Standard_Type) type = parent_->model()->lookupType(surface1_id);

    if ( type != STANDARD_TYPE( Geom_Plane ) )
      return Invalid;

    surface1_id_ = surface1_id;
    TopoDS_Shape shape1 = parent_->model()->lookupShape( surface1_id_ );

    // We already know this is a plane of some sort.
    face1_ = TopoDS::Face( shape1 );
    Handle(Geom_Surface) surface1 = BRep_Tool::Surface( face1_ );
    plane1_ = Handle(Geom_Plane)::DownCast( surface1 );

    // Since only a translation is possible in phase three, the surfaces
    // must already have parallel normals.

    gp_Dir n0 = plane0_->Pln().Axis().Direction();
    gp_Dir n1 = plane1_->Pln().Axis().Direction();

    if ( face0_.Orientation() == TopAbs_REVERSED ) n0.Reverse();
    if ( face1_.Orientation() == TopAbs_REVERSED ) n1.Reverse();

    if ( !n0.IsEqual( n1, Precision::Angular() ) )
      return Invalid;

    return ConstraintComplete;
  }

  void transform ( void )
  {
    // Only a translation in the characteristic direction is
    // possible at this point.

    gp_Dir n0 = plane0_->Pln().Axis().Direction();
    gp_Dir n1 = plane1_->Pln().Axis().Direction();

    if ( face0_.Orientation() == TopAbs_REVERSED ) n0.Reverse();
    if ( face1_.Orientation() == TopAbs_REVERSED ) n1.Reverse();

    gp_Vec delta( plane0_->Pln().Axis().Location(),
		  plane1_->Pln().Axis().Location() );
    double distance = ( delta * n1 ) / ( parent_->characteristic().Direction() *n1);
    gp_Vec translation = distance * parent_->characteristic().Direction();

    parent_->subassembly()->translate( translation );
  }
};

class MateOffsetPlnPln0 : public PlnPln {
public:
  MateOffsetPlnPln0 ( const AssemblyConstraint* parent )
    : PlnPln( parent ), offset_( 0 )
  {}

  MateOffsetPlnPln0 ( const QDomElement& xml_rep, const AssemblyConstraint* parent )
    : PlnPln( xml_rep, parent ), offset_( 0 )
  {
    offset_ = xml_rep.attribute( lC::STR::OFFSET ).toDouble();

    // I guess it's possible that the XML representation was not complete.
    if ( ! ( face0_.IsNull() || face1_.IsNull() ) )
      transform();
  }

  //! \return a (more or less) deep copy of this object.

  MateOffsetPlnPln0* clone ( void ) const
  {
    MateOffsetPlnPln0* new_plnpln = new MateOffsetPlnPln0( parent_ );

    new_plnpln->surface0_id_ = surface0_id_;
    new_plnpln->face0_ = face0_;
    new_plnpln->plane0_ = plane0_;
    new_plnpln->surface1_id_ = surface1_id_;
    new_plnpln->face1_ = face1_;
    new_plnpln->plane1_ = plane1_;
    new_plnpln->offset_ = offset_;

    return new_plnpln;
  }

  //! \return the coordinate system characteristic of this constraint.

  gp_Ax2 characteristic ( void ) const
  {
    if ( plane1_.IsNull() )
      return gp::XOY();		// Probably should be an error...

    // OK. Have to construct an appropriate coordinate system. The
    // Direction is the *face1's* outward normal, which may be reversed from
    // from the surface's normal.
    gp_Dir n1 = plane1_->Pln().Axis().Direction();

    if ( face1_.Orientation() == TopAbs_REVERSED ) n1.Reverse();
    return gp_Ax2( plane1_->Pln().Axis().Location(), n1,
		   plane1_->Pln().Position().XDirection() );
  }

  //! \return the offset.
  double offset ( void ) const { return offset_; }
  /*!
   * Save the offset but don't act on it yet.
   * \param offset the new offset.
   */
  void setOffset ( double offset )
  {
    offset_ = offset;
  }
  /*!
   * Change the offset and recompute the subassembly transformation.
   * \param offset the new offset.
   */
  void updateOffset ( double offset )
  {
    offset_ = offset;

    transform();
  }

  AssemblyConstraintStatus validate0 ( const QVector<uint>& surface0_id )
  {
    surface0_id_ = surface0_id;
    TopoDS_Shape shape0 = parent_->model()->lookupShape( surface0_id_ );

    // We already know this is a plane of some sort.
    face0_ = TopoDS::Face( shape0 );
    Handle(Geom_Surface) surface0 = BRep_Tool::Surface( face0_ );
    plane0_ = Handle(Geom_Plane)::DownCast( surface0 );
    // To the best of my knowledge, nothing can go wrong here.
    return OK;
  }

  AssemblyConstraintStatus validate1 ( const QVector<uint>& surface1_id )
  {
    Handle(Standard_Type) type = parent_->model()->lookupType(surface1_id);

    if ( type != STANDARD_TYPE( Geom_Plane ) )
      return Invalid;

    surface1_id_ = surface1_id;
    TopoDS_Shape shape1 = parent_->model()->lookupShape( surface1_id_ );

    // We already know this is a plane of some sort.
    face1_ = TopoDS::Face( shape1 );
    Handle(Geom_Surface) surface1 = BRep_Tool::Surface( face1_ );
    plane1_ = Handle(Geom_Plane)::DownCast( surface1 );

    // The subassembly is completely free, so any surface is acceptable.
    return ConstraintComplete;
  }

  void transform ( void )
  {
    gp_Dir n0 = plane0_->Pln().Axis().Direction();
    gp_Dir n1 = plane1_->Pln().Axis().Direction();

    if ( face0_.Orientation() == TopAbs_REVERSED ) n0.Reverse();
    if ( face1_.Orientation() == TopAbs_REVERSED ) n1.Reverse();

    // Twist the first subassembly around so that the normals point in
    // opposite directions.

    if ( !n0.IsOpposite( n1, Precision::Angular() ) ) {
      if ( n0.IsEqual( n1, Precision::Angular() ) ) {
	gp_Dir x_dir = plane0_->Pln().XAxis().Direction();
	parent_->subassembly()->rotate( x_dir, M_PI );
      }
      else {
	gp_Dir dir = n0 ^ n1;
	double angle = n0.Angle( n1 ) - M_PI;
	parent_->subassembly()->rotate( dir, angle );
      }

      // Need to reconstruct the transformation of the face.
      face0_ = TopoDS::Face( parent_->model()->lookupShape( surface0_id_));
      plane0_ = Handle(Geom_Plane)::DownCast( BRep_Tool::Surface( face0_ ) );
    }

    // Translate the origin of plane0 so that it offset "offset_" distance
    // from plane1.

    gp_Vec delta( plane0_->Pln().Axis().Location(),
		  plane1_->Pln().Axis().Location() );
    double distance = delta * n1 + offset_;
    gp_Vec translation = distance * n1;

    parent_->subassembly()->translate( translation );

    // Need to reconstruct the transformation of the face so that the
    // plane geometry is current.
    face0_ = TopoDS::Face( parent_->model()->lookupShape( surface0_id_));
    plane0_ = Handle(Geom_Plane)::DownCast( BRep_Tool::Surface( face0_ ) );

    updateDimension();
  }

  /*!
   * Append the offset to the XML representation as an attribute.
   * \param xml_rep XML representation
   */
  void write ( QDomElement& xml_rep ) const
  {
    PlnPln::write( xml_rep );
    xml_rep.setAttribute( lC::STR::OFFSET, lC::format( offset_ ) );
  }
private:
  double offset_;
};

class MateOffsetPlnPln1 : public PlnPln {
public:
  MateOffsetPlnPln1 ( const AssemblyConstraint* parent )
    : PlnPln( parent ), offset_( 0 )
  {}

  MateOffsetPlnPln1 ( const QDomElement& xml_rep, const AssemblyConstraint* parent )
    : PlnPln( xml_rep, parent ), offset_( 0 )
  {
    offset_ = xml_rep.attribute( lC::STR::OFFSET ).toDouble();

    // I guess it's possible that the XML representation was not complete.
    if ( ! ( face0_.IsNull() || face1_.IsNull() ) )
      transform();
  }

  //! \return a (more or less) deep copy of this object.

  MateOffsetPlnPln1* clone ( void ) const
  {
    MateOffsetPlnPln1* new_plnpln = new MateOffsetPlnPln1( parent_ );

    new_plnpln->surface0_id_ = surface0_id_;
    new_plnpln->face0_ = face0_;
    new_plnpln->plane0_ = plane0_;
    new_plnpln->surface1_id_ = surface1_id_;
    new_plnpln->face1_ = face1_;
    new_plnpln->plane1_ = plane1_;
    new_plnpln->offset_ = offset_;

    return new_plnpln;
  }

  //! \return the offset.
  double offset ( void ) const { return offset_; }
  /*!
   * Apply the offset.
   * \param offset the new offset.
   */
  void setOffset ( double offset )
  {
    offset_ = offset;
  }
  /*!
   * Change the offset and recompute the subassembly transformation.
   * \param offset the new offset.
   */
  void updateOffset ( double offset )
  {
    offset_ = offset;

    transform();
  }

  //! \return the coordinate system characteristic of this constraint.

  gp_Ax2 characteristic ( void ) const
  {
    if ( plane1_.IsNull() )
      return gp::XOY();		// Probably should be an error...

    // From this constraint, we report the remaining direction in which
    // the subassembly can be moved. That is, the subassembly is already
    // constrained in to two directions, so it can only move in the direction
    // of their cross product (so, Ax2 is overkill for this constraint).
    gp_Dir n1 = plane1_->Pln().Axis().Direction();

    if ( face1_.Orientation() == TopAbs_REVERSED ) n1.Reverse();

    gp_Dir n2 = n1.Crossed( parent_->characteristic().Direction() );

    return gp_Ax2( plane1_->Pln().Axis().Location(), n2 );
  }

  AssemblyConstraintStatus validate0 ( const QVector<uint>& surface0_id )
  {
    surface0_id_ = surface0_id;
    TopoDS_Shape shape0 = parent_->model()->lookupShape( surface0_id_ );

    // We already know this is a plane of some sort.
    face0_ = TopoDS::Face( shape0 );
    Handle(Geom_Surface) surface0 = BRep_Tool::Surface( face0_ );
    plane0_ = Handle(Geom_Plane)::DownCast( surface0 );

    if ( plane0_->Pln().Axis().Direction().
	 IsOpposite( parent_->characteristic().Direction(),
		     Precision::Angular() ) ||
	 plane0_->Pln().Axis().Direction().
	 IsEqual( parent_->characteristic().Direction(),
		  Precision::Angular() ) )
      return Invalid;

    return OK;
  }

  AssemblyConstraintStatus validate1 ( const QVector<uint>& surface1_id )
  {
    Handle(Standard_Type) type = parent_->model()->lookupType(surface1_id);

    if ( type != STANDARD_TYPE( Geom_Plane ) )
      return Invalid;

    surface1_id_ = surface1_id;
    TopoDS_Shape shape1 = parent_->model()->lookupShape( surface1_id_ );

    // We already know this is a plane of some sort.
    face1_ = TopoDS::Face( shape1 );
    Handle(Geom_Surface) surface1 = BRep_Tool::Surface( face1_ );
    plane1_ = Handle(Geom_Plane)::DownCast( surface1 );

    // In the second phase, the mate can only be made if the plane normals
    // make the same (polar) angle with the characteristic: That is, you must
    // be able to rotate the subassembly about the characteristic to make
    // the plane normals line up.

    gp_Dir n0 = plane0_->Pln().Axis().Direction();
    gp_Dir n1 = plane1_->Pln().Axis().Direction();

    if ( face0_.Orientation() == TopAbs_REVERSED ) n0.Reverse();
    if ( face1_.Orientation() == TopAbs_REVERSED ) n1.Reverse();

    if ( fabs( parent_->characteristic().Direction().Angle( n0 ) -
	       parent_->characteristic().Direction().Angle( n1 ) )
	 > Precision::Angular() )
      return Invalid;

    return ConstraintComplete;
  }

  void transform ( void )
  {
    // Twist the first subassembly around so that the normals point in
    // opposite directions.

    gp_Dir n0 = plane0_->Pln().Axis().Direction();
    gp_Dir n1 = plane1_->Pln().Axis().Direction();

    if ( face0_.Orientation() == TopAbs_REVERSED ) n0.Reverse();
    if ( face1_.Orientation() == TopAbs_REVERSED ) n1.Reverse();

    if ( !n0.IsOpposite( n1, Precision::Angular() ) ) {
      if ( n0.IsEqual( n1, Precision::Angular() ) ) {
	// Roll around on the same azimuth(?)
	gp_Dir meln = parent_->characteristic().Direction();
	parent_->subassembly()->rotate( meln, M_PI );
      }
      else {
	// If the two planes are not at least parallel, we have the
	// added convolution that the direction of rotation is the
	// characteristic. So, we have to find the azimuthal angle
	// which separates n0 and n1 with respect to the
	// characteristic.  Works like this:
	// 1. Project n0 onto the plane defined by the characteristic.
	double n0x = n0 * parent_->characteristic().XDirection();
	double n0y = n0 * parent_->characteristic().YDirection();
	gp_Vec n0p = n0x * parent_->characteristic().XDirection() +
	  n0y * parent_->characteristic().YDirection();
	// 2. Project n1 onto the plane defined by the characteristic.
	double n1x = n1 * parent_->characteristic().XDirection();
	double n1y = n1 * parent_->characteristic().YDirection();
	gp_Vec n1p = n1x * parent_->characteristic().XDirection() +
	  n1y * parent_->characteristic().YDirection();
	// 3. The angle between the projected directions is just the azimuthal
	// angle.
	double dphi = n0p.Angle( n1p );

	gp_Dir meln = n0p ^ n1p;
	parent_->subassembly()->rotate( meln, dphi - M_PI );
      }

      // Need to reconstruct the transformation of the face.
      face0_ = TopoDS::Face( parent_->model()->lookupShape( surface0_id_));
      plane0_ = Handle(Geom_Plane)::DownCast( BRep_Tool::Surface( face0_ ) );
    }

    // Translate the origin of plane0 so that it lies offset_ distance
    // from plane1 with the added requirement that the origin must
    // move parallel to the characteristic plane.

    // Compute a direction mostly normal to plane1 which also lies in the
    // plane of the characteristic.
    gp_Dir omega = parent_->characteristic().Direction().
      CrossCrossed( n1, parent_->characteristic().Direction() );

    // Compute the distance along omega from plane0's origin to plane1.
    double distance = ( gp_Vec( plane0_->Pln().Axis().Location(),
				plane1_->Pln().Axis().Location() ) * n1 ) /
      ( omega * n1 ) + offset_;

    gp_Vec translation = distance * omega;

    parent_->subassembly()->translate( translation );

    // Need to reconstruct the transformation of the face so that the
    // plane geometry is current.
    face0_ = TopoDS::Face( parent_->model()->lookupShape( surface0_id_));
    plane0_ = Handle(Geom_Plane)::DownCast( BRep_Tool::Surface( face0_ ) );

    updateDimension();
  }

  /*!
   * Append the offset to the XML representation as an attribute.
   * \param xml_rep XML representation
   */
  void write ( QDomElement& xml_rep ) const
  {
    PlnPln::write( xml_rep );
    xml_rep.setAttribute( lC::STR::OFFSET, lC::format( offset_ ) );
  }
private:
  double offset_;
};

class MateOffsetPlnPln2 : public PlnPln {
public:
  MateOffsetPlnPln2 ( const AssemblyConstraint* parent )
    : PlnPln( parent ), offset_( 0 )
  {}

  MateOffsetPlnPln2 ( const QDomElement& xml_rep, const AssemblyConstraint* parent )
    : PlnPln( xml_rep, parent ), offset_( 0 )
  {
    offset_ = xml_rep.attribute( lC::STR::OFFSET ).toDouble();

    // I guess it's possible that the XML representation was not complete.
    if ( ! ( face0_.IsNull() || face1_.IsNull() ) )
      transform();
  }

  //! \return a (more or less) deep copy of this object.

  MateOffsetPlnPln2* clone ( void ) const
  {
    MateOffsetPlnPln2* new_plnpln = new MateOffsetPlnPln2( parent_ );

    new_plnpln->surface0_id_ = surface0_id_;
    new_plnpln->face0_ = face0_;
    new_plnpln->plane0_ = plane0_;
    new_plnpln->surface1_id_ = surface1_id_;
    new_plnpln->face1_ = face1_;
    new_plnpln->plane1_ = plane1_;
    new_plnpln->offset_ = offset_;

    return new_plnpln;
  }

  //! \return the offset.
  double offset ( void ) const { return offset_; }
  /*!
   * Apply the offset.
   * \param offset the new offset.
   */
  void setOffset ( double offset )
  {
    offset_ = offset;
  }
  /*!
   * Change the offset and recompute the subassembly transformation.
   * \param offset the new offset.
   */
  void updateOffset ( double offset )
  {
    offset_ = offset;

    transform();
  }

  //! \return the coordinate system characteristic of this constraint.

  gp_Ax2 characteristic ( void ) const
  {
    // Nothing comes after this so this method should never be invoked...
    return gp::XOY();
  }

  AssemblyConstraintStatus validate0 ( const QVector<uint>& surface0_id )
  {
    surface0_id_ = surface0_id;
    TopoDS_Shape shape0 = parent_->model()->lookupShape( surface0_id_ );

    // We already know this is a plane of some sort.
    face0_ = TopoDS::Face( shape0 );
    Handle(Geom_Surface) surface0 = BRep_Tool::Surface( face0_ );
    plane0_ = Handle(Geom_Plane)::DownCast( surface0 );

    // In the third phase, the selected subassembly surface must not be
    // normal to the phase two characteristic.

    if ( (plane0_->Pln().Axis().Direction().
	  IsNormal( parent_->characteristic().Direction(), Precision::Angular() ) ))
      return Invalid;

    return OK;
  }

  AssemblyConstraintStatus validate1 ( const QVector<uint>& surface1_id )
  {
    Handle(Standard_Type) type = parent_->model()->lookupType(surface1_id);

    if ( type != STANDARD_TYPE( Geom_Plane ) )
      return Invalid;

    surface1_id_ = surface1_id;
    TopoDS_Shape shape1 = parent_->model()->lookupShape( surface1_id_ );

    // We already know this is a plane of some sort.
    face1_ = TopoDS::Face( shape1 );
    Handle(Geom_Surface) surface1 = BRep_Tool::Surface( face1_ );
    plane1_ = Handle(Geom_Plane)::DownCast( surface1 );

    // Since only a translation is possible in phase three, the surfaces
    // must already have opposite normals.

    gp_Dir n0 = plane0_->Pln().Axis().Direction();
    gp_Dir n1 = plane1_->Pln().Axis().Direction();

    if ( face0_.Orientation() == TopAbs_REVERSED ) n0.Reverse();
    if ( face1_.Orientation() == TopAbs_REVERSED ) n1.Reverse();

    if ( !n0.IsOpposite( n1, Precision::Angular() ) )
      return Invalid;

    return ConstraintComplete;
  }

  void transform ( void )
  {
    gp_Dir n0 = plane0_->Pln().Axis().Direction();
    gp_Dir n1 = plane1_->Pln().Axis().Direction();

    if ( face0_.Orientation() == TopAbs_REVERSED ) n0.Reverse();
    if ( face1_.Orientation() == TopAbs_REVERSED ) n1.Reverse();

    // Only a translation in the characteristic direction is
    // possible at this point. Include the effect of the offset, too.

    gp_Vec delta( plane0_->Pln().Axis().Location(),
		  plane1_->Pln().Axis().Location() );

    double distance = ( delta * n1 + offset_ ) /
      ( parent_->characteristic().Direction() * n1 );
    gp_Vec translation = distance * parent_->characteristic().Direction();

    parent_->subassembly()->translate( translation );

    // Need to reconstruct the transformation of the face so that the
    // plane geometry is current.
    face0_ = TopoDS::Face( parent_->model()->lookupShape( surface0_id_));
    plane0_ = Handle(Geom_Plane)::DownCast( BRep_Tool::Surface( face0_ ) );

    updateDimension();
  }

  /*!
   * Append the offset to the XML representation as an attribute.
   * \param xml_rep XML representation
   */
  void write ( QDomElement& xml_rep ) const
  {
    PlnPln::write( xml_rep );
    xml_rep.setAttribute( lC::STR::OFFSET, lC::format( offset_ ) );
  }
private:
  double offset_;
};

class AlignOffsetPlnPln0 : public PlnPln {
public:
  AlignOffsetPlnPln0 ( const AssemblyConstraint* parent )
    : PlnPln( parent ), offset_( 0 )
  {}

  AlignOffsetPlnPln0 ( const QDomElement& xml_rep, const AssemblyConstraint* parent)
    : PlnPln( xml_rep, parent ), offset_( 0 )
  {
    offset_ = xml_rep.attribute( lC::STR::OFFSET ).toDouble();

    // I guess it's possible that the XML representation was not complete.
    if ( ! ( face0_.IsNull() || face1_.IsNull() ) )
      transform();
  }

  //! \return a (more or less) deep copy of this object.

  AlignOffsetPlnPln0* clone ( void ) const
  {
    AlignOffsetPlnPln0* new_plnpln = new AlignOffsetPlnPln0( parent_ );

    new_plnpln->surface0_id_ = surface0_id_;
    new_plnpln->face0_ = face0_;
    new_plnpln->plane0_ = plane0_;
    new_plnpln->surface1_id_ = surface1_id_;
    new_plnpln->face1_ = face1_;
    new_plnpln->plane1_ = plane1_;
    new_plnpln->offset_ = offset_;

    return new_plnpln;
  }

  //! \return the offset.
  double offset ( void ) const { return offset_; }
  /*!
   * Apply the offset.
   * \param offset the new offset.
   */
  void setOffset ( double offset )
  {
    offset_ = offset;
  }

  /*!
   * Change the offset and recompute the subassembly transformation.
   * \param offset the new offset.
   */
  void updateOffset ( double offset )
  {
    offset_ = offset;

    transform();
  }

  //! \return the coordinate system characteristic of this constraint. For a
  //! plane, it's the normal of the SECOND reference plane.

  gp_Ax2 characteristic ( void ) const
  {
    if ( plane1_.IsNull() )
      return gp::XOY();
    // OK. Have to construct an appropriate coordinate system. The
    // Direction is the *face1's* outward normal, which may be reversed from
    // from the surface's normal.
    gp_Dir n1 = plane1_->Pln().Axis().Direction();

    if ( face1_.Orientation() == TopAbs_REVERSED ) n1.Reverse();

    return gp_Ax2( plane1_->Pln().Axis().Location(), n1,
		   plane1_->Pln().Position().XDirection() );
  }

  AssemblyConstraintStatus validate0 ( const QVector<uint>& surface0_id )
  {
    surface0_id_ = surface0_id;
    TopoDS_Shape shape0 = parent_->model()->lookupShape( surface0_id_ );

    // We already know this is a plane of some sort.
    face0_ = TopoDS::Face( shape0 );
    Handle(Geom_Surface) surface0 = BRep_Tool::Surface( face0_ );
    plane0_ = Handle(Geom_Plane)::DownCast( surface0 );
    // To the best of my knowledge, nothing can go wrong here.
    return OK;
  }

  AssemblyConstraintStatus validate1 ( const QVector<uint>& surface1_id )
  {
    Handle(Standard_Type) type = parent_->model()->lookupType(surface1_id);

    if ( type != STANDARD_TYPE( Geom_Plane ) )
      return Invalid;

    surface1_id_ = surface1_id;
    TopoDS_Shape shape1 = parent_->model()->lookupShape( surface1_id_ );

    // We already know this is a plane of some sort.
    face1_ = TopoDS::Face( shape1 );
    Handle(Geom_Surface) surface1 = BRep_Tool::Surface( face1_ );
    plane1_ = Handle(Geom_Plane)::DownCast( surface1 );

    // The subassembly is completely free, so any surface is acceptable.
    return ConstraintComplete;
  }

  void transform ( void )
  {
    gp_Dir n0 = plane0_->Pln().Axis().Direction();
    gp_Dir n1 = plane1_->Pln().Axis().Direction();

    if ( face0_.Orientation() == TopAbs_REVERSED ) n0.Reverse();
    if ( face1_.Orientation() == TopAbs_REVERSED ) n1.Reverse();

    // Twist the first subassembly around so that the normals point in
    // the same direction.

    if ( !n0.IsEqual( n1, Precision::Angular() ) ) {
      if ( n0.IsOpposite( n1, Precision::Angular() ) ) {
	gp_Dir x_dir = plane0_->Pln().XAxis().Direction();
	parent_->subassembly()->rotate( x_dir, M_PI );
      }
      else {
	gp_Dir dir = n0 ^ n1;
	double angle = n0.Angle( n1 );
	parent_->subassembly()->rotate( dir, angle );
      }

      // Need to reconstruct the transformation of the face.
      face0_ = TopoDS::Face( parent_->model()->lookupShape( surface0_id_));
      plane0_ = Handle(Geom_Plane)::DownCast( BRep_Tool::Surface( face0_ ) );
    }

    // Translate the origin of plane0 so that it lies on plane1. Note:

    gp_Vec delta( plane0_->Pln().Axis().Location(),
		  plane1_->Pln().Axis().Location() );
    double distance = delta * n1 + offset_;
    gp_Vec translation = distance * n1;

    parent_->subassembly()->translate( translation );

    // Need to reconstruct the transformation of the face so that the
    // plane geometry is current.
    face0_ = TopoDS::Face( parent_->model()->lookupShape( surface0_id_));
    plane0_ = Handle(Geom_Plane)::DownCast( BRep_Tool::Surface( face0_ ) );

    updateDimension();
  }

  /*!
   * Append the offset to the XML representation as an attribute.
   * \param xml_rep XML representation
   */
  void write ( QDomElement& xml_rep ) const
  {
    PlnPln::write( xml_rep );
    xml_rep.setAttribute( lC::STR::OFFSET, lC::format( offset_ ) );
  }
private:
  double offset_;
};

class AlignOffsetPlnPln1 : public PlnPln {
public:
  AlignOffsetPlnPln1 ( const AssemblyConstraint* parent )
    : PlnPln( parent ), offset_( 0 )
  {}

  AlignOffsetPlnPln1 ( const QDomElement& xml_rep, const AssemblyConstraint* parent)
    : PlnPln( xml_rep, parent ), offset_( 0 )
  {
    offset_ = xml_rep.attribute( lC::STR::OFFSET ).toDouble();

    // I guess it's possible that the XML representation was not complete.
    if ( ! ( face0_.IsNull() || face1_.IsNull() ) )
      transform();
  }

  //! \return a (more or less) deep copy of this object.

  AlignOffsetPlnPln1* clone ( void ) const
  {
    AlignOffsetPlnPln1* new_plnpln = new AlignOffsetPlnPln1( parent_ );

    new_plnpln->surface0_id_ = surface0_id_;
    new_plnpln->face0_ = face0_;
    new_plnpln->plane0_ = plane0_;
    new_plnpln->surface1_id_ = surface1_id_;
    new_plnpln->face1_ = face1_;
    new_plnpln->plane1_ = plane1_;
    new_plnpln->offset_ = offset_;

    return new_plnpln;
  }

  //! \return the offset.
  double offset ( void ) const { return offset_; }
  /*!
   * Apply the offset.
   * \param offset the new offset.
   */
  void setOffset ( double offset )
  {
    offset_ = offset;
  }

  /*!
   * Change the offset and recompute the subassembly transformation.
   * \param offset the new offset.
   */
  void updateOffset ( double offset )
  {
    offset_ = offset;

    transform();
  }

  //! \return the coordinate system characteristic of this constraint. For a
  //! plane, it's the normal of the SECOND plane.

  gp_Ax2 characteristic ( void ) const
  {
    if ( plane1_.IsNull() )
      return gp::XOY();		// Probably should be an error...

    // From this constraint, we report the remaining direction in which
    // the subassembly can be moved. That is, the subassembly is already
    // constrained in to two directions, so it can only move in the direction
    // of their cross product (so, Ax2 is overkill for this constraint).
    gp_Dir n1 = plane1_->Pln().Axis().Direction();

    if ( face1_.Orientation() == TopAbs_REVERSED ) n1.Reverse();

    gp_Dir n2 = n1.Crossed( parent_->characteristic().Direction() );

    return gp_Ax2( plane1_->Pln().Axis().Location(), n2 );
  }

  AssemblyConstraintStatus validate0 ( const QVector<uint>& surface0_id )
  {
    surface0_id_ = surface0_id;
    TopoDS_Shape shape0 = parent_->model()->lookupShape( surface0_id_ );

    // We already know this is a plane of some sort.
    face0_ = TopoDS::Face( shape0 );
    Handle(Geom_Surface) surface0 = BRep_Tool::Surface( face0_ );
    plane0_ = Handle(Geom_Plane)::DownCast( surface0 );

    // In phase two, the selected subassembly surface normal can point in
    // any direction except exactly in the same direction as the phase one
    // characteristic.

    if ( plane0_->Pln().Axis().Direction().
	 IsOpposite( parent_->characteristic().Direction(),
		     Precision::Angular() ) ||
	 plane0_->Pln().Axis().Direction().
	 IsEqual( parent_->characteristic().Direction(),
		  Precision::Angular() ) )
      return Invalid;

    return OK;
  }

  AssemblyConstraintStatus validate1 ( const QVector<uint>& surface1_id )
  {
    Handle(Standard_Type) type = parent_->model()->lookupType(surface1_id);

    if ( type != STANDARD_TYPE( Geom_Plane ) )
      return Invalid;

    surface1_id_ = surface1_id;
    TopoDS_Shape shape1 = parent_->model()->lookupShape( surface1_id_ );

    // We already know this is a plane of some sort.
    face1_ = TopoDS::Face( shape1 );
    Handle(Geom_Surface) surface1 = BRep_Tool::Surface( face1_ );
    plane1_ = Handle(Geom_Plane)::DownCast( surface1 );

    // In the second phase, the alignment can only be made if the plane normals
    // make the same (polar) angle with the characteristic: That is, you must
    // be able to rotate the subassembly about the characteristic to make
    // the plane normals line up.

    gp_Dir n0 = plane0_->Pln().Axis().Direction();
    gp_Dir n1 = plane1_->Pln().Axis().Direction();

    if ( face0_.Orientation() == TopAbs_REVERSED ) n0.Reverse();
    if ( face1_.Orientation() == TopAbs_REVERSED ) n1.Reverse();

    if ( fabs( parent_->characteristic().Direction().Angle( n0 ) -
	       parent_->characteristic().Direction().Angle( n1 ) )
	 > Precision::Angular() )
      return Invalid;

    return ConstraintComplete;
  }

  void transform ( void )
  {
    // Twist the first subassembly around so that the normals point in
    // the same direction.

    gp_Dir n0 = plane0_->Pln().Axis().Direction();
    gp_Dir n1 = plane1_->Pln().Axis().Direction();

    if ( face0_.Orientation() == TopAbs_REVERSED ) n0.Reverse();
    if ( face1_.Orientation() == TopAbs_REVERSED ) n1.Reverse();

    if ( !n0.IsEqual( n1, Precision::Angular() ) ) {
      if ( n0.IsOpposite( n1, Precision::Angular() ) ) {
	// Roll around on the same azimuth(?)
	gp_Dir meln = parent_->characteristic().Direction();
	parent_->subassembly()->rotate( meln, M_PI );
      }
      else {
	// If the normals are not already parallel, this has the added
	// convolution that the direction of rotation is the
	// characteristic. So, we have to find the azimuthal angle
	// which separates n0 and n1 with respect to the
	// characteristic.  Works like this:
	// 1. Project n0 onto the plane defined by the characteristic.
	double n0x = n0 * parent_->characteristic().XDirection();
	double n0y = n0 * parent_->characteristic().YDirection();
	gp_Vec n0p = n0x * parent_->characteristic().XDirection() +
	  n0y * parent_->characteristic().YDirection();
	// 2. Project n1 onto the plane defined by the characteristic.
	double n1x = n1 * parent_->characteristic().XDirection();
	double n1y = n1 * parent_->characteristic().YDirection();
	gp_Vec n1p = n1x * parent_->characteristic().XDirection() +
	  n1y * parent_->characteristic().YDirection();
	// 3. The angle between the projected directions is just the
	// azimuthal angle.
	double dphi = n0p.Angle( n1p );
	gp_Dir meln = n0p ^ n1p;
	parent_->subassembly()->rotate( meln, dphi );
      }

      // Need to reconstruct the transformation of the face.
      face0_ = TopoDS::Face( parent_->model()->lookupShape( surface0_id_));
      plane0_ = Handle(Geom_Plane)::DownCast( BRep_Tool::Surface( face0_ ) );
    }

    // Translate the origin of plane0 so that it lies on plane1 with the
    // added requirement that the origin must move parallel to the
    // characteristic plane.

    // Compute a direction mostly normal to plane1 which also lies in the
    // plane of the characteristic.
    gp_Dir omega = parent_->characteristic().Direction().
      CrossCrossed( n1, parent_->characteristic().Direction() );

    // Compute the distance along omega from plane0's origin to plane1.
    double distance = ( gp_Vec( plane0_->Pln().Axis().Location(),
				plane1_->Pln().Axis().Location() ) * n1 ) /
      ( omega * n1 ) + offset_;

    gp_Vec translation = distance * omega;

    parent_->subassembly()->translate( translation );

    // Need to reconstruct the transformation of the face so that the
    // plane geometry is current.
    face0_ = TopoDS::Face( parent_->model()->lookupShape( surface0_id_));
    plane0_ = Handle(Geom_Plane)::DownCast( BRep_Tool::Surface( face0_ ) );

    updateDimension();
  }

  /*!
   * Append the offset to the XML representation as an attribute.
   * \param xml_rep XML representation
   */
  void write ( QDomElement& xml_rep ) const
  {
    PlnPln::write( xml_rep );
    xml_rep.setAttribute( lC::STR::OFFSET, lC::format( offset_ ) );
  }
private:
  double offset_;
};

class AlignOffsetPlnPln2 : public PlnPln {
public:
  AlignOffsetPlnPln2 ( const AssemblyConstraint* parent )
    : PlnPln( parent ), offset_( 0 )
  {}

  AlignOffsetPlnPln2 ( const QDomElement& xml_rep, const AssemblyConstraint* parent)
    : PlnPln( xml_rep, parent ), offset_( 0 )
  {
    offset_ = xml_rep.attribute( lC::STR::OFFSET ).toDouble();

    // I guess it's possible that the XML representation was not complete.
    if ( ! ( face0_.IsNull() || face1_.IsNull() ) )
      transform();
  }

  //! \return a (more or less) deep copy of this object.

  AlignOffsetPlnPln2* clone ( void ) const
  {
    AlignOffsetPlnPln2* new_plnpln = new AlignOffsetPlnPln2( parent_ );

    new_plnpln->surface0_id_ = surface0_id_;
    new_plnpln->face0_ = face0_;
    new_plnpln->plane0_ = plane0_;
    new_plnpln->surface1_id_ = surface1_id_;
    new_plnpln->face1_ = face1_;
    new_plnpln->plane1_ = plane1_;
    new_plnpln->offset_ = offset_;

    return new_plnpln;
  }

  //! \return the offset.
  double offset ( void ) const { return offset_; }
  /*!
   * Apply the offset.
   * \param offset the new offset.
   */
  void setOffset ( double offset )
  {
    offset_ = offset;
  }

  /*!
   * Change the offset and recompute the subassembly transformation.
   * \param offset the new offset.
   */
  void updateOffset ( double offset )
  {
    offset_ = offset;

    transform();
  }

  //! \return the coordinate system characteristic of this constraint. For a
  //! plane, it's the normal of the SECOND plane.

  gp_Ax2 characteristic ( void ) const
  {
    // Nothing comes after this so this method should never be invoked...
    return gp::XOY();
  }

  AssemblyConstraintStatus validate0 ( const QVector<uint>& surface0_id )
  {
    surface0_id_ = surface0_id;
    TopoDS_Shape shape0 = parent_->model()->lookupShape( surface0_id_ );

    // We already know this is a plane of some sort.
    face0_ = TopoDS::Face( shape0 );
    Handle(Geom_Surface) surface0 = BRep_Tool::Surface( face0_ );
    plane0_ = Handle(Geom_Plane)::DownCast( surface0 );

    // In the third phase, the selected subassembly surface has to already be
    // non-normal to the phase two characteristic.

    if ( (plane0_->Pln().Axis().Direction().
	  IsNormal( parent_->characteristic().Direction(),
		    Precision::Angular() ) ) )
      return Invalid;

    return OK;
  }

  AssemblyConstraintStatus validate1 ( const QVector<uint>& surface1_id )
  {
    Handle(Standard_Type) type = parent_->model()->lookupType(surface1_id);

    if ( type != STANDARD_TYPE( Geom_Plane ) )
      return Invalid;

    surface1_id_ = surface1_id;
    TopoDS_Shape shape1 = parent_->model()->lookupShape( surface1_id_ );

    // We already know this is a plane of some sort.
    face1_ = TopoDS::Face( shape1 );
    Handle(Geom_Surface) surface1 = BRep_Tool::Surface( face1_ );
    plane1_ = Handle(Geom_Plane)::DownCast( surface1 );

    // Since only a translation is possible in phase three, the surfaces
    // must already have parallel normals.

    gp_Dir n0 = plane0_->Pln().Axis().Direction();
    gp_Dir n1 = plane1_->Pln().Axis().Direction();

    if ( face0_.Orientation() == TopAbs_REVERSED ) n0.Reverse();
    if ( face1_.Orientation() == TopAbs_REVERSED ) n1.Reverse();

    if ( !n0.IsEqual( n1, Precision::Angular() ) )
      return Invalid;

    return ConstraintComplete;
  }

  void transform ( void )
  {
    // Only a translation in the characteristic direction is
    // possible at this point.

    gp_Dir n0 = plane0_->Pln().Axis().Direction();
    gp_Dir n1 = plane1_->Pln().Axis().Direction();

    if ( face0_.Orientation() == TopAbs_REVERSED ) n0.Reverse();
    if ( face1_.Orientation() == TopAbs_REVERSED ) n1.Reverse();

    gp_Vec delta( plane0_->Pln().Axis().Location(),
		  plane1_->Pln().Axis().Location() );
    double distance = ( delta * n1 + offset_ ) /
      ( parent_->characteristic().Direction() * n1 );
    gp_Vec translation = distance * parent_->characteristic().Direction();

    parent_->subassembly()->translate( translation );

    // Need to reconstruct the transformation of the face so that the
    // plane geometry is current.
    face0_ = TopoDS::Face( parent_->model()->lookupShape( surface0_id_));
    plane0_ = Handle(Geom_Plane)::DownCast( BRep_Tool::Surface( face0_ ) );

    updateDimension();
  }

  /*!
   * Append the offset to the XML representation as an attribute.
   * \param xml_rep XML representation
   */
  void write ( QDomElement& xml_rep ) const
  {
    PlnPln::write( xml_rep );
    xml_rep.setAttribute( lC::STR::OFFSET, lC::format( offset_ ) );
  }
private:
  double offset_;
};

// Return the Model reference.

Model* AssemblyConstraint::model ( void ) const
{
  return subassembly_->model();
}

// Remove the first surface from the constraint.

void AssemblyConstraint::removeFirstReference ( void )
{
  // Effectively creates an empty constraint.

  if ( surfaces_ != 0 ) {
    delete surfaces_;
    surfaces_ = 0;
  }
}

// Remove the last selected surface from the constraint.

void AssemblyConstraint::removeLastReference ( void )
{
  // Have to delegate this operation to the surface pair.
  if ( surfaces_ != 0 )
    surfaces_->removeLastReference();
}

// Serialize the assembly constraint.

void AssemblyConstraint::write ( QDomElement& xml_rep ) const
{
  QDomDocument document = xml_rep.ownerDocument();
  QDomElement constraint_element = document.createElement( type() );

  constraint_element.setAttribute( lC::STR::PHASE, phase() );

  if ( surfaces_ != 0 )
    surfaces_->write( constraint_element );

  xml_rep.appendChild( constraint_element );
}

// Return the surface pair's first end point for a dimension

Space3D::Point AssemblyConstraint::end0 ( void ) const
{
  if ( surfaces_ != 0 )
    return surfaces_->end0();

  return Space3D::Point();
}

// Return the surface pair's second end point for a dimension

Space3D::Point AssemblyConstraint::end1 ( void ) const
{
  if ( surfaces_ != 0 )
    return surfaces_->end1();

  return Space3D::Point();
}

// Return the surface pair's normal direction for a dimension

Space3D::Vector AssemblyConstraint::normal ( void ) const
{
  if ( surfaces_ != 0 )
    return surfaces_->normal();

  return Space3D::Vector();
}

// Update the end points of the dimension (on the theory that
// the solid has been moved by subsequent constraints).

void AssemblyConstraint::updateDimension ( void )
{
  if ( surfaces_ != 0 )
    surfaces_->updateDimension();
}

namespace {
  QString MATE = lC::STR::MATE;
  QString ALIGN = lC::STR::ALIGN;
  QString MATE_OFFSET = lC::STR::MATE_OFFSET;
  QString ALIGN_OFFSET = lC::STR::ALIGN_OFFSET;
}

/*!
 * The template of a surface constraint.
 */
template<class PlaneConstraint, QString* Type, int Phase> class SConstraint
  : public AssemblyConstraint {
public:
  SConstraint ( Subassembly* subassembly, gp_Ax2 characteristic )
    : AssemblyConstraint( subassembly, characteristic )
  {}

  SConstraint ( const QDomElement& xml_rep, Subassembly* subassembly,
		gp_Ax2 characteristic )
    : AssemblyConstraint( subassembly, characteristic )
  {
    if ( xml_rep.attribute( lC::STR::SURFACE ) == lC::STR::PLANE ) {
      surfaces_ = new PlaneConstraint( xml_rep, this );
    }
  }

  QString type ( void ) const { return *Type; }

  int phase ( void ) const { return Phase; }

  AssemblyConstraint* clone ( void ) const
  {
    SConstraint<PlaneConstraint, Type, Phase>* new_sc =
      new SConstraint<PlaneConstraint, Type, Phase>( subassembly_, characteristic_);

    if ( surfaces_ != 0 )
      new_sc->surfaces_ = surfaces_->clone();

    return new_sc;
  }

  AssemblyConstraintStatus validate ( const QVector<uint>& surface_id )
  {
    // Determine what kind of surface is supplied and create a surface
    // pair object accordingly.

    AssemblyConstraintStatus status;

    if ( surfaces_ == 0 ) {
      Handle(Standard_Type) type = subassembly_->model()->lookupType(surface_id);

      if ( type == STANDARD_TYPE( Geom_Plane ) ) {
	surfaces_ = new PlaneConstraint( this );

	status = surfaces_->validate0( surface_id );

	if ( status == Invalid ) {
	  delete surfaces_;
	  surfaces_ = 0;
	}
      }
      else
	return Invalid;
    }
    else {
      return surfaces_->validate1( surface_id );
    }

    return status;
  }

  /*!
   * Update the constraint with what is probably a partial representation.
   */
  void update ( const QDomElement& xml_rep )
  {

    if ( surfaces_ == 0 ) {
      QString surface0_db_url = xml_rep.attribute( lC::STR::SURFACE0 );

      if ( !surface0_db_url.isEmpty() ) {
	QVector<uint> surface0_id =
	  subassembly_->model()->pathID( DBURL( surface0_db_url ) );

	Handle(Standard_Type) type =
	  subassembly_->model()->lookupType( surface0_id );

	if ( type == STANDARD_TYPE( Geom_Plane ) ) {
	  surfaces_ = new PlaneConstraint( this );

	}
      }
    }
    else {
      QString surface1_db_url = xml_rep.attribute( lC::STR::SURFACE1 );

      if ( !surface1_db_url.isEmpty() ) {
	QVector<uint> surface1_id =
	  subassembly_->model()->pathID( DBURL( surface1_db_url ) );

	surfaces_->validate1( surface1_id );
      }
      // Hmm. This seems really out of place...
      if ( xml_rep.hasAttribute( lC::STR::OFFSET ) ) {
	double offset = xml_rep.attribute( lC::STR::OFFSET ).toDouble();
	surfaces_->setOffset( offset );
      }

      surfaces_->transform();
    }
  }
};

// Return the constraint at the given phase without modifying the current
// constraint.

AssemblyConstraint* AssemblyConstraintManager::constraint ( uint phase ) const
{
    return constraints_[ phase ].get();
}

// Compute the current status of the assembly constraint.

AssemblyConstraintStatus AssemblyConstraintManager::status ( void ) const
{
  AssemblyConstraintStatus status = Invalid;

  if ( currentConstraint != 0 ) {
    if ( !currentConstraint->reference0().empty() ) {
      status = OK;
      if ( !currentConstraint->reference1().empty() ) {
	status = ConstraintComplete;
	if ( constraints_.count() == 3 )
	  status = PlacementComplete;
      }
    }
  }

  return status;
}

void AssemblyConstraintManager::addMate ( void )
{
  if ( constraints_.count() == 0 )
    constraints_.append( std::shared_ptr<AssemblyConstraint>(new SConstraint<MatePlnPln0,&MATE,0>( parent_, gp::XOY())));
  else if ( constraints_.count() == 1 )
    constraints_.append( std::shared_ptr<AssemblyConstraint>(new SConstraint<MatePlnPln1,&MATE,1>( parent_,
                       currentConstraint->phaseCharacteristic() ) ));
  else if ( constraints_.count() == 2 )
    constraints_.append( std::shared_ptr<AssemblyConstraint>(new SConstraint<MatePlnPln2,&MATE,2>( parent_,
                       currentConstraint->phaseCharacteristic() ) ) );

  parent_->newConstraint( currentConstraint );
}

void AssemblyConstraintManager::addMate ( const QDomElement& xml_rep )
{
  int phase = xml_rep.attribute( lC::STR::PHASE ).toInt();

  switch ( phase ) {
  case 0:
    if ( constraints_.count() == 0 ) {
      constraints_.append( std::shared_ptr<AssemblyConstraint>(new SConstraint<MatePlnPln0,&MATE,0>( xml_rep, parent_,
                                 gp::XOY() ) ) );

      parent_->newConstraint( currentConstraint );
    }
    else if ( constraints_.count() == 1 ) {
      AssemblyConstraint* old_constraint = currentConstraint->clone();

      currentConstraint->update( xml_rep );

      parent_->changeConstraint( old_constraint, currentConstraint );
      delete old_constraint;
    }
    break;
  case 1:
    if ( constraints_.count() == 1 ) {
      constraints_.append( std::shared_ptr<AssemblyConstraint>(new SConstraint<MatePlnPln1,&MATE,1>( xml_rep, parent_,
                       currentConstraint->phaseCharacteristic() ) ) );

      parent_->newConstraint( currentConstraint );
    }
    else if ( constraints_.count() == 2 ) {
      AssemblyConstraint* old_constraint = currentConstraint->clone();
      currentConstraint->update( xml_rep );
      parent_->changeConstraint( old_constraint, currentConstraint );
      delete old_constraint;
    }
    break;
  case 2:
    if ( constraints_.count() == 2 ) {
      constraints_.append( std::shared_ptr<AssemblyConstraint>(new SConstraint<MatePlnPln2,&MATE,2>( xml_rep, parent_,
                    currentConstraint->phaseCharacteristic() ) ) );

      parent_->newConstraint( currentConstraint );
    }
    else if ( constraints_.count() == 3 ) {
      AssemblyConstraint* old_constraint = currentConstraint->clone();
      currentConstraint->update( xml_rep );
      parent_->changeConstraint( old_constraint, currentConstraint );
      delete old_constraint;
    }
  }
}

void AssemblyConstraintManager::addAlign ( void )
{
  if ( constraints_.count() == 0 )
    constraints_.append( std::shared_ptr<AssemblyConstraint>(new SConstraint<AlignPlnPln0,&ALIGN,0>( parent_, gp::XOY())));
  else if ( constraints_.count() == 1 )
    constraints_.append(std::shared_ptr<AssemblyConstraint>( new SConstraint<AlignPlnPln1,&ALIGN,1>( parent_,
                       currentConstraint->phaseCharacteristic() ) ) );
  else if ( constraints_.count() == 2 )
    constraints_.append( std::shared_ptr<AssemblyConstraint>(new SConstraint<AlignPlnPln2,&ALIGN,2>( parent_,
                       currentConstraint->phaseCharacteristic() ) ) );

  parent_->newConstraint( currentConstraint );
}

void AssemblyConstraintManager::addAlign ( const QDomElement& xml_rep )
{
  int phase = xml_rep.attribute( lC::STR::PHASE ).toInt();

  switch ( phase ) {
  case 0:
    if ( constraints_.count() == 0 ) {
      constraints_.append(std::shared_ptr<AssemblyConstraint>( new SConstraint<AlignPlnPln0,&ALIGN,0>( xml_rep, parent_,
                                   gp::XOY() ) ) );

      parent_->newConstraint( currentConstraint );
    }
    else if ( constraints_.count() == 1 ) {
      AssemblyConstraint* old_constraint = currentConstraint->clone();
      currentConstraint->update( xml_rep );
      parent_->changeConstraint( old_constraint, currentConstraint );
      delete old_constraint;
    }
    break;
  case 1:
    if ( constraints_.count() == 1 ) {
      constraints_.append( std::shared_ptr<AssemblyConstraint>(new SConstraint<AlignPlnPln1,&ALIGN,1>( xml_rep, parent_,
                     currentConstraint->phaseCharacteristic() ) ) );

      parent_->newConstraint( currentConstraint );
    }
    else if ( constraints_.count() == 2 ) {
      AssemblyConstraint* old_constraint = currentConstraint->clone();
      currentConstraint->update( xml_rep );
      parent_->changeConstraint( old_constraint, currentConstraint );
      delete old_constraint;
    }
    break;
  case 2:
    if ( constraints_.count() == 2 ) {
      constraints_.append( std::shared_ptr<AssemblyConstraint>( new SConstraint<AlignPlnPln2,&ALIGN,2>( xml_rep, parent_,
                     currentConstraint->phaseCharacteristic() ) ) );

      parent_->newConstraint( currentConstraint );
    }
    else if ( constraints_.count() == 3 ) {
      AssemblyConstraint* old_constraint = currentConstraint->clone();
      currentConstraint->update( xml_rep );
      parent_->changeConstraint( old_constraint, currentConstraint );
      delete old_constraint;
    }
  }
}

void AssemblyConstraintManager::addMateOffset ( void )
{
  if ( constraints_.count() == 0 )
    constraints_.append( std::shared_ptr<AssemblyConstraint>(new SConstraint<MateOffsetPlnPln0,&MATE_OFFSET,0>( parent_,
                                    gp::XOY() ) ) );
  else if ( constraints_.count() == 1 )
    constraints_.append( std::shared_ptr<AssemblyConstraint>( new SConstraint<MateOffsetPlnPln1,&MATE_OFFSET,1>( parent_,
                     currentConstraint->phaseCharacteristic() ) ) );

  else if ( constraints_.count() == 2 )
    constraints_.append( std::shared_ptr<AssemblyConstraint>( new SConstraint<MateOffsetPlnPln2,&MATE_OFFSET,2>( parent_,
                     currentConstraint->phaseCharacteristic() ) ) );

  parent_->newConstraint( currentConstraint );
}

void AssemblyConstraintManager::addMateOffset ( const QDomElement& xml_rep )
{
  int phase = xml_rep.attribute( lC::STR::PHASE ).toInt();

  switch ( phase ) {
  case 0:
    if ( constraints_.count() == 0 ) {
      constraints_.append( std::shared_ptr<AssemblyConstraint>( new SConstraint<MateOffsetPlnPln0,&MATE_OFFSET,0>(xml_rep,
									  parent_,
                                   gp::XOY() ) ) );
      parent_->newConstraint( currentConstraint );
    }
    else if ( constraints_.count() == 1 ) {
      AssemblyConstraint* old_constraint = currentConstraint->clone();
      currentConstraint->update( xml_rep );
      parent_->changeConstraint( old_constraint, currentConstraint );
      delete old_constraint;
    }
    break;
  case 1:
    if ( constraints_.count() == 1 ) {
      constraints_.append(std::shared_ptr<AssemblyConstraint>( new SConstraint<MateOffsetPlnPln1,&MATE_OFFSET,1>(xml_rep,
									  parent_,
                    currentConstraint->phaseCharacteristic())));
      parent_->newConstraint( currentConstraint );
    }
    else if ( constraints_.count() == 2 ) {
      AssemblyConstraint* old_constraint = currentConstraint->clone();
      currentConstraint->update( xml_rep );
      parent_->changeConstraint( old_constraint, currentConstraint );
      delete old_constraint;
    }
    break;
  case 2:
    if ( constraints_.count() == 2 ) {
      constraints_.append(std::shared_ptr<AssemblyConstraint>(new SConstraint<MateOffsetPlnPln2,&MATE_OFFSET,2>(xml_rep,
									  parent_,
                    currentConstraint->phaseCharacteristic())));
      parent_->newConstraint( currentConstraint );
    }
    else if ( constraints_.count() == 3 ) {
      AssemblyConstraint* old_constraint = currentConstraint->clone();
      currentConstraint->update( xml_rep );
      parent_->changeConstraint( old_constraint, currentConstraint );
      delete old_constraint;
    }
  }
}

void AssemblyConstraintManager::addAlignOffset ( void )
{
  if ( constraints_.count() == 0 )
    constraints_.append(std::shared_ptr<AssemblyConstraint>(new SConstraint<AlignOffsetPlnPln0,&ALIGN_OFFSET,0>(parent_,
                                      gp::XOY())));
  else if ( constraints_.count() == 1 )
    constraints_.append(std::shared_ptr<AssemblyConstraint>(new SConstraint<AlignOffsetPlnPln1,&ALIGN_OFFSET,1>(parent_,
                    currentConstraint->phaseCharacteristic() ) ) );
									     
  else if ( constraints_.count() == 2 )
    constraints_.append(std::shared_ptr<AssemblyConstraint>(new SConstraint<AlignOffsetPlnPln2,&ALIGN_OFFSET,2>(parent_,
                    currentConstraint->phaseCharacteristic() ) ) );

  parent_->newConstraint( currentConstraint );
}

void AssemblyConstraintManager::addAlignOffset ( const QDomElement& xml_rep )
{
  int phase = xml_rep.attribute( lC::STR::PHASE ).toInt();

  switch ( phase ) {
  case 0:
    if ( constraints_.count() == 0 ) {
      constraints_.append( std::shared_ptr<AssemblyConstraint>(new SConstraint<AlignOffsetPlnPln0,&ALIGN_OFFSET,0>( xml_rep,
									    parent_,
                                      gp::XOY() ) ) );
      parent_->newConstraint( currentConstraint );
    }
    else if ( constraints_.count() == 1 ) {
      AssemblyConstraint* old_constraint = currentConstraint->clone();
      currentConstraint->update( xml_rep );
      parent_->changeConstraint( old_constraint, currentConstraint );
      delete old_constraint;
    }
    break;
  case 1:
    if ( constraints_.count() == 1 ) {
      constraints_.append( std::shared_ptr<AssemblyConstraint>( new SConstraint<AlignOffsetPlnPln1,&ALIGN_OFFSET,1>( xml_rep,
									    parent_,
                    currentConstraint->phaseCharacteristic())));
      parent_->newConstraint( currentConstraint );
    }
    else if ( constraints_.count() == 2 ) {
      AssemblyConstraint* old_constraint = currentConstraint->clone();
      currentConstraint->update( xml_rep );
      parent_->changeConstraint( old_constraint, currentConstraint );
      delete old_constraint;
    }
    break;
  case 2:
    if ( constraints_.count() == 2 ) {
      constraints_.append( std::shared_ptr<AssemblyConstraint>(new SConstraint<AlignOffsetPlnPln2,&ALIGN_OFFSET,2>( xml_rep,
									    parent_,
                  currentConstraint->phaseCharacteristic() ) ) );
      parent_->newConstraint( currentConstraint );
    }
    else if ( constraints_.count() == 3 ) {
      AssemblyConstraint* old_constraint = currentConstraint->clone();
      currentConstraint->update( xml_rep );
      parent_->changeConstraint( old_constraint, currentConstraint );
      delete old_constraint;
    }
  }
}

// Set the offset of the current constraint. This method implicitly defers
// the recalculation of the subassembly position until the transform() method
// is called.

void AssemblyConstraintManager::setOffset ( double offset )
{
  currentConstraint->setOffset( offset );
#if 0
  // It's probably the case that this modification also changed the previous
  // contraints' dimension end points.

  QPtrListIterator<AssemblyConstraint> constraint( constraints_ );

  for ( ; currentConstraint != 0; ++constraint ) {
    if ( currentConstraint->type() == lC::STR::MATE_OFFSET ||
     currentConstraint->type() == lC::STR::ALIGN_OFFSET ) {
      currentConstraint->updateDimension();

      parent_->changeConstraintOffset( currentConstraint );
    }
  }
#endif
}

// Set the offset of the given constraint. This method immediately
// recalculates the position of the subassembly.

void AssemblyConstraintManager::setOffset ( uint phase, double offset )
{
  constraint( phase )->updateOffset( offset );

  // It's probably the case that this modification also changed the previous
  // contraints' dimension end points.

  QList<std::shared_ptr<AssemblyConstraint>>::iterator constraint = constraints_.begin();

  for ( ; currentConstraint != 0; ++constraint ) {
    if ( currentConstraint->type() == lC::STR::MATE_OFFSET ||
     currentConstraint->type() == lC::STR::ALIGN_OFFSET ) {
      currentConstraint->updateDimension();

      parent_->changeConstraintOffset( currentConstraint );
    }
  }
}

AssemblyConstraintStatus
AssemblyConstraintManager::validate ( const QVector<uint>& surface_id )
{
  old_constraint_ = constraints_.last()->clone();

  AssemblyConstraintStatus status = constraints_.last()->validate( surface_id );

  if ( status != Invalid )
    parent_->addDependency( surface_id );

  return status;
}

void AssemblyConstraintManager::apply ( void )
{
  parent_->changeConstraint( old_constraint_, currentConstraint );

  delete old_constraint_;
}

void AssemblyConstraintManager::transform ( void )
{
  currentConstraint->transform();

  parent_->changeConstraint( old_constraint_, currentConstraint );

  delete old_constraint_;

  // It's probably the case that this modification also changed the previous
  // contraints' dimension end points.

  QList<std::shared_ptr<AssemblyConstraint>>::iterator constraint = constraints_.begin();

  for ( ; constraint != constraints_.end(); ++constraint ) {
    if ( constraint->get()->type() == lC::STR::MATE_OFFSET ||
     constraint->get()->type() == lC::STR::ALIGN_OFFSET ) {
      constraint->get()->updateDimension();

      parent_->changeConstraintOffset( currentConstraint );
    }
  }
}

void AssemblyConstraintManager::write ( QDomElement& xml_rep ) const
{
  QDomDocument document = xml_rep.ownerDocument();
  QDomElement constraints_element = document.createElement( lC::STR::CONSTRAINTS );

  QList<std::shared_ptr<AssemblyConstraint>>::const_iterator constraint = constraints_.begin();
  for ( ; constraint != constraints_.end(); ++constraint )
    constraint->get()->write( constraints_element );

  xml_rep.appendChild( constraints_element );
}

void AssemblyConstraintManager::addConstraint ( const QDomElement& xml_rep )
{
  if ( xml_rep.tagName() == lC::STR::MATE )
    addMate( xml_rep );
  else if ( xml_rep.tagName() == lC::STR::ALIGN )
    addAlign( xml_rep );
  else if ( xml_rep.tagName() == lC::STR::MATE_OFFSET )
    addMateOffset( xml_rep );
  else if ( xml_rep.tagName() == lC::STR::ALIGN_OFFSET )
    addAlignOffset( xml_rep );
}

void AssemblyConstraintManager::updateConstraint ( const QDomElement& xml_rep )
{
  if ( xml_rep.tagName() == lC::STR::MATE )
    addMate( xml_rep );
  else if ( xml_rep.tagName() == lC::STR::ALIGN )
    addAlign( xml_rep );
  else if ( xml_rep.tagName() == lC::STR::MATE_OFFSET )
    addMateOffset( xml_rep );
  else if ( xml_rep.tagName() == lC::STR::ALIGN_OFFSET )
    addAlignOffset( xml_rep );
}

void AssemblyConstraintManager::addConstraints ( const QDomElement& xml_rep )
{
  QDomNode n = xml_rep.firstChild();

  while ( !n.isNull() ) {
    QDomElement e = n.toElement();
    if ( !e.isNull() ) {
      addConstraint( e );
    }
    n = n.nextSibling();
  }

  QList<std::shared_ptr<AssemblyConstraint>>::iterator constraint = constraints_.begin();
  for ( ; constraint != constraints_.end(); ++constraint ) {
    if ( !constraint->get()->reference0().empty() )
      parent_->addDependency( currentConstraint->reference0() );
    if ( !constraint->get()->reference1().empty() )
      parent_->addDependency( currentConstraint->reference1() );
  }
}

void AssemblyConstraintManager::recompute ( void )
{
  QList<std::shared_ptr<AssemblyConstraint>>::iterator constraint = constraints_.begin();
  gp_Ax2 characteristic = gp::XOY();
  for ( ; constraint != constraints_.end(); ++constraint ) {
    constraint->get()->recompute( characteristic );
    characteristic = constraint->get()->phaseCharacteristic();
  }
}

void AssemblyConstraintManager::cancelCurrent ( void )
{
  QMutableListIterator<std::shared_ptr<AssemblyConstraint>> it (constraints_);
  while(it.hasNext()) {
      if(it.next().get() == currentConstraint) {
          it.remove();
          break;
      }
  }

  parent_->cancelLast();
}

void AssemblyConstraintManager::removeFirstReference ( void )
{
  // Ah. What this actually does is to reset the surface pair element of
  // the current constraint since we don't know the initial surface type
  // any more.

  AssemblyConstraint* old_constraint = currentConstraint->clone();

  currentConstraint->removeFirstReference();

  parent_->changeConstraint( old_constraint, currentConstraint );

  delete old_constraint;
}

void AssemblyConstraintManager::removeLastReference ( void )
{
  AssemblyConstraint* old_constraint = currentConstraint->clone();

  currentConstraint->removeLastReference();

  parent_->changeConstraint( old_constraint, currentConstraint );

  delete old_constraint;
}
