/* -*- c++ -*-
 * alignmentview.h
 *
 * Header for the Alignment Input class
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
#ifndef ALIGNMENTVIEW_H
#define ALIGNMENTVIEW_H

#include <qdom.h>

#include "graphics.h"

class PageView;

namespace Space2D {

  class ConstrainedLine;
  /*!
   * Handles the creation of coincident constaints.
   */
  class AlignmentCreateInput : public InputObject {
    Q_OBJECT
  public:
    /*!
     * A page view will construct one of these if it wants to implement
     * the creation of coincident constraints.
     * \param parent page view using this input object.
     */
    AlignmentCreateInput ( PageView* parent );
    /*!
     * If this operation is undone, it needs to delete the XML representation
     * of the last operation.
     */
    ~AlignmentCreateInput ( void );
    /*!
     * Coincident constaints are always created between geometry elements.
     * Also, you have to pick two geometries (i.e., PICK mode keeps the
     * last activation active until the operation is complete.
     * \return the selection mode for creating coincident alignments.
     */
    SelectionType selectionType ( void ) const
    {
      return SelectionType( PICK, EDGE );
    }
    //! \return true: do want elements highlighted before picking them.
    bool needsPrepressMouseCoordinates ( void ) const { return true; }
    /*!
     * Prepare to be displayed. Here: prepare for undo/redo, add a cancel
     * option to the context menu, set the active cursor.
     * \param context_menu the OpenGL view's context menu.
     */
    void startDisplay ( QPopupMenu* context_menu );
    /*!
     * Prepare to be hidden from display. Nothing to do really.
     * \param context_menu the OpenGL view's context menu.
     */
    void stopDisplay ( QPopupMenu* /*context_menu*/ ) {}
    /*!
     * (The page view handles everything here, chielfly highlighting geometry.)
     * \param me current mouse position (unused).
     * \param selected currently highlighted geometry elements (unused).
     */
    void mousePrepress ( QMouseEvent* /*me*/, const SelectedNames& /*selected*/ ) {}
    /*!
     * Each time the user clicks on a geometry, it is added to the list
     * of picked geometries. Performs some sanity checking.
     * \param me mouse position of click.
     * \param selected currently highlighted geometry elements.
     */
    void mousePress ( QMouseEvent* me, const SelectedNames& selected );
    //! Does nothing in this context.
    void mouseDrag ( QMouseEvent* /*me*/, const SelectedNames& /*selected*/ ) {}
    /*!
     * Apply the new coincident constraint. Note that this has to catch
     * all the other constraints which may be changed. These are recorded
     * in the undo list. Also, remove cancel option from context menu
     * and inform the parent view that the operation is complete.
     * \param me mouse position of release (unused).
     * \param selected currently highlighted geometry elements (unused).
     */
    bool mouseRelease ( QMouseEvent* me, const SelectedNames& selected );
    //! Does nothing in this context.
    void mouseDoubleClick ( QMouseEvent* /*me*/ ) {}
    /*!
     * Key_Escape cancels the operation.
     * \param ke user's keypress.
     */
    void keyPress ( QKeyEvent* ke );

  private slots:
    /*!
     * Stop the coincident constraint creation: erase the undo/redo command,
     * remove the cancel option from the context menu and inform the
     * parent view that the operation was canceled.
     */
    void cancelOperation ( void );
    /*!
     * When a constrained entity is reconstrained, it may result in
     * its dependencies being reconstrained as well. This slot catches
     * all the cascading changes so that they can be recorded in the
     * undo/redo command.
     */
    void cascadeConstraints ( void );

  private:
    //! Page view which uses this input object.
    PageView* parent_;
    //! Context menu id of the separator between the cancel option
    //! and the other context menu options.
    int separator_id_;
    //! The PRIMARY coincident constraint object.
    ConstrainedLine* from_;
    //! The SECONDARY coincident constraint object.
    ConstrainedLine* to_;
    //! The undo/redo command representation.
    QDomDocument* xml_rep_;
    //! Collects any reconstraints which occur when the from object is
    //! reconstained.
    QDomElement from_ref_xml_;
  };
} // End of Space2D namespace

#endif // ALIGNMENTVIEW_H
