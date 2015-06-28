/* -*- c++ -*-
 * ocsubassemblydraw.h
 *
 * Header for the OCSubassembly Draw classes using OpenCASCADE
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
#ifndef OCSUBASSEMBLYDRAW_H
#define OCSUBASSEMBLYDRAW_H

#include "constants.h"
#include "graphics.h"

class Subassembly;
class OpenGLView;

namespace Space3D {
  /*!
   * This class is supposed to present a uniform interface to drawing
   * and selecting subassemblies. A subassembly can be either a single part
   * or a hierarchy of parts and assemblies.
   */
  class OCSubassemblyDraw {
  public:
    virtual ~OCSubassemblyDraw ( void ) {}
    //! \return the subassembly to which this drawer refers.
    virtual Subassembly* subassembly ( void ) const = 0;
    /*!
     * Generate some text to show in the main window status bar indicating
     * what is selected by the given selection name and entity. For example,
     * if entity is FIGURE, just give the name of the figure. If entity is
     * FACE, give "figure/face".
     * \param selection_name item's selection name.
     * \param entity type of entity currently being sought.
     * \return selection string representation.
     */
    virtual QString selectionText ( const vector<GLuint>& selection_name,
				    SelectionEntity entity ) const = 0;
    /*!
     * Draw the subassembly with the specified attributes.
     * \param style style is wireframe, solid...
     * \param entity if elements are highlighted/activatd, expect this type of
     * element.
     * \param mode if entity == FIGURE, use this rendering mode (otherwise
     * the sub-element graphics views are consulted).
     */
    virtual void draw ( lC::Render::Style style, SelectionEntity entity,
			lC::Render::Mode mode ) const = 0;
    /*!
     * Run a selection scan in the given mode.
     * \param entity if elements are highlighted/activatd, select this type of
     * element.
     * \param mode if entity == FIGURE, select handles and dimensions according
     * to mode.
     */
    virtual void select ( SelectionEntity entity, lC::Render::Mode mode ) const = 0;
    /*!
     * Try to locate the geometry (face) selected by this list of GL names.
     * \param selection_names "path" to selected face.
     * \return path reference to selected face.
     */
    virtual QString geometry ( const vector<GLuint>& selection_names ) const = 0;
    /*!
     * Try to locate the geometry (face) selected by this list of GL names.
     * \param selection_names "path" to selected face.
     * \return ID of selected face.
     */
    virtual QValueVector<uint> ID ( const vector<GLuint>& selection_names ) const
      = 0;
    /*!
     * The inverse of the above function: Find the gl selection name list
     * from the ID.
     */
    virtual void lookup ( QValueVector<uint>& id_path, vector<GLuint>& name_path )
      const = 0;
    /*!
     * Set the highlighting status of the given item.
     * \param highlight to highlight or not to highlight.
     * \param entity should correspond to the last element of items.
     * \param items selection name hierarchy.
     */
    virtual void setHighlighted ( bool highlight, SelectionEntity entity,
				  const vector<GLuint>& items ) = 0;
    /*!
     * Set the activated status of the given item.
     * \param activate to activate or not to activate.
     * \param entity should correspond to the last element of items.
     * \param items selection name hierarchy.
     */
    virtual void setActivated ( bool activate, SelectionEntity entity,
				const vector<GLuint>& items ) = 0;
    //! Modify rendering properties (namely, display lists) when the material
    //! changes. [An signal for the drawer (except it's not a QObject:-()]
    virtual void updateMaterial ( void ) = 0;
    //! Generic update function. Generally the solid geometry has changed.
    virtual void update ( void ) = 0;
    //! Compute anything which depends on the view orientation (like, mostly,
    //! dimension arrows)
    virtual void updateViewNormal ( void ) = 0;
  };

  /*!
   * Now, the ever present factory class. Whatever we need to display,
   * pass one in and we'll return an object capable of drawing and
   * selecting it.
   */
  class OCSubassemblyDrawFactory {
  public:
    static OCSubassemblyDraw* drawer ( Subassembly* subassemly, OpenGLView* view );
  };
} // End of Space3D namespace

#endif // SUBASSEMBLYDRAW_H
