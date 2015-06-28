/* -*- c++ -*-
 * centerlineview.h
 *
 * Header for the Centerline View classes
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
#ifndef CENTERLINEVIEW_H
#define CENTERLINEVIEW_H

#include <qintdict.h>
#include <qdom.h>

#include "vectoralgebra.h"
#include "figureview.h"

class CenterlineInfoDialog;

namespace Space2D {
  class Page;
  class Centerline;
  class CenterlineView;
  class DimensionView;

  class CenterlineCreateInput : public InputObject {
    Q_OBJECT

    QPopupMenu* context_menu_;
    int separator_id_;

    Centerline* centerline_;
    CenterlineView* centerline_view_;
    Point start_;

    static QChar centerline_symbol_;

  public:
    CenterlineCreateInput ( Centerline* centerline,
			       CenterlineView* centerline_view );
    // Implemention of InputObject interface
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

  class CenterlineModifyInput : public InputObject {
    Q_OBJECT

    SelectionType selection_type_;

    Centerline* centerline_;
    CenterlineView* centerline_view_;

    GLuint drag_handle_;
    double old_offset_;
    Point start_pnt_;
    Point last_pnt_;
    Point current_pnt_;

    QDomDocument* xml_rep_;
    QDomElement from_ref_xml_;

  public:
    CenterlineModifyInput ( Centerline* centerline,
			       CenterlineView* centerline_view );

    // Implemention of InputObject interface
    SelectionType selectionType ( void ) const { return selection_type_; }
    bool needsPrepressMouseCoordinates ( void ) const { return true; }
    void startDisplay ( QPopupMenu* /*context_menu*/ ) {}
    void stopDisplay ( QPopupMenu* /*context_menu*/ ) {}
    void mousePrepress ( QMouseEvent* me, const SelectedNames& selected );
    void mousePress ( QMouseEvent* me, const SelectedNames& selected );
    void mouseDrag ( QMouseEvent* me, const SelectedNames& selected );
    bool mouseRelease ( QMouseEvent* me, const SelectedNames& selected );
    void mouseDoubleClick ( QMouseEvent* me );
    void keyPress ( QKeyEvent* /*me*/ ) {}

    // Other methods
    void setCenterline ( Centerline* centerline );

  public slots:
    void cancelOperation ( void ) {}
  };

  class CenterlineView : public FigureView {
    Q_OBJECT

    Centerline* centerline_;

    ListViewItem* list_view_item_;
    ListViewItem* line_list_view_;

    DimensionView* dimension_view_;

    GraphicsView line_;
    GraphicsView handle0_;
    GraphicsView handle1_;

    CenterlineCreateInput create_input_;
    CenterlineModifyInput modify_input_;

    QIntDict< GraphicsView > centerline_objects_;
    QIntDict< DimensionView > dimensionview_objects_;

  public:
    CenterlineView ( Centerline* centerline, PageView* parent );
    CenterlineView ( const QDomElement& xml_rep, PageView* parent );
    CenterlineView ( const QString& name, const QDomElement& xml_rep,
		     Page* page, PageView* parent );
    ~CenterlineView ( void );

    Centerline* centerline ( void ) const { return centerline_; }

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
    QString selectionText ( const vector<GLuint>& selection_name,
			    SelectionEntity entity ) const;
    View* lookup ( QStringList& path_components ) const;

    void setHighlighted ( bool highlight, SelectionEntity entity,
			  const vector<GLuint>& items );
    void setActivated( bool activate, SelectionEntity entity,
		       const vector<GLuint>& items );

    InputObject* createInput ( void ) { return &create_input_; }
    InputObject* modifyInput ( void );

    CreateObject* memento ( void );

    uint dimensionPickCount ( void ) const { return 3; }
    void setDimensionView ( const QDomElement& xml_rep );

    void viewAttributeChanged ( void );

    bool isGeometry ( GLuint selection_name ) const;
    Curve* geometry ( GLuint selection_name ) const;
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

  public:
    // Other methods
    void editInformation ( void );
    void setCursor ( GLuint selection_name );
    void setCursor ( const QCursor* cursor );
    void unsetCursor ( void );

  private:
    static CenterlineInfoDialog* centerline_info_dialog_;

    void init ( void );
    void editCenterlineInformation ( void );

  private slots:
    /*!
     * Set the centerline to the given name. Assumes this is an interactive
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
    void updateName ( const QString& name );
    void modifiedConstraint ( void );
    void modifiedPosition ( void );
    void dmvRefModified ( void );
    void destroyedDmVRef ( void );
    void updateDimensionViews ( void );
  };
} // End of Space2D namespace

#endif // CENTERLINEVIEW_H
