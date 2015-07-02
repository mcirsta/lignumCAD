/* -*- c++ -*-
 * dimensionview.h
 *
 * Header for the Dimension View class
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
#ifndef DIMENSIONVIEW_H
#define DIMENSIONVIEW_H

#include <qcolor.h>
#include <qdom.h>

#include "graphics.h"
#include "dimension.h"
#include "view.h"

class OpenGLBase;
class DimensionInfoDialog;
class PageView;

namespace Space2D {
  class Curve;
  class Dimension;
  class DimensionView;
  class ConstrainedLine;
  class FigureView;

  class DimensionCreateInput : public InputObject {
    Q_OBJECT

    QMenu* context_menu_;
    int separator_id_;

    PageView* parent_;

    uint n_geometries_;
    QVector< Point > points_;
    QList< ConstrainedLine > lines_;

    ConstrainedLine* from_reference_;
    QDomDocument* xml_rep_;
    QDomElement from_ref_xml_;

  public:
    DimensionCreateInput ( PageView* parent );
    ~DimensionCreateInput ( void );
    SelectionType selectionType ( void ) const
    {
      return SelectionType( PICK, EDGE );
    }
    bool needsPrepressMouseCoordinates ( void ) const { return true; }
    void startDisplay ( QMenu* context_menu );
    void stopDisplay ( QMenu* /*context_menu*/ ) {}
    void mousePrepress ( QMouseEvent* me, const SelectedNames& selected );
    void mousePress ( QMouseEvent* me, const SelectedNames& selected );
    void mouseDrag ( QMouseEvent* me, const SelectedNames& selected );
    bool mouseRelease ( QMouseEvent* me, const SelectedNames& selected );
    void mouseDoubleClick ( QMouseEvent* me ) { (void)me; }
    void keyPress ( QKeyEvent* ke );

  public slots:
    void cancelOperation ( void );
    
  private slots:
    void cascadeConstraints ( void );
  };

  class DimensionModifyInput : public InputObject {
    Q_OBJECT

    SelectionType selection_type_;

    ConstrainedLine* dimensioned_line_;
    DimensionView* dimension_view_;

    double old_extension_offset_;
    double old_dimension_offset_;
    enum Dimension::Placement old_placement_;

    Point start_pnt_;
    Point last_pnt_;
    Point current_pnt_;

    bool free_;

  public:
    DimensionModifyInput ( ConstrainedLine* dimension,
			   DimensionView* dimension_view );
    SelectionType selectionType ( void ) const { return selection_type_; }
    bool needsPrepressMouseCoordinates ( void ) const { return true; }
    void startDisplay ( QMenu* context_menu );
    void stopDisplay ( QMenu* /*context_menu*/ ) {}
    void mousePrepress ( QMouseEvent* me, const SelectedNames& selected );
    void mousePress ( QMouseEvent* me, const SelectedNames& selected );
    void mouseDrag ( QMouseEvent* me, const SelectedNames& selected );
    bool mouseRelease ( QMouseEvent* me, const SelectedNames& selected );
    void mouseDoubleClick ( QMouseEvent* me );
    void keyPress ( QKeyEvent* /*ke*/ ) {}

    // Other methods
    void setDimension ( ConstrainedLine* dimension );

  public slots:
    void cancelOperation ( void ) {}
  };

  /*!
   * The Dimension View holds the bits of geometry which the dimension
   * uses as its basis and the ModifyInput object. It also contains
   * some of the attributes of the dimension, such as its orientation.
   * The work of rendering the dimension is done by the Dimension class.
   */
  class DimensionView : public View, public GraphicsView {
    FigureView* parent_;

    ConstrainedLine* dimensioned_line_;
    Curve* from_reference_;
    Curve* to_reference_;

    Dimension* dimension_;

    DimensionModifyInput modify_input_;

  public:
    DimensionView ( ConstrainedLine* dimension,
		    Curve* from_reference, Curve* to_reference,
		    lC::Orientation orientation, lC::AnnotationSide annotation_side,
		    GLuint selection_name,
		    FigureView* parent );
    DimensionView ( const QDomElement& xml_rep, GLuint selection_name,
		    FigureView* parent );
    virtual ~DimensionView ( void );

    QString name ( void ) const;
    DBURL dbURL ( void ) const;

    InputObject* createInput ( void ) { return 0; }
    InputObject* modifyInput ( void ) { return &modify_input_; }

    FigureView* parent ( void ) const { return parent_; }

    ConstrainedLine* dimension ( void ) const { return dimensioned_line_; }
    Curve* fromReference ( void ) const { return from_reference_; }
    Curve* toReference ( void ) const { return to_reference_; }

    void setDimensionAttributes ( double extension_offset, double dimension_offset,
				  enum Dimension::Placement placement );

    double extensionOffset ( void ) const;
    double dimensionOffset ( void ) const;
    enum Dimension::Placement placement ( void ) const;

    void computeLayout ( void );

    void draw ( void ) const;
    void select ( SelectionType select_type ) const;

    void editInformation ( void );

    lC::AnnotationSide annotationSide ( void ) const { return annotation_side_; }
    void setAnnotationSide ( lC::AnnotationSide annotation_side );

    void setReferences ( Curve* from_reference, Curve* to_reference );

    bool adjust ( bool free, const Point& last_pnt, Point& current_pnt );

    void write ( QDomElement& xml_rep );

  private:
    void init ( void );
    void setName ( const QString& name );

    lC::Orientation orientation_; // This may be in the wrong place?
    lC::AnnotationSide annotation_side_;

    static DimensionInfoDialog* dimension_info_dialog_;
  };
} // End of Space2D namespace

#endif // DIMENSIONVIEW_H
