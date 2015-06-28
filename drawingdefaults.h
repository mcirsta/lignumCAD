/* -*- c++ -*-
 * drawingdefaults.h
 *
 * Header for the drawing default attributes classes.
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

#ifndef DRAWINGDEFAULTS_H
#define DRAWINGDEFAULTS_H

#include <qcolor.h>
#include <qfont.h>

#include "constants.h"

/*!
 * This structure collects the attributes which define the various
 * available page color schemes. A color scheme consists of the
 * following properties:
 *  Geometry outline color
 *  Annotation color
 *  The sketch view grid color
 *  Constraint primary color
 *  Constraint secondary color
 *  Background, may be one of the subtypes
 *   A. Solid
 *   B. Gradient
 *   C. Pattern
 * (and probably more eventually)
 */
class PageColorScheme {
  //! Name of the scheme.
  QString name_;

  //! Outline color of geometry.
  QColor geometry_color_;

  //! Annotation color.
  QColor annotation_color_;

  //! Grid color.
  QColor grid_color_;

  //! Constraint primary color.
  QColor constraint_primary_color_;

  //! Constraint secondary color.
  QColor constraint_secondary_color_;

  //! The background style
  enum lC::Background::Style background_style_;

  //! Solid background color
  QColor background_color_;

  //! Gradient background color
  QColor gradient_color_;

  //! Pattern (a grayscale image suitable for use as a texture map).
  //! Stored a as string so it can be retrieved via MIME lookup.
  QString pattern_file_;

  //! The (default) image for texturing geometry.
  //! Stored a as string so it can be retrieved via MIME lookup.
  QString texture_file_;

public:
  /*!
   * Constructor provides defaults for all the items except name.
   */
  PageColorScheme ( const QString& name,
		    QColor geometry_color = lC::DEFAULT_GEOMETRY_COLOR,
		    QColor annotation_color = lC::DEFAULT_ANNOTATION_COLOR,
		    QColor grid_color = lC::DEFAULT_GRID_COLOR,
    QColor constraint_primary_color = lC::DEFAULT_CONSTRAINT_PRIMARY_COLOR,
    QColor constraint_secondary_color = lC::DEFAULT_CONSTRAINT_SECONDARY_COLOR,
    enum lC::Background::Style background_style = lC::Background::SOLID,
		    QColor background_color = lC::DEFAULT_BACKGROUND_COLOR,
		    QColor gradient_color = lC::DEFAULT_GRADIENT_COLOR,
		    QString pattern_file = lC::DEFAULT_PATTERN_FILE,
		    QString texture_file = lC::DEFAULT_TEXTURE_FILE )
    : name_( name ), geometry_color_( geometry_color ),
      annotation_color_( annotation_color ),
      grid_color_( grid_color ),
      constraint_primary_color_( constraint_primary_color ),
      constraint_secondary_color_( constraint_secondary_color ),
      background_style_( background_style ),
      background_color_( background_color ), gradient_color_( gradient_color ),
      pattern_file_( pattern_file ), texture_file_( texture_file )
  {}

  /*!
   * \return the name of the scheme.
   */
  QString name ( void ) const { return name_; }

  /*!
   * Set the color for the outlines (edges) of geometric figures.
   * \param color the color of geometry outlines.
   */
  void setGeometryColor ( const QColor& color ) { geometry_color_ = color; }

  /*!
   * \return the color for geometry outlines.
   */
  QColor geometryColor ( void ) const { return geometry_color_; }

  /*!
   * Set the color for annotations (e.g. reference and centerlines,
   * and text annotations).
   * \param color the new color of annotations.
   */
  void setAnnotationColor ( const QColor& color ) { annotation_color_ = color; }

  /*!
   * \return the color for annotations.
   */
  QColor annotationColor ( void ) const { return annotation_color_; }

  /*!
   * Set the color for sketch view grid.
   * \param color the new color of the sketch view grid.
   */
  void setGridColor ( const QColor& color ) { grid_color_ = color; }

  /*!
   * \return the color for the sketch view grid.
   */
  QColor gridColor ( void ) const { return grid_color_; }

  /*!
   * Set the primary color of contraints.
   * \param color the primary color of constraints.
   */
  void setConstraintPrimaryColor ( const QColor& color ) { constraint_primary_color_ = color; }

  /*!
   * \return the primary color for constraints.
   */
  QColor constraintPrimaryColor ( void ) const { return constraint_primary_color_; }

  /*!
   * Set the secondary color of contraints.
   * \param color the secondary color of constraints.
   */
  void setConstraintSecondaryColor ( const QColor& color ) { constraint_secondary_color_ = color; }

  /*!
   * \return the secondary color for constraints.
   */
  QColor constraintSecondaryColor ( void ) const { return constraint_secondary_color_; }

  /*!
   * Set the background style.
   * \param style the new background style.
   */
  void setBackgroundStyle ( enum lC::Background::Style style ) { background_style_ = style; }

  /*!
   * \return the background style.
   */
  enum lC::Background::Style backgroundStyle ( void ) const { return background_style_; }

  /*!
   * Set the solid background color.
   * \param color the new solid background color.
   */
  void setBackgroundColor ( const QColor& color ) { background_color_ = color; }

  /*!
   * \return the solid background color.
   */
  QColor backgroundColor ( void ) const { return background_color_; }

  /*!
   * Set the gradient background color.
   * \param color the new gradient background color.
   */
  void setGradientColor ( const QColor& color ) { gradient_color_ = color; }

  /*!
   * \return the gradient background color.
   */
  QColor gradientColor ( void ) const { return gradient_color_; }

  /*!
   * Set the background pattern file name.
   * \param file the new background pattern file name.
   */
  void setPatternFile ( const QString& file ) { pattern_file_ = file; }

  /*!
   * \return the background pattern file name.
   */
  QString patternFile ( void ) const { return pattern_file_; }

  /*!
   * Set the (default) geometry texture image file name.
   * \param file the new geometry texture image file name.
   */
  void setTextureFile ( const QString& file ) { texture_file_ = file; }

  /*!
   * \return the geometry texture image file name.
   */
  QString textureFile ( void ) const { return texture_file_; }
};

/*!
 * This structure encapsulates the drawing attributes for dimensions.
 * See "Engineering Graphics" by Hammond, et al.
 */
class DimensionAttributes {
  //! The font used to draw the numerical value of the dimension.
  //! Stored as a string generated from QFont::toString.
  QString font_;

  //! Arrow head style
  enum lC::ArrowHeadStyle arrow_head_style_;

  //! Arrow head length in inches.
  double arrow_head_length_;

  //! Arrow head length to width ratio
  Ratio arrow_head_width_ratio_;

  //! Dimension number clearance (extra length for extension lines above
  //! and below the area of the dimension value text) in inches.
  double clearance_;

  //! Dimension line thickness in inches.
  double line_thickness_;

  //! Extension line offset (space between edge and extension line) in
  //! inches.
  double extension_line_offset_;

public:
  /*!
   * Constructor provides defaults for all of the attributes.
   * \param font a Qt string font description (from QFont::toString()).
   * \param arrow_head_style the arrow head style.
   * \param arrow_head_length the arrow head length in inches.
   * \param arrow_head_width_ratio the ratio of the arrow head width to length.
   * \param clearance extra length of extension lines above and below
   * the dimension value.
   * \param line_thickness the thickness of dimension lines (used when
   * printing only).
   * \param extension_line_offset gap between dimension reference point
   * and start of extension line.
   */
  DimensionAttributes ( const QString& font = lC::DEFAULT_FONT,
	enum lC::ArrowHeadStyle arrow_head_style = lC::DEFAULT_ARROW_HEAD_STYLE,
			double arrow_head_length = lC::DEFAULT_ARROW_HEAD_LENGTH,
	Ratio arrow_head_width_ratio = lC::DEFAULT_ARROW_HEAD_WIDTH_RATIO,
			double clearance = lC::DEFAULT_DIMENSION_CLEARANCE,
	double line_thickness = lC::DEFAULT_DIMENSION_LINE_THICKNESS,
	double extension_line_offset = lC::DEFAULT_EXTENSION_LINE_OFFSET )
    : font_( font ),
      arrow_head_style_( arrow_head_style ),
      arrow_head_length_( arrow_head_length ),
      arrow_head_width_ratio_( arrow_head_width_ratio ),
      clearance_( clearance ), line_thickness_( line_thickness ),
      extension_line_offset_( extension_line_offset )
  {}

  /*!
   * Set the font.
   * \param font Qt string font description (from QFont::toString()).
   */
  void setFont ( const QString& font ) { font_ = font; }

  /*!
   * \return the face's Qt font string.
   */
  QString font ( void ) const { return font_; }

  /*!
   * Set the style of arrow heads.
   * \param style the style of arrow heads.
   */
  void setArrowHeadStyle ( enum lC::ArrowHeadStyle style ) { arrow_head_style_ = style; }

  /*!
   * \return the style of arrow heads.
   */
  lC::ArrowHeadStyle arrowHeadStyle ( void ) const { return arrow_head_style_; }

  /*!
   * Set the length of arrow heads in inches.
   * \param length the length of arrow heads in inches.
   */
  void setArrowHeadLength ( double length ) { arrow_head_length_ = length; }

  /*!
   * \return the length of arrow heads in inches.
   */
  double arrowHeadLength ( void ) const { return arrow_head_length_; }

  /*!
   * Set the length to width ratio of arrow heads.
   * \param width_ratio the arrow head length to width ratio.
   */
  void setArrowHeadWidthRatio ( Ratio width_ratio ) { arrow_head_width_ratio_ = width_ratio; }

  /*!
   * \return the arrow head length to width ratio.
   */
  Ratio arrowHeadWidthRatio ( void ) const { return arrow_head_width_ratio_; }

  /*!
   * Set the dimension number clearance in inches. The dimension
   * number clearance is the extra length of the extension lines above
   * and below the area of the dimension value text.
   * \param clearance the dimension number clearance in inches.
   */
  void setClearance ( double clearance ) { clearance_ = clearance; }

  /*!
   * \return the dimension number clearance in inches.
   */
  double clearance ( void ) const { return clearance_; }

  /*!
   * Set the dimension line thickness in inches. This is used only
   * when printing.
   * \param thickness the dimension line thickness in inches.
   */
  void setLineThickness ( double thickness ) { line_thickness_ = thickness; }

  /*!
   * \return the dimension line thickness in inches.
   */
  double lineThickness ( void ) const { return line_thickness_; }

  /*!
   * Set the extension line offset in inches. This is the gap between
   * the corner of an edge and the start of the extension line.
   * \param offset the extension line offset in inches.
   */
  void setExtensionLineOffset ( double offset ) { extension_line_offset_ = offset; }

  /*!
   * \return the extension line offset in inches.
   */
  double extensionLineOffset ( void ) const { return extension_line_offset_; }
};

/*!
 * The attributes for drawing text annotations.
 */
class AnnotationAttributes {
  //! The font used to draw the text of an annotation.
  //! Stored as a string generated from QFont::toString.
  QString font_;
public:
  /*!
   * Constructor provides defaults for all of the attributes.
   * \param font a Qt string font description (from QFont::toString()).
   */
  AnnotationAttributes ( const QString& font = lC::DEFAULT_FONT )
    : font_( font )
  {}

  /*!
   * Set the font.
   * \param font Qt string font description (from QFont::toString()).
   */
  void setFont ( const QString& font ) { font_ = font; }

  /*!
   * \return the face's Qt font string.
   */
  QString font ( void ) const { return font_; }
};

/*!
 * The attributes for a resize handle.
 */
class HandleAttributes {
  //! The 1/2 size of the handle edges in inches.
  double size_;

public:
  /*!
   * Constructor provides defaults for all of the attributes.
   * \param size the 1/2 size of the handle edges in inches.
   */
  HandleAttributes ( double size = lC::DEFAULT_HANDLE_SIZE )
    : size_( size )
  {}

  /*!
   * Set the handle 1/2 size.
   * \param size the 1/2 size of the handle edges in inches.
   */
  void setSize ( double size ) { size_ = size; }

  /*!
   * \return the handle's 1/2 size.
   */
  double size ( void ) const { return size_; }
};
#endif // DRAWINGDEFAULTS_H
