/*
 * solidview.cpp
 *
 * OCSolidView classes
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
#include <qapplication.h>
#include <qlineedit.h>
#include <qcursor.h>

#include "command.h"
#include "cursorfactory.h"
#include "listviewitem.h"
#include "openglview.h"
#include "lignumcadmainwindow.h"
#include "designbookview.h"
#include "pageview.h"
#include "model.h"
#include "dimension.h"
#include "page.h"
#include "ocsolid.h"
#include "material.h"
#include "partinfodialog.h"
#include "parameterinfodialog.h"
#include "lcdefaultlengthconstraint.h"
#include "ocsoliddraw.h"
#include "ocsolidview.h"

#include "lcdebug.h"

namespace Space3D {
  class OCSolidViewCreate : public CreateObject {
    //! When CommandHistory undoes or redoes the creation of this object,
    //! the Design Book view is the only handle which is required. Everything
    //! else can be looked up.
    DesignBookView* design_book_view_;
    //! The details of the solid and view are stored as XML.
    QDomDocument xml_doc_;
  public:
    /*!
     * Create a memento of the current state of the solid and its view.
     * \param solid_view view object of the solid.
     */
    OCSolidViewCreate ( OCSolidView* solid_view )
    {
      design_book_view_ = solid_view->parent()->parent();

      QDomElement root = xml_doc_.createElement( lC::STR::MEMENTO );

      root.setAttribute( lC::STR::NAME, solid_view->dbURL().toString(true) );

      solid_view->solid()->write( root );
      solid_view->write( root );

      xml_doc_.appendChild( root );
    }
    //! Destructor doesn't do much.
    ~OCSolidViewCreate ( void ) {}
    /*!
     * (Re)create the solid object and its view as specified in
     * the XML representation.
     */
    void create ( void )
    {
      cout << "invoking the creat command for the solidview memento" << endl;
#if 0
      QDomNodeList memento_list = xml_doc_.elementsByTagName( lC::STR::MEMENTO );

      if ( memento_list.length() > 0 ) {
	DBURL url = memento_list.item(0).toElement().attribute( lC::STR::NAME );

	// First, we have to find the parent page and view for the solid.

	DBURL parent = url.parent();

	Page* page =
	  dynamic_cast<Page*>( design_book_view_->model()->lookup( parent ) );
	PageView* page_view =
	  dynamic_cast<PageView*>( design_book_view_->lookup( parent ) );

	// Now, we can extract the XML representations for the solid
	// and its view
	QDomNodeList solid_list =
	  xml_doc_.elementsByTagName( lC::STR::SOLID );

	if ( solid_list.length() > 0 ) {

	  QDomNodeList solid_view_list =
	    xml_doc_.elementsByTagName( lC::STR::SOLID_VIEW );

	  if ( solid_view_list.length() > 0 ) {
	    new OCSolid( solid_list.item(0).toElement(), page );

	    new OCSolidView( solid_view_list.item(0).toElement(),
			   page_view);
	  }
	}
      }
#endif
    }
    /*!
     * Delete the solid object and its view as specified in the
     * XML representation.
     */
    void remove ( void )
    {
      QDomNodeList memento_list = xml_doc_.elementsByTagName( lC::STR::MEMENTO );

      if ( memento_list.length() > 0 ) {
	DBURL url = memento_list.item(0).toElement().attribute( lC::STR::NAME );

	OCSolidView* solid_view =
	  dynamic_cast<OCSolidView*>( design_book_view_->lookup( url ) );

	DBURL parent = url.parent();

	PageView* page_view =
	  dynamic_cast<PageView*>( design_book_view_->lookup( parent ) );

	page_view->removeFigureView( solid_view );
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

      QDomNodeList solid_list =
	xml_doc_.elementsByTagName( lC::STR::SOLID );

      if ( solid_list.length() > 0 ) {
	QDomNode element = xml_rep->importNode( solid_list.item(0), true );

	rep_root.appendChild( element );
      }

      QDomNodeList solid_view_list =
	xml_doc_.elementsByTagName( lC::STR::SOLID_VIEW );

      if ( solid_view_list.length() > 0 ) {
	QDomNode element = xml_rep->importNode( solid_view_list.item(0),
						true );
	rep_root.appendChild( element );
      }
    }
    /*!
     * If the creation of a solid is immediately followed by a rename,
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

	if ( path != rename->oldDBURL().toString() )
	  return false;

	// Additional sanity check: make sure the object and its view have elements.

	QDomNodeList solid_list =
	  xml_doc_.elementsByTagName( lC::STR::SOLID );
	QDomNodeList solid_view_list =
	  xml_doc_.elementsByTagName( lC::STR::SOLID_VIEW );

	if ( solid_list.length() == 0 ||
	     solid_view_list.length() == 0 )
	  return false;

	// Update the name elements in the object and it's view.

	memento_list.item(0).toElement().setAttribute( lC::STR::NAME,
						       rename->newDBURL() );

	solid_list.item(0).toElement().
	  setAttribute( lC::STR::NAME, rename->newDBURL().name() );

	solid_view_list.item(0).toElement().setAttribute( lC::STR::SOLID,
							  rename->newDBURL() );

	return true;
      }

      return false;
    }
  };

  /*!
   * Internal class that handles the undo/redo command for modifying the size
   * of a solid. The slight complication here is that OCSolid is an
   * abstract class.
   */
  class ModifySolidCommand : public Command {
  public:
    /*!
     * Construct a command to undo/redo a size modification on an OCSolid.
     * \param name name of command.
     * \param solid reference to solid.
     * \param old_parameters old parameters of solid.
     */
    ModifySolidCommand ( const QString& name,
					     DesignBookView* design_book_view,
					     OCSolid* solid,
					     const std::map<QString,double>&
					     old_parameters )
      : Command( name ), design_book_view_( design_book_view ),
	db_url_( solid->dbURL() ), old_parameters_( old_parameters )
    {
      // Could improve the efficiency of this by limiting it to only
      // what's actually changed...
      std::map<QString,Parameter>::const_iterator parameter =
	solid->parametersBegin();
      for ( ; parameter != solid->parametersEnd(); ++parameter )
	new_parameters_[ (*parameter).first ] = (*parameter).second.value();
    }
    //! Delete the move annotation undo/redo command.
    ~ModifySolidCommand ( void )
    {}
    //! \return command type.
    Type type ( void ) const { return ModifySolid; }
    //! \return if possible, merge the argument command into this command.
    bool merge ( Command* /*command*/ ) { return false; }
    //! (Re)execute the command (i.e., redo).
    void execute ( void )
    {
      Model* model = design_book_view_->model();
      OCSolid* solid = dynamic_cast<OCSolid*>( model->lookup( db_url_ ) );
      solid->updateParameters( new_parameters_ );
    }
    //! Unexecute the command (i.e., undo).
    void unexecute ( void )
    {
      Model* model = design_book_view_->model();
      OCSolid* solid = dynamic_cast<OCSolid*>( model->lookup( db_url_ ) );
      solid->updateParameters( old_parameters_ );
    }
    /*!
     * Serialize this command.
     * \param document XML representation to append this command to.
     */
    void write ( QDomDocument* document ) const
    {
      QDomElement modify_solid_element =
	document->createElement( lC::STR::MODIFY_SOLID );
      QDomNode root_node = document->firstChild();
      if ( !root_node.isNull() )
	document->replaceChild( modify_solid_element, root_node );
      else
	document->appendChild( modify_solid_element );

      modify_solid_element.setAttribute( lC::STR::NAME, db_url_.toString(true) );
      
      std::map<QString,double>::const_iterator old_parameter =old_parameters_.begin();

      for ( ; old_parameter != old_parameters_.end(); ++old_parameter ) {
	std::map<QString,double>::const_iterator new_parameter =
	  new_parameters_.find( (*old_parameter).first );
	if ( new_parameter != new_parameters_.end() ) {
	  QDomElement parameter_element = document->createElement(lC::STR::PARAMETER);
	  parameter_element.setAttribute( lC::STR::NAME, (*old_parameter).first );
	  parameter_element.setAttribute(lC::STR::OLD_VALUE,(*old_parameter).second);
	  parameter_element.setAttribute(lC::STR::NEW_VALUE,(*new_parameter).second);
	  modify_solid_element.appendChild( parameter_element );
	}
      }
    }
  private:
    //! Reference to Design Book view.
    DesignBookView* design_book_view_;
    //! Reference of solid.
    DBURL db_url_;
    //! Old parameters.
    std::map<QString,double> old_parameters_;
    //! New parameters.
    std::map<QString,double> new_parameters_;
  };

  OCSolidModifyInput::OCSolidModifyInput ( OCSolid* solid,
					   OCSolidView* solid_view )
    : selection_type_( TRANSIENT, FIGURE ),
      solid_( solid ), solid_view_( solid_view )
  {
  }

  void OCSolidModifyInput::mousePrepress ( QMouseEvent* me,
					   const SelectedNames& selected )
  {
    if ( selected.empty() ) {
      solid_view_->unsetCursor();
      return;
    }

    SelectedNames::const_iterator f;

    for ( f = selected.begin(); f != selected.end(); ++f ) {
      if ( (*f).second[0] == solid_view_->selectionName() ) {
	if ( (*f).second.size() > 1 ) {
	  GLuint g = (*f).second[1];

	  if ( solid_view_->isGeometry( g ) ) {
	    solid_view_->setCursor( g );

	    me->accept();
	  }
	  else {
	    solid_view_->unsetCursor();
	  }
	  return;
	}
      }
    }
    solid_view_->unsetCursor();
  }

  void OCSolidModifyInput::mousePress ( QMouseEvent* me,
					const SelectedNames& selected )
  {
    // Let the Page handle these events

    if ( me->button() != Qt::LeftButton || selected.empty() ) {
      me->ignore();
      return;
    }

    SelectedNames::const_iterator f;

    for ( f = selected.begin(); f != selected.end(); ++f ) {

      if ( (*f).second[0] == solid_view_->selectionName() ) {

	if ( (*f).second.size() > 1 ) {

	  // The order in which selection names are allocated means
	  // that the handle selections are at the end of the selection
	  // name submap. So, we take the geometry of interest from
	  // the end (using rbegin()).

	  GLuint g = (*f).second[1];

	  if ( solid_view_->isGeometry( g ) ) {

	    std::map<QString,Parameter>::const_iterator parameter =
	      solid_->parametersBegin();
	    for ( ; parameter != solid_->parametersEnd(); ++parameter )
	      old_parameters_[ (*parameter).first ] = (*parameter).second.value();

	    last_pnt_ = solid_view_->view()->unproject3D( me->pos());
	    UnitsBasis::instance()->round( last_pnt_ );
	    start_pnt_ = last_pnt_;

	    drag_handle_ = g;

	    // Search for other geometry's to align with.
	    selection_type_ = SelectionType( TRANSIENT, NONE );

	    // Prevent highlighting of this figure's geometry.
	    solid_->setComplete( false );

	    return;
	  }
	}
      }
    }

    me->ignore();
  }

  void OCSolidModifyInput::mouseDrag ( QMouseEvent* me,
				       const SelectedNames& /*selected*/ )
  {
    if ( drag_handle_ == 0 ) return;

    //    cout << "Dragging around handle: " << drag_handle_ << endl;

    current_pnt_ = solid_view_->view()->unproject3D( me->pos() );
    UnitsBasis::instance()->round( current_pnt_ );
    
    //    cout << "drag to " << current_pnt_ << endl;

    lC::ValidDelta ok = solid_view_->adjust( drag_handle_, last_pnt_, current_pnt_);
	  
    if ( ok.dx_ok_ )
      last_pnt_[X] = current_pnt_[X];
    if ( ok.dy_ok_ )
      last_pnt_[Y] = current_pnt_[Y];
    if ( ok.dz_ok_ )
      last_pnt_[Z] = current_pnt_[Z];

    solid_view_->parent()->figureModified();
  }

  bool OCSolidModifyInput::mouseRelease ( QMouseEvent* /*me*/,
					  const SelectedNames& /*selected*/ )
  {
    if ( last_pnt_ != start_pnt_ ) {
      ModifySolidCommand* command =
	new ModifySolidCommand( "modify solid", solid_view_->parent()->parent(),
				solid_, old_parameters_ );
      CommandHistory::instance().addCommand( command );
    }

    selection_type_ = SelectionType( TRANSIENT, FIGURE );

    solid_->setComplete( true );

    drag_handle_ = 0;

    return false;
  }

  void OCSolidModifyInput::mouseDoubleClick ( QMouseEvent* /*me*/ )
  {
    solid_view_->editInformation();
  }

  void OCSolidModifyInput::setSolid ( OCSolid* solid )
  {
    solid_ = solid;
  }


  PartInfoDialog* OCSolidView::part_info_dialog_;
  ParameterInfoDialog* OCSolidView::parameter_info_dialog_;

  GLfloat OCSolidView::material_white_[4] = { 1., 1., 1., 1. };

  OCSolidView::OCSolidView ( OCSolid* solid, PageView* parent )
    : FigureView( parent ), solid_( solid ), modify_input_( solid, this )
  {
    init();
  }

  OCSolidView::OCSolidView ( const QDomElement& xml_rep,
			     PageView* parent )
    : FigureView( parent ), solid_( 0 ), modify_input_( 0, this )
  {
    DBURL db_url( xml_rep.attribute( lC::STR::SOLID ) );

    solid_ = dynamic_cast< OCSolid* >( model()->lookup( db_url ) );

    modify_input_.setSolid( solid_ );

    init();
  }
#if 0
  OCSolidView::OCSolidView ( const QString& name, const QDomElement& xml_rep,
			     Page* page, PageView* parent )
    : FigureView( parent ), solid_( 0 ), modify_input_( 0, this )
  {
    QDomDocument doc = xml_rep.ownerDocument();
    QDomNodeList solid_list = doc.elementsByTagName( lC::STR::SOLID );
    // Well, failure is not really an option...but it could happen if
    // somebody put a partial XML document into the clipboard...
    // In general, the failure modes of lignumCAD need substantially more
    // consideration......
#if 0
    if ( solid_list.count() > 0 )
      solid_ = new OCSolid( name, solid_list.item(0).toElement(),page);
#endif
    modify_input_.setSolid( solid_ );

    init();
  }
#endif
  void OCSolidView::init ( void )
  {
    QObject::setName( solid_->name().latin1() );

    QListViewItem* previous_item = parent()->previousItem( parent()->listViewItem(),
							   solid_->id() );

    list_view_item_ = new ListViewItem( parent()->listViewItem(), previous_item );

  list_view_item_->setText( lC::NAME, lC::formatName( solid_->name() )
			    + QString( " <%1>" ).arg( solid_->id() ) );
    list_view_item_->setText( lC::TYPE, trC( lC::STR::SOLID ) );
    QString detail = tr( "%1:" ).arg( solid_->baseShape() );
    QStringList param_strings;
    std::map<QString,Parameter>::const_iterator parameter = solid_->parametersBegin();
    for ( ; parameter != solid_->parametersEnd(); ++parameter )
      param_strings << tr( " %1: %2" ).
	arg( trC( (*parameter).second.title() ) ).
	arg( (*parameter).second.value() );
    detail.append( param_strings.join( tr( "," ) ) );
    list_view_item_->setText( lC::DETAIL, detail );
    list_view_item_->setOpen( true );

    connect( solid_, SIGNAL( modified() ), SLOT( updateTessellation() ) );

    connect( solid_, SIGNAL( materialChanged() ), SLOT( updateMaterial() ) );

    connect( solid_, SIGNAL( nameChanged(const QString&) ),
	     SLOT( updateName(const QString&) ) );

    if ( part_info_dialog_ == 0 )
      part_info_dialog_ = new PartInfoDialog( parent()->lCMW() );

    if ( parameter_info_dialog_ == 0 )
      parameter_info_dialog_ = new ParameterInfoDialog( parent()->lCMW() );

    view()->makeCurrent();

    edge_name_ = glGenLists( 3 );
    tri_name_ = edge_name_ + 1;
    hlr_name_ = tri_name_ + 1;

    connect( parent(), SIGNAL( orientationChangedHidden( const GLdouble* ) ),
	     SLOT( updateHiddenOutline( const GLdouble* ) ) );

    connect( parent(), SIGNAL( orientationChanged( const GLdouble* ) ),
	     SLOT( updateViewNormal( const GLdouble* ) ) );

    drawer_ = new OCSolidDraw( solid_, view() );

    updateTessellation();
    updateMaterial();
  }

  OCSolidView::~OCSolidView ( void )
  {
    delete drawer_;

    if ( solid_ ) {
      solid_->parent()->removeFigure( solid_ );
    }
  }

  CreateObject* OCSolidView::memento ( void )
  {
    cout << "Somebody asked for a memento of the solidview" << endl;
    return new OCSolidViewCreate( this );
  }

  /*
   * Recompute any layout due to changes in the view.
   */
  void OCSolidView::viewAttributeChanged ( void )
  {
    drawer_->update();
    drawer_->updateDimensionViews();
  }
  /*
   * Change the name of the solid.
   */
  void OCSolidView::setName ( const QString& name )
  {
    DBURL old_db_url = solid_->dbURL();

    solid_->setName( name );

    CommandHistory::instance().
      addCommand( new RenameCommand( "rename solid",
				     model(), old_db_url, solid_->dbURL() ));
  }

  void OCSolidView::draw ( void ) const
  {
    if ( !solid_->isValid() ) return;

    lC::Render::Mode mode = lC::Render::REGULAR;
    if ( isActivated() )
      mode = lC::Render::ACTIVATED;
    else if ( isHighlighted() )
      mode = lC::Render::HIGHLIGHTED;

    drawer_->draw( parent()->renderStyle(), parent()->selectionType().entity_,
		   mode, true );
  }

  void OCSolidView::select ( SelectionType selection_type ) const
  {
    if ( !solid_->isComplete() ) return;
    glLoadName( selectionName() );

    lC::Render::Mode mode = lC::Render::REGULAR;
    if ( isActivated() )
      mode = lC::Render::ACTIVATED;
    else if ( isHighlighted() )
      mode = lC::Render::HIGHLIGHTED;

    drawer_->select( selection_type.entity_,  mode, true );
  }

  void OCSolidView::setCursor ( GLuint /*selection_name*/ )
  {
    view()->
      setCursor( CursorFactory::instance().cursor( CursorFactory::RESIZE_SOLID ) );
  }

  void OCSolidView::setCursor ( const QCursor* cursor )
  {
    view()->setCursor( *cursor );
  }

  void OCSolidView::unsetCursor ( void )
  {
    view()->unsetCursor();
  }

  // Implementation of FigureView interface
  uint OCSolidView::id ( void ) const
  {
    return solid_->id();
  }

  QString OCSolidView::name ( void ) const
  {
    return solid_->name();
  }

  DBURL OCSolidView::dbURL ( void ) const
  {
    return solid_->dbURL();
  }

  QString OCSolidView::selectionText ( const std::vector<GLuint>& selection_name,
				       SelectionEntity entity ) const
  {
    QString text;

    switch ( entity ) {
    case NONE:
    case EDGE:
    case VERTEX:
      break;
    case FIGURE:
      text = solid_->name(); break;
    case FACE:
      text = lC::formatName( solid_->name() ) + '/' +
	lC::formatName( drawer_->faceName( selection_name[1] ) );
    }

    return text;
  }

  QString OCSolidView::type ( void ) const
  {
    return lC::STR::SOLID;
  }

  void OCSolidView::setHighlighted( bool highlight, SelectionEntity entity,
				    const std::vector<GLuint>& items )
  {
    
    if (entity == FIGURE ) {
      if ( items.size() > 1 ) {
	drawer_->setHighlighted( highlight, entity, items );
      }

      FigureViewBase::setHighlighted( highlight );
    }
    else if ( entity == FACE ) {
      if ( items.size() > 1 ) {
	drawer_->setHighlighted( highlight, entity, items );
      }
    }
  }

  void OCSolidView::setActivated( bool activate, SelectionEntity entity,
				  const std::vector<GLuint>& items )
  {
    if ( entity == FIGURE ) {

      drawer_->setActivated( activate, entity, items );

      FigureViewBase::setActivated( activate );
    }
    else if ( entity == FACE ) {
      if ( items.size() > 1 ) {
	drawer_->setActivated( activate, entity, items );
      }
    }
  }
  /*
   * Determine if this selection is part of the geometry representation;
   * this includes the handles if they happen to be highlighted, too.
   * The purpose here is really just to distinguish between the
   * line and the dimension views.
   */
  bool OCSolidView::isGeometry ( GLuint selection_name ) const
  {
    return drawer_->isHandle( selection_name );
  }

  QString OCSolidView::geometry ( GLuint selection_name ) const
  {
    return drawer_->faceName( selection_name );
  }

  FigureBase* OCSolidView::figure ( void ) const
  {
    return solid_;
  }

  /*
   * If the solid itself is modified by a class other than this,
   * the solid will report it here. Should cause the solid to be
   * re-tessellated.
   */
  void OCSolidView::updateTessellation ( void )
  {
    drawer_->update();

    if ( parent()->renderStyle() == lC::Render::HIDDEN )
      drawer_->updateHiddenOutline();

    // Update the list view.
    QString detail = tr( "%1:" ).arg( solid_->baseShape() );
    QStringList param_strings;
    std::map<QString,Parameter>::const_iterator parameter = solid_->parametersBegin();
    for ( ; parameter != solid_->parametersEnd(); ++parameter )
      param_strings << tr( " %1: %2" ).
	arg( trC( (*parameter).second.title() ) ).
	arg( (*parameter).second.value() );
    detail.append( param_strings.join( tr( "," ) ) );
    list_view_item_->setText( lC::DETAIL, detail );
  }

  void OCSolidView::updateViewNormal ( const GLdouble* /*modelview*/ )
  {
    drawer_->updateViewNormal();
  }

  void OCSolidView::updateHiddenOutline ( const GLdouble* /*modelview*/ )
  {
    drawer_->updateHiddenOutline();
  }

  void OCSolidView::updateMaterial ( void )
  {
    drawer_->updateMaterial();

    view()->updateGL();
  }

  lC::ValidDelta OCSolidView::adjust ( GLuint selection_name,
				       const Point& last_pnt, Point& current_pnt )
  {
    uint id = drawer_->handleID( selection_name );
    return solid_->adjustHandle( id, last_pnt, current_pnt );
  }

  void OCSolidView::editInformation ( void )
  {
    QString parameter_name = drawer_->activeDimensionName();

    if ( parameter_name != QString::null ) {
      const Parameter& parameter( solid_->parameter( parameter_name ) );

      parameter_info_dialog_->parameterLengthConstraint->
	setTitle( trC( parameter.title() ) );
      parameter_info_dialog_->parameterLengthConstraint->
	setLengthLimits( UnitsBasis::instance()->lengthUnit(),
			 UnitsBasis::instance()->format(),
			 UnitsBasis::instance()->precision(),
			 0, lC::MAXIMUM_DIMENSION,
			 parameter.value() );
      parameter_info_dialog_->parameterLengthConstraint->
	setLength( parameter.value() );

      int ret = parameter_info_dialog_->exec();

      if ( ret == QDialog::Rejected ) return;

      if ( parameter_info_dialog_->parameterLengthConstraint->edited() ) {
	std::map<QString,double> old_parameter;
	old_parameter[ parameter_name ] = parameter.value();
	//	cout << "Updating parameter " << parameter_name << " of " << solid_->path()
	//	     << endl;
	// Studies show that this can take a long time
	qApp->setOverrideCursor( Qt::WaitCursor );
	qApp->processEvents();


	solid_->updateParameter( parameter_name,
				 parameter_info_dialog_->
				 parameterLengthConstraint->specifiedLength() );

	qApp->restoreOverrideCursor();

	ModifySolidCommand* command =
	  new ModifySolidCommand( "modify solid", parent()->parent(), solid_,
				  old_parameter );

	CommandHistory::instance().addCommand( command );

	parent()->figureModified();
      }
    }
    else if ( FigureViewBase::isActivated() ) {
      editSolidInformation();
    }
  }

  void OCSolidView::editSolidInformation ( void )
  {
    part_info_dialog_->nameEdit->setText( lC::formatName( solid_->name() ) );

    int ret = part_info_dialog_->exec();

    if ( ret == QDialog::Rejected ) return;

    bool modified = false;

    if ( part_info_dialog_->nameEdit->edited() ) {
      parent()->setName( part_info_dialog_->nameEdit->text() );
      modified = true;
    }

    if ( modified ) parent()->figureModified();
  }

  void OCSolidView::write ( QDomElement& xml_rep ) const
  {
    if ( !solid_->isValid() ) return;

    QDomDocument document = xml_rep.ownerDocument();
    QDomElement solid_view_element =
      document.createElement( lC::STR::SOLID_VIEW );

    solid_view_element.setAttribute( lC::STR::SOLID,
				     solid_->dbURL().toString( true ) );

    xml_rep.appendChild( solid_view_element );
  }
  /*
   * Create an XML representation of a simplified version of this view
   * and its solid.
   */
  void OCSolidView::copyWrite ( QDomElement& xml_rep ) const
  {
    QDomDocument document = xml_rep.ownerDocument();
    QDomElement solid_view_element =
      document.createElement( lC::STR::SOLID_VIEW );

    solid_->copyWrite( solid_view_element );

    xml_rep.appendChild( solid_view_element );
  }
  /*
   * Detect changes to the name initiated elsewhere
   */
  void OCSolidView::updateName ( const QString& /*name*/ )
  {
    list_view_item_->setText( lC::NAME, lC::formatName( solid_->name() )
			      + QString( " <%1>" ).arg( solid_->id() ) );
  }
} // End of Space3D namespace
