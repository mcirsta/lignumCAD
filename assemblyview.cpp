/*
 * assemblyview.cpp
 *
 * AssemblyView class: a view of a Assembly.
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
#include <QMenu>
#include <qaction.h>
#include <qtabbar.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <QDockWidget>
#include <qlayout.h>
#include <qwhatsthis.h>
#include <qtextbrowser.h>
#include <qbuttongroup.h>
#include <qlabel.h>

#include <BRepTools.hxx>
#include <TopoDS_Compound.hxx>

#include "constants.h"
#include "units.h"
#include "cursorfactory.h"
#include "dimension.h"
#include "command.h"
#include "model.h"
#include "part.h"
#include "openglview.h"
#include "lignumcadmainwindow.h"
#include "designbookview.h"
#include "listviewitem.h"
#include "ocsolid.h"
#include "ochiddendraw.h"
#include "offsetinfodialog.h"
#include "lcdefaultlengthconstraint.h"
#include "pagefactory.h"
#include "subassembly.h"
#include "subassemblyview.h"
#include "assemblyconfigdialog.h"
#include "assemblyadddialog.h"
#include "assemblyconstraintform.h"
#include "assemblyview.h"

#include "lcdebug.h"

#include <QMouseEvent>

/*!
 * AssemblyViewCreate is a memento of the current state of a Assembly and its view.
 * It handles creating and deleting both objects.
 */
class AssemblyViewCreate : public CreateObject {
  //! When CommandHistory undoes or redoes the creation of this object,
  //! the Design Book view is the only handle which is required. Everything
  //! else can be looked up.
  DesignBookView* design_book_view_;
  //! The details of the assembly and view are stored as XML.
  QDomDocument xml_doc_;
public:
  /*!
   * Create a memento of the current state of the assembly and its view.
   * \param assembly_view view object of the assembly.
   */
  AssemblyViewCreate ( AssemblyView* assembly_view )
  {
    design_book_view_ = assembly_view->parent();

    QDomElement root = xml_doc_.createElement( lC::STR::MEMENTO );

    root.setAttribute( lC::STR::NAME, assembly_view->dbURL().toString() );

    assembly_view->assembly()->write( root );
    assembly_view->write( root );

    xml_doc_.appendChild( root );
  }
  //! Destructor does not do much.
  ~AssemblyViewCreate ( void ) {}
  /*!
   * (Re)create the assembly object and its view as specified in
   * the XML representation.
   */
  void create ( void )
  {
    QDomNodeList assembly_list = xml_doc_.elementsByTagName( lC::STR::ASSEMBLY );

    if ( assembly_list.length() > 0 ) {

      QDomNodeList assembly_view_list =
	xml_doc_.elementsByTagName( lC::STR::ASSEMBLY_VIEW );

      if ( assembly_view_list.length() > 0 ) {
	uint assembly_id =
	  assembly_list.item(0).toElement().attribute( lC::STR::ID ).toUInt();

	new Assembly( assembly_id, assembly_list.item(0).toElement(),
		      design_book_view_->model() );

	AssemblyView* assembly_view =
	  new AssemblyView( design_book_view_, assembly_view_list.item(0).toElement() );

	design_book_view_->addPageView( assembly_view );
	design_book_view_->showPageView( assembly_view );
      }
    }
  }
  /*!
   * Delete the assembly object and its view as specified in the
   * XML representation.
   */
  void remove ( void )
  {
    QDomNodeList assembly_view_list =xml_doc_.elementsByTagName(lC::STR::ASSEMBLY_VIEW);

    if ( assembly_view_list.length() > 0 ) {

      QDomElement assembly_view_element = assembly_view_list.item(0).toElement();

      QString assembly_path = assembly_view_element.attribute( lC::STR::ASSEMBLY );

      AssemblyView* assembly_view =
	dynamic_cast<AssemblyView*>( design_book_view_->lookup( assembly_path ) );

      design_book_view_->deletePage( assembly_view );
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

    QDomNodeList assembly_list = xml_doc_.elementsByTagName( lC::STR::ASSEMBLY );

    if ( assembly_list.length() > 0 ) {

      QDomNode element = xml_rep->importNode( assembly_list.item(0), true );
      rep_root.appendChild( element );
    }

    QDomNodeList assembly_view_list =
      xml_doc_.elementsByTagName( lC::STR::ASSEMBLY_VIEW );

    if ( assembly_view_list.length() > 0 ) {

      QDomNode element = xml_rep->importNode( assembly_view_list.item(0), true );
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

      if ( path != rename->oldDBURL().toString() )
	return false;

      // Additional sanity check: make sure the object and its view have elements.

      QDomNodeList assembly_list = xml_doc_.elementsByTagName( lC::STR::ASSEMBLY );
      QDomNodeList assembly_view_list =
	xml_doc_.elementsByTagName( lC::STR::ASSEMBLY_VIEW );

      if ( assembly_list.length() == 0 || assembly_view_list.length() == 0 )
	return false;

      // Update the name elements in the object and it's view.

      memento_list.item(0).toElement().setAttribute( lC::STR::NAME,
                         rename->newDBURL().toString() );

      assembly_list.item(0).toElement().
	setAttribute( lC::STR::NAME, rename->newDBURL().name() );

      assembly_view_list.item(0).toElement().setAttribute( lC::STR::ASSEMBLY,
                       rename->newDBURL().toString() );

      // Have to update the subassembly views as well!

      QDomNodeList subassembly_view_list =
	xml_doc_.elementsByTagName( lC::STR::SUBASSEMBLY_VIEW );

      for ( int i = 0; i < subassembly_view_list.length(); i++ ) {
	DBURL old_db_url =
	  subassembly_view_list.item(i).toElement().attribute(lC::STR::SUBASSEMBLY);
    DBURL new_db_url( rename->newDBURL().toString(),
			  old_db_url.name() + '.' + old_db_url.type() );

	subassembly_view_list.item(i).toElement().
      setAttribute( lC::STR::SUBASSEMBLY, new_db_url.toString( ) );
      }

      return true;
    }

    return false;
  }
};

/*!
 * I don't know. Try to record the process of making assembly constraints
 * so that they can be undone.
 */
class CreateConstraintCommand : public Command {
public:
  CreateConstraintCommand ( const QString& name, DesignBookView* design_book_view,
			    Subassembly* subassembly )
    : Command( name ), design_book_view_( design_book_view ),
      db_url_( subassembly->dbURL() )
  {
    QListIterator< std::shared_ptr<AssemblyConstraint>> constraints =
      subassembly->constraints().constraints();

    constraints.toBack();
//TODO check if this is correct
    if ( constraints.hasPrevious() ) {
      QDomElement constraint_element =
	xml_doc_.createElement( lC::STR::CREATE_CONSTRAINT);

      constraints.previous()->write( constraint_element );

      xml_doc_.appendChild( constraint_element );
    }
  }
  //! \return the type of this command (create [an assembly] constraint)
  Type type ( void ) const { return CreateConstraint; }
  /*!
   * Attempt to merge the argument command with this command so that
   * they are undone together. I think these will have to stand alone, however.
   * \param command command to attempt to merge.
   * \return true if command was merged.
   */
  bool merge ( Command* /*command*/ ) { return false; }
  /*!
   * Perform the constraint given in this command (i.e., redo constraint).
   */
  void execute ( void )
  {
    Subassembly* subassembly =
      dynamic_cast<Subassembly*>( design_book_view_->model()->lookup( db_url_ ) );

    QDomElement constraint_element = xml_doc_.firstChild().firstChild().toElement();

    subassembly->constraints().
      updateConstraint( xml_doc_.firstChild().firstChild().toElement() );
  }
  /*!
   * Restore the subassembly to its previous state (i.e. undo constraint).
   */
  void unexecute ( void )
  {
    Subassembly* subassembly =
      dynamic_cast<Subassembly*>( design_book_view_->model()->lookup( db_url_ ) );

    QDomElement constraint_element = xml_doc_.firstChild().firstChild().toElement();

    if ( constraint_element.attribute( lC::STR::SURFACE1 ).isEmpty() ) {
      if ( constraint_element.attribute( lC::STR::SURFACE0 ).isEmpty() )
	subassembly->constraints().cancelCurrent();
      else
	subassembly->constraints().removeFirstReference();
    }
    else
      subassembly->constraints().removeLastReference();
  }
  /*!
   * Write the operation to the dribble file in its XML representation.
   * \param document document to append XML representation to.
   */
  void write ( QDomDocument* xml_rep ) const
  {
    QDomElement constraint_element =
      xml_rep->importNode( xml_doc_.firstChild(), true ).toElement();

    constraint_element.setAttribute( lC::STR::SUBASSEMBLY, db_url_.toString() );

    QDomNode root_node = xml_rep->firstChild();

    if ( !root_node.isNull() )
      xml_rep->replaceChild( constraint_element, root_node );
    else
      xml_rep->appendChild( constraint_element );
  }

private:
  //! Reference to Design Book view for looking up items.
  DesignBookView* design_book_view_;
  //! Path to subassembly.
  DBURL db_url_;
  //! XML representation of current state of constraint.
  QDomDocument xml_doc_;
};

/*!
 * I don't know. Try to record the process of deleting assembly constraints
 * so that they can be redone.
 */
class DeleteConstraintCommand : public Command {
public:
  DeleteConstraintCommand ( const QString& name, DesignBookView* design_book_view,
			    Subassembly* subassembly )
    : Command( name ), design_book_view_( design_book_view ),
      db_url_( subassembly->dbURL() )
  {
    QListIterator<std::shared_ptr<AssemblyConstraint>> constraints =
      subassembly->constraints().constraints();

    constraints.toBack();
//TODO check if correct
    if ( constraints.hasPrevious() != 0 ) {
      QDomElement constraint_element =
	xml_doc_.createElement( lC::STR::DELETE_CONSTRAINT);

      constraints.previous()->write( constraint_element );

      xml_doc_.appendChild( constraint_element );
    }
  }
  //! \return the type of this command (delete [an assembly] constraint)
  Type type ( void ) const { return DeleteConstraint; }
  /*!
   * Attempt to merge the argument command with this command so that
   * they are undone together. I think these will have to stand alone, however.
   * \param command command to attempt to merge.
   * \return true if command was merged.
   */
  bool merge ( Command* /*command*/ ) { return false; }
  /*!
   * Perform the constraint given in this command (i.e., redo constraint).
   */
  void execute ( void )
  {
    Subassembly* subassembly =
      dynamic_cast<Subassembly*>( design_book_view_->model()->lookup( db_url_ ) );

    QDomElement constraint_element = xml_doc_.firstChild().firstChild().toElement();

    if ( constraint_element.attribute( lC::STR::SURFACE1 ).isEmpty() ) {
      if ( constraint_element.attribute( lC::STR::SURFACE0 ).isEmpty() )
	subassembly->constraints().cancelCurrent();
      else
	subassembly->constraints().removeFirstReference();
    }
    else
      subassembly->constraints().removeLastReference();
  }
  /*!
   * Restore the subassembly to its previous state (i.e. undo constraint).
   */
  void unexecute ( void )
  {
    Subassembly* subassembly =
      dynamic_cast<Subassembly*>( design_book_view_->model()->lookup( db_url_ ) );

    QDomElement constraint_element = xml_doc_.firstChild().firstChild().toElement();

    subassembly->constraints().
      updateConstraint( xml_doc_.firstChild().firstChild().toElement() );
  }
  /*!
   * Write the operation to the dribble file in its XML representation.
   * \param document document to append XML representation to.
   */
  void write ( QDomDocument* xml_rep ) const
  {
    QDomElement constraint_element =
      xml_rep->importNode( xml_doc_.firstChild(), true ).toElement();

    constraint_element.setAttribute( lC::STR::SUBASSEMBLY, db_url_.toString() );

    QDomNode root_node = xml_rep->firstChild();

    if ( !root_node.isNull() )
      xml_rep->replaceChild( constraint_element, root_node );
    else
      xml_rep->appendChild( constraint_element );
  }

private:
  //! Reference to Design Book view for looking up items.
  DesignBookView* design_book_view_;
  //! Path to subassembly.
  DBURL db_url_;
  //! XML representation of current state of constraint.
  QDomDocument xml_doc_;
};

/*!
 * Take over the input duties when a constraint is being made. Seems to
 * actually be generic now (except for offset constraints).
 */
class ConstraintInput : public InputObject {
  Q_OBJECT
public:
  /*!
   * This class is embedded in an Assembly view, but C++
   * does not have interior classes so it has to have a pointer to
   * the parent.
   * \param assembly_view parent assembly view.
   */
  ConstraintInput ( AssemblyView* assembly_view )
    : assembly_view_( assembly_view ), selection_type_( PICK, FACE )
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
   * Note: This can now be invoked at any time during a constraint's
   * construction, so this method has to be more sensitive to the
   * current status of the constraint.
   *
   * \param context_menu main OpenGL view's context menu.
   */
  void startDisplay ( QMenu* context_menu )
  {
    context_menu_ = context_menu;

    QAction* cancel_action = assembly_view_->lCMW()->getUi()->cancelAssemblyConstraintAction;

    separator_id_ = context_menu_->addSeparator();
    context_menu_->addAction(cancel_action);
    connect( cancel_action, SIGNAL( activated() ), SLOT( cancelCurrent() )  );

    status_ = subassembly_->constraints().status();

    switch ( status_ ) {
    case Invalid:
      selection_type_.filter_ = INCLUSIVE;

      assembly_view_->view()->
	setCursor( CursorFactory::instance().
		   cursor( CursorFactory::ASSEMBLY_CONSTRAINT ) );
      break; 
    case OK:
      selection_type_.filter_ = EXCLUSIVE;

      assembly_view_->view()->
	setCursor( CursorFactory::instance().
		   cursor( CursorFactory::ASSEMBLY_CONSTRAINTPLUS ) );
    case ConstraintComplete:
       break;
    case PlacementComplete:
       break;
   }
  }
  /*!
   * When changing the current page remove the context dialog action and
   * undo any cursor changes.
   */
  void stopDisplay ( QMenu* /*context_menu*/ )
  {
    QAction* cancel_action = assembly_view_->lCMW()->getUi()->cancelAssemblyConstraintAction;

    cancel_action->disconnect();
    context_menu_->removeAction( cancel_action);
    context_menu_->removeAction( separator_id_ );

    assembly_view_->view()->unsetCursor();
  }
  /*!
   * Respond to the mouse moving about the display. For this class, PageView
   * handles the highlighting.
   */
  void mousePrepress ( QMouseEvent* /*me*/, const SelectedNames& /*selected*/ )
  {}
  /*!
   * This class operates in pick mode, so take the selected face and
   * make sure it is OK with the constraint.
   * \param me Qt mouse event.
   * \param selected list of selected items.
   */
  void mousePress ( QMouseEvent* me, const SelectedNames& selected )
  {
    if ( selected.empty() ) {
      me->ignore();
      status_ = Invalid;
      return;
    }

    SelectedNames::const_iterator f = selected.begin();

    if ( (*f).second.size() < 2 ) {
      me->ignore();
      status_ = Invalid;
      return;
    }

    //TODO check this works
    std::shared_ptr<FigureViewBase> tmpFig = (assembly_view_-> figureSelectionNames()[ (*f).second[0] ]);
    SubassemblyView* sv =
      dynamic_cast<SubassemblyView*>( tmpFig.get() );

    // Validate this geometry against the current constraint, but don't
    // emit the constraint changed signal until the user releases the mouse
    // button (seems to make more sense, now).

    status_ = subassembly_->constraints().validate( sv->geomPath( (*f).second ) );

    if ( status_ == Invalid ) {
      QWhatsThis::showText(QCursor::pos (), tr( "The selected face is invalid." ) );
      //      me->ignore();
    }
  }
  /*!
   * Since it runs in pick mode, there is nothing to drag.
   */
  void mouseDrag ( QMouseEvent* /*me*/, const SelectedNames& /*selected*/ ) {}
  /*!
   * When the mouse is released, we can either keep asking for more
   * surfaces or report that the constraint is complete.
   */
  bool mouseRelease ( QMouseEvent* /*me*/, const SelectedNames& /*selected*/ )
  {
    switch ( status_ ) {
    case OK:
      // Now ready to signal that the constraint has changed.
      subassembly_->constraints().apply();

      CommandHistory::instance().
	addCommand( new CreateConstraintCommand( "create constraint",
						 assembly_view_->parent(),
						 subassembly_ ) );

      selection_type_.filter_ = EXCLUSIVE;

      assembly_view_->view()->
	setCursor(CursorFactory::instance().cursor(CursorFactory::
						   ASSEMBLY_CONSTRAINTPLUS));

      return false;		// Pick another.

    case Invalid:
      return false;		// Try again.

    case ConstraintComplete:
      return false;		// Try again.
    case PlacementComplete:
      return false;		// Try again.
    }

    // The constraint (and possibly the placement) is complete. Now
    // actually perform the subassembly transformation. Signals that
    // the constraint has changed (again).

    subassembly_->constraints().transform();

    assembly_view_->figureModified();

    CommandHistory::instance().
      addCommand( new CreateConstraintCommand( "create constraint",
					       assembly_view_->parent(),
					       subassembly_ ) );

    return true;
  }
  //! Can't think of anything for this to do.
  void mouseDoubleClick ( QMouseEvent* /*me*/ ) {}
  /*!
   * The only keyboard input currently of interest is the Escape key to
   * cancel the creation of the current constraint.
   */
  void keyPress ( QKeyEvent* ke )
  {
    if ( ke->key() == Qt::Key_Escape )
      cancelCurrent();
    else
      ke->ignore();
  }
  /*!
   * This object is reused as subassemblies are created. Set the subassembly
   * which is being modified.
   * \param subassembly subassembly gaining constraint.
   * \param subassembly_gl_name used to limit surface picks to: first, the
   * subassembly; then, the remainder of the items in the assembly.
   */
  void setSubassembly ( Subassembly* subassembly, GLuint subassembly_gl_name )
  {
    subassembly_ = subassembly;

    connect( subassembly_, SIGNAL( constraintCanceled() ), SLOT(cancelOperation()));
    connect( subassembly_, SIGNAL( constraintChanged( const AssemblyConstraint*,
						      const AssemblyConstraint* ) ),
	     SLOT(constraintChanged( const AssemblyConstraint*,
				     const AssemblyConstraint* ) ) );

    selection_type_.filter_ = INCLUSIVE;
    selection_type_.figure_ = subassembly_gl_name;
  }
public slots:
  /*!
   * Remove the elements of the user interface relating to this operation
   * and alert the parent view that this operation has been canceled.
   */
  void cancelOperation ( void )
  {
    QAction* cancel_action = assembly_view_->lCMW()->getUi()->cancelAssemblyConstraintAction;

    cancel_action->disconnect();
    context_menu_ ->removeAction( cancel_action );
    context_menu_->removeAction( separator_id_ );

    disconnect( subassembly_, SIGNAL( constraintCanceled() ),
		this, SLOT( cancelOperation() ) );
    disconnect( subassembly_, SIGNAL( constraintChanged( const AssemblyConstraint*,
							 const AssemblyConstraint* ) ),
		this, SLOT(constraintChanged( const AssemblyConstraint*,
					      const AssemblyConstraint* ) ) );

    emit done ( false );

    assembly_view_->constraintCanceled();
  }

signals:
  void done ( bool );

private slots:
  /*!
   * Cancel the current constraint.
   */
  void cancelCurrent ( void )
  {
    CommandHistory::instance().
      addCommand( new DeleteConstraintCommand( "delete constraint",
					       assembly_view_->parent(),
					       subassembly_ ) );
    // This causes the subassembly to emit a signal which we catch
    // below to tear down the user interface.
    subassembly_->constraints().cancelCurrent();
  }
  /*!
   * The assembly constraint has somehow changed. Note that this can be called
   * as a result of either user input or undo/redo operations.
   */
  void constraintChanged ( const AssemblyConstraint* /*old_constraint*/,
			   const AssemblyConstraint* new_constraint )
  {
    if ( new_constraint->reference0().empty() ) {
      selection_type_.filter_ = INCLUSIVE;

      assembly_view_->view()->
	setCursor( CursorFactory::instance().
		   cursor( CursorFactory::ASSEMBLY_CONSTRAINT ) );
    }
    else if ( new_constraint->reference1().empty() ) {
      selection_type_.filter_ = EXCLUSIVE;

      assembly_view_->view()->
	setCursor(CursorFactory::instance().cursor(CursorFactory::
						   ASSEMBLY_CONSTRAINTPLUS));
    }
    else {
      // Evidently we've completed the constraint.

      // Reset the UI.

      QAction* cancel_action=assembly_view_->lCMW()->getUi()->cancelAssemblyConstraintAction;

      cancel_action->disconnect();
      context_menu_ ->removeAction( cancel_action );
      context_menu_->removeAction( separator_id_ );

      assembly_view_->view()->unsetCursor();

      emit done (false );

      assembly_view_->constraintComplete();

      disconnect( subassembly_, SIGNAL( constraintCanceled() ),
		  this, SLOT( cancelOperation() ) );
      disconnect( subassembly_,
		  SIGNAL( constraintChanged( const AssemblyConstraint*,
					     const AssemblyConstraint* ) ),
		  this, SLOT(constraintChanged( const AssemblyConstraint*,
						const AssemblyConstraint* ) ) );

      // Also need to signal PlacementComplete (if so)....
      if ( subassembly_->constraints().status() == PlacementComplete )
	assembly_view_->placementComplete();
    }
  }

private:
  //! OpenGL view context menu.
  QMenu* context_menu_;
  //! Separator for Cancel action.
  QAction* separator_id_;
  //! Parent assembly view.
  AssemblyView* assembly_view_;
  //! Current subassembly being constrained.
  Subassembly* subassembly_;
  //! Includes selection name of subassembly and filter flag.
  SelectionType selection_type_;
  //! Status of last attempted surface pick.
  AssemblyConstraintStatus status_;
};

/*!
 * Take over the input duties when a constraint is being made. Seems to
 * actually be generic now (except for offset constraints).
 */
class OffsetConstraintInput : public InputObject {
  Q_OBJECT
public:
  /*!
   * This class is embedded in an Assembly view, but C++
   * does not have interior classes so it has to have a pointer to
   * the parent.
   * \param assembly_view parent assembly view.
   */
  OffsetConstraintInput ( AssemblyView* assembly_view )
    : assembly_view_( assembly_view ), selection_type_( PICK, FACE )
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
   * Note: This can now be invoked at any time during a constraint's
   * construction, so this method has to be more sensitive to the
   * current status of the constraint.
   *
   * \param context_menu main OpenGL view's context menu.
   */
  void startDisplay ( QMenu* context_menu )
  {
    context_menu_ = context_menu;

    QAction* cancel_action = assembly_view_->lCMW()->getUi()->cancelAssemblyConstraintAction;

    separator_id_ = context_menu_->addSeparator();
    context_menu_->addAction( cancel_action );
    connect( cancel_action, SIGNAL( activated() ), SLOT( cancelCurrent() )  );

    status_ = subassembly_->constraints().status();

    switch ( status_ ) {
    case Invalid:
      selection_type_.filter_ = INCLUSIVE;

      assembly_view_->view()->
	setCursor( CursorFactory::instance().
		   cursor( CursorFactory::ASSEMBLY_CONSTRAINT ) );
      break; 
    case OK:
      selection_type_.filter_ = EXCLUSIVE;

      assembly_view_->view()->
	setCursor( CursorFactory::instance().
		   cursor( CursorFactory::ASSEMBLY_CONSTRAINTPLUS ) );

    case ConstraintComplete:
      break;
    case PlacementComplete:
      break;
    }
  }
  /*!
   * When changing the current page, remove the context dialog action and
   * undo any cursor changes.
   */
  void stopDisplay ( QMenu* /*context_menu*/ )
  {
    QAction* cancel_action = assembly_view_->lCMW()->getUi()->cancelAssemblyConstraintAction;

    cancel_action->disconnect();
    context_menu_->removeAction( cancel_action );
    context_menu_->removeAction( separator_id_ );

    assembly_view_->view()->unsetCursor();
  }
  /*!
   * Respond to the mouse moving about the display. For this class, PageView
   * handles the highlighting.
   */
  void mousePrepress ( QMouseEvent* /*me*/, const SelectedNames& /*selected*/ )
  {}
  /*!
   * This class operates in pick mode, so take the selected face and
   * make sure it is OK with the constraint.
   * \param me Qt mouse event.
   * \param selected list of selected items.
   */
  void mousePress ( QMouseEvent* me, const SelectedNames& selected )
  {
    if ( selected.empty() ) {
      me->ignore();
      status_ = Invalid;
      return;
    }

    SelectedNames::const_iterator f = selected.begin();

    if ( (*f).second.size() < 2 ) {
      me->ignore();
      status_ = Invalid;
      return;
    }

    //TODO check this works
    std::shared_ptr<FigureViewBase> tmpFig = (assembly_view_-> figureSelectionNames()[ (*f).second[0] ]);
    SubassemblyView* sv =
      dynamic_cast<SubassemblyView*>( tmpFig.get() );

    // Validate this geometry against the current constraint, but don't
    // emit the constraint changed signal until the user releases the mouse
    // button (seems to make more sense, now).

    status_ = subassembly_->constraints().validate( sv->geomPath( (*f).second ) );

    if ( status_ == Invalid ) {
      QWhatsThis::showText( QCursor::pos(), tr( "The selected face is invalid." ) );
#if 0
      me->ignore();
#endif
    }
  }
  /*!
   * Since it runs in pick mode, there is nothing to drag.
   */
  void mouseDrag ( QMouseEvent* /*me*/, const SelectedNames& /*selected*/ ) {}
  /*!
   * When the mouse is released, we can either keep asking for more
   * surfaces or report that the constraint is complete.
   */
  bool mouseRelease ( QMouseEvent* /*me*/, const SelectedNames& /*selected*/ )
  {
    switch ( status_ ) {
    case OK:
      // Now ready to signal that the constraint has changed.
      subassembly_->constraints().apply();

      CommandHistory::instance().
	addCommand( new CreateConstraintCommand( "create constraint",
						 assembly_view_->parent(),
						 subassembly_ ) );

      selection_type_.filter_ = EXCLUSIVE;

      assembly_view_->view()->
	setCursor(CursorFactory::instance().cursor(CursorFactory::
						   ASSEMBLY_CONSTRAINTPLUS));
      return false;		// Pick another.

    case Invalid:
      return false;		// Try again.

    case ConstraintComplete:
      return false;		// Try again.
    case PlacementComplete:
       return false;		// Try again.
    }

    // Prompt for the size of the offset.
    double offset = 0;
    int ret = assembly_view_->editOffset( subassembly_->constraints().current()->
					  type(), offset );

    if ( ret == QDialog::Rejected ) {
      cancelCurrent();
      return true;
    }

    // Apply the offset.
    subassembly_->constraints().setOffset( offset );

    // The constraint (and possibly the placement) is complete. Now
    // actually perform the subassembly transformation. Signals that
    // the constraint has changed (again).

    subassembly_->constraints().transform();

    assembly_view_->figureModified();

    CommandHistory::instance().
      addCommand( new CreateConstraintCommand( "create constraint",
					       assembly_view_->parent(),
					       subassembly_ ) );


    return true;
  }
  //! Can't think of anything for this to do.
  void mouseDoubleClick ( QMouseEvent* /*me*/ ) {}
  /*!
   * The only keyboard input currently of interest is the Escape key to
   * cancel the creation of the current constraint.
   */
  void keyPress ( QKeyEvent* ke )
  {
    if ( ke->key() == Qt::Key_Escape )
      cancelCurrent();
    else
      ke->ignore();
  }
  /*!
   * This object is reused as subassemblies are created. Set the subassembly
   * which is being modified.
   * \param subassembly subassembly gaining constraint.
   * \param subassembly_gl_name used to limit surface picks to: first, the
   * subassembly; then, the remainder of the items in the assembly.
   */
  void setSubassembly ( Subassembly* subassembly, GLuint subassembly_gl_name )
  {
    subassembly_ = subassembly;

    connect( subassembly_, SIGNAL( constraintCanceled() ), SLOT(cancelOperation()));
    connect( subassembly_, SIGNAL( constraintChanged( const AssemblyConstraint*,
						      const AssemblyConstraint* ) ),
	     SLOT(constraintChanged( const AssemblyConstraint*,
				     const AssemblyConstraint* ) ) );

    selection_type_.filter_ = INCLUSIVE;
    selection_type_.figure_ = subassembly_gl_name;
  }
public slots:
  /*!
   * Remove the elements of the user interface relating to this operation
   * and alert the parent view that this operation has been canceled.
   */
  void cancelOperation ( void )
  {
    QAction* cancel_action = assembly_view_->lCMW()->getUi()->cancelAssemblyConstraintAction;

    cancel_action->disconnect();
    context_menu_->removeAction( cancel_action );
    context_menu_->removeAction( separator_id_ );

    disconnect( subassembly_, SIGNAL( constraintCanceled() ),
		this, SLOT( cancelOperation() ) );
    disconnect( subassembly_, SIGNAL( constraintChanged( const AssemblyConstraint*,
							 const AssemblyConstraint* ) ),
		this, SLOT(constraintChanged( const AssemblyConstraint*,
					      const AssemblyConstraint* ) ) );

    emit done ( false );

    assembly_view_->constraintCanceled();
  }

signals:
  void done ( bool );

private slots:
  /*!
   * Cancel the current constraint.
   */
  void cancelCurrent ( void )
  {
    CommandHistory::instance().
      addCommand( new DeleteConstraintCommand( "delete constraint",
					       assembly_view_->parent(),
					       subassembly_ ) );
    // This causes the subassembly to emit a signal which we catch
    // below to tear down the user interface.
    subassembly_->constraints().cancelCurrent();
  }
  /*!
   * The assembly constraint has somehow changed. Note that this can be called
   * as a result of either user input or undo/redo operations.
   */
  void constraintChanged ( const AssemblyConstraint* /*old_constraint*/,
			   const AssemblyConstraint* new_constraint )
  {
    if ( new_constraint->reference0().empty() ) {
      selection_type_.filter_ = INCLUSIVE;

      assembly_view_->view()->
	setCursor( CursorFactory::instance().
		   cursor( CursorFactory::ASSEMBLY_CONSTRAINT ) );
    }
    else if ( new_constraint->reference1().empty() ) {
      selection_type_.filter_ = EXCLUSIVE;

      assembly_view_->view()->
	setCursor(CursorFactory::instance().cursor(CursorFactory::
						   ASSEMBLY_CONSTRAINTPLUS));
    }
    else {
      // Evidently we've completed the constraint through redo.

      // Reset the UI.

      QAction* cancel_action=assembly_view_->lCMW()->getUi()->cancelAssemblyConstraintAction;

      cancel_action->disconnect();
      context_menu_->removeAction( cancel_action );
      context_menu_->removeAction( separator_id_ );

      assembly_view_->view()->unsetCursor();

      emit done (false );

      assembly_view_->constraintComplete();

      disconnect( subassembly_, SIGNAL( constraintCanceled() ),
		  this, SLOT( cancelOperation() ) );
      disconnect( subassembly_,
		  SIGNAL( constraintChanged( const AssemblyConstraint*,
					     const AssemblyConstraint* ) ),
		  this, SLOT(constraintChanged( const AssemblyConstraint*,
						const AssemblyConstraint* ) ) );

      // Also need to signal PlacementComplete (if so)....
      if ( subassembly_->constraints().status() == PlacementComplete )
	assembly_view_->placementComplete();
    }
  }

private:
  //! OpenGL view context menu.
  QMenu* context_menu_;
  //! Separator for Cancel action.
  QAction* separator_id_;
  //! Parent assembly view.
  AssemblyView* assembly_view_;
  //! Current subassembly being constrained.
  Subassembly* subassembly_;
  //! Includes selection name of subassembly and filter flag.
  SelectionType selection_type_;
  //! Status of last attempted surface pick.
  AssemblyConstraintStatus status_;
};

/*!
 * Let the user pick a model to remove from the assembly (maybe "remove"
 * is a better word than "delete").
 */
class ModelDeleteInput : public InputObject {
  Q_OBJECT
public:
  ModelDeleteInput ( AssemblyView* assembly_view )
    : assembly_view_( assembly_view )
  {}

  ~ModelDeleteInput ( void )
  {}

  //! \return the current selection type (always PICK)
  SelectionType selectionType ( void ) const
  {
    return SelectionType( PICK );
  }

  //! \return true does need selection scan.
  bool needsPrepressMouseCoordinates ( void ) const { return true; }

  void startDisplay ( QMenu* context_menu )
  {
    context_menu_ = context_menu;

    QAction* cancel_action = assembly_view_->lCMW()->getUi()->cancelConstraintDeleteAction;

    separator_id_ = context_menu_->addSeparator();
    context_menu_ ->addAction( cancel_action );
    connect( cancel_action, SIGNAL( activated() ), SLOT( cancelOperation() )  );

    assembly_view_->view()->
      setCursor( CursorFactory::instance().cursor( CursorFactory::DELETE_CONSTRAINT ) );
  }
  /*!
   * When changing the current page remove the context dialog action and
   * undo any cursor changes.
   */
  void stopDisplay ( QMenu* /*context_menu*/ )
  {
    // Not the only stopDisplay which needs a make-over...
#if 0
    QAction* cancel_action = parent_->lCMW()->cancelAssemblyConstraintAction;

    cancel_action->disconnect();
    cancel_action->removeFrom( context_menu_ );
    context_menu_->removeItem( separator_id_ );

    assembly_view_->view()->unsetCursor();
#endif
  }

  void mousePrepress ( QMouseEvent* /*me*/, const SelectedNames& /*selected*/ )
  {}

  void mousePress ( QMouseEvent* /*me*/, const SelectedNames& selected )
  {
    // This is in pick mode, so we only consider the first geometry
    // selected (rather than all of them like we had...)

    // If the user just clicks randomly, ignore it, but stay in pick mode.
    if ( selected.empty() )
      return;

    SelectedNames::const_iterator f = selected.begin();

    // This shouldn't happen, but we can ignore it and stay in pick mode.
    if ( (*f).second.size() < 1 )
      return;

    //TODO check this works
    std::shared_ptr<FigureViewBase> tmpFig = (assembly_view_-> figureSelectionNames()[ (*f).second[0] ]);
    SubassemblyView* sv =
      dynamic_cast<SubassemblyView*>( tmpFig.get() );
    if ( sv != 0 ) {
      if ( !assembly_view_->assembly()->referenced( sv->subassembly() ) ) {
	target_ = sv;
	return;
      }
      else
    QWhatsThis::showText(QCursor::pos(), tr( "<p>Cannot delete subassembly '%1' because it is "
				 "referenced by other subassemblies in this "
				 "assembly</p>" ).arg( sv->name() ) );
    }

    target_ = 0;
  }

  void mouseDrag ( QMouseEvent* /*me*/, const SelectedNames& /*selected*/ )
  {}

  bool mouseRelease ( QMouseEvent* /*me*/, const SelectedNames& /*selected*/ )
  {
    if ( target_ == 0 )
      return false;

    CommandHistory::instance().
      addCommand( new DeleteCommand( "delete subassembly", target_->memento()));

    // Is this enough?
    assembly_view_->removeFigureView( target_ );
    assembly_view_->updateHiddenModel();
#if 0
    // Evidently, pageview will clean us up on removeFigureView() by calling
    // cancelOperation! So, this needs some more thought...
    QAction* cancel_action = assembly_view_->lCMW()->cancelConstraintDeleteAction;
    cancel_action->disconnect();
    cancel_action->removeFrom( context_menu_ );
    context_menu_->removeItem( separator_id_ );
#endif
    assembly_view_->view()->unsetCursor();

    assembly_view_->figureComplete();

    return true;
  }

  //! Can't think of anything for this to do.
  void mouseDoubleClick ( QMouseEvent* /*me*/ ) {}

  void keyPress ( QKeyEvent* ke )
  {
    if ( ke->key() == Qt::Key_Escape )
      cancelOperation();
    else
      ke->ignore();
  }

public slots:
  void cancelOperation ( void )
  {
    QAction* cancel_action = assembly_view_->lCMW()->getUi()->cancelConstraintDeleteAction;

    cancel_action->disconnect();
    context_menu_ ->removeAction( cancel_action );
    context_menu_->removeAction ( separator_id_ );

    assembly_view_->cancelDeleteOperation();
  }
private:
  //! OpenGL view context menu.
  QMenu* context_menu_;
  //! Separator for Cancel action.
  QAction* separator_id_;
  //! Parent assembly view.
  AssemblyView* assembly_view_;
  //! Subassembly to remove.
  SubassemblyView* target_;
};

#include "assemblyview.moc"

AssemblyConfigDialog* AssemblyView::config_dialog_ = 0;

QDockWidget* AssemblyView::constraint_dock_ = 0;

AssemblyAddDialog* AssemblyView::add_dialog_ = 0;

AssemblyConstraintForm* AssemblyView::constraint_form_ = 0;

OffsetInfoDialog* AssemblyView::offset_info_dialog_ = 0;

AssemblyView::AssemblyView ( Assembly* assembly, DesignBookView* parent )
  : PageView( parent ), assembly_( assembly ), current_view_( 0 ),
    constraint_complete_( true ),
    constraint_input_( 0 ), offset_constraint_input_( 0 ),
    model_delete_input_( 0 )
{
  init();
}

AssemblyView::AssemblyView ( DesignBookView* parent, const QDomElement& xml_rep )
  : PageView( parent ), current_view_( 0 ),
    constraint_complete_( true ),
    constraint_input_( 0 ), offset_constraint_input_( 0 )
{
  DBURL db_url( xml_rep.attribute( lC::STR::ASSEMBLY ) );

  assembly_ = dynamic_cast<Assembly*>( model()->lookup( db_url ) );

  if ( assembly_ == 0 )		// This should be a fatal error.
    return;

  init();

  QDomNode n = xml_rep.firstChild();

  while ( !n.isNull() ) {
    QDomElement e = n.toElement();
    if ( !e.isNull() ) {
      if ( e.tagName() == lC::STR::RENDER_STYLE ) {
	setRenderStyle( lC::Render::style( e.attribute( lC::STR::STYLE ) ) );
      }
      else if ( e.tagName() == lC::STR::VIEW_DATA ) {
	setViewData( ViewData( e ) );
      }
      else if ( e.tagName() == lC::STR::SUBASSEMBLY_VIEW ) {
	SubassemblyView* sv = new SubassemblyView( e, this );
	// If this subassembly was not fully constrained when last we were here,
	// it is now the current view. Note that is is not entirely accurate
	// since the last figure view could be a datum...
	if ( lastFigureView() != 0 &&
	     sv->subassembly()->constraints().status() != PlacementComplete  )
	  current_view_ = sv;

	addFigureView( sv );
      }
    }
    n = n.nextSibling();
  }
}

AssemblyView::~AssemblyView ()
{
  clearFigureViews();

  delete hidden_drawer_;
  delete model_delete_input_;
  delete list_view_item_;

  if ( constraint_input_ != 0 )
    delete constraint_input_;

  if ( offset_constraint_input_ != 0 )
    delete offset_constraint_input_;

  if ( assembly_ != 0 )
    model()->removePage( assembly_ );
}

// (Probably should have made this an explicit part of the PageView interface.)

void AssemblyView::init ( void )
{
  hidden_drawer_ = new Space3D::OCHiddenDraw( assembly_->compound(), view() );

  model_delete_input_ = new ModelDeleteInput( this );

  tabIcon = lC::lookupPixmap( "assembly.png" );
  tabText = lC::formatTabName( assembly_->name() );

  ListViewItem* previous_item = parent()->previousItem( assembly_->id() );

  list_view_item_ = new ListViewItem( parent()->modelListItem(), previous_item );

  list_view_item_->setData( lC::formatName( assembly_->name() )
                + QString( " <%1>" ).arg( assembly_->id() ),
                lC::NAME);
  list_view_item_->setData( trC( lC::STR::ASSEMBLY ), lC::TYPE );
  // TODO
  //list_view_item_->setOpen( true );
  //list_view_item_->setRenameEnabled( lC::NAME, true );

  connect( this, SIGNAL( newInformation( const QString& ) ),
	   lCMW(), SLOT( updateInformation( const QString& ) ) );

  //TODO
  //connect( list_view_item_, SIGNAL( nameChanged( const QString& ) ),
  //	   SLOT( listNameChanged( const QString& ) ) );

  connect( assembly_, SIGNAL( nameChanged( const QString& ) ),
	   SLOT( updateName( const QString& ) ) );

  connect( assembly_, SIGNAL( assemblyChanged() ), SLOT( updateHiddenModel() ) );

  if ( config_dialog_ == 0 )
    config_dialog_ = new AssemblyConfigDialog( lCMW() );

  if ( constraint_dock_ == 0 ) {
    constraint_dock_ = new QDockWidget(  tr( "Assembly Constraints" ), lCMW() );

    constraint_dock_->setObjectName("constraint_dock");
    lCMW()->addDockWidget( Qt::RightDockWidgetArea, constraint_dock_ );
    //TODO
    //lCMW()->getUi()->setAppropriate( constraint_dock_, false );
    constraint_dock_->hide();
  }

  if ( add_dialog_ == 0 )
    add_dialog_ = new AssemblyAddDialog( lCMW() );

  if ( constraint_form_ == 0 ) {
    constraint_form_ = new AssemblyConstraintForm( constraint_dock_ );
    constraint_form_->setObjectName( "constraint_form" );
    constraint_dock_->setWidget( constraint_form_ );
  }

  if ( offset_info_dialog_ == 0 )
    offset_info_dialog_ = new OffsetInfoDialog( lCMW() );
}

// Do any post-creation configuration.

bool AssemblyView::configure ( void )
{
  config_dialog_->getUi()->nameEdit->setText( lC::formatName( name() ) );
  config_dialog_->getUi()->nameEdit->selectAll();
  config_dialog_->getUi()->nameEdit->setFocus();
  config_dialog_->getUi()->buttonOk->setDefault( true );

  // Canvas the available models (parts and assemblies) for the user's choice.
  config_dialog_->getUi()->modelListView->clear();

  const QMap<uint, PageBase*>& pages = model()->pages();
  QMap<uint, PageBase*>::const_iterator page = pages.begin();

  for ( ; page != pages.end(); ++page ) {
    // Skip self, of course.
    if ( page.value()->name() == name() &&
     page.value()->type() == lC::STR::ASSEMBLY ) continue;

    //TODO
//    if ( page.value()->type() == lC::STR::PART ||
//     page.value()->type() == lC::STR::ASSEMBLY )
//        new QListWidgetItem( config_dialog_->getUi()->modelListView,
//             config_dialog_->getUi()->modelListView->item(config_dialog_->getUi()->modelListView->count()),
//             lC::formatName( page.value()->name() ),
//             trC( page.value()->type() ) );
  }

  config_dialog_->getUi()->buttonOk->setEnabled( false );

  int ret = config_dialog_->exec();

  if ( ret == QDialog::Rejected ) return false;

  if ( config_dialog_->getUi()->nameEdit->isModified() )
    assembly_->setName( config_dialog_->getUi()->nameEdit->text() );

  // OK, lookup the model and create a view for it attached to this page.
  page = pages.begin();

  for ( ; page != pages.end(); ++page ) {
    if ( config_dialog_->getUi()->modelListView->currentItem()->text(0) ==
     lC::formatName( page.value()->name() ) &&
     config_dialog_->getUi()->modelListView->currentItem()->text(1) ==
     trC( page.value()->type() ) ) break;
  }

  addFigureView( new SubassemblyView( assembly_->
                      addModel( dynamic_cast<Space3D::Page*>( page.value() ) ),
				      this ) );

  return true;
}

CreateObject* AssemblyView::memento ( void )
{
  return new AssemblyViewCreate( this );
}

void AssemblyView::setName ( const QString& name )
{
  DBURL old_db_url = assembly_->dbURL();

  assembly_->setName( name );

  CommandHistory::instance().addCommand( new RenameCommand( "rename assembly",
							    model(),
							    old_db_url,
							    assembly_->dbURL() ) );
}

// Check a list view rename for uniqueness.

void AssemblyView::listNameChanged ( const QString& name )
{
  int ret = parent()->uniquePageName( this, name, assembly_->type() );

  switch ( ret ) {
  case lC::OK:
    setName( name );
    break;
  case lC::Redo:
      //TODO
      //list_view_item_->startRename( lC::NAME );
  case lC::Rejected:
    updateName( assembly_->name() ); // Repaint list item with old name.
  }
}

void AssemblyView::show ( void ) const
{
  lCMW()->getUi()->toolMenu->clear();

  lCMW()->getUi()->toolMenu->addAction( lCMW()->getUi()->toolAddModelAction );
  lCMW()->getUi()->toolMenu->addAction( lCMW()->getUi()->toolDeleteModelAction );
  lCMW()->getUi()->toolMenu->addSeparator();
  lCMW()->getUi()->toolMenu->addAction( lCMW()->getUi()->toolJointAction );

  lCMW()->getUi()->toolAddModelAction->disconnect();
  lCMW()->getUi()->toolDeleteModelAction->disconnect();
  lCMW()->getUi()->toolJointAction->disconnect();

  connect( lCMW()->getUi()->toolAddModelAction, SIGNAL( activated() ), SLOT( addModel() ) );
  connect( lCMW()->getUi()->toolDeleteModelAction, SIGNAL( activated() ),
	   SLOT( deleteModel() ) );

  // Save our sanity by not allowing the user to delete the first model.
  // Although eventually, this will be a legitimate action.
  if ( assembly_->figures().count() <= 1 )
    lCMW()->getUi()->toolDeleteModelAction->setEnabled( false );
  else
    lCMW()->getUi()->toolDeleteModelAction->setEnabled( true );
}

/*
 * Add this view's XML description to the output documennt
 */
void AssemblyView::write ( QDomElement& xml_rep ) const
{
  QDomDocument document = xml_rep.ownerDocument();
  QDomElement view_element = document.createElement( lC::STR::ASSEMBLY_VIEW );
  view_element.setAttribute( lC::STR::ASSEMBLY, assembly_->dbURL().toString() );

  QDomElement render_style_element = document.createElement( lC::STR::RENDER_STYLE);
  render_style_element.setAttribute( lC::STR::STYLE,
				     lC::Render::styleText( renderStyle() ) );
  view_element.appendChild( render_style_element );

  viewData().write( view_element );

  QListIterator< std::shared_ptr<FigureViewBase> > fv( figureViews() );

  while(fv.hasNext())
    fv.next()->write( view_element );

  xml_rep.appendChild( view_element );
}

SelectionType AssemblyView::defaultSelectionType ( void ) const
{
  // The default for assemblies is that nothing is active by default.
  return SelectionType( TRANSIENT, FIGURE );
}

/*
 * Append some useful actions to the OpenGL view context menu.
 */
void AssemblyView::startDisplay ( QMenu* context_menu )
{
  context_menu_ = context_menu;

  context_menu_->addSeparator();
  wireframe_id_ = context_menu_->addAction( tr( "Wireframe" ));
  wireframe_id_->setCheckable( true );
  hidden_id_ = context_menu_->addAction( tr( "Hidden Line" ));
  hidden_id_->setCheckable( true );
  solid_id_ = context_menu_->addAction( tr( "Solid" ));
  solid_id_->setCheckable( true );
  texture_id_ = context_menu_->addAction( tr( "Texture" ));
  texture_id_->setCheckable( true );

  connect( view(), SIGNAL( rotation( const GLdouble* ) ),
	   SIGNAL( orientationChanged( const GLdouble* ) ) );

  switch ( renderStyle() ) {
  case lC::Render::WIREFRAME:
    wireframe_id_->setChecked( true );
    break;
  case lC::Render::HIDDEN:
    hidden_id_->setChecked( true );
    connect( view(), SIGNAL( rotation( const GLdouble* ) ),
	     SLOT( updateHiddenOrientation( const GLdouble* ) ) );
    break;
  case lC::Render::SOLID:
    solid_id_->setChecked( true );
    break;
  case lC::Render::TEXTURED:
    texture_id_->setChecked( true );
    break;
  }

  connect( assembly_, SIGNAL( nameChanged( const QString& ) ),
	   lCMW(), SLOT( pageChanged( const QString& ) ) );

  // Probably not a good idea to leave the page while you're constraining
  // a subassembly, but we can try to provide for this action. Check
  // the current subassembly to see if it is completely constrained.
  // If not, engage the constraint editor.

  if ( current_view_ != 0 &&
       current_view_->subassembly()->constraints().status() != PlacementComplete ) {
    editConstraints( current_view_ );

    QListIterator<std::shared_ptr<AssemblyConstraint>> constraint =
      current_view_->subassembly()->constraints().constraints();

    while( constraint.hasNext() ) {
      constraints_text_.push_back( QStringList() );
      AssemblyConstraint* tmpAsc = constraint.next().get();
      if ( tmpAsc->type() == lC::STR::MATE_OFFSET ||
       tmpAsc->type() == lC::STR::ALIGN_OFFSET )
	constraints_text_.back() << QString( tr( "%1 (%2)" ) ).
      arg( trC( tmpAsc->type() ) ).
      arg( UnitsBasis::instance()->format( tmpAsc->offset(), false ) );
      else
    constraints_text_.back() << trC( tmpAsc->type() );

      if ( !tmpAsc->reference0().empty() )
    constraints_text_.back() << model()->idPath( tmpAsc->reference0() );

      if ( !tmpAsc->reference1().empty() )
    constraints_text_.back() << model()->idPath( tmpAsc->reference1() );
    }

    updateConstraintLabel();

    if ( current_view_->subassembly()->constraints().status() !=
	 ConstraintComplete ) { 

      constraint.toBack();

      if ( constraint.hasPrevious() != 0 ) {
    editConstraint( constraint.previous().get() );
      }
    }
  }
}
/*
 * Clean up when are not the current page.
 */
void AssemblyView::stopDisplay ( QMenu* /*context_menu*/ )
{
  if ( current_view_ != 0 &&
       current_view_->subassembly()->constraints().status() != PlacementComplete ) {

    if ( inputObject() != 0 )
      inputObject()->stopDisplay( context_menu_ );

    // This does not seem strictly necessary...
    // Also need to think about going to the page where the Undo occurs...
    disconnect( current_view_, SIGNAL( destroyedSubassembly() ),
		this, SLOT( subassemblyDestroyed() ) );

    disconnect( current_view_->subassembly(),
		SIGNAL( constraintCreated( const AssemblyConstraint*) ),
		this, SLOT( constraintAdded( const AssemblyConstraint* ) ) );

    disconnect( current_view_->subassembly(),
		SIGNAL( constraintChanged( const AssemblyConstraint*,
					   const AssemblyConstraint* ) ),
		this, SLOT( constraintModified( const AssemblyConstraint*,
						const AssemblyConstraint* ) ) );
    disconnect( current_view_->subassembly(),
		SIGNAL( constraintOffsetChanged( const AssemblyConstraint* ) ),
		this, SLOT( constraintOffsetChanged( const AssemblyConstraint* ) ));

    // Undo may take us back to the editing an existing subassembly.

    connect( current_view_->subassembly(),
	     SIGNAL( constraintChanged( const AssemblyConstraint*,
					const AssemblyConstraint* ) ),
	     SLOT( reeditSubassembly( const AssemblyConstraint*,
				      const AssemblyConstraint* ) ) );
	   

    QAction* cancel_action = lCMW()->getUi()->cancelAddModelAction;

    cancel_action->disconnect();
    context_menu_->removeAction( cancel_action );
    context_menu_->removeAction( separator_id_ );

    disconnect( constraint_form_->getUi()->matePushButton, SIGNAL( clicked() ),
		this, SLOT( mate() ) );
    disconnect( constraint_form_->getUi()->alignPushButton, SIGNAL( clicked() ),
		this, SLOT( align() ) );
    disconnect( constraint_form_->getUi()->mateOffsetPushButton, SIGNAL( clicked() ),
		this, SLOT( mateOffset() ) );
    disconnect( constraint_form_->getUi()->alignOffsetPushButton, SIGNAL( clicked() ),
		this, SLOT( alignOffset() ) );
    disconnect( constraint_form_->getUi()->cancelPushButton, SIGNAL( clicked() ),
		this, SLOT( cancelOperation() ) );

    constraint_dock_->hide();
  }
  /*
   * When we are not the current page, don't respond to changes in the view
   * orientation.
   */
  disconnect( view(), SIGNAL( rotation( const GLdouble* ) ),
	      this, SIGNAL( orientationChanged( const GLdouble* ) ) );
  if ( renderStyle() == lC::Render::HIDDEN )
    disconnect( view(), SIGNAL( rotation( const GLdouble* ) ),
		this, SLOT( updateHiddenOrientation( const GLdouble* ) ) );

  disconnect( assembly_, SIGNAL( nameChanged( const QString& ) ),
	      lCMW(), SLOT( pageChanged( const QString& ) ) );
}

/*
 * Draw the view.
 */
void AssemblyView::draw ( void ) const
{
  glPushAttrib( GL_LIGHTING_BIT );

  glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

  if ( renderStyle() == lC::Render::HIDDEN )
    hidden_drawer_->draw();

  QListIterator< std::shared_ptr<FigureViewBase> > f( figureViews() );

  while(f.hasNext())
    f.next()->draw();

  glPopAttrib();
}

/*!
 * Draw (in GL_SELECT mode) the elements indicated by the given
 * selection type. For speed, this is generally a simplified version
 * of the view.
 * \param select_type the selection mode type.
 */
void AssemblyView::select ( SelectionType /*select_type*/ ) const
{
  QListIterator< std::shared_ptr<FigureViewBase> > f( figureViews() );

  while (f.hasNext())
    f.next()->select( selectionType() );
}

void AssemblyView::updateName ( const QString& name )
{
  //TODO need to update ?
  tabText = lC::formatTabName( name );
  list_view_item_->setData( lC::formatName( assembly_->name() )
                + QString( " <%1>" ).arg( assembly_->id() ),
                            lC::NAME );
}

// Allow the user to switch among the various rendering styles.

void AssemblyView::toggleRenderStyle ( QAction* id )
{
  if ( renderStyle() == lC::Render::HIDDEN && id != hidden_id_ )
    disconnect( view(), SIGNAL( rotation( const GLdouble* ) ),
		this, SLOT( updateHiddenOrientation( const GLdouble* ) ) );

  if ( id == wireframe_id_ and renderStyle() != lC::Render::WIREFRAME ) {
    wireframe_id_->setChecked( true );
    hidden_id_->setChecked( false );
    solid_id_->setChecked( false );
    texture_id_->setChecked( false );

    setRenderStyle( lC::Render::WIREFRAME );
  }
  else if ( id == hidden_id_ and renderStyle() != lC::Render::HIDDEN ) {
      wireframe_id_->setChecked( false );
      hidden_id_->setChecked( true );
      solid_id_->setChecked( false );
      texture_id_->setChecked( false );

    setRenderStyle( lC::Render::HIDDEN );

    connect( view(), SIGNAL( rotation( const GLdouble* ) ),
	     SLOT( updateHiddenOrientation( const GLdouble* ) ) );

    updateHiddenOrientation( view()->viewOrientation() );
  }
  else if ( id == solid_id_ and renderStyle() != lC::Render::SOLID ) {
      wireframe_id_->setChecked( false );
      hidden_id_->setChecked( false );
      solid_id_->setChecked( true );
      texture_id_->setChecked( false );

    setRenderStyle( lC::Render::SOLID );
  }
  else if ( id == texture_id_ and renderStyle() != lC::Render::TEXTURED ) {
      wireframe_id_->setChecked( false );
      hidden_id_->setChecked( false );
      solid_id_->setChecked( false );
      texture_id_->setChecked( true );
    setRenderStyle( lC::Render::TEXTURED );
  }

  view()->updateGL();
}

/*
 * Determine which figure to create from its simplified XML representation.
 */
void AssemblyView::pasteFigure ( const QDomElement& /*xml_rep*/ )
{
}

void AssemblyView::cancelOperation ( void )
{
  placementComplete();

  figureModified();

  CommandHistory::instance().
    addCommand( new DeleteCommand( "delete subassembly", current_view_->memento()));

  removeFigureView( current_view_ );

  if ( assembly_->figures().count() <= 1 )
    lCMW()->getUi()->toolDeleteModelAction->setEnabled( false );

  SelectedNames none;
  highlightFigures( none );

  deactivateFigures();

  current_view_ = 0;

  emit newInformation( trC( lC::STR::NONE ) );
}

// Add another model to the assembly.

void AssemblyView::addModel ( void )
{
  // Canvas the available models (parts and assemblies) for the user's choice.
  add_dialog_->getUi()->modelListView->clear();

  const QMap<uint, PageBase*>& pages = model()->pages();
  QMap<uint, PageBase*>::const_iterator page = pages.begin();
  for ( ; page != pages.end(); ++page ) {
    // Skip self, of course.
    if ( page.value()->name() == name() &&
     page.value()->type() == lC::STR::ASSEMBLY ) continue;

    //TODO
//    if ( page.value()->type() == lC::STR::PART ||
//     page.value()->type() == lC::STR::ASSEMBLY )
//      new QListViewItem( add_dialog_->getUi()->modelListView,
//			 add_dialog_->modelListView->lastItem(),
//             lC::formatName( page.value()->name() ),
//             trC( page.value()->type() ) );
  }

  add_dialog_->getUi()->buttonOk->setEnabled( false );
  add_dialog_->getUi()->modelListView->setFocus();

  int ret = add_dialog_->exec();

  if ( ret == QDialog::Rejected ) return;

  // OK, lookup the model and create a view for it attached to this page.

  page = pages.begin();
  for ( ; page != pages.end(); ++page ) {
    if ( add_dialog_->getUi()->modelListView->currentItem()->text(0) ==
     lC::formatName( page.value()->name() ) &&
     add_dialog_->getUi()->modelListView->currentItem()->text(1) ==
     trC( page.value()->type() ) ) break;
  }

  SubassemblyView* sv =
    new SubassemblyView( assembly_->
             addModel( dynamic_cast<Space3D::Page*>( page.value() ) ),
			 this );

  addFigureView( sv );

  lCMW()->getUi()->toolDeleteModelAction->setEnabled( true );

  figureModified();

  CommandHistory::instance().
    addCommand( new CreateCommand( "create subassembly", sv->memento() ) );

  editConstraints( sv );
}

void AssemblyView::editConstraints ( SubassemblyView* subassembly_view )
{
  current_view_ = subassembly_view;

  connect( current_view_->subassembly(),
	   SIGNAL( constraintCreated( const AssemblyConstraint*) ),
	   SLOT( constraintAdded( const AssemblyConstraint* ) ) );

  connect( current_view_->subassembly(),
	   SIGNAL( constraintChanged( const AssemblyConstraint*,
				      const AssemblyConstraint* ) ),
	   SLOT( constraintModified( const AssemblyConstraint*,
				     const AssemblyConstraint* ) ) );
  connect( current_view_->subassembly(),
	   SIGNAL( constraintOffsetChanged( const AssemblyConstraint* ) ),
	   SLOT( constraintOffsetChanged( const AssemblyConstraint* ) ) );

  connect( current_view_, SIGNAL( destroyedSubassembly() ),
	   SLOT( subassemblyDestroyed() ) );

  disconnect( current_view_->subassembly(),
	      SIGNAL( constraintChanged( const AssemblyConstraint*,
					 const AssemblyConstraint* ) ),
	      this, SLOT( reeditSubassembly( const AssemblyConstraint*,
					     const AssemblyConstraint* ) ) );

  QAction* cancel_action = lCMW()->getUi()->cancelAddModelAction;

  separator_id_ = context_menu_->addSeparator();
  context_menu_->addAction( cancel_action );
  connect( cancel_action, SIGNAL( activated() ), SLOT( cancelOperation() )  );

  constraint_form_->getUi()->constraintsTextLabel->
    setText( tr( "<p>Defined Constraints for %1</p>" ).
	     arg( lC::formatName( current_view_->subassembly()->name() ) ) );

  constraints_text_.clear();

  updateConstraintLabel();

  constraint_complete_ = true;

  constraint_form_->getUi()->matePushButton->setChecked( false );
  constraint_form_->getUi()->alignPushButton->setChecked( false );
  constraint_form_->getUi()->mateOffsetPushButton->setChecked( false );
  constraint_form_->getUi()->alignOffsetPushButton->setChecked( false );

  connect( constraint_form_->getUi()->matePushButton, SIGNAL( clicked() ),
	   SLOT( mate() ) );
  connect( constraint_form_->getUi()->alignPushButton, SIGNAL( clicked() ),
	   SLOT( align() ));
  connect( constraint_form_->getUi()->mateOffsetPushButton, SIGNAL( clicked() ),
	   SLOT( mateOffset() ) );
  connect( constraint_form_->getUi()->alignOffsetPushButton, SIGNAL( clicked() ),
	   SLOT( alignOffset() ) );

  connect( constraint_form_->getUi()->cancelPushButton, SIGNAL( clicked() ),
	   SLOT( cancelOperation() ) );

  constraint_dock_->show();

  emit newInformation( tr( "Place Model" ) );
}

// Start editing an existing constraint (that somebody else added).

void AssemblyView::editConstraint ( const AssemblyConstraint* constraint )
{
  constraint_complete_ = false;

  if ( constraint->type() == lC::STR::MATE ) {
    constraint_form_->getUi()->matePushButton->setChecked ( true );

    if ( constraint_input_ == 0 )
      constraint_input_ = new ConstraintInput( this );

    constraint_input_->setSubassembly( current_view_->subassembly(),
				       current_view_->selectionName() );
    constraint_input_->startDisplay( context_menu_ );

    connect( constraint_input_, SIGNAL( done( bool ) ),
         constraint_form_->getUi()->matePushButton, SLOT( setChecked( bool ) ) );

    setInputObject( constraint_input_ );
  }
  else if ( constraint->type() == lC::STR::ALIGN ) {
    constraint_form_->getUi()->alignPushButton->setChecked( true );

    if ( constraint_input_ == 0 )
      constraint_input_ = new ConstraintInput( this );

    constraint_input_->setSubassembly( current_view_->subassembly(),
				       current_view_->selectionName() );
    constraint_input_->startDisplay( context_menu_ );

    connect( constraint_input_, SIGNAL( done( bool ) ),
         constraint_form_->getUi()->alignPushButton, SLOT( setChecked( bool ) ) );

    setInputObject( constraint_input_ );
  }
  else if ( constraint->type() == lC::STR::MATE_OFFSET ) {
    constraint_form_->getUi()->mateOffsetPushButton->setChecked( true );

    if ( offset_constraint_input_ == 0 )
      offset_constraint_input_ = new OffsetConstraintInput( this );

    offset_constraint_input_->setSubassembly( current_view_->subassembly(),
					      current_view_->selectionName() );
    offset_constraint_input_->startDisplay( context_menu_ );

    connect( offset_constraint_input_, SIGNAL( done( bool ) ),
         constraint_form_->getUi()->mateOffsetPushButton, SLOT( setChecked( bool ) ) );

    setInputObject( offset_constraint_input_ );
  }
  else if ( constraint->type() == lC::STR::ALIGN_OFFSET ) {
    constraint_form_->getUi()->alignOffsetPushButton->setChecked( true );

    if ( offset_constraint_input_ == 0 )
      offset_constraint_input_ = new OffsetConstraintInput( this );

    offset_constraint_input_->setSubassembly( current_view_->subassembly(),
					      current_view_->selectionName() );
    offset_constraint_input_->startDisplay( context_menu_ );

    connect( offset_constraint_input_, SIGNAL( done( bool ) ),
         constraint_form_->getUi()->alignOffsetPushButton, SLOT( setChecked( bool ) ) );

    setInputObject( offset_constraint_input_ );
  }

  if ( !constraint->reference0().empty() )
    activateFigure( parent()->lookup( constraint->reference0() ) );
}

void AssemblyView::constraintAdded ( const AssemblyConstraint* constraint )
{
  constraints_text_.push_back( QStringList() );

  if ( constraint->type() == lC::STR::MATE_OFFSET ||
       constraint->type() == lC::STR::ALIGN_OFFSET )
    constraints_text_.back() << QString( tr( "%1 (%2)" ) ).
      arg( trC( constraint->type() ) ).
      arg( UnitsBasis::instance()->format( constraint->offset(), false ) );
  else
    constraints_text_.back() << trC( constraint->type() );

  constraint_complete_ = false;

  InputObject* constraint_input = 0;

  if ( constraint->type() == lC::STR::MATE ) {
    constraint_form_->getUi()->matePushButton->setChecked( true );

    if ( constraint_input_ == 0 )
      constraint_input_ = new ConstraintInput( this );

    constraint_input = constraint_input_;

    constraint_input_->setSubassembly( current_view_->subassembly(),
				       current_view_->selectionName() );

    connect( constraint_input_, SIGNAL( done( bool ) ),
         constraint_form_->getUi()->matePushButton, SLOT( setChecked( bool ) ) );
  }
  else if ( constraint->type() == lC::STR::ALIGN ) {
    constraint_form_->getUi()->alignPushButton->setChecked( true );

    if ( constraint_input_ == 0 )
      constraint_input_ = new ConstraintInput( this );

    constraint_input = constraint_input_;

    constraint_input_->setSubassembly( current_view_->subassembly(),
				       current_view_->selectionName() );

    connect( constraint_input_, SIGNAL( done( bool ) ),
         constraint_form_->getUi()->alignPushButton, SLOT( setChecked( bool ) ) );
  }
  else if ( constraint->type() == lC::STR::MATE_OFFSET ) {
    constraint_form_->getUi()->mateOffsetPushButton->setChecked( true );

    if ( offset_constraint_input_ == 0 )
      offset_constraint_input_ = new OffsetConstraintInput( this );

    constraint_input = offset_constraint_input_;

    offset_constraint_input_->setSubassembly( current_view_->subassembly(),
					      current_view_->selectionName() );

    connect( offset_constraint_input_, SIGNAL( done( bool ) ),
         constraint_form_->getUi()->mateOffsetPushButton, SLOT( setChecked( bool ) ) );
  }
  else if ( constraint->type() == lC::STR::ALIGN_OFFSET ) {
    constraint_form_->getUi()->alignOffsetPushButton->setChecked( true );

    if ( offset_constraint_input_ == 0 )
      offset_constraint_input_ = new OffsetConstraintInput( this );

    constraint_input = offset_constraint_input_;

    offset_constraint_input_->setSubassembly( current_view_->subassembly(),
					      current_view_->selectionName() );

    connect( offset_constraint_input_, SIGNAL( done( bool ) ),
         constraint_form_->getUi()->alignOffsetPushButton, SLOT( setChecked( bool ) ) );
  }

  constraint_input->startDisplay( context_menu_ );

  setInputObject( constraint_input );

  // A partially completed constraint can be added due to undoing a delete
  // of a partially completed constraint.
  if ( !constraint->reference0().empty() ) {
    constraints_text_.back() << model()->idPath( constraint->reference0() );
    activateFigure( parent()->lookup( constraint->reference0() ) );
  }

  updateConstraintLabel();
}

  inline QString namePathToString ( const std::vector<GLuint>& ID )
  {
    QString id_string;

    if ( ID.size() < 1 ) return QString::null;

    id_string = QString::number( ID[0] );

    for ( uint i = 1; i < ID.size(); ++i )
      id_string += QString( " : %1" ).arg( ID[i] );

    return id_string;
  }

void AssemblyView::constraintModified ( const AssemblyConstraint* old_constraint,
					const AssemblyConstraint* new_constraint )
{
  if ( new_constraint->reference0().empty() &&
       !old_constraint->reference0().empty() ) {
    // Undid first selection. Deactivate it.
    deactivateFigure( parent()->lookup( old_constraint->reference0() ) );
  }
  else if ( !new_constraint->reference0().empty() &&
	    old_constraint->reference0().empty() ) {
    // Redid first selection. Activate it again.
    activateFigure( parent()->lookup( new_constraint->reference0() ) );
  }
  if ( new_constraint->reference1().empty() &&
       !old_constraint->reference1().empty() ) {
    // Undid last selection after last constraint was complete.
    // Reactivate first selection and restart edit of last constraint.
    editConstraint( new_constraint );
    // (Evidently after above call, PageView selection type is correct.)
    activateFigure( parent()->lookup( new_constraint->reference0() ) );
  }
  else if ( !new_constraint->reference1().empty() &&
	    old_constraint->reference1().empty() ) {
    // Redid second selection. Should result in completion of constraint...
    deactivateFigure( parent()->lookup( new_constraint->reference0() ) );
    deactivateFigure( parent()->lookup( new_constraint->reference1() ) );
  }


  constraints_text_.back().clear();

  if ( new_constraint->type() == lC::STR::MATE_OFFSET ||
       new_constraint->type() == lC::STR::ALIGN_OFFSET )
    constraints_text_.back() << QString( tr( "%1 (%2)" ) ).
      arg( trC( new_constraint->type() ) ).
      arg( UnitsBasis::instance()->format( new_constraint->offset(), false ) );
  else
    constraints_text_.back() << trC( new_constraint->type() );

  if ( !new_constraint->reference0().empty() )
    constraints_text_.back() << lC::formatName( model()->idPath( new_constraint->reference0() ) );

  if ( !new_constraint->reference1().empty() )
    constraints_text_.back() << lC::formatName( model()->idPath( new_constraint->reference1() ) );

  updateConstraintLabel();
}

void AssemblyView::constraintOffsetChanged ( const AssemblyConstraint* constraint )
{
  constraints_text_[constraint->phase()][0] =
    QString( tr( "%1 (%2)" ) ).
    arg( trC( constraint->type() ) ).
    arg( UnitsBasis::instance()->format( constraint->offset(), false ) );

  updateConstraintLabel();
}

void AssemblyView::updateConstraintLabel ( void )
{
  QString text = QString( "<table><tr bgcolor=\"dark gray\"><th>%1</th><th><nobr>%2</nobr></th><th><nobr>%3</nobr></th></tr>" ).
    arg( tr( "Type" ) ).arg( tr( "Face A" ) ).arg( tr( "Face B" ) );

  if ( !constraints_text_.empty() ) {

    QVector<QStringList>::const_iterator constraint_list =
      constraints_text_.begin();

    for ( ; constraint_list != constraints_text_.end(); ++constraint_list ) {
      // Needs to be internationalized, I think...
      switch ( (*constraint_list).size() ) {
      case 1:
	text += QString( "<tr bgcolor=\"%1\">" ).
	  arg( tr( "tomato", "incomplete constraint background color" ) );
	break;
      case 2:
	text += QString( "<tr bgcolor=\"%1\">" ).
	  arg( tr( "yellow", "partially complete constraint background color" ) );
	break;
      case 3:
	text += "<tr bgcolor=\"light gray\">";
	break;
      }

      QStringList::const_iterator constraint = (*constraint_list).begin();

      for ( ; constraint != (*constraint_list).end(); ++constraint )
	text += QString( "<td>%1</td>" ).arg( *constraint );

      text += "</tr>";
    }
  }
  else
    text += QString( "<tr><td>%1</td></tr>" ).arg( trC( lC::STR::NONE ) );

  text += "</table>";

  constraint_form_->getUi()->constraintListBrowser->setText( text );
}

void AssemblyView::constraintComplete ( void )
{
  constraint_complete_ = true;
  disconnect( inputObject(), SIGNAL( done(bool) ), 0, 0 );
  
  SelectedNames none;
  highlightFigures( none );

  deactivateFigures();

  setInputObject( 0 );
}

void AssemblyView::constraintCanceled ( void )
{
  constraint_complete_ = true;

  cancelConstraint();
}

void AssemblyView::mate ( void )
{
  if ( !constraint_complete_ ) {
    // Occurs if the user toggles the constraint form button before the
    // constraint is completed.
    constraint_complete_ = true;

    CommandHistory::instance().
      addCommand( new DeleteConstraintCommand( "delete constraint",
					       parent(),
					       current_view_->subassembly() ) );

    current_view_->subassembly()->constraints().cancelCurrent();
  }

  constraint_complete_ = false;

  current_view_->subassembly()->constraints().addMate();

  figureModified();

  CommandHistory::instance().
    addCommand( new CreateConstraintCommand( "create constraint",
					     parent(),
					     current_view_->subassembly() ) );
}

void AssemblyView::align ( void )
{
  if ( !constraint_complete_ ) {
    // Occurs if the user toggles the constraint form button before the
    // constraint is completed.
    constraint_complete_ = true;

    CommandHistory::instance().
      addCommand( new DeleteConstraintCommand( "delete constraint",
					       parent(),
					       current_view_->subassembly() ) );

    current_view_->subassembly()->constraints().cancelCurrent();
  }

  constraint_complete_ = false;

  current_view_->subassembly()->constraints().addAlign();

  figureModified();

  CommandHistory::instance().
    addCommand( new CreateConstraintCommand( "create constraint",
					     parent(),
					     current_view_->subassembly() ) );
}

void AssemblyView::mateOffset ( void )
{
  if ( !constraint_complete_ ) {
    // Occurs if the user toggles the constraint form button before the
    // constraint is completed.
    constraint_complete_ = true;

    CommandHistory::instance().
      addCommand( new DeleteConstraintCommand( "delete constraint",
					       parent(),
					       current_view_->subassembly() ) );

    current_view_->subassembly()->constraints().cancelCurrent();
  }

  constraint_complete_ = false;

  current_view_->subassembly()->constraints().addMateOffset();

  figureModified();

  CommandHistory::instance().
    addCommand( new CreateConstraintCommand( "create constraint",
					     parent(),
					     current_view_->subassembly() ) );
}

void AssemblyView::alignOffset ( void )
{
  if ( !constraint_complete_ ) {
    // Occurs if the user toggles the constraint form button before the
    // constraint is completed.
    constraint_complete_ = true;

    CommandHistory::instance().
      addCommand( new DeleteConstraintCommand( "delete constraint",
					       parent(),
					       current_view_->subassembly() ) );

    current_view_->subassembly()->constraints().cancelCurrent();
  }

  constraint_complete_ = false;

  current_view_->subassembly()->constraints().addAlignOffset();

  figureModified();

  CommandHistory::instance().
    addCommand( new CreateConstraintCommand( "create constraint",
					     parent(),
					     current_view_->subassembly() ) );
}

int AssemblyView::editOffset ( const QString& type, double& offset ) const
{
  offset_info_dialog_->getUi()->offsetLengthConstraint->setTitle( trC( type ) );
  offset_info_dialog_->getUi()->offsetLengthConstraint->
    setLengthLimits( UnitsBasis::instance()->lengthUnit(),
		     UnitsBasis::instance()->format(),
		     UnitsBasis::instance()->precision(),
		     lC::MINIMUM_DIMESION, lC::MAXIMUM_DIMENSION,
		     offset );

  int ret = offset_info_dialog_->exec();

  if ( ret != QDialog::Rejected )
    offset = offset_info_dialog_->getUi()->offsetLengthConstraint->specifiedLength();

  return ret;
}

void AssemblyView::placementComplete ( void )
{
  disconnect( current_view_, SIGNAL( destroyedSubassembly() ),
	      this, SLOT( subassemblyDestroyed() ) );

  disconnect( current_view_->subassembly(),
	      SIGNAL( constraintCreated( const AssemblyConstraint*) ),
	      this, SLOT( constraintAdded( const AssemblyConstraint* ) ) );

  disconnect( current_view_->subassembly(),
	      SIGNAL( constraintChanged( const AssemblyConstraint*,
					 const AssemblyConstraint* ) ),
	      this, SLOT( constraintModified( const AssemblyConstraint*,
					      const AssemblyConstraint* ) ) );
  disconnect( current_view_->subassembly(),
	      SIGNAL( constraintOffsetChanged( const AssemblyConstraint* ) ),
	      this, SLOT( constraintOffsetChanged( const AssemblyConstraint* ) ) );

  // Undo may take us back to the editing an existing subassembly.

  connect( current_view_->subassembly(),
	   SIGNAL( constraintChanged( const AssemblyConstraint*,
				      const AssemblyConstraint* ) ),
	   SLOT( reeditSubassembly( const AssemblyConstraint*,
				    const AssemblyConstraint* ) ) );
	   

  QAction* cancel_action = lCMW()->getUi()->cancelAddModelAction;

  cancel_action->disconnect();
  context_menu_->removeAction( cancel_action );
  context_menu_->removeAction( separator_id_ );

  disconnect( constraint_form_->getUi()->matePushButton, SIGNAL( clicked() ),
	      this, SLOT( mate() ) );
  disconnect( constraint_form_->getUi()->alignPushButton, SIGNAL( clicked() ),
	      this, SLOT( align() ) );
  disconnect( constraint_form_->getUi()->mateOffsetPushButton, SIGNAL( clicked() ),
	      this, SLOT( mateOffset() ) );
  disconnect( constraint_form_->getUi()->alignOffsetPushButton, SIGNAL( clicked() ),
	      this, SLOT( alignOffset() ) );
  disconnect( constraint_form_->getUi()->cancelPushButton, SIGNAL( clicked() ),
	      this, SLOT( cancelOperation() ) );

  constraint_dock_->hide();
}

// Restart the constraint editor for the subassembly (as after an undo).

void AssemblyView::reeditSubassembly ( const AssemblyConstraint* /*old_constraint*/,
				       const AssemblyConstraint* /*new_constraint*/)
{
  current_view_ = dynamic_cast<SubassemblyView*>( figureViews().last().get() );

  disconnect( current_view_->subassembly(),
	      SIGNAL( constraintChanged( const AssemblyConstraint*,
					 const AssemblyConstraint* ) ),
	      this, SLOT( reeditSubassembly( const AssemblyConstraint*,
					     const AssemblyConstraint* ) ) );
  disconnect( current_view_->subassembly(),
	      SIGNAL( constraintOffsetChanged( const AssemblyConstraint* ) ),
	      this, SLOT( constraintOffsetChanged( const AssemblyConstraint* ) ) );

  editConstraints( current_view_ );

  QListIterator<std::shared_ptr<AssemblyConstraint>> constraint =
    current_view_->subassembly()->constraints().constraints();

  while ( constraint.hasNext() ) {

    AssemblyConstraint* tmpAsc;
    constraints_text_.push_back( QStringList() );
#if 1
    if ( tmpAsc->type() == lC::STR::MATE_OFFSET ||
     tmpAsc->type() == lC::STR::ALIGN_OFFSET )
      constraints_text_.back() << QString( tr( "%1 (%2)" ) ).
    arg( tr( tmpAsc->type().toLatin1() ) ).
    arg( UnitsBasis::instance()->format( tmpAsc->offset(), false ) );
    else
      constraints_text_.back() << tr( tmpAsc->type().toLatin1() );
#else
    constraints_text_.back() << tr( tmpAsc->type() );
#endif
    if ( !tmpAsc->reference0().empty() )
      constraints_text_.back() << model()->idPath( tmpAsc->reference0() );

    if ( !tmpAsc->reference1().empty() )
      constraints_text_.back() << model()->idPath( tmpAsc->reference1() );
  }

  updateConstraintLabel();

  constraint.toBack();
  editConstraint( constraint.peekPrevious().get() );

  activateFigure( parent()->lookup( constraint.peekPrevious()->reference0() ) );
}

void AssemblyView::cancelConstraint ( void )
{
  if ( !constraints_text_.empty() ) {
    constraints_text_.pop_back();
    updateConstraintLabel();
  }

  disconnect( inputObject(), SIGNAL( done(bool) ), 0, 0 );

  SelectedNames none;
  highlightFigures( none );

  deactivateFigures();

  setInputObject( 0 );

  view()->updateGL();
}

void AssemblyView::subassemblyDestroyed ( void )
{
  if ( current_view_ != 0 ) {
    placementComplete();
    current_view_ = 0;
  }
}

void AssemblyView::updateHiddenModel ( void )
{
  //  cout << now() << name() << " updating hidden model view [start]" << endl;
  // Well, the assembly view has some serious work to do when the
  // view mode is hidden.
  const TopoDS_Compound& assembly = assembly_->compound();
  //  BRepTools::Dump( assembly, cout );
  hidden_drawer_->setAssembly( assembly );
  //  cout << now() << name() << " updating hidden model view [end]" << endl;
}

void AssemblyView::updateHiddenOrientation ( const GLdouble* modelview )
{
  // Well, the assembly view has some serious work to do when the
  // view mode is hidden.
  hidden_drawer_->setOrientation( modelview );
}

// Let the user pick a model to delete from the assembly.

void AssemblyView::deleteModel ( void )
{
  deactivateFigures();

  model_delete_input_->startDisplay( context_menu_ );

  setInputObject( model_delete_input_ );

  emit newInformation( tr( "Delete model" ) );
}

// Slighlty different actions if delete is canceled.

void AssemblyView::cancelDeleteOperation ( void )
{
  SelectedNames none;
  highlightFigures( none );

  deactivateFigures();

  view()->unsetCursor();

  emit newInformation( trC( lC::STR::NONE ) );
}

/*!
 * Instance of assembly metadata for a part.
 */
class AssemblyViewMetadata : public PageMetadata {
public:
  /*!
   * Construct a (what amounts to a const) assembly metadata object.
   */
  AssemblyViewMetadata ( void )
    : PageMetadata( lC::STR::ASSEMBLY,
		    "new_assembly.png",
		    QT_TRANSLATE_NOOP( "lignumCADMainWindow", "&Assembly" ),
		    "&Assembly...",
		    "Insert a new Assembly",
		    "<p><b>Insert|Assembly</b></p><p>Click this button to create a new assembly. An assembly is a collection of parts and subassemblies which can be manipulated as a group.</p>" )
  {
    //    cout << "Constructing assembly metadata" << endl;
    PageFactory::instance()->addPageMetadata( Assembly::PAGE_ID, this );
  }

  /*!
   * Create a new assembly and its view.
   * \return view handle for the assembly and its view.
   */
  AssemblyView* create ( DesignBookView* parent ) const
  {
    QString name = parent->uniqueName( &Assembly::newName, lC::STR::ASSEMBLY );

    Assembly* assembly = new Assembly( parent->model()->newID(), name,
				       parent->model() ); 
    AssemblyView* assembly_view = new AssemblyView( assembly, parent );
    parent->addPageView( assembly_view );
    return assembly_view;
  }

  /*!
   * Create an assembly from its XML representation.
   * \return handle for the assembly.
   */
  Assembly* create ( Model* parent, const QDomElement& xml_rep ) const
  {
    uint assembly_id = xml_rep.attribute( lC::STR::ID ).toUInt();
    return new Assembly( assembly_id, xml_rep, parent );
  }

  /*!
   * Create a assembly view from its XML representation.
   * \return handle for the assembly view.
   */
  AssemblyView* create ( DesignBookView* parent, const QDomElement& xml_rep ) const
  {
    AssemblyView* assembly_view = new AssemblyView( parent, xml_rep );
    parent->addPageView( assembly_view );
    return assembly_view;
  }

  /*!
   * Retrieve the QAction which can create this page.
   * \param lCMW pointer to the main window where the actions are stored.
   * \return the action associated with creating an instance of this page.
   */
  QAction* action ( lignumCADMainWindow* lCMW ) const
  {
    return lCMW->getUi()->insertAssemblyAction;
  }
};

AssemblyViewMetadata assembly_view_metadata;
