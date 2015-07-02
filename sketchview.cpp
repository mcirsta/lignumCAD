/*
 * sketchview.cpp
 *
 * SketchView class: a view of a Sketch.
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

#include "constants.h"
#include "command.h"
#include "lignumcadmainwindow.h"
#include "openglview.h"
#include "designbookview.h"
#include "model.h"
#include "rectangle.h"
#include "rectangleview.h"
#include "referenceline.h"
#include "referencelineview.h"
#include "centerline.h"
#include "centerlineview.h"
#include "annotation.h"
#include "annotationview.h"
#include "dimensionview.h"
#include "alignmentview.h"
#include "constraintview.h"
#include "listviewitem.h"
#include "pageinfodialog.h"
#include "pagefactory.h"
#include "sketchview.h"

using namespace Space2D;

/*!
 * SketchViewCreate is a memento of the current state of a Sketch and its view.
 * It handles creating and deleting both objects.
 */
class SketchViewCreate : public CreateObject {
    //! When CommandHistory undoes or redoes the creation of this object,
    //! the Design Book view is the only handle which is required. Everything
    //! else can be looked up.
    DesignBookView* design_book_view_;
    //! The details of the sketch and view are stored as XML.
    QDomDocument xml_doc_;
public:
    /*!
   * Create a memento of the current state of the sketch and its view.
   * \param sketch_view view object of the sketch.
   */
    SketchViewCreate ( SketchView* sketch_view )
    {
        design_book_view_ = sketch_view->parent();

        QDomElement root = xml_doc_.createElement( lC::STR::MEMENTO );

        //TODO maybe toString needs another encoding ?
        root.setAttribute( lC::STR::NAME, sketch_view->dbURL().toString() );

        sketch_view->sketch()->write( root );
        sketch_view->write( root );

        xml_doc_.appendChild( root );
    }
    //! Destructor does not do much.
    ~SketchViewCreate ( void ) {}
    /*!
   * (Re)create the sketch object and its view as specified in
   * the XML representation.
   */
    void create ( void )
    {
        QDomNodeList sketch_list = xml_doc_.elementsByTagName( lC::STR::SKETCH );

        if ( sketch_list.length() > 0 ) {

            QDomNodeList sketch_view_list =
                    xml_doc_.elementsByTagName( lC::STR::SKETCH_VIEW );

            if ( sketch_view_list.length() > 0 ) {
                uint sketch_id =
                        sketch_list.item(0).toElement().attribute( lC::STR::ID ).toUInt();

                new Sketch( sketch_id, sketch_list.item(0).toElement(),
                            design_book_view_->model() );

                SketchView* sketch_view =
                        new SketchView( design_book_view_, sketch_view_list.item(0).toElement() );

                design_book_view_->addPageView( sketch_view );
                design_book_view_->showPageView( sketch_view );
            }
        }
    }
    /*!
   * Delete the sketch object and its view as specified in the
   * XML representation.
   */
    void remove ( void )
    {
        QDomNodeList sketch_view_list =xml_doc_.elementsByTagName(lC::STR::SKETCH_VIEW);

        if ( sketch_view_list.length() > 0 ) {

            QDomElement sketch_view_element = sketch_view_list.item(0).toElement();

            QString sketch_path = sketch_view_element.attribute( lC::STR::SKETCH );

            SketchView* sketch_view =
                    dynamic_cast<SketchView*>( design_book_view_->lookup( sketch_path ) );

            design_book_view_->deletePage( sketch_view );
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

        QDomNodeList sketch_list = xml_doc_.elementsByTagName( lC::STR::SKETCH );

        if ( sketch_list.length() > 0 ) {

            QDomNode element = xml_rep->importNode( sketch_list.item(0), true );
            rep_root.appendChild( element );
        }

        QDomNodeList sketch_view_list =
                xml_doc_.elementsByTagName( lC::STR::SKETCH_VIEW );

        if ( sketch_view_list.length() > 0 ) {

            QDomNode element = xml_rep->importNode( sketch_view_list.item(0), true );
            rep_root.appendChild( element );
        }
    }
    /*!
   * If the creation of a sketch is immediately followed by a rename,
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

            QDomNodeList sketch_list = xml_doc_.elementsByTagName( lC::STR::SKETCH );
            QDomNodeList sketch_view_list =
                    xml_doc_.elementsByTagName( lC::STR::SKETCH_VIEW );

            if ( sketch_list.length() == 0 || sketch_view_list.length() == 0 )
                return false;

            // Update the name elements in the object and it's view.

            memento_list.item(0).toElement().setAttribute( lC::STR::NAME,
                                                           rename->newDBURL().toString() );

            sketch_list.item(0).toElement().
                    setAttribute( lC::STR::NAME, rename->newDBURL().name() );

            sketch_view_list.item(0).toElement().setAttribute( lC::STR::SKETCH,
                                                               rename->newDBURL().toString() );

            return true;
        }

        return false;
    }
};

static GLushort dashes0[] = { 0xff00, 0x7f80, 0x3fc0, 0x1fe0,
                              0x0ff0, 0x07f8, 0x03fc, 0x01fe,
                              0x00ff, 0x807f, 0xc03f, 0xe01f,
                              0xf00f, 0xf807, 0xfc03, 0xfe01 };
static GLushort dashes1[] = { 0xf0f0, 0x7878, 0x3c3c, 0x1e1e,
                              0x0f0f, 0x8787, 0xc3c3, 0xe1e1 };
static GLushort dashes2[] = { 0xcccc, 0x6666, 0x3333, 0x9999 };
static GLushort dashes3[] = { 0xaaaa, 0x5555 };

GLushort* SketchView::dashes[4] = { dashes0, dashes1, dashes2, dashes3 };

PageInfoDialog* SketchView::config_dialog_ = 0;

SketchView::SketchView ( Sketch* sketch, DesignBookView* parent )
    : PageView( parent ), sketch_( sketch ), currentTab( 0 )
{
    init();
}

SketchView::SketchView ( DesignBookView* parent, const QDomElement& xml_rep )
    : PageView( parent ), currentTab( 0 )
{
    DBURL db_url( xml_rep.attribute( lC::STR::SKETCH ) );

    sketch_ = dynamic_cast<Sketch*>( model()->lookup( db_url ) );

    if ( sketch_ == 0 )		// This should be a fatal error.
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
            else if ( e.tagName() == lC::STR::REFERENCE_LINE_VIEW ) {
                addFigureView( new ReferenceLineView ( e, this ) );
            }
            else if ( e.tagName() == lC::STR::CENTERLINE_VIEW ) {
                addFigureView( new CenterlineView ( e, this ) );
            }
            else if ( e.tagName() == lC::STR::RECTANGLE_VIEW ) {
                addFigureView( new RectangleView ( e, this ) );
            }
            else if ( e.tagName() == lC::STR::ANNOTATION_VIEW ) {
                addFigureView( new AnnotationView ( e, this ) );
            }
        }
        n = n.nextSibling();
    }
}

SketchView::~SketchView ()
{
    delete dimension_create_input_;
    delete alignment_create_input_;
    // Have to delete everthing from the bottom up. So, clear the page
    // before the PageView's destructor is invoked.
    clearFigureViews();

    delete list_view_item_;

    if ( sketch_ )
        model()->removePage( sketch_ );
}

void SketchView::init ( void )
{
    dimension_create_input_ = new DimensionCreateInput( this );
    alignment_create_input_ = new AlignmentCreateInput( this );
    constraint_delete_input_ = new ConstraintDeleteInput( this );

    tabIcon_ =  lC::lookupPixmap( "sketch.png" );
    tabText_ =  lC::formatTabName( sketch_->name() );

    QStandardItem* previous_item = parent()->previousItem( sketch_->id() );

    list_view_item_ = new ListViewItem( parent()->modelListItem(), previous_item );

    list_view_item_->setData( lC::formatName( sketch_->name() )
                              + QString( " <%1>" ).arg( sketch_->id() ) ,
                              lC::NAME );
    list_view_item_->setData( trC( lC::STR::SKETCH ), lC::TYPE  );

    //TODO see how to implement these
    //list_view_item_->setOpen( true );
    //list_view_item_->setRenameEnabled( lC::NAME, true );

    connect( this, SIGNAL( newInformation( const QString& ) ),
             lCMW(), SLOT( updateInformation( const QString& ) ) );

    //TODO see how to implement this
    //connect( list_view_item_, SIGNAL( nameChanged( const QString& ) ),
    //     this, SLOT( listNameChanged( const QString& ) ) );

    connect( sketch_, SIGNAL( nameChanged( const QString& ) ),
             SLOT( updateName( const QString& ) ) );

    if ( config_dialog_ == 0 )
        config_dialog_ = new PageInfoDialog;
}

// Do any post-creation configuration.

bool SketchView::configure ( void )
{
    // Just set the name of the sketch.

    config_dialog_->getUi()->nameEdit->setText( lC::formatName( name() ) );
    config_dialog_->getUi()->nameEdit->selectAll();
    config_dialog_->getUi()->nameEdit->setFocus();
    config_dialog_->getUi()->buttonOk->setDefault( true );

redo:
    int ret = config_dialog_->exec();

    if ( ret == QDialog::Rejected ) return false;

    if ( config_dialog_->getUi()->nameEdit->isModified() ) {
        // DesignBookView handles checking the name and putting up the error dialog
        // if necessary.
        int ret = parent()->uniquePageName( this,
                                            config_dialog_->getUi()->nameEdit->text(),
                                            sketch_->type() );

        switch ( ret ) {
        case lC::Rejected:
            return false;
        case lC::Redo:
            config_dialog_->getUi()->nameEdit->setText( lC::formatName( sketch_->name() ) );
            goto redo;
        }

        sketch_->setName( config_dialog_->getUi()->nameEdit->text() );
    }

    return true;
}

CreateObject* SketchView::memento ( void )
{
    return new SketchViewCreate( this );
}

void SketchView::setName ( const QString& name )
{
    DBURL old_db_url = sketch_->dbURL();

    sketch_->setName( name );

    CommandHistory::instance().addCommand( new RenameCommand( "rename sketch",
                                                              model(),
                                                              old_db_url,
                                                              sketch_->dbURL() ) );
}

// Check a list view rename for uniqueness.

void SketchView::listNameChanged ( const QString& name )
{
    int ret = parent()->uniquePageName( this, name, sketch_->type() );

    switch ( ret ) {
    case lC::OK:
        setName( name );
        break;
    case lC::Redo:
        //TODO implement this
        //list_view_item_->startRename( lC::NAME );
    case lC::Rejected:
        updateName( sketch_->name() ); // Repaint list item with old name.
    }
}

/*!
 * Display this view. Among other things, set the proper actions
 * into the Tool menu
 */
void SketchView::show ( void ) const
{
    lCMW()->getUi()->toolMenu->clear();
    lCMW()->getUi()->toolMenu->addAction( lCMW()->getUi()->toolRectangleAction );
    lCMW()->getUi()->toolMenu->addSeparator();
    lCMW()->getUi()->toolMenu->addAction ( lCMW()->getUi()->toolReferenceLineAction  );
    lCMW()->getUi()->toolMenu->addAction ( lCMW()->getUi()->toolCenterlineAction );
    lCMW()->getUi()->toolMenu->addAction ( lCMW()->getUi()->toolAnnotationAction );
    lCMW()->getUi()->toolMenu->addSeparator();
    lCMW()->getUi()->toolMenu->addAction ( lCMW()->getUi()->toolDimensionAction );
    lCMW()->getUi()->toolMenu->addAction ( lCMW()->getUi()->toolAlignmentAction );
    lCMW()->getUi()->toolMenu->addSeparator();
    lCMW()->getUi()->toolMenu->addAction ( lCMW()->getUi()->toolConstraintDeleteAction );

    lCMW()->getUi()->toolRectangleAction->disconnect();
    lCMW()->getUi()->toolReferenceLineAction->disconnect();
    lCMW()->getUi()->toolCenterlineAction->disconnect();
    lCMW()->getUi()->toolAnnotationAction->disconnect();
    lCMW()->getUi()->toolDimensionAction->disconnect();
    lCMW()->getUi()->toolAlignmentAction->disconnect();
    lCMW()->getUi()->toolConstraintDeleteAction->disconnect();

    connect( lCMW()->getUi()->toolRectangleAction, SIGNAL( activated() ),
             SLOT( createRectangle() ) );
    connect( lCMW()->getUi()->toolCenterlineAction, SIGNAL( activated() ),
             SLOT( createCenterline() ) );
    connect( lCMW()->getUi()->toolReferenceLineAction, SIGNAL( activated() ),
             SLOT( createReferenceLine() ) );
    connect( lCMW()->getUi()->toolAnnotationAction, SIGNAL( activated() ),
             SLOT( createAnnotation() ) );
    connect( lCMW()->getUi()->toolDimensionAction, SIGNAL( activated() ),
             SLOT( createDimension() ) );
    connect( lCMW()->getUi()->toolAlignmentAction, SIGNAL( activated() ),
             SLOT( createAlignment() ) );
    connect( lCMW()->getUi()->toolConstraintDeleteAction, SIGNAL( activated() ),
             SLOT( deleteConstraint() ) );
}

/*!
 * Add this view's XML description to the output documennt
 */
void SketchView::write ( QDomElement& xml_rep ) const
{
    QDomDocument document = xml_rep.ownerDocument();
    QDomElement view_element = document.createElement( lC::STR::SKETCH_VIEW );
    view_element.setAttribute( lC::STR::SKETCH, sketch_->dbURL().toString( ) );

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

/*
 * Append some useful actions to the OpenGL view context menu.
 */
void SketchView::startDisplay ( QMenu* context_menu )
{
    context_menu_ = context_menu;

    context_menu_->addSeparator();
    wireframe_id_ = context_menu_->addAction( tr( "Wireframe" ) );
    wireframe_id_->setCheckable(true);
    solid_id_ = context_menu_->addAction( tr( "Solid" ) );
    solid_id_->setCheckable(true);
    texture_id_ = context_menu_->addAction( tr( "Texture" ) );
    texture_id_->setCheckable(true);

    connect(context_menu, SIGNAL ( triggered(QAction *) ), this, SLOT ( toggleRenderStyle (QAction *id ) ) );

    switch ( renderStyle() ) {
    case lC::Render::WIREFRAME:
        wireframe_id_->setChecked(true);
        break;
    case lC::Render::SOLID:
        solid_id_->setChecked(true);
        break;
    case lC::Render::TEXTURED:
        texture_id_->setChecked(true);
        break;
    case lC::Render::PARENT:
    case lC::Render::STIPPLE:
    case lC::Render::HIDDEN:
    case lC::Render::HIGHLIGHTS:
        break;
    }

    connect( sketch_, SIGNAL( nameChanged( const QString& ) ),
             lCMW(), SLOT( pageChanged( const QString& ) ) );
}

// Clean up since we are not visible any more.

void SketchView::stopDisplay ( QMenu* /*context_menu*/ )
{
    disconnect( sketch_, SIGNAL( nameChanged( const QString& ) ),
                lCMW(), SLOT( pageChanged( const QString& ) ) );
}
/*!
 * Draw the view.
 */
void SketchView::draw ( void ) const
{
    // First draw the background grid (if not printing. user option...?)

    if ( !view()->printing() ) {

        // Compute the minimum grid spacing for this scale: start with the
        // log-base-2 of the space occupied by 10 pixels. (note: ilogb returns
        // the floor of the base-2 logarithm, but we really want the ceil,
        // so add one and live with the over estimate.)
        Space2D::Point o = view()->unproject2D( QPoint(0,0) );
        Space2D::Vector d = view()->unproject2D( QPoint(10,10) ) - o;

        int n_log2 = ilogb( d[Space2D::X] ) + 1;

        // This is an aesthetic judgement: don't make grid lines smaller than
        // 1/8" (probably a job for UnitsBasis...)
        if ( n_log2 < -3 ) n_log2 = -3;
        double step = pow( 2., n_log2 );

        // Determine the minimum and maximum multiples of step which are
        // needed to cover the screen.
        int i_start = (int)rint( floor( view()->llCorner()[Space2D::X] / step ) );
        int i_end = (int)rint( ceil( view()->urCorner()[Space2D::X] / step ) );
        int j_start = (int)rint( floor( view()->llCorner()[Space2D::Y] / step ) );
        int j_end = (int)rint( ceil( view()->urCorner()[Space2D::Y] / step ) );

        glEnable( GL_LINE_STIPPLE );

        view()->qglColor( view()->gridColor() );

        for ( int i_x = i_start; i_x <= i_end; i_x++ ) {
            if ( abs( i_x % 2 ) == 1 ) {
                glLineStipple( 1, dashes[3][view()->stipplePhase().y() % 2] );
            }
            else if ( abs( i_x % 4 ) == 2 ) {
                glLineStipple( 1, dashes[2][view()->stipplePhase().y() % 4] );
            }
            else if ( abs( i_x % 8 ) == 4 ) {
                glLineStipple( 1, dashes[1][view()->stipplePhase().y() % 8] );
            }
            else if ( abs( i_x % 8 ) == 0 ) {
                glLineStipple( 1, dashes[0][view()->stipplePhase().y() % 16] );
            }
            glBegin( GL_LINES );
            glVertex2d( i_x * step, view()->llCorner()[Space2D::Y] );
            glVertex2d( i_x * step, view()->urCorner()[Space2D::Y] );
            glEnd();
        }
        for ( int j_y = j_start; j_y <= j_end; j_y++ ) {
            if ( abs( j_y % 2 ) == 1 ) {
                glLineStipple( 1, dashes[3][view()->stipplePhase().x() % 2] );
            }
            else if ( abs( j_y % 4 ) == 2 ) {
                glLineStipple( 1, dashes[2][view()->stipplePhase().x() % 4] );
            }
            else if ( abs( j_y % 8 ) == 4 ) {
                glLineStipple( 1, dashes[1][view()->stipplePhase().x() % 8] );
            }
            else if ( abs( j_y % 8 ) == 0 ) {
                glLineStipple( 1, dashes[0][view()->stipplePhase().x() % 16] );
            }
            glBegin( GL_LINES );
            glVertex2d( view()->llCorner()[Space2D::X], j_y * step );
            glVertex2d( view()->urCorner()[Space2D::X], j_y * step );
            glEnd();
        }

        glDisable( GL_LINE_STIPPLE );
    }

    // Then, draw all the figures on the page.

    QListIterator< std::shared_ptr <FigureViewBase> > f( figureViews() );

    while(f.hasNext())
        f.next()->draw();
}

/*
 * Draw (in GL_SELECT mode) the elements indicated by the given
 * selection type. For speed, this is generally a simplified version
 * of the view.
 */
void SketchView::select ( SelectionType /*select_type*/ ) const
{
    QListIterator< std::shared_ptr <FigureViewBase> > f( figureViews() );

    while(f.hasNext())
        f.next()->select( selectionType() );
}


// Detect changes to the name via the hierarchy view.

void SketchView::updateName ( const QString& /*name*/ )
{
    tabText_= lC::formatTabName( sketch_->name() );
    //TODO update tab with above text
    list_view_item_->setData( lC::formatName( sketch_->name() )
                              + QString( " <%1>" ).arg( sketch_->id(),
                                                        lC::NAME) );
}

/*
 * Allow the user to switch among the various rendering styles.
 * Only wireframe and solid are allowed here (though textured might
 * be kind of fun, too.)
 */
void SketchView::toggleRenderStyle ( QAction* id )
{
    if ( id == wireframe_id_ and renderStyle() != lC::Render::WIREFRAME ) {
        wireframe_id_->setChecked( true );
        solid_id_->setChecked( false );
        texture_id_->setChecked( false );

        setRenderStyle( lC::Render::WIREFRAME );
    }
    else if ( id == solid_id_ and renderStyle() != lC::Render::SOLID ) {
        wireframe_id_->setChecked( false );
        solid_id_->setChecked( true );
        texture_id_->setChecked( false );

        setRenderStyle( lC::Render::SOLID );
    }
    else if ( id == texture_id_ and renderStyle() != lC::Render::TEXTURED ) {
        wireframe_id_->setChecked( false );
        solid_id_->setChecked( false );
        texture_id_->setChecked( true );

        setRenderStyle( lC::Render::TEXTURED );
    }

    view()->updateGL();
}

/*
 * Determine which figure to create from its simplified XML representation.
 */
void SketchView::pasteFigure ( const QDomElement& xml_rep )
{
    DBURL db_url = xml_rep.attribute( lC::STR::URL );

    QString name = db_url.name();
    QString type = db_url.type();

    if ( type == lC::STR::RECTANGLE ) {
        // See if there's a rectangle on this page which
        // has the same name and get a new one if necessary.
        QListIterator< std::shared_ptr <FigureViewBase> > f( figureViews() );

        while(f.hasNext())
            if ( f.peekNext()->name() == name &&
                 f.next()->type() == lC::STR::RECTANGLE ) {
                name = uniqueName( &Rectangle::newName, lC::STR::RECTANGLE );
            }

        // Perform a sanity check to prevent the most egregious abuse.
        QDomNodeList rectangle_view_list =
                xml_rep.ownerDocument().elementsByTagName( lC::STR::RECTANGLE_VIEW );

        if ( rectangle_view_list.count() == 0 ) {
            return;
        }

        QDomNodeList rectangle_list =
                xml_rep.ownerDocument().elementsByTagName( lC::STR::RECTANGLE );

        if ( rectangle_list.count() == 0 ) {
            return;
        }

        // Should be relatively safe to do this now.
        RectangleView* rv =
                new RectangleView( name, rectangle_view_list.item(0).toElement(), sketch_,
                                   this);
        addFigureView( rv );

        // Nudge the rectangle position by 10 pixels.
        Space2D::Point o = view()->unproject2D( QPoint(0,0) );
        Space2D::Vector d = view()->unproject2D( QPoint(10,10) ) - o;

        rv->rectangle()->move( d );

        CommandHistory::instance().
                addCommand( new CreateCommand( "create rectangle", rv->memento() ) );

        activateFigure( rv );
    }
    else if ( type == lC::STR::REFERENCE_LINE ) {
        // See if there's a reference line on this page which
        // has the same name and get a new one if necessary.
        QListIterator< std::shared_ptr <FigureViewBase> > f( figureViews() );

        while( f.hasNext() )
            if ( f.peekNext()->name() == name &&
                 f.next()->type() == lC::STR::REFERENCE_LINE ) {
                name = uniqueName( &ReferenceLine::newName, lC::STR::REFERENCE_LINE );
            }

        // Perform a sanity check to prevent the most egregious abuse.
        QDomNodeList reference_line_view_list =
                xml_rep.ownerDocument().elementsByTagName( lC::STR::REFERENCE_LINE_VIEW );

        if ( reference_line_view_list.count() == 0 ) {
            return;
        }

        QDomNodeList reference_line_list =
                xml_rep.ownerDocument().elementsByTagName( lC::STR::REFERENCE_LINE );

        if ( reference_line_list.count() == 0 ) {
            return;
        }

        // Should be relatively safe to do this now.

        ReferenceLineView* rlv =
                new ReferenceLineView( name, reference_line_view_list.item(0).toElement(),
                                       sketch_, this );
        addFigureView( rlv );

        // Nudge the reference line position by 10 pixels.
        Space2D::Point o = view()->unproject2D( QPoint(0,0) );
        Space2D::Vector d = view()->unproject2D( QPoint(10,10) ) - o;

        rlv->referenceLine()->move( d );

        CommandHistory::instance().
                addCommand( new CreateCommand( "create reference line", rlv->memento() ) );

        activateFigure( rlv );
    }
    else if ( type == lC::STR::CENTERLINE ) {
        // See if there's a centerline on this page which
        // has the same name and get a new one if necessary.
        QListIterator< std::shared_ptr <FigureViewBase> > f( figureViews() );

        while ( f.hasNext() )
            if ( f.peekNext()->name() == name &&
                 f.next()->type() == lC::STR::CENTERLINE ) {
                name = uniqueName( &Centerline::newName, lC::STR::CENTERLINE );
            }

        // Perform a sanity check to prevent the most egregious abuse.
        QDomNodeList centerline_view_list =
                xml_rep.ownerDocument().elementsByTagName( lC::STR::CENTERLINE_VIEW );

        if ( centerline_view_list.count() == 0 ) {
            return;
        }

        QDomNodeList centerline_list =
                xml_rep.ownerDocument().elementsByTagName( lC::STR::CENTERLINE );

        if ( centerline_list.count() == 0 ) {
            return;
        }

        // Should be relatively safe to do this now.

        CenterlineView* clv =
                new CenterlineView( name, centerline_view_list.item(0).toElement(), sketch_,
                                    this );

        addFigureView( clv );

        // Nudge the centerline position by 10 pixels.
        Space2D::Point o = view()->unproject2D( QPoint(0,0) );
        Space2D::Vector d = view()->unproject2D( QPoint(10,10) ) - o;

        clv->centerline()->move( d );

        CommandHistory::instance().
                addCommand( new CreateCommand( "create centerline", clv->memento() ) );

        activateFigure( clv );
    }
    else if ( type == lC::STR::ANNOTATION ) {
        // See if there's an annotation on this page which
        // has the same name and get a new one if necessary.
        QListIterator< std::shared_ptr <FigureViewBase> > f( figureViews() );

        while( f.hasNext() )
            if ( f.peekNext()->name() == name &&
                 f.next()->type() == lC::STR::ANNOTATION ) {
                name = uniqueName( &Annotation::newName, lC::STR::ANNOTATION );
            }

        // Perform a sanity check to prevent the most egregious abuse.
        QDomNodeList annotation_view_list =
                xml_rep.ownerDocument().elementsByTagName( lC::STR::ANNOTATION_VIEW );

        if ( annotation_view_list.count() == 0 ) {
            return;
        }

        QDomNodeList annotation_list =
                xml_rep.ownerDocument().elementsByTagName( lC::STR::ANNOTATION );

        if ( annotation_list.count() == 0 ) {
            return;
        }

        // Should be relatively safe to do this now.

        AnnotationView* av =
                new AnnotationView( name, annotation_view_list.item(0).toElement(), sketch_,
                                    this );

        addFigureView( av );

        // Nudge the annotation position by 10 pixels.
        Space2D::Point o = view()->unproject2D( QPoint(0,0) );
        Space2D::Vector d = view()->unproject2D( QPoint(10,10) ) - o;

        av->annotation()->move( d );

        CommandHistory::instance().
                addCommand( new CreateCommand( "create annotation", av->memento() ) );

        activateFigure( av );
    }
}

/*
 * Create a rectangle (and let the user define its placement)
 */
void SketchView::createRectangle ( void )
{
    deactivateFigures();

    QString name = uniqueName( &Rectangle::newName, lC::STR::RECTANGLE );

    RectangleView* rv =
            new RectangleView( new Rectangle( sketch_->newID(), name, sketch_ ), this );

    addFigureView( rv );

    rv->createInput()->startDisplay( context_menu_ );

    setInputObject( rv->createInput() );

    emit newInformation( tr( "Construct Rectangle" ) );
}

/*
 * Create a reference line (and let the user define its placement)
 */
void SketchView::createReferenceLine ( void )
{
    deactivateFigures();

    QString name = uniqueName( &ReferenceLine::newName, lC::STR::REFERENCE_LINE );

    ReferenceLineView* rlv =
            new ReferenceLineView( new ReferenceLine( sketch_->newID(), name, sketch_),
                                   this );

    addFigureView( rlv );

    rlv->createInput()->startDisplay( context_menu_ );

    setInputObject( rlv->createInput() );

    emit newInformation( tr( "Construct Reference Line" ) );
}

/*
 * Create a centerline (and let the user define its placement)
 */
void SketchView::createCenterline ( void )
{
    deactivateFigures();

    QString name = uniqueName( &Centerline::newName, lC::STR::CENTERLINE );

    CenterlineView* clv =
            new CenterlineView( new Centerline( sketch_->newID(), name, sketch_ ), this );

    addFigureView( clv );

    clv->createInput()->startDisplay( context_menu_ );

    setInputObject( clv->createInput() );

    emit newInformation( tr( "Construct Centerline" ) );
}

/*
 * Create an annotation.
 */
void SketchView::createAnnotation ( void )
{
    deactivateFigures();

    QString name = uniqueName( &Annotation::newName, lC::STR::ANNOTATION );

    AnnotationView* av =
            new AnnotationView( new Annotation( sketch_->newID(), name, sketch_), this );

    addFigureView( av );

    av->createInput()->startDisplay( context_menu_ );

    setInputObject( av->createInput() );

    emit newInformation( tr( "Construct Annotation" ) );
}

/*
 * Create a dimension.
 */
void SketchView::createDimension ( void )
{
    deactivateFigures();

    dimension_create_input_->startDisplay( context_menu_ );

    setInputObject( dimension_create_input_ );

    emit newInformation( tr( "Construct Dimension" ) );
}

/*
 * Create an alignment.
 */
void SketchView::createAlignment ( void )
{
    deactivateFigures();

    alignment_create_input_->startDisplay( context_menu_ );

    setInputObject( alignment_create_input_ );

    emit newInformation( tr( "Construct Alignment" ) );
}

/*
 * Cancel the current input operation. Can this be generic in PageView
 * will it have to be pushed down into the various concrete pages.
 * Well, here it is because we need access to the context menu.
 */
void SketchView::cancelOperation ( void )
{
    SelectedNames none;
    highlightFigures( none );

    deactivateFigures();

    view()->unsetCursor();

    emit newInformation( trC( lC::STR::NONE ) );
}
/*
 * Pick a constraint, either a dimension (offset) or alignment (coincident),
 * to delete. Well, deletion means that the constraint reverts to a
 * coordinate axis reference.
 */
void SketchView::deleteConstraint ( void )
{
    deactivateFigures();

    constraint_delete_input_->startDisplay( context_menu_ );

    setInputObject( constraint_delete_input_ );

    emit newInformation( tr( "Delete Constraint" ) );
}

/*!
 * Instance of page metadata for a sketch.
 */
class SketchViewMetadata : public PageMetadata {
public:
    /*!
   * Construct a (what amounts to a const) sketch metadata object.
   */
    SketchViewMetadata ( void )
        : PageMetadata( lC::STR::SKETCH,
                        "new_sketch.png",
                        QT_TRANSLATE_NOOP( "lignumCADMainWindow", "&Sketch" ),
                        "&Sketch...",
                        "Insert a new Sketch",
                        "<p><b>Insert|Sketch</b></p><p>Click this button to insert a new page on which you can sketch the general two-dimensional outlines of the model.</p>" )
    {
        //    cout << "Constructing sketch metadata" << endl;
        PageFactory::instance()->addPageMetadata( Sketch::PAGE_ID, this );
    }

    /*!
   * Create a new sketch and its view.
   * \return view handle for the sketch and its view.
   */
    SketchView* create ( DesignBookView* parent ) const
    {
        QString name = parent->uniqueName( &Sketch::newName, lC::STR::SKETCH );

        Sketch* sketch = new Sketch( parent->model()->newID(), name, parent->model() );
        SketchView* sketch_view = new SketchView( sketch, parent );
        parent->addPageView( sketch_view );
        return sketch_view;
    }

    /*!
   * Create a sketch from its XML representation.
   * \return handle for the sketch.
   */
    Sketch* create ( Model* parent, const QDomElement& xml_rep ) const
    {
        uint sketch_id = xml_rep.attribute( lC::STR::ID ).toUInt();
        return new Sketch( sketch_id, xml_rep, parent );
    }

    /*!
   * Create a sketch view from its XML representation.
   * \return handle for the sketch view.
   */
    SketchView* create ( DesignBookView* parent, const QDomElement& xml_rep ) const
    {
        SketchView* sketch_view = new SketchView( parent, xml_rep );
        parent->addPageView( sketch_view );
        return sketch_view;
    }

    /*!
   * Retrieve the QAction which can create this page.
   * \param lCMW pointer to the main window where the actions are stored.
   * \return the action associated with creating an instance of this page.
   */
    QAction* action ( lignumCADMainWindow* lCMW ) const
    {
        return lCMW->getUi()->insertSketchAction;
    }
};

static const SketchViewMetadata sketch_view_metadata;
