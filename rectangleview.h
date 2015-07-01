/* -*- c++ -*-
 * rectangleview.h
 *
 * Header for the Rectangle View class
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
#ifndef RECTANGLEVIEW_H
#define RECTANGLEVIEW_H

#include <map>
#include <qdom.h>

#include "line.h"
#include "figureview.h"
#include "handle.h"

class Style;
class RectangleInfoDialog;

namespace lC {
  class ValidDelta;
}

namespace Space2D {
  class Page;
  class DimensionView;
  class Rectangle;
  class RectangleView;

  class RectangleCreateInput : public InputObject {
    Q_OBJECT

    QPopupMenu* context_menu_;
    int separator_id_;

    Rectangle* rectangle_;
    RectangleView* rectangle_view_;

  public:
    RectangleCreateInput ( Rectangle* rectangle, RectangleView* rectangle_view );
    SelectionType selectionType ( void ) const
    {
      return SelectionType( TRANSIENT, EDGE );
    }
    bool needsPrepressMouseCoordinates ( void ) const { return true; }
    void startDisplay ( QPopupMenu* context_menu );
    void stopDisplay ( QPopupMenu* /*context_menu*/ ) {}
    void mousePrepress ( QMouseEvent* me, const SelectedNames& selected );
    void mousePress ( QMouseEvent* me, const SelectedNames& selected );
    void mouseDrag ( QMouseEvent* me, const SelectedNames& selected );
    bool mouseRelease ( QMouseEvent* me, const SelectedNames& selected );
    void mouseDoubleClick ( QMouseEvent* /*me*/ ) {}
    void keyPress ( QKeyEvent* ke );

  public slots:
    void cancelOperation ( void );
  };

  class RectangleModifyInput : public InputObject {
    Q_OBJECT

    SelectionType selection_type_;
    Rectangle* rectangle_;
    RectangleView* rectangle_view_;
    GLuint drag_handle_;
    Point start_pnt_;
    Point last_pnt_;
    Point current_pnt_;

    QPtrList< ConstrainedLine > edges_;

    double old_x0_offset_;
    double old_y0_offset_;
    double old_x1_offset_;
    double old_y1_offset_;

    QDomDocument* xml_rep_;
    QDomElement from_ref_xml_;

  public:
    RectangleModifyInput ( Rectangle* rectangle, RectangleView* rectangle_view );
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

    void setRectangle ( Rectangle* rectangle );

  public slots:
    void cancelOperation ( void ) {}

  private slots:
    void cascadeConstraints ( void );
  };

  class RectangleView : public FigureView {
    Q_OBJECT

    Rectangle* rectangle_;

    RectangleCreateInput create_input_;
    RectangleModifyInput modify_input_;

    DimensionView* x0_dimensionview_;
    DimensionView* y0_dimensionview_;
    DimensionView* x1_dimensionview_;
    DimensionView* y1_dimensionview_;

    lC::AnnotationSide width_annotation_side_;
    lC::AnnotationSide height_annotation_side_;

    ListViewItem* list_view_item_;

    ListViewItem* x0_list_view_;
    ListViewItem* y0_list_view_;
    ListViewItem* x1_list_view_;
    ListViewItem* y1_list_view_;

    GraphicsView left_edge_;
    GraphicsView right_edge_;
    GraphicsView bottom_edge_;
    GraphicsView top_edge_;

    GraphicsView solid_rectangle_;

    GraphicsView lb_handle_;
    GraphicsView mb_handle_;
    GraphicsView rb_handle_;
    GraphicsView mr_handle_;
    GraphicsView rt_handle_;
    GraphicsView mt_handle_;
    GraphicsView lt_handle_;
    GraphicsView ml_handle_;

    QIntDict< GraphicsView > rectangle_objects_;
    QIntDict< DimensionView > dimensionview_objects_;

    std::map< int, lC::ValidDelta( Rectangle::* ) ( const Point&, Point& )> adjustments_;

    //! Optional style for the rectangle. Otherwise inherits page default.
    Style* style_;

  public:
    RectangleView ( Rectangle* rectangle, PageView* parent );
    RectangleView ( const QDomElement& xml_rep, PageView* parent );
    RectangleView ( const QString& name, const QDomElement& xml_rep,
		    Page* page, PageView* parent );
    ~RectangleView ( void );

    Rectangle* rectangle ( void ) const { return rectangle_; }

    InputObject* createInput ( void ) { return &create_input_; }
    InputObject* modifyInput ( void );

    CreateObject* memento ( void );

    QString nameOf ( GLuint name ) const;

    void setWidthAnnotationSide( lC::AnnotationSide width_annotation_side );
    void setHeightAnnotationSide( lC::AnnotationSide height_annotation_side );
    /*!
     * Set the view's style. View takes ownership of object. A style of 0
     * resets the rectangle view's style to be inherited from the page.
     * \param style new rectangle view style.
     */
    void setStyle ( Style* style );

    // Implementation of FigureView interface

    //! \return the list view item for this page view.
    ListViewItem* listViewItem ( void ) const { return list_view_item_; }
    uint id ( void ) const;
    QString name ( void ) const;
    QString type ( void ) const;
    DBURL dbURL ( void ) const;
    QString selectionText ( const std::vector<GLuint>& selection_name,
			    SelectionEntity entity ) const;
    View* lookup ( QStringList& path_components ) const;

    void setHighlighted ( bool highlight, SelectionEntity entity,
			  const std::vector<GLuint>& items );
    void setActivated ( bool activate, SelectionEntity entity,
			const std::vector<GLuint>& items );

    uint dimensionPickCount ( void ) const { return 2; }
    void setDimensionView ( const QDomElement& xml_rep );

    bool isGeometry ( GLuint selection_name ) const;
    Curve* geometry ( GLuint selection_name ) const;
    FigureBase* figure ( void ) const;

    bool isDimensionView ( GLuint selection_name ) const
    {
      return dimensionview_objects_[selection_name] != 0;
    }
    bool isActivated ( GLuint selection_name = 0 ) const;

    void viewAttributeChanged ( void );
    void updateDimensions ( void ) const;

    void editInformation ( void );
  
    void draw ( void ) const;
    void select ( SelectionType select_type ) const;

    QPtrList< ConstrainedLine > dragEdges (  GLuint selection_name ) const;

    lC::ValidDelta adjust ( int selection_name, const Point& last_pnt,
			    Point& current_pnt );

    void setCursor ( GLuint selection_name );
    void setCursor ( const QCursor* cursor );
    void unsetCursor ( void );
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

  private slots:
    /*!
     * Set the rectangle to the given name. Assumes this is an interactive
     * modification and records an undo/redo command.
     * \param name new name.
     */
    void setName ( const QString& );
    /*!
     * The name in the list view has changed. This has to be checked for
     * redundancy before it can be allowed.
     * \param name new name for list view edit.
     */
    void listNameChanged ( const QString& name );
    void x0SetName ( const QString& );
    void y0SetName ( const QString& );
    void x1SetName ( const QString& );
    void y1SetName ( const QString& );
    void updateName ( const QString& );
    void x0UpdateName ( const QString& );
    void y0UpdateName ( const QString& );
    void x1UpdateName ( const QString& );
    void y1UpdateName ( const QString& );
    void rectangleInitialized ( void );
    void rectangleMoved ( void );
    void rectangleResized ( void );
    void x0ModifiedConstraint ( void );
    void y0ModifiedConstraint ( void );
    void x1ModifiedConstraint ( void );
    void y1ModifiedConstraint ( void );
    void x0DmVDimModified ( void );
    void y0DmVDimModified ( void );
    void x1DmVDimModified ( void );
    void y1DmVDimModified ( void );
    void x0DmVRefModified ( void );
    void y0DmVRefModified ( void );
    void x1DmVRefModified ( void );
    void y1DmVRefModified ( void );
    void x0DestroyedDmVRef ( void );
    void y0DestroyedDmVRef ( void );
    void x1DestroyedDmVRef ( void );
    void y1DestroyedDmVRef ( void );
    void x0ModifiedPosition ( void );
    void y0ModifiedPosition ( void );
    void x1ModifiedPosition ( void );
    void y1ModifiedPosition ( void );
    void updateDimensionViews ( void );

  private:
    void init ( void );

    //    static RectangleInformation* rectangle_information_;
    static RectangleInfoDialog* rectangle_info_dialog_;
    void editRectangleInformation ( void );
    /*!
     * Check to see if the edge name is unique in this rectangle.
     * \param name new name of edge.
     * \param role role of edge.
     * \return OK if unique, Rejected if the user decides not to carry out
     * the rename, or Redo to have a second chance for nameing it.
     */
    lC::RenameStatus uniqueEdgeName ( const QString& name, const QString& role )
      const;

    void drawTextured ( GLuint gl_texture_name, const QImage& image ) const;
  };
} // End of Space2D namespace

#endif // RECTANGLEVIEW_H
