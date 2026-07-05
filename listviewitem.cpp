/*
 * listviewitem.cpp
 *
 * ListViewItem class
 * Copyright (C) 2002 lignum Computing, Inc. <lignumcad@lignumcomputing.com>
 * Copyright (C) 2026 Marius Cirsta <mforce2@gmail.com>
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

#include <QAbstractItemDelegate>

#include "constants.h"

ModelHierarchyTreeWidget::ModelHierarchyTreeWidget ( QWidget* parent )
  : QTreeWidget( parent )
{
  setEditTriggers( QAbstractItemView::SelectedClicked | QAbstractItemView::EditKeyPressed );

  connect( this, &QTreeWidget::itemActivated,
	   this, []( QTreeWidgetItem* item, int ) {
	     ListViewItem* list_item = dynamic_cast<ListViewItem*>( item );
	     if ( list_item != 0 )
	       list_item->activate();
	   } );

  connect( itemDelegate(), &QAbstractItemDelegate::closeEditor,
	   this, [this]( QWidget*, QAbstractItemDelegate::EndEditHint ) {
	     if ( editing_item_ != 0 )
	       editing_item_->finishEditing( editing_column_ );

	     editing_item_ = 0;
	     editing_column_ = -1;
	   } );
}

bool ModelHierarchyTreeWidget::edit ( const QModelIndex& index,
				      EditTrigger trigger, QEvent* event )
{
  ListViewItem* list_item = dynamic_cast<ListViewItem*>( itemFromIndex( index ) );
  if ( list_item == 0 || index.column() != lC::NAME || !list_item->isNameEditable() )
    return false;

  editing_item_ = list_item;
  editing_column_ = index.column();

  return QTreeWidget::edit( index, trigger, event );
}

ListViewItem::ListViewItem ( ModelHierarchyTreeWidget* parent )
  : QTreeWidgetItem( parent )
{
  setText( lC::NAME, tr( "unnamed" ) );
  //  cout << "created list view item @ " << this << endl;
}

ListViewItem::ListViewItem ( ListViewItem* parent, ListViewItem* after )
  : QTreeWidgetItem()
{
  int index = 0;
  if ( after != 0 ) {
    index = parent->indexOfChild( after );
    if ( index < 0 )
      index = parent->childCount() - 1;
    ++index;
  }

  parent->insertChild( index, this );
  setText( lC::NAME, tr( "unnamed" ) );
  //  cout << "created ordered list view item @ " << this << endl;
}

ListViewItem::~ListViewItem ( void )
{
  //cout << "deleting list view item @ " << this << ": " << text(0) << endl;
}

ListViewItem* ListViewItem::firstChild ( void ) const
{
  if ( childCount() == 0 )
    return 0;

  return dynamic_cast<ListViewItem*>( child( 0 ) );
}

ListViewItem* ListViewItem::nextSibling ( void ) const
{
  QTreeWidgetItem* parent_item = QTreeWidgetItem::parent();
  if ( parent_item != 0 ) {
    int index = parent_item->indexOfChild( const_cast<ListViewItem*>( this ) );
    if ( index >= 0 && index + 1 < parent_item->childCount() )
      return dynamic_cast<ListViewItem*>( parent_item->child( index + 1 ) );
  }
  else if ( treeWidget() != 0 ) {
    int index = treeWidget()->indexOfTopLevelItem( const_cast<ListViewItem*>( this ) );
    if ( index >= 0 && index + 1 < treeWidget()->topLevelItemCount() )
      return dynamic_cast<ListViewItem*>( treeWidget()->topLevelItem( index + 1 ) );
  }

  return 0;
}

void ListViewItem::setNameEditable ( bool editable )
{
  name_editable_ = editable;

  if ( editable )
    setFlags( flags() | Qt::ItemIsEditable );
  else
    setFlags( flags() & ~Qt::ItemIsEditable );
}

bool ListViewItem::isNameEditable ( void ) const
{
  return name_editable_;
}

void ListViewItem::startEditingName ( void )
{
  ModelHierarchyTreeWidget* tree = dynamic_cast<ModelHierarchyTreeWidget*>( treeWidget() );
  if ( tree == 0 || !name_editable_ )
    return;

  tree->setCurrentItem( this, lC::NAME );
  tree->editItem( this, lC::NAME );
}

void ListViewItem::finishEditing ( int column )
{
  if ( column != lC::NAME || !name_editable_ )
    return;

  emit nameChanged( text( lC::NAME ) );
}

void ListViewItem::activate ( void )
{
  emit picked();
}
