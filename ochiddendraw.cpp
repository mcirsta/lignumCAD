/*
 * ochiddendraw.cpp
 *
 * OCHiddenDraw classes
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
#include <BRepMesh_IncrementalMesh.hxx>
#include <HLRBRep_Algo.hxx>
#include <HLRBRep_HLRToShape.hxx>
#include <Poly_Polygon3D.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopExp_Explorer.hxx>

#include "openglview.h"
#include "ochiddendraw.h"

namespace Space3D {
  OCHiddenDraw::OCHiddenDraw ( const TopoDS_Compound& assembly, OpenGLView* view )
    : assembly_( assembly ), view_( view )
  {
    view_->makeCurrent();

    hlr_fg_name_ = glGenLists( 2 );
    hlr_bg_name_ = hlr_fg_name_ + 1;

    update();
  }

  void OCHiddenDraw::setAssembly ( const TopoDS_Compound& assembly )
  {
    assembly_ = assembly;

    update();
  }

  void OCHiddenDraw::setOrientation ( const GLdouble* /*modelview*/ )
  {
    update();
  }

  void OCHiddenDraw::draw ( void ) const
  {
    glDisable( GL_DEPTH_TEST );
    glPushMatrix();
    glLoadIdentity();
    glColor3ubv( lC::qCubv( view_->geometryColor().dark() ) );
    glCallList( hlr_bg_name_ );
    glColor3ubv( lC::qCubv( view_->geometryColor() ) );
    glCallList( hlr_fg_name_ );
    glPopMatrix();
    glEnable( GL_DEPTH_TEST );
  }

  void OCHiddenDraw::update ( void )
  {
    // It would appear that HLRAlgo breaks if there is no geometry in
    // in the Compound. And, the only way to figure this out is to
    // check the compound manually.
    TopExp_Explorer solids( assembly_, TopAbs_SOLID );
    if ( !solids.More() )
      return;

    const GLdouble* modelview = view_->viewOrientation();

    Handle( HLRBRep_Algo ) brep_hlr = new HLRBRep_Algo;
    brep_hlr->Add( assembly_ );

    gp_Trsf transform;
    transform.SetValues( modelview[0], modelview[4], modelview[8], modelview[12],
			 modelview[1], modelview[5], modelview[9], modelview[13],
             modelview[2], modelview[6], modelview[10], modelview[14]);
    // TODO ? these were here		 1e-3, 1e-3 );

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

    if ( !visible_edges.IsNull() ) {

      BRepMesh_IncrementalMesh( visible_edges, 1. );

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

      BRepMesh_IncrementalMesh( visible_edges, 1. );

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

      BRepMesh_IncrementalMesh( hidden_edges, 1. );

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

      BRepMesh_IncrementalMesh( hidden_edges, 1. );

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
} // End of Space3D namespace
