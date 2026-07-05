/* -*- c++ -*-
 * listviewitem.h
 *
 * Header for the ListViewItem class
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
#ifndef LISTVIEWITEM_H
#define LISTVIEWITEM_H

#include <QTreeWidget>

// Subclass of QTreeWidgetItem to act on changes to the name of an object
// in the model hierarchy view. Emits a signal when the user completes
// a valid name change.

class ListViewItem;

class ModelHierarchyTreeWidget : public QTreeWidget {
public:
  explicit ModelHierarchyTreeWidget ( QWidget* parent = 0 );

protected:
  bool edit ( const QModelIndex& index, EditTrigger trigger, QEvent* event ) override;

private:
  ListViewItem* editing_item_ = 0;
  int editing_column_ = -1;
};

class ListViewItem : public QObject, public QTreeWidgetItem {
  Q_OBJECT
public:
  ListViewItem ( ModelHierarchyTreeWidget* parent );
  ListViewItem ( ListViewItem* parent, ListViewItem* after );
  ~ListViewItem ();

  ListViewItem* firstChild ( void ) const;
  ListViewItem* nextSibling ( void ) const;

  void setNameEditable ( bool editable );
  bool isNameEditable ( void ) const;
  void startEditingName ( void );
  void finishEditing ( int column );
  void activate ( void );

private:
  bool name_editable_ = false;

signals:
  void nameChanged ( const QString& name );
  void picked ( void );
};

#endif // LISTVIEWITEM_H
