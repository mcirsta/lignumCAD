/* -*- c++ -*-
 * style.h
 *
 * Header for Style: Defines a common graphics style representation
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
#ifndef STYLE_H
#define STYLE_H

#include "constants.h"
//#include "graphics.h"

class QDomElement;

/*!
 * Store the information necessary to render a GraphicsObject in
 * a given style. The styles are defined in constants.h (why?)
 */
class Style {
public:
  /*!
   * These are constructed lazily, usually with information extracted from a
   * dialog. So, just start with some harmless default values.
   * Note that style represents a sliding scale; each higher level implies
   * the use of the lower level attributes. For example, the SOLID
   * style uses the edge stipple, the wireframe color and the solid color.
   */
  Style ( void )
    : style_( lC::Render::PARENT ), edge_( lC::Edge::SOLID )
  {}
  /*!
   * Restore a Style from its XML representation.
   * \param xml_rep XML representation of style.
   */
  Style ( const QDomElement& xml_rep );

  /*!
   * Set the style to the given format.
   * \param style the new style.
   */
  void setStyle ( lC::Render::Style style ) { style_ = style; }
  /*!
   * \return the current style.
   */
  lC::Render::Style style ( void ) const { return style_; }

  /*!
   * Set the edge stipple style to the given format.
   * \param edge the new edge stipple style.
   */
  void setEdge ( lC::Edge::Style edge ) { edge_ = edge; }
  /*!
   * \return the current edge stipple style.
   */
  lC::Edge::Style edge ( void ) const { return edge_; }

  /*!
   * Set the wireframe (outline) color.
   * \param color the new wireframe (outline) color.
   */
  void setWireframeColor ( const QColor& color ) { wireframe_color_ = color; }
  /*!
   * \return the current wireframe (outline) color.
   */
  QColor wireframeColor ( void ) const { return wireframe_color_; }

  /*!
   * Set the solid fill color.
   * \param color the new solid fill color.
   */
  void setSolidColor ( const QColor& color ) { solid_color_ = color; }
  /*!
   * \return the current solid fill color.
   */
  QColor solidColor ( void ) const { return solid_color_; }

  /*!
   * Set the name of file containing the texture.
   * \param file the name of the file containing the new texture.
   */
  void setTextureFile ( const QString& file ) { texture_file_ = file; }
  /*!
   * \return the name of the file containing the current texture.
   */
  QString textureFile ( void ) const { return texture_file_; }
  /*!
   * Serialize Style into it XML representation.
   * \param xml_rep XML DOM element to append style representation to.
   */
  void write ( QDomElement& xml_rep ) const;

private:
  //! The general style.
  lC::Render::Style style_;
  //! A custom edge stipple style.
  enum lC::Edge::Style edge_;
  //! Custom outline color.
  QColor wireframe_color_;
  //! Custom fill color.
  QColor solid_color_;
  //! Custom texture.
  QString texture_file_;
};

#endif // STYLE_H
