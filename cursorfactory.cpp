/*
 * cursorfactory.cpp
 *
 * CursorFactory class: Cache custom cursors
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
#include <qbitmap.h>

#include "cursorfactory.h"

#include "cursors/create_align.xbm"
#include "cursors/create_align_mask.xbm"
#include "cursors/create_align_plus.xbm"
#include "cursors/create_align_plus_mask.xbm"
#include "cursors/create_ctrline.xbm"
#include "cursors/create_ctrline_mask.xbm"
#include "cursors/create_dim.xbm"
#include "cursors/create_dim_mask.xbm"
#include "cursors/create_dim_plus.xbm"
#include "cursors/create_dim_plus_mask.xbm"
#include "cursors/create_rect.xbm"
#include "cursors/create_rect_mask.xbm"
#include "cursors/create_refline.xbm"
#include "cursors/create_refline_mask.xbm"
#include "cursors/line_rotate.xbm"
#include "cursors/line_rotate_mask.xbm"
#include "cursors/create_annotation.xbm"
#include "cursors/create_annotation_mask.xbm"
#include "cursors/delete_constraint.xbm"
#include "cursors/delete_constraint_mask.xbm"
#include "cursors/create_asm_constraint.xbm"
#include "cursors/create_asm_constraint_mask.xbm"
#include "cursors/create_asm_constraint_plus.xbm"
#include "cursors/create_asm_constraint_plus_mask.xbm"
#include "cursors/resize_solid.xbm"
#include "cursors/resize_solid_mask.xbm"

CursorFactory CursorFactory::cursor_factory_;

namespace {
  inline QCursor* createCursor ( uchar bits[], uchar mask[],
				 int width, int height, int x_hot, int y_hot ) {
    QBitmap bits_bm( width, height, bits, true );
    QBitmap mask_bm( width, height, mask, true );
    return new QCursor( bits_bm, mask_bm, x_hot, y_hot );
  }
}

CursorFactory::CursorFactory ( void )
{
  cursors_.setAutoDelete( true );
}

CursorFactory& CursorFactory::instance ( void )
{
  return cursor_factory_;
}

QCursor& CursorFactory::cursor ( enum Cursors c )
{
  QCursor* cursor = cursors_[c];

  if ( cursor != 0 ) return *cursor;

  switch ( c ) {
  case ALIGNMENT:
    cursors_.insert( ALIGNMENT,
		     createCursor( create_align_bits, create_align_mask_bits,
				   create_align_width, create_align_height,
				   create_align_x_hot, create_align_y_hot ) );
    break;
  case ALIGNMENTPLUS:
    cursors_.insert( ALIGNMENTPLUS,
	     createCursor( create_align_plus_bits, create_align_plus_mask_bits,
			   create_align_plus_width, create_align_plus_height,
			   create_align_plus_x_hot, create_align_plus_y_hot ) );
    break;
  case CENTERLINE:
    cursors_.insert( CENTERLINE,
		     createCursor( create_ctrline_bits, create_ctrline_mask_bits,
				   create_ctrline_width, create_ctrline_height,
				   create_ctrline_x_hot, create_ctrline_y_hot ) );
    break;
  case DIMENSION:
    cursors_.insert( DIMENSION,
		     createCursor( create_dim_bits, create_dim_mask_bits,
				   create_dim_width, create_dim_height,
				   create_dim_x_hot, create_dim_y_hot ) );
    break;
  case DIMENSIONPLUS:
    cursors_.insert( DIMENSIONPLUS,
		     createCursor( create_dim_plus_bits, create_dim_plus_mask_bits,
				   create_dim_plus_width, create_dim_plus_height,
				   create_dim_plus_x_hot, create_dim_plus_y_hot ) );
    break;
  case RECTANGLE:
    cursors_.insert( RECTANGLE,
		     createCursor( create_rect_bits, create_rect_mask_bits,
				   create_rect_width, create_rect_height,
				   create_rect_x_hot, create_rect_y_hot ) );
    break;
  case REFERENCELINE:
    cursors_.insert( REFERENCELINE,
		     createCursor( create_refline_bits, create_refline_mask_bits,
				   create_refline_width, create_refline_height,
				   create_refline_x_hot, create_refline_y_hot ) );
    break;
  case LINE_ROTATE:
    cursors_.insert( LINE_ROTATE,
		     createCursor( line_rotate_bits, line_rotate_mask_bits,
				   line_rotate_width, line_rotate_height,
				   line_rotate_x_hot, line_rotate_y_hot ) );
    break;
  case ANNOTATION:
    cursors_.insert( ANNOTATION,
		     createCursor( create_annotation_bits, create_annotation_mask_bits,
				   create_annotation_width, create_annotation_height,
				   create_annotation_x_hot, create_annotation_y_hot ) );
    break;
  case DELETE_CONSTRAINT:
    cursors_.insert( DELETE_CONSTRAINT,
		     createCursor( delete_constraint_bits, delete_constraint_mask_bits,
				   delete_constraint_width, delete_constraint_height,
				   delete_constraint_x_hot, delete_constraint_y_hot ) );
    break;
  case ASSEMBLY_CONSTRAINT:
    cursors_.insert( ASSEMBLY_CONSTRAINT,
		     createCursor( create_asm_constraint_bits, create_asm_constraint_ma_bits,
				   create_asm_constraint_width, create_asm_constraint_height,
				   create_asm_constraint_x_hot, create_asm_constraint_y_hot ) );
    break;
  case ASSEMBLY_CONSTRAINTPLUS:
    cursors_.insert( ASSEMBLY_CONSTRAINTPLUS,
		     createCursor( create_asm_constraint_pl_bits, create_asm_constr_pl_ma_bits,
				   create_asm_constraint_pl_width, create_asm_constraint_pl_height,
				   create_asm_constraint_pl_x_hot, create_asm_constraint_pl_y_hot ) );
    break;
  case RESIZE_SOLID:
    cursors_.insert( RESIZE_SOLID,
		     createCursor( resize_solid_bits, resize_solid_mask_bits,
				   resize_solid_width, resize_solid_height,
				   resize_solid_x_hot, resize_solid_y_hot ) );
    break;
  }
  return *cursors_[c];
}

// Evidently you have to release the cursors before you drop your X connection?
void CursorFactory::clear ( void )
{
  cursors_.clear();
}
