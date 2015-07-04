/*
 * pagefactory.cpp
 *
 * PageFactory class: a factory of a model page.
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
#include <QVector.h>

#include "constants.h"
#include "pagefactory.h"

PageFactory* PageFactory::instance_ = 0;

PageFactory::PageFactory ( void )
{
  //  cout << "constructing a page view factory" << endl;
}

PageFactory* PageFactory::instance ( void )
{
  if ( instance_ == 0 )
    instance_ = new PageFactory;

  return instance_;
}

// Add a new page type.

void PageFactory::addPageMetadata ( uint id, PageMetadata* page_data )
{
  page_data_.insert( id, page_data );
}

// Extract the keys from the page data array.

QVector<uint> PageFactory::pageIDs ( void ) const
{
  QVector<uint> page_ids( page_data_.count() );
  QMap<uint, PageMetadata*>::const_iterator page = page_data_.begin();
  for ( uint i = 0; page != page_data_.end(); ++page, ++i ) {
    page_ids[i] = page.key();
  }
  return page_ids;
}

// Create a page from its XML representation.

PageBase* PageFactory::create ( Model* parent,
				const QDomElement& xml_rep ) const
{
  // Scan the list for the proper page.
  QMap<uint, PageMetadata*>::const_iterator page = page_data_.begin();
  for ( ; page != page_data_.end(); ++page ) {
    if ( (*page)->type() == xml_rep.tagName() ) {
      return (*page)->create( parent, xml_rep );
    }
  }
  return 0;
}

// Create a page view from its XML representation.

PageView* PageFactory::create ( DesignBookView* parent,
				const QDomElement& xml_rep ) const
{
  // Scan the list for the proper page.
  QMap<uint, PageMetadata*>::const_iterator page = page_data_.begin();
  for ( ; page != page_data_.end(); ++page ) {
    if ( (*page)->type() + lC::STR::VIEW_EXT == xml_rep.tagName() ) {
      return (*page)->create( parent, xml_rep );
    }
  }
  return 0;
}
