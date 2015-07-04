/*
 * partview.cpp
 *
 * PartView class: a view of a Part.
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
#include <qradiobutton.h>
#include <qpushbutton.h>

#include "constants.h"
#include "units.h"
#include "command.h"
#include "model.h"
#include "lcdefaultlengthspinbox.h"
#include "openglview.h"
#include "lignumcadmainwindow.h"
#include "designbookview.h"
#include "listviewitem.h"
#include "newpartwizard.h"
#include "materialdialog.h"
#include "material.h"
#include "ocsolid.h"
#include "ocsolidview.h"
#include "pageinfodialog.h"
#include "pagefactory.h"
#include "partview.h"

/*!
 * PartViewCreate is a memento of the current state of a Part and its view.
 * It handles creating and deleting both objects.
 */
class PartViewCreate : public CreateObject {
    //! When CommandHistory undoes or redoes the creation of this object,
    //! the Design Book view is the only handle which is required. Everything
    //! else can be looked up.
    DesignBookView* design_book_view_;
    //! The details of the part and view are stored as XML.
    QDomDocument xml_doc_;
public:
    /*!
   * Create a memento of the current state of the part and its view.
   * \param part_view view object of the part.
   */
    PartViewCreate ( PartView* part_view )
    {
        design_book_view_ = part_view->parent();

        QDomElement root = xml_doc_.createElement( lC::STR::MEMENTO );

        root.setAttribute( lC::STR::NAME, part_view->dbURL().toString() );

        part_view->part()->write( root );
        part_view->write( root );

        xml_doc_.appendChild( root );
    }
    //! Destructor does not do much.
    ~PartViewCreate ( void ) {}
    /*!
   * (Re)create the part object and its view as specified in
   * the XML representation.
   */
    void create ( void )
    {
        QDomNodeList part_list = xml_doc_.elementsByTagName( lC::STR::PART );

        if ( part_list.length() > 0 ) {

            QDomNodeList part_view_list =
                    xml_doc_.elementsByTagName( lC::STR::PART_VIEW );

            if ( part_view_list.length() > 0 ) {
                uint part_id =
                        part_list.item(0).toElement().attribute( lC::STR::ID ).toUInt();

                new Part( part_id, part_list.item(0).toElement(),
                          design_book_view_->model() );

                PartView* part_view =
                        new PartView( design_book_view_, part_view_list.item(0).toElement() );

                design_book_view_->addPageView( part_view );
                design_book_view_->showPageView( part_view );
            }
        }
    }
    /*!
   * Delete the part object and its view as specified in the
   * XML representation.
   */
    void remove ( void )
    {
        QDomNodeList part_view_list =xml_doc_.elementsByTagName(lC::STR::PART_VIEW);

        if ( part_view_list.length() > 0 ) {

            QDomElement part_view_element = part_view_list.item(0).toElement();

            QString part_path = part_view_element.attribute( lC::STR::PART );

            PartView* part_view =
                    dynamic_cast<PartView*>( design_book_view_->lookup( part_path ) );

            design_book_view_->deletePage( part_view );
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

        QDomNodeList part_list = xml_doc_.elementsByTagName( lC::STR::PART );

        if ( part_list.length() > 0 ) {

            QDomNode element = xml_rep->importNode( part_list.item(0), true );
            rep_root.appendChild( element );
        }

        QDomNodeList part_view_list =
                xml_doc_.elementsByTagName( lC::STR::PART_VIEW );

        if ( part_view_list.length() > 0 ) {

            QDomNode element = xml_rep->importNode( part_view_list.item(0), true );
            rep_root.appendChild( element );
        }
    }
    /*!
   * If the creation of a part is immediately followed by a rename,
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

            QDomNodeList part_list = xml_doc_.elementsByTagName( lC::STR::PART );
            QDomNodeList part_view_list =
                    xml_doc_.elementsByTagName( lC::STR::PART_VIEW );

            if ( part_list.length() == 0 || part_view_list.length() == 0 )
                return false;

            // Update the name elements in the object and it's view.

            memento_list.item(0).toElement().setAttribute( lC::STR::NAME,
                                                           rename->newDBURL().toString() );

            part_list.item(0).toElement().
                    setAttribute( lC::STR::NAME, rename->newDBURL().name() );

            part_view_list.item(0).toElement().setAttribute( lC::STR::PART,
                                                             rename->newDBURL().toString() );

            // The part might also have a solid which needs to be renamed.
            QDomNodeList solid_list = xml_doc_.elementsByTagName( lC::STR::SOLID );
            if ( solid_list.length() > 0 ) {
                solid_list.item(0).toElement().
                        setAttribute( lC::STR::NAME, rename->newDBURL().name() );
            }

            return true;
        }

        return false;
    }
};


/*!
 * Change the material of part.
 */
class ChangeMaterialCommand : public Command {
    //! Used to lookup the part.
    Model* model_;
    //! Name of the part.
    DBURL db_url_;
    //! The old material name.
    QString old_material_;
    //! The new material name.
    QString new_material_;

public:
    ChangeMaterialCommand ( const QString& name, Part* part,
                            const QString& old_material, const QString& new_material )
        : Command( name ),
          model_( part->model() ), db_url_( part->dbURL() ),
          old_material_( old_material ), new_material_( new_material )
    {}
    ~ChangeMaterialCommand ( void )
    {}

    Type type ( void ) const { return ChangeMaterial; }

    bool merge ( Command* /*command*/ ) { return false; }

    void execute ( void )
    {
        Part* part =
                dynamic_cast<Part*>( model_->lookup( db_url_ ) );

        part->setSolidMaterial( MaterialDatabase::instance().material( new_material_ ));
    }

    void unexecute ( void )
    {
        Part* part =
                dynamic_cast<Part*>( model_->lookup( db_url_ ) );

        part->setSolidMaterial( MaterialDatabase::instance().material( old_material_ ));
    }

    void write ( QDomDocument* document ) const
    {
        QDomElement material_element =
                document->createElement( lC::STR::CHANGE_MATERIAL );

        material_element.setAttribute( lC::STR::NAME, db_url_.toString() );
        material_element.setAttribute( lC::STR::OLD_MATERIAL, old_material_ );
        material_element.setAttribute( lC::STR::NEW_MATERIAL, new_material_ );

        QDomNode root_node = document->firstChild();
        if ( !root_node.isNull() )
            document->replaceChild( material_element, root_node );
        else
            document->appendChild( material_element );
    }
};

NewPartWizard* PartView::new_part_wizard_ = 0;

MaterialDialog* PartView::material_dialog_ = 0;

PageInfoDialog* PartView::config_dialog_ = 0;

PartView::PartView ( Part* part, DesignBookView* parent )
    : PageView( parent ), part_( part )
{
    init();
}

PartView::PartView ( DesignBookView* parent, const QDomElement& xml_rep )
    : PageView( parent )
{
    DBURL db_url( xml_rep.attribute( lC::STR::PART ) );

    part_ = dynamic_cast<Part*>( model()->lookup( db_url ) );

    if ( part_ == 0 )		// This should be a fatal error.
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
            else if ( e.tagName() == lC::STR::SOLID_VIEW ) {
                addFigureView( new Space3D::OCSolidView( e, this ) );
            }
        }
        n = n.nextSibling();
    }
#if 0
    // Evidently, SolidView has no persistent attributes(?)
    addFigureView( new Space3D::OCSolidView( part_->solid(), this ) );
#endif
}

PartView::~PartView ( void )
{
    // Even though the figure views are automatically deleted, we still
    // have to remove them first so that deleting this part view's list item
    // does not try to delete (the now deleted) figure view's list items.
    clearFigureViews();

    delete list_view_item_;

    if ( part_ )
        model()->removePage( part_ );
}

// The usual init routine.

void PartView::init ( void )
{
    tabIcon = lC::lookupPixmap( "part.png" );
    tabText = lC::formatTabName( part_->name()  );

    ListViewItem* previous_item = parent()->previousItem( part_->id() );

    list_view_item_ = new ListViewItem( parent()->modelListItem(), previous_item );

    list_view_item_->setData( lC::formatName( part_->name() )
                              + QString( " <%1>" ).arg( part_->id() ),
                              lC::NAME );
    list_view_item_->setData( trC( lC::STR::PART ), lC::TYPE );
    //TODO see how to implement
    //list_view_item_->setOpen( true );
    //list_view_item_->setRenameEnabled( lC::NAME, true );
    //list_view_item_->listView()->ensureItemVisible( list_view_item_ );

    connect( this, SIGNAL( newInformation( const QString& ) ),
             lCMW(), SLOT( updateInformation( const QString& ) ) );

    //TODO see how to implement
    //connect( list_view_item_, SIGNAL( nameChanged( const QString& ) ),
    //   SLOT( listNameChanged( const QString& ) ) );

    connect( part_, SIGNAL( nameChanged( const QString& ) ),
             SLOT( updateName( const QString& ) ) );

    if ( material_dialog_ == 0 ) {
        material_dialog_ = new MaterialDialog( lCMW() );
        material_dialog_->setObjectName("material dialog");
    }

    if ( new_part_wizard_ == 0 ) {
        new_part_wizard_ = new NewPartWizard( lCMW() );
        material_dialog_->setObjectName("new part wizard");
    }

    if ( config_dialog_ == 0 )
        config_dialog_ = new PageInfoDialog;
}

// Do any post-creation configuration.

bool PartView::configure ( void )
{
    new_part_wizard_->setPartView( this );
    //TODO not needed ?
    //new_part_wizard_->showPage( new_part_wizard_->initialPartPage );

    new_part_wizard_->getUi()->nameEdit->setText( lC::formatName( part_->name() ) );

    new_part_wizard_->getUi()->partLibraryListView->setFocus();
    //TODO not needed ?
    //new_part_wizard_->getUi()->nextButton()->setDefault( true );

    int ret = new_part_wizard_->exec();

    if ( ret == QDialog::Rejected ) return false;

    if ( new_part_wizard_->getUi()->nameEdit->isModified() )
        part_->setName( new_part_wizard_->getUi()->nameEdit->text() );

    part_->makeSolidParameters( new_part_wizard_->part(),
                                new_part_wizard_->parameters() );

    addFigureView( new Space3D::OCSolidView( part_->solid(), this ) );

    figureComplete();

    return true;
}

CreateObject* PartView::memento ( void )
{
    return new PartViewCreate( this );
}

void PartView::setName ( const QString& name )
{
    DBURL old_db_url = part_->dbURL();

    part_->setName( name );

    CommandHistory::instance().addCommand( new RenameCommand( "rename part",
                                                              model(),
                                                              old_db_url,
                                                              part_->dbURL() ) );
}

// Check a list view rename for uniqueness.

void PartView::listNameChanged ( const QString& name )
{
    int ret = parent()->uniquePageName( this, name, part_->type() );

    switch ( ret ) {
    case lC::OK:
        setName( name );
        break;
    case lC::Redo:
        //TODO see how to implement this
        //list_view_item_->startRename( lC::NAME );
    case lC::Rejected:
        updateName( part_->name() ); // Repaint list item with old name.
    }
}

void PartView::show ( void ) const
{
    lCMW()->getUi()->toolMenu->clear();

    lCMW()->getUi()->toolMenu->addAction( lCMW()->getUi()->toolStraightCutAction );
    lCMW()->getUi()->toolMenu->addAction( lCMW()->getUi()->toolFreeCutAction );
    lCMW()->getUi()->toolMenu->addAction( lCMW()->getUi()->toolHoleAction );
    lCMW()->getUi()->toolMenu->addAction( lCMW()->getUi()->toolEdgeTreatmentAction );
    lCMW()->getUi()->toolMenu->addSeparator();
    lCMW()->getUi()->toolMenu->addAction( lCMW()->getUi()->toolMaterialAction );

    lCMW()->getUi()->toolStraightCutAction->disconnect();
    lCMW()->getUi()->toolFreeCutAction->disconnect();
    lCMW()->getUi()->toolHoleAction->disconnect();
    lCMW()->getUi()->toolEdgeTreatmentAction->disconnect();
    lCMW()->getUi()->toolMaterialAction->disconnect();

    connect( lCMW()->getUi()->toolMaterialAction, SIGNAL( activated() ),
             SLOT( setMaterial() ) );
}

/*!
 * Add this view's XML description to the output documennt
 */
void PartView::write ( QDomElement& xml_rep ) const
{
    QDomDocument document = xml_rep.ownerDocument();
    QDomElement view_element = document.createElement( lC::STR::PART_VIEW );
    view_element.setAttribute( lC::STR::PART, part_->dbURL().toString( ) );

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
void PartView::startDisplay ( QMenu* context_menu )
{
    context_menu_ = context_menu;

    context_menu_->addSeparator();
    wireframe_id_ = context_menu_->addAction( tr( "Wireframe" ) );
    wireframe_id_->setCheckable(true);
    hidden_id_ = context_menu_->addAction( tr( "Hidden Line" ) );
    hidden_id_->setCheckable(true);
    solid_id_ = context_menu_->addAction( tr( "Solid" ) );
    solid_id_->setCheckable(true);
    texture_id_ = context_menu_->addAction( tr( "Texture" ) );
    texture_id_->setCheckable(true);

    connect( view(), SIGNAL( rotation( const GLdouble* ) ),
             SIGNAL( orientationChanged( const GLdouble* ) ) );

    switch ( renderStyle() ) {
    case lC::Render::WIREFRAME:
        wireframe_id_->setChecked(true);
        break;
    case lC::Render::HIDDEN:
        hidden_id_->setChecked(true);
        connect( view(), SIGNAL( rotation( const GLdouble* ) ),
                 SIGNAL( orientationChangedHidden( const GLdouble* ) ) );
        break;
    case lC::Render::SOLID:
        solid_id_->setChecked(true);
        break;
    case lC::Render::TEXTURED:
        texture_id_->setChecked(true);
        break;
    }

    connect( part_, SIGNAL( nameChanged( const QString& ) ),
             lCMW(), SLOT( pageChanged( const QString& ) ) );
}
/*
 * Clean up when are not the current page.
 */
void PartView::stopDisplay ( QMenu* /*context_menu*/ )
{
    /*
   * When we are not the current page, don't respond to changes in the view
   * orientation.
   */
    disconnect( view(), SIGNAL( rotation( const GLdouble* ) ),
                this, SIGNAL( orientationChanged( const GLdouble* ) ) );
    if ( renderStyle() == lC::Render::HIDDEN )
        disconnect( view(), SIGNAL( rotation( const GLdouble* ) ),
                    this, SIGNAL( orientationChangedHidden( const GLdouble* ) ) );

    disconnect( part_, SIGNAL( nameChanged( const QString& ) ),
                lCMW(), SLOT( pageChanged( const QString& ) ) );
}
/*
 * Draw the view.
 */
void PartView::draw ( void ) const
{
    glPushAttrib( GL_LIGHTING_BIT );

    glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

    // Then, draw all the figures on the page (well, technically,
    // there should be only one figure: The Part)

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
void PartView::select ( SelectionType /*select_type*/ ) const
{
    QListIterator< std::shared_ptr<FigureViewBase> > f( figureViews() );

    while(f.hasNext())
        f.next()->select( selectionType() );
}

void PartView::updateName ( const QString& /*name*/ )
{
    tabText = lC::formatTabName( part_->name() );
    list_view_item_->setData( lC::formatName( part_->name() )
                              + QString( " <%1>" ).arg( part_->id() ),
                              lC::NAME );
}

// Allow the user to switch among the various rendering styles.

void PartView::toggleRenderStyle (QAction *id )
{
    if ( renderStyle() == lC::Render::HIDDEN && id != hidden_id_ )
        disconnect( view(), SIGNAL( rotation( const GLdouble* ) ),
                    this, SIGNAL( orientationChangedHidden( const GLdouble* ) ) );

    if ( id == wireframe_id_ and renderStyle() != lC::Render::WIREFRAME ) {
        wireframe_id_->setChecked(true);
        hidden_id_->setChecked(false);
        solid_id_->setChecked(false);
        texture_id_->setChecked(false);

        setRenderStyle( lC::Render::WIREFRAME );
    }
    else if ( id == hidden_id_ and renderStyle() != lC::Render::HIDDEN ) {
        wireframe_id_->setChecked(false);
        hidden_id_->setChecked(true);
        solid_id_->setChecked(false);
        texture_id_->setChecked(false);


        setRenderStyle( lC::Render::HIDDEN );

        connect( view(), SIGNAL( rotation( const GLdouble* ) ),
                 SIGNAL( orientationChangedHidden( const GLdouble* ) ) );

        emit orientationChangedHidden( view()->viewOrientation() );
    }
    else if ( id == solid_id_ and renderStyle() != lC::Render::SOLID ) {
        wireframe_id_->setChecked(false);
        hidden_id_->setChecked(true);
        solid_id_->setChecked(false);
        texture_id_->setChecked(false);


        setRenderStyle( lC::Render::SOLID );
    }
    else if ( id == texture_id_ and renderStyle() != lC::Render::TEXTURED ) {
        wireframe_id_->setChecked(false);
        hidden_id_->setChecked(false);
        solid_id_->setChecked(false);
        texture_id_->setChecked(true);


        setRenderStyle( lC::Render::TEXTURED );
    }

    view()->updateGL();
}

/*
 * Determine which figure to create from its simplified XML representation.
 */
void PartView::pasteFigure ( const QDomElement& /*xml_rep*/ )
{
}

void PartView::cancelOperation ( void )
{
    SelectedNames none;
    highlightFigures( none );

    deactivateFigures();

    emit newInformation( trC( lC::STR::NONE ) );
}

void PartView::setMaterial ( void )
{
    if ( part_->solid() == 0 ) return; // Really an error, should be avoided...

    material_dialog_->setMaterial( part_->solid()->material() );

    int ret = material_dialog_->exec();

    if ( ret == QDialog::Rejected ) return;

    QListWidgetItem* current_item =  material_dialog_->getUi()->MaterialList->currentItem();
    if ( current_item != 0 ) {

        QString old_material;
        if ( part_->solid()->material() != 0 )
            old_material = part_->solid()->material()->name();

        CommandHistory::instance().
                addCommand( new ChangeMaterialCommand( "change material", part_,
                                                       old_material,
                                                       current_item->text( ) ) );

        part_->setSolidMaterial( MaterialDatabase::instance().
                                 material( current_item->text( ) ) );
    }
}

/*!
 * Instance of page metadata for a part.
 */
class PartViewMetadata : public PageMetadata {
public:
    /*!
   * Construct a (what amounts to a const) part metadata object.
   */
    PartViewMetadata ( void )
        : PageMetadata( lC::STR::PART,
                        "new_part.png",
                        QT_TRANSLATE_NOOP( "lignumCADMainWindow", "&Part" ),
                        "&Part...",
                        "Insert a new Part",
                        "<p><b>Insert|Part</b></p><p>Click this button to create a new part. You will be prompted to specify the basic size of the stock from which a single piece of the model will be &quot;milled&quot;.</p>" )

    {
        //    cout << "Constructing part metadata" << endl;
        PageFactory::instance()->addPageMetadata( Part::PAGE_ID, this );
    }

    /*!
   * Create a new part and its view.
   * \return view handle for the part and its view.
   */
    PartView* create ( DesignBookView* parent ) const
    {
        QString name = parent->uniqueName( &Part::newName, lC::STR::PART );

        Part* part = new Part( parent->model()->newID(), name, parent->model() );
        PartView* part_view = new PartView( part, parent );
        parent->addPageView( part_view );
        return part_view;
    }

    /*!
   * Create a part from its XML representation.
   * \return handle for the part.
   */
    Part* create ( Model* parent, const QDomElement& xml_rep ) const
    {
        uint part_id = xml_rep.attribute( lC::STR::ID ).toUInt();
        return new Part( part_id, xml_rep, parent );
    }

    /*!
   * Create a part view from its XML representation.
   * \return handle for the part view.
   */
    PartView* create ( DesignBookView* parent, const QDomElement& xml_rep ) const
    {
        PartView* part_view = new PartView( parent, xml_rep );
        parent->addPageView( part_view );
        return part_view;
    }

    /*!
   * Retrieve the QAction which can create this page.
   * \param lCMW pointer to the main window where the actions are stored.
   * \return the action associated with creating an instance of this page.
   */
    QAction* action ( lignumCADMainWindow* lCMW ) const
    {
        return lCMW->getUi()->insertPartAction;
    }
};

PartViewMetadata part_view_metadata;
