/* -*- c++ -*-
 * arrow.h
 *
 * Header for the Arrow class
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
#ifndef ARROW_H
#define ARROW_H

#include "vectoralgebra.h"
#include "constants.h"

// In the tradition of Handle, here's an arrow
namespace Space2D {
  class Point;
  /*!
   * Draw an arrow: a straight leader and an arrow head.
   * Arrow lets you cache a few attributes of an arrow.
   */
  class Arrow { // Eventually, this will have to a graphics object
  public:
    /*!
     * Construct an arrow structure to cache the details of the image.
     * \param style style of arrowhead.
     * \param length length of arrowhead in paper units.
     * \param width_ratio ratio of arrowhead width to length.
     */
    Arrow ( enum lC::ArrowHeadStyle style = lC::FILLED, double length = 3./16.,
	    double width_ratio = 1./4. );
    //! \return the current style of the arrow head
    enum lC::ArrowHeadStyle style ( void ) const { return style_; }
    /*!
     * Set the style of the arrowhead.
     * \param style new style of the arrowhead.
     */
    void setStyle ( enum lC::ArrowHeadStyle style );
    //! \return the length (in paper units) of the arrowhead.
    double length ( void ) const { return length_; }
    /*!
     * Set the length of the arrow head.
     * \param length new length of arrowhead.
     */
    void setLength ( double length );
    //! \return ratio of arrowhead width to length.
    double widthRatio ( void ) const { return width_ratio_; }
    /*!
     * Set the width to length ratio of the arrowhead.
     * \param width_ratio new arrowhead width to length ratio.
     */
    void setWidthRatio ( double width_ratio );
    /*!
     * Draw an arrow from one point to another. The arrowhead is
     * oriented along the vector between the two points.
     * \param from start point of arrow leader.
     * \param to end point of arrow.
     */
    void draw ( const Point& from, const Point& to ) const;
    /*!
     * Draw the arrow in selection mode (actually same as draw method).
     * \param from start point of arrow leader.
     * \param to end point of arrow.
     */
    void select ( const Point& from, const Point& to ) const;

  private:
    /*!
     * Draw an arrow with an open arrowhead.
     * \param length overall length of arrow.
     */
    void drawOpen ( double length ) const;
    /*!
     * Draw an arrow with a hollow arrowhead.
     * \param length overall length of arrow.
     */
    void drawHollow ( double length ) const;
    /*!
     * Draw an arrow with a filled arrowhead.
     * \param length overall length of arrow.
     */
    void drawFilled ( double length ) const;
    //! Arrowhead style.
    enum lC::ArrowHeadStyle style_;
    //! Arrowhead length in paper units.
    double length_;
    //! Arrowhead width to length ratio.
    double width_ratio_;
  };
} // End of Space2D namespace

namespace Space3D {
  class Point;
  /*!
   * Draw an arrow in a 3D view: a straight leader and an arrow head.
   * Arrow lets you cache a few attributes of an arrow.
   */
  class Arrow { // Eventually, this will have to a graphics object
  public:
    /*!
     * Construct an arrow structure to cache the details of the image.
     * \param style style of arrowhead.
     * \param length length of arrowhead in paper units.
     * \param width_ratio ratio of arrowhead width to length.
     */
    Arrow ( enum lC::ArrowHeadStyle style = lC::FILLED, double length = 3./16.,
	    double width_ratio = 1./4. );
    //! \return the current style of the arrow head
    enum lC::ArrowHeadStyle style ( void ) const { return style_; }
    /*!
     * Set the style of the arrowhead.
     * \param style new style of the arrowhead.
     */
    void setStyle ( enum lC::ArrowHeadStyle style );
    //! \return the length (in paper units) of the arrowhead.
    double length ( void ) const { return length_; }
    /*!
     * Set the length of the arrow head.
     * \param length new length of arrowhead.
     */
    void setLength ( double length );
    //! \return ratio of arrowhead width to length.
    double widthRatio ( void ) const { return width_ratio_; }
    /*!
     * Set the width to length ratio of the arrowhead.
     * \param width_ratio new arrowhead width to length ratio.
     */
    void setWidthRatio ( double width_ratio );
    //! \return the view normal.
    Vector viewNormal ( void ) const { return view_normal_; }
    /*!
     * Set the normal direction of the modelview. This is used to rotate
     * the arrowhead so that it is drawn as much in full view as possible.
     * \param view_normal normal view vector (typically the 3rd column
     * of the modelview matrix).
     */
    void setViewNormal ( const Vector& view_normal );
    /*!
     * Draw an arrow from one point to another. The arrowhead is
     * oriented along the vector between the two points.
     * \param from start point of arrow leader.
     * \param to end point of arrow.
     */
    void draw ( const Point& from, const Point& to ) const;
    /*!
     * Draw the arrow in selection mode (actually same as draw method).
     * \param from start point of arrow leader.
     * \param to end point of arrow.
     */
    void select ( const Point& from, const Point& to ) const;

  private:
    /*!
     * Draw an arrow with an open arrowhead.
     * \param length overall length of arrow.
     */
    void drawOpen ( double length ) const;
    /*!
     * Draw an arrow with a hollow arrowhead.
     * \param length overall length of arrow.
     */
    void drawHollow ( double length ) const;
    /*!
     * Draw an arrow with a filled arrowhead.
     * \param length overall length of arrow.
     */
    void drawFilled ( double length ) const;
    //! Style of arrow head.
    enum lC::ArrowHeadStyle style_;
    //! Length of arrow head.
    double length_;
    //! Ratio of arrow head width to length.
    double width_ratio_;
    //! View direction normal.
    Vector view_normal_;
  };
} // End of Space3D namespace

#endif // ARROW_H
