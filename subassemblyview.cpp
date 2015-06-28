/*
 * subassemblyview.cpp
 *
 * SubassemblyView class: a view of a Subassembly.
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
#include <qdom.h>

#include "constants.h"
#include "units.h"
#include "command.h"
#include "listviewitem.h"
#include "model.h"
#include "assembly.h"
#include "part.h"
#include "openglview.h"
#include "designbookview.h"
#include "assemblyview.h"
#include "ocsolid.h"
#include "ocsoliddraw.h"
#include "dimension.h"
#include "pageview.h"
#include "lcdefaultlengthconstraint.h"
#include "lignumcadmainwindow.h"
#include "offsetinfodialog.h"
#include "subassembly.h"
#include "ocsubassemblydraw.h"
#include "subassemblyview.h"

#include "lcdebug.h"

/*!
 * SubassemblyViewCreate is a memento of the current state of a Subassembly
 * and its view. It handles creating and deleting both objects.
 */
class SubassemblyViewCreate : public CreateObject {
  //! When CommandHistory undoes or redoes the creation of this object,
  //! the Design Book view is the only handle which is required. Everything
  //! else can be looked up.
  DesignBookView* design_book_view_;
  //! The details of the assembly and view are stored as XML.
  QDomDocument xml_doc_;
public:
  /*!
   * Create a memento of the current state of the subassembly and its view.
   * \param subassembly_view view object of the subassembly.
   */
  SubassemblyViewCreate ( SubassemblyView* subassembly_view )
  {
    design_book_view_ = subassembly_view->parent()->parent();

    QDomElement root = xml_doc_.createElement( lC::STR::MEMENTO );

    root.setAttribute( lC::STR::NAME, subassembly_view->dbURL().toString(true) );

    subassembly_view->subassembly()->write( root );
    subassembly_view->write( root );

    xml_doc_.appendChild( root );
  }
  //! Destructor does not do much.
  ~SubassemblyViewCreate ( void ) {}
  /*!
   * (Re)create the assembly object and its view as specified in
   * the XML representation.
   */
  void create ( void )
  {
    QDomNodeList memento_list = xml_doc_.elementsByTagName( lC::STR::MEMENTO );

    if ( memento_list.length() > 0 ) {
      DBURL url = memento_list.item(0).toElement().attribute( lC::STR::NAME );

      // First, we have to find the parent page and view for the subassembly.

      DBURL parent = url.parent();

      Space3D::Page* page =
	dynamic_cast<Space3D::Page*>( design_book_view_->model()->lookup( parent ));
      AssemblyView* assembly_view =
	dynamic_cast<AssemblyView*>( design_book_view_->lookup( parent ) );

      // Now, we can extract the XML representations for the subassembly
      // and its view
      QDomNodeList subassembly_list =
	xml_doc_.elementsByTagName( lC::STR::SUBASSEMBLY );

      if ( subassembly_list.length() > 0 ) {

	QDomNodeList subassembly_view_list =
	  xml_doc_.elementsByTagName( lC::STR::SUBASSEMBLY_VIEW );

	if ( subassembly_view_list.length() > 0 ) {
	  uint subassembly_id =
	    subassembly_list.item(0).toElement().attribute( lC::STR::ID ).toUInt();

	  Subassembly* subassembly =
	    new Subassembly( subassembly_id, subassembly_list.item(0).toElement(),
			     page );

	  SubassemblyView* subassembly_view =
	    new SubassemblyView( subassembly_view_list.item(0).
				 toElement(), assembly_view );
	  assembly_view->addFigureView( subassembly_view );

	  // Highly experimental...

	  // Move yourself a little bit just in case.
	  if ( subassembly->constraints().constraints().count() == 0 )
	    subassembly->translate( gp_Vec( 20, 0, 0 ) );
	  
	  if ( subassembly->constraints().constraints().count() < 3 )
	    assembly_view->editConstraints( subassembly_view );
	}
      }
    }
  }
  /*!
   * Delete the assembly object and its view as specified in the
   * XML representation.
   */
  void remove ( void )
  {
    QDomNodeList memento_list = xml_doc_.elementsByTagName( lC::STR::MEMENTO );

    if ( memento_list.length() > 0 ) {
      DBURL url = memento_list.item(0).toElement().attribute( lC::STR::NAME );

      SubassemblyView* subassembly_view =
	dynamic_cast<SubassemblyView*>( design_book_view_->lookup( url ) );

      DBURL parent = url.parent();

      PageView* page_view =
	dynamic_cast<PageView*>( design_book_view_->lookup( parent ) );

      page_view->removeFigureView( subassembly_view );
    }
  }
  /*!
   * \return the XML representation.
   */
  QDomDocument* representation ( void ) { return &xml_doc_; }
  /*!
   * Append the contents of the XML representation to the argument XML
   * document.
   * \param xml_rep CommandHistory's global XML document.
   */
  void write ( QDomDocument* xml_rep )
  {
    QDomElement rep_root = xml_rep->firstChild().toElement();
    QDomElement doc_root = xml_doc_.firstChild().toElement();

    rep_root.setAttribute( lC::STR::NAME, doc_root.attribute( lC::STR::NAME ) );

    QDomNodeList subassembly_list =
      xml_doc_.elementsByTagName( lC::STR::SUBASSEMBLY );

    if ( subassembly_list.length() > 0 ) {
      QDomNode element = xml_rep->importNode( subassembly_list.item(0), true );
      rep_root.appendChild( element );
    }

    QDomNodeList subassembly_view_list =
      xml_doc_.elementsByTagName( lC::STR::SUBASSEMBLY_VIEW );

    if ( subassembly_view_list.length() > 0 ) {
      QDomNode element = xml_rep->importNode( subassembly_view_list.item(0), true );
      rep_root.appendChild( element );
    }
  }
  /*!
   * If the creation of a assembly is immediately followed by a rename,
   * merge this into the representation rather than having a separate
   * undo step.
   * \param rename the rename command.
   * \return true if the rename command applied to this object.
   */
  bool mergeRename ( RenameCommand* rename )
  {
    // First, make sure the rename refers to this object. Start by extracting
    // the old path (which is stored in the view element).
    QDomNodeList memento_list = xml_doc_.elementsByTagName( lC::STR::MEMENTO );

    if ( memento_list.length() > 0 ) {
      QString path = memento_list.item(0).toElement().attribute( lC::STR::NAME );

      if ( path != rename->oldDBURL().toString(true) )
	return false;

      // Additional sanity check: make sure the object and its view have elements.

      QDomNodeList assembly_list = xml_doc_.elementsByTagName( lC::STR::ASSEMBLY );
      QDomNodeList assembly_view_list =
	xml_doc_.elementsByTagName( lC::STR::ASSEMBLY_VIEW );

      if ( assembly_list.length() == 0 || assembly_view_list.length() == 0 )
	return false;

      // Update the name elements in the object and it's view.

      memento_list.item(0).toElement().setAttribute( lC::STR::NAME,
					     rename->newDBURL().toString(true) );

      assembly_list.item(0).toElement().
	setAttribute( lC::STR::NAME, rename->newDBURL().name() );

      assembly_view_list.item(0).toElement().setAttribute( lC::STR::ASSEMBLY,
					   rename->newDBURL().toString(true) );

      return true;
    }

    return false;
  }
};

/*!
 * Internal class that handles the undo/redo command for modifying the offset
 * of a constraint.
 */
class ChangeOffsetCommand : public Command {
public:
  /*!
   * Construct a command to undo/redo an offset modification on a subassembly
   * constraint.
   * \param name name of command.
   * \param design_book_view pointer for looking up other pointers.
   * \param subassembly subassembly with changed offset constraint.
   * \param phase constraint phase changed
   * \param old_offset the old offset.
   * \param new_offset the new offset.
   */
  ChangeOffsetCommand::ChangeOffsetCommand ( const QString& name,
					     DesignBookView* design_book_view,
					     Subassembly* subassembly,
					     int phase, double old_offset,
					     double new_offset )
    : Command( name ), design_book_view_( design_book_view ),
      db_url_( subassembly->dbURL() ), phase_( phase ), old_offset_( old_offset ),
      new_offset_( new_offset )
  {}
  //! Delete the move annotation undo/redo command.
  ~ChangeOffsetCommand ( void )
  {}
  //! \return command type.
  Type type ( void ) const { return ChangeConstraintOffset; }
  //! \return if possible, merge the argument command into this command.
  bool merge ( Command* /*command*/ ) { return false; }
  //! (Re)execute the command (i.e., redo).
  void execute ( void )
  {
    Model* model = design_book_view_->model();
    Subassembly* subassembly = dynamic_cast<Subassembly*>( model->lookup( db_url_));
    subassembly->constraints().setOffset( phase_, new_offset_ );
  }
  //! Unexecute the command (i.e., undo).
  void unexecute ( void )
  {
    Model* model = design_book_view_->model();
    Subassembly* subassembly = dynamic_cast<Subassembly*>( model->lookup( db_url_));
    subassembly->constraints().setOffset( phase_, old_offset_ );
  }
  /*!
   * Serialize this command.
   * \param document XML representation to append this command to.
   */
  void write ( QDomDocument* document ) const
  {
    QDomElement change_offset_element =
      document->createElement( lC::STR::CHANGE_OFFSET );
    QDomNode root_node = document->firstChild();
    if ( !root_node.isNull() )
      document->replaceChild( change_offset_element, root_node );
    else
      document->appendChild( change_offset_element );

    change_offset_element.setAttribute( lC::STR::NAME, db_url_.toString(true) );
    change_offset_element.setAttribute( lC::STR::PHASE, phase_ );
    change_offset_element.setAttribute( lC::STR::OLD_OFFSET, old_offset_ );
    change_offset_element.setAttribute( lC::STR::NEW_OFFSET, new_offset_ );
  }

private:
  //! Reference to Design Book view.
  DesignBookView* design_book_view_;
  //! Reference of subassembly.
  DBURL db_url_;
  //! Phase of changed constraint.
  int phase_;
  //! Old offset.
  double old_offset_;
  //! New offset.
  double new_offset_;
};

/*!
 * So far, just a very simple IO to keep the program from crashing.
 */
class SubassemblyModifyInput : public InputObject {
  Q_OBJECT
public:
  /*!
   * Currently, all this does is allow editing of offset constraint sizes.
   * \param subassembly_view the view of the selected subassembly.
   */
  SubassemblyModifyInput ( SubassemblyView* subassembly_view )
    : subassembly_view_( subassembly_view )
  {}

  // Implemention of InputObject interface

  //! \return the current selection type (always PICK, but the filter
  //! OpenGL name changes).
  SelectionType selectionType ( void ) const
  {
    return selection_type_;
  }
  //! \return true does need selection scan.
  bool needsPrepressMouseCoordinates ( void ) const { return true; }
  /*!
   * Add whatever entries this class wants to have in the OpenGLView's
   * context menu. Also do any other UI tweaks, such as a custom cursor.
   *
   * \param context_menu main OpenGL view's context menu.
   */
  void startDisplay ( QPopupMenu* /*context_menu*/ )
  {
  }
  /*!
   * Does nothing now, but may later. (For example, when changing the current
   * page: remove the context dialog action.)
   */
  void stopDisplay ( QPopupMenu* /*context_menu*/ )
  {
  }
  /*!
   * Respond to the mouse moving about the display. For this class, PageView
   * handles the highlighting.
   */
  void mousePrepress ( QMouseEvent* /*me*/, const SelectedNames& /*selected*/ )
  {}
  /*!
   * \param me Qt mouse event.
   * \param selected list of selected items.
   */
  void mousePress ( QMouseEvent* me, const SelectedNames& /*selected*/ )
  {
    me->ignore();
  }
  /*!
   * Since it runs in pick mode, there is nothing to drag.
   */
  void mouseDrag ( QMouseEvent* /*me*/, const SelectedNames& /*selected*/ )
  {
  }
  /*!
   * When the mouse is released, we can either keep asking for more
   * surfaces or report that the constraint is complete.
   */
  bool mouseRelease ( QMouseEvent* /*me*/, const SelectedNames& /*selected*/ )
  {
    return true;
  }
  //! If a dimension is activated, edit it.
  void mouseDoubleClick ( QMouseEvent* /*me*/ )
  {
    subassembly_view_->editInformation();
  }
  /*!
   */
  void keyPress ( QKeyEvent* /*ke*/ )
  {
  }

public slots:
  /*!
   * Remove the elements of the user interface relating to this operation
   * and alert the parent view that this operation has been canceled.
   */
  void cancelOperation ( void )
  {
  }

private:
  //! View of the selected subassembly.
  SubassemblyView* subassembly_view_;
  //! Includes selection name of subassembly and filter flag.
  SelectionType selection_type_;
};

#include ".moc/subassemblyview.moc"

OffsetInfoDialog* SubassemblyView::offset_info_dialog_ = 0;

SubassemblyView::SubassemblyView ( Subassembly* subassembly, PageView* parent )
  : FigureView( parent ), subassembly_( subassembly )
{
  init();
}

SubassemblyView::SubassemblyView ( const QDomElement& xml_rep, PageView* parent )
  : FigureView( parent ), subassembly_( 0 )
{
  DBURL db_url = xml_rep.attribute( lC::STR::SUBASSEMBLY );

  subassembly_ = dynamic_cast< Subassembly* >( model()->lookup( db_url ) );

  init();
}

SubassemblyView::~SubassemblyView ( void )
{
  emit destroyedSubassembly();

  if ( subassembly_ ) {
    subassembly_->parent()->removeFigure( subassembly_ );
  }

  delete list_view_item_;

  delete modify_input_;

  delete drawer_;

  for ( uint i = 0; i < 3; i++ )
    if ( dimensions_[i] != 0 )
      delete dimensions_[i];
}

void SubassemblyView::init ( void )
{
  QObject::setName( subassembly_->name().latin1() );

  drawer_ = Space3D::OCSubassemblyDrawFactory::drawer( subassembly_, view() );

  modify_input_ = new SubassemblyModifyInput( this );

  dimensions_[0] = dimensions_[1] = dimensions_[2] = 0;

  dimension_name_ = view()->genSelectionName();

  QListViewItem* previous_item = parent()->previousItem( parent()->listViewItem(),
							 subassembly_->id() );

  list_view_item_ = new ListViewItem( parent()->listViewItem(), previous_item );

  list_view_item_->setText( lC::NAME, lC::formatName( subassembly_->name() )
			    + QString( " <%1>" ).arg( subassembly_->id() ) );
  list_view_item_->setText( lC::TYPE, trC( subassembly_->type() ) );
  list_view_item_->setText( lC::DETAIL, tr( "Model: %1.%2 <%3>" ).
		    arg( lC::formatName( subassembly_->subassembly()->name()) ).
		    arg( trC( subassembly_->subassembly()->type() ) ).
		    arg( lC::idToString( subassembly_->subassembly()->ID() ) ) );
  list_view_item_->listView()->ensureItemVisible( list_view_item_ );

  QPtrListIterator<AssemblyConstraint> constraint =
    subassembly_->constraints().constraints();

  QListViewItem* constraint_item = 0;
  for ( ; constraint.current(); ++constraint ) {
    constraint_item = new ListViewItem( list_view_item_, constraint_item );
    constraint_item->setText( lC::NAME, trC( lC::STR::CONSTRAINT ) );
    constraint_item->setText( lC::TYPE, trC( constraint.current()->type() ) );
    constraint_item->setText( lC::DETAIL, QString::null );

    updateChangedConstraint( 0, constraint.current() );

    // Are there any offset constraints which need dimensions?
    if ( constraint.current()->type() == lC::STR::MATE_OFFSET ||
	 constraint.current()->type() == lC::STR::ALIGN_OFFSET )
      updateChangedOffset( constraint.current() );
  }

#if 0
  connect( subassembly_, SIGNAL( locationChanged() ), SLOT( updateLocation() ) );
  connect( subassembly_, SIGNAL( solidChanged() ), SLOT( updateTessellation() ) );
#else
  connect( subassembly_, SIGNAL( locationChanged() ), SLOT( updateTessellation() ) );
#endif
  connect( subassembly_, SIGNAL( materialChanged() ), SLOT( updateMaterial() ) );

  connect( subassembly_, SIGNAL( constraintCreated( const AssemblyConstraint*) ),
	   SLOT( updateNewConstraint( const AssemblyConstraint* ) ) );
  connect( subassembly_, SIGNAL( constraintChanged( const AssemblyConstraint*,
						    const AssemblyConstraint*) ),
	   SLOT( updateChangedConstraint( const AssemblyConstraint*,
					  const AssemblyConstraint* ) ) );
  connect( subassembly_, SIGNAL( constraintOffsetChanged( const AssemblyConstraint*) ),
	   SLOT( updateChangedOffset( const AssemblyConstraint* ) ) );

 connect( subassembly_, SIGNAL( constraintCanceled() ),
	   SLOT( updateCanceledConstraint() ) );

  connect( subassembly_->subassembly(), SIGNAL( nameChanged(const QString&) ),
	   SLOT( updateModelName( const QString& ) ) );

  connect( parent(), SIGNAL( orientationChanged( const GLdouble* ) ),
	   SLOT( updateViewNormal( const GLdouble* ) ) );

  if ( offset_info_dialog_ == 0 )
    offset_info_dialog_ = new OffsetInfoDialog( parent()->lCMW() );
}

CreateObject* SubassemblyView::memento ( void )
{
  return new SubassemblyViewCreate( this );
}

void SubassemblyView::viewAttributeChanged ( void )
{
  for ( uint i = 0; i < 3; i++ )
    if ( dimensions_[i] != 0 ) {
      AssemblyConstraint* constraint = subassembly_->constraints().constraint( i );
      dimensions_[i]->setEndsNormal( constraint->end0(), constraint->end1(),
				     constraint->normal(), view() );
    }
}

void SubassemblyView::draw ( void ) const
{
  lC::Render::Mode mode = lC::Render::REGULAR;

  if ( isActivated() )
    mode = lC::Render::ACTIVATED;
  else if ( isHighlighted() )
    mode = lC::Render::HIGHLIGHTED;

  drawer_->draw( parent()->renderStyle(), parent()->selectionType().entity_, mode );

  // If activated and we have any dimensioned constraints, draw the dimensions

  if ( isActivated() )
    for ( uint i = 0; i < 3; i++ )
      if ( dimensions_[i] != 0 )
	dimensions_[i]->draw();
}

void SubassemblyView::select ( SelectionType /*select_type*/ ) const
{
  glLoadName( selectionName() );

  drawer_->select( parent()->selectionType().entity_, lC::Render::REGULAR );

  if ( isActivated() ) {
    glPushName( dimension_name_ );
    for ( uint i = 0; i < 3; i++ )
      if ( dimensions_[i] != 0 ) {
	glPushName( i );
	dimensions_[i]->select();
	glPopName();
      }
    glPopName();
  }
}

// Implementation of FigureView interface
uint SubassemblyView::id ( void ) const
{
  return subassembly_->id();
}

QString SubassemblyView::name ( void ) const
{
  return subassembly_->name();
}

DBURL SubassemblyView::dbURL ( void ) const
{
  return subassembly_->dbURL();
}

QString SubassemblyView::selectionText ( const vector<GLuint>& selection_name,
					 SelectionEntity entity ) const
{
  QString text;

  switch ( entity ) {
  case FIGURE:
    text = subassembly_->subassembly()->name(); break;
  case FACE:
    text = drawer_->selectionText( selection_name, entity );
  }

  return text;
}

QString SubassemblyView::type ( void ) const
{
  return lC::STR::SUBASSEMBLY;
}

InputObject* SubassemblyView::modifyInput ( void )
{
  return modify_input_;
}

// This would seem to be an implicit look up of the dimension view.
// Is there anything else this could be asked about?
View* SubassemblyView::lookup ( QStringList& /*path_components*/ ) const
{
  return 0;
}

QString SubassemblyView::geometry ( const vector<GLuint>& selection_names ) const
{
  QString name = lC::STR::PATH_PATTERN.arg( parent()->dbURL() ).
    arg( drawer_->geometry( selection_names ) );

  return name;
}

QValueVector<uint> SubassemblyView::geomPath ( const vector<GLuint>& selection_names ) const
{
  QValueVector<uint> id_path = parent()->ID();
  QValueVector<uint> subcomponent_path = drawer_->ID( selection_names );

  uint id_path_size = id_path.size();

  id_path.resize( id_path_size + subcomponent_path.size() );
  for ( uint i = 0; i < subcomponent_path.size(); ++i )
    id_path[ id_path_size + i ] = subcomponent_path[i];

  return id_path;
}

// Essentially the reciprocal of the above function. Find the gl selection
// path corresponding to the id path.

void SubassemblyView::lookup ( QValueVector<uint>& id_path,
			       vector<GLuint>& name_path ) const
{
  drawer_->lookup( id_path, name_path );
}

void SubassemblyView::setHighlighted( bool highlight, SelectionEntity entity,
				      const vector<GLuint>& items )
{
  if ( entity == FACE ) {
    if ( items.size() > 1 ) {
      drawer_->setHighlighted( highlight, entity, items );
    }
  }
  else if ( entity == FIGURE ) {

    if ( items.size() > 2 && items[1] == dimension_name_ ) {
      if ( dimensions_[items[2]]->mode() != lC::Render::ACTIVATED ) {
	if ( highlight )
	  dimensions_[ items[2] ]->setMode( lC::Render::HIGHLIGHTED );
	else
	  dimensions_[ items[2] ]->setMode( lC::Render::REGULAR );
      }
    }

    FigureViewBase::setHighlighted( highlight );
  }
}

void SubassemblyView::setActivated( bool activate, SelectionEntity entity,
				    const vector<GLuint>& items )
{
  if ( entity == FACE ) {
    if ( items.size() > 1 ) {
      drawer_->setActivated( activate, entity, items );
    }
  }
  else if ( entity == FIGURE ) {
    for ( uint i = 0; i < 3; ++i ) {
      if ( dimensions_[i] != 0 ) {
	if ( activate )
	  dimensions_[i]->setMode( lC::Render::REGULAR );
	else
	  dimensions_[i]->setMode( lC::Render::INVISIBLE );
      }
    }
    if ( items.size() > 2 && items[1] == dimension_name_ ) {
      if ( activate )
	dimensions_[ items[2] ]->setMode( lC::Render::ACTIVATED );
      else
	dimensions_[ items[2] ]->setMode( lC::Render::REGULAR );
    }

    FigureViewBase::setActivated( activate );
  }
}

void SubassemblyView::updateModelName ( const QString& /*name*/ )
{
  list_view_item_->setText( lC::DETAIL, tr( "Model: %1.%2 <%3>" ).
		    arg( lC::formatName( subassembly_->subassembly()->name()) ).
		    arg( trC( subassembly_->subassembly()->type() ) ).
		    arg( lC::idToString( subassembly_->subassembly()->ID() ) ) );
}

void SubassemblyView::updateLocation ( void )
{
  //  drawer_->setTransform( subassembly_->location() );
}

void SubassemblyView::updateMaterial ( void )
{
  drawer_->updateMaterial();
}

void SubassemblyView::updateTessellation ( void )
{
  //  cout << now() << name() << " updating tessellation view [before]" << endl;
  drawer_->update();
  //  cout << now() << name() << " updating tessellation view [after]" << endl;
}

void SubassemblyView::updateViewNormal ( const GLdouble* modelview )
{
  drawer_->updateViewNormal();
  for ( uint i = 0; i < 3; ++i )
    if ( dimensions_[i] != 0 )
      dimensions_[i]->
	setViewNormal( Space3D::Vector( modelview[2], modelview[6], modelview[10]));
}

void SubassemblyView::updateNewConstraint ( const AssemblyConstraint* constraint )
{
  QListViewItem* last_item = list_view_item_->firstChild();
  if ( last_item != 0 )
    for ( ; last_item->nextSibling() != 0; last_item = last_item->nextSibling() );

  ListViewItem* constraint_item = new ListViewItem( list_view_item_, last_item );
  constraint_item->setText( lC::NAME, trC( lC::STR::CONSTRAINT ) );
  constraint_item->setText( lC::TYPE, trC( constraint->type() ) );
  constraint_item->setText( lC::DETAIL, QString::null );
  constraint_item->setOpen( true );
  constraint_item->listView()->ensureItemVisible( constraint_item );
}

void SubassemblyView::updateChangedConstraint (
				       const AssemblyConstraint* /*old_constraint*/,
				       const AssemblyConstraint* new_constraint )
{
  QListViewItem* last_item = list_view_item_->firstChild();
  if ( last_item != 0 )
    for ( ; last_item->nextSibling() != 0; last_item = last_item->nextSibling() );

  QString text;

  if ( new_constraint->type() == lC::STR::MATE_OFFSET ||
       new_constraint->type() == lC::STR::ALIGN_OFFSET )
    text = tr( "Offset %1: " ).
      arg( UnitsBasis::instance()->format( new_constraint->offset(),false));

  if ( new_constraint->reference1().empty() ) {
    if ( !new_constraint->reference0().empty() ) {
      text += lC::formatName( model()->idPath( new_constraint->reference0() ) );

      if ( ( new_constraint->type() == lC::STR::MATE_OFFSET ||
	   new_constraint->type() == lC::STR::ALIGN_OFFSET ) &&
	   dimensions_[ new_constraint->phase() ] != 0 ) {
	delete dimensions_[new_constraint->phase()];
	dimensions_[new_constraint->phase()] = 0;
      }

      addDependency( new_constraint->reference0() );
    }
  }
  else {
    text += tr( "%1 to %2" ).
      arg( lC::formatName( model()->idPath( new_constraint->reference0() ) ) ).
      arg( lC::formatName( model()->idPath( new_constraint->reference1() ) ) );

    if ( new_constraint->type() == lC::STR::MATE_OFFSET ||
	 new_constraint->type() == lC::STR::ALIGN_OFFSET )
      updateChangedOffset( new_constraint );

    addDependency( new_constraint->reference0() );
    addDependency( new_constraint->reference1() );
  }
  last_item->setText( lC::DETAIL, text );
}

void SubassemblyView::updateChangedOffset ( const AssemblyConstraint* constraint )
{
  if ( dimensions_[ constraint->phase() ] != 0 ) {
    dimensions_[ constraint->phase() ]->
      setEndsNormal( constraint->end0(), constraint->end1(),
		     constraint->normal(), view() );
  }
  else {
    dimensions_[ constraint->phase() ]
      = new Space3D::Dimension( constraint->end0(), constraint->end1(),
				constraint->normal(), view() );
    dimensions_[ constraint->phase() ]->setMode( lC::Render::REGULAR );
  }

  QListViewItem* last_item = list_view_item_->firstChild();
  if ( last_item != 0 )
    for ( int phase = 0; phase < constraint->phase(); phase++ )
      last_item = last_item->nextSibling();

  QString text = tr( "Offset %1: %2 to %3" ).
    arg( UnitsBasis::instance()->format( constraint->offset(), false ) ).
    arg( lC::formatName( model()->idPath( constraint->reference0()  ) ) ).
    arg( lC::formatName( model()->idPath( constraint->reference1() ) ) );


  last_item->setText( lC::DETAIL, text );
}

void SubassemblyView::updateCanceledConstraint ( void )
{
  QListViewItem* last_item = list_view_item_->firstChild();
  int phase = 0;

  if ( last_item != 0 ) {
    for ( ; last_item->nextSibling() != 0;
	  last_item = last_item->nextSibling(), ++phase );
  }

  if ( last_item != 0 )
    delete last_item;

  if ( dimensions_[phase] != 0 ) {
    delete dimensions_[phase];
    dimensions_[phase] = 0;
  }
}

void SubassemblyView::addDependency ( const QValueVector<uint>& surface_id )
{
  // Basically, the idea here is to connect to each component of the
  // references path whose name change could affect the display.  So,
  // exhustively examine each element of the path and see what it
  // means to us. Note, since part faces are not real ModelItems, we
  // skip the last element of the id_path.
  QValueVector<uint> id_path;
  id_path.reserve( surface_id.size()-1 );

  for ( uint i = 0; i < surface_id.size()-1; ++i ) {
    id_path.push_back( surface_id[i] );
    ModelItem* item = model()->lookup( id_path );
    // The first order check is that we don't already depend on this item.
    if ( dependencies_.findRef( item ) != -1 ) {
      continue;
    }
    // If it's a model, part or assembly, then it can be renamed by the user.
    else if ( dynamic_cast<Model*>( item ) != 0 ||
	      dynamic_cast<Assembly*>( item ) != 0 ||
	      dynamic_cast<Part*>( item ) != 0 ) {
      dependencies_.append( item );
      connect( item, SIGNAL( nameChanged(const QString&) ),
	       SLOT( updateConstraintName( const QString&) ) );
    }
  }
}

void SubassemblyView::updateConstraintName ( const QString& /*name*/ )
{
  // The budget approach is just to recompute all of the list view strings.
  // Not sure what being more selective would avail us.
  QListViewItem* list_item = list_view_item_->firstChild();

  QPtrListIterator<AssemblyConstraint> constraint( subassembly_->constraints().
						   constraints() );

  for ( ; constraint.current() != 0; ++constraint ) {
    QString text;
    if ( constraint.current()->type() == lC::STR::MATE_OFFSET ||
	 constraint.current()->type() == lC::STR::ALIGN_OFFSET )
      text = tr( "Offset %1: " ).
	arg( UnitsBasis::instance()->format( constraint.current()->offset(),false));

    if ( constraint.current()->reference1().empty() ) {
      text += lC::formatName( model()->idPath( constraint.current()->reference0()));
    }
    else {
      text += tr( "%1 to %2" ).
	arg( lC::formatName( model()->idPath( constraint.current()->reference0()))).
	arg( lC::formatName( model()->idPath( constraint.current()->reference1())));
    }

    list_item->setText( lC::DETAIL, text );

    list_item = list_item->nextSibling();
  }
}

void SubassemblyView::write ( QDomElement& xml_rep ) const
{
  QDomDocument document = xml_rep.ownerDocument();
  QDomElement subassembly_view_element =
    document.createElement( lC::STR::SUBASSEMBLY_VIEW );
  subassembly_view_element.setAttribute(lC::STR::SUBASSEMBLY,
					subassembly_->dbURL().toString(true) );

  xml_rep.appendChild( subassembly_view_element );
}

void SubassemblyView::editInformation ( void )
{
  for ( uint i = 0; i < 3; i++ ) {
    if ( dimensions_[i] != 0 && dimensions_[i]->mode() == lC::Render::ACTIVATED ) {

      QString type = subassembly_->constraints().constraint( i )->type();
      double old_offset = subassembly_->constraints().constraint( i )->offset();

      offset_info_dialog_->offsetLengthConstraint->setTitle( trC( type ) );
      offset_info_dialog_->offsetLengthConstraint->
	setLengthLimits( UnitsBasis::instance()->lengthUnit(),
			 UnitsBasis::instance()->format(),
			 UnitsBasis::instance()->precision(),
			 lC::MINIMUM_DIMESION, lC::MAXIMUM_DIMENSION,
			 old_offset );

      int ret = offset_info_dialog_->exec();

      if ( ret != QDialog::Rejected ) {
	double offset = offset_info_dialog_->offsetLengthConstraint->specifiedLength();

	subassembly_->constraints().setOffset( i, offset );

	CommandHistory::instance().
	  addCommand( new ChangeOffsetCommand( "change offset", parent()->parent(),
					       subassembly(),
					       i, old_offset, offset ) );
      }

      break;
    }
  }
}
