/* -*- c++ -*-
 * constructiondatumview.h
 *
 * Header for the Constructiondatumview class
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
#ifndef CONSTRUCTIONDATUMVIEW_H
#define CONSTRUCTIONDATUMVIEW_H

#include <map>

#include "graphics.h"
#include "constructiondatum.h"

class OpenGLBase;

namespace Space3D {
  /*!
   * Abstract view of a construction datum. Mostly virtual.
   */
  class ConstructionDatumView {
  public:
    virtual ~ConstructionDatumView ( void ) {}
    virtual void draw ( OpenGLBase* view ) const = 0;
    virtual void select ( SelectionType select_type ) const = 0;
  };

  /*!
   * The metadata defines a couple of methods to invoke in order to
   * create construction datum views without knowing exactly what
   * kind of datum it is. (Too complicated?...)
   */
  class ConstructionDatumViewMetadata {
  public:
    ConstructionDatumViewMetadata ( void ) {}
    //! Subclass may have something to do (but probably not).
    virtual ~ConstructionDatumViewMetadata ( void ) {}

    //! \return a view for this construction datum.
    virtual ConstructionDatumView* create ( const ConstructionDatum* datum ) const = 0;
  };

  /*!
   * The ConstructionDatumView factory constructs concrete graphics objects
   * for datums.
   */
  class ConstructionDatumViewFactory {
  public:
    //! \return the singleton instance of the constructiondatumview factory.
    static ConstructionDatumViewFactory* instance( void );

    void addConstructionDatumViewMetadata ( const QString& type,
				    const ConstructionDatumViewMetadata* view );

    /*!
     * Create a page and its view from its factory.
     * \param type of construction datum.
     * \param datum construction datum to view.
     * \return the pointer to the new construction datum view.
     */
    ConstructionDatumView* create ( const ConstructionDatum* datum )
    {
      return views_[ datum->type() ]->create( datum );
    }

  protected:
    //! Per "Design Patterns": It can only construct itself. 
    ConstructionDatumViewFactory();

  private:
    //! The singleton instance of the constructiondatumview factory.
    static ConstructionDatumViewFactory* instance_;

    std::map< QString, const ConstructionDatumViewMetadata*> views_;
  };
} // end of Space3D namespace
#endif // CONSTRUCTIONDATUMVIEW_H
