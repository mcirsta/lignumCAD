/* -*- c++ -*-
 * figure.h
 *
 * Header for the Figure classes
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
#ifndef FIGURE_H
#define FIGURE_H

#include <Standard_Type.hxx>

#include "modelitem.h"
#include "vectoralgebra.h"

class Model;
class QStringList;
class QDomElement;
class TopoDS_Shape;

class FigureBase : public ModelItem {
public:
  FigureBase ( uint id, const QString& name, const QString& type );
  virtual ~FigureBase ( void ) {}
  /*!
   * Create a full XML representation of this object suitable for
   * saving in a file with the rest of the model information.
   * \param xml_rep the XML representation of the model to append
   * this representation to.
   */
  virtual void write ( QDomElement& xml_rep ) const = 0;
  /*!
   * Create an XML representation of the object suitable for use
   * as a clipboard selection. Generally, this will be simplified
   * compared to the full representation.
   * \param xml_rep the XML representation of the selection to append
   * this representation to.
   */
  virtual void copyWrite ( QDomElement& xml_rep ) const = 0;
};

namespace Space2D {
  class Page;
  class Geometry;
  class Curve;

  // An object representing the intersection of this curve with another.

  struct Vertex {
    Curve* curve1_;
    Curve* curve2_;
    Point p_;

    Vertex ( Curve* curve )
      : curve1_( curve ), curve2_( 0 )
    {}
  };

  // Sort of the converse of the above: An object representing a
  // (possibly) finite segment of a curve

  struct Segment {
    Curve* curve_;
    Point from_;
    Point to_;
    bool valid_from_;
    bool valid_to_;

    Segment ( Curve* curve = 0,
	      bool valid_from = false, bool valid_to = false )
      : curve_( curve ), valid_from_( valid_from ), valid_to_( valid_to )
    {}
  };

  class Figure : public FigureBase {
    Q_OBJECT

    Page* parent_;

    bool valid_;
    bool complete_;

  public:
    Figure ( uint id, const QString& name, const QString& type, Page* parent );
    virtual ~Figure ( void ) {}

    Page* parent ( void ) const { return parent_; }

    Model* model ( void ) const;

    bool isValid ( void ) const { return valid_; }
    void setValid ( bool valid );

    bool isComplete ( void ) const { return complete_; }
    void setComplete ( bool complete );

    QVector<uint> ID ( void ) const;
    QString path ( void ) const;

    virtual Vertex closestVertex( Point p, Curve* curve ) const = 0;
    virtual Segment closestVertices( Curve* curve ) const = 0;
    virtual bool needsExtensionLine ( void ) const = 0;

    virtual ModelItem* lookup ( QStringList& path_components ) const = 0;
    /*!
     * Examine the figure's children and see if the given (display) name
     * is unique.
     * \param geometry reference to geometry being renamed.
     * \param name new name in display format.
     * \return true if new name is OK.
     */
    virtual bool uniqueGeometryName ( const Geometry* geometry, const QString& name)
      const = 0;
  public slots:
    void pageRenamed ( const QString& name );
  };
} // End of Space2D namespace

namespace Space3D {
  class Curve;
  class Page;

  // An object representing the intersection of this curve with another.

  struct Vertex {
    Curve* curve1_;
    Curve* curve2_;
    Point p_;

    Vertex ( Curve* curve ) : curve1_( curve ), curve2_( 0 ) {}
  };

  class Figure : public FigureBase {
    Q_OBJECT

    Page* parent_;

    bool valid_;
    bool complete_;

  public:
    Figure ( uint id, const QString& name, const QString& type, Page* parent );
    virtual ~Figure ( void ) {}

    Page* parent ( void ) const { return parent_; }

    Model* model ( void ) const;

    bool isValid ( void ) const { return valid_; }
    void setValid ( bool valid );

    bool isComplete ( void ) const { return complete_; }
    void setComplete ( bool complete );

    QValueVector<uint> ID ( void ) const;
    QString path ( void ) const;
#if 0
    virtual Vertex closestVertex( Point p, Curve* curve ) const = 0;
#endif
    virtual ModelItem* lookup ( QStringList& path_components ) const = 0;
    virtual ModelItem* lookup ( QValueVector<uint>& id_path ) const = 0;
    virtual Handle(Standard_Type) lookupType ( QStringList& path_components ) const = 0;
    virtual Handle(Standard_Type) lookupType ( QValueVector<uint>& id_path ) const = 0;
    virtual TopoDS_Shape lookupShape ( QStringList& path_components ) const = 0;
    virtual TopoDS_Shape lookupShape ( QValueVector<uint>& id_path ) const = 0;
    /*!
     * Construct a string path to the specified item. May be either the
     * figure itself or one of its geometries or subfigures.
     * \param id_path id path to item.
     * \return string encoded path to item.
     */
    virtual QString idPath ( QValueVector<uint> id_path ) const = 0;
    /*!
     * Construct the id path to the specified item. May be either the figure
     * itself or one of its geometries or subfigures.
     * \param path_components broken down components of the orignal DBURL with
     * paths to current item removed.
     * \param id_path id path to update with id's.
     */
    virtual void pathID ( QStringList& path_components, QValueVector<uint>& id_path ) const = 0;

  public slots:
    void pageRenamed ( const QString& name );
  };
} // End of Space3D namespace

#endif // FIGURE_H
