/*
 * pageview.cpp
 *
 * PageView class: a view of a model page.
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
#include <algorithm>

#include <qapplication.h>
#include <qtabbar.h>
#include <qclipboard.h>
#include <qcursor.h>
#include <qmessagebox.h>
#include <qwhatsthis.h>

#include "configuration.h"
#include "command.h"
#include "constrainthistory.h"
#include "figure.h"
#include "listviewitem.h"
#include "designbookview.h"
#include "openglview.h"
#include "pageview.h"

#include "lcdebug.h"

PageView::PageView ( DesignBookView* parent )
  : parent_( parent ), input_object_( 0 ),
    valid_view_( false ), render_style_( lC::Render::WIREFRAME )
{
  figure_views_.setAutoDelete( true );
}

PageView::~PageView ( void )
{}

Model* PageView::model ( void ) const
{
  return parent_->model();
}

OpenGLView* PageView::view ( void ) const
{
  return parent_->view();
}

lignumCADMainWindow* PageView::lCMW ( void ) const
{
  return parent_->lCMW();
}

// Search for the place where this page view goes in the hierarchy list.

QListViewItem* PageView::previousItem ( ListViewItem* page_item, uint id ) const
{
  QListViewItem* previous_item = 0;
  QListViewItem* item = page_item->firstChild();
  QPtrListIterator<FigureViewBase> f( figure_views_ );
  for ( ; f.current() != 0; ++f ) {
    if ( f.current()->id() > id ) break;

    previous_item = item;
    item = item->nextSibling();
  }
  return previous_item;
}

void PageView::setRenderStyle ( lC::Render::Style render_style )
{
  render_style_ = render_style;
}

QPtrListIterator< FigureViewBase > PageView::figureViews ( void ) const
{
  return QPtrListIterator< FigureViewBase >( figure_views_ );
}

const QIntDict< FigureViewBase >& PageView::figureSelectionNames ( void ) const
{
  return figure_selection_names_;
}

/*
 * If an input object is set on the page, then PageView sends mouse
 * events to it first. If the object ignores the event, PageView
 * processes it some more. When a new input object is set,
 * any current selections are cleared and the view is notified to
 * update the display and start a new select scan.
 * \param input_object A new object expecting mouse (keyboard?) input.
 */
void PageView::setInputObject( InputObject* input_object )
{
  //  cout << "Page view got a new input object " << input_object << endl;

  input_object_ = input_object;

  if ( input_object_ == 0 )
    view()->unsetCursor();

  view()->inputObjectChanged();
}

/*
 * Add the figure's view to the page view.
 * \param figure_view The figure's view.
 */
void PageView::addFigureView ( FigureViewBase* figure_view )
{
  figure_views_.append( figure_view );
  figure_selection_names_.replace( figure_view->selectionName(),
				   figure_view );
}

void PageView::removeFigureView ( FigureViewBase* figure_view )
{
  // If this figure is highlighted or activated, deactive it
  bool active = false;
  // Search for all instances of activated parts of this figure.
  SelectedNames::iterator f = activated_.begin();
  while ( f != activated_.end() ) {
    if ( (*f).second[0] == figure_view->selectionName() ) {
      active = true;
      figure_selection_names_[ (*f).second[0] ]->setActivated( false,
							       selectionType().entity_,
							       (*f).second );
      SelectedNames::iterator g = f;
      ++f;
      activated_.erase( g );
    }
    else
      ++f;
  }
  // Search for all instances of highlighted parts of this figure.
  f = highlighted_.begin();
  while ( f != highlighted_.end() ) {
    if ( (*f).second[0] == figure_view->selectionName() ) {
      active = true;
      figure_selection_names_[ (*f).second[0] ]->setHighlighted( false,
								 selectionType().entity_,
								 (*f).second );
      SelectedNames::iterator g = f;
      ++f;
      highlighted_.erase( g );
    }
    else
      ++f;
  }

  // Assume that if the figure was activated, it was participating in
  // an input operation which now must be cancelled.
  if ( active && input_object_ != 0 ) {
    input_object_->cancelOperation();
    setInputObject( 0 );
  }

  figure_selection_names_.remove( figure_view->selectionName() );
  // Note: Autodelete causes the figure view to be deleted. The
  // the figure view destructor in turn deletes the figure itself.
  figure_views_.removeRef( figure_view );
}

void PageView::clearFigureViews ( void )
{
  figure_views_.clear();
}

/*
 * Delete (with the possibility to get it back later) any activated
 * figures.
 */
void PageView::cut ( void )
{
  if ( !canCutPaste() ) return;

  setInputObject( 0 );

  copy();

  SelectedNames::const_iterator f;

  for ( f = activated_.begin(); f != activated_.end(); ++f ) {
    FigureViewBase* fv = figure_selection_names_[ (*f).second[0] ];

    fv->setActivated( false, selectionType().entity_, (*f).second );

    SelectedNames::iterator fh = highlighted_.begin();
    while ( fh != highlighted_.end() ) {
      if ( (*fh).second[0] == fv->selectionName() ) {
	figure_selection_names_[ (*fh).second[0] ]->setHighlighted( false,
								    selectionType().entity_,
								    (*fh).second );
	SelectedNames::iterator gh = fh;
	++fh;
	highlighted_.erase( gh );
      }
      else
	++fh;
    }

    figure_selection_names_.remove( fv->selectionName() );

    // It's like this:
    // 1. make a memento (essentially an XML representation of the object).

    DeleteCommand* deleteit = new DeleteCommand( QString( "delete %1" ).
						 arg( fv->type() ),
						 fv->memento() );

    // 2. Get ready to catch any constraints which are broken by this
    // deletion.

    ConstraintHistory::instance().init();

    // 3. Delete it (via AutoDelete). This may cause a cascade of changing
    // constraints...

    figure_views_.removeRef( fv );

    // 4. Add the list of constraints which were affected by the
    // deletion to the memento.

    ConstraintHistory::instance().
      takeConstraints( deleteit->creator()->representation() );

    CommandHistory::instance().addCommand( deleteit );
  }

  activated_.clear();

  // Alert the UI that we can paste something (but now there is no active
  // selection)
  emit cutCopyChanged( false );
}

/*
 * Copy the activated figure into the clipboard/selection system.
 */
void PageView::copy ( void )
{
  if ( !canCutPaste() ) return;

  QDomDocument xml_doc( lC::STR::UI_SELECTION );
  QDomElement selection = xml_doc.createElement( lC::STR::UI_SELECTION );

  QDomElement page_element = xml_doc.createElement( lC::STR::PAGE );

  page_element.setAttribute( lC::STR::TYPE, type() );

  SelectedNames::const_iterator f;

  for ( f = activated_.begin(); f != activated_.end(); ++f ) {
    QDomElement figure_element = xml_doc.createElement( lC::STR::FIGURE );

    FigureViewBase* fv = figure_selection_names_[ (*f).second[0] ];

    figure_element.setAttribute( lC::STR::URL, fv->dbURL() );

    fv->copyWrite( figure_element );

    page_element.appendChild( figure_element );
  }

  selection.appendChild( page_element );
  xml_doc.appendChild( selection );

  QApplication::clipboard()->setText( xml_doc.toString() );

  if ( QApplication::clipboard()->supportsSelection() ) {
    QApplication::clipboard()->setSelectionMode( true );
    QApplication::clipboard()->setText( xml_doc.toString() );
    QApplication::clipboard()->setSelectionMode( false );
  }

  // Alert the UI that we can now paste something.
  emit cutCopyChanged( true );
}

/*
 * If it's appropriate, copy the figure in the clipboard into the page.
 */
void PageView::paste ( void )
{
  if ( !canCutPaste() ) return;

  QDomDocument xml_doc;

  xml_doc.setContent( QApplication::clipboard()->text() );

  if ( xml_doc.doctype().name() != lC::STR::UI_SELECTION ) return;

  QDomNodeList page_list = xml_doc.elementsByTagName( lC::STR::PAGE );

  if ( page_list.count() == 0 ) return;

  for ( uint p = 0; p < page_list.count(); ++p ) {

    QDomElement page_element = page_list.item( p ).toElement();

    if ( !page_element.isElement() ) continue;

    if ( page_element.attribute( lC::STR::TYPE ) != type() ) {
      cout << "selection type not commensurate with current page" << endl;
      continue;
    }

    QDomNode n = page_element.firstChild();

    while ( !n.isNull() ) {

      QDomElement e = n.toElement();

      if ( !e.isNull() ) {
	pasteFigure( e );
      }

      n = n.nextSibling();
    }
  }

  view()->updateGL();
}

/*
 * Lookup the figure view 
 */
View* PageView::lookup ( QStringList& path_components ) const
{
  // The front path component is the name of a figure with ".type" appended
  // to it.
  int dot_pos = path_components.front().findRev( '.' );
  QString name = path_components.front().left( dot_pos );
  QString type = path_components.front().right( path_components.front().length()
						- dot_pos - 1 );

  QPtrListIterator< FigureViewBase > fv( figure_views_ );

  for ( ; fv.current(); ++fv ) {
    if ( fv.current()->name() == name && fv.current()->type() == type ) {
      // Pop the figure name off the list
      path_components.erase( path_components.begin() );
      if ( path_components.empty() )
	return fv.current();
      else
	return fv.current()->lookup( path_components );
    }
  }
  return 0;
}

// Lookup the OpenGL selection name path for the given object.

std::vector<GLuint> PageView::lookup ( QVector<uint>& id_path ) const
{
  std::vector<GLuint> name_path;

  QPtrListIterator< FigureViewBase > fv( figure_views_ );

  for ( ; fv.current(); ++fv ) {
    if ( fv.current()->id() == id_path[0] ) {
      name_path.push_back( fv.current()->selectionName() );

      // Pop the figure name off the list
      id_path.erase( id_path.begin() );

      if ( !id_path.empty() )
	fv.current()->lookup( id_path, name_path );

      break;
    }
  }

  return name_path;
}
/*
 * If the page is visible for the first time, then it does not
 * have valid transformation matrices. Defer to the OpenGL view.
 */
bool PageView::viewData ( ViewData& view_data ) const
{
  if ( !valid_view_ ) return false;

  view_data = view_data_;

  return true;
}

/*
 * The OpenGL view has a new viewing transformation for us.
 */
void PageView::setViewData ( const ViewData& view_data )
{
  view_data_ = view_data;

  valid_view_ = true;
}

/*
 * Something about the view has changed. Pass this on to all the figure
 * views.
 */
void PageView::viewAttributeChanged ( void )
{
  QPtrListIterator< FigureViewBase > fv( figure_views_ );

  for ( ; fv.current() != 0; ++fv )
    fv.current()->viewAttributeChanged();
}

SelectionType PageView::defaultSelectionType ( void ) const
{
  return SelectionType();
}

SelectionType PageView::selectionType ( void ) const
{
  if ( input_object_ != 0 )
    return input_object_->selectionType();
  else
    return defaultSelectionType();
}

SelectedNames PageView::filter ( const SelectedNames& selected )
{
  SelectedNames filtered_names;

  switch ( selectionType().filter_ ) {
  case OFF:
    return selected;
  case INCLUSIVE: {
    SelectedNames::const_iterator s = selected.begin();
    for ( ; s != selected.end(); ++s ) {
      if ( (*s).second[0] == selectionType().figure_ )
	filtered_names.insert( *s );
    }
  }
  break;
  case EXCLUSIVE: {
    SelectedNames::const_iterator s = selected.begin();
    for ( ; s != selected.end(); ++s ) {
      if ( (*s).second[0] != selectionType().figure_ )
	filtered_names.insert( *s );
    }
  }
  }

  return filtered_names;
}

bool PageView::needsPrepressMouseCoordinates ( void ) const
{
  if ( input_object_ != 0 )
    return input_object_->needsPrepressMouseCoordinates();
  else
    return true;
}

/*
 * Turn off the old highlights and highlight the new ones. NOTE: this
 * only works if the selection type is same as the last time
 * highlights were set. In general, you need to call this routine
 * with an empty selected argument between selection type changes.
 */
void PageView::highlightFigures ( const SelectedNames& selected )
{
  SelectedNames::const_iterator f;

  // No, No ... shut them all down - C3PO
  for ( f = highlighted_.begin(); f != highlighted_.end(); f++ )
    figure_selection_names_[ (*f).second[0] ]->setHighlighted( false,
							   selectionType().entity_,
							   (*f).second );
  // Need to save a copy of the current selection so we can accurately
  // tell if the selection has changed.
  highlighted_ = selected;

  if ( !selected.empty() ) {
    GLfloat z = (*selected.begin()).first;
    for ( f = selected.begin(); f != selected.end(); f++ ) {
      if ( (*f).first > z ) break;
      figure_selection_names_[ (*f).second[0] ]->setHighlighted( true,
								 selectionType().entity_,
								 (*f).second );
    }
  }

  // Give the user a little bit more feedback about what is highlighted.
  // (In general, we need some kind of mechanism for selecting more than
  // one thing at a time...)
  if ( !selected.empty() ) {
    QString text = figure_selection_names_[ (*selected.begin()).second[0] ]->
      selectionText( (*selected.begin()).second, selectionType().entity_ );
    emit newInformation( lC::formatName( text ) );
  }
  else {
    emit newInformation( trC( lC::STR::NONE ) );
  }
}

// Temporarily turn off any highlights and activations.

void PageView::hideHighlights ( void )
{
  SelectedNames::const_iterator f;

  for ( f = highlighted_.begin(); f != highlighted_.end(); f++ )
    figure_selection_names_[ (*f).second[0] ]->setHighlighted( false,
							   selectionType().entity_,
							   (*f).second );

  for ( f = activated_.begin(); f != activated_.end(); f++ )
    figure_selection_names_[ (*f).second[0] ]->setActivated( false,
							 selectionType().entity_,
							 (*f).second );
}

// Turn on any highlights or activations which were previously active.

void PageView::restoreHighlights ( void )
{
  SelectedNames::const_iterator f;

  for ( f = highlighted_.begin(); f != highlighted_.end(); f++ )
    figure_selection_names_[ (*f).second[0] ]->setHighlighted( true,
							   selectionType().entity_,
							   (*f).second );

  for ( f = activated_.begin(); f != activated_.end(); f++ )
    figure_selection_names_[ (*f).second[0] ]->setActivated( true,
							 selectionType().entity_,
							 (*f).second );
}

void PageView::mousePrepress ( QMouseEvent* me, const SelectedNames& selected )
{
  //  cout << now() << "(unfiltered) Prepress: " << endl << selected;
  // Limit our use to the interesting (as determined by selectionType) records.
  SelectedNames filtered_names = filter( selected );

  // If the same figures are selected as before, then the mouse hasn't
  // moved enough to bother redrawing anything (this is signaled to the
  // OpenGLView by ignoring the mouse event).

  if ( filtered_names == highlighted_ ) {
    me->ignore();
    return;
  }

  // Turn off the old highlights and highlight these instead.
  if ( selectionType().entity_ != NONE ) {
    highlightFigures( filtered_names );
  }

  if ( input_object_ != 0 )
    input_object_->mousePrepress( me, filtered_names );
}

void PageView::mouseDrag ( QMouseEvent* me, const SelectedNames& selected )
{
  SelectedNames filtered_names = filter( selected );
  // Clear out the old highlights and draw any new ones (except in Pick mode,
  // where it does noting and looks kind of strange).
  if ( selectionType().mode_ != PICK && selectionType().entity_ != NONE )
    highlightFigures( filtered_names );

  if ( selectionType().mode_ != PICK )
    if ( input_object_ != 0 )
      input_object_->mouseDrag( me, filtered_names );
}
/*
 * PICKing is different from TRANSIENT selection mode in that the
 * input object is basically waiting for a list of things for the
 * user to select. So, we just keep adding activated figures until
 * the input object has enough.
 */
void PageView::pickFigure ( QMouseEvent* /*me*/, const SelectedNames& selected )
{
  // Get the "first" element
  SelectedNames::const_iterator f = selected.begin();

  // Protect against random clicking
  if ( f == selected.end() ) return;

  // Add this figure and its geometry detail to the activated list
  activated_ += selected;

  // Activate the figure itself and only its first geometry.
  figure_selection_names_[ (*f).second[0] ]->setActivated( true,
						       selectionType().entity_,
						       (*f).second );

  // Turn off any highlights which may have been on
  highlightFigures( SelectedNames() );
}
/*
 * Activate the top-most selected figure. The exact figure selected
 * would seem to depend on the details of the implementation of QMap.
 * Currently: "The items in the map are traversed in the order defined
 * by operator<(Key, Key)"
 * Should definitely ask the user what is wanted if more than one
 * figure is selected...
 */
void PageView::activateFigure ( QMouseEvent* me, const SelectedNames& selected )
{
  // Get the "first" element
  SelectedNames::const_iterator f = selected.begin();
  // This will be the only figure in activated_ after this operation
  activated_.clear();
  activated_.insert( (*selected.begin()) );

  // And activate the figure itself. Note, if we coalesce rectangle and
  // dimension view, we'll have to pass the sub-names, too...
  figure_selection_names_[ (*f).second[0] ]->setActivated( true,
						       selectionType().entity_,
						       (*f).second );
  // Turn off any highlights in the current mode
  highlightFigures( SelectedNames() );

  // This now the active input object, too
  setInputObject( figure_selection_names_[ (*f).second[0] ]->modifyInput() );

  input_object_->mousePrepress( me, selected );
  input_object_->mousePress( me, selected );

  // Redraw the highlights according to the current selection mode
  highlightFigures( selected );

  // Alert the UI that we can cut or copy something. Note: this should
  // be limited to real geometry
  emit cutCopyChanged( true );
}
/*
 * Programmatically activate the figure.
 */
void PageView::activateFigure ( FigureViewBase* figure_view )
{
  deactivateFigures();

  SelectedNames::iterator f =
    activated_.insert( std::pair<GLfloat, std::vector<GLuint> >( 0., std::vector<GLuint>(1) ) );
  (*f).second[0] = figure_view->selectionName();
  
  figure_view->setActivated( true, selectionType().entity_, (*f).second );

  setInputObject( figure_view->modifyInput() );

  // Alert the UI that we can cut or copy something. Note: this should
  // be limited to real geometry
  emit cutCopyChanged( true );
}

// Programmatically activate (pick, actually) the figure given by the GL path.

void PageView::activateFigure ( const std::vector<GLuint>& selection_name )
{
  activated_.insert( std::pair<GLfloat, std::vector<GLuint> >( 0., selection_name ) );

  FigureViewBase* figure_view = figure_selection_names_[ selection_name[0] ];
  
  figure_view->setActivated( true, selectionType().entity_, selection_name );

  // Alert the UI that we can cut or copy something. Note: this should
  // be limited to real geometry
  emit cutCopyChanged( true );

  view()->updateGL();
}

/*
 * Turn off the given geometry of the figure
 */
void PageView::deactivateFigure ( GLuint figure, GLuint geometry )
{

  SelectedNames::iterator f = activated_.begin();

  for ( ; f != activated_.end(); ++f ) {
    if ( (*f).second[0] == figure ) {
      std::vector<GLuint>::const_iterator g = find( (*f).second.begin(),
					       (*f).second.end(), geometry );
      if ( g != (*f).second.end() ) {
	figure_selection_names_[ (*f).second[0] ]->setActivated( false,
								 selectionType().entity_,
								 (*f).second );
	// I think we can just remove this record and be done with this...
	SelectedNames::iterator fp = f;
	++f;
	activated_.erase( fp );
#if 0
	// Well, what happens without it?
	// Need to make sure that this always reasonable...
	setInputObject( 0 );
#endif
	break;
      }
    }
  }
}

// Turn off the given figure (geometry, etc...)

void PageView::deactivateFigure ( const std::vector<GLuint>& selection_name )
{
  if ( selection_name.empty() ) return;

  // Find this figure is in the activated list.

  SelectedNames::iterator f = activated_.begin();

  for ( ; f != activated_.end(); ++f ) {
    // Note: compare the full vectors!
    if ( (*f).second == selection_name ) {

      figure_selection_names_[ (*f).second[0] ]->setActivated( false,
						       selectionType().entity_,
							       selection_name );

      // I think we can just remove this record and be done with this...
      activated_.erase( f );

      break;
    }
  }
}

/*
 * Turn off all the figures which were activated
 */
void PageView::deactivateFigures ( void )
{
  // No, No ... shut them all down - C3PO
  SelectedNames::const_iterator f;

  for ( f = activated_.begin(); f != activated_.end(); f++ )
    figure_selection_names_[ (*f).second[0] ]->setActivated( false,
							     selectionType().entity_,
							     (*f).second );
  activated_.clear();

  // Enforces TRANSIENT FIGURE selection mode.
  setInputObject( 0 );

  // Alert the UI that nothing is selected
  emit cutCopyChanged( false );
}

/*
 * This routine is in desperate need of an explanation...
 */
void PageView::mousePress ( QMouseEvent* me, const SelectedNames& selected )
{
  /*
   * Some explanation:
   * If no figure is expecting input, then input_object_ is zero and either:
   * 1. There was nothing under the mouse when the button was pressed
   *    -> enter VIEW_CHANGE mode (handled by OpenGLView and indicated by
   *    this routine ignoring the mouse event)
   * 2. There was something under the mouse -> activate the top-most figure
   */
  if ( input_object_ == 0 ) {

    if ( selected.empty() )
      me->ignore();
    else {
      // activate the object under the pointer: the object can only be
      // a Figure in the current usage (i.e., when there is no input_object
      // the selection type is always for TRANSIENT FIGURE)
      activateFigure( me, selected );
    }
  }
  /*
   * If an input object is active, then give it a chance to handle
   * the figure selections.
   */
  else {
    SelectedNames filtered_names = filter( selected );
    // Hmm. IO's mousePress method can change the selection mode, so
    // we need to unhighlight anything in the current selection mode.
    highlightFigures( SelectedNames() );

    input_object_->mousePress( me, filtered_names );

    switch ( selectionType().mode_ ) {
    // In PICK mode, if it was of interest to the input object, then
    // add the figure to the list of activated figures.
    case PICK:
      if ( me->isAccepted() )
	pickFigure( me, filtered_names );
      break;
    case TRANSIENT:
      // If it doesn't want them, deactivate any activated figures
      if ( !me->isAccepted() ) {
	deactivateFigures();
	// If the mouse was hovering over something, activate that instead
	if ( !filtered_names.empty() ) {
	  activateFigure( me, filtered_names );
	}
      }
    }
  }
}
 
bool PageView::mouseRelease ( QMouseEvent* me, const SelectedNames& selected )
{
  if ( input_object_ != 0 ) {
    // Save the currently highlighted figures for the call to mouseRelease.
    // This is what is currently drawn on the screen and, hopefully, what
    // the user intends to act on. Note that mouseRelease can change the
    // selection mode, so we need to turn off any highlights now.
    SelectedNames current_highlights = highlighted_;

    highlightFigures( SelectedNames() );

    bool done = input_object_->mouseRelease( me, current_highlights ); 

    // If IO still wants input, treat this as a new prepress event as well.
    if ( !done ) {
      SelectedNames filtered_names = filter( selected );

      highlightFigures( filtered_names );

      // Check to see if there is anything meaningful under the cursor,
      // which may have moved between calling this routine and now.
      // (Why? Well, Annotation pops up a dialog forcing the user to
      // let go of the mouse.)
      QMouseEvent new_me( QEvent::MouseMove,
			  view()->mapFromGlobal( QCursor::pos() ), 0, 0 );
      SelectedNames new_selects;
      view()->select( &new_me, new_selects );
      filtered_names = filter( new_selects );
      input_object_->mousePrepress( &new_me, new_selects );
    }

    if ( done ) {
      // Only in PICK mode is it necessary to turn off the activated figures.
      // In TRANSIENT mode, we can continue to manipulate the activated figure.
      if ( selectionType().mode_ == PICK )
	deactivateFigures();

      setInputObject( 0 );
    }
  }

  return false;
}
 
void PageView::mouseDoubleClick ( QMouseEvent* me )
{
  if ( input_object_ != 0 ) {
    input_object_->mouseDoubleClick( me );
    // The mouse might have gone on holiday.
    QMouseEvent new_me( QEvent::MouseMove,
			view()->mapFromGlobal( QCursor::pos() ), 0, 0 );
    SelectedNames new_selects;
    view()->select( &new_me, new_selects );
    input_object_->mousePrepress( &new_me, new_selects );
  }
}

void PageView::keyPress ( QKeyEvent* ke )
{
  if ( input_object_ != 0 )
    input_object_->keyPress( ke );
  else
    ke->ignore();
}

/*
 * When a figure's construction is complete, let the user interface know, too.
 * Probably should take an argument noting whether the change was
 * accepted by the user.
 */
void PageView::figureComplete ( void )
{
  parent_->modelChanged( true );

  emit newInformation( trC( lC::STR::NONE ) );
}

/*
 * Works, but probably not the best solution.
 */
void PageView::figureModified ( void )
{
  parent_->modelChanged( true );
}

// Make sure the name is unique.

lC::RenameStatus PageView::uniqueFigureName ( const FigureViewBase* figure_view,
					      const QString& name,
					      const QString& type ) const
{
  QPtrListIterator<FigureViewBase> fv( figure_views_ );

  for ( ; fv.current() != 0; ++fv ) {
    // Skip one's self.
    if ( fv.current() == figure_view ) continue;

    if ( lC::formatName( fv.current()->name() ) == name &&
	 fv.current()->type() == type ) {
      QMessageBox mb( trC( lC::STR::LIGNUMCAD ),
		      tr( "The name \"%1\" for a figure of type %2 already exists." ).
		      arg( name ).arg( trC( type ) ),
		      QMessageBox::Warning,
		      QMessageBox::Yes | QMessageBox::Default,
		      QMessageBox::Cancel,
		      QMessageBox::NoButton );
      mb.setButtonText( QMessageBox::Yes, tr( "Enter another name" ) );
      mb.setButtonText( QMessageBox::Cancel, tr( "Cancel figure edit" ) );

      switch ( mb.exec() ) {
      case QMessageBox::Yes:
	return lC::Redo;
      case QMessageBox::Cancel:
	return lC::Rejected;
      }
      break;
    }
  }

  return lC::OK;
}

// Generate a unique name for the given type.

QString PageView::uniqueName ( QString (*newName)( void ), const QString& type )
{
  QString name = newName();

 redo:
  QPtrListIterator<FigureViewBase> fv( figure_views_ );

  for ( ; fv.current() != 0; ++fv ) {
    if ( fv.current()->name() == name && fv.current()->type() == type ) {
      name = newName();
      goto redo;
    }
  }

  return name;
}
