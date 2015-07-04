/* -*- c++ -*-
 * ocsoliddraw.h
 *
 * Header for the OCSolid Draw classes using OpenCASCADE
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
#ifndef OCSOLIDDRAW_H
#define OCSOLIDDRAW_H
#include <map>
#include <vector>

#include "constants.h"
#include "graphics.h"

class OpenGLView;
//class TopLoc_Location;

/*!
 * This is a slightly different take on GraphicsView: assume that we're
 * going to be stored in a QIntDict or map<GLuint...>, so don't bother
 * saving the selection_name item.
 */
class GV {
public:
  /*!
   * All defaults for constructor.
   * \param display_list display list for rendering item.
   * \param cursor_shape cursor shape to use when item is ACTIVATED.
   * \param mode current mode for rendering item.
   */
  GV ( GLuint display_list = 0,
       Qt::CursorShape cursor_shape = Qt::ArrowCursor,
       lC::Render::Mode mode = lC::Render::REGULAR )
    : display_list_( display_list ), cursor_shape_( cursor_shape ), mode_( mode )
  {}
  //! \return the display list.
  const GLuint displayList ( void ) const { return display_list_; }
  //! \return the ACTIVATED cursor shape.
  const Qt::CursorShape cursorShape ( void ) const { return cursor_shape_; }
  //! \return the current mode.
  lC::Render::Mode mode ( void ) const { return mode_; }
  /*!
   * Set the current rendering mode.
   * \param mode new rendering mode.
   */
  void setMode ( lC::Render::Mode mode ) { mode_ = mode; }
private:
  //! Display list for this item.
  GLuint display_list_;
  //! Special cursor for this item.
  Qt::CursorShape cursor_shape_;
  //! Current rendering mode.
  lC::Render::Mode mode_;
};

class ConstructionDatumView;

namespace Space3D {
  class OCSolid;
  class Dimension;
  /*!
   * This class handles drawing an OpenCASCADE solid. It is separate from
   * OCSolidView so that Subassembly can apply slightly different semantics
   * to modifying a solid while still inheriting from FigureView and without
   * duplicating the rendering code. Suggests that perhaps our Model/View/Controller
   * paradigm is not quite correct. (To wit: the InputObject classes don't exactly
   * direct the View classes so much as they are directed by the View. Is
   * there a better organization?) (Or perhaps the problem is that FigureViews
   * are tracked Children of PageView when they should just doing the drawing
   * and something else should represent the collection of views on a page?)
   * (Just to belabor this issue some more: The page view could transiently
   * create figure views to draw the figures, however, there are intricate
   * calculations involved for some figures and there is a need to store some
   * information about the view that is not really germane to the figure itself,
   * like rendering style and dimension value position.)
   */
  class OCSolidDraw {
  public:
    OCSolidDraw ( OCSolid* solid, OpenGLView* view );
    /*!
     * Draw the solid with the specified attributes. Note that for subassemblies,
     * the decision to draw the parameter dimensions is more involved
     * than the general mode implies; so, it gets its own flag.
     * \param style style is wireframe, solid...
     * \param entity if elements are highlighted/activatd, expect this type of
     * element.
     * \param mode if entity == FIGURE, use this rendering mode (otherwise
     * the sub-element graphics views are consulted).
     * \param draw_parameters if false, don't draw the parameter dimensions
     * even if they are otherwise marked as visible.
     */
    void draw ( lC::Render::Style style, SelectionEntity entity,
		lC::Render::Mode mode, bool draw_parameters ) const;
    /*!
     * Run a selection scan in the given mode.
     * \param entity if elements are highlighted/activatd, select this type of
     * element.
     * \param mode if entity == FIGURE, select handles and dimensions according
     * to mode.
     * \param select_parameters if false, don't include the parameter dimensions
     * in the select scan, even if they are otherwise mark as selectable.
     */
    void select ( SelectionEntity entity, lC::Render::Mode mode,
		  bool select_parameters );

    /*!
     * Set the highlighting status of the given item.
     * \param highlight to highlight or not to highlight.
     * \param entity should correspond to second element of items.
     * \param items selection name hierarchy.
     */
    void setHighlighted ( bool highlight, SelectionEntity entity,
			  const std::vector<GLuint>& items );

    /*!
     * Set the activated status of the given item.
     * \param activate to activate or not to activate.
     * \param entity should correspond to second element of items.
     * \param items selection name hierarchy.
     */
    void setActivated ( bool activate, SelectionEntity entity,
			const std::vector<GLuint>& items );
    //! Generic update function. Generally the solid geometry has changed.
    void update ( void );
    //! Compute the hidden lines for the current orientation of the view.
    void updateHiddenOutline ( void );
    //! Modify rendering properties (namely, display lists) when the material
    //! changes.
    void updateMaterial ( void );
    //! Modify any renderings which depend on the orientation of the view.
    void updateViewNormal ( void );
    //! Compute the selection names and attributes for the parameter's dimensions.
    void updateDimensionViews ( void );
    //! Compute the selection names and attributes for any construction datums.
    void updateDatums ( void );
    /*!
     * Retrieve the face name associated with this selection name.
     * \param selection_name second level identifier in selection name array.
     * \return selected face name.
     */
    QString faceName ( GLuint selection_name ) const;
    /*!
     * Retrieve the face name associated with this selection name, but without
     * the type identifiers.
     * \param selection_name second level identifier in selection name array.
     * \return selected face name.
     */
    QString faceNameUntyped ( GLuint selection_name ) const;
    /*!
     * Retrieve the ID path for the face associated with this selection name.
     * \param selection_name second level identifier in selection name array.
     * \return selected face ID path.
     */
    QVector<uint> faceID ( GLuint selection_name ) const;
    /*!
     * Retrieve the GL selection name for the given face id.
     * \param id face id.
     * \return GL selection name for face id.
     */
    GLuint faceGLName ( uint id )
    {
      std::map<uint,GLuint>::const_iterator gl_name = face_ids_.find( id );
      if ( gl_name != face_ids_.end() )
	return (*gl_name).second;
      return 0;			// Really an error...
    }
    /*!
     * Distinguish between handles and dimensions in the selection name.
     * \param selection_name second level identifier in selection name array.
     * \return true of the selection name is that of a handle.
     */
    bool isHandle ( GLuint selection_name ) const;
    /*!
     * Retrieve the handle name associated with this selection name.
     * \param selection_name second level identifier in selection name array.
     * \return selected handle id.
     */
    uint handleID ( GLuint selection_name ) const;
    //! \return the name of (an) active parameter dimension (or QString::null
    //! if there aren't any.
    QString activeDimensionName ( void ) const;
  private:
    //! Compute the display lists for the wireframe views.
    void updateWireframe ( void );
    //! Compute the display lists for the solid face views.
    void updateSolid ( void );
    //! Compute the selection names for the handles.
    void updateHandles ( void );
    
    //! The solid.
    OCSolid* solid_;
    //! The OpenGL view.
    OpenGLView* view_;
    //! Display list for the wireframe edge view.
    GLuint edge_name_;
    //! Display list for hidden line view foreground edges
    GLuint hlr_fg_name_;
    //! Display list for hidden line view background edges
    GLuint hlr_bg_name_;

    //! The selection names for the wire and solid assigned to each face.
    std::map< uint, GLuint > face_ids_;
    //! The display lists and the attributes of the corresponding graphics item.
    std::map< GLuint, GV > face_views_;
    //! The display lists and the attributes of the corresponding graphics item.
    std::map< GLuint, GV > wire_views_;
    //! The selection name assigned to each handle.
    std::map< uint, GLuint > handle_ids_;
    //! The selection name assigned to each parameter's dimension.
    std::map< QString, GLuint > dimension_names_;
    //! The dimension drawer objects.
    std::map< GLuint, Dimension > dimension_views_;
    //! The selection name assigned to each construction datum.
    std::map< QString, GLuint > datum_names_;
    //! The construction data drawer objects.
    std::map< GLuint, ConstructionDatumView* > datum_views_;

    //! Standard color material (not quite white).
    static GLfloat material_std_[4];
    //! Pure white material.
    static GLfloat material_white_[4];
  };
} // End of Space3D namespace

#endif // OCSOLIDDRAW_H
