/* -*- c++ -*-
 * referencelineview.h
 *
 * Header for the Reference Line View classes
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
#ifndef REFERENCELINEVIEW_H
#define REFERENCELINEVIEW_H

#include <qintdict.h>
#include <qdom.h>

#include "vectoralgebra.h"
#include "figureview.h"

class ReferenceLineInfoDialog;

namespace Space2D {
  class Page;
  class ReferenceLine;
  class ReferenceLineView;
  class DimensionView;

  class ReferenceLineCreateInput : public InputObject {
    Q_OBJECT

    QPopupMenu* context_menu_;
    int separator_id_;

    ReferenceLine* reference_line_;
    ReferenceLineView* reference_line_view_;
    Point start_;

  public:
    ReferenceLineCreateInput ( ReferenceLine* reference_line,
			       ReferenceLineView* reference_line_view );

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

  class ReferenceLineModifyInput : public InputObject {
    Q_OBJECT

    SelectionType selection_type_;

    ReferenceLine* reference_line_;
    ReferenceLineView* reference_line_view_;

    GLuint drag_handle_;
    double old_offset_;
    Point start_pnt_;
    Point last_pnt_;
    Point current_pnt_;

    QDomDocument* xml_rep_;
    QDomElement from_ref_xml_;

  public:
    ReferenceLineModifyInput ( ReferenceLine* reference_line,
			       ReferenceLineView* reference_line_view );

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
    void setReferenceLine ( ReferenceLine* reference_line );
  public slots:
    void cancelOperation ( void ) {}
  };

  class ReferenceLineView : public FigureView {
    Q_OBJECT

    ReferenceLine* reference_line_;

    ListViewItem* list_view_item_;
    ListViewItem* line_list_view_;

    DimensionView* dimension_view_;

    GraphicsView line_;
    GraphicsView handle0_;
    GraphicsView handle1_;

    ReferenceLineCreateInput create_input_;
    ReferenceLineModifyInput modify_input_;

    QIntDict< GraphicsView > reference_line_objects_;
    QIntDict< DimensionView > dimensionview_objects_;

  public:
    ReferenceLineView ( ReferenceLine* reference_line, PageView* parent );
    ReferenceLineView ( const QDomElement& xml_rep, PageView* parent );
    ReferenceLineView ( const QString& name, const QDomElement& xml_rep,
			Page* page, PageView* parent );
    ~ReferenceLineView ( void );

    ReferenceLine* referenceLine ( void ) const { return reference_line_; }

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
    QString selectionText ( const std::vector<GLuint>& selection_name,
			    SelectionEntity entity ) const;
    View* lookup ( QStringList& path_components ) const;

    void setHighlighted( bool highlight, SelectionEntity entity,
			 const std::vector<GLuint>& items );
    void setActivated( bool activate, SelectionEntity entity,
		       const std::vector<GLuint>& items );

    InputObject* createInput ( void ) { return &create_input_; }
    InputObject* modifyInput ( void );

    CreateObject* memento ( void );

    uint dimensionPickCount ( void ) const { return 2; }
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
    void init ( void );
    void editInformation ( void );
    void setCursor ( GLuint selection_name );
    void setCursor ( const QCursor* cursor );
    void unsetCursor ( void );

  private:
    static ReferenceLineInfoDialog* reference_line_info_dialog_;

    void editReferenceLineInformation ( void );

  private slots:
    /*!
     * Set the reference line to the given name. Assumes this is an interactive
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

#endif // REFERENCELINEVIEW_H
