/* -*- c++ -*-
 * cursorfactory.h
 *
 * Header for the CursorFactory class
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
#ifndef CURSORFACTORY_H
#define CURSORFACTORY_H

#include <qintdict.h>
#include <qcursor.h>

/*!
 * All of the items which might want a custom cursor can share the
 * same QCursor. This is a singleton which doles out cursors as
 * requested. It also gives us a chance to clean up the memory used at
 * program exit.  (QCursors don't have a parent which can easily
 * delete them at program exit.) However, don't call this after
 * the QApplication is destroyed since we no longer have an X connection.
 */

class CursorFactory {
  QIntDict< QCursor > cursors_;

  static CursorFactory cursor_factory_;

public:
  enum Cursors {
    ALIGNMENT,
    ALIGNMENTPLUS,
    CENTERLINE,
    DIMENSION,
    DIMENSIONPLUS,
    RECTANGLE,
    REFERENCELINE,
    LINE_ROTATE,
    ANNOTATION,
    DELETE_CONSTRAINT,
    ASSEMBLY_CONSTRAINT,
    ASSEMBLY_CONSTRAINTPLUS,
    RESIZE_SOLID
  };

  static CursorFactory& instance ( void );

  QCursor& cursor ( enum Cursors c );
  void clear ( void );

protected:
  CursorFactory ( void );
};

#endif /* CURSORFACTORY_H */
