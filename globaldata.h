/* -*- c++ -*-
 * globaldata.h
 *
 * Header for Global Data.
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
#ifndef GLOBALDATA_H
#define GLOBALDATA_H

class ModelItem;
class QStringList;

/*!
 * There are a few items of global data (possibly you might even
 * call these constants). This class provides the routine to look
 * them up. There are a few specific names which indicate global
 * items: currently we have SPACE2D and SPACE3D.
 */
class GlobalData {
public:
  /*!
   * Lookup the global model item by name.
   * \param path_components path to item.
   * \return reference to item.
   */
  static ModelItem* lookup ( QStringList& path_components );
};
#endif // GLOBALDATA_H
