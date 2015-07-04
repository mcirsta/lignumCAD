/*
 * openglview.cpp
 *
 * OpenGLView class: the OpenGL widget.
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
#include <qcursor.h>
#include <QMenu>
#include <qaction.h>
#include <qpainter.h>
#include <qprinter.h>
#include <qwhatsthis.h>

#include "constants.h"
#include "units.h"
#include "lignumcadmainwindow.h"
#include "designbookview.h"
#include "pageview.h"
#include "OGLFT.h"
#include "gl2ps.h"
#include "command.h"
#include "openglview.h"

#include <QMouseEvent>

/*!
 * Record changes to the view orientation.
 */
class ChangeViewCommand : public Command {
    //! Used to lookup the URL of the page view.
    DesignBookView* design_book_view_;
    //! The URL of the page view whose view orientation has changed.
    DBURL db_url_;
    //! The old view orientation.
    ViewData old_view_;
    //! The new view orientation.
    ViewData new_view_;
public:
    /*!
   * Constructor takes the usual arguments defining the old and new state.
   */
    ChangeViewCommand ( const QString& name,
                        const PageView* page_view,
                        const ViewData& old_view,
                        const ViewData& new_view )
        : Command( name ), design_book_view_( page_view->parent() ),
          db_url_( page_view->dbURL() ),
          old_view_( old_view ), new_view_( new_view )
    {}
    //! Nothing for the destructor to do.
    ~ChangeViewCommand ( void )
    {}
    //! \return type of command (ChangeView).
    Type type ( void ) const { return ChangeView; }
    /*!
   * In general, successive view change commands for the same page
   * are merged together. However, if the new view to be merged is the same
   * as the old view of the current ChangeViewCommand, the new
   * ChangeViewCommand is not merged. It seems slightly less confusing.
   * \param command Command to attempt to merge.
   * \return true if the command was merged.
   */
    bool merge ( Command* command )
    {
        if ( command->type() == ChangeView ) {

            ChangeViewCommand* change_view = dynamic_cast< ChangeViewCommand* >( command);

            if ( db_url_ == change_view->db_url_ &&
                 !( old_view_ == change_view->new_view_ )) {
                new_view_ = change_view->new_view_;
                return true;
            }
        }

        return false;
    }

    void execute ( void )
    {
        PageView* page_view =
                dynamic_cast<PageView*>( design_book_view_->lookup( db_url_ ) );
        page_view->setViewData( new_view_ );

        // Only alert the OpenGL view if this is the active page.
        if ( page_view == page_view->view()->pageView() )
            page_view->view()->setViewData( new_view_ );
    }

    void unexecute ( void )
    {
        PageView* page_view =
                dynamic_cast<PageView*>( design_book_view_->lookup( db_url_ ) );
        page_view->setViewData( old_view_ );

        // Only alert the OpenGL view if this is the active page.
        if ( page_view == page_view->view()->pageView() )
            page_view->view()->setViewData( old_view_ );
    }

    void write ( QDomDocument* document ) const
    {
        QDomElement view_element = document->createElement( lC::STR::CHANGE_VIEW );
        QDomNode root_node = document->firstChild();
        if ( !root_node.isNull() )
            document->replaceChild( view_element, root_node );
        else
            document->appendChild( view_element );

        view_element.setAttribute( lC::STR::URL, db_url_.toString() );

        QDomElement old_view_element = document->createElement( lC::STR::OLD_VIEW );
        old_view_.write( old_view_element );

        QDomElement new_view_element = document->createElement( lC::STR::NEW_VIEW );
        new_view_.write( new_view_element );

        view_element.appendChild( old_view_element );
        view_element.appendChild( new_view_element );
    }
};

using namespace Space3D;

const GLdouble OpenGLView::PICK_APERTURE = 3.;

GLuint OpenGLView::selection_name_ = 0;

GLuint OpenGLView::genSelectionName ( void )
{
    return ++selection_name_;
}

/*!
 * Construct an OpenGL View. The lignumCADMainWindow is only for grabbing
 * the Actions defined in Designer.
 * \param parent The parent widget (a layout, most likely).
 * \param name Widget name.
 * \param lCMW The lignumCADMainWindow: holds any Actions which the view
 * might display in a context menu.
 */
OpenGLView::OpenGLView ( DesignBookView* parent, const char* name,
                         lignumCADMainWindow* lCMW, QGLWidget* share_widget )
    : OpenGLBase( parent, name, share_widget ),
      mouse_mode_( MODEL )
{
    page_view_ = 0;

    scale_ = view_data_.scale_;

    widthIN_ = scale_ * width() / logicalDpiX();
    heightIN_ = scale_ * height() / logicalDpiY();

    ll_corner_ = view_data_.view_point_ +
            Vector( -widthIN_/2., -heightIN_/2., -widthIN_/2. );
    ur_corner_ = view_data_.view_point_ +
            Vector( widthIN_/2., heightIN_/2., widthIN_/2. );

    viewport_[0] = 0;
    viewport_[1] = 0;
    viewport_[2] = width();
    viewport_[3] = height();

    projection_[0] = 1; projection_[1] = 0; projection_[2] = 0; projection_[3] = 0;
    projection_[4] = 0; projection_[5] = 1; projection_[6] = 0; projection_[7] = 0;
    projection_[8] = 0; projection_[9] = 0; projection_[10] = 1; projection_[11] = 0;
    projection_[12] = 0; projection_[13] = 0; projection_[14] = 0; projection_[15]= 1;

    modelview_[0] = 1; modelview_[1] = 0; modelview_[2] = 0; modelview_[3] = 0;
    modelview_[4] = 0; modelview_[5] = 1; modelview_[6] = 0; modelview_[7] = 0;
    modelview_[8] = 0; modelview_[9] = 0; modelview_[10] = 1; modelview_[11] = 0;
    modelview_[12] = 0; modelview_[13] = 0; modelview_[14] = 0; modelview_[15]= 1;

    arrow_.setLength( scale_ * arrowHeadLength() );
    arrow_.setViewNormal( Vector( 0, 0, 1 ) );

    setFocusPolicy( Qt::FocusPolicy::StrongFocus );
    // Printing view doesn't need this either...
    context_menu_ = new QMenu( "openglview_context", this );
    view_menu_ = new QMenu( "view_menu", this );

    if ( lCMW != 0 ) { // Note: printing view doesn't have a reference to lCMW!
        view_menu_->addAction( lCMW->getUi()->viewLeftAction );
        view_menu_->addAction( lCMW->getUi()->viewRightAction );
        view_menu_->addAction( lCMW->getUi()->viewFrontAction );
        view_menu_->addAction( lCMW->getUi()->viewBackAction );
        view_menu_->addAction( lCMW->getUi()->viewBottomAction );
        view_menu_->addAction( lCMW->getUi()->viewTopAction );

        connect( lCMW->getUi()->viewRestoreAction, SIGNAL( activated() ), SLOT( restoreView()));
        connect( lCMW->getUi()->zoomInAction, SIGNAL( activated() ), SLOT( zoomIn()));
        connect( lCMW->getUi()->zoomOutAction, SIGNAL( activated() ), SLOT( zoomOut()));
        connect( lCMW->getUi()->toggleCSysAction, SIGNAL( activated() ), SLOT( toggleCSys()));
        connect( lCMW->getUi()->viewLeftAction, SIGNAL( activated() ), SLOT( viewLeft() ) );
        connect( lCMW->getUi()->viewRightAction, SIGNAL( activated() ), SLOT( viewRight() ) );
        connect( lCMW->getUi()->viewFrontAction, SIGNAL( activated() ), SLOT( viewFront() ) );
        connect( lCMW->getUi()->viewBackAction, SIGNAL( activated() ), SLOT( viewBack() ) );
        connect( lCMW->getUi()->viewBottomAction, SIGNAL( activated() ), SLOT( viewBottom() ) );
        connect( lCMW->getUi()->viewTopAction, SIGNAL( activated() ), SLOT( viewTop() ) );
    }

    connect( OpenGLGlobals::instance(), SIGNAL( attributeChanged() ),
             SLOT( updateAttributes() ) );
    connect( UnitsBasis::instance(), SIGNAL( unitsChanged() ),
             SLOT( updateAttributes() ) );

    connect( OpenGLGlobals::instance(), SIGNAL( backgroundAttributeChanged() ),
             SLOT( updateBackground() ) );

    this->setWhatsThis( tr( "This is the main window which displays the \
                            pages of the model. Select a current page by clicking on the tabs below \
                            this window." ) );
}

OpenGLView::~OpenGLView ( void )
{}

void OpenGLView::setViewData ( const ViewData& view_data )
{
    view_data_ = view_data;

    scale_ = view_data_.scale_;

    switch ( page_view_->space() ) {
    case SPACE2D:
        set2DView(); break;
    case SPACE3D:
        set3DView(); break;
    }

    page_view_->viewAttributeChanged();

    emit scale( view_data_.scale_ );
    emit rotation( modelview_ );

    updateGL();
}

QColor OpenGLView::geometryColor ( void ) const
{
    return OpenGLGlobals::instance()->geometryColor();
}

QColor OpenGLView::annotationColor ( void ) const
{
    return OpenGLGlobals::instance()->annotationColor();
}

QColor OpenGLView::gridColor ( void ) const
{
    return OpenGLGlobals::instance()->gridColor();
}

QColor OpenGLView::constraintPrimaryColor ( void ) const
{
    return OpenGLGlobals::instance()->constraintPrimaryColor();
}

QColor OpenGLView::constraintSecondaryColor ( void ) const
{
    return OpenGLGlobals::instance()->constraintSecondaryColor();
}

QString OpenGLView::dimensionFont ( void ) const
{
    return OpenGLGlobals::instance()->dimensionFont();
}

double OpenGLView::arrowHeadLength ( void ) const
{
    return OpenGLGlobals::instance()->arrowHeadLength();
}

Ratio OpenGLView::arrowHeadWidthRatio ( void ) const
{
    return OpenGLGlobals::instance()->arrowHeadWidthRatio();
}

lC::ArrowHeadStyle OpenGLView::arrowHeadStyle ( void ) const
{
    return OpenGLGlobals::instance()->arrowHeadStyle();
}

double OpenGLView::clearance ( void ) const
{
    return OpenGLGlobals::instance()->clearance();
}

double OpenGLView::extensionLineOffset ( void ) const
{
    return OpenGLGlobals::instance()->extensionLineOffset();
}

QString OpenGLView::annotationFont ( void ) const
{
    return OpenGLGlobals::instance()->annotationFont();
}

double OpenGLView::handleSize ( void ) const
{
    return OpenGLGlobals::instance()->handleSize();
}

QString OpenGLView::format ( double value ) const
{
    return UnitsBasis::instance()->format( value );
}

void OpenGLView::setPageView ( PageView* page_view )
{
    if ( page_view_ != 0 )
        page_view_->stopDisplay( context_menu_ );

    page_view_ =  page_view;

    // Clear out any actions which the previous object may have appended to
    // the context menu

    context_menu_->clear();
    context_menu_->addAction( dynamic_cast<DesignBookView*>( parentWidget() )->lCMW()->getUi()->viewRestoreAction );

    mouse_mode_ = VIEW;

    makeCurrent();

    if ( page_view_ != 0 ) {

        // Offer it the chance to set the view.

        if ( !page_view_->viewData( view_data_ ) ) {

            // If it doesn't have a predefined view, give it the default.
            view_data_.reset( page_view_->space() );

            page_view_->setViewData( view_data_ );
        }

        scale_ = view_data_.scale_;

        switch ( page_view_->space() ) {
        case SPACE2D:
            set2DView(); break;
        case SPACE3D:
            set3DView();
            QAction* submenuAct = context_menu_->addMenu( view_menu_ );
            //TODO does this work ??
            submenuAct->setIcon( lC::lookupPixmap( "view_orientation2.png" ) );
            submenuAct->setText( tr( "Quick Views" ) );
            context_menu_->addSeparator();
            ;
            context_menu_->addAction( dynamic_cast<DesignBookView*>( parentWidget() )->lCMW()->getUi()->toggleCSysAction );
            // TODO isOn==setChecked ??
            dynamic_cast<DesignBookView*>( parentWidget() )->lCMW()->getUi()->toggleCSysAction->
                    setChecked( view_data_.show_csys_ );
            break;
        }

        page_view_->viewAttributeChanged();

        // Offer it the chance to append any actions to the context menu and
        // any other pre-display configuration it needs (e.g., signal connections)
        page_view_->startDisplay( context_menu_ );

        emit scale( view_data_.scale_ );
        emit rotation( modelview_ );

        if ( page_view_->needsPrepressMouseCoordinates() ) {
            setMouseTracking( true );
        }
        else
            setMouseTracking( false );

        selection_type_ = page_view_->selectionType();
        mouse_mode_ = MODEL;
    }

    updateGL();
}

// How is this different from the above routine?
void OpenGLView::inputObjectChanged ( void )
{
    if ( page_view_ != 0 ) {
        if ( page_view_->needsPrepressMouseCoordinates() ) {
            setMouseTracking( true );
        }
        else
            setMouseTracking( false );

        selection_type_ = page_view_->selectionType();
    }
}

// Some external attribute has changed so recompute the display.
// Should probably try to make this go away. If an attribute
// is changed, it should emit a signal. (However, for efficiency
// one might want to bundle multiple changes together, of course.)

void OpenGLView::redisplay ( void )
{
    if ( page_view_ == 0 ) return;

    makeCurrent();

    page_view_->viewAttributeChanged();

    updateGL();
}

/*
 * Take a window coordinate and compute it's stipple phase in a
 * big-endian sense.
 * \param c window (pixel) coordinate.
 * \return phase of coordinate.
 */
inline GLushort OpenGLView::phase ( GLdouble c )
{
    GLshort p = (int)rint( c );
    p %= 16;
    GLushort q;
    if ( p < 0 ) q = -p;
    else q = 16 - p;
    return q;
}

/*
 * Take a window coordinate and compute it's stipple phase in a
 * big-endian sense.
 * \param c window (pixel) coordinate.
 * \return phase of coordinate.
 */
inline GLushort OpenGLView::phase ( int c )
{
    GLshort p = c % 16;
    GLushort q;
    if ( p < 0 ) q = -p;
    else q = 16 - p;
    return q;
}

/*
 * Take a window position and compute it's stipple phase in a
 * big-endian sense.
 * \param p window (pixel) position.
 * \return phases of position.
 */
inline QPoint OpenGLView::toPhase ( const QPoint& p )
{
    return QPoint( phase( p.x() ), phase( p.y() ) );
}

/*
 * "Unproject" the mouse coordinates into model coordinates. For 2D,
 * take the z window coordinate to be zero.
 */
Space2D::Point OpenGLView::unproject2D ( const QPoint& p ) const
{
    return mpv_inverse_ * Point( p.x(), height()-p.y(), 0 );
}

/*
 * "Unproject" the mouse coordinates into model coordinates. For 3D,
 * take the z window coordinate to be zero, too.
 */
Space3D::Point OpenGLView::unproject3D ( const QPoint& p ) const
{
    return mpv_inverse_ * Point( p.x(), height()-p.y(), 0 );
}

/*
 * "Unproject" (sub)pixel coordinates into model coordinates. For 3D,
 * take the z window coordinate to be zero, too.
 */
Space3D::Point OpenGLView::unproject3D ( const Space2D::Point& p ) const
{
    return mpv_inverse_ * Point( p[X], p[Y], 0 );
}

/*
 * Project the model coordinates into screen coordinates. For 2D,
 * take the z window coordinate to be zero.
 */
QPoint OpenGLView::project2D ( const Space2D::Point& p ) const
{
    double x, y, z;
    gluProject( p[X], p[Y], 0.,
                modelview_, projection_, viewport_, &x, &y, &z );
    return QPoint( (int)rint(x), (int)rint(y) );
}
/*
 * Temporarily set the MODELVIEW (only) to this subwindow.
 */
QRect OpenGLView::newWindow ( const Space2D::Point& origin,
                              const Space2D::Vector& size )
{
    // Compute the origin and size in (OpenGL) screen coordinates.
    QPoint o = project2D( origin - Space2D::Vector( 0, scale_ * fabs(size[Y]) ) );
    int w = (int)fabs( rint( size[X] * logicalDpiX() ) );
    int h = (int)fabs( rint( size[Y] * logicalDpiY() ) );

    // Create the local window coordinate system such that the origin
    // is positioned at the upper left corner in model coordinates
    // but the size of the window is in paper inches (equivalent to
    // a number of pixels). The resulting coordinate system is essentially
    // in pixels.
    glPushMatrix();
    glLoadIdentity();
    glTranslated( origin[X], origin[Y], 0. );

    scale_ = logicalDpiX();
    glScaled( view_data_.scale_/(double)logicalDpiX(),
              view_data_.scale_/(double)logicalDpiY(), 1. );

    // This is a bit of gloss: don't render outside the bounding box.
    glEnable( GL_SCISSOR_TEST );
    glScissor( o.x(), o.y(), w, h );

    return QRect( 0, 0, w, h );
}
/*
 * Restore the previous model view.
 */
void OpenGLView::resetWindow ( void )
{
    glDisable( GL_SCISSOR_TEST );
    glPopMatrix();
    scale_ = view_data_.scale_;
}
void OpenGLView::initializeGL ( void )
{
    glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
    glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
#if defined(GL_RASTER_POSITION_UNCLIPPED_IBM)
    glEnable( GL_RASTER_POSITION_UNCLIPPED_IBM );
#endif
    glShadeModel( GL_FLAT );
}

void OpenGLView::resizeGL ( int width, int height )
{
    if ( page_view_ != 0 ) {
        switch ( page_view_->space() ) {
        case SPACE2D:
            set2DView(); break;
        case SPACE3D:
            set3DView(); break;
        }

        page_view_->viewAttributeChanged();
    }
    else
        glViewport( 0, 0, width, height );
}

void OpenGLView::paintGL ( void )
{
    if ( page_view_ != 0 ) {
        OpenGLGlobals::instance()->clear( this, page_view_->space() == SPACE3D );

        if ( page_view_->space() == SPACE3D && view_data_.show_csys_ ) csys();

        page_view_->draw();
    }
    else
        OpenGLGlobals::instance()->clear( this, false );
}

void OpenGLView::set2DView ( void )
{
    // The caller must have set the scale and the view point.

    widthIN_ = scale_ * width() / logicalDpiX();
    heightIN_ = scale_ * height() / logicalDpiY();

    // The view point becomes the lower left corner of the window.

    ll_corner_ = view_data_.view_point_;
    ur_corner_ = ll_corner_ + Vector( widthIN_, heightIN_, 2 );

    // No need for either the depth test *or* even writing to the depth buffer.
    glDisable( GL_DEPTH_TEST );
    glDepthMask( GL_FALSE );

    // Use the whole window
    glViewport( 0, 0, (GLsizei)width(), (GLsizei)height() );
    glGetIntegerv( GL_VIEWPORT, viewport_ );
    setViewportInverse();

    // Draw things exactly to scale for starters
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();

    glOrtho( ll_corner_[X], ur_corner_[X],
             ll_corner_[Y], ur_corner_[Y],
             ll_corner_[Z], ur_corner_[Z] );

    glGetDoublev( GL_PROJECTION_MATRIX, projection_ );
    setProjectionInverse();

    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();

    glGetDoublev( GL_MODELVIEW_MATRIX, modelview_ );
    setModelviewInverse();

    stipple_phase_ = toPhase( project2D( Point( 0., 0. ) ) );
}

void OpenGLView::set3DView ( void )
{
    // The caller must have set the scale, the center (view point),
    // and any required re-orientation.

    widthIN_ = scale_ * width() / logicalDpiX();
    heightIN_ = scale_ * height() / logicalDpiY();

    // The view point becomes the center of the window. Note: the
    // Z near/far coordinates really need to be big enough to not clip
    // what is being shown!... (But, +/-infinity is no good because
    // that reduces the precision of the depth buffer.)

    ll_corner_ = view_data_.view_point_ +
            Vector( -widthIN_/2., -heightIN_/2., -widthIN_ );
    ur_corner_ = view_data_.view_point_ +
            Vector( widthIN_/2., heightIN_/2., widthIN_ );

    // Enable the depth test *and* writing to the depth buffer
    glEnable( GL_DEPTH_TEST );
    glDepthMask( GL_TRUE );

    // Use the whole window
    glViewport( 0, 0, (GLsizei)width(), (GLsizei)height() );
    glGetIntegerv( GL_VIEWPORT, viewport_ );
    setViewportInverse();

    // Draw things exactly to scale for starters
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();

    glOrtho( ll_corner_[X], ur_corner_[X],
             ll_corner_[Y], ur_corner_[Y],
             ll_corner_[Z], ur_corner_[Z] );

    glGetDoublev( GL_PROJECTION_MATRIX, projection_ );
    setProjectionInverse();

    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();

    glRotated( view_data_.z_angle_1_, 0, 0, 1 );
    glRotated( view_data_.y_angle_0_, 0, 1, 0 );
    glRotated( view_data_.z_angle_0_, 0, 0, 1 );

    glGetDoublev( GL_MODELVIEW_MATRIX, modelview_ );
    setModelviewInverse();

    stipple_phase_ = toPhase( project2D( Point( 0., 0. ) ) );

    arrow_.setLength( scale_ * arrowHeadLength() );
    arrow_.setViewNormal( Vector( modelview_[2], modelview_[6], modelview_[10] ) );

    GLfloat position[] = {0, 0, 1, 0};

    glMatrixMode( GL_MODELVIEW );
    glPushMatrix();
    glLoadIdentity();
    glLightfv( GL_LIGHT0, GL_POSITION, position );
    glPopMatrix();

    GLfloat low[] = { 0.2, 0.2, 0.2, 1 };
    GLfloat std[] = { 0.8, 0.8, 0.8, 1 };
    glLightfv( GL_LIGHT0, GL_AMBIENT, low );
    glLightfv( GL_LIGHT0, GL_DIFFUSE, std );
}

void OpenGLView::mousePressEvent ( QMouseEvent* me )
{
    if ( page_view_ == 0 ) return;

    SelectedNames selected_names;

    select( me, selected_names );

    page_view_->mousePress( me, selected_names );

    // If the page isn't listening for any particular selections
    // or if the user didn't actually click on anything, assume it's
    // a view manipulation command.

    if ( ( !me->isAccepted() /*&& selected_names.empty()*/ ) &&
         ( me->button() == Qt::LeftButton || me->button() == Qt::MidButton ) ) {
        mouse_mode_ = VIEW;
        mouse_position_ = me->pos();
        old_cursor_ = cursor();
        setCursor( QCursor( Qt::SizeAllCursor ) );
    }

    updateGL();
}

void OpenGLView::mouseReleaseEvent ( QMouseEvent* me )
{
    // Evidently, the mouse can move between the last reported position
    // and the actual release of the mouse button. Save the user's
    // sanity by only acting only on what has been drawn on the
    // screen. However, since the mouse can end up in Never Never land,
    // we need to act like it really got there. In general, then, this
    // means that the IO should call mousePrepress with the currently
    // selected figures after responding to mouseRelease with the
    // previously selected figures.
    if ( mouse_mode_ == VIEW && ( me->button() == Qt::LeftButton ||
                                  me->button() == Qt::MidButton ) ) {
        mouse_mode_ = MODEL;
        setCursor( old_cursor_ );
    }

    else if ( page_view_ != 0 ) {
        SelectedNames selected_names;

        select( me, selected_names );

        page_view_->mouseRelease( me, selected_names );
    }

    updateGL();
}

void OpenGLView::mouseMoveEvent ( QMouseEvent* me )
{
    if ( page_view_ == 0 ) return;

    // Experimental: If a QWhatThis is hoisted by a mouse press event, the
    // mouse release event is eaten. Hard to know if this is right, but
    // if we're in VIEW mode and the mouse is not pressed, then switch back
    // to MODEL mode.
    //TODO really needed ?
    //  if ( mouse_mode_ == VIEW && me->state() == 0 ) {
    //    mouse_mode_ = MODEL;
    //    setCursor( old_cursor_ );
    //  }

    if ( mouse_mode_ == VIEW ) {
        QPoint mouse_delta = me->pos() - mouse_position_;

        if ( mouse_delta == QPoint( 0, 0 ) ) return;

        mouse_position_ = me->pos();

        ViewData old_view_data = view_data_;

        switch ( page_view_->space() ) {
        case SPACE2D:
            //TODO state() == buttons() ?
            if ( me->buttons() & Qt::LeftButton ) {
                pan2D( mouse_delta );
            }
            break;
        case SPACE3D:
            //TODO state() == buttons() ?
            if ( me->buttons() & Qt::LeftButton ) {
                spin3D( mouse_delta );
                emit rotation( modelview_ );
            }
            //TODO state() == buttons() ?
            else if ( me->buttons() & Qt::MidButton ) {
                pan3D( mouse_delta );
            }
        }

        CommandHistory::instance().
                addCommand( new ChangeViewCommand( "change view", page_view_,
                                                   old_view_data, view_data_ ) );
    }
    else {
        SelectedNames selected_names;

        select( me, selected_names );

        //TODO state() == buttons() ?
        if ( me->buttons() & Qt::LeftButton )
            page_view_->mouseDrag( me, selected_names );
        else
            page_view_->mousePrepress( me, selected_names );

        if ( !me->isAccepted() ) return;
    }

    updateGL();
}

void OpenGLView::mouseDoubleClickEvent ( QMouseEvent* me )
{
    if ( page_view_ == 0 ) return;

    page_view_->mouseDoubleClick( me );

    updateGL();
}

/*
 * Pan the view in 2D
 */
void OpenGLView::pan2D ( const QPoint& mouse_delta )
{
    Vector window_delta( mouse_delta.x(), -mouse_delta.y(), 0. );
    // Faster then gluUnproject?
    window_delta *= widthIN_ / viewport_[2];

    view_data_.view_point_ -= window_delta;

    ll_corner_ -= window_delta;
    ur_corner_ -= window_delta;

    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();

    glOrtho( ll_corner_[X], ur_corner_[X],
             ll_corner_[Y], ur_corner_[Y],
             ll_corner_[Z], ur_corner_[Z] );

    glGetDoublev( GL_PROJECTION_MATRIX, projection_ );
    setProjectionInverse();

    glMatrixMode( GL_MODELVIEW );

    stipple_phase_ = toPhase( project2D( Point( 0., 0. ) ) );

    page_view_->setViewData( view_data_ );
}

/*!
 * Pan the view in 3D
 */
void OpenGLView::pan3D ( const QPoint& mouse_delta )
{
    Vector window_delta( mouse_delta.x(), -mouse_delta.y() );
    window_delta *= widthIN_ / viewport_[2];

    view_data_.view_point_ -= window_delta;

    ll_corner_ -= window_delta;
    ur_corner_ -= window_delta;

    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();

    glOrtho( ll_corner_[X], ur_corner_[X],
             ll_corner_[Y], ur_corner_[Y],
             ll_corner_[Z], ur_corner_[Z] );

    glGetDoublev( GL_PROJECTION_MATRIX, projection_ );
    setProjectionInverse();

    glMatrixMode( GL_MODELVIEW );

    page_view_->setViewData( view_data_ );
}

/*!
 * Spin the view in 3D
 */
void OpenGLView::spin3D ( const QPoint& mouse_delta )
{
    GLfloat position[] = {0, 0, 1, 0};

    glMatrixMode( GL_MODELVIEW );
    glPushMatrix();
    glLoadIdentity();
    glLightfv( GL_LIGHT0, GL_POSITION, position );
    glPopMatrix();

    glRotated( mouse_delta.y(),
               modelview_[0],
            modelview_[4],
            modelview_[8] );
    glRotated( mouse_delta.x(),
               modelview_[1],
            modelview_[5],
            modelview_[9] );

    glGetDoublev( GL_MODELVIEW_MATRIX, modelview_ );

    // Try to figure out which global rotations result in this MODELVIEW
    // (something we couldn't figure out in CoGeDif!)

    view_data_.z_angle_0_ = 0.;

    // Get the axes from the modelview matrix.
    Vector x = Vector( modelview_[0],
            modelview_[4],
            modelview_[8] );

    Vector y = Vector( modelview_[1],
            modelview_[5],
            modelview_[9] );

    Vector z = Vector( modelview_[2],
            modelview_[6],
            modelview_[10] );

    arrow_.setViewNormal( z );
    //  cout << "Modelview vectors: " << x << ", " << y << ", " << z << endl;

    // Is there any reason to rotate the Z axis?

    if ( fabs( fabs( z * Vector( 0, 0, 1 ) ) - 1. ) > lC::EPSILON ) {
        // Project the z axis into the XY plane (makes z')
        Space2D::Vector z2 = z;
        z2.normalize();

        // Rotate the MODELVIEW vectors such that z' is in the XZ plane.
        Matrix Z0( Matrix::ZROTATION, z2[Space2D::X], z2[Space2D::Y] );
        x = Z0 * x;
        y = Z0 * y;
        z = Z0 * z;

        view_data_.z_angle_0_ = -z2.orientation();
    }

    // Find the cos(angle) between z' and Z axis, i.e. z' dot Z,  which
    // is just the Z component of z', and because z' is a unit vector,
    // sin(angle) is just equal to the X component.
    Space2D::Vector y2( z[Z], z[X] );

    // Rotate the x',y',z' vectors such that z' aligns with the Z axis
    Matrix Y0( Matrix::YROTATION, y2[Space2D::X], y2[Space2D::Y] );
    x = Y0 * x;
    y = Y0 * y;
    z = Y0 * z;

    view_data_.y_angle_0_ = -y2.orientation();

    // Find the cos(angle) between x'' and the X axis.
    Space2D::Vector x2 = x;

    // Rotate the x'',y'',z'' vectors such that x'' aligns with the X axis
    Matrix Z1( Matrix::ZROTATION, x2[Space2D::X], x2[Space2D::Y] );
    x = Z1 * x;
    y = Z1 * y;
    z = Z1 * z;

    view_data_.z_angle_1_ = -x2.orientation();

    page_view_->setViewData( view_data_ );
    setModelviewInverse();
}
/*!
 * A wheel event delivered to the OpenGL view controls the zoom in/out.
 * \param we Wheel event delivered by the window system.
 */
void OpenGLView::wheelEvent ( QWheelEvent* we )
{
    if ( page_view_ == 0 ) return;

    ViewData old_view_data = view_data_;

    if ( we->delta() < 0 )
        --view_data_.scale_;
    else
        ++view_data_.scale_;

    scale_ = view_data_.scale_;

    switch ( page_view_->space() ) {
    case SPACE2D:
        scale2D();
        break;
    case SPACE3D:
        scale3D();
        break;
    }

    page_view_->viewAttributeChanged();

    emit scale( view_data_.scale_ );
    emit rotation( modelview_ );  //?...//

    CommandHistory::instance().
            addCommand( new ChangeViewCommand( "change view", page_view_,
                                               old_view_data, view_data_ ) );

    updateGL();
}

/*
 * Scale (zoom) the view in 2D.
 */
void OpenGLView::scale2D ( void )
{
    widthIN_ = scale_ * width() / logicalDpiX();
    heightIN_ = scale_ * height() / logicalDpiY();

    Vector size( widthIN_, heightIN_, 2. );

    // Zoom in around the center of the window

    Point center = ll_corner_ + .5 * ( ur_corner_ - ll_corner_ );

    view_data_.view_point_ = center - .5 * size;

    ll_corner_ = center - .5 * size;
    ur_corner_ = center + .5 * size;

    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();

    glOrtho( ll_corner_[X], ur_corner_[X],
             ll_corner_[Y], ur_corner_[Y],
             ll_corner_[Z], ur_corner_[Z] );

    glGetDoublev( GL_PROJECTION_MATRIX, projection_ );
    setProjectionInverse();

    glMatrixMode( GL_MODELVIEW );

    stipple_phase_ = toPhase( project2D( Point( 0., 0. ) ) );

    page_view_->setViewData( view_data_ );
}

/*
 * Scale (zoom) the view in 3D.
 */
void OpenGLView::scale3D ( void )
{
    widthIN_ = scale_ * width() / logicalDpiX();
    heightIN_ = scale_ * height() / logicalDpiY();

    Vector size( widthIN_, heightIN_, 2. * widthIN_ );

    // Zoom in around the view point (the center of the window)

    ll_corner_ = view_data_.view_point_ - .5 * size;
    ur_corner_ = view_data_.view_point_ + .5 * size;

    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();

    glOrtho( ll_corner_[X], ur_corner_[X],
             ll_corner_[Y], ur_corner_[Y],
             ll_corner_[Z], ur_corner_[Z] );

    glGetDoublev( GL_PROJECTION_MATRIX, projection_ );
    setProjectionInverse();

    glMatrixMode( GL_MODELVIEW );

    stipple_phase_ = toPhase( project2D( Point( 0., 0. ) ) );

    arrow_.setLength( scale_ * arrowHeadLength() );

    page_view_->setViewData( view_data_ );
}

// Compute the inverse of the viewport transformation. Adjust the
// the un-projection matrix.

void OpenGLView::setViewportInverse ( void )
{
    v_inverse_[X] = Vector( 2. / viewport_[2], 0, 0 );
    v_inverse_[Y] = Vector( 0, 2. / viewport_[3], 0 );
    v_inverse_[Z] = Vector( 0, 0, 2 );
    v_inverse_[W] = Vector( 2. * viewport_[0] / viewport_[2] - 1,
            2. * viewport_[1] / viewport_[3] - 1,
            -1 );

    mpv_inverse_ = m_inverse_ * p_inverse_ * v_inverse_;
}

// Compute the inverse of the orthographic projection matrix. Adjust
// the un-projection matrix.

void OpenGLView::setProjectionInverse ( void )
{
    p_inverse_[X] = Vector( ( ur_corner_[X] - ll_corner_[X] ) / 2, 0, 0 );
    p_inverse_[Y] = Vector( 0, ( ur_corner_[Y] - ll_corner_[Y] ) / 2, 0 );
    p_inverse_[Z] = Vector( 0, 0, -( ur_corner_[Z] - ll_corner_[Z] ) / 2 );
    p_inverse_[W] = Vector( ( ur_corner_[X] + ll_corner_[X] ) / 2,
                            ( ur_corner_[Y] + ll_corner_[Y] ) / 2,
                            ( ur_corner_[Z] + ll_corner_[Z] ) / 2 );

    mpv_inverse_ = m_inverse_ * p_inverse_ * v_inverse_;
}

// Compute the inverse of the modelview matrix. Adjust
// the un-projection matrix.

void OpenGLView::setModelviewInverse ( void )
{
    // As far as lignumCAD goes, only rotations appear in the modelview
    // matrix. Translation and scaling of the view are handled by the
    // projection matrix.
    m_inverse_ = Space3D::Matrix( Space3D::Matrix::ROTATION, 0, 0, 1,
                                  -M_PI * view_data_.z_angle_0_/180. );
    m_inverse_ *= Space3D::Matrix( Space3D::Matrix::ROTATION, 0, 1, 0,
                                   -M_PI * view_data_.y_angle_0_/180. );
    m_inverse_ *= Space3D::Matrix( Space3D::Matrix::ROTATION, 0, 0, 1,
                                   -M_PI * view_data_.z_angle_1_/180. );

    mpv_inverse_ = m_inverse_ * p_inverse_ * v_inverse_;
}

void OpenGLView::keyPressEvent ( QKeyEvent* ke )
{
    if ( page_view_ == 0 ) return;

    if ( ke->key() == Qt::Key_Menu ) {
        context_menu_->exec( QCursor::pos() );
        return;
    }

    ViewData old_view_data = view_data_;
    bool view_changed = false;

    switch ( ke->key() ) {
    case Qt::Key_Up:
        switch ( page_view_->space() ) {
        case SPACE2D:
            pan2D( QPoint( 0, -1 ) );
            break;
        case SPACE3D:
            //TODO state == modifiers
            if ( ke->modifiers() == Qt::NoModifier )
                pan3D( QPoint( 0, -1 ) );
            //TODO modifiers == buttons
            else if ( ke->modifiers() == Qt::ControlModifier ) {
                spin3D( QPoint( 0, -1 ) );
                emit rotation( modelview_ );
            }
            break;
        }

        CommandHistory::instance().
                addCommand( new ChangeViewCommand( "change view", page_view_,
                                                   old_view_data, view_data_ ) );
        view_changed = true;
        break;
    case Qt::Key_Down:
        switch ( page_view_->space() ) {
        case SPACE2D:
            pan2D( QPoint( 0, +1 ) );
            break;
        case SPACE3D:
            if ( ke->modifiers() == Qt::NoModifier )
                pan3D( QPoint( 0, +1 ) );
            else if ( ke->modifiers() == Qt::ControlModifier ) {
                spin3D( QPoint( 0, +1 ) );
                emit rotation( modelview_ );
            }
            break;
        }

        CommandHistory::instance().
                addCommand( new ChangeViewCommand( "change view", page_view_,
                                                   old_view_data, view_data_ ) );
        view_changed = true;
        break;
    case Qt::Key_Left:
        switch ( page_view_->space() ) {
        case SPACE2D:
            pan2D( QPoint( -1, 0 ) ); // Should be a fraction of the visible view...
            break;
        case SPACE3D:
            if ( ke->modifiers() == Qt::NoModifier )
                pan3D( QPoint( -1, 0 ) );
            else if ( ke->modifiers() == Qt::ControlModifier )
                spin3D( QPoint( -1, 0 ) );
            emit rotation( modelview_ );
            break;
        }

        CommandHistory::instance().
                addCommand( new ChangeViewCommand( "change view", page_view_,
                                                   old_view_data, view_data_ ) );
        view_changed = true;
        break;
    case Qt::Key_Right:
        switch ( page_view_->space() ) {
        case SPACE2D:
            pan2D( QPoint( +1, 0 ) );
            break;
        case SPACE3D:
            if ( ke->modifiers() == Qt::NoModifier )
                pan3D( QPoint( +1, 0 ) );
            else if ( ke->modifiers() == Qt::ControlModifier )
                spin3D( QPoint( +1, 0 ) );
            emit rotation( modelview_ );
            break;
        }

        CommandHistory::instance().
                addCommand( new ChangeViewCommand( "change view", page_view_,
                                                   old_view_data, view_data_ ) );
        view_changed = true;
        break;
    case Qt::Key_PageUp:
        ++view_data_.scale_;
        scale_ = view_data_.scale_;
        switch ( page_view_->space() ) {
        case SPACE2D:
            scale2D();
            break;
        case SPACE3D:
            scale3D();
            break;
        }
        page_view_->viewAttributeChanged();
        emit scale( view_data_.scale_ );
        emit rotation( modelview_ );

        CommandHistory::instance().
                addCommand( new ChangeViewCommand( "change view", page_view_,
                                                   old_view_data, view_data_ ) );
        view_changed = true;
        break;

    case Qt::Key_PageDown:
        --view_data_.scale_;
        scale_ = view_data_.scale_;
        switch ( page_view_->space() ) {
        case SPACE2D:
            scale2D();
            break;
        case SPACE3D:
            scale3D();
            break;
        }
        page_view_->viewAttributeChanged();
        emit scale( view_data_.scale_ );
        emit rotation( modelview_ );

        CommandHistory::instance().
                addCommand( new ChangeViewCommand( "change view", page_view_,
                                                   old_view_data, view_data_ ) );
        view_changed = true;
        break;

    default:
        page_view_->keyPress( ke );
        if ( ke->isAccepted() ) view_changed = true;
    }

    if ( view_changed )
        updateGL();
}

/*!
 * Render the object in GL_SELECT mode then examine the hit buffer
 * to determine which figures were under the cursor.
 * \param me The mouse position delivered by the window system.
 * \param selected_names Filled in with the list of figures under the
 * cursor.
 */
void OpenGLView::select( QMouseEvent* me, SelectedNames& selected_names )
{
    const int SELBUFSIZE = 64;
    GLuint select_buffer[SELBUFSIZE];

    glSelectBuffer( SELBUFSIZE, select_buffer );
    glRenderMode( GL_SELECT );
    glInitNames();
    glPushName( 0 );

    glMatrixMode( GL_PROJECTION );
    glPushMatrix();
    glLoadIdentity();
    // Note: convert X window convention to OpenGL window convention for
    // y coordinate.
    gluPickMatrix( me->x(), viewport_[3] - me->y(),
            PICK_APERTURE, PICK_APERTURE,
            viewport_ );
    glOrtho( ll_corner_[X], ur_corner_[X],
             ll_corner_[Y], ur_corner_[Y],
             ll_corner_[Z], ur_corner_[Z] );

    glMatrixMode( GL_MODELVIEW );

    page_view_->select( selection_type_ );

    glMatrixMode( GL_PROJECTION );
    glPopMatrix();
    glMatrixMode( GL_MODELVIEW );

    GLint hits = glRenderMode( GL_RENDER );

    if ( hits == 0 ) return;

    GLuint* ptr = select_buffer;

    for ( GLint i = 0; i < hits; i++ ) {
        GLuint names = *ptr;

        selected_names.addRecord( ptr );

        ptr += names + 3; // Names + name count + 2 depth entries
    }

    //  cout << selected_names << endl;
}

void OpenGLView::contextMenuEvent ( QContextMenuEvent* cme )
{
    context_menu_->exec( cme->globalPos() );
    cme->accept();
}

/*
 * Restore the view to the default scale and position.
 */
void OpenGLView::restoreView ( void )
{
    if ( page_view_ == 0 ) return;

    makeCurrent();

    ViewData old_view_data = view_data_;

    view_data_.reset( page_view_->space() );

    scale_ = view_data_.scale_;

    page_view_->setViewData( view_data_ );

    switch ( page_view_->space() ) {
    case SPACE2D:
        set2DView(); break;
    case SPACE3D:
        set3DView(); break;
    }

    page_view_->viewAttributeChanged();

    emit scale( view_data_.scale_ );
    emit rotation( modelview_ );

    CommandHistory::instance().
            addCommand( new ChangeViewCommand( "change view", page_view_,
                                               old_view_data, view_data_ ) );

    updateGL();
}

void OpenGLView::zoomIn ( void )
{
    if ( page_view_ == 0 ) return;

    ViewData old_view_data = view_data_;

    --view_data_.scale_;
    scale_ = view_data_.scale_;

    switch ( page_view_->space() ) {
    case SPACE2D:
        scale2D();
        break;
    case SPACE3D:
        scale3D();
        break;
    }
    page_view_->viewAttributeChanged();
    emit scale( view_data_.scale_ );
    emit rotation( modelview_ );

    CommandHistory::instance().
            addCommand( new ChangeViewCommand( "change view", page_view_,
                                               old_view_data, view_data_ ) );

    updateGL();
}

void OpenGLView::zoomOut ( void )
{
    if ( page_view_ == 0 ) return;

    ViewData old_view_data = view_data_;

    ++view_data_.scale_;
    scale_ = view_data_.scale_;

    switch ( page_view_->space() ) {
    case SPACE2D:
        scale2D();
        break;
    case SPACE3D:
        scale3D();
        break;
    }
    page_view_->viewAttributeChanged();
    emit scale( view_data_.scale_ );
    emit rotation( modelview_ );

    CommandHistory::instance().
            addCommand( new ChangeViewCommand( "change view", page_view_,
                                               old_view_data, view_data_ ) );

    updateGL();
}

void OpenGLView::toggleCSys ( void )
{
    if ( view_data_.show_csys_ )
        view_data_.show_csys_ = false;
    else
        view_data_.show_csys_ = true;

    //TODO setON == setchecked ?
    dynamic_cast<DesignBookView*>( parentWidget() )->lCMW()->getUi()->
            toggleCSysAction->setChecked( view_data_.show_csys_ );

    page_view_->setViewData( view_data_ );

    updateGL();
}

// Look at the object from the left side

void OpenGLView::viewLeft ( void )
{
    if ( page_view_ == 0 ) return;

    makeCurrent();

    ViewData old_view_data = view_data_;

    // First, don't change the scale I guess.
    view_data_.z_angle_0_ = 0.;
    view_data_.y_angle_0_ = 90.;
    view_data_.z_angle_1_ = 0.;
    view_data_.view_point_ = Point( 0, 0, 0 );

    page_view_->setViewData( view_data_ );

    set3DView();

    page_view_->viewAttributeChanged();

    emit rotation( modelview_ );

    CommandHistory::instance().
            addCommand( new ChangeViewCommand( "change view", page_view_,
                                               old_view_data, view_data_ ) );

    updateGL();
}

// Look at the object from the right side

void OpenGLView::viewRight ( void )
{
    if ( page_view_ == 0 ) return;

    makeCurrent();

    ViewData old_view_data = view_data_;

    // First, don't change the scale I guess.
    view_data_.z_angle_0_ = 0.;
    view_data_.y_angle_0_ = -90.;
    view_data_.z_angle_1_ = 0.;
    view_data_.view_point_ = Point( 0, 0, 0 );

    page_view_->setViewData( view_data_ );

    set3DView();

    page_view_->viewAttributeChanged();

    emit rotation( modelview_ );

    CommandHistory::instance().
            addCommand( new ChangeViewCommand( "change view", page_view_,
                                               old_view_data, view_data_ ) );

    updateGL();
}

// Look at the object from the front

void OpenGLView::viewFront ( void )
{
    if ( page_view_ == 0 ) return;

    makeCurrent();

    ViewData old_view_data = view_data_;

    // First, don't change the scale I guess.
    view_data_.z_angle_0_ = 90.;
    view_data_.y_angle_0_ = 90.;
    view_data_.z_angle_1_ = 90.;
    view_data_.view_point_ = Point( 0, 0, 0 );

    page_view_->setViewData( view_data_ );

    set3DView();

    page_view_->viewAttributeChanged();

    emit rotation( modelview_ );

    CommandHistory::instance().
            addCommand( new ChangeViewCommand( "change view", page_view_,
                                               old_view_data, view_data_ ) );

    updateGL();
}

// Look at the object from the back

void OpenGLView::viewBack ( void )
{
    if ( page_view_ == 0 ) return;

    makeCurrent();

    ViewData old_view_data = view_data_;

    // First, don't change the scale I guess.
    view_data_.z_angle_0_ = 90.;
    view_data_.y_angle_0_ = -90.;
    view_data_.z_angle_1_ = -90.;
    view_data_.view_point_ = Point( 0, 0, 0 );

    page_view_->setViewData( view_data_ );

    set3DView();

    page_view_->viewAttributeChanged();

    emit rotation( modelview_ );

    CommandHistory::instance().
            addCommand( new ChangeViewCommand( "change view", page_view_,
                                               old_view_data, view_data_ ) );

    updateGL();
}

// Look at the object from the bottom

void OpenGLView::viewBottom ( void )
{
    if ( page_view_ == 0 ) return;

    makeCurrent();

    ViewData old_view_data = view_data_;

    // First, don't change the scale I guess.
    view_data_.z_angle_0_ = 0.;
    view_data_.y_angle_0_ = 180.;
    view_data_.z_angle_1_ = 0.;
    view_data_.view_point_ = Point( 0, 0, 0 );

    page_view_->setViewData( view_data_ );

    set3DView();

    page_view_->viewAttributeChanged();

    emit rotation( modelview_ );

    CommandHistory::instance().
            addCommand( new ChangeViewCommand( "change view", page_view_,
                                               old_view_data, view_data_ ) );

    updateGL();
}

// Look at the object from the top (may well be the default) (well,
// it's the default orientation, but the scale doesn't change)

void OpenGLView::viewTop ( void )
{
    if ( page_view_ == 0 ) return;

    makeCurrent();

    ViewData old_view_data = view_data_;

    // First, don't change the scale I guess.
    view_data_.z_angle_0_ = 0.;
    view_data_.y_angle_0_ = 0.;
    view_data_.z_angle_1_ = 0.;
    view_data_.view_point_ = Point( 0, 0, 0 );

    page_view_->setViewData( view_data_ );

    set3DView();

    page_view_->viewAttributeChanged();

    emit rotation( modelview_ );

    CommandHistory::instance().
            addCommand( new ChangeViewCommand( "change view", page_view_,
                                               old_view_data, view_data_ ) );

    updateGL();
}

// Well, not all the state is available from this object. Some
// is in the OpenGL context. So, make this current before
// asking the graphics object to update itself.

void OpenGLView::updateAttributes ( void )
{
    if ( page_view_ == 0 ) return;

    makeCurrent();

    page_view_->viewAttributeChanged();

    updateGL();
}

// Almost everything about the background is dynamic except the
// (possible) texture background.

void OpenGLView::updateBackground ( void )
{
    makeCurrent();

    if ( OpenGLGlobals::instance()->backgroundStyle() == lC::Background::PATTERN )
        createBackgroundTexture( OpenGLGlobals::instance()->patternFile(),
                                 OpenGLGlobals::instance()->backgroundColor(),
                                 OpenGLGlobals::instance()->gradientColor() );
    updateGL();
}

// Draw a coordinate system at the origin (will definitely need to
// generalize this eventually)

void OpenGLView::csys ( void )
{
    const GLubyte quads[6][4] = {
        { 0, 1, 2, 3 },
        { 4, 7, 6, 5 },
        { 5, 6, 2, 1 },
        { 7, 4, 0, 3 },
        { 4, 5, 1, 0 },
        { 6, 7, 3, 2 }
    };

    GLdouble vertices[8][3] = {
        { scale_/8, 0,        scale_/8 },
        { scale_/8, scale_/8, scale_/8 },
        { 0,        scale_/8, scale_/8 },
        { 0,        0,        scale_/8 },
        { scale_/8, 0,        0        },
        { scale_/8, scale_/8, 0        },
        { 0,        scale_/8, 0        },
        { 0,        0,        0        }
    };

    qglColor( annotationColor() );
    arrow_.draw( Point(), Point( scale_/2, 0, 0 ) );
    arrow_.draw( Point(), Point( 0, scale_/2, 0 ) );
    arrow_.draw( Point(), Point( 0, 0, scale_/2 ) );
    FaceData face_data( dimensionFont(), 0, annotationColor().rgb() );
    font( face_data )->draw( scale_/2, 0, 0, "X" );
    font( face_data )->draw( 0, scale_/2, 0, "Y" );
    font( face_data )->draw( 0, 0, scale_/2, "Z" );
    glEnable( GL_LIGHTING );
    glEnable( GL_LIGHT0 );
    GLfloat std[] = { 0.8, 0.8, 0.8, 1 };
    glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, std );
    glEnableClientState( GL_VERTEX_ARRAY );
    glVertexPointer( 3, GL_FLOAT, 0, vertices );
    glNormal3f( 0, 0, 1 );
    glDrawElements( GL_QUADS, 4, GL_UNSIGNED_BYTE, quads[0] );
    glNormal3f( 0, 0, -1 );
    glDrawElements( GL_QUADS, 4, GL_UNSIGNED_BYTE, quads[1] );
    glNormal3f( 0, 1, 0 );
    glDrawElements( GL_QUADS, 4, GL_UNSIGNED_BYTE, quads[2] );
    glNormal3f( 0, -1, 0 );
    glDrawElements( GL_QUADS, 4, GL_UNSIGNED_BYTE, quads[3] );
    glNormal3f( 1, 0, 0 );
    glDrawElements( GL_QUADS, 4, GL_UNSIGNED_BYTE, quads[4] );
    glNormal3f( -1, 0, 0 );
    glDrawElements( GL_QUADS, 4, GL_UNSIGNED_BYTE, quads[5] );
    glDisable( GL_LIGHTING );
    glDisableClientState( GL_VERTEX_ARRAY );
}
