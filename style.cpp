/*
 * style.cpp
 *
 * Style class
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
#include <qdom.h>

#include "style.h"

// Construct a Style from its XML representation.

Style::Style ( const QDomElement& xml_rep )
{
  setStyle( lC::Render::style( xml_rep.attribute( lC::STR::STYLE ) ) );
  setEdge( lC::Edge::style( xml_rep.attribute( lC::STR::EDGE ) ) );
  setWireframeColor( xml_rep.attribute( lC::STR::WIREFRAME ) );
  setSolidColor( xml_rep.attribute( lC::STR::SOLID ) );
  setTextureFile( xml_rep.attribute( lC::STR::TEXTURE ) );
}

// Serialize a Style into its XML representation.

void Style::write ( QDomElement& xml_rep ) const
{
  QDomDocument document = xml_rep.ownerDocument();
  QDomElement style_element = document.createElement( lC::STR::STYLE );

  style_element.setAttribute( lC::STR::STYLE, lC::Render::styleText( style()));
  style_element.setAttribute( lC::STR::EDGE, lC::Edge::styleText( edge() ) );
  style_element.setAttribute( lC::STR::WIREFRAME, wireframeColor().name() );
  style_element.setAttribute( lC::STR::SOLID, solidColor().name() );
  style_element.setAttribute( lC::STR::TEXTURE, textureFile() );

  xml_rep.appendChild( style_element );
}
