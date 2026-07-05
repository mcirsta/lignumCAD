/*
 * TabBarContext.cpp
 *
 * TabBarContext class: A tab bar with a context menu.
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

#include <QMenu>
#include <QContextMenuEvent>
#include "tabbarcontext.h"

TabBarContext::TabBarContext ( QWidget* parent, const char* name )
  : QTabBar( parent )
{
  setObjectName( name );

  context_menu_ = new QMenu( this );
  context_menu_->setObjectName( "page_context" );

  setWhatsThis( tr( "<p><b>Page Tab Bar</b></p>\
<p>This tab bar shows the pages in the model.\
 Clicking on a tab makes that the current page. The type of page is symbolized by\
 the icons:<ul>\
<li><img src=\":/images/sketch.png\" width=\"16\" height=\"16\">Sketch</li>\
<li><img src=\":/images/part.png\" width=\"16\" height=\"16\">Part</li>\
<li><img src=\":/images/assembly.png\" width=\"16\" height=\"16\">Assembly</li>\
<li><img src=\":/images/drawing.png\" width=\"16\" height=\"16\">Drawing</li>\
</ul>\
</p>" ) );
}

void TabBarContext::contextMenuEvent ( QContextMenuEvent* cme )
{
  context_menu_->exec( cme->globalPos() );

  cme->accept();
}
