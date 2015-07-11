/*
 * ocsubassemblydraw.cpp
 *
 * OCSubassemblyDraw classes
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
#include "dburl.h"
#include "openglview.h"
#include "part.h"
#include "assembly.h"
#include "subassembly.h"
#include "ocsoliddraw.h"
#include "ocsubassemblydraw.h"

#include "lcdebug.h"

namespace Space3D {
  /*!
   * Simple class to draw subassemblies which are themselves simply parts.
   */
  class OCSubassemblyPartDraw : public OCSubassemblyDraw {
  public:
    /*!
     * Construct a simple OCSolidDraw-er for the part.
     * \param part part to draw.
     * \param view OpenGLView to draw in.
     */
    OCSubassemblyPartDraw( Subassembly* subassembly, OpenGLView* view )
    {
      subassembly_ = subassembly;
      Part* part = dynamic_cast<Part*>( subassembly->subassembly() );
      drawer_ = new OCSolidDraw( part->solid(), view );
    }
    //! \return the subassembly to which this drawer refers.
    Subassembly* subassembly ( void ) const { return subassembly_; }
    /*!
     * Generate some text to show in the main window status bar indicating
     * what is selected by the given selection name and entity. For example,
     * if entity is FIGURE, just give the name of the figure. If entity is
     * FACE, give "figure/face".
     * \param selection_name item's selection name.
     * \param entity type of entity currently being sought.
     * \return selection string representation.
     */
    QString selectionText ( const std::vector<GLuint>& selection_name,
			    SelectionEntity entity ) const
    {
      QString text;
      switch ( entity ) {
      case FIGURE:
	text = subassembly_->subassembly()->name(); break;
      case FACE:
	text = drawer_->faceNameUntyped( selection_name[1] );
      }
      return text;
    }
    
    /*!
     * Set the designated item to the given highlight mode.
     * \param highlight new highlight mode.
     * \param entity entity path to find (expect) in item.
     * \param item OpenGL selection name path of subcomponent.
     */
    void setHighlighted ( bool highlight, SelectionEntity entity,
			  const std::vector<GLuint>& item )
    {
      drawer_->setHighlighted( highlight, entity, item );
    }
    /*!
     * Set the designated item to the given activation mode.
     * \param activate new activation mode.
     * \param entity FIGURE/GEOMETRY selector.
     * \param item OpenGL selection name path of subcomponent.
     */
    void setActivated ( bool activate, SelectionEntity entity,
			const std::vector<GLuint>& item )
    {
      drawer_->setActivated( activate, entity, item );
    }
    /*!
     * Draw the part with the specified attributes.
     * \param style style is wireframe, solid...
     * \param entity if elements are highlighted/activatd, expect this type of
     * element.
     * \param mode if entity == FIGURE, use this rendering mode (otherwise
     * the sub-element graphics views are consulted).
     */
    void draw ( lC::Render::Style style, SelectionEntity entity,
		lC::Render::Mode mode ) const
    {
      // In the case of assemblies, the view handles drawing the hidden
      // line style. This drawing is limited to only the highlighted
      // elements.
      if ( style == lC::Render::HIDDEN )
	style = lC::Render::HIGHLIGHTS;

      if ( !subassembly_->location().IsIdentity() )
	useTransform();

      drawer_->draw( style, entity, mode, false );

      if ( !subassembly_->location().IsIdentity() )
	glPopMatrix();
    }
    /*!
     * Run a selection scan in the given mode.
     * \param entity if elements are highlighted/activatd, select this type of
     * element.
     * \param mode if entity == FIGURE, select handles and dimensions according
     * to mode.
     */
    void select ( SelectionEntity entity, lC::Render::Mode mode ) const
    {
      if ( !subassembly_->location().IsIdentity() )
	useTransform();

      drawer_->select( entity, mode, false );

      if ( !subassembly_->location().IsIdentity() )
	glPopMatrix();
    }
    /*!
     * Try to locate the geometry (face) selected by this list of GL names.
     * \param selection_names "path" to selected face.
     * \return name of selected face.
     */
    QString geometry ( const std::vector<GLuint>& selection_names ) const
    {
      // At this point, any assembly refereces should have been stripped out.
      // But that is currently not the case...
      return QString( "%1.%2/%3.%4/%5" ).
	arg( subassembly_->name() ).
	arg( subassembly_->type() ).
	arg( subassembly_->subassembly()->name() ).
	arg( subassembly_->subassembly()->type() ).
	arg( drawer_->faceName( selection_names[1] ) );
    }
    /*!
     * Try to locate the geometry (face) selected by this list of GL names.
     * \param selection_names "path" to selected face.
     * \return ID of selected face.
     */
    QVector<uint> ID ( const std::vector<GLuint>& selection_names ) const
    {
      QVector<uint> id_path( 4 );
      id_path[0] = subassembly_->id();
      id_path[1] = subassembly_->subassembly()->id();
      QVector<uint> solid_path = drawer_->faceID( selection_names[1] );
      id_path[2] = solid_path[0];
      id_path[3] = solid_path[1];
      return id_path;
    }
    /*!
     * The inverse of the above function: Find the gl selection name list
     * from the ID.
     */
    void lookup ( QVector<uint>& id_path, std::vector<GLuint>& name_path )
      const
    {
      // There should now be exactly three elements in the id_path
      // subassembly:part:face
      GLuint gl_name = drawer_->faceGLName( id_path[2] );
      if ( gl_name != 0 )
	name_path.push_back( gl_name );
    }
    //! Generic update function. Generally the solid geometry has changed.
    void update ( void )
    {
      //      cout << now() << subassembly_->path() << " part drawer update [start]" << endl;
      drawer_->update();
    }
    //! Modify rendering properties (namely, display lists) when the material
    //! changes.
    void updateMaterial ( void )
    {
      drawer_->updateMaterial();
    }
    //! Update anything which depends on the view orientation (mostly, this is
    //! dimension arrows)
    void updateViewNormal ( void )
    {
      drawer_->updateViewNormal();
    }
  private:
    //! Apply the ***SUB***assembly's transform.
    void useTransform ( void ) const
    {
      gp_Trsf transform = subassembly_->location();
      // OpenCASCADE could be a lot kinder to OpenGL...
      double modelview[16];
      modelview[0] = transform.Value( 1, 1 );
      modelview[1] = transform.Value( 2, 1 );
      modelview[2] = transform.Value( 3, 1 );
      modelview[3] = 0;
      modelview[4] = transform.Value( 1, 2 );
      modelview[5] = transform.Value( 2, 2 );
      modelview[6] = transform.Value( 3, 2 );
      modelview[7] = 0;
      modelview[8] = transform.Value( 1, 3 );
      modelview[9] = transform.Value( 2, 3 );
      modelview[10] = transform.Value( 3, 3 );
      modelview[11] = 0;
      modelview[12] = transform.Value( 1, 4 );
      modelview[13] = transform.Value( 2, 4 );
      modelview[14] = transform.Value( 3, 4 );
      modelview[15] = 1;

      glPushMatrix();
      glMultMatrixd( modelview );
    }

    Subassembly* subassembly_;
    OCSolidDraw* drawer_;
  };

  /*!
   * The substantially more complicated assembly drawer. Every subassembly
   * gets an extra layer of GL selection name.
   */
  class OCSubassemblyAsmDraw : public OCSubassemblyDraw {
  public:
    /*!
     * Construct a list of OCSubassemblyDraw-er for the assembly.
     * \param assembly assembly to draw.
     * \param view OpenGLView to draw in.
     */
    OCSubassemblyAsmDraw( Subassembly* subassembly, OpenGLView* view )
    {
      subassembly_ = subassembly;

      Assembly* assembly = dynamic_cast<Assembly*>( subassembly->subassembly() );

      const QMap<uint,Figure*>& figures = assembly->figures();
      QMap<uint,Figure*>::const_iterator figure = figures.begin();

      for ( ; figure != figures.end(); ++figure ) {
    Subassembly* subassembly = dynamic_cast<Subassembly*>( figure.value() );
	OCSubassemblyDraw* drawer =
	  OCSubassemblyDrawFactory::drawer( subassembly, view );
	drawers_[ view->genSelectionName() ] = drawer;
      }
    }
    //! \return the subassembly to which this drawer refers.
    Subassembly* subassembly ( void ) const { return subassembly_; }
    /*!
     * Generate some text to show in the main window status bar indicating
     * what is selected by the given selection name and entity. For example,
     * if entity is FIGURE, just give the name of the figure. If entity is
     * FACE, give "figure/face".
     * \param selection_name item's selection name.
     * \param entity type of entity currently being sought.
     * \return selection string representation.
     */
    QString selectionText ( const std::vector<GLuint>& selection_name,
			    SelectionEntity entity ) const
    {
      std::vector<GLuint> subcomponent( selection_name.size()-1 );
      for ( uint i = 1; i < selection_name.size(); ++i )
	subcomponent[i-1] = selection_name[i];

      std::map<GLuint,OCSubassemblyDraw*>::const_iterator drawer =
	drawers_.find( selection_name[1] );

      if ( drawer != drawers_.end() )
	return (*drawer).second->selectionText( subcomponent, entity );

      return QString::null;
    }
    /*!
     * Set the designated item to the given highlight mode.
     * \param highlight new highlight mode.
     * \param entity FIGURE/GEOMETRY selector.
     * \param item OpenGL selection name path of subcomponent.
     */
    void setHighlighted ( bool highlight, SelectionEntity entity,
			  const std::vector<GLuint>& item )
    {
      std::vector<GLuint> subcomponent( item.size()-1 );
      for ( uint i = 1; i < item.size(); ++i )
	subcomponent[i-1] = item[i];

      std::map<GLuint,OCSubassemblyDraw*>::const_iterator drawer =
	drawers_.find( subcomponent[0] );

      if ( drawer != drawers_.end() )
	(*drawer).second->setHighlighted( highlight, entity, subcomponent );
    }
    /*!
     * Set all the designated items to the given activation mode.
     * \param activate new activation mode.
     * \param entity GEOMETRY/FIGURE selector (but can only be FIGURE here).
     * \param item OpenGL selection names of subcomponents (none here).
     */
    void setActivated ( bool activate, SelectionEntity entity,
			const std::vector<GLuint>& item )
    {
      std::vector<GLuint> subcomponent( item.size()-1 );
      for ( uint i = 1; i < item.size(); ++i )
	subcomponent[i-1] = item[i];

      std::map<GLuint,OCSubassemblyDraw*>::const_iterator drawer =
	drawers_.find( subcomponent[0] );

      if ( drawer != drawers_.end() )
	(*drawer).second->setActivated( activate, entity, subcomponent );
    }
    /*!
     * Draw the assembly with the specified attributes.
     * \param style style is wireframe, solid...
     * \param entity if elements are highlighted/activatd, expect this type of
     * element.
     * \param mode if entity == FIGURE, use this rendering mode (otherwise
     * the sub-element graphics views are consulted).
     */
    void draw ( lC::Render::Style style, SelectionEntity entity,
		lC::Render::Mode mode ) const
    {
      std::map<GLuint,OCSubassemblyDraw*>::const_iterator drawer = drawers_.begin();

      if ( !subassembly_->location().IsIdentity() )
	useTransform();

      for ( ; drawer != drawers_.end(); ++drawer )
	(*drawer).second->draw( style, entity, mode );

      if ( !subassembly_->location().IsIdentity() )
	glPopMatrix();
    }
    /*!
     * Run a selection scan in the given mode.
     * \param entity if elements are highlighted/activatd, select this type of
     * element.
     * \param mode if entity == FIGURE, select handles and dimensions according
     * to mode.
     */
    void select ( SelectionEntity entity, lC::Render::Mode mode ) const
    {
      std::map<GLuint,OCSubassemblyDraw*>::const_iterator drawer = drawers_.begin();

      if ( !subassembly_->location().IsIdentity() )
	useTransform();

      for ( ; drawer != drawers_.end(); ++drawer ) {
	glPushName( (*drawer).first );
	(*drawer).second->select( entity, mode );
	glPopName();
      }

      if ( !subassembly_->location().IsIdentity() )
	glPopMatrix();
    }
    /*!
     * Try to locate the geometry (face) selected by this list of GL names.
     * \param item "path" to selected face.
     * \return name of selected face.
     */
    QString geometry ( const std::vector<GLuint>& item ) const
    {
      std::vector<GLuint> subcomponent( item.size()-1 );
      for ( uint i = 1; i < item.size(); ++i )
	subcomponent[i-1] = item[i];

      std::map<GLuint,OCSubassemblyDraw*>::const_iterator drawer =
	drawers_.find( item[1] );

      if ( drawer != drawers_.end() )
	return QString( "%1.%2/%3.%4/%5" ).
	  arg( subassembly_->name() ).arg( subassembly_->type() ).
	  arg( subassembly_->subassembly()->name() ).
	  arg( subassembly_->subassembly()->type() ).
	  arg( (*drawer).second->geometry( subcomponent ) );

      return QString::null;
    }
    /*!
     * Try to locate the geometry (face) selected by this list of GL names.
     * \param item "path" to selected face.
     * \return ID of selected face.
     */
    QVector<uint> ID ( const std::vector<GLuint>& item ) const
    {
      std::vector<GLuint> subcomponent( item.size()-1 );
      for ( uint i = 1; i < item.size(); ++i )
	subcomponent[i-1] = item[i];

      std::map<GLuint,OCSubassemblyDraw*>::const_iterator drawer =
	drawers_.find( item[1] );

      QVector<uint> id_path( 2 );
      id_path[0] = subassembly_->id();
      id_path[1] = subassembly_->subassembly()->id();

      if ( drawer != drawers_.end() ) {
	QVector<uint> subcomponent_path = (*drawer).second->ID( subcomponent );
	id_path.resize( subcomponent_path.size() + 2 );
	for ( uint i = 0; i < subcomponent_path.size(); ++i )
	  id_path[2 + i] = subcomponent_path[i];
      }

      return id_path;
    }
    /*!
     * The inverse of the above function: Find the gl selection name list
     * from the ID.
     */
    void lookup ( QVector<uint>& id_path, std::vector<GLuint>& name_path )
      const
    {
      id_path.erase( id_path.begin() );	// Erase subassembly (self?)

      std::map<GLuint,OCSubassemblyDraw*>::const_iterator drawer = drawers_.begin();

      for ( ; drawer != drawers_.end(); ++drawer ) {
	if ( (*drawer).second->subassembly()->id() == id_path[0] ) {
	  name_path.push_back( (*drawer).first );

	  id_path.erase( id_path.begin() );

	  if ( !id_path.empty() ) {
	    (*drawer).second->lookup( id_path, name_path );
	  }

	  break;
	}
      }
    }
    //! Generic update function. Generally the solid geometry has changed.
    void update ( void )
    {
      //      cout << now() << subassembly_->path() << " assembly drawer update [start]" << endl;
      std::map<GLuint,OCSubassemblyDraw*>::const_iterator drawer = drawers_.begin();

      for ( ; drawer != drawers_.end(); ++drawer )
	(*drawer).second->update();
    }
    //! Modify rendering properties (namely, display lists) when the material
    //! changes.
    void updateMaterial ( void )
    {}
    //! Update anything which depends on the view orientation (mostly, this is
    //! dimension arrows)
    void updateViewNormal ( void )
    {
      std::map<GLuint,OCSubassemblyDraw*>::const_iterator drawer = drawers_.begin();

      for ( ; drawer != drawers_.end(); ++drawer )
	(*drawer).second->updateViewNormal();
    }

  private:
    //! Apply the ***SUB***assembly's transform.
    void useTransform ( void ) const
    {
      gp_Trsf transform = subassembly_->location();
      // OpenCASCADE could be a lot kinder to OpenGL...
      double modelview[16];
      modelview[0] = transform.Value( 1, 1 );
      modelview[1] = transform.Value( 2, 1 );
      modelview[2] = transform.Value( 3, 1 );
      modelview[3] = 0;
      modelview[4] = transform.Value( 1, 2 );
      modelview[5] = transform.Value( 2, 2 );
      modelview[6] = transform.Value( 3, 2 );
      modelview[7] = 0;
      modelview[8] = transform.Value( 1, 3 );
      modelview[9] = transform.Value( 2, 3 );
      modelview[10] = transform.Value( 3, 3 );
      modelview[11] = 0;
      modelview[12] = transform.Value( 1, 4 );
      modelview[13] = transform.Value( 2, 4 );
      modelview[14] = transform.Value( 3, 4 );
      modelview[15] = 1;

      glPushMatrix();
      glMultMatrixd( modelview );
    }

    Subassembly* subassembly_;
    std::map<GLuint,OCSubassemblyDraw*> drawers_;
  };

  OCSubassemblyDraw* OCSubassemblyDrawFactory::drawer ( Subassembly* subassembly,
							OpenGLView* view )
  {
    if ( dynamic_cast<Part*>( subassembly->subassembly() ) != 0 )
      return new OCSubassemblyPartDraw( subassembly , view );
    else if ( dynamic_cast<Assembly*>( subassembly->subassembly() ) != 0 )
      return new OCSubassemblyAsmDraw( subassembly, view );

    return 0;
  }

} // End of Space3D namespace
