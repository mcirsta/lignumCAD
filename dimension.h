/* -*- c++ -*-
 * dimension.h
 *
 * Header for the Dimension class
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
#ifndef DIMENSION_H
#define DIMENSION_H

#include "vectoralgebra.h"
#include "openglbase.h"
#include "arrow.h"
#include "constants.h"

class OpenGLBase;

namespace Space2D {
  /*!
   * This class is for drawing and selecting a 2D dimension line. The
   * dimension is specified by its two reference points, its orientation
   * and quite a lot of other visual attributes. It is distinct from
   * DimensionView; DimensionView controls the references and placement
   * of a dimension; this class is purely cosmetic and just draws what it's
   * told.
   *
   * A dimension line can represent something other than the Cartesian
   * distance between the two reference points. For example:
   *	  |<--- 12 --->|       	         ->\  
   *	  | 	       	       	  12.042    \ 
   *   	  |    	  +----o      \<-       +----o
   *	  	  |    |  vs.  \        |    |
   *   	  o-------+    |       	o-------+    |
   *   	  |            |       	|            |
   *   
   * The exact distance presented in the dimension value depends on the
   * direction of the extension line vector (called normal_ in the code).
   *
   * Here are the various positions of the arrows and the dimension value
   * (according to ANSI Y14.5-1967)
   *     |<- # ->| : standard
   *   ->| # |<-   : exterior arrows
   *   #-|<->|     : exterior number at end0 (not currently supported...)
   *     |<->|-#   : exterior number at end1 (not currently supported...)
   *  #->| |<-     : exterior arrows and number at end0
   *   ->| |<-#    : exterior arrows and number at end1 (not currently supported...)
   */
  class Dimension {
  public:
    //! Dimension has two ends
    enum Ends { END0=0, END1=1 };
    //! The placement of the value.
    enum Placement { INTERIOR, EXTERIOR_END0, EXTERIOR_END1 };

  private:
    //! Reference point for "Starting" end of dimension.
    Point end0_;
    //! Reference point for "Ending" end of dimension.
    Point end1_;
    //! Direction of extension line.
    Vector normal_;
    //! We need to lookup font metrics in order to compute the layout.
    OpenGLBase* view_;

    // Properties

    struct ExtensionLine {
      double offset;		//<! distance from reference point to start of line
      double thickness;		//<! (printed) width of line
      Point from_;		//<! computed starting point
      Point to_;		//<! computed end point
      bool use_;		//<! Extension lines are not always necessary.
    } extension_lines_[2];

    //! User adjustment to extension position. This is in paper inches.
    double extension_offset_;
    //! User adjustment to text position. This is in scale inches if
    //! the dimension text is INTERIOR and in paper inches if the
    //! the dimension text is EXTERIOR.
    double dimension_offset_;

    struct DimensionLine {
      double clearance;		//!< separation from the end of the extension lines
      double thickness;		//!< rendered width of the line
    } dimension_line_;

    //! Holds a few rendering attributes (both arrows are drawn the same).
    Arrow arrow_;

    struct Arrows {
      Point from_;		//!< Computed starting point of arrow
      Point to_;		//!< Pointy end of arrow
    } arrows_[2];

    struct DimensionValue {
      double value;		//!< Dimension size
      QString text;		//!< string represention of value
      FaceData face_data_;	//!< font information
      Point position;		//!< Computed position of the text
      Vector bbox;		//!< Unrotated bounding box of text
      enum Placement placement;	//!< Even if the dimension CAN be interior,
				//!< the user may drag it outside.
    } dimension_value_;

    struct DimensionLocator {	// For externl dim/intern arrs, have an extra line
      Point from_;		//!< Locator start point
      Point to_;		//!< Locator end point
      bool use_;		//!< Locator not always necessary.
    } locator_;

    QString note_;		//!< Additional dimension value text.

  public:
    Dimension ( OpenGLBase* view );

    Dimension ( const Point& end0, const Point& end1, const Vector& normal,
		OpenGLBase* view, const QString& note = QString::null );

    void init ( void );

    Point end0 ( void ) const { return end0_; }
    Point end1 ( void ) const { return end1_; }
    Vector normal ( void ) const { return normal_; }

    void setEndsNormal ( const Point& end0, const Point& end1,const Vector& normal,
			 OpenGLBase* view, const QString& note = QString::null );
    void setUseExtensionLine ( bool end0, bool end1 );

    void setDimensionAttributes ( double extension_offset, double dimension_offset,
				  enum Placement placement );

    void adjustExtension ( const Vector& delta );
    double extensionOffset ( void ) const { return extension_offset_; }

    void adjustDimension ( const Vector& delta );
    double dimensionOffset ( void ) const { return dimension_offset_; }

    enum Placement placement ( void ) const { return dimension_value_.placement; }

    void draw ( lC::Render::Mode mode ) const;
    void select ( lC::Render::Mode mode ) const;

  private:
    void computeLayout ( void );
  };

  /*!
   * Retrieve the string representation of the dimension text placement.
   * \param placement Placement enum to convert to string representation.
   * \return string representation of placement.
   */
  inline QString placementText ( enum Dimension::Placement placement )
  {
    switch ( placement ) {
    case Dimension::EXTERIOR_END0:
      return "EXTERIOR_END0";
    case Dimension::EXTERIOR_END1:
      return "EXTERIOR_END1";
    case Dimension::INTERIOR:
      return "INTERIOR";
    }
    return "INTERIOR";
  }
  /*!
   * Convert string into Dimension Placement enum.
   * \param text string representation of Placement enum.
   * \return Dimension::Placement enum described by text.
   */
  inline enum Dimension::Placement placement ( const QString& text )
  {
    if ( text == "EXTERIOR_END0" )
      return Dimension::EXTERIOR_END0;
    else if ( text == "EXTERIOR_END1" )
      return Dimension::EXTERIOR_END1;

    return Dimension::INTERIOR;
  }
} // End of Space2D namespace

namespace Space3D {
  /*!
   * This class is for drawing and selecting a 3D dimension line. The
   * dimension is specified by its two reference points, its orientation
   * and quite a lot of other visual attributes. It is distinct from
   * DimensionView; DimensionView controls the references and placement
   * of a dimension; this class is purely cosmetic and just draws what it's
   * told.
   *
   * A dimension line can represent something other than the Cartesian
   * distance between the two reference points. For example:
   *	  |<--- 12 --->|       	         ->\  
   *	  | 	       	       	  12.042    \ 
   *   	  |    	  +----o      \<-       +----o
   *	  	  |    |  vs.  \        |    |
   *   	  o-------+    |       	o-------+    |
   *   	  |            |       	|            |
   *   
   * The exact distance presented in the dimension value depends on the
   * direction of the extension line vector (called normal_ in the code).
   *
   * Here are the various positions of the arrows and the dimension value
   * (according to ANSI Y14.5-1967)
   *     |<- # ->| : standard
   *   ->| # |<-   : exterior arrows
   *   #-|<->|     : exterior number at end0 (not currently supported...)
   *     |<->|-#   : exterior number at end1 (not currently supported...)
   *  #->| |<-     : exterior arrows and number at end0
   *   ->| |<-#    : exterior arrows and number at end1 (not currently supported...)
   */
  class Dimension {
  public:
    //! Dimension has two ends
    enum Ends { END0=0, END1=1 };
    //! The placement of the value.
    enum Placement { INTERIOR, EXTERIOR_END0, EXTERIOR_END1 };

  private:
    //! Reference point for "Starting" end of dimension.
    Point end0_;
    //! Reference point for "Ending" end of dimension.
    Point end1_;
    //! Direction of extension line.
    Vector normal_;
    //! We need to lookup font metrics in order to compute the layout.
    OpenGLBase* view_;
    //! Additional dimension value text.
    QString note_;
    //! The rendering mode for the dimension.
    lC::Render::Mode mode_;

    // Properties

    struct ExtensionLine {
      double offset;		//<! distance from reference point to start of line
      double thickness;		//<! (printed) width of line
      Point from_;		//<! computed starting point
      Point to_;		//<! computed end point
      bool use_;		//<! Extension lines are not always necessary.
    } extension_lines_[2];

    //! User adjustment to extension position. This is in paper inches.
    double extension_offset_;
    //! User adjustment to text position. This is in scale inches if
    //! the dimension text is INTERIOR and in paper inches if the
    //! the dimension text is EXTERIOR.
    double dimension_offset_;

    struct DimensionLine {
      double clearance;		//!< separation from the end of the extension lines
      double thickness;		//!< rendered width of the line
    } dimension_line_;

    //! Holds a few rendering attributes (both arrows are drawn the same).
    Arrow arrow_;

    struct Arrows {
      Point from_;		//!< Computed starting point of arrow
      Point to_;		//!< Pointy end of arrow
    } arrows_[2];

    struct DimensionValue {
      double value;		//!< Dimension size
      QString text;		//!< string represention of value
      FaceData face_data_;	//!< font information
      Point position;		//!< Computed position of the text
      Space2D::Point bb_min_;	//!< Position of lower left corner of text in 2D
      Space2D::Point bb_max_;	//!< Position of upper right corner of text in 2D
      Point bb_ll_;		//!< Position of lower left corner of text in 3D
      Point bb_lr_;		//!< Position of lower right corner of text in 3D
      Point bb_ur_;		//!< Position of upper right corner of text in 3D
      Point bb_ul_;		//!< Position of upper left corner of text in 3D
      enum Placement placement;	//!< Even if the dimension CAN be interior,
				//!< the user may drag it outside.
    } dimension_value_;

    struct DimensionLocator {	// For externl dim/intern arrs, have an extra line
      Point from_;		//!< Locator start point
      Point to_;		//!< Locator end point
      bool use_;		//!< Locator not always necessary.
    } locator_;

  public:
    Dimension ( OpenGLBase* view );

    Dimension ( const Point& end0, const Point& end1, const Vector& normal,
		OpenGLBase* view, const QString& note = QString::null );

    void init ( void );

    Point end0 ( void ) const { return end0_; }
    Point end1 ( void ) const { return end1_; }
    Vector normal ( void ) const { return normal_; }

    lC::Render::Mode mode ( void ) const { return mode_; }

    void setEndsNormal ( const Point& end0, const Point& end1,const Vector& normal,
			 OpenGLBase* view, const QString& note = QString::null );
    void setUseExtensionLine ( bool end0, bool end1 );

    void setDimensionAttributes ( double extension_offset, double dimension_offset,
				  enum Placement placement );

    void setMode ( lC::Render::Mode mode );
    void setViewNormal ( const Vector& view_normal );

    void adjustExtension ( const Vector& delta );
    double extensionOffset ( void ) const { return extension_offset_; }

    void adjustDimension ( const Vector& delta );
    double dimensionOffset ( void ) const { return dimension_offset_; }

    enum Placement placement ( void ) const { return dimension_value_.placement; }

    void draw ( void ) const;
    void select ( void ) const;

  private:
    void computeLayout ( void );
  };

  /*!
   * Retrieve the string representation of the dimension text placement.
   * \param placement Placement enum to convert to string representation.
   * \return string representation of placement.
   */
  inline QString placementText ( enum Dimension::Placement placement )
  {
    switch ( placement ) {
    case Dimension::EXTERIOR_END0:
      return "EXTERIOR_END0";
    case Dimension::EXTERIOR_END1:
      return "EXTERIOR_END1";
    case Dimension::INTERIOR:
      return "INTERIOR";
    }
    return "INTERIOR";
  }
  /*!
   * Convert string into Dimension Placement enum.
   * \param text string representation of Placement enum.
   * \return Dimension::Placement enum described by text.
   */
  inline enum Dimension::Placement placement ( const QString& text )
  {
    if ( text == "EXTERIOR_END0" )
      return Dimension::EXTERIOR_END0;
    else if ( text == "EXTERIOR_END1" )
      return Dimension::EXTERIOR_END1;

    return Dimension::INTERIOR;
  }
} // End of Space3D namespace

#endif // DIMENSION_H
