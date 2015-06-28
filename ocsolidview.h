/* -*- c++ -*-
 * ocsolidview.h
 *
 * Header for the OCSolid View classes using OpenCASCADE
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
#ifndef OCSOLIDVIEW_H
#define OCSOLIDVIEW_H

#include <map>

#include <gp_Trsf.hxx>

#include "vectoralgebra.h"
#include "figureview.h"
#include "handle.h"

class QDomElement;
class PartInfoDialog;
class ParameterInfoDialog;
class GLUtesselator;

namespace Space3D {
  class Page;
  class OCSolid;
  class OCSolidView;
  class OCSolidDraw;
  class Dimension;
  class ConstructionDatumView;

  class OCSolidModifyInput : public InputObject {
    Q_OBJECT

  public:
    OCSolidModifyInput ( OCSolid* solid, OCSolidView* solid_view );
    SelectionType selectionType ( void ) const { return selection_type_; }
    bool needsPrepressMouseCoordinates ( void ) const { return true; }
    void startDisplay ( QPopupMenu* /*context_menu*/ ) {}
    void stopDisplay ( QPopupMenu* /*context_menu*/ ) {}
    void mousePrepress ( QMouseEvent* me, const SelectedNames& selected );
    void mousePress ( QMouseEvent* me, const SelectedNames& selected );
    void mouseDrag ( QMouseEvent* me, const SelectedNames& selected );
    bool mouseRelease ( QMouseEvent* me, const SelectedNames& selected );
    void mouseDoubleClick ( QMouseEvent* me );
    void keyPress ( QKeyEvent* /*ke*/ ) {}

    void setSolid ( OCSolid* solid );

  public slots:
    void cancelOperation ( void ) {}

  private:
    SelectionType selection_type_;
    OCSolid* solid_;
    OCSolidView* solid_view_;

    std::map< QString, double > old_parameters_;

    GLuint drag_handle_;
    Point start_pnt_;
    Point last_pnt_;
    Point current_pnt_;
  };

  class OCSolidView : public FigureView {
    Q_OBJECT

    OCSolid* solid_;
    OCSolidDraw* drawer_;

    OCSolidModifyInput modify_input_;

    ListViewItem* list_view_item_;

    GLuint edge_name_;
    GLuint tri_name_;
    GLuint hlr_name_;

    // Here are some more OpenGL attributes.
    GLfloat material_color_[4];
    static GLfloat material_white_[4];
    GLuint face_texture_;
    GLuint end_texture_;
    GLuint edge_texture_;

    gp_Trsf transformation_;

  public:
    OCSolidView ( OCSolid* solid, PageView* parent );
    OCSolidView ( const QDomElement& xml_rep, PageView* parent );
#if 0
    OCSolidView ( const QString& name, const QDomElement& xml_rep,
		  Page* page, PageView* parent );
#endif
    ~OCSolidView ( void );

    OCSolid* solid ( void ) const { return solid_; }

    // Implementation of GraphicsObject interface
    void draw ( void ) const;
    void select ( SelectionType select_type ) const;

    // Implementation of FigureView interface

    //! \return the list view item for this page view.
    ListViewItem* listViewItem ( void ) const { return list_view_item_; }
    uint id ( void ) const;
    QString name ( void ) const;
    QString type ( void ) const;
    DBURL dbURL ( void ) const;
    /*!
     * Return a string which represents the selection for the entity type.
     * \param selection_name item's selection name.
     * \param entity entity selected for in view.
     * \return string representing selection.
     */
    QString selectionText ( const std::vector<GLuint>& selection_name,
			    SelectionEntity entity ) const;

    View* lookup ( QStringList& /*path_components*/ ) const
    { return 0; }
    void lookup ( QValueVector<uint>& /*id_path*/, std::vector<GLuint>& /*name_path*/ )
      const
    {}

    void setHighlighted ( bool highlight, SelectionEntity entity,
			  const std::vector<GLuint>& items );
    void setActivated( bool activate, SelectionEntity entity,
		       const std::vector<GLuint>& items );

    InputObject* createInput ( void ) { return 0; }
    InputObject* modifyInput ( void ) { return &modify_input_; }

    CreateObject* memento ( void );

    uint dimensionPickCount ( void ) const { return 0; }
    void setDimensionView ( const QDomElement& /*xml_rep*/ ) {}

    void viewAttributeChanged ( void );

    bool isGeometry ( GLuint selection_name ) const;
    QString geometry ( GLuint selection_name ) const;
    FigureBase* figure ( void ) const;
    /*!
     * Create an XML representation of this view (only) suitable for
     * saving in a file with the rest of the model information.
     * \param xml_rep the XML representation of the model to append
     * this representation to.
     */
    void write ( QDomElement& xml_rep ) const;
    /*!
     * Create an XML representation for this view *AND* its object suitable
     * for use as a clipboard selection.
     * \param xml_rep the XML representation of the selection to append
     * this representation to.
     */
    void copyWrite ( QDomElement& xml_rep ) const;
    /*!
     * Set the basic solid color of the solid.
     * \param color solid color.
     */
    void setSolidColor ( const QColor& color );
    /*!
     * Apply an additional spatial transformation (for assemblies).
     * \param transform transformation of solid origin.
     */
    void setTransform ( const gp_Trsf& transform ) { transformation_ = transform; }

  public slots:
    void setName ( const QString& name );
    void updateName ( const QString& name );

  public:
    // Other methods
    void editInformation ( void );
    void setCursor ( GLuint selection_name );
    void setCursor ( const QCursor* cursor );
    void unsetCursor ( void );

    lC::ValidDelta adjust ( GLuint selection_name, const Point& last_pnt,
			    Point& current_pnt );

  private:
    void init ( void );
    void editSolidInformation ( void );

  private slots:
    void updateTessellation ( void );
    void updateHiddenOutline ( const GLdouble* modelview );
    void updateMaterial ( void );
    void updateViewNormal ( const GLdouble* modelview );

  private:

    static PartInfoDialog* part_info_dialog_;
    static ParameterInfoDialog* parameter_info_dialog_;
  };
} // End of Space3D namespace

#endif // SOLIDVIEW_H
