/*
 * figureview.cpp
 *
 * Implementation of FigureView classes
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

#include "designbookview.h"
#include "openglview.h"
#include "pageview.h"
#include "listviewitem.h"
#include "figureview.h"

FigureViewBase::FigureViewBase ( PageView* parent )
  : parent_( parent ), highlighted_( false ), activated_( false )
{
  selection_name_ = parent_->parent()->view()->genSelectionName();
}

FigureViewBase::~FigureViewBase ( void )
{}

Model* FigureViewBase::model ( void ) const
{
  return parent_->model();
}

OpenGLView* FigureViewBase::view ( void ) const
{
  return parent_->view();
}

void FigureViewBase::setHighlighted ( bool highlight )
{
  highlighted_ = highlight;
}

void FigureViewBase::setActivated ( bool activate )
{
  activated_ = activate;
}

namespace Space2D {
  FigureView::FigureView ( PageView* parent )
    : FigureViewBase( parent )
  {}
} // End of Space2D namespace

namespace Space3D {
  FigureView::FigureView ( PageView* parent )
    : FigureViewBase( parent )
  {}
} // End of Space2D namespace
