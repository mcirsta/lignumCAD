/* -*- c++ -*-
 * pageview.h
 *
 * Header for the PageView class
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
#ifndef PAGEVIEW_H
#define PAGEVIEW_H

#include <iostream>

#include <qptrlist.h>
#include <qintdict.h>
#include <qvaluevector.h>
#include <qmap.h>

#include "constants.h"
#include "graphics.h"
#include "viewdata.h"
#include "selectednames.h"
#include "figureview.h"
#include "view.h"

class QTab;
class QAction;
class QListViewItem;
class ListViewItem;
class OpenGLView;
class Model;
class QDomElement;
class DesignBookView;
class lignumCADMainWindow;
class CreateObject;
/*!
 * A PageView is a view of some aspect of the model. Note: this turned
 * out to be too painful to actually do anything, particularly since
 * the Tab really needed to know about the derived Views' type. So,
 * now this is just an interface.
 *
 * Well, it couldn't stay down long! I guess we can keep a list of
 * the figure views here (as long as they don't need to belong
 * to a specific Space#D). I guess the parent can go here, too, as
 * long as we're putting stuff back into this class.
 */
class PageView : public InputObject, public GraphicsObject, public View {
  Q_OBJECT
  DesignBookView* parent_;

  QPtrList< FigureViewBase > figure_views_;

  InputObject* input_object_;

  //! The view transformation as last left by the user.
  ViewData view_data_;

  //! Defer the initial view transform to the OpenGL view.
  bool valid_view_;

  //! The default rendering style of the page (which is always wireframe)
  lC::Render::Style render_style_;

  //! This is a mapping from OpenGL selection names (GLuints) to
  //! Figure views so that when the select() method is run, we can
  //! figure out just which Figure's were selected.
  QIntDict< FigureViewBase > figure_selection_names_;

  //! Figures which are highlighted because the mouse is hovering over them
  SelectedNames highlighted_;
  //! Figures which have been activated
  SelectedNames activated_;

public:
  PageView ( DesignBookView* parent );
  virtual ~PageView ( void );

  /*!
   * A page view can have a dialog which precedes its initial display.
   * If the user cancels the dialog, take this to mean that the page
   * wasn't really wanted. The Design Book view will then remove the page
   * (which will already be in the page view list).
   * \return false if the user cancels the initial configuration dialog.
   * Return true if everything is OK.
   */
  virtual bool configure ( void ) = 0;

  // These are the real things this class can do, which consists
  // entirely of tracking its parent and its children.

  DesignBookView* parent ( void ) const { return parent_; }

  virtual QTab* tab ( void ) const = 0;
  virtual ListViewItem* listViewItem ( void ) const = 0;

  OpenGLView* view ( void ) const;
  Model* model ( void ) const;
  lignumCADMainWindow* lCMW ( void ) const;

  /*!
   * Find the figure view item which would precede the item with the
   * given id.
   * \param page_item list view item for the page.
   * \param id id of figure being added.
   * \return reference to previous list view item in list.
   */
  QListViewItem* previousItem ( ListViewItem* page_item, uint id ) const;

  lC::Render::Style renderStyle ( void ) const { return render_style_; }
  void setRenderStyle ( lC::Render::Style render_style );

  QPtrListIterator< FigureViewBase > figureViews ( void ) const;
  const QIntDict< FigureViewBase >& figureSelectionNames ( void ) const;

  InputObject* inputObject( void ) const { return input_object_; }
  void setInputObject( InputObject* input_object );

  void addFigureView ( FigureViewBase* figure_view );
  void removeFigureView ( FigureViewBase* figure_view );
  FigureViewBase* lastFigureView ( void ) { return figure_views_.last(); }
  void clearFigureViews ( void );

  void cut ( void );
  void copy ( void );
  void paste ( void );

  virtual View* lookup ( QStringList& path_components ) const;
  virtual std::vector<GLuint> lookup ( QValueVector<uint>& id_path ) const;

  /*!
   * (Temporarily) turn off any highlights or activations
   * (usually while printing).
   */
  void hideHighlights ( void );
  /*!
   * Restore any highlights or activations (usually when done printing).
   */
  void restoreHighlights ( void );

  // Subclasses must implement these methods, which mostly consist
  // of presentation of the View.
  virtual Space space ( void ) const = 0;

  virtual uint id ( void ) const = 0;
  virtual QString name ( void ) const = 0;
  virtual void setName ( const QString& name ) = 0;
  virtual DBURL dbURL ( void ) const = 0;
  virtual QValueVector<uint> ID ( void ) const = 0;

  virtual QString type ( void ) const = 0;

  virtual CreateObject* memento ( void ) = 0;

  virtual void show ( void ) const = 0;
  virtual void write ( QDomElement& xml_rep ) const = 0;

  bool viewData ( ViewData& view_data ) const;
  void setViewData ( const ViewData& transform );
  const ViewData& viewData ( void ) const { return view_data_; }

  /*!
   * Paste the given figure onto the page. Reimplemented in
   * each subclass to accept the specific figures.
   * \param xml_rep simplified XML representation of figure.
   */
  virtual void pasteFigure ( const QDomElement& xml_rep ) = 0;

  /*!
   * Alert the page that some attribute of the view has changed,
   * including possibly the view itself. This is passed along
   * to the page's figure views. Why not use a signal? Because
   * there are at least two OpenGL views (the screen and the printer)
   * and the page view does not know about more than the one
   * provided by the design book view.
   */
  virtual void viewAttributeChanged ( void );

  virtual SelectionType defaultSelectionType ( void ) const;

  SelectionType selectionType ( void ) const;
  bool needsPrepressMouseCoordinates ( void ) const;
  virtual void startDisplay ( QPopupMenu* context_menu ) = 0;
  virtual void stopDisplay ( QPopupMenu* context_menu ) = 0;
  void mousePrepress ( QMouseEvent* me, const SelectedNames& selected );
  void mousePress ( QMouseEvent* me, const SelectedNames& selected );
  void mouseDrag ( QMouseEvent* me, const SelectedNames& selected );
  bool mouseRelease ( QMouseEvent* me, const SelectedNames& selected );
  void mouseDoubleClick ( QMouseEvent* me );
  void keyPress ( QKeyEvent* ke );

  void figureComplete ( void );
  void figureModified ( void );
  virtual void cancelOperation ( void ) = 0;

  void activateFigure ( FigureViewBase* figure_view );
  void activateFigure ( const std::vector<GLuint>& selection_name );
  void deactivateFigure( GLuint figure, GLuint geometry );
  void deactivateFigure ( const std::vector<GLuint>& selection_name );
  /*!
   * Check to make sure the new name is not used by another figure
   * of the same type on the page. Displays a modal dialog if the
   * name is already used.
   * \param figure_view figure being changed (so page view can tell
   * which figure view to ignore).
   * \param name new name of figure.
   * \param type type of figure being renamed.
   * \return flag indicating status: OK = rename can proceed, Rejected =
   * name exists and user doesn't want to apply changes after all, Redo =
   * go back to previous editing step.
   */
  lC::RenameStatus uniqueFigureName ( const FigureViewBase* figure_view,
				      const QString& name,
				      const QString& type ) const;
  /*!
   * Make up a unique name for this new figure.
   * \param newName pointer to function returning a possible name.
   * \param type type of figure being named.
   * \return unique name for figure on page.
   */
  QString uniqueName ( QString (*newName)( void ), const QString& type );

signals:
  void cutCopyChanged ( bool copy_available );
  void newInformation ( const QString& information );

protected:
  void deactivateFigures ( void );
  void highlightFigures ( const SelectedNames& selected );
  virtual bool canCutPaste ( void ) const = 0;

private:
  void pickFigure ( QMouseEvent* me, const SelectedNames& selected );
  void activateFigure ( QMouseEvent* me, const SelectedNames& selected );
  void clearHighlighted ( void );
  void clearActivated ( void );
  SelectedNames filter ( const SelectedNames& selected );
};

#endif // PAGEVIEW_H
