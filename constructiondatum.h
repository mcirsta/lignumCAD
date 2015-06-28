/* -*- c++ -*-
 * constructiondatum.h
 *
 * Header for the Construction Datum classes
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
#ifndef CONSTRUCTIONDATUM_H
#define CONSTRUCTIONDATUM_H

#include "figure.h"

namespace Space3D {
  /*!
   * Construction Datums are figures like Axes or Datum planes. Not entirely
   * sure where this class is going, though.
   */
  class ConstructionDatum : public Figure {
    Q_OBJECT

  public:
    ConstructionDatum ( uint id, const QString& name, const QString& type,
			Page* parent )
      : Figure( id, name, type, parent )
      {}
    virtual ~ConstructionDatum ( void ) {}
  };
} // end of Space3D namespace
#endif // CONSTRUCTIONDATUM_H
