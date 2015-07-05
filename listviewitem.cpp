/*
 * listviewitem.cpp
 *
 * ListViewItem class
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

#include "listviewitem.h"

ListViewItem::ListViewItem ( QStandardItem* parent )
  : QStandardItem( 0 ) //TODO constructor ?
{
   //TODO row ?
    parent->setChild(0, this );
  //  cout << "created list view item @ " << this << endl;
}

ListViewItem::ListViewItem ( QStandardItem* parent, QStandardItem* after )
{
    //TODO
    parent->setChild(0, this);
    //  cout << "created ordered list view item @ " << this << endl;
}

ListViewItem::~ListViewItem ( void )
{
  //cout << "deleting list view item @ " << this << ": " << text(0) << endl;
}

void ListViewItem::okRename ( int col )
{
  if ( col != 0 ) return;

  ListViewItem::okRename( col );

  emit nameChanged( text( ) );
}

void ListViewItem::activate ( void )
{
  emit picked();
}
