/* -*- c++ -*-
 * constraintview.h
 *
 * Header for the Constraint Input class
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
#ifndef CONSTRAINTVIEW_H
#define CONSTRAINTVIEW_H

#include <qdom.h>

#include "graphics.h"

class PageView;

namespace Space2D {

  class ConstrainedLine;
  /*!
   * Allow the user to delete a constraint. In this case, deletion
   * means to revert back to a reference to a coordinate axis.
   */
  class ConstraintDeleteInput : public InputObject {
    Q_OBJECT

    QMenu* context_menu_;
    int separator_id_;

    PageView* parent_;

    ConstrainedLine* target_;

    QDomDocument* xml_rep_;
    QDomElement from_ref_xml_;

  public:
    ConstraintDeleteInput ( PageView* parent );
    ~ConstraintDeleteInput ( void );

    SelectionType selectionType ( void ) const
    {
      return SelectionType( PICK, EDGE );
    }
    bool needsPrepressMouseCoordinates ( void ) const { return true; }
    void startDisplay ( QMenu* context_menu );
    void stopDisplay ( QMenu* /*context_menu*/ ) {}
    void mousePrepress ( QMouseEvent* me, const SelectedNames& selected );
    void mousePress ( QMouseEvent* me, const SelectedNames& selected );
    void mouseDrag ( QMouseEvent* me, const SelectedNames& selected );
    bool mouseRelease ( QMouseEvent* me, const SelectedNames& selected );
    void mouseDoubleClick ( QMouseEvent* /*me*/ ) {}
    void keyPress ( QKeyEvent* me );

  private slots:
    void cancelOperation ( void );
    void cascadeConstraints ( void );
  };
} // End of Space2D namespace

#endif // CONSTRAINTVIEW_H
