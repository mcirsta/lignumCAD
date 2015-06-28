/* -*- c++ -*-
 * referenceline.h
 *
 * Header for the Reference Line classes
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
#ifndef REFERENCELINE_H
#define REFERENCELINE_H

#include "figure.h"

namespace Space2D {
  class Page;
  class Line;
  class ConstrainedLine;

  class ReferenceLine : public Figure {
    Q_OBJECT

    ConstrainedLine* line_;

    static uint unique_index_;

  public:
    ReferenceLine ( uint id, const QString& name, Page* parent );
    ReferenceLine ( uint id, const QDomElement& xml_rep, Page* parent );
    ReferenceLine ( uint id, const QString& name, const QDomElement& xml_rep,
		    Page* parent );
    ~ReferenceLine ( void );

    // Internal state reflection
    ConstrainedLine* line ( void ) const { return line_; }
    /*!
     * Adjust the position of the reference line by the given amount
     * (mostly for the benefit of the Paste adjustment).
     * \param delta distance in inches to move the reference line.
     */
    void move ( const Vector& delta );

    // Implementation of Figure interface
    Vertex closestVertex( Point p, Curve* curve ) const;
    Segment closestVertices( Curve* curve ) const;
    bool needsExtensionLine ( void ) const { return false; }

    ModelItem* lookup ( QStringList& path_components ) const;

    // Implementation of Model Item interface
  public slots:
    void setName ( const QString& name );
  public:
    /*!
     * Create a full XML representation of this reference line suitable for
     * saving in a file with the rest of the model information.
     * \param xml_rep the XML representation of the model to append
     * this representation to.
     */
    void write ( QDomElement& xml_rep ) const;
    /*!
     * Create an XML representation of this reference line suitable for use
     * as a clipboard selection. The reference line is simplified;
     * in particular, the constraint information is discarded.
     * \param xml_rep the XML representation of the selection to append
     * this representation to.
     */
    void copyWrite ( QDomElement& xml_rep ) const;
    /*!
     * See if the given (display) name is unique. Always true since there
     * is only one geometry object.
     * \param geometry reference to edge being renamed.
     * \param name new name in display format.
     * \return true if new name is OK.
     */
    bool uniqueGeometryName ( const Geometry* /*geometry*/,
			      const QString& /*name*/ ) const
    { return true; }

    // Global methods
    static QString newName ( void );

  signals:
    void modified();
  private slots:
    void lineModified ( void );
    void lineRenamed ( QString );
  };
} // End of Space2D namespace

#endif // REFERENCELINE_H
