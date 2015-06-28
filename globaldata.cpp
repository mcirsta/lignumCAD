/*
 * globaldata.cpp
 *
 * GlobalData class: access model items which are global in nature.
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
#include <qstringlist.h>

#include "constants.h"
#include "line.h"
#include "globaldata.h"

// Lookup the global item by name.

ModelItem* GlobalData::lookup ( QStringList& path_components )
{
  if ( path_components.front() == lC::STR::SPACE2D ) {
    path_components.erase( path_components.begin() );
    if ( path_components.front() == lC::STR::X_AXIS )
      return &Space2D::X_AXIS;
    else if ( path_components.front() == lC::STR::Y_AXIS )
      return &Space2D::Y_AXIS;
  }

  return 0;
}
