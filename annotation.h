/* -*- c++ -*-
 * annotation.h
 *
 * Header for the Annotation classes
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
#ifndef ANNOTATION_H
#define ANNOTATION_H

#include "figure.h"

namespace lC {
  class ValidDelta;
}

namespace Space2D {
  class Page;
  /*!
   * Annotations are blocks of text providing additional information
   * about the current view. Text can be styled to certain extent.
   */
  class Annotation : public Figure {
    Q_OBJECT

  public:
    /*!
     * Standard constructor invoked from the user interface.
     * \param id the id of the annotation.
     * \param name the name of the annotation.
     * \param parent parent page of the annotation.
     */
    Annotation ( uint id, const QString& name, Page* parent );
    /*!
     * Constructor invoked when reading a model from a file.
     * \param id the id of the annotation.
     * \param xml_rep XML representation of annotation.
     * \param parent parent page of the annotation.
     */
    Annotation ( uint id, const QDomElement& xml_rep, Page* parent );
    /*!
     * Constructor invoked when an annotation is pasted onto the page.
     * \param id the id of the annotation.
     * \param name new name of annotation (name in the XML representation is
     * ignored).
     * \param xml_rep XML representation of annotation on the clipboard.
     * \param parent new parent of the annotation (parent in the XML representation
     * is ignored).
     */
    Annotation ( uint id, const QString& name, const QDomElement& xml_rep, Page* parent );
    //! Destructor (Qt seems to clean up everything itself).
    ~Annotation ( void ) {}
    /*!
     * Set the origin of the annotation.
     * \param origin the new origin of the annotation, given in model coordinates
     * and in the basic unit (namely, inches).
     */
    void setOrigin ( const Point& origin );
    //! \return the origin of the annotation's text box, given in model
    //! coordinates and in the basic unit (namely, inches).
    Point origin ( void ) const { return origin_; }
    /*!
     * Set the text of the annotation.
     * \param text the new text of the annotation. Note that this is usually
     * in the form of Qt's default style sheet markup.
     */
    void setText ( QString text );
    //! \return the text of the annotation.
    QString text ( void ) const { return text_; }
    /*!
     * Record the origin of the annotation as it is interactively created.
     * \param origin initial origin of the annotation.
     */
    void initialize ( const Point& origin );
    /*!
     * Complete construction of the annotation.
     */
    void finish ( void );
    /*!
     * Move the annotation to a new origin.
     * \param delta the distance and direction to move the origin. Note that
     * this is an increment, not an absolute position.
     */
    void move ( const Vector& delta );

    // Implementation of Figure interface
    /*!
     * Given the point and the curve, find the point on the annotation
     * which is closest to them. For annotation, this always returns
     * the origin.
     * \param p proximity point.
     * \param curve proximity curve.
     * \return point and curve which are closest to the given proximity
     * position.
     */
    Vertex closestVertex( Point p, Curve* curve ) const;
    /*!
     * Determine the extents of the curve which is part of this figure.
     * (Annotation's don't have any curves, anyway.)
     * \param curve figure curve.
     * \return a structure containing the end points of the curve in
     * the figure (or none if the curve is unbounded).
     */
    Segment closestVertices( Curve* curve ) const;
    /*!
     * For aesthetic reasons, some figures don't need extension lines.
     * \return false: Annotations don't currently support dimensions anyway.
     */
    bool needsExtensionLine ( void ) const { return false; }
    /*!
     * Lookup any subcomponents of the annotation (but it doesn't
     * have any).
     * \return 0: Annotations don't have subcomponents.
     */
    ModelItem* lookup ( QStringList& /*path_components*/ ) const { return 0; }

  public slots:
    /*!
     * Standard name setting method. Annotations don't have any naming side
     * effects.
     * \param name new name.
     */
    void setName ( const QString name );

  public:
    /*!
     * Create a full XML representation of this annotation suitable for
     * saving in a file with the rest of the model information.
     * \param xml_rep the XML representation of the model to append
     * this representation to.
     */
    void write ( QDomElement& xml_rep ) const;
    /*!
     * Create an XML representation of this annotation suitable for use
     * as a clipboard selection. (Currently, this is the same as the
     * regular write() method.)
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

    //! \return a globally unique name for an Annotation.
    static QString newName ( void );

  signals:
    //! Emitted when the initialize method is invoked.
    void initialized ( void );
    //! Emitted when the setOrigin or move methods are invoked.
    void moved ( void );
    //! Emitted when the setText method is invoked.
    void textChanged ( void );

  private:
    //! The origin of the annotation text box in modeling coordinates, units
    //! are inches.
    Point origin_;
    //! Text of annotation.
    QString text_;

    //! Default annotation names look like Annotation[#]. This is the
    //! next # to dispense.
    static uint unique_index_;
  };
} // End of Space2D namespace

#endif // ANNOTATION_H
