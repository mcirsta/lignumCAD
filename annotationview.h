/* -*- c++ -*-
 * annotationview.h
 *
 * Header for the Annotation View class
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
#ifndef ANNOTATIONVIEW_H
#define ANNOTATIONVIEW_H

#include <map>
#include <qdom.h>

#include "vectoralgebra.h"
#include "figureview.h"
#include "handle.h"

class AnnotationInfoDialog;

namespace lC {
  class ValidDelta;
}
class OGLPaintDevice;

namespace Space2D {
  class Page;
  class Annotation;
  class AnnotationView;

  /*!
   * This input object is used by the page view when creating an annotation.
   */
  class AnnotationCreateInput : public InputObject {
    Q_OBJECT

  public:
    /*!
     * Following the Model-View-Controller paradigm, this is a controller:
     * it modifies the annotation and directs the annotation view.
     * \param annotation object to control.
     * \param annotation_view view of object to direct.
     */
    AnnotationCreateInput ( Annotation* annotation,
			    AnnotationView* annotation_view );
    /*!
     * Annotations don't have contraints so we really don't need to
     * be notified of any highlighted objects when the Annotation is
     * created.
     * \return selection mode: nothing.
     */
    SelectionType selectionType ( void ) const
    {
      return SelectionType( TRANSIENT, NONE );
    }
    //! \return false: Annotations don't need references to other geometry.
    bool needsPrepressMouseCoordinates ( void ) const { return false; }
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
     * (The page view handles everything here.)
     * \param me current mouse position (unused).
     * \param selected currently highlighted geometry elements (unused).
     */
    void mousePrepress ( QMouseEvent* /*me*/, const SelectedNames& /*selected*/ ) {}
    /*!
     * The initial point picked by the user becomes the origin of the annotation.
     * \param me mouse position of click.
     * \param selected currently highlighted geometry elements (unused).
     */
    void mousePress ( QMouseEvent* me, const SelectedNames& selected );
    /*!
     * Drag out the text box.
     * \param me current position of mouse.
     * \param selected currently highlighted geometry elements (unused).
     */
    void mouseDrag ( QMouseEvent* me, const SelectedNames& selected );
    /*!
     * Completes text box. Invoke the annotation text editor dialog to
     * finish creating the annotation.
     * \param me current position of mouse.
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
  public slots:
    /*!
     * Stop the annotation creation operation: remove the cancel option
     * from the context menu and inform the parent view that the
     * operation was canceled.
     */
    void cancelOperation ( void );

  private:
    //! Context menu id of the separator between the cancel option
    //! and the other context menu options.
    int separator_id_;

    //! Annotation object to manipulate.
    Annotation* annotation_;
    //! Annotation view to direct.
    AnnotationView* annotation_view_;
  };

  /*!
   * This input object is used by the page view when modifying the
   * size and shape of an annotation. (Changing the text is handled
   * by the view directly.)
   */
  class AnnotationModifyInput : public InputObject {
    Q_OBJECT

  public:
    /*!
     * Following the Model-View-Controller paradigm, this is a controller:
     * it modifies the annotation and directs the annotation view. Note that
     * this object also has to deal with undoing the modification operation.
     * \param annotation object to control.
     * \param annotation_view view of object to direct.
     */
    AnnotationModifyInput ( Annotation* annotation,
			    AnnotationView* annotation_view );
    /*!
     * After the annotation is activated for modification, the selection
     * mode changes to NONE since the annotation doesn't support any
     * constraints.
     * \return the current selection mode: initially TRANSIENT,FIGURE
     * but becomes TRANSIENT,NONE after activation.
     */
    SelectionType selectionType ( void ) const { return selection_type_; }
    //! \return true: Annotations need to actively define the cursor.
    bool needsPrepressMouseCoordinates ( void ) const { return true; }
    //! Does nothing in this context.
    void startDisplay ( QPopupMenu* /*context_menu*/ ) {}
    //! Does nothing in this context.
    void stopDisplay ( QPopupMenu* /*context_menu*/ ) {}
    /*!
     * Mostly just sets the cursor when it is over a resize handle.
     * \param me current mouse position.
     * \param selected currently highlighted geometry elements.
     */
    void mousePrepress ( QMouseEvent* me, const SelectedNames& selected );
    /*!
     * Locate the highlighted resize handle and start the resize.
     * \param me position of mouse click.
     * \param selected currently highlighted geometry elements.
     */
    void mousePress ( QMouseEvent* me, const SelectedNames& selected );
    /*!
     * Move the annotation or resize the text box as indicated.
     * \param me current mouse position.
     * \param selected currently highlighted geometry elements (unused).
     */
    void mouseDrag ( QMouseEvent* me, const SelectedNames& selected );
    /*!
     * Complete the move/resize operation. Saves a undo/redo command.
     * \param me current mouse position (unused).
     * \param selected currently highlighted geometry elements (unused).
     */
    bool mouseRelease ( QMouseEvent* me, const SelectedNames& selected );
    /*!
     * Invoke the annotation text editor.
     * \param me mouse position of double click (unused).
     */
    void mouseDoubleClick ( QMouseEvent* me );
    //! Does nothing in this context.
    void keyPress ( QKeyEvent* /*ke*/ ) {}
    /*!
     * When an annotation view is constructed from an XML representation,
     * it has not yet discovered its annotation reference. So, it can
     * set that here when it finds out.
     * \param annotation annotation to modify.
     */
    void setAnnotation ( Annotation* annotation );
  public slots:
    /*!
     * Stop the annotation modification operation: actually, there's
     * nothing to do.
     */
    void cancelOperation ( void ) {}

  private:
    //! The current selection mode.
    SelectionType selection_type_;
    //! The annotation to modify.
    Annotation* annotation_;
    //! The annotation view to direct.
    AnnotationView* annotation_view_;
    //! The OpenGL id of the selected drag handle.
    GLuint drag_handle_;
    //! The initial mouse position on click.
    Point start_pnt_;
    //! The mouse position reported on the last drag event.
    Point last_pnt_;
    //! The current mouse drag position.
    Point current_pnt_;
    //! The original origin of the annotation before the move/resize started.
    Point old_origin_;
    //! The old size (diagonal) of the annotation before the move/resize started.
    Vector old_size_;
    //! The undo/redo command representation.
    QDomDocument* xml_rep_;
  };

  /*!
   * AnnotationView is responsible for displaying the annotation. This
   * is an annotation for a 2D view.
   */
  class AnnotationView : public FigureView {
    Q_OBJECT

  public:
    /*!
     * Constructor called when creating an annotation interactively.
     * \param annotation an empty annotation.
     * \param parent parent page view.
     */
    AnnotationView ( Annotation* annotation, PageView* parent );
    /*!
     * Constructor called when an annotation view is created from a file.
     * \param xml_rep XML representation of view.
     * \param parent parent page view.
     */
    AnnotationView ( const QDomElement& xml_rep, PageView* parent );
    /*!
     * Constructor called when an annotation view is pasted. Creates the
     * annotation itself, too.
     * \param name new name of pasted annotation.
     * \param xml_rep XML represention of clipboard-stored annotation.
     * \param page new parent of annotation.
     * \param parent parent page view.
     */
    AnnotationView ( const QString& name, const QDomElement& xml_rep,
		     Page* page, PageView* parent );
    /*!
     * Currently, deleting a view also deletes the annotation object.
     */
    ~AnnotationView ( void );

    //! \return the annotation we're viewing.
    Annotation* annotation ( void ) const { return annotation_; }
    //! \return the creation input object.
    InputObject* createInput ( void ) { return &create_input_; }
    //! \return the modification input object.
    InputObject* modifyInput ( void );
    //! \return an object which is capable of recreating the annotation
    //! and its view (for undo/redo).
    CreateObject* memento ( void );

    // Implementation of FigureView interface

    //! \return the list view item for this page view.
    ListViewItem* listViewItem ( void ) const { return list_view_item_; }
    //! \return the id of the view (same as the annotation).
    uint id ( void ) const;
    //! \return the name of the view (same as the annotation).
    QString name ( void ) const;
    //! \return the type of the view (namely, annotation).
    QString type ( void ) const;
    //! \return the database URL of the object.
    DBURL dbURL ( void ) const;
    /*!
     * Return a string which represents the selection for the entity type.
     * For annotations, it's the name of the annotation regardless of the
     * entity.
     * \param selection_name item's selection name.
     * \param entity entity selected for in view.
     * \return string representing selection.
     */
    QString selectionText ( const std::vector<GLuint>& selection_name,
			    SelectionEntity entity ) const;
    //! \return 0: has no subcomponents
    View* lookup ( QStringList& /*path_components*/ ) const { return 0; }
    /*!
     * Set all the designated items to the given highlight mode.
     * \param highlight new highlight mode.
     * \param entity GEOMETRY/FIGURE selector (but can only be FIGURE here).
     * \param item OpenGL selection names of subcomponents (none here).
     */
    void setHighlighted ( bool highlight, SelectionEntity entity,
			  const std::vector<GLuint>& items );
    /*!
     * Set all the designated items to the given activation mode.
     * \param activate new activation mode.
     * \param entity GEOMETRY/FIGURE selector (but can only be FIGURE here).
     * \param item OpenGL selection names of subcomponents (none here).
     */
    void setActivated ( bool activate, SelectionEntity entity,
			const std::vector<GLuint>& items );
    //! \return 0: annotations cannot be dimensioned.
    uint dimensionPickCount ( void ) const { return 0; }
    //! Does nothing in this context.
    void setDimensionView ( const QDomElement& /*xml_rep*/ ) {}
    /*!
     * Determine if this selection is part of the geometry representation;
     * this includes the handles if they happen to be highlighted, too.
     * Should always return true for the Annotation.
     */
    bool isGeometry ( GLuint selection_name ) const;
    //! \return 0: Annotations have no geometry.
    Curve* geometry ( GLuint /*selection_name*/ ) const { return 0; }
    //    //! \return true if the annotation is activated.
    //    bool isActivated ( GLuint selection_name = 0 ) const;
    /*!
     * Recompute any layout due to changes in the view.
     */
    void viewAttributeChanged ( void );
    /*!
     * General routine activate an editing dialog (usually on double
     * click).
     */
    void editInformation ( void );
    /*!
     * Invoke the annotation text editing dialog. Modifies the text if
     * the user edits it.
     * \return QDialog::Accept or QDialog::Reject depending on
     * what the user chooses.
     */
    int editAnnotationInformation ( void );
    /*!
     * Draw the annotation in the current view.
     */
    void draw ( void ) const;
    /*!
     * Draw the annotation in OpenGL selection mode.
     * \param select_type determines the detail of selection rendering
     * (unused).
     */
    void select ( SelectionType select_type ) const;
    /*!
     * Since the size of the annotation text box is display property, not
     * an Annotation property, the modify object reports a resize here.
     * \param corner current position of mouse in model coordinates.
     */
    void moveCorner ( const Point& corner );
    /*!
     * Set an explicit size for the text box. Note that units of the text
     * box size are paper inches.
     * \param size new size for the text box.
     */
    void setSize ( const Vector& size ) { size_ = size; }
    //! \return the width of the annotation text box in paper units.
    double width ( void ) const { return size_[X]; }
    //! \return the height of the annotation text box in paper units.
    double height ( void ) const { return size_[Y]; }
    //! \return the size of the annotation text box in paper units.
    Vector size ( void ) const { return size_; }
    //! \return the lower left corner of the text box in model units.
    Point lbVertex ( void ) const;
    //! \return the upper left corner of the text box in model units.
    Point ltVertex ( void ) const;
    //! \return the lower right corner of the text box in model units.
    Point rbVertex ( void ) const;
    //! \return the upper right corner of the text box in model units.
    Point rtVertex ( void ) const;
    /*!
     * Execute the appropriate move or resize command based on the handle.
     * \param selection_name selection name of the handle under the mouse.
     * \param last_pnt the previous mouse position in model units.
     * \para current_pnt the current mouse position in model units.
     */
    lC::ValidDelta adjust ( int selection_name, const Point& last_pnt,
			    Point& current_pnt );
    /*!
     * Move the annotation text box by the delta between the two
     * arguments.
     * \param last_pnt the previous mouse position in model units.
     * \para current_pnt the current mouse position in model units.
     */
    lC::ValidDelta move ( const Point& last_pnt, Point& current_pnt );
    /*!
     * Move the left edge of the annotation; changes the width by the delta
     * between the two arguments.
     * \param last_pnt the previous mouse position in model units.
     * \para current_pnt the current mouse position in model units.
     */
    lC::ValidDelta resizeLeft ( const Point& last_pnt, Point& current_pnt );
    /*!
     * Move the right edge of the annotation; changes the width by the delta
     * between the two arguments.
     * \param last_pnt the previous mouse position in model units.
     * \para current_pnt the current mouse position in model units.
     */
    lC::ValidDelta resizeRight ( const Point& last_pnt, Point& current_pnt );
    /*!
     * Move the bottom edge of the annotation; changes the height by the delta
     * between the two arguments.
     * \param last_pnt the previous mouse position in model units.
     * \para current_pnt the current mouse position in model units.
     */
    lC::ValidDelta resizeBottom ( const Point& last_pnt, Point& current_pnt );
    /*!
     * Move the top edge of the annotation; changes the height by the delta
     * between the two arguments.
     * \param last_pnt the previous mouse position in model units.
     * \para current_pnt the current mouse position in model units.
     */
    lC::ValidDelta resizeTop ( const Point& last_pnt, Point& current_pnt );
    /*!
     * Move the left and bottom edges of the annotation; changes the
     * width and height by the delta between the two arguments.
     * \param last_pnt the previous mouse position in model units.
     * \para current_pnt the current mouse position in model units.
     */
    lC::ValidDelta resizeLeftBottom ( const Point& last_pnt, Point& current_pnt );
    /*!
     * Move the right and bottom edges of the annotation; changes the
     * width and height by the delta between the two arguments.
     * \param last_pnt the previous mouse position in model units.
     * \para current_pnt the current mouse position in model units.
     */
    lC::ValidDelta resizeRightBottom ( const Point& last_pnt, Point& current_pnt );
    /*!
     * Move the right and top edges of the annotation; changes the
     * width and height by the delta between the two arguments.
     * \param last_pnt the previous mouse position in model units.
     * \para current_pnt the current mouse position in model units.
     */
    lC::ValidDelta resizeRightTop ( const Point& last_pnt, Point& current_pnt );
    /*!
     * Move the left and top edges of the annotation; changes the
     * width and height by the delta between the two arguments.
     * \param last_pnt the previous mouse position in model units.
     * \para current_pnt the current mouse position in model units.
     */
    lC::ValidDelta resizeLeftTop ( const Point& last_pnt, Point& current_pnt );
    /*!
     * Set the mouse cursor to the appropriate shape for the selected item.
     * \param selection_name graphics object under the mouse.
     */
    void setCursor ( GLuint selection_name );
    /*!
     * Set the mouse cursor to given shape.
     * \param cursor explicit cursor to use.
     */
    void setCursor ( const QCursor* cursor );
    /*!
     * Set the mouse cursor to its default shape.
     */
    void unsetCursor ( void );
    /*!
     * Create an XML representation of this view (only) suitable for
     * saving in a file with the rest of the model information.
     * \param xml_rep the XML representation of the model to append
     * this representation to.
     */
    void write ( QDomElement& xml_rep ) const;
    /*!
     * Create an XML representation for this view *AND* its object suitable
     * for use as a clipboard selection.
     * \param xml_rep the XML representation of the selection to append
     * this representation to.
     */
    void copyWrite ( QDomElement& xml_rep ) const;

  private slots:
    /*!
     * Set the annotation to the given name. Assumes this is an interactive
     * modification and records an undo/redo command.
     * \param name new name.
     */
    void setName ( const QString& name );
    /*!
     * The name in the list view has changed. This has to be checked for
     * redundancy before it can be allowed.
     * \param name new name for list view edit.
     */
    void listNameChanged ( const QString& name );
    /*!
     * On the other hand, if the annotation just spontaneously renames itself,
     * respond to it.
     * \param name new name.
     */
    void updateName ( const QString& name );
    /*!
     * Keep the various views of the annotation up to date due to
     * the annotation being initialized.
     */
    void annotationInitialized ( void );
    /*!
     * Keep the various views of the annotation up to date due to the
     * annotation being moved.
     */
    void annotationMoved ( void );
    /*!
     * Keep the various views of the annotation up to date due to the
     * annotation being resized.
     */
    void annotationResized ( void );

  private:
    //! Handle the common constructor actions.
    void init ( void );
    //! The annotation.
    Annotation* annotation_;
    //! The size of the annotation text box. Given in paper units (inches).
    Vector size_;
    //! The interactive creation input object.
    AnnotationCreateInput create_input_;
    //! The interactive modification input object.
    AnnotationModifyInput modify_input_;
    //! A special Qt paint device which renders using OpenGL; specifically
    //! for text rendering.
    OGLPaintDevice* ogl_paintdevice_;
    //! Model hierarchy view.
    ListViewItem* list_view_item_;
    //! The line describing the left edge of the text box.
    GraphicsView left_edge_;
    //! The line describing the right edge of the text box.
    GraphicsView right_edge_;
    //! The line describing the bottom edge of the text box.
    GraphicsView bottom_edge_;
    //! The line describing the top edge of the text box.
    GraphicsView top_edge_;
    //! The rectangle describing the entire text box.
    GraphicsView solid_annotation_;
    //! The lower left resize handle.
    GraphicsView lb_handle_;
    //! The bottom resize handle.
    GraphicsView mb_handle_;
    //! The lower right resize handle.
    GraphicsView rb_handle_;
    //! The right resize handle.
    GraphicsView mr_handle_;
    //! The upper right resize handle.
    GraphicsView rt_handle_;
    //! The top resize handle.
    GraphicsView mt_handle_;
    //! The upper left resize handle.
    GraphicsView lt_handle_;
    //! The left resize handle.
    GraphicsView ml_handle_;
    //! All of the graphics objects used in the annotation view. For quick
    //! lookup via OpenGL selection name.
    QIntDict< GraphicsView > annotation_objects_;
    //! Convert a handle selection name into a modification action.
    std::map< int, lC::ValidDelta( AnnotationView::* ) ( const Point&, Point& )> adjustments_;
    //! The annotation text editing dialog.
    static AnnotationInfoDialog* annotation_info_dialog_;
  };
} // End of Space2D namespace

#endif // ANNOTATIONVIEW_H
