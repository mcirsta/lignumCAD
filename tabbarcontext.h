/* -*- c++ -*-
 *
 * Header for the TabBarContext class
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
#ifndef TABBARCONTEXT_H
#define TABBARCONTEXT_H

#include <qtabbar.h>

class QPopupMenu;

/*!
 * A QTabBar with an attached context menu.
 */
class TabBarContext : public QTabBar {
  Q_OBJECT

  QPopupMenu* context_menu_;
public:
  TabBarContext ( QWidget* parent = 0, const char* name = 0 );
  ~TabBarContext ( void ) {}

  QPopupMenu* contextMenu ( void ) const { return context_menu_; }

  void contextMenuEvent ( QContextMenuEvent* cme );
};

#endif // TABBARCONTEXT_H
