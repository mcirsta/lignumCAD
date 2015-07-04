/*
 * ocsolid.cpp
 *
 * Solid classes based on OpenCASCADE
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

#include <gce_MakeLin.hxx>
#include <gp_Pln.hxx>
#include <gp_Cone.hxx>
#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepAlgoAPI_Common.hxx>
#include <BRepAlgoAPI_Cut.hxx>
#include <BRepAlgoAPI_Fuse.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakePolygon.hxx>
#include <BRepBuilderAPI_Transform.hxx>
#include <BRepMesh.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepPrimAPI_MakeCylinder.hxx>
#include <BRepPrimAPI_MakeCone.hxx>
#include <BRepPrimAPI_MakeHalfSpace.hxx>
#include <BRepPrimAPI_MakePrism.hxx>
#include <BRepPrimAPI_MakeRevolution.hxx>
#include <BRepTools.hxx>
#include <Convert_CompBezierCurvesToBSplineCurve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_Line.hxx>
#include <Geom_Plane.hxx>
#include <Geom_ConicalSurface.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_SurfaceOfRevolution.hxx>
#include <GeomAPI_IntCS.hxx>
#include <GeomAPI_IntSS.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_Array1OfInteger.hxx>

#include "constants.h"
#include "axis.h"
#include "units.h"
#include "material.h"
#include "model.h"
#include "page.h"
#include "ocsolid.h"

#include "lcdebug.h"

namespace Space3D {

  uint OCSolid::unique_index_ = 0;

  QString OCSolid::newName ( void )
  {
    return tr( "Solid[%1]" ).arg( ++unique_index_ );
  }

  OCSolid::OCSolid ( uint id, const QString& name, Page* parent )
    : Figure( id, name, lC::STR::SOLID, parent ),
      material_( 0 ), grain_csys_(gp::XOY())
  {}

  OCSolid::OCSolid ( uint id, const QDomElement& xml_rep, Page* parent )
    : Figure( id, QString::null, lC::STR::SOLID, parent ), material_( 0 )
  {
    setName( xml_rep.attribute( lC::STR::NAME ) );

    QRegExp regexp( tr("Solid\\[([0-9]+)\\]" ) );
    int position = regexp.search( name() );
    if ( position >= 0 ) {
      OCSolid::unique_index_ = QMAX( regexp.cap(1).toUInt(),
				     OCSolid::unique_index_ );
    }

    QDomNode n = xml_rep.firstChild();

    while ( !n.isNull() ) {
      n = n.nextSibling();
    }

    // If the solid is being recreated as part of an undo, it may
    // have internal self-references which need to resolved.
    parent->model()->resolveNow();

    setValid( true );
    setComplete( true );
  }

  // This constructor uses the simple XML representation (and ignores
  // the name element) [Used for pasting?]

  OCSolid::OCSolid ( uint id, const QString& name, const QDomElement& /*xml_rep*/,
			 Page* parent )
    : Figure( id, name, lC::STR::SOLID, parent )
  {
    QRegExp regexp( tr("Solid\\[([0-9]+)\\]" ) );
    int position = regexp.search( name );
    if ( position >= 0 ) {
      OCSolid::unique_index_ = QMAX( regexp.cap(1).toUInt(),
				     OCSolid::unique_index_ );
    }

    setValid( true );
    setComplete( true );
  }

  OCSolid::~OCSolid ( void )
  {}

  ModelItem* OCSolid::lookup ( QStringList& path_components ) const
  {
    // The front path component is the name of a figure with ".type" appended
    // to it.
    int dot_pos = path_components.front().findRev( '.' );
    QString name = path_components.front().left( dot_pos );
    QString type = path_components.front().right( path_components.front().length()
						  - dot_pos - 1 );

    return 0;
  }

  // Find the ModelItem associated with the path (presumably some kind
  // of geometry)

  ModelItem* OCSolid::lookup (QVector<uint> & /*id_path*/ ) const
  {
    return 0;			// Really an error...
  }

  // Find the OpenCASCADE type associated with the path (presumably some kind
  // of geometry)

  Handle(Standard_Type) OCSolid::lookupType ( QStringList& path_components )
    const
  {
    // The front path component is the name of a figure with ".type" appended
    // to it.
    int dot_pos = path_components.front().findRev( '.' );
    QString name = path_components.front().left( dot_pos );
    QString type = path_components.front().right( path_components.front().length()
						  - dot_pos - 1 );
    if ( type == lC::STR::FACE ) {
      const TopoDS_Face ref_face = face( name );
      Handle(Geom_Surface) surface;
      try {
	surface = BRep_Tool::Surface( ref_face );
      }
      catch ( ... ) {
	return Handle(Standard_Type)();
      }
      if ( !surface.IsNull() )
	return surface->DynamicType();
    }
    return Handle(Standard_Type)();
  }

  // Find the OpenCASCADE type associated with the path (presumably some kind
  // of geometry)

  Handle(Standard_Type) OCSolid::lookupType ( QVector<uint>& id_path )
    const
  {
    const TopoDS_Face ref_face = face( id_path[0] );
    Handle(Geom_Surface) surface;
    try {
      surface = BRep_Tool::Surface( ref_face );
    }
    catch ( ... ) {
      return Handle(Standard_Type)();
    }
    if ( !surface.IsNull() )
      return surface->DynamicType();

    return Handle(Standard_Type)();
  }

  // Find the OpenCASCADE shape associated with the path.

  TopoDS_Shape OCSolid::lookupShape ( QStringList& path_components ) const
  {
    // The front path component is the name of a figure with ".type" appended
    // to it.
    int dot_pos = path_components.front().findRev( '.' );
    QString name = path_components.front().left( dot_pos );
    QString type = path_components.front().right( path_components.front().length()
						  - dot_pos - 1 );
    if ( type == lC::STR::FACE ) {
      return face( name );
    }

    return TopoDS_Shape();
  }

  // Find the OpenCASCADE shape associated with the path.

  TopoDS_Shape OCSolid::lookupShape (QVector<uint> &id_path ) const
  {
    return face( id_path[0] );
  }

  // Return the name of the geometry (face) indicated in the id path. (That
  // should be all that's left by this point, anyway.)

  QString OCSolid::idPath (QVector<uint> id_path ) const
  {
    __gnu_cxx::hash_map<TopoDS_Face,uint,lCShapeHasher>::const_iterator face =
      face_names_.begin();

    for ( ; face != face_names_.end(); ++face )
      if ( (*face).second == id_path[0] )
	return PartFactory::instance()->name( id_path[0] ) + '.' + lC::STR::FACE;

    return QString::null;	// Really an error...
  }

  // Return the name of the geometry (face) indicated in the string path. (That
  // should be all that's left by this point, anyway.)

  void OCSolid::pathID ( QStringList& path_components, QVector<uint>& id_path )
    const
  {
    // The front path component is the name of a figure with ".type" appended
    // to it.
    int dot_pos = path_components.front().findRev( '.' );
    QString name = path_components.front().left( dot_pos );
    QString type = path_components.front().right( path_components.front().length()
						  - dot_pos - 1 );

    __gnu_cxx::hash_map<TopoDS_Face,uint,lCShapeHasher>::const_iterator face =
      face_names_.begin();

    for ( ; face != face_names_.end(); ++face )
      if ( PartFactory::instance()->name( (*face).second ) == name &&
	   type == lC::STR::FACE ) {
	id_path.push_back( (*face).second );
      }
  }

  // By hook or by crook, determine which end of this edge is closest
  // to the given point and return the edges which intersect there

  Vertex OCSolid::closestVertex ( Point /*p*/, Curve* curve ) const
  {
    Vertex vertex( curve );
    return vertex;
  }

  // Use the given material (if non-zero).

  void OCSolid::setMaterial ( const Material* material )
  {
    material_ = material;

    emit materialChanged();
  }

  // Use the given grain orientation.

  void OCSolid::setGrain ( const gp_Ax2& grain_csys )
  {
    grain_csys_ = grain_csys;
    // The material hasn't changed, but any computations of material
    // appearance (e.g., texture maps) may differ.
    emit materialChanged();
  }

  // Add a resize handle point

  void OCSolid::addResizeHandle ( uint id, const HandleData& handle )
  {
    handles_[ id ] = handle;
  }

  // Add a dimensional parameter

  void OCSolid::addParameter ( const QString& name, const Parameter& parameter )
  {
    parameters_[ name ] = parameter;
  }

  // Add a construction datum

  void OCSolid::addDatum ( const QString& name, const ConstructionDatum* datum )
  {
    datums_[ name ] = datum;
  }

  // Add a name to a face in the solid

  void OCSolid::addFaceName ( const TopoDS_Face& face, const QString& name )
  {
    face_names_[ face ] = PartFactory::instance()->name( name ).second;
  }

  // Set the list of handle names for the face

  void OCSolid::setFaceHandles ( const QString& name,
				 const QVector<uint>& handles )
  {
    face_handles_[ name ] = handles;
  }

  /*
   * Append a simplified representation of the solid to the
   * argument XML document.
   */
  void OCSolid::copyWrite ( QDomElement& xml_rep ) const
  {
    QDomDocument document = xml_rep.ownerDocument();
    QDomElement solid_element = document.createElement( lC::STR::SOLID );
    solid_element.setAttribute( lC::STR::NAME, name() );

    xml_rep.appendChild( solid_element );
  }

  // A specific solid: the ever useful Rectangular Parallelipiped

  OCSolidRPP::OCSolidRPP ( uint id, const QString& name,
			   double length, double width, double thickness,
			   Page* parent )
    : OCSolid( id, name, parent )
  {
    create( length, width, thickness );
  }

  OCSolidRPP::OCSolidRPP ( uint id, const QString& name, const QDomElement& xml_rep,
			   Page* parent )
    : OCSolid( id, name, parent )
  {
    double length = xml_rep.attribute( lC::STR::LENGTH ).toDouble();
    double width = xml_rep.attribute( lC::STR::WIDTH ).toDouble();
    double thickness = xml_rep.attribute( lC::STR::THICKNESS ).toDouble();

    create( length, width, thickness );
  }

  void OCSolidRPP::create ( double length, double width, double thickness )
  {
    // Create the basic parameters (here, w/o dimension data)
    addParameter( lC::STR::LENGTH, Parameter( lC::STR::LENGTH_GRAIN,
					      Parameter::LINEAR, gp::DY()));
    addParameter( lC::STR::WIDTH, Parameter( lC::STR::WIDTH,
					     Parameter::LINEAR, gp::DX()));
    addParameter( lC::STR::THICKNESS, Parameter( lC::STR::THICKNESS,
						 Parameter::LINEAR, -gp::DX() ) );
    // Create the handles (here, w/o placement, just the orientation flags)
    addResizeHandle( 0, HandleData( gp_XYZ( -1, -1, -1 ) ) );
    addResizeHandle( 1, HandleData( gp_XYZ( -1, -1, 1 ) ) );
    addResizeHandle( 2, HandleData( gp_XYZ( -1, 1, -1 ) ) );
    addResizeHandle( 3, HandleData( gp_XYZ( -1, 1, 1 ) ) );
    addResizeHandle( 4, HandleData( gp_XYZ( 1, -1, -1 ) ) );
    addResizeHandle( 5, HandleData( gp_XYZ( 1, -1, 1 ) ) );
    addResizeHandle( 6, HandleData( gp_XYZ( 1, 1, -1 ) ) );
    addResizeHandle( 7, HandleData( gp_XYZ( 1, 1, 1 ) ) );

    // Setup the (cosmetic) relationships between faces and handles.
    QVector<uint> handles;
    handles.push_back( 4 );
    handles.push_back( 5 );
    handles.push_back( 6 );
    handles.push_back( 7 );
    setFaceHandles( lC::STR::RIGHT, handles ); handles.clear();
    handles.push_back( 0 );
    handles.push_back( 1 );
    handles.push_back( 2 );
    handles.push_back( 3 );
    setFaceHandles( lC::STR::LEFT, handles ); handles.clear();
    handles.push_back( 2 );
    handles.push_back( 3 );
    handles.push_back( 6 );
    handles.push_back( 7 );
    setFaceHandles( lC::STR::FRONT, handles ); handles.clear();
    handles.push_back( 0 );
    handles.push_back( 1 );
    handles.push_back( 4 );
    handles.push_back( 5 );
    setFaceHandles( lC::STR::BACK, handles ); handles.clear();
    handles.push_back( 1 );
    handles.push_back( 3 );
    handles.push_back( 5 );
    handles.push_back( 7 );
    setFaceHandles( lC::STR::TOP, handles ); handles.clear();
    handles.push_back( 0 );
    handles.push_back( 2 );
    handles.push_back( 4 );
    handles.push_back( 6 );
    setFaceHandles( lC::STR::BOTTOM, handles ); handles.clear();

    // Construct the solid and setup the dimension dependent for the parameters
    // and handles. (This allows all parameter dependent values to redone with one
    // routine.)
    construct( length, width, thickness );

    // Somehow, the grain orientation has ended up being here.
    gp_Ax2 grain( gp::YOZ() );
    grain.Rotate( gp::OX(), 3. * M_PI_2 );
    setGrain( grain );

    // OK, ready to roll.
    setValid( true );
    setComplete( true );
  }

  void OCSolidRPP::construct ( double length, double width, double thickness )
  {
    try {
      // Super budget action thanks to OpenCASCADE
      solid_ = BRepPrimAPI_MakeBox( gp_Pnt( -length/2, -width/2, -thickness/2 ),
				    length, width, thickness ).Solid();
    }
    catch ( ... ) {
      cout << "Construction of RPP failed" << endl;
    }
    // (Try to) figure out which face is which so that they can be given
    // unique names. Sigh. It seems that it would be a lot easier to
    // construct these things ourselves rather than use the BRepPrim routines...
    
    clearFaceNames();
    TopExp_Explorer faces( solid_, TopAbs_FACE );

    for ( ; faces.More(); faces.Next() ) {
      TopoDS_Face face = TopoDS::Face( faces.Current() );
      bool reverse( face.Orientation() == TopAbs_REVERSED );
      Handle( Geom_Surface ) surface = BRep_Tool::Surface( face );
      Handle( Geom_Plane ) plane = Handle(Geom_Plane)::DownCast( surface );
      gp_Dir normal = plane->Pln().Axis().Direction();
      if ( reverse ) normal.Reverse();
      if ( surface->DynamicType() == STANDARD_TYPE( Geom_Plane ) ) {
	if ( normal.IsEqual( gp::DX(), gp::Resolution() ) ) {
	  addFaceName( face, lC::STR::RIGHT );
	}
	else if ( normal.IsEqual( -gp::DX(), gp::Resolution() ) ) {
	  addFaceName( face, lC::STR::LEFT );
	}
	else if ( normal.IsEqual( gp::DY(), gp::Resolution() ) ) {
	  addFaceName( face, lC::STR::FRONT );
	}
	else if ( normal.IsEqual( -gp::DY(), gp::Resolution() ) ) {
	  addFaceName( face, lC::STR::BACK );
	}
	else if ( normal.IsEqual( gp::DZ(), gp::Resolution() ) ) {
	  addFaceName( face, lC::STR::TOP );
	}
	else if ( normal.IsEqual( -gp::DZ(), gp::Resolution() ) ) {
	  addFaceName( face, lC::STR::BOTTOM );
	}
	else
	  cout << "Hmm. Unrecognized plane" << endl;
      }
      else
	cout << "Hmm. None of the above" << endl;
    }

    // Create the discrete representation. Deflection argument needs more thought...
    BRepMesh::Mesh( solid_, 1. );

    // Set the value and dimension end points of the parameters.
    parameter( lC::STR::LENGTH ).setValueEnds( length,
					       gp_Pnt( -length/2, width/2, 0 ),
					       gp_Pnt( length/2., width/2, 0 ) );
    parameter( lC::STR::WIDTH ).setValueEnds( width,
					      gp_Pnt( length/2, -width/2, 0 ),
					      gp_Pnt( length/2, width/2, 0. ) );
    parameter( lC::STR::THICKNESS ).setValueEnds( thickness,
						  gp_Pnt( -length/2,0,-thickness/2),
						  gp_Pnt( -length/2,0,thickness/2));

    // Set the position of the handles.
    handle( 0 ).setPosition( gp_Pnt( -length/2, -width/2, -thickness/2) );
    handle( 1 ).setPosition( gp_Pnt( -length/2, -width/2, thickness/2 ) );
    handle( 2 ).setPosition( gp_Pnt( -length/2, width/2, -thickness/2 ) );
    handle( 3 ).setPosition( gp_Pnt( -length/2, width/2, thickness/2 ) );
    handle( 4 ).setPosition( gp_Pnt( length/2, -width/2, -thickness/2 ) );
    handle( 5 ).setPosition( gp_Pnt( length/2, -width/2, thickness/2 ) );
    handle( 6 ).setPosition( gp_Pnt( length/2, width/2, -thickness/2 ) );
    handle( 7 ).setPosition( gp_Pnt( length/2, width/2, thickness/2 ) );
  }

  void OCSolidRPP::updateParameter ( const QString& name, double value )
  {
    parameter( name ).setValue( value );

    double length = parameter( lC::STR::LENGTH ).value();
    double width = parameter( lC::STR::WIDTH ).value();
    double thickness = parameter( lC::STR::THICKNESS ).value();

    construct( length, width, thickness );

    emit modified();
  }

  void OCSolidRPP::updateParameters ( const std::map<QString,double>& parameters )
  {
    std::map<QString,double>::const_iterator p = parameters.begin();
    for ( ; p != parameters.end(); ++p ) {
      parameter( (*p).first ).setValue( (*p).second );
    }

    double length = parameter( lC::STR::LENGTH ).value();
    double width = parameter( lC::STR::WIDTH ).value();
    double thickness = parameter( lC::STR::THICKNESS ).value();

    construct( length, width, thickness );

    emit modified();
  }

  lC::ValidDelta OCSolidRPP::adjustHandle ( uint id, const Point& last_pnt,
					    Point& current_pnt )
  {
    Vector delta = current_pnt - last_pnt;

    delta[X] *= 2 * handle(id).xResizeSign();
    delta[Y] *= 2 * handle(id).yResizeSign();
    delta[Z] *= 2 * handle(id).zResizeSign();

    LengthUnit* length_unit = UnitsBasis::instance()->lengthUnit();
    lCPrecision* precision = length_unit->precision( UnitsBasis::instance()->format(),
					   UnitsBasis::instance()->precision() );
    double minimum_size = 1. / precision->unitsPerIn();
    lC::ValidDelta valid_delta ( delta[X] != 0, delta[Y] != 0, delta[Z] != 0 );

    double length = parameter( lC::STR::LENGTH ).value();
    double width = parameter( lC::STR::WIDTH ).value();
    double thickness = parameter( lC::STR::THICKNESS ).value();
    std::map<QString,double> parameters;

    if ( length + delta[X] <= minimum_size ) {
      if ( length <= minimum_size )
	valid_delta.dx_ok_ = false;
      else {
	delta[X] = minimum_size - length;
	current_pnt[X] = last_pnt[X] + handle(id).xResizeSign() * delta[X] / 2.;
      }
    }
    if ( valid_delta.dx_ok_ )
      parameters[lC::STR::LENGTH] = length + delta[X];

    if ( width + delta[Y] <= minimum_size ) {
      if ( width <= minimum_size )
	valid_delta.dy_ok_ = false;
      else {
	delta[Y] = minimum_size - width;
	current_pnt[Y] = last_pnt[Y] + handle(id).yResizeSign() * delta[Y] / 2.;
      }
    }
    if ( valid_delta.dy_ok_ )
      parameters[lC::STR::WIDTH] = width + delta[Y];

    if ( thickness + delta[Z] <= minimum_size ) {
      if ( thickness <= minimum_size )
	valid_delta.dz_ok_ = false;
      else {
	delta[Z] = minimum_size - thickness;
	current_pnt[Z] = last_pnt[Z] + handle(id).zResizeSign() * delta[Z] / 2.;
      }
    }
    if ( valid_delta.dz_ok_ )
      parameters[lC::STR::THICKNESS] = thickness + delta[Z];

    if ( valid_delta.dx_ok_ || valid_delta.dy_ok_ || valid_delta.dz_ok_ )
      updateParameters( parameters );

    return valid_delta;
  }

  void OCSolidRPP::write ( QDomElement& xml_rep ) const
  {
    if ( !isValid() ) return;

    QDomDocument document = xml_rep.ownerDocument();
    QDomElement solid_element = document.createElement( lC::STR::SOLID );
    solid_element.setAttribute( lC::STR::NAME, name() );

    if ( material() != 0 )
      solid_element.setAttribute( lC::STR::MATERIAL, material()->commonName() );
      
    QDomElement rpp_element = document.createElement( lC::STR::RPP );

    std::map<QString,Parameter>::const_iterator parameter = parametersBegin();
    for ( ; parameter != parametersEnd(); ++parameter )
      rpp_element.setAttribute( (*parameter).first,
				lC::format( (*parameter).second.value() ) );

    solid_element.appendChild( rpp_element );
    xml_rep.appendChild( solid_element );
  }

  // A specific solid: the unlikely Parallelogram Parallelipiped

  OCSolidPGP::OCSolidPGP ( uint id, const QString& name,
			   double length, double width, double thickness,
			   Page* parent )
    : OCSolid( id, name, parent )
  {
    create( length, width, thickness );
  }

  OCSolidPGP::OCSolidPGP ( uint id, const QString& name, const QDomElement& xml_rep,
			   Page* parent )
    : OCSolid( id, name, parent )
  {
    double length = xml_rep.attribute( lC::STR::LENGTH ).toDouble();
    double width = xml_rep.attribute( lC::STR::WIDTH ).toDouble();
    double thickness = xml_rep.attribute( lC::STR::THICKNESS ).toDouble();

    create( length, width, thickness );
  }

  void OCSolidPGP::create ( double length, double width, double thickness )
  {
    // Create the basic parameters (here, w/o dimension data)
    addParameter( lC::STR::LENGTH, Parameter( lC::STR::LENGTH_GRAIN,
					      Parameter::LINEAR, gp::DY()));
    addParameter( lC::STR::WIDTH, Parameter( lC::STR::WIDTH,
					     Parameter::LINEAR, gp::DX()));
    addParameter( lC::STR::THICKNESS, Parameter( lC::STR::THICKNESS,
						 Parameter::LINEAR, -gp::DX() ) );
    // Create the handles (here, w/o placement, just the orientation flags)
    addResizeHandle( 0, HandleData( gp_XYZ( -1, -1, -1 ) ) );
    addResizeHandle( 1, HandleData( gp_XYZ( -1, -1, 1 ) ) );
    addResizeHandle( 2, HandleData( gp_XYZ( -1, 1, -1 ) ) );
    addResizeHandle( 3, HandleData( gp_XYZ( -1, 1, 1 ) ) );
    addResizeHandle( 4, HandleData( gp_XYZ( 1, -1, -1 ) ) );
    addResizeHandle( 5, HandleData( gp_XYZ( 1, -1, 1 ) ) );
    addResizeHandle( 6, HandleData( gp_XYZ( 1, 1, -1 ) ) );
    addResizeHandle( 7, HandleData( gp_XYZ( 1, 1, 1 ) ) );

    // Setup the (cosmetic) relationships between faces and handles.
    QVector<uint> handles;
    handles.push_back( 4 );
    handles.push_back( 5 );
    handles.push_back( 6 );
    handles.push_back( 7 );
    setFaceHandles( lC::STR::RIGHT, handles ); handles.clear();
    handles.push_back( 0 );
    handles.push_back( 1 );
    handles.push_back( 2 );
    handles.push_back( 3 );
    setFaceHandles( lC::STR::LEFT, handles ); handles.clear();
    handles.push_back( 2 );
    handles.push_back( 3 );
    handles.push_back( 6 );
    handles.push_back( 7 );
    setFaceHandles( lC::STR::FRONT, handles ); handles.clear();
    handles.push_back( 0 );
    handles.push_back( 1 );
    handles.push_back( 4 );
    handles.push_back( 5 );
    setFaceHandles( lC::STR::BACK, handles ); handles.clear();
    handles.push_back( 1 );
    handles.push_back( 3 );
    handles.push_back( 5 );
    handles.push_back( 7 );
    setFaceHandles( lC::STR::TOP, handles ); handles.clear();
    handles.push_back( 0 );
    handles.push_back( 2 );
    handles.push_back( 4 );
    handles.push_back( 6 );
    setFaceHandles( lC::STR::BOTTOM, handles ); handles.clear();

    // Construct the solid and setup the dimension dependent for the parameters
    // and handles. (This allows all parameter dependent values to redone with one
    // routine.)
    construct( length, width, thickness );

    // Somehow, the grain orientation has ended up being here.
    gp_Ax2 grain( gp::YOZ() );
    grain.Rotate( gp::OX(), 3. * M_PI_2 );
    setGrain( grain );

    // OK, ready to roll.
    setValid( true );
    setComplete( true );
  }

  void OCSolidPGP::construct ( double length, double width, double thickness )
  {
    try {
      // Here we'll try something a little different (since I think
      // MakeWedge can't do exactly what we want)
      BRepBuilderAPI_MakePolygon base( gp_Pnt( 0, 0, 0 ),
				       gp_Pnt( length, 0, 0 ),
				       gp_Pnt( length + M_SQRT1_2 * width,
					       M_SQRT1_2 * width, 0 ),
				       gp_Pnt( M_SQRT1_2 * width, M_SQRT1_2 * width,
					       0 ),
				       Standard_True );
      TopoDS_Face base_face = BRepBuilderAPI_MakeFace( base.Wire() );
      TopoDS_Shape shape =
	BRepPrimAPI_MakePrism( base_face, gp_Vec( 0, M_SQRT1_2 * thickness,
						  M_SQRT1_2 * thickness ) ).Shape();
      TopTools_IndexedMapOfShape solids;
      TopExp::MapShapes( shape, TopAbs_SOLID, solids );
      if ( solids.Extent() == 1 )
	solid_ = TopoDS::Solid( solids(1) );
      else
	cout << "Hmm. there are " << solids.Extent() << " solids in the prism?"
	     << endl;
    }
    catch ( ... ) {
      cout << "Construction of PGP failed" << endl;
    }
    // (Try to) figure out which face is which so that they can be given
    // unique names. Sigh. It seems that it would be a lot easier to
    // construct these things ourselves rather than use the BRepPrim routines...
    
    clearFaceNames();
    TopExp_Explorer faces( solid_, TopAbs_FACE );

    for ( ; faces.More(); faces.Next() ) {
      TopoDS_Face face = TopoDS::Face( faces.Current() );
      bool reverse( face.Orientation() == TopAbs_REVERSED );
      Handle( Geom_Surface ) surface = BRep_Tool::Surface( face );
      Handle( Geom_Plane ) plane = Handle(Geom_Plane)::DownCast( surface );
      gp_Dir normal = plane->Pln().Axis().Direction();
      if ( reverse ) normal.Reverse();

      if ( surface->DynamicType() == STANDARD_TYPE( Geom_Plane ) ) {
	if ( normal.IsEqual( gp::DZ(), Precision::Confusion() ) ) {
	  addFaceName( face, lC::STR::TOP );
	}
	else if ( normal.IsEqual( -gp::DZ(), Precision::Confusion() ) ) {
	  addFaceName( face, lC::STR::BOTTOM );
	}
	else if ( normal * gp::DX() > 0 ) {
	  addFaceName( face, lC::STR::RIGHT );
	}
	else if ( normal * -gp::DX() > 0 ) {
	  addFaceName( face, lC::STR::LEFT );
	}
	else if ( normal * gp::DY() > 0 && fabs( normal * gp::DX() ) < Precision::Confusion() ) {
	  addFaceName( face, lC::STR::FRONT );
	}
	else if ( normal * -gp::DY() > 0 && fabs( normal * gp::DX() ) < Precision::Confusion() ) {
	  addFaceName( face, lC::STR::BACK );
	}
	else
	  cout << "Hmm. Unrecognized plane" << endl;
      }
      else
	cout << "Hmm. None of the above" << endl;
    }

    // Create the discrete representation. Deflection argument needs more thought...
    BRepMesh::Mesh( solid_, 1. );

    // Set the value and dimension end points of the parameters.
    parameter( lC::STR::LENGTH ).setValueEnds( length,
					       gp_Pnt( -length/2, width/2, 0 ),
					       gp_Pnt( length/2., width/2, 0 ) );
    parameter( lC::STR::WIDTH ).setValueEnds( width,
					      gp_Pnt( length/2, -width/2, 0 ),
					      gp_Pnt( length/2, width/2, 0. ) );
    parameter( lC::STR::THICKNESS ).setValueEnds( thickness,
						  gp_Pnt( -length/2,0,-thickness/2),
						  gp_Pnt( -length/2,0,thickness/2));

    // Set the position of the handles.
    handle( 0 ).setPosition( gp_Pnt( 0, 0, 0 ) );
    handle( 1 ).setPosition( gp_Pnt( 0, M_SQRT1_2 * thickness,
				       M_SQRT1_2 * thickness ) );
    handle( 2 ).setPosition( gp_Pnt( M_SQRT1_2 * width, M_SQRT1_2 * width, 0 ) );
    handle( 3 ).setPosition( gp_Pnt( M_SQRT1_2 * width, M_SQRT1_2 * width +
				       M_SQRT1_2 * thickness,
				       M_SQRT1_2 * thickness ) );
    handle( 4 ).setPosition( gp_Pnt( length, 0, 0 ) );
    handle( 5 ).setPosition( gp_Pnt( length, M_SQRT1_2 * thickness,
				       M_SQRT1_2 * thickness ) );
    handle( 6 ).setPosition( gp_Pnt( length + M_SQRT1_2 * width,
				       M_SQRT1_2 * width, 0 ) );
    handle( 7 ).setPosition( gp_Pnt( length + M_SQRT1_2 * width,
				       M_SQRT1_2 * width + M_SQRT1_2 * thickness,
				       M_SQRT1_2 * thickness ) );
  }

  void OCSolidPGP::updateParameter ( const QString& name, double value )
  {
    parameter( name ).setValue( value );

    double length = parameter( lC::STR::LENGTH ).value();
    double width = parameter( lC::STR::WIDTH ).value();
    double thickness = parameter( lC::STR::THICKNESS ).value();

    construct( length, width, thickness );

    emit modified();
  }

  void OCSolidPGP::updateParameters ( const std::map<QString,double>& parameters )
  {
    std::map<QString,double>::const_iterator p = parameters.begin();
    for ( ; p != parameters.end(); ++p ) {
      parameter( (*p).first ).setValue( (*p).second );
    }

    double length = parameter( lC::STR::LENGTH ).value();
    double width = parameter( lC::STR::WIDTH ).value();
    double thickness = parameter( lC::STR::THICKNESS ).value();

    construct( length, width, thickness );

    emit modified();
  }

  lC::ValidDelta OCSolidPGP::adjustHandle ( uint id, const Point& last_pnt,
					    Point& current_pnt )
  {
    Vector delta = current_pnt - last_pnt;

    delta[X] *= 2 * handle(id).xResizeSign();
    delta[Y] *= 2 * handle(id).yResizeSign();
    delta[Z] *= 2 * handle(id).zResizeSign();

    LengthUnit* length_unit = UnitsBasis::instance()->lengthUnit();
    lCPrecision* precision = length_unit->precision( UnitsBasis::instance()->format(),
					   UnitsBasis::instance()->precision() );
    double minimum_size = 1. / precision->unitsPerIn();
    lC::ValidDelta valid_delta ( delta[X] != 0, delta[Y] != 0, delta[Z] != 0 );

    double length = parameter( lC::STR::LENGTH ).value();
    double width = parameter( lC::STR::WIDTH ).value();
    double thickness = parameter( lC::STR::THICKNESS ).value();
    std::map<QString,double> parameters;

    if ( length + delta[X] <= minimum_size ) {
      if ( length <= minimum_size )
	valid_delta.dx_ok_ = false;
      else {
	delta[X] = minimum_size - length;
	current_pnt[X] = last_pnt[X] + handle(id).xResizeSign() * delta[X] / 2.;
      }
    }
    if ( valid_delta.dx_ok_ )
      parameters[lC::STR::LENGTH] = length + delta[X];

    if ( width + delta[Y] <= minimum_size ) {
      if ( width <= minimum_size )
	valid_delta.dy_ok_ = false;
      else {
	delta[Y] = minimum_size - width;
	current_pnt[Y] = last_pnt[Y] + handle(id).yResizeSign() * delta[Y] / 2.;
      }
    }
    if ( valid_delta.dy_ok_ )
      parameters[lC::STR::WIDTH] = width + delta[Y];

    if ( thickness + delta[Z] <= minimum_size ) {
      if ( thickness <= minimum_size )
	valid_delta.dz_ok_ = false;
      else {
	delta[Z] = minimum_size - thickness;
	current_pnt[Z] = last_pnt[Z] + handle(id).zResizeSign() * delta[Z] / 2.;
      }
    }
    if ( valid_delta.dz_ok_ )
      parameters[lC::STR::THICKNESS] = thickness + delta[Z];

    if ( valid_delta.dx_ok_ || valid_delta.dy_ok_ || valid_delta.dz_ok_ )
      updateParameters( parameters );

    return valid_delta;
  }

  void OCSolidPGP::write ( QDomElement& xml_rep ) const
  {
    if ( !isValid() ) return;

    QDomDocument document = xml_rep.ownerDocument();
    QDomElement solid_element = document.createElement( lC::STR::SOLID );
    solid_element.setAttribute( lC::STR::NAME, name() );

    if ( material() != 0 )
      solid_element.setAttribute( lC::STR::MATERIAL, material()->commonName() );
      
    QDomElement pgp_element = document.createElement( lC::STR::PGP );

    std::map<QString,Parameter>::const_iterator parameter = parametersBegin();
    for ( ; parameter != parametersEnd(); ++parameter )
      pgp_element.setAttribute( (*parameter).first,
				lC::format( (*parameter).second.value() ) );

    solid_element.appendChild( pgp_element );
    xml_rep.appendChild( solid_element );
  }

  // A specific solid: the cylinder (aka Turning).

  OCSolidCYL::OCSolidCYL ( uint id, const QString& name,
			   double length, double diameter,
			   Page* parent )
    : OCSolid( id, name, parent )
  {
    create( length, diameter );
  }

  OCSolidCYL::OCSolidCYL ( uint id, const QString& name, const QDomElement& xml_rep,
			   Page* parent )
    : OCSolid( id, name, parent )
  {
    double length = xml_rep.attribute( lC::STR::LENGTH ).toDouble();
    double diameter = xml_rep.attribute( lC::STR::DIAMETER ).toDouble();

    create( length, diameter );
  }

  void OCSolidCYL::create ( double length, double diameter )
  {
    // Create the basic parameters (here, w/o dimension data)
    addParameter( lC::STR::LENGTH, Parameter( lC::STR::LENGTH_GRAIN,
					      Parameter::LINEAR, gp::DY()));
    addParameter( lC::STR::DIAMETER, Parameter( lC::STR::DIAMETER,
						Parameter::DIAMETER, -gp::DZ()));
    // Create the handles (here, w/o placement, just the orientation flags)
    addResizeHandle( 0, HandleData( gp_XYZ( 0, 1, 1 ) ) );
    addResizeHandle( 1, HandleData( gp_XYZ( 0, 1, -1 ) ) );
    addResizeHandle( 2, HandleData( gp_XYZ( 0, -1, 1 ) ) );
    addResizeHandle( 3, HandleData( gp_XYZ( 0, -1, -1 ) ) );

    // Setup the (cosmetic) relationships between faces and handles.
    QVector<uint> handles;
    handles.push_back( 0 );
    handles.push_back( 2 );
    setFaceHandles( lC::STR::TOP, handles ); handles.clear();
    handles.push_back( 1 );
    handles.push_back( 3 );
    setFaceHandles( lC::STR::BOTTOM, handles ); handles.clear();
    handles.push_back( 0 );
    handles.push_back( 1 );
    handles.push_back( 2 );
    handles.push_back( 3 );
    setFaceHandles( lC::STR::CYL, handles ); handles.clear();

    // Although it is really not used in the construction per se, add an axis
    // construction datum.
    axis_ = new Axis( 0, lC::STR::AXIS, parent() );
    addDatum( lC::STR::AXIS, axis_ );

    // Construct the solid and setup the dimension dependent for the parameters
    // and handles. (This allows all parameter dependent values to redone with one
    // routine.)
    construct( length, diameter );

    // Somehow, the grain orientation has ended up being here.
    // However, the default grain orientation is OK for the CYL.

    // OK, ready to roll.
    setValid( true );
    setComplete( true );
  }

  void OCSolidCYL::construct ( double length, double diameter )
  {
    try {
      // Super budget action thanks to OpenCASCADE
      // (Note: place origin at centroid of the cylinder)
      gp_Ax2 axis = gp::XOY();
      axis.Translate( gp_Vec( 0, 0, -length/2 ) );
      solid_ = BRepPrimAPI_MakeCylinder( axis, diameter/2., length ).Solid();
    }
    catch ( ... ) {
      cout << "Construction of CYL failed" << endl;
    }
    // (Try to) figure out which face is which so that they can be given
    // unique names. Sigh. It seems that it would be a lot easier to
    // construct these things ourselves rather than use the BRepPrim routines...
    
    clearFaceNames();
    TopExp_Explorer faces( solid_, TopAbs_FACE );

    for ( ; faces.More(); faces.Next() ) {
      TopoDS_Face face = TopoDS::Face( faces.Current() );
      bool reverse( face.Orientation() == TopAbs_REVERSED );
      Handle( Geom_Surface ) surface = BRep_Tool::Surface( face );
      if ( surface->DynamicType() == STANDARD_TYPE( Geom_Plane ) ) {
	Handle( Geom_Plane ) plane = Handle(Geom_Plane)::DownCast( surface );
	gp_Dir normal = plane->Pln().Axis().Direction();
	if ( reverse ) normal.Reverse();
	if ( normal.IsEqual( gp::DZ(), gp::Resolution() ) ) {
	  addFaceName( face, lC::STR::TOP );
	}
	else if ( normal.IsEqual( -gp::DZ(), gp::Resolution() ) ) {
	  addFaceName( face, lC::STR::BOTTOM );
	}
	else
	  cout << "Hmm. Unrecognized plane" << endl;
      }
      else if ( surface->DynamicType() == STANDARD_TYPE( Geom_CylindricalSurface )){
	addFaceName( face, lC::STR::CYL );
      }
      else
	cout << "Hmm. None of the above" << endl;
    }

    // Create the discrete representation. Deflection argument needs more thought...
    BRepMesh::Mesh( solid_, 1. );

    // Set the (cosmetic) ends of the construction axis
    axis_->setMinimum( -1.1 * ( length / 2 ) );
    axis_->setMaximum(  1.1 * ( length / 2 ) );

    // Set the value and dimension end points of the parameters.
    parameter( lC::STR::LENGTH ).setValueEnds( length,
					       gp_Pnt( 0, diameter/2, -length/2 ),
					       gp_Pnt( 0, diameter/2, length/2 ) );
    parameter( lC::STR::DIAMETER ).setValueEnds( diameter,
						 gp_Pnt( 0, -diameter/2, -length/2),
						 gp_Pnt( 0, diameter/2, -length/2));

    // Set the position of the handles.
    handle( 0 ).setPosition( gp_Pnt( 0, diameter/2, length/2 ) );
    handle( 1 ).setPosition( gp_Pnt( 0, diameter/2, -length/2 ) );
    handle( 2 ).setPosition( gp_Pnt( 0, -diameter/2, length/2 ) );
    handle( 3 ).setPosition( gp_Pnt( 0, -diameter/2, -length/2 ) );
  }

  void OCSolidCYL::updateParameter ( const QString& name, double value )
  {
    parameter( name ).setValue( value );

    double length = parameter( lC::STR::LENGTH ).value();
    double diameter = parameter( lC::STR::DIAMETER ).value();

    construct( length, diameter );

    emit modified();
  }

  void OCSolidCYL::updateParameters ( const std::map<QString,double>& parameters )
  {
    std::map<QString,double>::const_iterator p = parameters.begin();
    for ( ; p != parameters.end(); ++p ) {
      parameter( (*p).first ).setValue( (*p).second );
    }

    double length = parameter( lC::STR::LENGTH ).value();
    double diameter = parameter( lC::STR::DIAMETER ).value();

    construct( length, diameter );

    emit modified();
  }

  lC::ValidDelta OCSolidCYL::adjustHandle ( uint id, const Point& last_pnt,
					    Point& current_pnt )
  {
    Vector delta = current_pnt - last_pnt;

    delta[X] *= 2 * handle(id).xResizeSign();
    delta[Y] *= 2 * handle(id).yResizeSign();
    delta[Z] *= 2 * handle(id).zResizeSign();

    LengthUnit* length_unit = UnitsBasis::instance()->lengthUnit();
    lCPrecision* precision = length_unit->precision( UnitsBasis::instance()->format(),
					   UnitsBasis::instance()->precision() );
    double minimum_size = 1. / precision->unitsPerIn();
    lC::ValidDelta valid_delta ( delta[X] != 0, delta[Y] != 0, delta[Z] != 0 );

    double length = parameter( lC::STR::LENGTH ).value();
    double diameter = parameter( lC::STR::DIAMETER ).value();
    std::map<QString,double> parameters;

    if ( length + delta[Z] <= minimum_size ) {
      if ( length <= minimum_size )
	valid_delta.dz_ok_ = false;
      else {
	delta[Z] = minimum_size - length;
	current_pnt[Z] = last_pnt[Z] + handle(id).zResizeSign() * delta[Z] / 2.;
      }
    }
    if ( valid_delta.dz_ok_ )
      parameters[lC::STR::LENGTH] = length + delta[Z];

    if ( diameter + delta[Y] <= minimum_size ) {
      if ( diameter <= minimum_size )
	valid_delta.dy_ok_ = false;
      else {
	delta[Y] = minimum_size - diameter;
	current_pnt[Y] = last_pnt[Y] + handle(id).yResizeSign() * delta[Y] / 2.;
      }
    }
    if ( valid_delta.dy_ok_ )
      parameters[lC::STR::DIAMETER] = diameter + delta[Y];

    if ( valid_delta.dy_ok_ || valid_delta.dz_ok_ )
      updateParameters( parameters );

    return valid_delta;
  }

  void OCSolidCYL::write ( QDomElement& xml_rep ) const
  {
    if ( !isValid() ) return;

    QDomDocument document = xml_rep.ownerDocument();
    QDomElement solid_element = document.createElement( lC::STR::SOLID );
    solid_element.setAttribute( lC::STR::NAME, name() );

    if ( material() != 0 )
      solid_element.setAttribute( lC::STR::MATERIAL, material()->commonName() );

    QDomElement cyl_element = document.createElement( lC::STR::CYL );

    std::map<QString,Parameter>::const_iterator parameter = parametersBegin();
    for ( ; parameter != parametersEnd(); ++parameter )
      cyl_element.setAttribute( (*parameter).first,
				lC::format( (*parameter).second.value() ) );

    solid_element.appendChild( cyl_element );
    xml_rep.appendChild( solid_element );
  }

  // A specific solid: a rectangular solid which transistions into a
  // cone (aka Turned, tapered Leg).

  OCSolidRPPCON::OCSolidRPPCON ( uint id, const QString& name,
				 double length,
				 double rec_length, double rec_width,
				 double con_length,
				 double con_top_diameter,
				 double con_bottom_diameter,
				 Page* parent )
    : OCSolid( id, name, parent )
  {
    create( length, rec_length, rec_width, con_length, con_top_diameter,
	    con_bottom_diameter );
  }

  OCSolidRPPCON::OCSolidRPPCON ( uint id, const QString& name,
				 const QDomElement& xml_rep, Page* parent )
    : OCSolid( id, name, parent )
  {
    double length = xml_rep.attribute( lC::STR::LENGTH ).toDouble();
    double rec_length = xml_rep.attribute( lC::STR::REC_LENGTH ).toDouble();
    double rec_width = xml_rep.attribute( lC::STR::REC_WIDTH ).toDouble();
    double con_length = xml_rep.attribute( lC::STR::CON_LENGTH ).toDouble();
    double con_top_diameter = xml_rep.attribute( lC::STR::CON_TOP_DIAMETER ).
      toDouble();
    double con_bottom_diameter = xml_rep.attribute( lC::STR::CON_BOTTOM_DIAMETER ).
      toDouble();

    create( length, rec_length, rec_width, con_length, con_top_diameter,
	    con_bottom_diameter );
  }

  void OCSolidRPPCON::create( double length, double rec_length, double rec_width,
			      double con_length, double con_top_diameter,
			      double con_bottom_diameter )
  {
    // Create the basic parameters (here, w/o dimension data)
    addParameter( lC::STR::LENGTH,
		  Parameter( lC::STR::LENGTH_GRAIN, Parameter::LINEAR, gp::DY() ) );
    addParameter( lC::STR::REC_WIDTH,
		  Parameter( "Top width", Parameter::LINEAR, gp::DZ() ) );
    addParameter( lC::STR::REC_LENGTH,
		  Parameter( "Top length", Parameter::LINEAR, -gp::DY() ) );
    addParameter( lC::STR::CON_LENGTH,
		  Parameter( "Turning length", Parameter::LINEAR, -gp::DY() ) );
    addParameter( lC::STR::CON_TOP_DIAMETER,
		  Parameter( "Turning top diameter",Parameter::DIAMETER,-gp::DX()));
    addParameter( lC::STR::CON_BOTTOM_DIAMETER,
		  Parameter( "Turning bottom diameter", Parameter::DIAMETER,
			     -gp::DZ() ) );

    // Create the resize handles (here, w/o placement, just the orientation flags)
    addResizeHandle( 0, HandleData( gp_XYZ( -1, -1,  1 ) ) );
    addResizeHandle( 1, HandleData( gp_XYZ(  1, -1,  1 ) ) );
    addResizeHandle( 2, HandleData( gp_XYZ(  1,  1,  1 ) ) );
    addResizeHandle( 3, HandleData( gp_XYZ( -1,  1,  1 ) ) );
    addResizeHandle( 4, HandleData( gp_XYZ(  0,  0, -1 ) ) );
    addResizeHandle( 5, HandleData( gp_XYZ(  0,  0, -1 ) ) );
    addResizeHandle( 6, HandleData( gp_XYZ(  0, -1,  1 ) ) );
    addResizeHandle( 7, HandleData( gp_XYZ(  0,  1,  1 ) ) );
    addResizeHandle( 8, HandleData( gp_XYZ(  0, -1, -1 ) ) );
    addResizeHandle( 9, HandleData( gp_XYZ(  0,  1, -1 ) ) );

    // Setup the (cosmetic) relationships between faces and handles.
    QVector<uint> handles;
    handles.push_back( 1 );
    handles.push_back( 2 );
    setFaceHandles( lC::STR::RIGHT, handles ); handles.clear();
    handles.push_back( 0 );
    handles.push_back( 3 );
    setFaceHandles( lC::STR::LEFT, handles ); handles.clear();
    handles.push_back( 2 );
    handles.push_back( 3 );
    handles.push_back( 5 );
    setFaceHandles( lC::STR::FRONT, handles ); handles.clear();
    handles.push_back( 0 );
    handles.push_back( 1 );
    handles.push_back( 4 );
    setFaceHandles( lC::STR::BACK, handles ); handles.clear();
    handles.push_back( 0 );
    handles.push_back( 1 );
    handles.push_back( 2 );
    handles.push_back( 3 );
    setFaceHandles( lC::STR::TOP, handles ); handles.clear();
    handles.push_back( 8 );
    handles.push_back( 9 );
    setFaceHandles( lC::STR::BOTTOM, handles ); handles.clear();
    handles.push_back( 4 );
    handles.push_back( 5 );
    handles.push_back( 6 );
    handles.push_back( 7 );
    setFaceHandles( lC::STR::TRANSISTION, handles ); handles.clear();
    handles.push_back( 6 );
    handles.push_back( 7 );
    handles.push_back( 8 );
    handles.push_back( 9 );
    setFaceHandles( lC::STR::SHANK, handles ); handles.clear();

    // Although it is really not used in the construction per se, add an axis
    // construction datum.
    axis_ = new Axis( 0, lC::STR::AXIS, parent() );
    addDatum( lC::STR::AXIS, axis_ );

    // Construct the solid and setup the dimension dependent for the parameters
    // and handles. (This allows all parameter dependent values to redone with one
    // routine.)
    construct( length, rec_length, rec_width, con_length, con_top_diameter,
	       con_bottom_diameter );

    // Somehow, the grain orientation has ended up being here.
    // However, the default grain orientation is OK for the RPPCON.

    // OK, ready to roll.
    setValid( true );
    setComplete( true );
  }

  void OCSolidRPPCON::construct ( double length,
				  double rec_length, double rec_width,
				  double con_length, double con_top_diameter,
				  double con_bottom_diameter )
  {
    try {
      // 1. Make an RPP the size of the whole leg, centered at the origin.

      TopoDS_Shape rpp = BRepPrimAPI_MakeBox( gp_Pnt( -rec_width/2., -rec_width/2.,
						      -length / 2. ),
					      rec_width, rec_width, length).Shape();

      // 2. Construct a negative Z-directed axis located at the transistion/cone
      // intersection (i.e. at the beginning of the conical section).

      gp_Ax2 axis = gp::XOY();
      axis.SetDirection( -axis.Direction() );
      axis.Translate( gp_Vec( 0, 0, -length/2 + con_length ) );

      // 3. Make the tapered cone

      TopoDS_Shape cone = BRepPrimAPI_MakeCone( axis, con_top_diameter/2,
						con_bottom_diameter/2,
						con_length /*+ 1*/ ).Shape();

      // 4. Now we need to construct a transition cone (tcone) with
      //    the following properties:
      //    A. One end is coincident with the internal end of the main cone
      //    B. The other end extends beyond the end RPP.
      //    C. Its meridian intersects the axial center of the RPP faces at
      //       rectangular_length distance from the end of the RPP. That is,
      //       The base of the hyperbola formed by the intersection of the
      //       cone and the side faces of the RPP is the length of the
      //       rectangular section.

      // 4.A: Just reverse the polarity of the main direction of the cone's axis
      axis.SetDirection( -axis.Direction() );

      // 4.B: The top plane of the transision cone is just beyond the
      // end of the RPP.  Simple enough, but we need to construct a
      // plane there for the next step.
      gp_Pnt tcone_top_origin( 0, 0, length/2. );
      gp_Ax3 tcone_top_csys( tcone_top_origin, gp::DZ() );
      Handle( Geom_Plane ) tcone_top_plane = new Geom_Plane( tcone_top_csys );

      // 4.C: The radius of the cone-intersecting end of the tcone is set,
      //      as is the height; all we have to compute now is the radius of
      //      the top. We pick this such that RPP section faces have length
      //      rectangular_length. We compute this by finding the line (which
      //      is a meridian of the tcone) in the
      //      XZ plane (for example) which connects the edge of the cone
      //      and intersects the normal face of the RPP at rectangular_length
      //      distance from the end of the RPP. Clear?
      //	 |       .
      // tcone   | +-----+\ <-transistion cone
      // inter-  | |     | |--_ 
      // sects   | | RPP | |--  CONE
      // this    | +-----+/
      // plane-> |       .

      gce_MakeLin gce_meridian( gp_Pnt( -con_top_diameter/2,0,-length/2+con_length),
				gp_Pnt( -rec_width/2, 0, length/2 - rec_length ) );
      Handle( Geom_Line ) tcone_meridian = new Geom_Line( gce_meridian.Value() );

      // Now, the intersection of this line with the cone top plane gives
      // the radius at the top end of the cone.
      GeomAPI_IntCS intersection( tcone_meridian, tcone_top_plane );

      TopoDS_Shape tcone =
	BRepPrimAPI_MakeCone( axis, con_top_diameter/2,
			      fabs( intersection.Point(1).X() ),
			      length - con_length ).Shape();

      // 5. The cone and tcone form a single tool path.
      TopoDS_Shape tool = BRepAlgoAPI_Fuse( cone, tcone ).Shape();

      // 6. Apply the tool to the RPP. The shape is everything that is
      // both inside the "blank" and the tool shape. Note that since
      // the Common operation generally returns a Compound shape, we need to
      // extract just the TopoDS_Solid through the mapper.
      BRepAlgoAPI_Common leg( tool, rpp );
      TopTools_IndexedMapOfShape solids;
      TopExp::MapShapes( leg.Shape(), TopAbs_SOLID, solids );
      if ( solids.Extent() == 1 )
	solid_ = TopoDS::Solid( solids(1) );
    }
    catch ( ... ) {
      cout << "Construction of RPPCON failed" << endl;
    }
    // (Try to) figure out which face is which so that they can be given
    // unique names. Sigh. It seems that it would be a lot easier to
    // construct these things ourselves rather than use the BRepPrim routines...
    
    clearFaceNames();
    TopExp_Explorer faces( solid_, TopAbs_FACE );

    for ( ; faces.More(); faces.Next() ) {
      TopoDS_Face face = TopoDS::Face( faces.Current() );
      bool reverse( face.Orientation() == TopAbs_REVERSED );
      Handle( Geom_Surface ) surface = BRep_Tool::Surface( face );
      if ( surface->DynamicType() == STANDARD_TYPE( Geom_Plane ) ) {
	Handle( Geom_Plane ) plane = Handle(Geom_Plane)::DownCast( surface );
	gp_Dir normal = plane->Pln().Axis().Direction();
	if ( reverse ) normal.Reverse();
	if ( normal.IsEqual( gp::DX(), gp::Resolution() ) ) {
	  addFaceName( face, lC::STR::RIGHT );
	}
	else if ( normal.IsEqual( -gp::DX(), gp::Resolution() ) ) {
	  addFaceName( face, lC::STR::LEFT );
	}
	else if ( normal.IsEqual( gp::DY(), gp::Resolution() ) ) {
	  addFaceName( face, lC::STR::FRONT );
	}
	else if ( normal.IsEqual( -gp::DY(), gp::Resolution() ) ) {
	  addFaceName( face, lC::STR::BACK );
	}
	else if ( normal.IsEqual( gp::DZ(), gp::Resolution() ) ) {
	  addFaceName( face, lC::STR::TOP );
	}
	else if ( normal.IsEqual( -gp::DZ(), gp::Resolution() ) ) {
	  addFaceName( face, lC::STR::BOTTOM );
	}
	else
	  cout << "Hmm. Unrecognized plane" << endl;
      }
      else if ( surface->DynamicType() == STANDARD_TYPE( Geom_ConicalSurface )){
	Handle( Geom_ConicalSurface ) conical_surface =
	  Handle(Geom_ConicalSurface)::DownCast( surface );
	gp_Dir direction  = conical_surface->Cone().Axis().Direction();
	if ( direction.IsEqual( gp::DZ(), gp::Resolution() ) ) {
	  addFaceName( face, lC::STR::TRANSISTION );
	}
	else if ( direction.IsEqual( -gp::DZ(), gp::Resolution() ) ) {
	  addFaceName( face, lC::STR::SHANK );
	}
	else
	  cout << "Hmm. Unrecognized cone" << endl;
      }
      else
	cout << "Hmm. None of the above" << endl;
    }

    // Create the discrete representation. Deflection argument needs more thought...
    BRepMesh::Mesh( solid_, .25 );

    // Set the (cosmetic) ends of the construction axis
    axis_->setMinimum( -1.1 * ( length / 2 ) );
    axis_->setMaximum(  1.1 * ( length / 2 ) );

    // Set the value and dimension end points of the parameters.
    parameter( lC::STR::LENGTH ).setValueEnds( length,
			       gp_Pnt( 0, con_bottom_diameter/2, -length/2 ),
			       gp_Pnt( 0, rec_width/2, length/2 ) );

    parameter( lC::STR::REC_WIDTH ).setValueEnds( rec_width,
						  gp_Pnt( 0, -rec_width/2,length/2),
						  gp_Pnt( 0, rec_width/2,length/2));

    parameter( lC::STR::REC_LENGTH ).setValueEnds( rec_length,
			   gp_Pnt( 0,-rec_width/2,length/2),
			   gp_Pnt( 0, -rec_width/2, length/2 - rec_length ) );

    parameter( lC::STR::CON_LENGTH ).setValueEnds( con_length,
			   gp_Pnt( 0, -con_top_diameter/2, -length/2 + con_length ),
			   gp_Pnt( 0, -con_bottom_diameter/2, -length/2 ) );

    parameter( lC::STR::CON_TOP_DIAMETER ).setValueEnds( con_top_diameter,
			 gp_Pnt( 0, -con_top_diameter/2, -length/2 + con_length),
			 gp_Pnt( 0, con_top_diameter/2, -length/2 + con_length) );

    parameter( lC::STR::CON_BOTTOM_DIAMETER ).setValueEnds( con_bottom_diameter,
				    gp_Pnt( 0, -con_bottom_diameter/2, -length/2 ),
				    gp_Pnt( 0, con_bottom_diameter/2, -length/2) );

    // Set the position of the handles.
    handle( 0 ).setPosition( gp_Pnt( -rec_width/2, -rec_width/2, length/2 ) );
    handle( 1 ).setPosition( gp_Pnt( rec_width/2, -rec_width/2, length/2 ) );
    handle( 2 ).setPosition( gp_Pnt( rec_width/2, rec_width/2, length/2 ) );
    handle( 3 ).setPosition( gp_Pnt( -rec_width/2, rec_width/2, length/2 ) );
    handle( 4 ).setPosition( gp_Pnt( 0, -rec_width/2, length/2-rec_length ) );
    handle( 5 ).setPosition( gp_Pnt( 0, rec_width/2, length/2-rec_length ) );
    handle( 6 ).setPosition( gp_Pnt( 0, -con_top_diameter/2, -length/2 + con_length ) );
    handle( 7 ).setPosition( gp_Pnt( 0, con_top_diameter/2, -length/2 + con_length ) );
    handle( 8 ).setPosition( gp_Pnt( 0, -con_bottom_diameter/2, -length/2 ) );
    handle( 9 ).setPosition( gp_Pnt( 0, con_bottom_diameter/2, -length/2) );
  }

  void OCSolidRPPCON::updateParameter ( const QString& name, double value )
  {
    parameter( name ).setValue( value );

    double length = parameter( lC::STR::LENGTH ).value();
    double rec_length = parameter( lC::STR::REC_LENGTH ).value();
    double rec_width = parameter( lC::STR::REC_WIDTH ).value();
    double con_length = parameter( lC::STR::CON_LENGTH ).value();
    double con_top_diameter = parameter( lC::STR::CON_TOP_DIAMETER ).value();
    double con_bottom_diameter = parameter( lC::STR::CON_BOTTOM_DIAMETER ).value();

    construct( length, rec_length, rec_width, con_length, con_top_diameter,
	       con_bottom_diameter );

    emit modified();
  }

  void OCSolidRPPCON::updateParameters ( const std::map<QString,double>& parameters )
  {
    std::map<QString,double>::const_iterator p = parameters.begin();
    for ( ; p != parameters.end(); ++p ) {
      parameter( (*p).first ).setValue( (*p).second );
    }

    double length = parameter( lC::STR::LENGTH ).value();
    double rec_length = parameter( lC::STR::REC_LENGTH ).value();
    double rec_width = parameter( lC::STR::REC_WIDTH ).value();
    double con_length = parameter( lC::STR::CON_LENGTH ).value();
    double con_top_diameter = parameter( lC::STR::CON_TOP_DIAMETER ).value();
    double con_bottom_diameter = parameter( lC::STR::CON_BOTTOM_DIAMETER ).value();

    construct( length, rec_length, rec_width, con_length, con_top_diameter,
	       con_bottom_diameter );

    emit modified();
  }

  lC::ValidDelta OCSolidRPPCON::adjustHandle ( uint id, const Point& last_pnt,
					       Point& current_pnt )
  {
    Vector delta = current_pnt - last_pnt;

    delta[X] *= 2 * handle(id).xResizeSign();
    delta[Y] *= 2 * handle(id).yResizeSign();
    delta[Z] *= 2 * handle(id).zResizeSign();

    LengthUnit* length_unit = UnitsBasis::instance()->lengthUnit();
    lCPrecision* precision = length_unit->precision( UnitsBasis::instance()->format(),
					   UnitsBasis::instance()->precision() );
    double minimum_size = 1. / precision->unitsPerIn();
    lC::ValidDelta valid_delta ( delta[X] != 0, delta[Y] != 0, delta[Z] != 0 );

    double length = parameter( lC::STR::LENGTH ).value();
    double rec_length = parameter( lC::STR::REC_LENGTH ).value();
    double rec_width = parameter( lC::STR::REC_WIDTH ).value();
    double con_length = parameter( lC::STR::CON_LENGTH ).value();
    double con_top_diameter = parameter( lC::STR::CON_TOP_DIAMETER ).value();
    double con_bottom_diameter = parameter( lC::STR::CON_BOTTOM_DIAMETER ).value();
    std::map<QString,double> parameters;

    if ( id == 0 || id == 1 || id == 2 || id == 3 ) {
      if ( length + delta[Z] <= minimum_size ) {
	if ( length <= minimum_size )
	  valid_delta.dz_ok_ = false;
	else {
	  delta[Z] = minimum_size - length;
	  current_pnt[Z] = last_pnt[Z] + handle(id).zResizeSign() * delta[Z] / 2.;
	}
      }
      if ( valid_delta.dz_ok_ )
	parameters[lC::STR::LENGTH] = length + delta[Z];
    }
    if ( id == 0 || id == 1 || id == 2 || id == 3 ) {
      if ( fabs( delta[X] ) > fabs( delta[Y] ) ) {
	if ( rec_width + delta[X] <= minimum_size ) {
	  if ( rec_width <= minimum_size )
	    valid_delta.dx_ok_ = false;
	  else {
	    delta[X] = minimum_size - rec_width;
	    current_pnt[X] = last_pnt[X] + handle(id).xResizeSign() * delta[X] / 2.;
	  }
	}
	if ( valid_delta.dx_ok_ )
	  parameters[ lC::STR::REC_WIDTH] = rec_width + delta[X];
      }
      else {
	if ( rec_width + delta[Y] <= minimum_size ) {
	  if ( rec_width <= minimum_size )
	    valid_delta.dy_ok_ = false;
	  else {
	    delta[Y] = minimum_size - rec_width;
	    current_pnt[Y] = last_pnt[Y] + handle(id).yResizeSign() * delta[Y] / 2.;
	  }
	}
	if ( valid_delta.dy_ok_ )
	  parameters[lC::STR::REC_WIDTH] = rec_width + delta[Y];
      }
    }
    if ( id == 4 || id == 5 ) {
      delta[Z] /= 2;
      if ( rec_length + delta[Z] <= minimum_size ) {
	if ( rec_length <= minimum_size )
	  valid_delta.dz_ok_ = false;
	else {
	  delta[Z] = minimum_size - rec_length;
	  current_pnt[Z] = last_pnt[Z] + handle(id).zResizeSign() * delta[Z];
	}
      }
      if ( valid_delta.dz_ok_ )
	parameters[ lC::STR::REC_LENGTH] = rec_length + delta[Z];
    }
    if ( id == 6 || id == 7 ) {
      delta[Z] /= 2;
      if ( con_length + delta[Z] <= minimum_size ) {
	if ( con_length <= minimum_size )
	  valid_delta.dz_ok_ = false;
	else {
	  delta[Z] = minimum_size - con_length;
	  current_pnt[Z] = last_pnt[Z] + handle(id).zResizeSign() * delta[Z];
	}
      }
      if ( valid_delta.dz_ok_ )
	parameters[lC::STR::CON_LENGTH] = con_length + delta[Z];

      if ( con_top_diameter + delta[Y] <= minimum_size ) {
	if ( con_top_diameter <= minimum_size )
	  valid_delta.dy_ok_ = false;
	else {
	  delta[Y] = minimum_size - con_top_diameter;
	  current_pnt[Y] = last_pnt[Y] + handle(id).yResizeSign() * delta[Y] / 2.;
	}
      }
      if ( valid_delta.dy_ok_ )
	parameters[lC::STR::CON_TOP_DIAMETER] = con_top_diameter + delta[Y];
    }

    if ( id == 8 || id == 9 ) {
      if ( length + delta[Z] <= minimum_size ) {
	if ( length <= minimum_size )
	  valid_delta.dz_ok_ = false;
	else {
	  delta[Z] = minimum_size - length;
	  current_pnt[Z] = last_pnt[Z] + handle(id).zResizeSign() * delta[Z] / 2.;
	}
      }
      if ( valid_delta.dz_ok_ )
	parameters[lC::STR::LENGTH] = length + delta[Z];

      if ( con_bottom_diameter + delta[Y] <= minimum_size ) {
	if ( con_bottom_diameter <= minimum_size )
	  valid_delta.dy_ok_ = false;
	else {
	  delta[Y] = minimum_size - con_bottom_diameter;
	  current_pnt[Y] = last_pnt[Y] + handle(id).yResizeSign() * delta[Y] / 2.;
	}
      }
      if ( valid_delta.dy_ok_ )
	parameters[lC::STR::CON_BOTTOM_DIAMETER] = con_bottom_diameter + delta[Y];
    }

    if ( valid_delta.dx_ok_ || valid_delta.dy_ok_ || valid_delta.dz_ok_ )
      updateParameters( parameters );

    return valid_delta;
  }

  void OCSolidRPPCON::write ( QDomElement& xml_rep ) const
  {
    if ( !isValid() ) return;

    QDomDocument document = xml_rep.ownerDocument();
    QDomElement solid_element = document.createElement( lC::STR::SOLID );
    solid_element.setAttribute( lC::STR::NAME, name() );

    if ( material() != 0 )
      solid_element.setAttribute( lC::STR::MATERIAL, material()->commonName() );

    QDomElement rppcon_element = document.createElement( lC::STR::RPPCON );

    std::map<QString,Parameter>::const_iterator parameter = parametersBegin();
    for ( ; parameter != parametersEnd(); ++parameter )
      rppcon_element.setAttribute( (*parameter).first,
				   lC::format( (*parameter).second.value() ) );

    solid_element.appendChild( rppcon_element );
    xml_rep.appendChild( solid_element );
  }

  // A specific solid: a rectangular solid which transistions into a
  // pyramid (like a leg tapered on two sides).

  OCSolidRPPPYR::OCSolidRPPPYR ( uint id, const QString& name,
				 double length, double rec_length, double rec_width,
				 double pyr_width,
				 Page* parent )
    : OCSolid( id, name, parent )
  {
    create( length, rec_length, rec_width, pyr_width );
  }

  OCSolidRPPPYR::OCSolidRPPPYR ( uint id, const QString& name,
				 const QDomElement& xml_rep, Page* parent )
    : OCSolid( id, name, parent )
  {
    double length = xml_rep.attribute( lC::STR::LENGTH ).toDouble();
    double rec_length = xml_rep.attribute(lC::STR::REC_LENGTH).toDouble();
    double rec_width = xml_rep.attribute( lC::STR::REC_WIDTH ).toDouble();
    double pyr_width = xml_rep.attribute( lC::STR::PYR_WIDTH ).toDouble();

    create( length, rec_length, rec_width, pyr_width );
  }

  void OCSolidRPPPYR::create( double length, double rec_length, double rec_width,
			      double pyr_width )
  {
    // Create the basic parameters (here, w/o dimension data)
    addParameter( lC::STR::LENGTH,
		  Parameter( lC::STR::LENGTH_GRAIN, Parameter::LINEAR, -gp::DY() ) );
    addParameter( lC::STR::REC_LENGTH,
		  Parameter( "Top length", Parameter::LINEAR, gp::DY() ) );
    addParameter( lC::STR::REC_WIDTH,
		  Parameter( "Top width", Parameter::LINEAR, gp::DZ() ) );
    addParameter( lC::STR::PYR_WIDTH,
		  Parameter( "Bottom width", Parameter::LINEAR, -gp::DZ() ) );

    // Create the resize handles (here, w/o placement, just the orientation flags)
    // (Top handles)
    addResizeHandle( 0, HandleData( gp_XYZ( -1, -1,  1 ) ) );
    addResizeHandle( 1, HandleData( gp_XYZ(  1, -1,  1 ) ) );
    addResizeHandle( 2, HandleData( gp_XYZ(  1,  1,  1 ) ) );
    addResizeHandle( 3, HandleData( gp_XYZ( -1,  1,  1 ) ) );
    // (Bottom handles)
    addResizeHandle( 4, HandleData( gp_XYZ(  0,  0, -1 ) ) );
    addResizeHandle( 5, HandleData( gp_XYZ(  0,  0, -1 ) ) );
    addResizeHandle( 6, HandleData( gp_XYZ(  0,  0, -1 ) ) );
    addResizeHandle( 7, HandleData( gp_XYZ(  1,  1, -1 ) ) );
    // (Transition handles)
    addResizeHandle( 8, HandleData( gp_XYZ(  0,  0, -1 ) ) );
    addResizeHandle( 9, HandleData( gp_XYZ(  0,  0, -1 ) ) );
    addResizeHandle( 10, HandleData( gp_XYZ(  0,  0, -1 ) ) );

    // Setup the (cosmetic) relationships between faces and handles.
    QVector<uint> handles;
    handles.push_back( 1 );
    handles.push_back( 2 );
    handles.push_back( 8 );
    handles.push_back( 9 );
    setFaceHandles( lC::STR::RIGHT, handles ); handles.clear();
    handles.push_back( 0 );
    handles.push_back( 3 );
    handles.push_back( 4 );
    handles.push_back( 6 );
    handles.push_back( 10 );
    setFaceHandles( lC::STR::LEFT, handles ); handles.clear();
    handles.push_back( 2 );
    handles.push_back( 3 );
    handles.push_back( 9 );
    handles.push_back( 10 );
    setFaceHandles( lC::STR::FRONT, handles ); handles.clear();
    handles.push_back( 0 );
    handles.push_back( 1 );
    handles.push_back( 4 );
    handles.push_back( 5 );
    handles.push_back( 8 );
    setFaceHandles( lC::STR::BACK, handles ); handles.clear();
    handles.push_back( 0 );
    handles.push_back( 1 );
    handles.push_back( 2 );
    handles.push_back( 3 );
    setFaceHandles( lC::STR::TOP, handles ); handles.clear();
    handles.push_back( 4 );
    handles.push_back( 5 );
    handles.push_back( 6 );
    handles.push_back( 7 );
    setFaceHandles( lC::STR::BOTTOM, handles ); handles.clear();
    handles.push_back( 5 );
    handles.push_back( 7 );
    handles.push_back( 8 );
    handles.push_back( 9 );
    setFaceHandles( lC::STR::RIGHT_TAPER, handles ); handles.clear();
    handles.push_back( 6 );
    handles.push_back( 7 );
    handles.push_back( 9 );
    handles.push_back( 10 );
    setFaceHandles( lC::STR::FRONT_TAPER, handles ); handles.clear();

    // Construct the solid and setup the dimension dependent for the parameters
    // and handles. (This allows all parameter dependent values to redone with one
    // routine.)
    construct( length, rec_length, rec_width, pyr_width );

    // Somehow, the grain orientation has ended up being here.
    // However, the default grain orientation is OK for the RPPPYR.

    // OK, ready to roll.
    setValid( true );
    setComplete( true );
  }

  void OCSolidRPPPYR::construct ( double length, double rec_length,
				  double rec_width, double pyr_width )
  {
    try {
      // The Leg's axial center is taken to the be the origin (note: it's not
      // the centroid since it wouldn't necessarily balance there.)

      TopoDS_Shape rpp = BRepPrimAPI_MakeBox( gp_Pnt( -rec_width/2, -rec_width/2,
						      -length/2 ),
					      rec_width, rec_width, length).Shape();

      // Construct a HalfSpace tool to taper the rectangle. Note that this needs
      // to be big enough to span the RPP. An infinitely large face would do,
      // but that would appear (from the documentation) not to work.

      // 1. Start with a line such as the user might draw on the face to taper.

      gce_MakeLin gce_right_edge_line( gp_Pnt( -rec_width/2 + pyr_width, 0,
					       -length/2),
				       gp_Pnt( rec_width/2, 0,
					       length/2 - rec_length ) );

      // 2. Create planes some distance from the ends and sides of the leg.

      gp_Pnt bottom_pnt( 0, 0, -length/2 - 1 );
      gp_Ax3 bottom_plane_csys( bottom_pnt, gp::DZ() );
      Handle( Geom_Plane ) bottom_geom_plane = new Geom_Plane( bottom_plane_csys );

      gp_Pnt top_pnt( 0, 0, +length/2 + 1 );
      gp_Ax3 top_plane_csys( top_pnt, gp::DZ() );
      Handle( Geom_Plane ) top_geom_plane = new Geom_Plane( top_plane_csys );

      gp_Pnt front_pnt( 0, rec_width + 1, 0 );
      gp_Ax3 front_plane_csys( front_pnt, gp::DY() );
      Handle( Geom_Plane ) front_geom_plane = new Geom_Plane( front_plane_csys );

      gp_Pnt back_pnt( 0, -rec_width - 1, 0 );
      gp_Ax3 back_plane_csys( back_pnt, gp::DY() );
      Handle( Geom_Plane ) back_geom_plane = new Geom_Plane( back_plane_csys );

      // 3. Compute the plane through the drawn line and normal to the front face
      // (the one on which the line was "drawn")

      gp_Dir cut_plane_normal = gce_right_edge_line.Value().Direction() ^ gp::DY();
      gp_Ax3 cut_plane_csys( gp_Pnt( -rec_width/2 + pyr_width, 0, -length/2),
			     cut_plane_normal );

      Handle( Geom_Plane ) cut_geom_plane = new Geom_Plane( cut_plane_csys );

      // 4. Make a wire out of the intersection of the cut plane with the
      // bounding planes.

      BRepBuilderAPI_MakePolygon polygon;

      GeomAPI_IntSS plane_intersector;
      GeomAPI_IntCS line_intersector;

      plane_intersector.Perform( cut_geom_plane, bottom_geom_plane, 1e-7 );
      line_intersector.Perform( plane_intersector.Line(1), back_geom_plane );
      polygon.Add( line_intersector.Point(1) );

      plane_intersector.Perform( cut_geom_plane, back_geom_plane, 1e-7 );
      line_intersector.Perform( plane_intersector.Line(1), top_geom_plane );
      polygon.Add( line_intersector.Point(1) );

      plane_intersector.Perform( cut_geom_plane, top_geom_plane, 1e-7 );
      line_intersector.Perform( plane_intersector.Line(1), front_geom_plane );
      polygon.Add( line_intersector.Point(1) );

      plane_intersector.Perform( cut_geom_plane, front_geom_plane, 1e-7 );
      line_intersector.Perform( plane_intersector.Line(1), bottom_geom_plane );
      polygon.Add( line_intersector.Point(1) );

      polygon.Close();

      TopoDS_Face face = BRepBuilderAPI_MakeFace( polygon.Wire() );

      // 5. Make a Ha'Space "solid" out of our face.

      TopoDS_Solid cut_tool =
	BRepPrimAPI_MakeHalfSpace( face, gp_Pnt( rec_width/2, 0,-length/2)).Solid();

      // 6. Hack off the side of the leg.

      BRepAlgoAPI_Cut cut( rpp, cut_tool );

      // 7. Rotate the cut plane 90 degrees to do the back side of the leg.
      gp_Trsf transform;
      transform.SetRotation( gp::OZ(), M_PI/2 );
      BRepBuilderAPI_Transform rotated_cut( cut_tool, transform );

      BRepAlgoAPI_Cut cut2( cut.Shape(), rotated_cut );

      // Note that since the Cut operation generally returns a
      // Compound shape, we need to extract just the TopoDS_Solid
      // through the mapper.
      TopTools_IndexedMapOfShape solids;
      TopExp::MapShapes( cut2.Shape(), TopAbs_SOLID, solids );
      if ( solids.Extent() == 1 )
	solid_ = TopoDS::Solid( solids(1) );
    }
    catch ( ... ) {
      cout << "Construction of RPPPYR failed" << endl;
    }
    // (Try to) figure out which face is which so that they can be given
    // unique names. Sigh. It seems that it would be a lot easier to
    // construct these things ourselves rather than use the BRepPrim routines...

    clearFaceNames();
    TopExp_Explorer faces( solid_, TopAbs_FACE );

    for ( ; faces.More(); faces.Next() ) {
      TopoDS_Face face = TopoDS::Face( faces.Current() );
      bool reverse( face.Orientation() == TopAbs_REVERSED );
      Handle( Geom_Surface ) surface = BRep_Tool::Surface( face );
      if ( surface->DynamicType() == STANDARD_TYPE( Geom_Plane ) ) {
	Handle( Geom_Plane ) plane = Handle(Geom_Plane)::DownCast( surface );
	gp_Dir normal = plane->Pln().Axis().Direction();
	if ( reverse ) normal.Reverse();

	if ( normal.IsEqual( gp::DX(), gp::Resolution() ) ) {
	  addFaceName( face, lC::STR::RIGHT );
	}
	else if ( normal.IsEqual( -gp::DX(), gp::Resolution() ) ) {
	  addFaceName( face, lC::STR::LEFT );
	}
	else if ( normal.IsEqual( gp::DY(), gp::Resolution() ) ) {
	  addFaceName( face, lC::STR::FRONT );
	}
	else if ( normal.IsEqual( -gp::DY(), gp::Resolution() ) ) {
	  addFaceName( face, lC::STR::BACK );
	}
	else if ( normal.IsEqual( gp::DZ(), gp::Resolution() ) ) {
	  addFaceName( face, lC::STR::TOP );
	}
	else if ( normal.IsEqual( -gp::DZ(), gp::Resolution() ) ) {
	  addFaceName( face, lC::STR::BOTTOM );
	}
	else if ( normal * gp::DX() > normal * gp::DY() ) {
	  addFaceName( face, lC::STR::RIGHT_TAPER );
	}
	else if ( normal * gp::DY() > normal * gp::DX() ) {
	  addFaceName( face, lC::STR::FRONT_TAPER );
	}
	else
	  cout << "Hmm. Unrecognized plane" << endl;
      }
      else
	cout << "Hmm. None of the above" << endl;
    }

    // Create the discrete representation. Deflection argument needs more thought...
    BRepMesh::Mesh( solid_, 1. );

    // Set the value and dimension end points of the parameters.
    parameter( lC::STR::LENGTH ).setValueEnds( length,
					       gp_Pnt( 0, -rec_width/2, -length/2 ),
					       gp_Pnt( 0, -rec_width/2, length/2 ));
    parameter( lC::STR::REC_LENGTH ).setValueEnds( rec_length,
				   gp_Pnt( 0, rec_width/2, (length/2 - rec_length)),
				   gp_Pnt( 0, rec_width/2, length/2 ) );
    parameter( lC::STR::REC_WIDTH ).setValueEnds( rec_width,
						  gp_Pnt( 0, -rec_width/2,length/2),
						  gp_Pnt( 0, rec_width/2,length/2));

    parameter( lC::STR::PYR_WIDTH ).setValueEnds( pyr_width,
				  gp_Pnt( 0,-rec_width/2,-length/2),
				  gp_Pnt( 0, -rec_width/2 + pyr_width, -length/2 ));

    // Set the position of the handles.
    // (Top handles)
    handle( 0 ).setPosition( gp_Pnt( -rec_width/2, -rec_width/2, length/2 ) );
    handle( 1 ).setPosition( gp_Pnt( rec_width/2, -rec_width/2, length/2 ) );
    handle( 2 ).setPosition( gp_Pnt( rec_width/2, rec_width/2, length/2 ) );
    handle( 3 ).setPosition( gp_Pnt( -rec_width/2, rec_width/2, length/2 ) );
    // (Bottom handles)
    handle( 4 ).setPosition( gp_Pnt( -rec_width/2, -rec_width/2, -length/2 ) );
    handle( 5 ).setPosition( gp_Pnt( -rec_width/2+pyr_width, -rec_width/2, -length/2 ) );
    handle( 6 ).setPosition( gp_Pnt( -rec_width/2, -rec_width/2+pyr_width, -length/2 ) );
    
    handle( 7 ).setPosition( gp_Pnt( -rec_width/2+pyr_width, -rec_width/2+pyr_width, -length/2 ) );
    // (Transition handles)
    handle( 8 ).setPosition( gp_Pnt( rec_width/2, -rec_width/2, length/2 - rec_length ) );
    handle( 9 ).setPosition( gp_Pnt( rec_width/2, rec_width/2, length/2 - rec_length ) );
    handle( 10 ).setPosition( gp_Pnt( -rec_width/2, rec_width/2, length/2 - rec_length ) );
  }

  void OCSolidRPPPYR::updateParameter ( const QString& name, double value )
  {
    parameter( name ).setValue( value );

    double length = parameter( lC::STR::LENGTH ).value();
    double rec_length = parameter( lC::STR::REC_LENGTH ).value();
    double rec_width = parameter( lC::STR::REC_WIDTH ).value();
    double pyr_width = parameter( lC::STR::PYR_WIDTH ).value();

    construct( length, rec_length, rec_width, pyr_width );
    //    cout << now() << path() << " modified [before emit]" << endl;
    emit modified();
    //    cout << now() << path() << " modified [after emit]" << endl;
  }

  void OCSolidRPPPYR::updateParameters ( const std::map<QString,double>& parameters )
  {
    std::map<QString,double>::const_iterator p = parameters.begin();
    for ( ; p != parameters.end(); ++p ) {
      parameter( (*p).first ).setValue( (*p).second );
    }

    double length = parameter( lC::STR::LENGTH ).value();
    double rec_length = parameter( lC::STR::REC_LENGTH ).value();
    double rec_width = parameter( lC::STR::REC_WIDTH ).value();
    double pyr_width = parameter( lC::STR::PYR_WIDTH ).value();

    construct( length, rec_length, rec_width, pyr_width );

    emit modified();
  }

  lC::ValidDelta OCSolidRPPPYR::adjustHandle ( uint id, const Point& last_pnt,
					       Point& current_pnt )
  {
    Vector delta = current_pnt - last_pnt;

    delta[X] *= 2 * handle(id).xResizeSign();
    delta[Y] *= 2 * handle(id).yResizeSign();
    delta[Z] *= 2 * handle(id).zResizeSign();

    LengthUnit* length_unit = UnitsBasis::instance()->lengthUnit();
    lCPrecision* precision = length_unit->precision( UnitsBasis::instance()->format(),
					   UnitsBasis::instance()->precision() );
    double minimum_size = 1. / precision->unitsPerIn();
    lC::ValidDelta valid_delta ( delta[X] != 0, delta[Y] != 0, delta[Z] != 0 );

    double length = parameter( lC::STR::LENGTH ).value();
    double rec_length = parameter( lC::STR::REC_LENGTH ).value();
    double rec_width = parameter( lC::STR::REC_WIDTH ).value();
    double pyr_width = parameter( lC::STR::PYR_WIDTH ).value();
    std::map<QString,double> parameters;

    if ( id == 0 || id == 1 || id == 2 || id == 3 ||
	 id == 4 || id == 5 || id == 6 || id == 7 ) {
      if ( length + delta[Z] <= minimum_size ) {
	if ( length <= minimum_size )
	  valid_delta.dz_ok_ = false;
	else {
	  delta[Z] = minimum_size - length;
	  current_pnt[Z] = last_pnt[Z] + handle(id).zResizeSign() * delta[Z]/2;
	}
      }
      if ( valid_delta.dz_ok_ )
	parameters[lC::STR::LENGTH] = length + delta[Z];
    }
    if ( id == 0 || id == 1 || id == 2 || id == 3 ) {
      if ( fabs( delta[X] ) > fabs( delta[Y] ) ) {
	if ( rec_width + delta[X] <= minimum_size ) {
	  if ( rec_width <= minimum_size ) {
	    valid_delta.dx_ok_ = false;
	  }
	  else {
	    delta[X] = minimum_size - rec_width;
	    current_pnt[X] = last_pnt[X] + handle(id).xResizeSign() * delta[X]/2;
	  }
	}
	if ( valid_delta.dx_ok_ )
	  parameters[lC::STR::REC_WIDTH] = rec_width + delta[X];
      }
      else {
	if ( rec_width + delta[Y] <= minimum_size ) {
	  if ( rec_width <= minimum_size )
	    valid_delta.dy_ok_ = false;
	  else {
	    delta[Y] = minimum_size - rec_width;
	    current_pnt[Y] = last_pnt[Y] + handle(id).yResizeSign() * delta[Y]/2;
	  }
	}
	if ( valid_delta.dy_ok_ )
	  parameters[lC::STR::REC_WIDTH] = rec_width + delta[Y];
      }
    }
    else if ( id == 4 || id == 5 || id == 6 || id == 7 ) {
      if ( pyr_width + delta[Y] <= minimum_size ) {
	if ( pyr_width <= minimum_size )
	  valid_delta.dy_ok_ = false;
	else {
	  delta[Y] = minimum_size - pyr_width;
	  current_pnt[Y] = last_pnt[Y] + handle(id).yResizeSign() * delta[Y]/2;
	}
      }
      if ( valid_delta.dy_ok_ )
	parameters[lC::STR::PYR_WIDTH] = pyr_width + delta[Y];
    }
    if ( id == 8 || id == 9 || id == 10 ) {
      delta[Z] /= 2;
      if ( rec_length + delta[Z] <= minimum_size ) {
	if ( rec_length <= minimum_size )
	  valid_delta.dz_ok_ = false;
	else {
	  delta[Z] = minimum_size - rec_length;
	  current_pnt[Z] = last_pnt[Z] + handle(id).zResizeSign() * delta[Z];
	}
      }
      if ( valid_delta.dz_ok_ ) {
	parameters[lC::STR::REC_LENGTH] = rec_length + delta[Z];
      }
    }

    if ( valid_delta.dx_ok_ || valid_delta.dy_ok_ || valid_delta.dz_ok_ ) {
      try { updateParameters( parameters ); }
      catch (...) {
	cout << "Hmm. I caught an exception while trying to resize the RPPPYR"
	     << endl;
      }
    }

    return valid_delta;
  }

  void OCSolidRPPPYR::write ( QDomElement& xml_rep ) const
  {
    if ( !isValid() ) return;

    QDomDocument document = xml_rep.ownerDocument();
    QDomElement solid_element = document.createElement( lC::STR::SOLID );
    solid_element.setAttribute( lC::STR::NAME, name() );

    if ( material() != 0 )
      solid_element.setAttribute( lC::STR::MATERIAL, material()->commonName() );

    QDomElement rpppyr_element = document.createElement( lC::STR::RPPPYR );

    std::map<QString,Parameter>::const_iterator parameter = parametersBegin();
    for ( ; parameter != parametersEnd(); ++parameter )
      rpppyr_element.setAttribute( (*parameter).first,
				   lC::format( (*parameter).second.value() ) );
    
    solid_element.appendChild( rpppyr_element );
    xml_rep.appendChild( solid_element );
  }

  // Make a revolved bspline with cylindrical base: like a turned knob.

  OCSolidBSPCYL::OCSolidBSPCYL ( uint id, const QString& name,
				 double length,
				 double bsp_diameter, double cyl_diameter,
				 double cyl_length,
				 Page* parent )
    : OCSolid( id, name, parent )
  {
    create( length, bsp_diameter, cyl_diameter, cyl_length );
  }

  OCSolidBSPCYL::OCSolidBSPCYL ( uint id, const QString& name,
				 const QDomElement& xml_rep, Page* parent )
    : OCSolid( id, name, parent )
  {
    double length = xml_rep.attribute( lC::STR::LENGTH ).toDouble();
    double bsp_diameter = xml_rep.attribute( lC::STR::BSP_DIAMETER ).toDouble();
    double cyl_diameter = xml_rep.attribute( lC::STR::CYL_DIAMETER ).toDouble();
    double cyl_length = xml_rep.attribute( lC::STR::CYL_LENGTH ).toDouble();

    create( length, bsp_diameter, cyl_diameter, cyl_length );
  }

  void OCSolidBSPCYL::create ( double length, double bsp_diameter,
			       double cyl_diameter, double cyl_length )
  {
    // Create the basic parameters (here, w/o dimension data)
    addParameter( lC::STR::LENGTH, Parameter( lC::STR::LENGTH_GRAIN,
					      Parameter::LINEAR, gp::DX() ) );
    addParameter( lC::STR::BSP_DIAMETER, Parameter( lC::STR::DIAMETER,
						    Parameter::DIAMETER, gp::DZ()));
    addParameter( lC::STR::CYL_DIAMETER, Parameter( lC::STR::BASE_DIAMETER,
						    Parameter::DIAMETER,-gp::DZ()));
    addParameter( lC::STR::CYL_LENGTH, Parameter( lC::STR::BASE_LENGTH,
						  Parameter::LINEAR, -gp::DX()));

    // Create the resize handles (here, w/o placement, just the orientation flags)
    addResizeHandle( 0, HandleData( gp_XYZ( -1, 0, -1 ) ) );
    addResizeHandle( 1, HandleData( gp_XYZ(  1, 0, -1 ) ) );
    addResizeHandle( 2, HandleData( gp_XYZ( -1, 0, 1 ) ) );
    addResizeHandle( 3, HandleData( gp_XYZ(  1, 0, 1 ) ) );
    addResizeHandle( 4, HandleData( gp_XYZ( -1, 0, 1 ) ) );
    addResizeHandle( 5, HandleData( gp_XYZ(  1, 0, 1 ) ) );

    // Setup the (cosmetic) relationships between faces and handles.
    QVector<uint> handles;
    handles.push_back( 0 );
    handles.push_back( 1 );
    handles.push_back( 4 );
    handles.push_back( 5 );
    setFaceHandles( lC::STR::BASE, handles ); handles.clear();
    handles.push_back( 2 );
    handles.push_back( 3 );
    handles.push_back( 4 );
    handles.push_back( 5 );
    setFaceHandles( lC::STR::TURNING, handles ); handles.clear();
    // This surface is an artifact
    setFaceHandles( lC::STR::TOP, handles ); handles.clear();
    handles.push_back( 0 );
    handles.push_back( 1 );
    setFaceHandles( lC::STR::BOTTOM, handles ); handles.clear();

    // Although it is really not used in the construction per se, add an axis
    // construction datum.
    axis_ = new Axis( 0, lC::STR::AXIS, parent() );
    addDatum( lC::STR::AXIS, axis_ );

    // Construct the solid and setup the dimension dependent for the parameters
    // and handles. (This allows all parameter dependent values to redone with one
    // routine.)
    construct( length, bsp_diameter, cyl_diameter, cyl_length );

    // Somehow, the grain orientation has ended up being here.
    // However, the default grain orientation is OK for the BSPCYL.

    // OK, ready to roll.
    setValid( true );
    setComplete( true );
  }

  void OCSolidBSPCYL::construct ( double length, double bsp_diameter,
				  double cyl_diameter, double cyl_length )
  {
    try {
#if 0
      // This is the original set of points from digitizing the knob in The GIMP.
      const double points[16][2] = {
		      {0.28, 0.00}, {0.21, 0.08},
	{0.21, 0.16}, {0.21, 0.28}, {0.21, 0.40},
	{0.30, 0.52}, {0.37, 0.56}, {0.43, 0.61},
	{0.57, 0.68}, {0.57, 0.74}, {0.57, 0.81},
	{0.41, 0.90}, {0.33, 0.93}, {0.24, 0.97},
	{0.14, 1.00}, {0.00, 1.00} };
#endif
      // 1. Construct a BSpline from a set of consecutive Bezier curves.
      //    The trick here is to scale the template Bezier curves to the
      //    diameter and length. The BSpline itself is constructed in the
      //    XZ plane: Z is the axis of rotation and X is the offset from
      //    the axis.

      // These are the parameters of the template Beziers. They fill
      // a [s,t]=[0..1,0..1] parametric space. The BSpline will interpolate
      // these points after they are scaled to the right size. (s corresponds
      // to X and t corresponds to Z.)
      static const double points[6][2] = {
	{ 1, 0 }, { 1./2., 1./3. }, { 3./4., 1./2. }, { 1, 3./4. }, { 5./8., 0.95 },
	{0.1, 1} };
      // Each point has a tangent associated with it. Actually there are
      // two tangents: one for the Bezier to the left and one for the
      // Bezier to the right. (As near as I can tell, though, the two
      // directions are always 180 degrees apart. This seems to be the nature
      // of the The GIMP's Bezier curves.)
      static const double tangents[6] =
      { 129 * M_PI/180., 90 * M_PI/180., 33 * M_PI/180.,
	90 * M_PI/180., 157 * M_PI/180., 180 * M_PI/180. };
      // These values determine the magnitude of the tangent vector at each point.
      static const double scales[6] =
      { 0.105, .121, .0782, .0656, 0.0926, .138 };

      TColgp_Array1OfPnt bezier_pnts(1,4);

      Convert_CompBezierCurvesToBSplineCurve bez2bsp;

      gp_Vec center( 0, 0, cyl_length - length/2 );// Translation to axial center

      for ( int i = 0; i < 5; i++ ) {
	// The X values of the interpolated points are scaled by a linear
	// function such that the first point at s=0 is "cyl_diameter" distant
	// from the Z axis and the point at s=3/4 is "bsp_diameter" distant from
	// the Z axis.
	double y0 = points[i][0] *
	  ( ( bsp_diameter - cyl_diameter ) * points[i][1] * 4. / 3. + cyl_diameter)
	  / 2;
	double y3 = points[i+1][0] *
	  ( ( bsp_diameter - cyl_diameter ) * points[i+1][1] * 4. / 3.+cyl_diameter)
	  / 2;
	// The z values are just scaled by the length of the bspline section.
	double z0 = points[i][1] * ( length - cyl_length );
	double z3 = points[i+1][1] * ( length - cyl_length );
	gp_Pnt p0( y0, 0, z0 );
	gp_Pnt p3( y3, 0, z3 );
	// The intermediate points of the Beziers are just offset from their
	// respective end points by the tangent vector.
	gp_Vec v0( cos(tangents[i]), 0, sin(tangents[i]) );
	gp_Vec v3( cos(tangents[i+1]+M_PI), 0, sin(tangents[i+1]+M_PI) );
	v0.Scale( ( length - cyl_length ) * scales[i] );
	v3.Scale( ( length - cyl_length ) * scales[i+1] );
	gp_Pnt p1( p0.Translated( v0 ) );
	gp_Pnt p2( p3.Translated( v3 ) );

	// Finally, all the points are translated so that the solid is centered
	// along its Z axis length.
	bezier_pnts(1) = p0.Translated( center );
	bezier_pnts(2) = p1.Translated( center );
	bezier_pnts(3) = p2.Translated( center );
	bezier_pnts(4) = p3.Translated( center );

	bez2bsp.AddCurve( bezier_pnts );
      }

      bez2bsp.Perform();

      TColgp_Array1OfPnt poles(1, bez2bsp.NbPoles() );
      TColStd_Array1OfReal knots(1, bez2bsp.NbKnots() );
      TColStd_Array1OfInteger mults(1, bez2bsp.NbKnots() );

      bez2bsp.Poles( poles );
      bez2bsp.KnotsAndMults( knots, mults );

      Handle(Geom_BSplineCurve) spline = new Geom_BSplineCurve( poles, knots, mults,
								bez2bsp.Degree() );

      // 2. Convert the bspline into a solid of revolution.
      BRepPrimAPI_MakeRevolution revolver( spline );

      // 3. Create the cylindrical base.
      gp_Ax2 axis = gp::XOY();
      axis.SetDirection( -axis.Direction() );
      axis.Translate( center );
      BRepPrimAPI_MakeCylinder base( axis, cyl_diameter/2, cyl_length );

      // 4. Stick them together.
      BRepAlgoAPI_Fuse knob( revolver.Shape(), base.Shape() );

      // Note that since the Fuse operation generally returns a
      // Compound shape, we need to extract just the TopoDS_Solid
      // through the mapper.
      TopTools_IndexedMapOfShape solids;
      TopExp::MapShapes( knob.Shape(), TopAbs_SOLID, solids );
      if ( solids.Extent() == 1 )
	solid_ = TopoDS::Solid( solids(1) );
    }
    catch ( ... ) {
      cout << "Construction of BSPCYL failed" << endl;
    }
    // (Try to) figure out which face is which so that they can be given
    // unique names. Sigh. It seems that it would be a lot easier to
    // construct these things ourselves rather than use the BRepPrim routines...

    clearFaceNames();
    TopExp_Explorer faces( solid_, TopAbs_FACE );

    for ( ; faces.More(); faces.Next() ) {
      TopoDS_Face face = TopoDS::Face( faces.Current() );
      bool reverse( face.Orientation() == TopAbs_REVERSED );
      Handle( Geom_Surface ) surface = BRep_Tool::Surface( face );
      if ( surface->DynamicType() == STANDARD_TYPE( Geom_Plane ) ) {
	Handle( Geom_Plane ) plane = Handle(Geom_Plane)::DownCast( surface );
	gp_Dir normal = plane->Pln().Axis().Direction();
	if ( reverse ) normal.Reverse();
	if ( normal.IsEqual( gp::DZ(), gp::Resolution() ) ) {
	  addFaceName( face, lC::STR::TOP );
	}
	else if ( normal.IsEqual( -gp::DZ(), gp::Resolution() ) ) {
	  addFaceName( face, lC::STR::BOTTOM );
	}
	else
	  cout << "Hmm. Unrecognized plane" << endl;
      }
      else if ( surface->DynamicType() == STANDARD_TYPE( Geom_CylindricalSurface )){
	addFaceName( face, lC::STR::BASE );
      }
      else if ( surface->DynamicType() == STANDARD_TYPE( Geom_SurfaceOfRevolution)){
	addFaceName( face, lC::STR::TURNING );
      }
      else
	cout << "Hmm. None of the above" << endl;
    }

    // Create the discrete representation. Deflection argument needs more thought...
    BRepMesh::Mesh( solid_, .0125 );

    // Set the (cosmetic) ends of the construction axis
    axis_->setMinimum( -1.1 * ( length / 2 ) );
    axis_->setMaximum(  1.1 * ( length / 2 ) );

    // Set the value and dimension end points of the parameters.
    parameter( lC::STR::LENGTH ).setValueEnds( length,
					       gp_Pnt( cyl_diameter/2, 0,-length/2),
					       gp_Pnt( 0, 0, length/2 ));
    parameter( lC::STR::BSP_DIAMETER ).setValueEnds( bsp_diameter,
		     gp_Pnt( -bsp_diameter/2, 0, (length - cyl_length) * 3./4. -
			     ( length/2 - cyl_length ) ),
		     gp_Pnt(  bsp_diameter/2, 0, (length - cyl_length) * 3./4. -
			      ( length/2 - cyl_length ) ) );
    parameter( lC::STR::CYL_DIAMETER ).setValueEnds( cyl_diameter,
		      gp_Pnt( -cyl_diameter/2, 0, -length/2 ),
		      gp_Pnt(  cyl_diameter/2, 0, -length/2 ) );
    parameter( lC::STR::CYL_LENGTH ).setValueEnds( cyl_length,
		      gp_Pnt( -cyl_diameter/2, 0, -length/2 ),
		      gp_Pnt(  cyl_diameter/2, 0, -length/2 + cyl_length ) );

    // Set the position of the handles.
    handle( 0 ).setPosition( gp_Pnt( -cyl_diameter/2, 0, -length/2 ) );
    handle( 1 ).setPosition( gp_Pnt(  cyl_diameter/2, 0, -length/2 ) );
    handle( 2 ).setPosition( gp_Pnt( -bsp_diameter/2, 0,
				       ( length - cyl_length ) * 3. / 4. -
				       ( length/2 - cyl_length ) ) );
    handle( 3 ).setPosition( gp_Pnt(  bsp_diameter/2, 0,
				       ( length - cyl_length ) * 3. / 4. -
					( length/2 - cyl_length ) ) );
    handle( 4 ).setPosition( gp_Pnt( -cyl_diameter/2, 0, cyl_length - length/2 ));
    handle( 5 ).setPosition( gp_Pnt(  cyl_diameter/2, 0, cyl_length - length/2 ));
  }

  void OCSolidBSPCYL::updateParameter ( const QString& name, double value )
  {
    parameter( name ).setValue( value );

    double length = parameter( lC::STR::LENGTH ).value();
    double bsp_diameter = parameter( lC::STR::BSP_DIAMETER ).value();
    double cyl_diameter = parameter( lC::STR::CYL_DIAMETER ).value();
    double cyl_length = parameter( lC::STR::CYL_LENGTH ).value();

    construct( length, bsp_diameter, cyl_diameter, cyl_length );

    emit modified();
  }

  void OCSolidBSPCYL::updateParameters ( const std::map<QString,double>& parameters )
  {
    std::map<QString,double>::const_iterator p = parameters.begin();
    for ( ; p != parameters.end(); ++p ) {
      parameter( (*p).first ).setValue( (*p).second );
    }

    double length = parameter( lC::STR::LENGTH ).value();
    double bsp_diameter = parameter( lC::STR::BSP_DIAMETER ).value();
    double cyl_diameter = parameter( lC::STR::CYL_DIAMETER ).value();
    double cyl_length = parameter( lC::STR::CYL_LENGTH ).value();

    construct( length, bsp_diameter, cyl_diameter, cyl_length );

    emit modified();
  }

  lC::ValidDelta OCSolidBSPCYL::adjustHandle ( uint id, const Point& last_pnt,
					       Point& current_pnt )
  {
    Vector delta = current_pnt - last_pnt;

    delta[X] *= 2 * handle(id).xResizeSign();
    delta[Y] *= 2 * handle(id).yResizeSign();
    delta[Z] *= 2 * handle(id).zResizeSign();

    LengthUnit* length_unit = UnitsBasis::instance()->lengthUnit();
    lCPrecision* precision = length_unit->precision( UnitsBasis::instance()->format(),
					   UnitsBasis::instance()->precision() );
    double minimum_size = 1. / precision->unitsPerIn();
    lC::ValidDelta valid_delta ( delta[X] != 0, delta[Y] != 0, delta[Z] != 0 );

    double length = parameter( lC::STR::LENGTH ).value();
    double bsp_diameter = parameter( lC::STR::BSP_DIAMETER ).value();
    double cyl_diameter = parameter( lC::STR::CYL_DIAMETER ).value();
    double cyl_length = parameter( lC::STR::CYL_LENGTH ).value();
    std::map<QString,double> parameters;

    if ( id == 2 || id == 3 ) {
      if ( length + delta[Z] <= minimum_size ) {
	if ( length <= minimum_size )
	  valid_delta.dz_ok_ = false;
	else {
	  delta[Z] = minimum_size - length;
	  current_pnt[Z] = last_pnt[Z] + handle(id).zResizeSign() * delta[Z];
	}
      }
      if ( valid_delta.dz_ok_ )
	parameters[lC::STR::LENGTH] = length + delta[Z];

      if ( bsp_diameter + delta[X] <= minimum_size ) {
	if ( bsp_diameter <= minimum_size )
	  valid_delta.dx_ok_ = false;
	else {
	  delta[X] = minimum_size - bsp_diameter;
	  current_pnt[X] = last_pnt[X] + handle(id).xResizeSign() * delta[X];
	}
      }
      if ( valid_delta.dx_ok_ )
	parameters[lC::STR::BSP_DIAMETER] = bsp_diameter + delta[X];
    }
    else if ( id == 0 || id == 1 ) {
      if ( length + delta[Z] <= minimum_size ) {
	if ( length <= minimum_size )
	  valid_delta.dz_ok_ = false;
	else {
	  delta[Z] = minimum_size - length;
	  current_pnt[Z] = last_pnt[Z] + handle(id).zResizeSign() * delta[Z];
	}
      }
      if ( valid_delta.dz_ok_ )
	parameters[lC::STR::LENGTH] = length + delta[Z];

      if ( cyl_diameter + delta[X] <= minimum_size ) {
	if ( cyl_diameter <= minimum_size )
	  valid_delta.dx_ok_ = false;
	else {
	  delta[X] = minimum_size - cyl_diameter;
	  current_pnt[X] = last_pnt[X] + handle(id).xResizeSign() * delta[X];
	}
      }
      if ( valid_delta.dx_ok_ )
	parameters[lC::STR::CYL_DIAMETER] = cyl_diameter + delta[X];
    }
    else if ( id == 4 || id == 5 ) {
      if ( cyl_length + delta[Z] <= minimum_size ) {
	if ( cyl_length <= minimum_size )
	  valid_delta.dz_ok_ = false;
	else {
	  delta[Z] = minimum_size - cyl_length;
	  current_pnt[Z] = last_pnt[Z] + handle(id).zResizeSign() * delta[Z] / 2.;
	}
      }
      if ( valid_delta.dz_ok_ )
	parameters[lC::STR::CYL_LENGTH] = cyl_length + delta[Z] / 2.;

      if ( cyl_diameter + delta[X] <= minimum_size ) {
	if ( cyl_diameter <= minimum_size )
	  valid_delta.dx_ok_ = false;
	else {
	  delta[X] = minimum_size - cyl_diameter;
	  current_pnt[X] = last_pnt[X] + handle(id).xResizeSign() * delta[X];
	}
      }
      if ( valid_delta.dx_ok_ )
	parameters[lC::STR::CYL_DIAMETER] = cyl_diameter + delta[X];
    }

    if ( valid_delta.dx_ok_ || valid_delta.dz_ok_ )
      updateParameters( parameters );

    return valid_delta;
  }

  void OCSolidBSPCYL::write ( QDomElement& xml_rep ) const
  {
    if ( !isValid() ) return;

    QDomDocument document = xml_rep.ownerDocument();
    QDomElement solid_element = document.createElement( lC::STR::SOLID );
    solid_element.setAttribute( lC::STR::NAME, name() );

    if ( material() != 0 )
      solid_element.setAttribute( lC::STR::MATERIAL, material()->commonName() );

    QDomElement bspcyl_element = document.createElement( lC::STR::BSPCYL );

    std::map<QString,Parameter>::const_iterator parameter = parametersBegin();
    for ( ; parameter != parametersEnd(); ++parameter )
      bspcyl_element.setAttribute( (*parameter).first,
				   lC::format( (*parameter).second.value() ) );

    solid_element.appendChild( bspcyl_element );
    xml_rep.appendChild( solid_element );
  }
} // End of Space3D namespace

