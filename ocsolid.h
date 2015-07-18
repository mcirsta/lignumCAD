/* -*- c++ -*-
 * ocsolid.h
 *
 * Header for the Solid class based on OpenCASCADE
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
#ifndef SOLID_H
#define SOLID_H

#include <QVector>

#include <map>
#include <unordered_map>

#include <gp_Ax2.hxx>
#include <TopoDS_Solid.hxx>
#include <TopoDS_Face.hxx>

#include "part.h"
#include "figure.h"

namespace lC {
  class ValidDelta;
};
class Material;
class QDomElement;

namespace Space3D {
  class ConstructionDatum;
  class Axis;
  /*!
   * Parameter describes some dimensional attribute of the solid which
   * can be adjusted interactively.
   */
  class Parameter {
  public:
    //! Parameters can be of a certain type (mostly for annotation purposes
    //! so far)
    typedef enum { LINEAR, DIAMETER, ANGULAR } Type;
    /*!
     * Constructor.
     * \param title display string for the parameter.
     * \param type type of parameter.
     * \param value current value of parameter.
     * \param end0 position of first end of the dimension.
     * \param end1 position of the other end of the dimension.
     * \param normal orientation of extension lines.
     */
    Parameter ( const QString& title = QString::null,
		Type type = LINEAR, gp_Dir normal = gp::DZ(),
		double value = 0,
		gp_Pnt end0 = gp::Origin(), gp_Pnt end1 = gp::Origin() )
      : title_( title ), type_( type ), normal_( normal ), value_( value ),
	end0_( end0 ), end1_( end1 )
    {}
    //! \return the display string for the parameter.
    QString title ( void ) const { return title_; }
    //! \return the type of the parameter.
    Type type ( void ) const { return type_; }
    //! \return the orientation of the extension lines.
    gp_Dir normal ( void ) const { return normal_; }
    //! \return the value of the parameter.
    double value ( void ) const { return value_; }
    //! \return the first end of the dimension.
    gp_Pnt end0 ( void ) const { return end0_; }
    //! \return the other end of the dimension.
    gp_Pnt end1 ( void ) const { return end1_; }
    //! \return an optional notation for this parameter's dimension value.
    QString note ( void ) const
    {
      if ( type_ == DIAMETER )
	return QString( QChar( 0x2300 ) ); // UNICODE diameter symbol

      return QString::null;
    }
    /*!
     * Update the current value of the parameter.
     * \param value new value of parameter. May leave ends in an
     * inconsistent state.
     */
    void setValue ( double value )
    {
      value_ = value;
    }
    /*!
     * Update the positions of the dimension end points.
     * \param value new value of parameter.
     * \param end0 new position of the first end of the dimension.
     * \param end1 new position of the other end of the dimension.
     */
    void setValueEnds ( double value, gp_Pnt end0, gp_Pnt end1 )
    {
      value_ = value;
      end0_ = end0;
      end1_ = end1;
    }
  private:
    //! Display string for the dimensional parameter.
    QString title_;
    //! Type of the parameter.
    Type type_;
    //! Orientation of the extension lines.
    gp_Dir normal_;
    //! Current value of the dimensional parameter.
    double value_;
    //! Position of first end of the dimension.
    gp_Pnt end0_;
    //! Position of other end of the dimension.
    gp_Pnt end1_;
  };

  /*!
   * This is just a guess about what would be useful to store in the resize
   * handle representation.
   */
  class HandleData {
  public:
    /*!
     * Construct a handle data structure.
     * \param position the position of the handle in Cartesian coordinates.
     * \param resize_signs signum factors for each resize handle direction.
     */
    HandleData ( gp_XYZ resize_signs = gp_XYZ( 1, 1, 1 ),
		 gp_Pnt position = gp::Origin() )
      : resize_signs_( resize_signs ), position_( position )
    {}
    //! \return sign of X resize direction.
    double xResizeSign ( void ) const { return resize_signs_.X(); }
    //! \return sign of Y resize direction.
    double yResizeSign ( void ) const { return resize_signs_.Y(); }
    //! \return sign of Z resize direction.
    double zResizeSign ( void ) const { return resize_signs_.Z(); }
    /*!
     * Set the position of the handle.
     * \param position new position of handle.
     */
    void setPosition ( gp_Pnt position )
    {
      position_ = position;
    }
    //! \return position of handle.
    gp_Pnt position ( void ) const { return position_; }
  private:
    //! Signum factors for each resize handle.
    gp_XYZ resize_signs_;
    //! Position of resize handle in Cartesian coordinates.
    gp_Pnt position_;
  };
  /*!
   * OpenCASCADE provides a hashing function for its transient entities.
   * This function is used to "name" faces in a std::hash_map.
   */
  struct lCShapeHasher {
    /*!
     * Compute the hash value for a face.
     * \param f reference to the Topological Data Dtructure Face to hash.
     * \return hash value of face.
     */
    size_t operator() ( const TopoDS_Face& f ) const
    { return f.HashCode( IntegerLast() );}
  };

  class OCSolid : public Figure {
    Q_OBJECT

  public:
    OCSolid ( uint id, const QString& name, Page* parent );
    OCSolid ( uint id, const QDomElement& xml_rep, Page* parent );
    OCSolid ( uint id, const QString& name,const QDomElement& xml_rep,Page* parent);

    ~OCSolid ( void );

    ModelItem* lookup ( QStringList& path_components ) const;
    ModelItem* lookup ( QVector<uint>& id_path ) const;

    static QString newName ( void );

    Vertex closestVertex( Point p, Curve* curve ) const;
    //! \return the OpenCASCADE shape
    const TopoDS_Solid& shape ( void ) const { return solid_; }

    /*!
     * Apply the given material to the solid. Material is optional.
     * \param material new material to apply to solid.
     */
    void setMaterial (Material *material );
    //! \return the current material.
    const Material* material ( void ) const { return material_; }
    /*!
     * Change the grain orientation.
     * \param grain new grain orientation.
     */
    void setGrain ( const gp_Ax2& grain_csys );

    //! \return the grain coordinate system.
    const gp_Ax2& grain ( void ) const { return grain_csys_; }

    /*!
     * Add a named reference to a face in the solid.
     * \param name name of face.
     * \param face designated face.
     */
    void addFaceName ( const TopoDS_Face& face, const QString& name );
    /*!
     * Delete face/name associations (for example, if the geometry
     * is modified).
     */
    void clearFaceNames ( void ) { face_names_.clear(); }
    /*!
     * Retrieve the name associated with a face, if it has one.
     * \param face face to query.
     * \return name of face
     */
    QString faceName ( const TopoDS_Face& face ) const
    {
      std::unordered_map<TopoDS_Face,uint,lCShapeHasher>::const_iterator face_name;

      face_name = face_names_.find( face );

      if ( face_name != face_names_.end() )
	return PartFactory::instance()->name( (*face_name).second );
      else
	return QString::null;
    }
    /*!
     * Retrieve the face associated with the name.
     */
    const TopoDS_Face face ( const QString& name ) const
    {
      std::pair<QString,uint> name_id = PartFactory::instance()->name( name );

      std::unordered_map<TopoDS_Face,uint,lCShapeHasher>::const_iterator face =
	face_names_.begin();

      for ( ; face != face_names_.end(); ++face )
	if ( (*face).second == name_id.second )
	  return (*face).first;

      return TopoDS_Face();
    }
    /*!
     * Retrieve the face associated with the id.
     */
    const TopoDS_Face face ( uint id ) const
    {
      std::unordered_map<TopoDS_Face,uint,lCShapeHasher>::const_iterator face =
	face_names_.begin();
      for ( ; face != face_names_.end(); ++face )
	if ( (*face).second == id )
	  return (*face).first;

      return TopoDS_Face();
    }
    /*!
     * Retrieve the face id associated with the name.
     */
    uint faceID ( const QString& name ) const
    {
      std::pair<QString,uint> name_id = PartFactory::instance()->name( name );
      return name_id.second;
    }
    /*!
     * Retrieve the face id associated with the face.
     */
    uint faceID ( const TopoDS_Face& face ) const
    {
      std::unordered_map<TopoDS_Face,uint,lCShapeHasher>::const_iterator f =
	face_names_.find( face );
      if ( f != face_names_.end() )
	return (*f).second;

      return 0;
    }
    /*!
     * Add a resize handle (a position in space, really) to this solid.
     * \param id id of handle.
     * \param handle handle data.
     */
    void addResizeHandle ( uint id, const HandleData& handle );

    //! \return an iterator to the beginning of the list of handles.
    std::map<uint, HandleData>::const_iterator handlesBegin ( void ) const
    { return handles_.begin(); }

    //! \return an iterator to the end of the list of handles.
    std::map<uint, HandleData>::const_iterator handlesEnd ( void ) const
    { return handles_.end(); }

    /*!
     * Retrieve a particular handle.
     * \param id id of handle to retrieve.
     * \return reference to handle
     */
    HandleData& handle ( uint id )
    {
      return handles_[id];
    }
    /*!
     * Add a dimension parameter to this solid.
     * \param name name (essentially the index) of the parameter.
     * \param parameter parameter data.
     */
    void addParameter ( const QString& name, const Parameter& parameter );

    //! \return an iterator to the beginning of the list of parameters
    std::map<QString, Parameter>::const_iterator parametersBegin ( void ) const
    { return  parameters_.begin(); }

    //! \return an iterator to the end of the list of parameters
    std::map<QString, Parameter>::const_iterator parametersEnd ( void ) const
    { return  parameters_.end(); }

    /*!
     * Retrieve a particular parameter.
     * \param name name of parameter to retrieve
     * \return reference to parameter.
     */
    Parameter& parameter ( const QString& name )
    {
      return parameters_[name];
    }
    /*!
     * Add a construction datum to this solid.
     * \param name name of datum.
     * \param datum construction datum.
     */
    void addDatum ( const QString& name, const ConstructionDatum* datum );

    //! \return an iterator to the beginning of the list of datums.
    std::map<QString, const ConstructionDatum*>::const_iterator datumsBegin ( void) const
    { return datums_.begin(); }

    //! \return an iterator to the end of the list of datums.
    std::map<QString, const ConstructionDatum*>::const_iterator datumsEnd ( void ) const
    { return datums_.end(); }

    /*!
     * Retrieve a particular construction datum.
     * \param name name of datum to retrieve.
     * \return reference to datum.
     */
    const ConstructionDatum* datum ( const QString& name )
    {
      return datums_[name];
    }
    //! \return a string to display in the list view for the solid's basic shape.
    virtual QString baseShape ( void ) = 0;
    /*!
     * Update the solid based on the parameter change. This doesn't seem
     * quite right...
     * \param name name of parameter to update.
     * \param value new value entered by user.
     */
    virtual void updateParameter ( const QString& name, double value ) = 0;
    /*!
     * Update some (or all) the solid parameters at one go.
     * \param parameters list of parameters to update.
     */
    virtual void updateParameters ( const std::map<QString,double>& parameters ) = 0;
    /*!
     * Report an adjustment to the n-th handle.
     * \param id id of adjusted handle.
     * \param last_pnt the previous mouse position.
     * \param current_pnt the current mouse position.
     */
    virtual lC::ValidDelta adjustHandle ( uint id, const Point& last_pnt,
					  Point& current_pnt ) = 0;
    /*!
     * Retrieve the list of handle names for the given face.
     * \param name name of face.
     * \return reference to list of handle names.
     */
    const QVector<uint>& faceHandles ( const QString& name )
    { return face_handles_[ name ]; }
    /*!
     * Set the handles which are associated with the given face. I think
     * this is wholy cosmetic.
     * \param name name of face.
     * \param handles list of handle names associated with the named face.
     */
    void setFaceHandles ( const QString& name, const QVector<uint>& handles );
    /*!
     * Create a full XML representation of this solid suitable for
     * saving in a file with the rest of the model information.
     * \param xml_rep the XML representation of the model to append
     * this representation to.
     */
    virtual void write ( QDomElement& ) const = 0;
    /*!
     * Create an XML representation of this solid suitable for use
     * as a clipboard selection. The solid is simplified to just
     * an origin and a width and height; the constraint information is
     * discarded.
     * \param xml_rep the XML representation of the selection to append
     * this representation to.
     */
    void copyWrite ( QDomElement& ) const;
    /*!
     * Find the OpenCASCADE type of the given surface (identified by
     * path).
     * \param path_components path to solid.
     * \return OpenCASCADE standard type identifier.
     */
    Handle(Standard_Type) lookupType ( QStringList& path_components ) const;
    /*!
     * Find the OpenCASCADE type of the given surface (identified by
     * path).
     * \param id_path id path to solid.
     * \return OpenCASCADE standard type identifier.
     */
    Handle(Standard_Type) lookupType ( QVector<uint>& path_components ) const;
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
    TopoDS_Shape lookupShape ( QVector<uint>& id_path ) const;
    /*!
     * Construct a string path to the specified item. May be either the
     * figure itself or one of its geometries.
     * \param id_path id path to item.
     * \return string encoded path to item.
     */
    QString idPath ( QVector<uint> id_path ) const;
    /*!
     * Construct a id path to the specified item. May be either the
     * figure itself or one of its geometries.
     * \param id_path id path to item.
     * \return string encoded path to item.
     */
    void pathID ( QStringList& path_components, QVector<uint>& id_path ) const;

private slots:
signals:
    /*!
     * Emitted when the geometry of the solid is changed.
     */
    void modified ( void );
    /*!
     * Emitted when the material of the solid is changed.
     */
    void materialChanged ( void );

  protected:
    //! An OpenCASCADE solid
    TopoDS_Solid solid_;

  private:
    //! (Optional) material out of which this solid is made.
    const Material* material_;
    //! Grain coordinate system. This is probably more complicated than is
    //! necessary.
    gp_Ax2 grain_csys_;

    //! List of the resize handles supplied by this solid.
    std::map<uint, HandleData> handles_;
    //! List of dimensional parameters.
    std::map<QString, Parameter> parameters_;
    //! List of construction datum used by this solid.
    std::map<QString, const ConstructionDatum*> datums_;

    //! The mapping the face data structures to their names.
    std::unordered_map< TopoDS_Face, uint, lCShapeHasher > face_names_;

    //! The list of handles which are associated with each face.
    std::map<QString, QVector<uint> > face_handles_;

    //! Is this necessary? (probably not...)
    static uint unique_index_;
  };

  /*!
   * A handy predefined solid: the rectangular parallelipiped.
   */
  class OCSolidRPP : public OCSolid {
  public:
    /*!
     * Create a rectangular parallelipiped with the given attributes.
     * \param id id of figure.
     * \param name (figure) name.
     * \param length length of solid in X direction.
     * \param width width of solid in Y direction.
     * \param thickness thickness of solid in Z direction.
     * \param parent parent page.
     */
    OCSolidRPP ( uint id, const QString& name,
		 double length, double width, double thickness,
		 Page* parent );
    /*!
     * Create a rectangular parallelipiped from its XML representation.
     * \param id id of figure.
     * \param name (figure) name.
     * \param xml_rep XML representation.
     * \param parent parent page.
     */
    OCSolidRPP ( uint id, const QString& name, const QDomElement& xml_rep,
		 Page* parent );
    //! \return a string to display in the list view for the solid's basic shape.
    QString baseShape ( void ) { return tr( "RPP" ); }
    /*!
     * Update the solid based on the parameter change. This doesn't seem
     * quite right...
     * \param parameter parameter to update.
     * \param value new value entered by user.
     */
    void updateParameter ( const QString& name, double value );
    /*!
     * Update some (or all) the solid parameters at one go.
     * \param parameters list of parameters to update.
     */
    void updateParameters ( const std::map<QString,double>& parameters );
    /*!
     * Report an adjustment to the n-th handle.
     * \param id id of adjusted handle.
     * \param last_pnt the previous mouse position.
     * \param current_pnt the current mouse position.
     */
    lC::ValidDelta adjustHandle ( uint id, const Point& last_pnt,
				  Point& current_pnt );
    /*!
     * Create a full XML representation of the RPP suitable for
     * saving in a file with the rest of the model information.
     * \param xml_rep the XML representation of the model to append
     * this representation to.
     */
    void write ( QDomElement& ) const;

  private:
    //! Create the general description of the solid and then construct
    //! the geometry.
    void create ( double length, double width, double thickness );
    //! Construct the solid
    void construct ( double length, double width, double thickness );
    //! Designate the various handles: L(eft), R(ight):B(ack), F(ront):B(ottom), T(op)
    enum { LBB, LBT, LFB, LFT, RBB, RBT, RFB, RFT };
  };

  /*!
   * The unlikely parallelogram parallelipiped.
   */
  class OCSolidPGP : public OCSolid {
  public:
    /*!
     * Create a parallelogram parallelipiped with the given attributes.
     * \param id id of figure.
     * \param name (figure) name.
     * \param length length of solid in X direction.
     * \param width width of solid in Y direction.
     * \param thickness thickness of solid in Z direction.
     * \param parent parent page.
     */
    OCSolidPGP ( uint id, const QString& name,
		 double length, double width, double thickness,
		 Page* parent );
    /*!
     * Create a parallelogram parallelipiped from its XML representation.
     * \param id id of figure.
     * \param name (figure) name.
     * \param xml_rep XML representation.
     * \param parent parent page.
     */
    OCSolidPGP ( uint id, const QString& name, const QDomElement& xml_rep,
		 Page* parent );
    //! \return a string to display in the list view for the solid's basic shape.
    QString baseShape ( void ) { return tr( "PGP" ); }
    /*!
     * Update the solid based on the parameter change. This doesn't seem
     * quite right...
     * \param parameter parameter to update.
     * \param value new value entered by user.
     */
    void updateParameter ( const QString& name, double value );
    /*!
     * Update some (or all) the solid parameters at one go.
     * \param parameters list of parameters to update.
     */
    void updateParameters ( const std::map<QString,double>& parameters );
    /*!
     * Report an adjustment to the n-th handle.
     * \param id id of adjusted handle.
     * \param last_pnt the previous mouse position.
     * \param current_pnt the current mouse position.
     */
    lC::ValidDelta adjustHandle ( uint id, const Point& last_pnt,
				  Point& current_pnt );
    /*!
     * Create a full XML representation of the RPP suitable for
     * saving in a file with the rest of the model information.
     * \param xml_rep the XML representation of the model to append
     * this representation to.
     */
    void write ( QDomElement& ) const;

  private:
    //! Create the general description of the solid and then construct
    //! the geometry.
    void create ( double length, double width, double thickness );
    //! Construct the solid
    void construct ( double length, double width, double thickness );
    //! Designate the various handles: L(eft), R(ight):B(ack), F(ront):B(ottom), T(op)
    enum { LBB, LBT, LFB, LFT, RBB, RBT, RFB, RFT };
  };

  /*!
   * A cylinder, but we'll call it a Turning!
   */
  class OCSolidCYL : public OCSolid {
  public:
    /*!
     * Create a solid cylinder with the given attributes.
     * \param id id of figure.
     * \param name (figure) name.
     * \param length axial length of cylinder.
     * \param diameter diameter of cylinder.
     * \param parent parent page.
     */
    OCSolidCYL ( uint id, const QString& name,
		 double length, double diameter,
		 Page* parent);
    /*!
     * Create a cylinder from its XML representation.
     * \param id id of figure.
     * \param name (figure) name.
     * \param xml_rep XML representation.
     * \param parent parent page.
     */
    OCSolidCYL ( uint id, const QString& name, const QDomElement& xml_rep,
		 Page* parent );
    //! \return a string to display in the list view for the solid's basic shape.
    QString baseShape ( void ) { return tr( "CYL" ); }
    /*!
     * Update the solid based on the parameter change. This doesn't seem
     * quite right...
     * \param parameter parameter to update.
     * \param value new value entered by user.
     */
    void updateParameter ( const QString& name, double value );
    /*!
     * Update some (or all) the solid parameters at one go.
     * \param parameters list of parameters to update.
     */
    void updateParameters ( const std::map<QString,double>& parameters );
    /*!
     * Report an adjustment to the n-th handle.
     * \param id id of adjusted handle.
     * \param last_pnt the previous mouse position.
     * \param current_pnt the current mouse position.
     */
    lC::ValidDelta adjustHandle ( uint id, const Point& last_pnt,
				  Point& current_pnt );
    /*!
     * Create a full XML representation of the RPP suitable for
     * saving in a file with the rest of the model information.
     * \param xml_rep the XML representation of the model to append
     * this representation to.
     */
    void write ( QDomElement& ) const;

  private:
    //! Create the general description of the solid and then construct
    //! the geometry.
    void create ( double length, double diameter );
    //! Construct the solid
    void construct ( double length, double diameter );

    //! The construction axis for this solid
    Axis* axis_;
  };

  /*!
   * An RPP which transistions into a cone: like a turned, tapered leg.
   */
  class OCSolidRPPCON : public OCSolid {
  public:
    /*!
     * Create a rectangular parallelipiped which transistions in to a
     * conical form. The rectangular parallelipiped section has a square
     * cross section.
     * \param id id of figure.
     * \param name (figure) name.
     * \param length overall length of the solid.
     * \param rectangular_length length of the rectangular section.
     * \param rectangular_width edge length of the square rectangular section.
     * \param conical_length length of the conical section.
     * \param conical_top_diameter diameter of conical section at start of
     * transistion.
     * \param conical_bottom_diameter diameter of conical section at end
     * of solid.
     * \param parent parent page.
     */
    OCSolidRPPCON ( uint id, const QString& name,
		    double length, double rectangular_length,
		    double rectangular_width, double conical_length,
		    double conical_top_diameter, double conical_bottom_diameter,
		    Page* parent );
    /*!
     * Create an rpp/con from its XML representation.
     * \param id id of figure.
     * \param name (figure) name.
     * \param xml_rep XML representation.
     * \param parent parent page.
     */
    OCSolidRPPCON ( uint id, const QString& name, const QDomElement& xml_rep,
		    Page* parent );
    //! \return a string to display in the list view for the solid's basic shape.
    QString baseShape ( void ) { return tr( "RPPCON" ); }
    /*!
     * Update the solid based on the parameter change. This doesn't seem
     * quite right...
     * \param parameter parameter to update.
     * \param value new value entered by user.
     */
    void updateParameter ( const QString& name, double value );
    /*!
     * Update some (or all) the solid parameters at one go.
     * \param parameters list of parameters to update.
     */
    void updateParameters ( const std::map<QString,double>& parameters );
    /*!
     * Report an adjustment to the n-th handle.
     * \param id id of adjusted handle.
     * \param last_pnt the previous mouse position.
     * \param current_pnt the current mouse position.
     */
    lC::ValidDelta adjustHandle ( uint id, const Point& last_pnt,
				  Point& current_pnt );
    /*!
     * Create a full XML representation of the RPPCON suitable for
     * saving in a file with the rest of the model information.
     * \param xml_rep the XML representation of the model to append
     * this representation to.
     */
    void write ( QDomElement& ) const;
  private:
    //! Initialize the solid and parameter definitions.
    void create ( double length, double rec_length,
		  double rec_width, double con_length,
		  double con_top_diameter, double con_bottom_diameter );
    //! Construct the solid
    void construct ( double length, double rec_length,
		     double rec_width, double con_length,
		     double con_top_diameter, double con_bottom_diameter );

    //! The construction axis for this solid
    Axis* axis_;
  };

  /*!
   * An RPP attached to a truncated pyramid: like a tapered leg.
   */
  class OCSolidRPPPYR : public OCSolid {
  public:
    /*!
     * Create a rectangular parallelipiped attached to a truncated pyramid.
     * Two sides of the pyramid are coincident with sides of the rpp; the
     * other two are tapered. Both the rpp and the pyramid have square
     * cross sections.
     * \param id id of figure.
     * \param name (figure) name.
     * \param length overall length of the solid.
     * \param rectangular_length length of the rectangular section.
     * \param rectangular_width edge length of the rectangular section.
     * \param pyramidal_width edge length of the narrow end of the
     * pyramidal section.
     * \param parent parent page.
     */
    OCSolidRPPPYR ( uint id, const QString& name,
		    double length, double rectangular_length,
		    double rectangular_width, double pyramidal_width,
		    Page* parent );
    /*!
     * Create an rpp/pyr from its XML representation.
     * \param id id of figure.
     * \param name (figure) name.
     * \param xml_rep XML representation.
     * \param parent parent page.
     */
    OCSolidRPPPYR ( uint id, const QString& name, const QDomElement& xml_rep,
		    Page* parent );
    //! \return a string to display in the list view for the solid's basic shape.
    QString baseShape ( void ) { return tr( "RPPPYR" ); }
    /*!
     * Update the solid based on the parameter change. This doesn't seem
     * quite right...
     * \param parameter parameter to update.
     * \param value new value entered by user.
     */
    void updateParameter ( const QString& name, double value );
    /*!
     * Update some (or all) the solid parameters at one go.
     * \param parameters list of parameters to update.
     */
    void updateParameters ( const std::map<QString,double>& parameters );
    /*!
     * Report an adjustment to the n-th handle.
     * \param id id of adjusted handle.
     * \param last_pnt the previous mouse position.
     * \param current_pnt the current mouse position.
     */
    lC::ValidDelta adjustHandle ( uint id, const Point& last_pnt,
				  Point& current_pnt );
    /*!
     * Create a full XML representation of the rpp/pyr suitable for
     * saving in a file with the rest of the model information.
     * \param xml_rep the XML representation of the model to append
     * this representation to.
     */
    void write ( QDomElement& ) const;
  private:
    //! Initialize the solid and parameter definitions.
    void create ( double length, double rectangular_length,
		  double rectangular_width, double pyramidal_width );
    //! Construct the solid
    void construct ( double length, double rectangular_length,
		     double rectangular_width, double pyramidal_width );
  };

  /*!
   * A Revolved spline with a cylindical base: like a turned knob.
   */
  class OCSolidBSPCYL : public OCSolid {
  public:
    /*!
     * Create a revolved spline with a cylindrical base.
     * \param id id of figure.
     * \param name (figure) name.
     * \param length overall length of solid.
     * \param bsp_diameter diameter of bspline at largest point.
     * \param cyl_diameter diameter of cylindrical section.
     * \param cyl_length length of cylindrical section.
     */
    OCSolidBSPCYL ( uint id, const QString& name,
		    double length, double bsp_diameter,
		    double cyl_diameter, double cyl_length,
		    Page* parent );
    /*!
     * Create an bsp/cyl from its XML representation.
     * \param id id of figure.
     * \param name (figure) name.
     * \param xml_rep XML representation.
     * \param parent parent page.
     */
    OCSolidBSPCYL ( uint id, const QString& name, const QDomElement& xml_rep,
		    Page* parent );
    //! \return a string to display in the list view for the solid's basic shape.
    QString baseShape ( void ) { return tr( "BSPCYL" ); }
    /*!
     * Update the solid based on the parameter change. This doesn't seem
     * quite right...
     * \param parameter parameter to update.
     * \param value new value entered by user.
     */
    void updateParameter ( const QString& name, double value );
    /*!
     * Update some (or all) the solid parameters at one go.
     * \param parameters list of parameters to update.
     */
    void updateParameters ( const std::map<QString,double>& parameters );
    /*!
     * Report an adjustment to the n-th handle.
     * \param id id of adjusted handle.
     * \param last_pnt the previous mouse position.
     * \param current_pnt the current mouse position.
     */
    lC::ValidDelta adjustHandle ( uint id, const Point& last_pnt,
				  Point& current_pnt );
    /*!
     * Create a full XML representation of the BSPCYL suitable for
     * saving in a file with the rest of the model information.
     * \param xml_rep the XML representation of the model to append
     * this representation to.
     */
    void write ( QDomElement& ) const;

  private:
    //! Initialize the solid and parameter definitions.
    void create ( double length, double bsp_diameter, double cyl_diameter,
		  double cyl_length );
    //! Construct the solid
    void construct ( double length, double bsp_diameter, double cyl_diameter,
		     double cyl_length );

    //! The construction axis for this solid
    Axis* axis_;
  };
} // End of Space3D namespace

#endif // SOLID_H
