/*
 * viewdata.cpp
 *
 * ViewData class.
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

#include "constants.h"
#include "graphics.h"
#include "viewdata.h"

using namespace Space3D;

ViewData::ViewData ( void )
  : scale_( Ratio(10,2) ), z_angle_0_( 0 ), y_angle_0_( 0 ), z_angle_1_( 0 ),
    show_csys_( true )
{}

ViewData::ViewData ( const QDomElement& xml_rep )
{
  QDomNode n = xml_rep.firstChild();

  while ( !n.isNull() ) {
    QDomElement e = n.toElement();

    if ( !e.isNull() ) {
      if ( e.tagName() == lC::STR::SCALE ) {
	scale_ = Ratio( e.attribute( lC::STR::RATIO_INDEX ).toInt() );
      }
      else if ( e.tagName() == lC::STR::VIEW_POINT ) {
	view_point_[X] = e.attribute( lC::STR::X ).toDouble();
	view_point_[Y] = e.attribute( lC::STR::Y ).toDouble();
	view_point_[Z] = e.attribute( lC::STR::Z ).toDouble();
      }
      else if ( e.tagName() == lC::STR::ORIENTATION ) {
	z_angle_0_ = e.attribute( lC::STR::Z0_ANGLE ).toDouble();
	y_angle_0_ = e.attribute( lC::STR::Y0_ANGLE ).toDouble();
	z_angle_1_ = e.attribute( lC::STR::Z1_ANGLE ).toDouble();
      }
      else if ( e.tagName() == lC::STR::DISPLAY_OPTIONS ) {
	show_csys_ = e.attribute( lC::STR::ORIGIN_CSYS ).toInt();
      }
    }
    n = n.nextSibling();
  }
}

void ViewData::reset ( Space space )
{
  scale_ = Ratio( 10, 2 );

  z_angle_0_ = 0.;
  y_angle_0_ = 0.;
  z_angle_1_ = 0.;

  switch ( space ) {
  case SPACE2D:
    view_point_ = Point( 0, 0, -1 );
    break;
  case SPACE3D:
    view_point_ = Point( 0, 0, 0 );
    break;
  }

  //  show_csys_ = true; // I guess this is just annoying.
}

void ViewData::write ( QDomElement& xml_rep ) const
{
  QDomDocument document = xml_rep.ownerDocument();
  QDomElement view_data_element = document.createElement( lC::STR::VIEW_DATA );

  QDomElement scale_element = document.createElement( lC::STR::SCALE );
  scale_element.setAttribute( lC::STR::RATIO_INDEX, scale_.serial() );

  QDomElement view_point_element = document.createElement( lC::STR::VIEW_POINT );
  view_point_element.setAttribute( lC::STR::X, lC::format( view_point_[X] ) );
  view_point_element.setAttribute( lC::STR::Y, lC::format( view_point_[Y] ) );
  view_point_element.setAttribute( lC::STR::Z, lC::format( view_point_[Z] ) );

  QDomElement orientation_element = document.createElement( lC::STR::ORIENTATION );
  orientation_element.setAttribute( lC::STR::Z0_ANGLE, lC::format( z_angle_0_ ) );
  orientation_element.setAttribute( lC::STR::Y0_ANGLE, lC::format( y_angle_0_ ) );
  orientation_element.setAttribute( lC::STR::Z1_ANGLE, lC::format( z_angle_1_ ) );

  QDomElement display_options_element = document.createElement( lC::STR::DISPLAY_OPTIONS );
  display_options_element.setAttribute( lC::STR::ORIGIN_CSYS, show_csys_ );

  view_data_element.appendChild( scale_element );
  view_data_element.appendChild( view_point_element );
  view_data_element.appendChild( orientation_element );
  view_data_element.appendChild( display_options_element );

  xml_rep.appendChild( view_data_element );
}

bool ViewData::operator== ( const ViewData& b ) const
{
  return scale_ == b.scale_ && show_csys_ == b.show_csys_ &&
    fabs( view_point_[X] - b.view_point_[X] ) < lC::EPSILON &&
    fabs( view_point_[Y] - b.view_point_[Y] ) < lC::EPSILON &&
    fabs( view_point_[Z] - b.view_point_[Z] ) < lC::EPSILON &&
    fabs( z_angle_0_ - b.z_angle_0_ ) < lC::EPSILON &&
    fabs( y_angle_0_ - b.y_angle_0_ ) < lC::EPSILON &&
    fabs( z_angle_1_ - b.z_angle_1_ ) < lC::EPSILON;
}

