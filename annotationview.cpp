/*
 * annotationview.cpp
 *
 * Annotation View classes
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

#include <qaction.h>
#include <QMenu>
#include <qlineedit.h>
#include <qtextedit.h>
#include <qcursor.h>
#include <qpainter.h>
#include <qrect.h>

#include "ui_lignumcadmainwindow.h"
#include "command.h"
#include "cursorfactory.h"
#include "page.h"
#include "pageview.h"
#include "designbookview.h"
#include "model.h"
#include "openglview.h"
#include "listviewitem.h"
#include "oglpaintdevice.h"
#include "units.h"
#include "annotation.h"
#include "annotationinfodialog.h"
#include "annotationview.h"

namespace Space2D {
  /*!
   * Internal class which implements a memento of the annotation.
   */
  class AnnotationViewCreate : public CreateObject {
    //! When CommandHistory undoes or redoes the creation of this object,
    //! the Design Book view is the only handle which is required. Everything
    //! else can be looked up.
    DesignBookView* design_book_view_;
    //! The details of the annotation and view are stored as XML.
    QDomDocument xml_doc_;
  public:
  /*!
   * Create a memento of the current state of the annotation and its view.
   * \param annotation_view view object of the annotation.
   */
    AnnotationViewCreate ( AnnotationView* annotation_view )
    {
      design_book_view_ = annotation_view->parent()->parent();

      QDomElement root = xml_doc_.createElement( lC::STR::MEMENTO );

      root.setAttribute( lC::STR::NAME, annotation_view->dbURL().toString(true) );

      annotation_view->annotation()->write( root );
      annotation_view->write( root );

      xml_doc_.appendChild( root );
    }
    //! Destructor doesn't do much.
    ~AnnotationViewCreate ( void ) {}
    /*!
     * (Re)create the annotation object and its view as specified in
     * the XML representation.
     */
    void create ( void )
    {
      QDomNodeList memento_list = xml_doc_.elementsByTagName( lC::STR::MEMENTO );

      if ( memento_list.length() > 0 ) {
	DBURL url = memento_list.item(0).toElement().attribute( lC::STR::NAME );

	// First, we have to find the parent page and view for the annotation.

	DBURL parent = url.parent();

	Page* page =
	  dynamic_cast<Page*>( design_book_view_->model()->lookup( parent ) );
	PageView* page_view =
	  dynamic_cast<PageView*>( design_book_view_->lookup( parent ) );

	// Now, we can extract the XML representations for the annotation
	// and its view
	QDomNodeList annotation_list =
	  xml_doc_.elementsByTagName( lC::STR::ANNOTATION );

	if ( annotation_list.length() > 0 ) {

	  QDomNodeList annotation_view_list =
	    xml_doc_.elementsByTagName( lC::STR::ANNOTATION_VIEW );

	  if ( annotation_view_list.length() > 0 ) {
	    uint annotation_id =
	      annotation_list.item(0).toElement().attribute( lC::STR::ID ).toUInt();

	    new Annotation( annotation_id, annotation_list.item(0).toElement(),
			    page );

	    page_view->
	      addFigureView( new AnnotationView( annotation_view_list.
						 item(0).toElement(), page_view ) );
	  }
	}
      }
    }
    /*!
     * Delete the annotation object and its view as specified in the
     * XML representation.
     */
    void remove ( void )
    {
      QDomNodeList memento_list = xml_doc_.elementsByTagName( lC::STR::MEMENTO );

      if ( memento_list.length() > 0 ) {
	DBURL url = memento_list.item(0).toElement().attribute( lC::STR::NAME );

	AnnotationView* annotation_view =
	  dynamic_cast<AnnotationView*>( design_book_view_->lookup( url ) );

	DBURL parent = url.parent();

	PageView* page_view =
	  dynamic_cast<PageView*>( design_book_view_->lookup( parent ) );

	page_view->removeFigureView( annotation_view );
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

      QDomNodeList annotation_list =
	xml_doc_.elementsByTagName( lC::STR::ANNOTATION );

      if ( annotation_list.length() > 0 ) {
	QDomNode element = xml_rep->importNode( annotation_list.item(0), true );

	rep_root.appendChild( element );
      }

      QDomNodeList annotation_view_list =
	xml_doc_.elementsByTagName( lC::STR::ANNOTATION_VIEW );

      if ( annotation_view_list.length() > 0 ) {
	QDomNode element = xml_rep->importNode( annotation_view_list.item(0),
						true );
	rep_root.appendChild( element );
      }

      QDomNodeList constraint_list =
	xml_doc_.elementsByTagName( lC::STR::CONSTRAINTS );

      if ( constraint_list.length() > 0 ) {
	QDomNode element = xml_rep->importNode( constraint_list.item(0), true );

	rep_root.appendChild( element );
      }

      QDomNodeList unconstraint_list =
	xml_doc_.elementsByTagName( lC::STR::UNCONSTRAINTS );

      if ( unconstraint_list.length() > 0 ) {
	QDomNode element = xml_rep->importNode( unconstraint_list.item(0), true );

	rep_root.appendChild( element );
      }
    }
    /*!
     * If the creation of a annotation is immediately followed by a rename,
     * merge this into the representation rather than having a separate
     * undo step.
     * \param rename the rename command.
     * \return true if the rename command applied to this object.
     */
    bool mergeRename ( RenameCommand* rename )
    {
      // First, make sure the rename refers to this object. Start by extracting
      // the old path.
      QDomNodeList memento_list = xml_doc_.elementsByTagName( lC::STR::MEMENTO );

      if ( memento_list.length() > 0 ) {
	QString path = memento_list.item(0).toElement().attribute( lC::STR::NAME );

	if ( path != rename->oldDBURL().toString(true) )
	  return false;

	// Additional sanity check: make sure the object and its view have elements.

	QDomNodeList annotation_list =
	  xml_doc_.elementsByTagName( lC::STR::ANNOTATION );
	QDomNodeList annotation_view_list =
	  xml_doc_.elementsByTagName( lC::STR::ANNOTATION_VIEW );

	if ( annotation_list.length() == 0 ||
	     annotation_view_list.length() == 0 )
	  return false;

	// Update the name elements in the object and it's view.

	memento_list.item(0).toElement().setAttribute( lC::STR::NAME,
					       rename->newDBURL().toString(true) );

	annotation_list.item(0).toElement().
	  setAttribute( lC::STR::NAME, rename->newDBURL().name() );

	annotation_view_list.item(0).toElement().setAttribute( lC::STR::ANNOTATION,
					       rename->newDBURL().toString(true) );

	return true;
      }

      return false;
    }
  };

  /*
   * Construct a creation input object for an annotation.
   */
  AnnotationCreateInput::AnnotationCreateInput ( Annotation* annotation,
					       AnnotationView* annotation_view )
    : annotation_( annotation ), annotation_view_( annotation_view )
  {}

  /*
   * Set up UI for creating an annotation.
   */
  void AnnotationCreateInput::startDisplay ( QPopupMenu* /*context_menu*/ )
  {
    QAction* cancel_action =
      annotation_view_->parent()->lCMW()->cancelAnnotationAction;

    separator_id_ = annotation_view_->view()->contextMenu()->insertSeparator();
    cancel_action->addTo( annotation_view_->view()->contextMenu() );
    connect( cancel_action, SIGNAL( activated() ), SLOT( cancelOperation() )  );

    annotation_view_->view()->
      setCursor( CursorFactory::instance().cursor( CursorFactory::ANNOTATION ) );
  }

  /*
   * Initialize the annotation's origin.
   */
  void AnnotationCreateInput::mousePress ( QMouseEvent* me,
					   const SelectedNames& /*selected*/ )
  {
    Point origin = annotation_view_->view()->unproject2D( me->pos() );
    // Hmm. I guess I don't see any point in this...
    //    UnitsBasis::instance()->round( origin );
    annotation_->initialize( origin );
  }

  /*
   * Resize the annotation text box.
   */
  void AnnotationCreateInput::mouseDrag ( QMouseEvent* me,
					  const SelectedNames& /*selected*/ )
  {
    Point corner = annotation_view_->view()->unproject2D( me->pos() );
    // Hmm. I guess I don't see any point in this...
    //    UnitsBasis::instance()->round( corner );
    annotation_view_->moveCorner( corner );
  }

  /*
   * Complete the annotation text box and put up the text editing dialog.
   */
  bool AnnotationCreateInput::mouseRelease ( QMouseEvent* me,
					     const SelectedNames& /*selected*/ )
  {
    Point corner = annotation_view_->view()->unproject2D( me->pos() );
    // Hmm. I guess I don't see any point in this...
    //    UnitsBasis::instance()->round( corner );
    annotation_view_->moveCorner( corner );

    int ret = annotation_view_->editAnnotationInformation();

    if ( ret == QDialog::Rejected ) {
      cancelOperation();
      return true;
    }

    annotation_->finish();

    PageView* page_view = annotation_view_->parent();

    QAction* cancel_action = page_view->lCMW()->cancelAnnotationAction;
    cancel_action->disconnect();
    cancel_action->removeFrom( annotation_view_->view()->contextMenu() );
    annotation_view_->view()->contextMenu()->removeItem( separator_id_ );

    annotation_view_->unsetCursor();

    annotation_view_->parent()->figureComplete();

    annotation_view_->parent()->activateFigure( annotation_view_ );

    CommandHistory::instance().
      addCommand( new CreateCommand( "create annotation",
				     annotation_view_->memento() ) );

    return false;
  }

  /*
   * Cancel the operation on the Escape key.
   */
  void AnnotationCreateInput::keyPress ( QKeyEvent* ke )
  {
    if ( ke->key() == Qt::Key_Escape )
      cancelOperation();
    else
      ke->ignore();
  }

  /*
   * Cancel the annotation creation.
   */
  void AnnotationCreateInput::cancelOperation ( void )
  {
    PageView* page_view = annotation_view_->parent();

    QAction* cancel_action = page_view->lCMW()->cancelAnnotationAction;

    cancel_action->disconnect();
    cancel_action->removeFrom( annotation_view_->view()->contextMenu() );
    annotation_view_->view()->contextMenu()->removeItem( separator_id_ );

    page_view->removeFigureView( annotation_view_ );

    page_view->cancelOperation();
  }

  /*!
   * Internal class that handles the undo/redo command for moveing/resizing
   * the annotation text box.
   */
  class MoveAnnotationCommand : public Command {
  public:
    /*!
     * Construct a command to undo/redo a move/resize of an annotation view.
     * \param name name of command.
     * \param design_book_view reference to Design Book View.
     * \param db_url reference to annotation object.
     * \param old_origin the old origin of the annotation (in model units)
     * \param old_size the old size of the annotation box (in paper units)
     * \param new_origin the new origin of the annotation (in model units)
     * \param new_size the new size of the annotation box (in paper units)
     */
    MoveAnnotationCommand ( const QString& name,
						   DesignBookView* design_book_view,
						   const DBURL& db_url,
						   Point old_origin,
						   Vector old_size,
						   Point new_origin,
						   Vector new_size )
      : Command( name ), design_book_view_( design_book_view ),
	db_url_( db_url ), old_origin_( old_origin ),
	old_size_( old_size ), new_origin_( new_origin ), new_size_( new_size )
    {}
    //! Delete the move annotation undo/redo command.
    ~MoveAnnotationCommand ( void )
    {}
    //! \return command type.
    Type type ( void ) const { return MoveAnnotation; }
    //! \return if possible, merge the argument command into this command.
    bool merge ( Command* /*command*/ ) { return false; }
    //! (Re)execute the command (i.e., redo).
    void execute ( void )
    {
      Model* model = design_book_view_->model();
      Annotation* annotation = dynamic_cast<Annotation*>( model->lookup(db_url_));
      annotation->setOrigin( new_origin_ );
      AnnotationView* annotation_view =
	dynamic_cast<AnnotationView*>( design_book_view_->lookup( db_url_ ) );
      annotation_view->setSize( new_size_ );
    }
    //! Unexecute the command (i.e., undo).
    void unexecute ( void )
    {
      Model* model = design_book_view_->model();
      Annotation* annotation = dynamic_cast<Annotation*>( model->lookup(db_url_));
      annotation->setOrigin( old_origin_ );
      AnnotationView* annotation_view =
	dynamic_cast<AnnotationView*>( design_book_view_->lookup( db_url_ ) );
      annotation_view->setSize( old_size_ );
    }
    /*!
     * Serialize this command.
     * \param document XML representation to append this command to.
     */
    void write ( QDomDocument* document ) const
    {
      QDomElement move_element = document->createElement( lC::STR::MOVE_ANNOTATION);
      QDomNode root_node = document->firstChild();
      if ( !root_node.isNull() )
	document->replaceChild( move_element, root_node );
      else
	document->appendChild( move_element );
      move_element.setAttribute( lC::STR::URL, db_url_ );
      move_element.setAttribute( lC::STR::OLD_X, lC::format( old_origin_[X] ) );
      move_element.setAttribute( lC::STR::OLD_Y, lC::format( old_origin_[Y] ) );
      move_element.setAttribute( lC::STR::OLD_WIDTH, lC::format( old_size_[X] ) );
      move_element.setAttribute( lC::STR::OLD_HEIGHT, lC::format( old_size_[Y] ) );
      move_element.setAttribute( lC::STR::NEW_X, lC::format( new_origin_[X] ) );
      move_element.setAttribute( lC::STR::NEW_Y, lC::format( new_origin_[Y] ) );
      move_element.setAttribute( lC::STR::NEW_WIDTH, lC::format( new_size_[X] ) );
      move_element.setAttribute( lC::STR::NEW_HEIGHT, lC::format( new_size_[Y] ) );
    }
  private:
    //! Design Book view containing this annotation.
    DesignBookView* design_book_view_;
    //! Reference of annotation.
    DBURL db_url_;
    //! Old annotation origin (in model units)
    Point old_origin_;
    //! Old annotation box size (in paper units)
    Vector old_size_;
    //! new annotation origin (in model units)
    Point new_origin_;
    //! New annotation box size (in paper units)
    Vector new_size_;
  };
  /*!
   * Construct an input object for modifying an annotation.
   * \param annotation reference to the annotation.
   * \param annotation_view reference to the annotation's view.
   */
  AnnotationModifyInput::AnnotationModifyInput ( Annotation* annotation,
						 AnnotationView* annotation_view )
    : selection_type_( TRANSIENT, FIGURE ),
      annotation_( annotation ), annotation_view_( annotation_view ), xml_rep_( 0 )
  {}
  /*!
   * Accept the scanning mouse position.
   * \param me mouse position.
   * \param selected selected name records.
   */
  void AnnotationModifyInput::mousePrepress ( QMouseEvent* me,
					      const SelectedNames& selected )
  {
    if ( selected.empty() ) {
      annotation_view_->unsetCursor();
      return;
    }

    SelectedNames::const_iterator f;

    for ( f = selected.begin(); f != selected.end(); ++f ) {

      if ( (*f).second[0] == annotation_view_->selectionName() ) {

	if ( (*f).second.size() > 1 ) {
	  GLuint g = (*f).second[1]; // (*(*f).second->rbegin()).first;

	  if ( annotation_view_->isGeometry( g ) ) {

	    annotation_view_->setCursor( g );

	    me->accept();
	  }
	  else {
	    annotation_view_->unsetCursor();
	  }

	  return; // Probably not the last word here
	}
      }
    }
    annotation_view_->unsetCursor();
  }

  void AnnotationModifyInput::mousePress ( QMouseEvent* me,
					  const SelectedNames& selected )
  {
    // Let the Page handle these events

    if ( me->button() != Qt::LeftButton || selected.empty() ) {
      me->ignore();
      return;
    }

    SelectedNames::const_iterator f;

    for ( f = selected.begin(); f != selected.end(); ++f ) {

      if ( (*f).second[0] == annotation_view_->selectionName() ) {

	if ( (*f).second.size() > 1 ) {
	  GLuint g = (*f).second[1]; // (*(*f).second->rbegin()).first;

	  if ( annotation_view_->isGeometry( g ) ) {
	    start_pnt_ = annotation_view_->view()->unproject2D( me->pos() );
	    // Hmm. I guess I don't see any point in this...
	    //	  UnitsBasis::instance()->round( start_pnt_ );
	    last_pnt_ = start_pnt_;

	    drag_handle_ = g;

	    old_origin_ = annotation_->origin();
	    old_size_ = annotation_view_->size();

	    selection_type_ = SelectionType( TRANSIENT, NONE );

	    return;
	  }
	}
      }
    }

    me->ignore();
  }

  void AnnotationModifyInput::mouseDrag ( QMouseEvent* me,
					  const SelectedNames& /*selected*/ )
  {
    if ( drag_handle_ == 0 ) return;

    current_pnt_ = annotation_view_->view()->unproject2D( me->pos() );
    // Hmm. I guess I don't see any point in this...
    //    UnitsBasis::instance()->round( current_pnt_ );

    lC::ValidDelta ok = annotation_view_->adjust( drag_handle_,
						  last_pnt_, current_pnt_);
    //    last_pnt_ = current_pnt_;

    if ( ok.dx_ok_ )
      last_pnt_[X] = current_pnt_[X];
    if ( ok.dy_ok_ )
      last_pnt_[Y] = current_pnt_[Y];

    annotation_view_->parent()->figureModified();
  }

  bool AnnotationModifyInput::mouseRelease ( QMouseEvent* /*me*/,
					     const SelectedNames& /*selected*/ )
  {
    if ( last_pnt_ != start_pnt_ ) {
      MoveAnnotationCommand* command =
	new MoveAnnotationCommand( "move annotation",
				   annotation_view_->parent()->parent(),
				   annotation_->dbURL(),
				   old_origin_, old_size_,
				   annotation_->origin(), annotation_view_->size());

      CommandHistory::instance().addCommand( command );
    }

    selection_type_ = SelectionType( TRANSIENT, FIGURE );

    drag_handle_ = 0;
    // Might do a sanity check comparing to the original point (?)
    //  annotation_view_->unsetCursor(); //
    // Continue in this mode until unselected
    return false;
  }

  void AnnotationModifyInput::mouseDoubleClick ( QMouseEvent* /*me*/ )
  {
    annotation_view_->editInformation();
  }

  void AnnotationModifyInput::setAnnotation ( Annotation* annotation )
  {
    annotation_ = annotation;
  }

  class ChangeAnnotationCommand : public Command {
    Model* model_;
    DBURL db_url_;
    QString old_text_;
    QString new_text_;

  public:
    ChangeAnnotationCommand ( const QString& name, Model* model,const DBURL& db_url,
			      QString old_text, QString new_text )
      : Command( name ), model_( model ), db_url_( db_url ),
	old_text_( old_text ), new_text_( new_text )
    {}

    ~ChangeAnnotationCommand ( void )
    {}

    Type type ( void ) const { return ChangeAnnotation; }

    bool merge ( Command* /*command*/ ) { return false; }

    void execute ( void )
    {
      Annotation* annotation = dynamic_cast<Annotation*>( model_->lookup(db_url_));
      annotation->setText( new_text_ );
    }

    void unexecute ( void )
    {
      Annotation* annotation = dynamic_cast<Annotation*>( model_->lookup(db_url_));
      annotation->setText( old_text_ );
    }

    void write ( QDomDocument* document ) const
    {
      QDomElement change_element =
	document->createElement( lC::STR::CHANGE_ANNOTATION );

      QDomNode root_node = document->firstChild();
      if ( !root_node.isNull() )
	document->replaceChild( change_element, root_node );
      else
	document->appendChild( change_element );

      change_element.setAttribute( lC::STR::URL, db_url_ );
      change_element.setAttribute( lC::STR::OLD_TEXT, old_text_ );
      change_element.setAttribute( lC::STR::NEW_TEXT, new_text_ );
    }
  };

  AnnotationInfoDialog* AnnotationView::annotation_info_dialog_ = 0;

  AnnotationView::AnnotationView ( Annotation* annotation, PageView* parent )
    : FigureView( parent ), annotation_( annotation ),
      create_input_( annotation, this ), modify_input_( annotation, this )
  {
    init();
  }

  AnnotationView::AnnotationView ( const QDomElement& xml_rep, PageView* parent )
    : FigureView( parent ), annotation_( 0 ),
      create_input_( 0, this ), modify_input_( 0, this )
  {
    DBURL db_url = xml_rep.attribute( lC::STR::ANNOTATION );
    size_ = Vector( xml_rep.attribute( lC::STR::WIDTH ).toDouble(),
		    xml_rep.attribute( lC::STR::HEIGHT ).toDouble() );

    annotation_ =
      dynamic_cast< Annotation* >( model()->lookup( db_url ) );

    init();

    list_view_item_->setText( lC::DETAIL,
			     trC( lC::STR::RECTANGLE_FMT ).
			     arg( annotation_->origin()[X] ).
			     arg( annotation_->origin()[Y] ).
			     arg( width() ).
			     arg( height() ) );

    modify_input_.setAnnotation( annotation_ );
  }

  AnnotationView::AnnotationView ( const QString& name, const QDomElement& xml_rep,
				   Page* page, PageView* parent )
    : FigureView( parent ), annotation_( 0 ),
      create_input_( 0, this ), modify_input_( 0, this )
  {
    QDomDocument doc = xml_rep.ownerDocument();
    QDomNodeList annotation_list = doc.elementsByTagName( lC::STR::ANNOTATION );
    // Well, failure is not really an option...but it could happen if
    // somebody put a partial XML document into the clipboard...
    // In general, the failure modes of lignumCAD need substantially more
    // consideration......
    if ( annotation_list.count() > 0 )
      annotation_ = new Annotation( page->newID(), name,
				    annotation_list.item(0).toElement(), page );

    size_ = Vector( xml_rep.attribute( lC::STR::WIDTH ).toDouble(),
		    xml_rep.attribute( lC::STR::HEIGHT ).toDouble() );

    init();

    list_view_item_->setText( lC::DETAIL,
			     trC( lC::STR::RECTANGLE_FMT ).
			     arg( annotation_->origin()[X] ).
			     arg( annotation_->origin()[Y] ).
			     arg( width() ).
			     arg( height() ) );

    modify_input_.setAnnotation( annotation_ );
  }

  AnnotationView::~AnnotationView ( void )
  {
    if ( annotation_ )
      annotation_->parent()->removeFigure( annotation_ );

    delete ogl_paintdevice_;

    delete list_view_item_;
  }

  /*!
   * Decide whether the annotation or the dimension is being
   * manipulated and return the responsible controller
   */
  InputObject* AnnotationView::modifyInput ( void )
  {
    if ( isActivated() )
      return &modify_input_;

    return 0;
  }

  void AnnotationView::init ( void )
  {
    QObject::setName( annotation_->name() + lC::STR::VIEW_EXT );

    ogl_paintdevice_ = new OGLPaintDevice( view() );

    left_edge_.setSelectionName( view()->genSelectionName() );
    right_edge_.setSelectionName( view()->genSelectionName() );
    bottom_edge_.setSelectionName( view()->genSelectionName() );
    top_edge_.setSelectionName( view()->genSelectionName() );

    left_edge_.setCursorShape( Qt::SizeAllCursor );
    right_edge_.setCursorShape( Qt::SizeAllCursor );
    bottom_edge_.setCursorShape( Qt::SizeAllCursor );
    top_edge_.setCursorShape( Qt::SizeAllCursor );

    solid_annotation_.setSelectionName( view()->genSelectionName() );

    solid_annotation_.setCursorShape( Qt::SizeAllCursor );

    lb_handle_.setSelectionName( view()->genSelectionName() );
    mb_handle_.setSelectionName( view()->genSelectionName() );
    rb_handle_.setSelectionName( view()->genSelectionName() );
    mr_handle_.setSelectionName( view()->genSelectionName() );
    rt_handle_.setSelectionName( view()->genSelectionName() );
    mt_handle_.setSelectionName( view()->genSelectionName() );
    lt_handle_.setSelectionName( view()->genSelectionName() );
    ml_handle_.setSelectionName( view()->genSelectionName() );

    lb_handle_.setCursorShape( Qt::SizeBDiagCursor );
    mb_handle_.setCursorShape( Qt::SizeVerCursor );
    rb_handle_.setCursorShape( Qt::SizeFDiagCursor );
    mr_handle_.setCursorShape( Qt::SizeHorCursor );
    rt_handle_.setCursorShape( Qt::SizeBDiagCursor );
    mt_handle_.setCursorShape( Qt::SizeVerCursor );
    lt_handle_.setCursorShape( Qt::SizeFDiagCursor );
    ml_handle_.setCursorShape( Qt::SizeHorCursor );

    // Reverse lookup of GL selection name to object
    annotation_objects_.insert( left_edge_.selectionName(), &left_edge_ );
    annotation_objects_.insert( right_edge_.selectionName(), &right_edge_ );
    annotation_objects_.insert( bottom_edge_.selectionName(), &bottom_edge_ );
    annotation_objects_.insert( top_edge_.selectionName(), &top_edge_ );
    annotation_objects_.insert( solid_annotation_.selectionName(),
				&solid_annotation_ );
    annotation_objects_.insert( lb_handle_.selectionName(), &lb_handle_ );
    annotation_objects_.insert( mb_handle_.selectionName(), &mb_handle_ );
    annotation_objects_.insert( rb_handle_.selectionName(), &rb_handle_ );
    annotation_objects_.insert( mr_handle_.selectionName(), &mr_handle_ );
    annotation_objects_.insert( rt_handle_.selectionName(), &rt_handle_ );
    annotation_objects_.insert( mt_handle_.selectionName(), &mt_handle_ );
    annotation_objects_.insert( lt_handle_.selectionName(), &lt_handle_ );
    annotation_objects_.insert( ml_handle_.selectionName(), &ml_handle_ );

    adjustments_[left_edge_.selectionName()] = &AnnotationView::move;
    adjustments_[right_edge_.selectionName()] = &AnnotationView::move;
    adjustments_[bottom_edge_.selectionName()] = &AnnotationView::move;
    adjustments_[top_edge_.selectionName()] = &AnnotationView::move;

    adjustments_[solid_annotation_.selectionName()] = &AnnotationView::move;

    adjustments_[ml_handle_.selectionName()] = &AnnotationView::resizeLeft;
    adjustments_[mr_handle_.selectionName()] = &AnnotationView::resizeRight;
    adjustments_[mb_handle_.selectionName()] = &AnnotationView::resizeBottom;
    adjustments_[mt_handle_.selectionName()] = &AnnotationView::resizeTop;

    adjustments_[lb_handle_.selectionName()] = &AnnotationView::resizeLeftBottom;
    adjustments_[rb_handle_.selectionName()] = &AnnotationView::resizeRightBottom;
    adjustments_[rt_handle_.selectionName()] = &AnnotationView::resizeRightTop;
    adjustments_[lt_handle_.selectionName()] = &AnnotationView::resizeLeftTop;

    if ( annotation_info_dialog_ == 0 )
      annotation_info_dialog_ = new AnnotationInfoDialog( parent()->lCMW() );

    QListViewItem* previous_item = parent()->previousItem( parent()->listViewItem(),
							   annotation_->id() );

    list_view_item_ = new ListViewItem( parent()->listViewItem(), previous_item );

    list_view_item_->setText( lC::NAME, lC::formatName( annotation_->name() )
			      + QString( " <%1>" ).arg( annotation_->id() ) );
    list_view_item_->setText( lC::TYPE, trC( lC::STR::ANNOTATION ) );
    list_view_item_->setText( lC::DETAIL, trC( lC::STR::UNDEFINED ) );
    list_view_item_->setOpen( true );
    list_view_item_->setRenameEnabled( lC::NAME, true );

    list_view_item_->listView()->ensureItemVisible( list_view_item_ );

    connect( list_view_item_, SIGNAL( nameChanged( const QString& ) ),
	     SLOT( listNameChanged( const QString& ) ) );

    connect( annotation_, SIGNAL( initialized() ), SLOT( annotationInitialized()));
    connect( annotation_, SIGNAL( moved() ), SLOT( annotationMoved() ) );

    connect( annotation_, SIGNAL( nameChanged( const QString& ) ),
	     SLOT( updateName( const QString& ) ) );
  }

  CreateObject* AnnotationView::memento ( void )
  {
    return new AnnotationViewCreate( this );
  }

  // Implementation of FigureView interface
  uint AnnotationView::id ( void ) const
  {
    return annotation_->id();
  }

  QString AnnotationView::name ( void ) const
  {
    return annotation_->name();
  }

  QString AnnotationView::type ( void ) const
  {
    return lC::STR::ANNOTATION;
  }

  DBURL AnnotationView::dbURL ( void ) const
  {
    return annotation_->dbURL();
  }

  QString AnnotationView::selectionText ( const std::vector<GLuint>& /*selection_name*/,
					  SelectionEntity /*entity*/ ) const
  {
    return annotation_->name();
  }


  void AnnotationView::setHighlighted ( bool highlight, SelectionEntity entity,
					const std::vector<GLuint>& /*items*/ )
  {
    if ( entity == FIGURE )
      FigureViewBase::setHighlighted( highlight );
  }

  void AnnotationView::setActivated ( bool activate, SelectionEntity entity,
				      const std::vector<GLuint>& /*items*/ )
  {
    if ( entity == FIGURE )
      FigureViewBase::setActivated( activate );
  }

  /*!
   * Determine if this selection is part of the geometry representation;
   * this includes the handles if they happen to be highlighted, too.
   * The purpose here is really just to distinguish between the
   * lines and the dimension views.
   */
  bool AnnotationView::isGeometry ( GLuint selection_name ) const
  {
    QIntDictIterator< GraphicsView > igv( annotation_objects_ );

    for (; igv.current(); ++igv )
      if ( (GLuint)igv.currentKey() == selection_name ) return true;

    return false;
  }

  void AnnotationView::editInformation ( void )
  {
    if ( FigureViewBase::isActivated() ) {
      editAnnotationInformation();
    }
  }

  int AnnotationView::editAnnotationInformation ( void )
  {
    annotation_info_dialog_->nameEdit->
      setText( lC::formatName( annotation_->name() ) );

    annotation_info_dialog_->annotationTextEdit->setText( annotation_->text() );
  redo:
    int ret = annotation_info_dialog_->exec();

    if ( ret == QDialog::Rejected ) return ret;

    bool modified = false;

    // Cannot let the user choose a name which is already used for an object
    // of this type on this page.
    if ( annotation_info_dialog_->nameEdit->edited() ) {
      // Pageview handles checking the name and putting up the error dialog
      // if necessary.
      int ret = parent()->uniqueFigureName( this,
					  annotation_info_dialog_->nameEdit->text(),
					    annotation_->type() );

      switch ( ret ) {
      case lC::Rejected:
	return QDialog::Rejected;
      case lC::Redo:
	annotation_info_dialog_->nameEdit->
	  setText( lC::formatName( annotation_->name() ) );
	goto redo;
      }
    }

    if ( annotation_info_dialog_->annotationTextEdit->isModified() ) {
      // If this a change applied after the annotation is created,
      // then we need to record this as an undoable modification.
      if ( annotation_->isComplete() ) {
	ChangeAnnotationCommand* change_command =
	  new ChangeAnnotationCommand( "change annotation", model(),
				       annotation_->dbURL(),
				       annotation_->text(),
		       annotation_info_dialog_->annotationTextEdit->text() );

	CommandHistory::instance().addCommand( change_command );

      }
      annotation_->setText( annotation_info_dialog_->annotationTextEdit->text() );
      modified = true;
    }

    // Renaming the annotation itself needs to follow the annotation
    // change command so that the rename is merged with the change
    // command (if there was one).

    if ( annotation_info_dialog_->nameEdit->edited() ) {
      // Again, only if this is not the initial activation of this
      // dialog do we want to create an undo-able rename.
      if ( annotation_->isComplete() )
	setName( annotation_info_dialog_->nameEdit->text() );
      else
	annotation_->setName( annotation_info_dialog_->nameEdit->text() );
      modified = true;
    }

    if ( modified ) parent()->figureModified();

    return ret;
  }

  void AnnotationView::draw ( void ) const
  {
    if ( !annotation_->isValid() ) return;

    // If highlighted, draw an "X" at the origin and a box around the text.

    if ( isHighlighted() || isActivated() ) {
      glColor3ubv( lC::qCubv( view()->constraintPrimaryColor()));

      double size = 1./16 * view()->scale();
      double gap = size / 2.;

      glBegin( GL_LINES );

      Point p;
      p = annotation_->origin() + Vector( gap, gap );
      glVertex2dv( p );
      p += Vector( size, size );
      glVertex2dv( p );

      p = annotation_->origin() + Vector( -gap, gap );
      glVertex2dv( p );
      p += Vector( -size, size );
      glVertex2dv( p );

      p = annotation_->origin() + Vector( -gap, -gap );
      glVertex2dv( p );
      p += Vector( -size, -size );
      glVertex2dv( p );

      p = annotation_->origin() + Vector( gap, -gap );
      glVertex2dv( p );
      p += Vector( size, -size );
      glVertex2dv( p );

      glEnd();
    }

    // In addition to the usual times, draw this as the text box is
    // being constructed, too. And, when ever there isn't any text
    // at all!

    if ( isHighlighted() || isActivated() || !annotation_->isComplete()
	 || annotation_->text().isEmpty() ) {
      QColor color = view()->annotationColor();
      if ( isHighlighted() || isActivated() )
	color = color.light();

      glEnable( GL_LINE_STIPPLE );
      glLineStipple( 1, 0xaaaa );
      // That is, draw the glRect below as a line
      glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

      glColor3ubv( lC::qCubv( color ) );

      glRectdv( lbVertex(), rtVertex() );

      glDisable( GL_LINE_STIPPLE );
    }

    // Draw the main annotation itself.

    if ( !annotation_->text().isEmpty() ) {
      ogl_paintdevice_->setView( view() );

      QPainter painter( ogl_paintdevice_ );

      QFont font;

      if ( !view()->annotationFont().isEmpty() )
	font.fromString( view()->annotationFont() );

      QSimpleRichText text( annotation_->text(), font );
      QColorGroup cg( parent()->parent()->appPalette() );

      if ( isHighlighted() || isActivated() )
	cg.setColor( QColorGroup::Text, view()->annotationColor().light() );
      else
	cg.setColor( QColorGroup::Text, view()->annotationColor() );

      // ***** NEEDS A RADICAL CHANGE IN THE MODELVIEW MATRIX **** //
      QRect clip = view()->newWindow( ltVertex(), size_ );

      // Note: after this, use width() as the limit on resizing the
      // text box...
      text.setWidth( clip.width() );

      // Note: It might be worthwhile to cache the results of the draw.
      // The OGLPaintDevice gets a lot of commands from the RichText
      // formatter...
      text.draw( &painter, 0, 0, clip, cg );

      view()->resetWindow();

      // If the text was larger than the box, draw a solid line at
      // the bottom of the box. Note: Qt's opinion of size is 1 pixel
      // larger than OpenGL's.
      if ( (text.height()-1) > clip.height() ) {
	glBegin( GL_LINES );
	glVertex2dv( lbVertex() );
	glVertex2dv( rbVertex() );
	glEnd();
      }
    }
    // Just as an optimization, don't call handle draw routines if they
    // are invisible.

    lC::Render::Mode mode = lC::Render::INVISIBLE;
    if ( isHighlighted() )
      mode = lC::Render::HIGHLIGHTED;
    if ( isActivated() )
      mode = lC::Render::ACTIVATED;

    if ( mode != lC::Render::INVISIBLE ) {
      ResizeHandle::draw( view(), mode, lbVertex() );
      ResizeHandle::draw( view(), mode, lbVertex() + 0.5 * ( rbVertex() - lbVertex() ) );
      ResizeHandle::draw( view(), mode, rbVertex() );
      ResizeHandle::draw( view(), mode, rbVertex() + 0.5 * ( rtVertex() - rbVertex() ) );
      ResizeHandle::draw( view(), mode, rtVertex() );
      ResizeHandle::draw( view(), mode, rtVertex() + 0.5 * ( ltVertex() - rtVertex() ) );
      ResizeHandle::draw( view(), mode, ltVertex() );
      ResizeHandle::draw( view(), mode, ltVertex() + 0.5 * ( lbVertex() - ltVertex() ) );
    }
  }

  void AnnotationView::select ( SelectionType /*selection_type*/ ) const
  {
    if ( !annotation_->isComplete() ) return;

    glLoadName( selectionName() );

    // The handles override any edge selection

    if ( isHighlighted() || isActivated() ) {
      glPushName( lb_handle_.selectionName() );
      ResizeHandle::select( view(), lbVertex() );
      glPopName();

      glPushName( mb_handle_.selectionName() );
      ResizeHandle::select( view(), lbVertex() + 0.5 * ( rbVertex() - lbVertex() ) );
      glPopName();

      glPushName( rb_handle_.selectionName() );
      ResizeHandle::select( view(), rbVertex() );
      glPopName();

      glPushName( mr_handle_.selectionName() );
      ResizeHandle::select( view(), rbVertex() + 0.5 * ( rtVertex() - rbVertex() ) );
      glPopName();

      glPushName( rt_handle_.selectionName() );
      ResizeHandle::select( view(), rtVertex() );
      glPopName();

      glPushName( mt_handle_.selectionName() );
      ResizeHandle::select( view(), rtVertex() + 0.5 * ( ltVertex() - rtVertex() ) );
      glPopName();

      glPushName( lt_handle_.selectionName() );
      ResizeHandle::select( view(), ltVertex() );
      glPopName();

      glPushName( ml_handle_.selectionName() );
      ResizeHandle::select( view(), ltVertex() + 0.5 * ( lbVertex() - ltVertex() ) );
      glPopName();
    }

    glPushName( solid_annotation_.selectionName() );

    glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

    glRectd( lbVertex()[X], lbVertex()[Y], rtVertex()[X], rtVertex()[Y] );

    glPopName();
  }

  lC::ValidDelta AnnotationView::adjust ( int selection_name,
					  const Point& last_pnt, Point& current_pnt)
  {
    return (this->*adjustments_[selection_name])( last_pnt, current_pnt);
  }

  void AnnotationView::moveCorner ( const Point& corner )
  {
    Vector delta = corner - annotation_->origin();
    delta *= 1. / view()->scale();
    size_ = delta;

    annotationResized();
  }

  Point AnnotationView::rtVertex ( void ) const
  {
    if ( size_[X] >= 0 ) {
      if ( size_[Y] >= 0 ) {
	return annotation_->origin() + view()->scale() * size_;
      }
      else {
	return annotation_->origin() + view()->scale() * Vector( size_[X], 0 );
      }
    }
    else {
      if ( size_[Y] >= 0 ) {
	return annotation_->origin() + view()->scale() * Vector( 0, size_[Y] );
      }
      else {
	return annotation_->origin();
      }
    }
  }

  Point AnnotationView::lbVertex ( void ) const
  {
    if ( size_[X] >= 0 ) {
      if ( size_[Y] >= 0 ) {
	return annotation_->origin();
      }
      else {
	return annotation_->origin() + view()->scale() * Vector( 0, size_[Y] );
      }
    }
    else {
      if ( size_[Y] >= 0 ) {
	return annotation_->origin() + view()->scale() * Vector( size_[X], 0 );
      }
      else {
	return annotation_->origin() + view()->scale() * size_;
      }
    }
  }

  Point AnnotationView::rbVertex ( void ) const
  {
    if ( size_[X] >= 0 ) {
      if ( size_[Y] >= 0 ) {
	return annotation_->origin() + view()->scale() * Vector( size_[X], 0 );
      }
      else {
	return annotation_->origin() + view()->scale() * size_;
      }
    }
    else {
      if ( size_[Y] >= 0 ) {
	return annotation_->origin();
      }
      else {
	return annotation_->origin() + view()->scale() * Vector( 0, size_[Y] );
      }
    }
  }

  Point AnnotationView::ltVertex ( void ) const
  {
    if ( size_[X] >= 0 ) {
      if ( size_[Y] >= 0 ) {
	return annotation_->origin() + view()->scale() * Vector( 0, size_[Y] );
      }
      else {
	return annotation_->origin();
      }
    }
    else {
      if ( size_[Y] >= 0 ) {
	return annotation_->origin() + view()->scale() * size_;
      }
      else {
	return annotation_->origin() + view()->scale() * Vector( size_[X], 0 );
      }
    }
  }

  lC::ValidDelta AnnotationView::move ( const Point& last_pnt, Point& current_pnt )
  {
    Vector delta = current_pnt - last_pnt;
    annotation_->move( delta );

    return lC::ValidDelta();
  }

  lC::ValidDelta AnnotationView::resizeLeft ( const Point& last_pnt,
					      Point& current_pnt )
  {
    Vector delta = current_pnt - last_pnt;
    double delta_x = delta[X] / view()->scale();
    delta[Y] = 0.;

    if ( ( fabs( width() ) - delta_x ) <= lC::MINIMUM_SIZE ) {
      if ( fabs( width() ) <= lC::MINIMUM_SIZE )
	return lC::ValidDelta( false );
      else {
	delta_x = fabs( width() ) - lC::MINIMUM_SIZE;
	delta[X] = view()->scale() * delta_x;
	current_pnt[X] = last_pnt[X] + delta[X];
      }
    }

    if ( size_[X] >= 0 ) {
      annotation_->move( delta );
      size_[X] -= delta_x;
    }
    else {
      size_[X] += delta_x;
    }

    annotationResized();

    return lC::ValidDelta();
  }

  lC::ValidDelta AnnotationView::resizeRight ( const Point& last_pnt,
					       Point& current_pnt )
  {
    Vector delta = current_pnt - last_pnt;
    double delta_x = delta[X] / view()->scale();
    delta[Y] = 0.;

    if ( ( fabs( width() ) + delta_x ) <= lC::MINIMUM_SIZE ) {
      if ( fabs( width() ) <= lC::MINIMUM_SIZE )
	return lC::ValidDelta( false );
      else {
	delta_x = -fabs( width() ) + lC::MINIMUM_SIZE;
	delta[X] = view()->scale() * delta_x;
	current_pnt[X] = last_pnt[X] + delta[X];
      }
    }

    if ( size_[X] < 0 ) {
      annotation_->move( delta );
      size_[X] -= delta_x;
    }
    else {
      size_[X] += delta_x;
    }

    annotationResized();

    return lC::ValidDelta();
  }

  lC::ValidDelta AnnotationView::resizeBottom ( const Point& last_pnt,
						Point& current_pnt )
  {
    Vector delta = current_pnt - last_pnt;
    double delta_y = delta[Y] / view()->scale();
    delta[X] = 0.;

    if ( ( fabs( height() ) - delta_y ) <= lC::MINIMUM_SIZE ) {
      if ( fabs( height() ) <= lC::MINIMUM_SIZE )
	return lC::ValidDelta( true, false );
      else {
	delta_y = fabs( height() ) - lC::MINIMUM_SIZE;
	delta[Y] = view()->scale() * delta_y;
	current_pnt[Y] = last_pnt[Y] + delta[Y];
      }
    }

    if ( size_[Y] >= 0 ) {
      annotation_->move( delta );
      size_[Y] -= delta_y;
    }
    else {
      size_[Y] += delta_y;
    }

    annotationResized();

    return lC::ValidDelta();
  }

  lC::ValidDelta AnnotationView::resizeTop ( const Point& last_pnt,
					     Point& current_pnt )
  {
    Vector delta = current_pnt - last_pnt;
    double delta_y = delta[Y] / view()->scale();
    delta[X] = 0.;

    if ( ( fabs( height() ) + delta_y ) <= lC::MINIMUM_SIZE ) {
      if ( fabs( height() ) <= lC::MINIMUM_SIZE )
	return lC::ValidDelta( true, false );
      else {
	delta_y = - fabs( height() ) + lC::MINIMUM_SIZE;
	delta[Y] = view()->scale() * delta_y;
	current_pnt[Y] = last_pnt[Y] + delta[Y];
      }
    }

    if ( size_[Y] < 0 ) {
      annotation_->move( delta );
      size_[Y] -= delta_y;
    }
    else {
      size_[Y] += delta_y;
    }

    annotationResized();

    return lC::ValidDelta();
  }

  lC::ValidDelta AnnotationView::resizeLeftBottom ( const Point& last_pnt,
						    Point& current_pnt )
  {
    lC::ValidDelta ok_left = resizeLeft( last_pnt, current_pnt );
    lC::ValidDelta ok_bottom = resizeBottom( last_pnt, current_pnt );

    return lC::ValidDelta( ok_left.dx_ok_, ok_bottom.dy_ok_ );
  }

  lC::ValidDelta AnnotationView::resizeLeftTop ( const Point& last_pnt,
						 Point& current_pnt )
  {
    lC::ValidDelta ok_left = resizeLeft( last_pnt, current_pnt );
    lC::ValidDelta ok_top = resizeTop( last_pnt, current_pnt );

    return lC::ValidDelta( ok_left.dx_ok_, ok_top.dy_ok_ );
  }

  lC::ValidDelta AnnotationView::resizeRightBottom ( const Point& last_pnt,
						     Point& current_pnt )
  {
    lC::ValidDelta ok_right = resizeRight( last_pnt, current_pnt );
    lC::ValidDelta ok_bottom = resizeBottom( last_pnt, current_pnt );

    return lC::ValidDelta( ok_right.dx_ok_, ok_bottom.dy_ok_ );
  }

  lC::ValidDelta AnnotationView::resizeRightTop ( const Point& last_pnt,
						  Point& current_pnt )
  {
    lC::ValidDelta ok_right = resizeRight( last_pnt, current_pnt );
    lC::ValidDelta ok_top = resizeTop( last_pnt, current_pnt );

    return lC::ValidDelta( ok_right.dx_ok_, ok_top.dy_ok_ );
  }

  void AnnotationView::setCursor ( GLuint selection_name )
  {
    GraphicsView* graphics_object = annotation_objects_[ selection_name ];

    if ( graphics_object != 0 )
      view()->setCursor( QCursor( graphics_object->cursorShape() ) );
  }

  void AnnotationView::setCursor ( const QCursor* cursor )
  {
    view()->setCursor( *cursor );
  }

  void AnnotationView::unsetCursor ( void )
  {
    view()->unsetCursor();
  }
  /*
   * Create an XML representation of this view.
   */
  void AnnotationView::write ( QDomElement& xml_rep ) const
  {
    if ( !annotation_->isValid() ) return;

    QDomDocument document = xml_rep.ownerDocument();
    QDomElement annotation_view_element =
      document.createElement( lC::STR::ANNOTATION_VIEW );

    annotation_view_element.setAttribute( lC::STR::ANNOTATION,
					  annotation_->dbURL().toString(true) );

    annotation_view_element.setAttribute( lC::STR::WIDTH, lC::format( size_[X] ) );
    annotation_view_element.setAttribute( lC::STR::HEIGHT, lC::format( size_[Y] ) );

    xml_rep.appendChild( annotation_view_element );
  }
  /*
   * Create an XML representation of a simplified version of this view
   * *AND* its annotation.
   */
  void AnnotationView::copyWrite ( QDomElement& xml_rep ) const
  {
    QDomDocument document = xml_rep.ownerDocument();
    QDomElement annotation_view_element =
      document.createElement( lC::STR::ANNOTATION_VIEW );

    annotation_view_element.setAttribute( lC::STR::WIDTH, lC::format( size_[X] ) );
    annotation_view_element.setAttribute( lC::STR::HEIGHT, lC::format( size_[Y] ) );

    annotation_->copyWrite( annotation_view_element );

    xml_rep.appendChild( annotation_view_element );
  }
  // The annotation was initialized
  void AnnotationView::annotationInitialized ( void )
  {
    list_view_item_->setText( lC::DETAIL,
			     trC( lC::STR::RECTANGLE_FMT ).
			     arg( annotation_->origin()[X] ).
			     arg( annotation_->origin()[Y] ).
			     arg( size_[X] ).
			     arg( size_[Y] ) );
  }

  // The annotation was moved
  void AnnotationView::annotationMoved ( void )
  {
    list_view_item_->setText( lC::DETAIL,
			     trC( lC::STR::RECTANGLE_FMT ).
			     arg( annotation_->origin()[X] ).
			     arg( annotation_->origin()[Y] ).
			     arg( size_[X] ).
			     arg( size_[Y] ) );
  }

  // The annotation was resized
  void AnnotationView::annotationResized ( void )
  {
    list_view_item_->setText( lC::DETAIL,
			     trC( lC::STR::RECTANGLE_FMT ).
			     arg( annotation_->origin()[X] ).
			     arg( annotation_->origin()[Y] ).
			     arg( size_[X] ).
			     arg( size_[Y] ) );
  }

   // Recompute any layout due to changes in the view.

  void AnnotationView::viewAttributeChanged ( void )
  {}

  // Change the name of the annotation.

  void AnnotationView::setName ( const QString& name )
  {
    DBURL old_db_url = annotation_->dbURL();

    annotation_->setName( name );

    CommandHistory::instance().
      addCommand( new RenameCommand( "rename annotation", model(), old_db_url,
				     annotation_->dbURL() ) );
  }

  // Check a list view rename for uniqueness.

  void AnnotationView::listNameChanged ( const QString& name )
  {
    int ret = parent()->uniqueFigureName( this, name, annotation_->type() );

    switch ( ret ) {
    case lC::OK:
      setName( name );
      break;
    case lC::Redo:
      list_view_item_->startRename( lC::NAME );
    case lC::Rejected:
      updateName( annotation_->name() ); // Repaint list item with old name.
    }
  }

  // Update the static displays of the annotation's name

  void AnnotationView::updateName ( const QString& /*name*/ )
  {
    list_view_item_->setText( lC::NAME, lC::formatName( annotation_->name() )
			      + QString( " <%1>" ).arg( annotation_->id() ) );
  }
} // End of Space2D namespace
