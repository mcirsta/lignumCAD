/* -*- c++ -*-
 * axis.h
 *
 * Header for the Axis classes
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
#ifndef AXIS_H
#define AXIS_H

#include <gp_Ax2.hxx>
#include <Geom_Axis2Placement.hxx>

#include "constants.h"
#include "constructiondatum.h"

namespace Space3D {
  /*!
   * Axis is the drawable version of the OpenCASCADE gp_Ax2 (essentially
   * a right handed coordinate system).
   */
  class Axis : public ConstructionDatum {
    Q_OBJECT

  public:
    Axis ( uint id, const QString& name, Page* parent )
      : ConstructionDatum( id, name, lC::STR::AXIS, parent ), axis_( gp::XOY() ),
	min_( 0 ), max_( 0 )
      {}
    virtual ~Axis ( void ) {}
    /*!
     * Set the minimum position (i.e., z near) of the axis extent (purely
     * cosmetic).
     * \param min distance from origin to the near end of the axis.
     */
    void setMinimum ( double min )
    {
      min_ = min;
    }
    //! \return the position in space of the near end of the axis.
    gp_Pnt minimum ( void ) const;
    /*!
     * Set the maximum position (i.e., z far) of the axis extent (purely
     * cosmetic).
     * \param max distance from origin to the far end of the axis.
     */
    void setMaximum ( double max )
    {
      max_ = max;
    }
    //! \return the position in space of the far end of the axis.
    gp_Pnt maximum ( void ) const;
    /*!
     * Create a full XML representation of this annotation suitable for
     * saving in a file with the rest of the model information.
     * \param xml_rep the XML representation of the model to append
     * this representation to.
     */
    void write ( QDomElement& /*xml_rep*/ ) const {}
    /*!
     * Create an XML representation of this annotation suitable for use
     * as a clipboard selection. (Currently, this is the same as the
     * regular write() method.)
     * \param xml_rep the XML representation of the selection to append
     * this representation to.
     */
    void copyWrite ( QDomElement& /*xml_rep*/ ) const {}
    /*!
     * Given the point and the curve, find the point on the annotation
     * which is closest to them. For annotation, this always returns
     * the origin.
     * \param p proximity point.
     * \param curve proximity curve.
     * \return point and curve which are closest to the given proximity
     * position.
     */
    Vertex closestVertex( Point /*p*/, Curve* curve ) const { return Vertex(curve); }
    /*!
     * Lookup any subcomponents of the construction datum (but it doesn't
     * have any).
     * \return 0: Annotations don't have subcomponents.
     */
    ModelItem* lookup ( QStringList& /*path_components*/ ) const { return 0; }
    /*!
     * Lookup any subcomponents of the construction datum (but it doesn't
     * have any).
     * \return 0: Construction datums don't have subcomponents.
     */
    ModelItem* lookup ( QValueVector<uint>& /*id_path*/ ) const { return 0; }
    /*!
     * Find the OpenCASCADE type of the given surface (identified by
     * path). In this case, returns the Geom version.
     * \param path_components path to solid.
     * \return OpenCASCADE standard type identifier.
     */
    Handle(Standard_Type) lookupType ( QStringList& /*path_components*/ ) const
    { return STANDARD_TYPE(Geom_Axis2Placement); }
    /*!
     * Find the OpenCASCADE type of the given surface (identified by
     * path). In this case, returns the Geom version.
     * \param id_path id path to solid.
     * \return OpenCASCADE standard type identifier.
     */
    Handle(Standard_Type) lookupType ( QValueVector<uint>& /*path_components*/ ) const
    { return STANDARD_TYPE(Geom_Axis2Placement); }
    /*!
     * Look up the topology of an OpenCASCADE shape. Should return
     * a shape which has the proper Location based on its traversal
     * of the assembly hierarchy.
     * \param path_components string list of path components.
     */
    TopoDS_Shape lookupShape ( QStringList& path_components ) const;
    /*!
     * Look up the topology of an OpenCASCADE shape. Should return
     * a shape which has the proper Location based on its traversal
     * of the assembly hierarchy.
     * \param id_path id path to solid.
     */
    TopoDS_Shape lookupShape ( QValueVector<uint>& id_path ) const;
    /*!
     * Construct a string path to the specified item. May be either the
     * figure itself or one of its geometries or subfigures.
     * \param id_path id path to item.
     * \return string encoded path to item.
     */
    QString idPath ( QValueVector<uint> /*id_path*/ ) const { return QString::null;}
    /*!
     * Construct a id path to the specified item. May be either the
     * figure itself or one of its geometries or subfigures.
     * \param id_path id path to item.
     * \return string encoded path to item.
     */
    void pathID ( QStringList& /*path_components*/, QValueVector<uint>& /*id_path*/)
      const
    {}

  private:
    //! Essentially a right handed coordinate system.
    gp_Ax2 axis_;
    //! Distance from the origin of the near end of the axis.
    double min_;
    //! Distance from the origin of the far end of the axis.
    double max_;
  };
} // end of Space3D namespace
#endif // AXIS_H
