/* -*- c++ -*-
 * rectangle.h
 *
 * Header for the Rectangle class
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
#ifndef RECTANGLE_H
#define RECTANGLE_H

#include "figure.h"

class QDomElement;
class ModelItem;

namespace lC {
  class ValidDelta;
};

namespace Space2D {
  class Line;
  class ConstrainedLine;
  class OffsetLine;
  class Dimension;

  class Rectangle : public Figure {
    Q_OBJECT

    static uint unique_index_;

    ConstrainedLine* x0_;
    ConstrainedLine* y0_;
    ConstrainedLine* x1_;
    ConstrainedLine* y1_;

  public:
    Rectangle ( uint id, const QString& name, Page* parent );
    Rectangle ( uint id, const QDomElement& xml_rep, Page* parent );
    Rectangle ( uint id, const QString& name, const QDomElement& xml_rep, Page* parent );
    ~Rectangle ( void );

    ModelItem* lookup ( QStringList& path_components ) const;

    ConstrainedLine* x0 ( void ) const { return x0_; }
    ConstrainedLine* y0 ( void ) const { return y0_; }
    ConstrainedLine* x1 ( void ) const { return x1_; }
    ConstrainedLine* y1 ( void ) const { return y1_; }

    ConstrainedLine* left ( void ) const;
    ConstrainedLine* right ( void ) const;
    ConstrainedLine* bottom ( void ) const;
    ConstrainedLine* top ( void ) const;

    Point origin ( void ) const;
    Point corner ( void ) const;
    double width ( void ) const;
    double height ( void ) const;

    static QString newName ( void );

    void initializeEdges ( Point p, Line* x_reference, Line* y_reference );
    void moveEdges ( Point p );
    void finishEdges ( Point p, Line* x_reference, Line* y_reference);

    Point lbVertex ( void ) const;
    Point rbVertex ( void ) const;
    Point rtVertex ( void ) const;
    Point ltVertex ( void ) const;

    void setOrigin ( const Point& origin );
    void setWidth ( double width );
    void setHeight ( double height );

    Vertex closestVertex( Point p, Curve* curve ) const;
    Segment closestVertices( Curve* curve ) const;
    bool needsExtensionLine ( void ) const { return true; }

    lC::ValidDelta move ( const Vector& delta );
    lC::ValidDelta move ( const Point& last_pnt, Point& current_pnt );
    lC::ValidDelta resizeLeft ( const Point& last_pnt, Point& current_pnt );
    lC::ValidDelta resizeRight ( const Point& last_pnt, Point& current_pnt );
    lC::ValidDelta resizeBottom ( const Point& last_pnt, Point& current_pnt );
    lC::ValidDelta resizeTop ( const Point& last_pnt, Point& current_pnt );
    lC::ValidDelta resizeLeftBottom ( const Point& last_pnt, Point& current_pnt );
    lC::ValidDelta resizeRightBottom ( const Point& last_pnt, Point& current_pnt );
    lC::ValidDelta resizeLeftTop ( const Point& last_pnt, Point& current_pnt );
    lC::ValidDelta resizeRightTop ( const Point& last_pnt, Point& current_pnt );
    /*!
     * Create a full XML representation of this rectangle suitable for
     * saving in a file with the rest of the model information.
     * \param xml_rep the XML representation of the model to append
     * this representation to.
     */
    void write ( QDomElement& ) const;
    /*!
     * Create an XML representation of this rectangle suitable for use
     * as a clipboard selection. The rectangle is simplified to just
     * an origin and a width and height; the constraint information is
     * discarded.
     * \param xml_rep the XML representation of the selection to append
     * this representation to.
     */
    void copyWrite ( QDomElement& ) const;
    /*!
     * Examine the rectangle's edges and see if the given (display) name
     * is unique.
     * \param geometry reference to edge being renamed.
     * \param name new name in display format.
     * \return true if new name is OK.
     */
    bool uniqueGeometryName ( const Geometry* geometry, const QString& name) const;
  signals:
    void initialized ( void );
    void moved ( void );
    void resized ( void );

  private slots:
    void x0ModifiedConstraint ( void );
    void y0ModifiedConstraint ( void );
    void x1ModifiedConstraint ( void );
    void y1ModifiedConstraint ( void );
  };
} // End of Space2D namespace

#endif // RECTANGLE_H
