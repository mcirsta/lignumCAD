/*
 * ocsoliddraw.cpp
 *
 * OCSolidDraw classes
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

#include <BRep_Tool.hxx>
#include <BRepMesh.hxx>
#include <BRepTools.hxx>
#include <BRepTools_WireExplorer.hxx>
#include <GeomLProp_SLProps.hxx>
#include <HLRBRep_Algo.hxx>
#include <HLRBRep_HLRToShape.hxx>
#include <Poly_Polygon3D.hxx>
#include <Poly_PolygonOnTriangulation.hxx>
#include <Poly_Triangulation.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Wire.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TColgp_Array1OfPnt2d.hxx>

#include "graphics.h"
#include "dburl.h"
#include "handle.h"
#include "material.h"
#include "dimension.h"
#include "ocsolid.h"
#include "octexturefunction.h"
#include "openglview.h"
#include "constructiondatumview.h"
#include "ocsoliddraw.h"

#include "lcdebug.h"

namespace Space3D {

  GLfloat OCSolidDraw::material_std_[4] = { .8, .8, .8, 1. };
  GLfloat OCSolidDraw::material_white_[4] = { 1., 1., 1., 1. };

  OCSolidDraw::OCSolidDraw ( OCSolid* solid, OpenGLView* view )
    : solid_( solid ), view_( view )
  {
    view_->makeCurrent();

    edge_name_ = glGenLists( 3 );
    hlr_fg_name_ = edge_name_ + 1;
    hlr_bg_name_ = hlr_fg_name_ + 1;

    updateWireframe();
    updateSolid();
    updateHandles();
    updateHiddenOutline();
    updateDimensionViews();
    updateDatums();
  }

  void OCSolidDraw::update ( void )
  {
    //    cout << now() << "solid drawer update [start wireframe]" << endl;
    updateWireframe();
    //    cout << now() << "solid drawer update [start solid]" << endl;
    updateSolid();
    //    cout << now() << "solid drawer update [start dimensions]" << endl;
    updateDimensionViews();
    //    cout << now() << "solid drawer update [start datums]" << endl;
    updateDatums();
    //    cout << now() << "solid drawer update [end]" << endl;
  }

  void OCSolidDraw::updateWireframe ( void )
  {
    // There are two sets of wireframe display lists: one which draws
    // the entire set of unique edges and then one display list for the
    // wire of each face.

    // Start with the unique edges.
    glNewList( edge_name_, GL_COMPILE );

    TopTools_IndexedDataMapOfShapeListOfShape edges_faces;
    TopExp::MapShapesAndAncestors( solid_->shape(), TopAbs_EDGE, TopAbs_FACE,
				   edges_faces );

    for ( int e = 1; e <= edges_faces.Extent(); e++ ) {
      // I'd call this overly complicated. You can't get a tessellation of
      // an edge independently of the face triangulation. So, there might
      // be two different approximations of the edge. For now, we'll just
      // take the first face we find.
      TopoDS_Edge edge = TopoDS::Edge( edges_faces.FindKey(e) );
      TopoDS_Face face = TopoDS::Face( edges_faces(e).First() );
      TopLoc_Location location;

      Handle( Poly_Triangulation ) triangles =
	BRep_Tool::Triangulation( face, location );

      Handle(Poly_PolygonOnTriangulation) edges_polygon =
	BRep_Tool::PolygonOnTriangulation( edge, triangles, location );

      glBegin( GL_LINE_STRIP );
      const TColStd_Array1OfInteger& indices( edges_polygon->Nodes() );
      const TColgp_Array1OfPnt& nodes( triangles->Nodes() );
      for ( int i = indices.Lower(); i <= indices.Upper(); i++ ) {
	gp_Pnt vertex = nodes( indices(i) ).Transformed( location.Transformation());
	glVertex3f( vertex.X(), vertex.Y(), vertex.Z() );
      }
      glEnd();
    }

    glEndList();

    // Now do the wires for each face.
    TopTools_IndexedDataMapOfShapeListOfShape wires_faces;
    TopExp::MapShapesAndAncestors( solid_->shape(), TopAbs_WIRE, TopAbs_FACE,
				   wires_faces );

    for ( int w = 1; w <= wires_faces.Extent(); w++ ) {

      TopoDS_Wire wire = TopoDS::Wire( wires_faces.FindKey(w) );
      TopoDS_Face face = TopoDS::Face( wires_faces(w).First() );

      // Assign a selection name to this face if it doesn't have one already.
      GLuint selection_name;

      std::map< uint, GLuint >::const_iterator face_id =
	face_ids_.find( solid_->faceID( face ) );
      if ( face_id == face_ids_.end() ) {
	selection_name = view_->genSelectionName();
	face_ids_[solid_->faceID( face )] = selection_name;
      }
      else {
	selection_name = (*face_id).second;
      }

      std::map<GLuint,GV>::const_iterator wire_view =
	wire_views_.find( selection_name );
      if ( wire_view == wire_views_.end() )
	wire_views_[selection_name] = GV( glGenLists(1) );

      BRepTools_WireExplorer wire_explorer( wire, face );

      glNewList( wire_views_[selection_name].displayList(), GL_COMPILE );

      for ( ; wire_explorer.More(); wire_explorer.Next() ) {
	TopoDS_Edge edge = wire_explorer.Current();
	TopLoc_Location location;

	Handle( Poly_Triangulation ) triangles =
	  BRep_Tool::Triangulation( face, location );

	Handle(Poly_PolygonOnTriangulation) edges_polygon =
	  BRep_Tool::PolygonOnTriangulation( edge, triangles, location );

	glBegin( GL_LINE_STRIP );
	const TColStd_Array1OfInteger& indices( edges_polygon->Nodes() );
	const TColgp_Array1OfPnt& nodes( triangles->Nodes() );
	for ( int i = indices.Lower(); i <= indices.Upper(); i++ ) {
	  gp_Pnt vertex = nodes( indices(i)).Transformed(location.Transformation());
	  glVertex3f( vertex.X(), vertex.Y(), vertex.Z() );
	}
	glEnd();
      }

      glEndList();
    }
  }

  namespace {
    struct Harlequin : std::map<QString,QColor> {
      Harlequin ( void )
      {
	(*this)[lC::STR::FRONT] = Qt::green;
	(*this)[lC::STR::BACK] = Qt::yellow;
	(*this)[lC::STR::LEFT] = Qt::red;
	(*this)[lC::STR::RIGHT] = Qt::magenta;
	(*this)[lC::STR::TOP] = Qt::blue;
	(*this)[lC::STR::BOTTOM] = Qt::cyan;
      }
    };
    
    static Harlequin harlequin_colors;
  }

  void OCSolidDraw::updateSolid ( void )
  {
    // Pretty much the same as our other code, except OpenCASCADE does
    // the tessellation itself.

    TopExp_Explorer faces( solid_->shape(), TopAbs_FACE );

    for ( ; faces.More(); faces.Next() ) {

      TopoDS_Face face = TopoDS::Face( faces.Current() );

      // Assign a selection name to this face if it doesn't have one already.
      GLuint selection_name;

      std::map< uint, GLuint >::const_iterator face_id =
	face_ids_.find( solid_->faceID( face ) );
      if ( face_id == face_ids_.end() ) {
	selection_name = view_->genSelectionName();
	face_ids_[solid_->faceID( face )] = selection_name;
      }
      else {
	selection_name = (*face_id).second;
      }

      std::map<GLuint,GV>::const_iterator face_view =
	face_views_.find( selection_name );
      if ( face_view == face_views_.end() )
	face_views_[selection_name] = GV( glGenLists(1) );

      // Get a texture function for this face. A texture function determines
      // the texture image and the transformation of points on the surface
      // into texture coordinates (on the image).
      OCTextureFunction* texture_function =
	OCTextureFactory::instance()->function( view_, face, solid_->grain(),
						solid_->material() );

      // Each face gets its own display list.
      glNewList( face_views_[selection_name].displayList(), GL_COMPILE );

      glEnable( GL_CULL_FACE );
      glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );

      // IF THERE IS A MATERIAL: Select a texture image based on the
      // orientation of the face.
      glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
      glBindTexture( GL_TEXTURE_2D, texture_function->texture() );
      glColor3ubv( lC::qCubv( harlequin_colors[solid_->faceName( face )] ) );

      glBegin( GL_TRIANGLES );

      // If the topological use of the underlying geometric surface implies that
      // the opposite side of the geometry is outside the solid, then
      // we must reverse the polarity of the surface normals in order for
      // lighting to properly computed.

      bool reverse( face.Orientation() == TopAbs_REVERSED );

      // The properties object is used to evaluate the normals on the surface.
      Handle( Geom_Surface ) surface = BRep_Tool::Surface( face );
      GeomLProp_SLProps properties( surface, 1, 0.1 );

      // Triangulation() *returns* the transformation of the surface
      // in this context.
      TopLoc_Location location;

      Handle( Poly_Triangulation ) triangles =
	BRep_Tool::Triangulation( face, location );

      // Grab lists of the nodes, parameters and triangles (i.e., indices
      // in the nodes array).
      const TColgp_Array1OfPnt& nodes( triangles->Nodes() );
      const TColgp_Array1OfPnt2d& uvs( triangles->UVNodes() );
      const Poly_Array1OfTriangle& tris( triangles->Triangles() );

      // Waltz over the list of triangles while drawing them.

      for ( Standard_Integer i = tris.Lower(); i <= tris.Upper(); ++i ) {
	Standard_Integer a, b, c;

	tris(i).Get( a, b, c );

	// Well, back face culling didn't work because the triangles
	// are not always ordered properly. Maybe we can fix this...
	gp_Pnt va, vb, vc;
	va = nodes(a).Transformed( location.Transformation() );
	vb = nodes(b).Transformed( location.Transformation() );
	vc = nodes(c).Transformed( location.Transformation() );
	gp_Vec e1( va, vb );
	gp_Vec e2( va, vc );

	properties.SetParameters( uvs(a).X(), uvs(a).Y() );
	gp_Dir normal = properties.Normal();
	if ( reverse ) normal.Reverse();

	double sense = normal.DotCross( e1, e2 );
	if ( sense < 0 ) {
	  int tmp = b;
	  b = c;
	  c = tmp;
	  gp_Pnt v_tmp;
	  v_tmp = vb;
	  vb = vc;
	  vc = v_tmp;
	}

	// For each triangle vertex: compute the surface normal (note that
	// these are evidently already transformed for location), compute the
	// the vertex location (evidently, the triangulation is *NOT* transformed
	// to the current location).

	glNormal3f( normal.X(), normal.Y(), normal.Z() );
	glTexCoord2fv( texture_function->texCoords( uvs(a), va ) );
	glVertex3f( va.X(), va.Y(), va.Z() );

	properties.SetParameters( uvs(b).X(), uvs(b).Y() );
	normal = properties.Normal();
	if ( reverse ) normal.Reverse();
	glNormal3f( normal.X(), normal.Y(), normal.Z() );
	glTexCoord2fv( texture_function->texCoords( uvs(b), vb ) );
	glVertex3f( vb.X(), vb.Y(), vb.Z() );

	properties.SetParameters( uvs(c).X(), uvs(c).Y() );
	normal = properties.Normal();
	if ( reverse ) normal.Reverse();
	glNormal3f( normal.X(), normal.Y(), normal.Z() );
	glTexCoord2fv( texture_function->texCoords( uvs(c), vc ) );
	glVertex3f( vc.X(), vc.Y(), vc.Z() );
      }

      glEnd();

      glDisable( GL_CULL_FACE );

      glEndList();
    }
  }

  QString OCSolidDraw::faceName ( GLuint selection_name ) const
  {
    // Unfortunately, this involves a search...

    std::map<uint,GLuint>::const_iterator face = face_ids_.begin();
    for ( ; face != face_ids_.end(); ++face )
      if ( (*face).second == selection_name )
	return lC::formatName( solid_->name() ) + '.' + solid_->type() + '/' +
	  lC::formatName( PartFactory::instance()->name( (*face).first ) ) +
	  ".face";

    return QString::null;
  }

  QString OCSolidDraw::faceNameUntyped ( GLuint selection_name ) const
  {
    // Unfortunately, this involves a search...

    std::map<uint,GLuint>::const_iterator face = face_ids_.begin();
    for ( ; face != face_ids_.end(); ++face )
      if ( (*face).second == selection_name )
	return lC::formatName( solid_->name() ) + '/' +
	  lC::formatName( PartFactory::instance()->name( (*face).first ) );

    return QString::null;
  }

  QVector<uint> OCSolidDraw::faceID ( GLuint selection_name ) const
  {
    QVector<uint> id_path( 2 );
    // Unfortunately, this involves a search...

    std::map<uint,GLuint>::const_iterator face = face_ids_.begin();
    for ( ; face != face_ids_.end(); ++face )
      if ( (*face).second == selection_name ) {
	id_path[0] = solid_->id();
	id_path[1] = (*face).first;
      }

    return id_path;
  }

  void OCSolidDraw::updateHandles ( void )
  {
    // Only make new handle graphics views if the handle name is unknown
    // (i.e., this should only occur once, immediately after solid creation).
    std::map<uint, HandleData>::const_iterator handle = solid_->handlesBegin();
    for ( ; handle != solid_->handlesEnd(); ++handle ) {
      if ( handle_ids_.find( (*handle).first ) == handle_ids_.end() )
	handle_ids_[ (*handle).first ] = view_->genSelectionName();
    }
  }

  bool OCSolidDraw::isHandle ( GLuint selection_name ) const
  {
    // Unfortunately, this involves a search...
    std::map<uint,GLuint>::const_iterator handle = handle_ids_.begin();
    for ( ; handle != handle_ids_.end(); ++handle )
      if ( (*handle).second == selection_name )
	return true;

    return false;
  }

  uint OCSolidDraw::handleID ( GLuint selection_name ) const
  {
    std::map<uint,GLuint>::const_iterator handle = handle_ids_.begin();
    for ( ; handle != handle_ids_.end(); ++handle )
      if ( (*handle).second == selection_name )
	return (*handle).first;

    return 0;			// Really an error...
  }

  QString OCSolidDraw::activeDimensionName ( void ) const
  {
    std::map<QString,GLuint>::const_iterator dimension_name = dimension_names_.begin();
    for ( ; dimension_name != dimension_names_.end(); ++dimension_name ) {
      std::map<GLuint,Dimension>::const_iterator dimension_view =
	dimension_views_.find( (*dimension_name).second );
      if ( dimension_view != dimension_views_.end() ) {
	if ( (*dimension_view).second.mode() == lC::Render::ACTIVATED )
	  return (*dimension_name).first;
      }
    }
    return QString::null;
  }

  void OCSolidDraw::updateHiddenOutline ( void )
  {
    const GLdouble* modelview = view_->viewOrientation();

    Handle( HLRBRep_Algo ) brep_hlr = new HLRBRep_Algo;
    brep_hlr->Add( solid_->shape() );

    gp_Trsf transform;
    transform.SetValues( modelview[0], modelview[4], modelview[8], modelview[12],
			 modelview[1], modelview[5], modelview[9], modelview[13],
			 modelview[2], modelview[6], modelview[10], modelview[14],
			 1e-3, 1e-3 );

    HLRAlgo_Projector projector( transform, false, 0. );
    brep_hlr->Projector( projector );
    brep_hlr->Update();
    brep_hlr->Hide();

    HLRBRep_HLRToShape shapes( brep_hlr );

    // Each kind of edge (visible or hidden, sharp or smooth) gets its
    // own compound shape. Visit each edge in each compound shape in
    // the usual way.

    glNewList( hlr_fg_name_, GL_COMPILE );

    TopExp_Explorer edges;
    
    TopoDS_Shape visible_edges;

    visible_edges = shapes.OutLineVCompound();

    // Ah, we learned something from valgrind: BRepMesh accesses an uninitialized
    // bounding box if this shape is NULL!

    if ( !visible_edges.IsNull() ) {

      BRepMesh::Mesh( visible_edges, 1. );

      edges.Init( visible_edges, TopAbs_EDGE );
    
      for ( ; edges.More(); edges.Next() ) {
	TopoDS_Edge edge = TopoDS::Edge( edges.Current() );
	TopLoc_Location location;
	Handle( Poly_Polygon3D ) polygon = BRep_Tool::Polygon3D( edge, location );
	if ( !polygon.IsNull() ) {
	  const TColgp_Array1OfPnt& nodes = polygon->Nodes();
	  glBegin( GL_LINE_STRIP );
	  for ( int i = nodes.Lower(); i<= nodes.Upper(); i++ )
	    glVertex3f( nodes(i).X(), nodes(i).Y(), nodes(i).Z() );
	  glEnd();
	}
      }
    }

    visible_edges = shapes.VCompound();

    if ( !visible_edges.IsNull() ) {
      BRepMesh::Mesh( visible_edges, 1. );

      edges.Init( visible_edges, TopAbs_EDGE );
    
      for ( ; edges.More(); edges.Next() ) {
	TopoDS_Edge edge = TopoDS::Edge( edges.Current() );
	TopLoc_Location location;
	Handle( Poly_Polygon3D ) polygon = BRep_Tool::Polygon3D( edge, location );
	if ( !polygon.IsNull() ) {
	  const TColgp_Array1OfPnt& nodes = polygon->Nodes();
	  glBegin( GL_LINE_STRIP );
	  for ( int i = nodes.Lower(); i<= nodes.Upper(); i++ )
	    glVertex3f( nodes(i).X(), nodes(i).Y(), nodes(i).Z() );
	  glEnd();
	}
      }
    }

    glEndList();

    // Do the "invisible" edges, too. Probably should give the user
    // some control over this... Draw with alpha?...

    glNewList( hlr_bg_name_, GL_COMPILE );

    TopoDS_Shape hidden_edges;

    hidden_edges = shapes.OutLineHCompound();

    if ( !hidden_edges.IsNull() ) {

      BRepMesh::Mesh( hidden_edges, 1. );

      edges.Init( hidden_edges, TopAbs_EDGE );
    
      for ( ; edges.More(); edges.Next() ) {
	TopoDS_Edge edge = TopoDS::Edge( edges.Current() );
	TopLoc_Location location;
	Handle( Poly_Polygon3D ) polygon = BRep_Tool::Polygon3D( edge, location );
	if ( !polygon.IsNull() ) {
	  const TColgp_Array1OfPnt& nodes = polygon->Nodes();
	  glBegin( GL_LINE_STRIP );
	  for ( int i = nodes.Lower(); i<= nodes.Upper(); i++ )
	    glVertex3f( nodes(i).X(), nodes(i).Y(), nodes(i).Z() );
	  glEnd();
	}
      }
    }

    hidden_edges = shapes.HCompound();

    if ( !hidden_edges.IsNull() ) {

      BRepMesh::Mesh( hidden_edges, 1. );

      edges.Init( hidden_edges, TopAbs_EDGE );
    
      for ( ; edges.More(); edges.Next() ) {
	TopoDS_Edge edge = TopoDS::Edge( edges.Current() );
	TopLoc_Location location;
	Handle( Poly_Polygon3D ) polygon = BRep_Tool::Polygon3D( edge, location );
	if ( !polygon.IsNull() ) {
	  const TColgp_Array1OfPnt& nodes = polygon->Nodes();
	  glBegin( GL_LINE_STRIP );
	  for ( int i = nodes.Lower(); i<= nodes.Upper(); i++ )
	    glVertex3f( nodes(i).X(), nodes(i).Y(), nodes(i).Z() );
	  glEnd();
	}
      }
    }

    glEndList();

    delete &brep_hlr;
  }

  void OCSolidDraw::updateMaterial ( void )
  {
    // Only the solid display list incorporates details of the material
    // (namely the texture image OpenGL names).
    updateSolid();
  }

  void OCSolidDraw::updateDimensionViews ( void )
  {
    std::map<QString,Parameter>::const_iterator parameter = solid_->parametersBegin();
    for ( ; parameter != solid_->parametersEnd(); ++parameter ) {

      GLuint selection_name;
      std::map<QString,GLuint>::const_iterator dimension_name =
	dimension_names_.find( (*parameter).first );
      if ( dimension_name == dimension_names_.end() ) {
	selection_name = view_->genSelectionName();
	dimension_names_[ (*parameter).first ] = selection_name;
      }
      else
	selection_name = (*dimension_name).second;

      Point end0( (*parameter).second.end0().X(),
		  (*parameter).second.end0().Y(),
		  (*parameter).second.end0().Z() );
      Point end1( (*parameter).second.end1().X(),
		  (*parameter).second.end1().Y(),
		  (*parameter).second.end1().Z() );
      Vector normal( (*parameter).second.normal().X(),
		     (*parameter).second.normal().Y(),
		     (*parameter).second.normal().Z() );

      std::map<GLuint,Dimension>::iterator dimension =
	dimension_views_.find( selection_name );
      if ( dimension == dimension_views_.end() )
	dimension_views_.insert( std::pair<GLuint,Dimension>( selection_name,
          Dimension( end0, end1, normal, view_, (*parameter).second.note() ) ) );
      else
	(*dimension).second.setEndsNormal( end0, end1, normal, view_,
					   (*parameter).second.note() );
    }
  }

  void OCSolidDraw::updateDatums ( void )
  {
    std::map<QString,const ConstructionDatum*>::const_iterator datum =
      solid_->datumsBegin();
    for ( ; datum != solid_->datumsEnd(); ++datum ) {

      GLuint selection_name;
      std::map<QString,GLuint>::const_iterator datum_name =
	datum_names_.find( (*datum).first );
      if ( datum_name == datum_names_.end() ) {
	selection_name = view_->genSelectionName();
	datum_names_[ (*datum).first ] = selection_name;
      }
      else
	selection_name = (*datum_name).second;

      std::map<GLuint,ConstructionDatumView*>::iterator datum_view =
	datum_views_.find( selection_name );
      if ( datum_view == datum_views_.end() )
	datum_views_.insert( std::pair<GLuint,ConstructionDatumView*>( selection_name,
	  ConstructionDatumViewFactory::instance()->create( (*datum).second ) ) );
    }
  }

  void OCSolidDraw::updateViewNormal ( void )
  {
    const GLdouble* modelview = view_->viewOrientation();

    std::map<GLuint,Dimension>::iterator dimension = dimension_views_.begin();
    for ( ; dimension != dimension_views_.end(); ++dimension )
      (*dimension).second.setViewNormal( Vector( modelview[2],
						 modelview[6],
						 modelview[10] ) );
  }

  void OCSolidDraw::draw ( lC::Render::Style style, SelectionEntity entity,
			   lC::Render::Mode mode, bool draw_parameters ) const
  {
    switch ( style ) {
    case lC::Render::PARENT:
    case lC::Render::STIPPLE:
      break;
    case lC::Render::WIREFRAME:
      switch ( entity ) {
      case EDGE:
      case VERTEX:
	break;
      case NONE:
      case FIGURE:
	switch ( mode ) {
	case lC::Render::INVISIBLE:
	  break;
	case lC::Render::REGULAR:
	  glColor3ubv( lC::qCubv( view_->geometryColor() ) );
	  glCallList( edge_name_ );
	  break;
	case lC::Render::HIGHLIGHTED:
	case lC::Render::ACTIVATED:
	  glColor3ubv( lC::qCubv( view_->geometryColor().light() ) );
	  glCallList( edge_name_ );
	  break;
	}
	break;
      case FACE:
	glColor3ubv( lC::qCubv( view_->geometryColor() ) );
	std::map<GLuint,GV>::const_iterator wv = wire_views_.begin();
	for ( ; wv != wire_views_.end(); ++wv ) {

	  switch ( (*wv).second.mode() ) {

	  case lC::Render::INVISIBLE:
	  case lC::Render::HIGHLIGHTED:
	  case lC::Render::ACTIVATED:
	    break;
	  case lC::Render::REGULAR:
	    glCallList( (*wv).second.displayList() );
	    break;
	  }
	}

	glDisable( GL_DEPTH_TEST );
	glColor3ubv( lC::qCubv( view_->geometryColor().light() ) );
	wv = wire_views_.begin();
	for ( ; wv != wire_views_.end(); ++wv ) {

	  switch ( (*wv).second.mode() ) {
	  case lC::Render::INVISIBLE:
	  case lC::Render::REGULAR:
	    break;
	  case lC::Render::HIGHLIGHTED:
	  case lC::Render::ACTIVATED:
	    glCallList( (*wv).second.displayList() );
	    break;
	  }
	}
	glEnable( GL_DEPTH_TEST );
	break;
      }
      break;

    case lC::Render::HIDDEN: {
      glDisable( GL_DEPTH_TEST );
      glPushMatrix();
      glLoadIdentity();
      glColor3ubv( lC::qCubv( view_->geometryColor().dark() ) );
      glCallList( hlr_bg_name_ );
      glColor3ubv( lC::qCubv( view_->geometryColor() ) );
      glCallList( hlr_fg_name_ );
      glPopMatrix();

      glColor3ubv( lC::qCubv( view_->geometryColor().light() ) );
      std::map<GLuint,GV>::const_iterator wv = wire_views_.begin();
      for ( ; wv != wire_views_.end(); ++wv ) {

	switch ( (*wv).second.mode() ) {
	case lC::Render::INVISIBLE:
	case lC::Render::REGULAR:
	  break;
	case lC::Render::HIGHLIGHTED:
	case lC::Render::ACTIVATED:
	  glCallList( (*wv).second.displayList() );
	  break;
	}
      }
      glEnable( GL_DEPTH_TEST );
    }
    break;

    case lC::Render::SOLID: {
      glEnable( GL_LIGHTING );
      glEnable( GL_LIGHT0 );
      glShadeModel( GL_SMOOTH );
      glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

      QColor color = view_->geometryColor();
      if ( solid_->material() != 0 )
	color = solid_->material()->color();
      std::map<GLuint,GV>::const_iterator fv = face_views_.begin();

      switch ( entity ) {

      case EDGE:
      case VERTEX:
	break;
      case NONE:
      case FIGURE:

	switch ( mode ) {

	case lC::Render::INVISIBLE:
	  break;
	case lC::Render::REGULAR:
	  glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE,
			lC::qCfv( color ) );
	  for ( ; fv != face_views_.end(); ++fv )
	    glCallList( (*fv).second.displayList() );
	  break;

	case lC::Render::HIGHLIGHTED:
	case lC::Render::ACTIVATED:
	  glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE,
			lC::qCfv( color.light( 110 ) ) );
	  for ( ; fv != face_views_.end(); ++fv )
	    glCallList( (*fv).second.displayList() );
	  break;
	}
	break;

      case FACE:
	for ( ; fv != face_views_.end(); ++fv ) {

	  switch ( (*fv).second.mode() ) {

	  case lC::Render::INVISIBLE:
	    break;
	  case lC::Render::REGULAR:
	    glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE,
			  lC::qCfv( color ) );
	    glCallList( (*fv).second.displayList() );
	    break;

	  case lC::Render::HIGHLIGHTED:
	  case lC::Render::ACTIVATED:
	    glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE,
			  lC::qCfv( color.light() ) );
	    glCallList( (*fv).second.displayList() );
	    break;
	  }
	}
      }
      glDisable( GL_LIGHTING );
    }
    break; 

    case lC::Render::TEXTURED: {
      glEnable( GL_TEXTURE_2D );
      glEnable( GL_LIGHTING );
      glEnable( GL_LIGHT0 );
      glEnable( GL_LIGHT1 );
      glShadeModel( GL_SMOOTH );
      glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

      QColor color = view_->geometryColor();
      if ( solid_->material() != 0 )
	color = solid_->material()->color();
      std::map<GLuint,GV>::const_iterator fv = face_views_.begin();

      switch ( entity ) {

      case EDGE:
      case VERTEX:
	break;
      case NONE:
      case FIGURE:

	switch ( mode ) {

	case lC::Render::INVISIBLE:
	  break;
	case lC::Render::REGULAR:
	  glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, material_std_);
	  for ( ; fv != face_views_.end(); ++fv ) {
	    glCallList( (*fv).second.displayList() );
	  }
	  break;

	case lC::Render::HIGHLIGHTED:
	case lC::Render::ACTIVATED:
	  glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, material_white_);
	  for ( ; fv != face_views_.end(); ++fv )
	    glCallList( (*fv).second.displayList() );
	  break;
	}
	break;

      case FACE:
	for ( ; fv != face_views_.end(); ++fv ) {

	  switch ( (*fv).second.mode() ) {

	  case lC::Render::INVISIBLE:
	    break;
	  case lC::Render::REGULAR:
	    glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, material_std_);
	    glCallList( (*fv).second.displayList() );
	    break;

	  case lC::Render::HIGHLIGHTED:
	  case lC::Render::ACTIVATED:
	    glMaterialfv( GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE,material_white_);
	    glCallList( (*fv).second.displayList() );
	    break;
	  }
	}
      }
      glDisable( GL_LIGHTING );
      glDisable( GL_TEXTURE_2D );
    }
    break; 

    case lC::Render::HIGHLIGHTS: {

      glColor3ubv( lC::qCubv( view_->geometryColor().light() ) );
      std::map<GLuint,GV>::const_iterator wv = wire_views_.begin();
      for ( ; wv != wire_views_.end(); ++wv ) {

	switch ( (*wv).second.mode() ) {
	case lC::Render::INVISIBLE:
	case lC::Render::REGULAR:
	  break;
	case lC::Render::HIGHLIGHTED:
	case lC::Render::ACTIVATED:
	  glCallList( (*wv).second.displayList() );
	  break;
	}
      }
    }
    }

    // Draw the handles as required

    switch ( entity ) {

    case EDGE:
    case VERTEX:
      break;
    case NONE:
    case FIGURE:
      switch ( mode ) {
      case lC::Render::INVISIBLE:
      case lC::Render::REGULAR:
        break;
      case lC::Render::HIGHLIGHTED:
      case lC::Render::ACTIVATED:
	std::map<uint, HandleData>::const_iterator handle = solid_->handlesBegin();
	for ( ; handle != solid_->handlesEnd(); ++handle ) {
	  Point p( (*handle).second.position().X(),
		   (*handle).second.position().Y(),
		   (*handle).second.position().Z() );
	  ResizeHandle::draw( view_, mode, p );
	}
      }
      break;

    case FACE: {
      std::map<uint,GLuint>::const_iterator fid = face_ids_.begin();
      for ( ; fid != face_ids_.end(); ++fid ) {
	// Note the need here for a map from face selection name
	// to the face string name...
	std::map<GLuint,GV>::const_iterator fv = face_views_.find((*fid).second);
	if ( fv != face_views_.end() ) {
	  switch ( (*fv).second.mode() ) {
	  case lC::Render::INVISIBLE:
	  case lC::Render::REGULAR:
	    break;
	  case lC::Render::HIGHLIGHTED:
	  case lC::Render::ACTIVATED:
	    QVector<uint> handles( solid_->
					faceHandles( PartFactory::instance()->
						     name( (*fid).first ) ) );
	    QVector<uint>::const_iterator h = handles.begin();
	    for ( ; h != handles.end(); ++h ) {
	      const HandleData& handle( solid_->handle( *h ) );
	      Point p( handle.position().X(),
		       handle.position().Y(),
		       handle.position().Z() );
	      ResizeHandle::draw( view_, (*fv).second.mode(), p );
	    }
	  }
	}
      }
    }
    }

    // Draw the parameter dimensions as required. (Construction datums, too.)

    if ( draw_parameters ) {
      switch ( entity ) {

      case NONE:
      case FIGURE:
	switch ( mode ) {
	case lC::Render::INVISIBLE:
	case lC::Render::REGULAR:
	case lC::Render::HIGHLIGHTED:
	  break;
	case lC::Render::ACTIVATED:
	  std::map<GLuint,Dimension>::const_iterator dimension =
	    dimension_views_.begin();
	  for ( ; dimension != dimension_views_.end(); ++dimension )
	    (*dimension).second.draw();

	  std::map<GLuint,ConstructionDatumView*>::const_iterator datum_view =
	    datum_views_.begin();
	  for ( ; datum_view != datum_views_.end(); ++datum_view )
	    (*datum_view).second->draw( view_ );
	  break;
	}
      case FACE:
      case EDGE:
      case VERTEX:
	break;
      }
    }
  }

  void OCSolidDraw::select ( SelectionEntity entity, lC::Render::Mode mode,
			     bool select_parameters )
  {
    std::map<GLuint,GV>::const_iterator fv = face_views_.begin();
    switch ( entity ) {
    case FIGURE:
      // Note: In this case, we expect the view to have already pushed
      // the selection name.
      for ( ; fv != face_views_.end(); ++fv )
	glCallList( (*fv).second.displayList() );

      switch ( mode ) {
      case lC::Render::ACTIVATED: {
	if ( select_parameters ) {
	  std::map<GLuint,Dimension>::const_iterator dimension =
	    dimension_views_.begin();
	  for ( ; dimension != dimension_views_.end(); ++dimension ) {
	    glPushName( (*dimension).first );
	    (*dimension).second.select();
	    glPopName();
	  }
	}
      }
      // Note, delibrate fall through.
      case lC::Render::HIGHLIGHTED: {
	std::map<uint, HandleData>::const_iterator handle = solid_->handlesBegin();
	for ( ; handle != solid_->handlesEnd(); ++handle ) {
	  std::map<uint,GLuint>::const_iterator h = handle_ids_.find( (*handle).first );
	  if ( h != handle_ids_.end() ) {
	    Point p( (*handle).second.position().X(),
		     (*handle).second.position().Y(),
		     (*handle).second.position().Z() );
	    glPushName( (*h).second );
	    ResizeHandle::select( view_, p );
	    glPopName();
	  }
	}
      }
      case lC::Render::INVISIBLE:
      case lC::Render::REGULAR:
	break;
      }
      break;
     case FACE:
      for ( ; fv != face_views_.end(); ++fv ) {
	glPushName( (*fv).first );
	glCallList( (*fv).second.displayList() );
	glPopName();
      }
      break;
    case NONE:
    case EDGE:
    case VERTEX:
      break;
    }
  }

  void OCSolidDraw::setHighlighted ( bool highlight, SelectionEntity entity,
				     const std::vector<GLuint>& items )
  {
    // I guess this is now out of sync with the previous milieu
    switch ( entity ) {
    case FIGURE: {
      std::map<GLuint,Dimension>::iterator dimension = dimension_views_.find( items[1] );
      if ( dimension != dimension_views_.end() ) {
	if ( (*dimension).second.mode() != lC::Render::ACTIVATED ) {
	  if ( highlight )
	    (*dimension).second.setMode( lC::Render::HIGHLIGHTED );
	  else
	    (*dimension).second.setMode( lC::Render::REGULAR );
	}
      }
    }
    break;
    case FACE:
      if ( wire_views_[items[1]].mode() != lC::Render::ACTIVATED ) {
	if ( highlight ) {
	  wire_views_[ items[1] ].setMode( lC::Render::HIGHLIGHTED );
	  face_views_[ items[1] ].setMode( lC::Render::HIGHLIGHTED );
	}
	else {
	  wire_views_[ items[1] ].setMode( lC::Render::REGULAR );
	  face_views_[ items[1] ].setMode( lC::Render::REGULAR );
	}
      }
    case NONE:
    case EDGE:
    case VERTEX:
      break;
    }
  }

  void OCSolidDraw::setActivated ( bool activate, SelectionEntity entity,
				   const std::vector<GLuint>& items )
  {
    // I guess this is now out of sync with the previous milieu
    switch ( entity ) {
    case FIGURE: {
      std::map<GLuint,Dimension>::iterator dimension = dimension_views_.begin();
      for ( ; dimension != dimension_views_.end(); ++dimension )
	if ( activate )
	  (*dimension).second.setMode( lC::Render::REGULAR );
	else
	  (*dimension).second.setMode( lC::Render::INVISIBLE );
      
      if ( items.size() > 1 ) {
	dimension = dimension_views_.find( items[1] );
	if ( dimension != dimension_views_.end() ) {
	  if ( activate )
	    (*dimension).second.setMode( lC::Render::ACTIVATED );
	  else
	    (*dimension).second.setMode( lC::Render::REGULAR );
	}
      }
    }
    break;
    case FACE:
      if ( activate ) {
	wire_views_[ items[1] ].setMode( lC::Render::ACTIVATED );
	face_views_[ items[1] ].setMode( lC::Render::ACTIVATED );
      }
      else {
	wire_views_[ items[1] ].setMode( lC::Render::REGULAR );
	face_views_[ items[1] ].setMode( lC::Render::REGULAR );
      }
    case NONE:
    case EDGE:
    case VERTEX:
      break;
    }
  }
} // End of Space3D namespace
