/*
 * TabBarContext.cpp
 *
 * TabBarContext class: A tab bar with a context menu.
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

#include <QMenu>
#include <qwhatsthis.h>

#include "tabbarcontext.h"

TabBarContext::TabBarContext ( QWidget* parent, const char* name )
  : QTabBar( parent, name )
{
  context_menu_ = new QMenu( this, "page_context" );

  QWhatsThis::add( this, tr( "<p><b>Page Tab Bar</b></p>\
<p>This tab bar shows the pages in the model.\
 Clicking on a tab makes that the current page. The type of page is symbolized by\
 the icons:<ul>\
<li><img src=\"sketch_min.png\">Sketch</li>\
<li><img src=\"part_min.png\">Part</li>\
<li><img src=\"assembly_min.png\">Assembly</li>\
<li><img src=\"drawing_min.png\">Drawing</li>\
</ul>\
</p>" ) );
}

void TabBarContext::contextMenuEvent ( QContextMenuEvent* cme )
{
  context_menu_->exec( cme->globalPos() );

  cme->accept();
}
