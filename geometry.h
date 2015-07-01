/* -*- c++ -*-
 * geometry.h
 *
 * Header for the Geometry classes.
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
#ifndef GEOMTRY_H
#define GEOMTRY_H

#include <qstring.h>
#include <qtextstream.h>

#include "modelitem.h"

class Model;

// Geometries can exist in either 2D or 3D spaces.

namespace Space2D {
  class Figure;
  class Point;

  class Geometry : public ModelItem {
    Q_OBJECT

    Figure* parent_;

  public:
    Geometry ( uint id, const QString& name, const QString& type, Figure* parent );

    virtual ~Geometry ( void )
    {
      emit destroyedGeometry();
    }

    Figure* parent ( void ) const { return parent_; }

    Model* model ( void ) const;

    /*!
     * Determine the full hierarchical object id. The subclass
     * which knows the parent of this object should implement this method.
     * Note: The hierarchical object id is a QValueVector (as opposed to a
     * regular STL vector<>) since the Qt version is a QShared object,
     * which should improve performance as a return value.
     * \return the hierarchical object id.
     */
    QVector<uint> ID ( void ) const;

    /*!
     * Return the fully qualified path to this geometry. Note: it appears
     * that this has to be called from the destructor; so, Geometry
     * has to know enough about the object to construct its path.
     * \return fully qualified name of geometry.
     */
    QString path ( void ) const;
#if 0
    // All sorts of grim realities are ignored by this!
    virtual Point intersect ( const Geometry* g ) const = 0;
#endif
  public slots:
    void figureRenamed ( const QString& name );
  signals:
    void modifiedPosition ( void );
    void destroyedGeometry ( void );
  };
} // End of Space2D namespace

namespace Space3D {
  class Figure;
  class Curve;

  class Geometry : public ModelItem {
    Q_OBJECT

    Figure* parent_;

  public:
    Geometry ( uint id, const QString& name, const QString& type, Figure* parent );

    virtual ~Geometry ( void )
    {
      emit destroyedGeometry();
    }

    Figure* figure ( void ) const { return parent_; }

    Model* model ( void ) const;

    /*!
     * Determine the full hierarchical object id. The subclass
     * which knows the parent of this object should implement this method.
     * Note: The hierarchical object id is a QValueVector (as opposed to a
     * regular STL vector<>) since the Qt version is a QShared object,
     * which should improve performance as a return value.
     * \return the hierarchical object id.
     */
    QVector<uint> ID ( void ) const;

    /*!
     * Return the fully qualified path to this geometry. Note: it appears
     * that this has to be called from the destructor; so, Geometry
     * has to know enough about the object to construct its path.
     * \return fully qualified name of geometry.
     */
    QString path ( void ) const;
#if 0
    // All sorts of grim realities are ignored by this!
    virtual Geometry* intersect ( const Geometry* g ) const = 0;
#endif
  public slots:
    void figureRenamed ( const QString& name );
  signals:
    void modifiedPosition ( void );
    void destroyedGeometry ( void );
  };
} // End of Space3D namespace

#endif // GEOMETRY_H
