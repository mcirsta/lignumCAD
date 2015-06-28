/****************************************************************************
** Form implementation generated from reading ui file 'newpartwizard.ui'
**
** Created: Wed Dec 18 08:54:56 2002
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#include "newpartwizard.h"

#include <qvariant.h>
#include <qapplication.h>
#include <qgroupbox.h>
#include <qheader.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qlistview.h>
#include <qpushbutton.h>
#include <qwidget.h>
#include <qmime.h>
#include <qdragobject.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qimage.h>
#include <qpixmap.h>

#include "constants.h"
#include "designbookview.h"
#include "lcdefaultlengthconstraint/lcdefaultlengthconstraint.h"
#include "part.h"
#include "partview.h"
#include "newpartwizard.ui.h"
static QPixmap uic_load_pixmap_NewPartWizard( const QString &name )
{
    const QMimeSource *m = QMimeSourceFactory::defaultFactory()->data( name );
    if ( !m )
	return QPixmap();
    QPixmap pix;
    QImageDrag::decode( m, pix );
    return pix;
}
/* 
 *  Constructs a NewPartWizard which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f'.
 *
 *  The wizard will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal wizard.
 */
NewPartWizard::NewPartWizard( QWidget* parent,  const char* name, bool modal, WFlags fl )
    : QWizard( parent, name, modal, fl )
{
    if ( !name )
	setName( "NewPartWizard" );
    resize( 563, 462 ); 
    setCaption( trUtf8( "New Part Wizard" ) );
    setIcon( uic_load_pixmap_NewPartWizard( "lignumCAD.png" ) );

    initialPartPage = new QWidget( this, "initialPartPage" );
    initialPartPageLayout = new QGridLayout( initialPartPage, 1, 1, 11, 6, "initialPartPageLayout"); 

    Layout6 = new QVBoxLayout( 0, 0, 6, "Layout6"); 
    QSpacerItem* spacer = new QSpacerItem( 0, 51, QSizePolicy::Minimum, QSizePolicy::Expanding );
    Layout6->addItem( spacer );

    PixmapLabel5 = new QLabel( initialPartPage, "PixmapLabel5" );
    PixmapLabel5->setPixmap( uic_load_pixmap_NewPartWizard( "machines.png" ) );
    PixmapLabel5->setScaledContents( FALSE );
    Layout6->addWidget( PixmapLabel5 );
    QSpacerItem* spacer_2 = new QSpacerItem( 0, 61, QSizePolicy::Minimum, QSizePolicy::Expanding );
    Layout6->addItem( spacer_2 );

    initialPartPageLayout->addMultiCellLayout( Layout6, 0, 1, 0, 0 );

    nameEdit = new QLineEdit( initialPartPage, "nameEdit" );
    QToolTip::add( nameEdit, trUtf8( "Edit the name of the part." ) );
    QWhatsThis::add( nameEdit, trUtf8( "<p><b>Part Name</b></p>\n"
"Enter a name for the part." ) );

    initialPartPageLayout->addWidget( nameEdit, 0, 2 );

    nameLabel = new QLabel( initialPartPage, "nameLabel" );
    nameLabel->setText( trUtf8( "N&ame:" ) );

    initialPartPageLayout->addWidget( nameLabel, 0, 1 );

    partLibraryGroupBox = new QGroupBox( initialPartPage, "partLibraryGroupBox" );
    partLibraryGroupBox->setTitle( trUtf8( "&Initial Part Library" ) );
    partLibraryGroupBox->setColumnLayout(0, Qt::Vertical );
    partLibraryGroupBox->layout()->setSpacing( 6 );
    partLibraryGroupBox->layout()->setMargin( 11 );
    partLibraryGroupBoxLayout = new QGridLayout( partLibraryGroupBox->layout() );
    partLibraryGroupBoxLayout->setAlignment( Qt::AlignTop );

    partLibraryListView = new QListView( partLibraryGroupBox, "partLibraryListView" );
    partLibraryListView->addColumn( trUtf8( "Parts" ) );
    partLibraryListView->addColumn( trUtf8( "Example" ) );
    partLibraryListView->header()->setClickEnabled( FALSE, partLibraryListView->header()->count() - 1 );
    QToolTip::add( partLibraryListView, trUtf8( "Select a template part from the list." ) );
    QWhatsThis::add( partLibraryListView, trUtf8( "<p><b>Part Library</b></p>\n"
"<p>\n"
"This list contains the available template parts. The parts are categorized by group. <em>Blanks</em> are generally used in milling operations to create custom parts. You can also modify the more specific parts, but this is generally not necessary. \n"
"</p>" ) );

    partLibraryGroupBoxLayout->addWidget( partLibraryListView, 0, 0 );

    initialPartPageLayout->addMultiCellWidget( partLibraryGroupBox, 1, 1, 1, 2 );
    addPage( initialPartPage, trUtf8( "Select an initial form for the part" ) );

    partParametersPage = new QWidget( this, "partParametersPage" );
    partParametersPageLayout = new QGridLayout( partParametersPage, 1, 1, 11, 6, "partParametersPageLayout"); 

    Layout6_2 = new QVBoxLayout( 0, 0, 6, "Layout6_2"); 
    QSpacerItem* spacer_3 = new QSpacerItem( 0, 51, QSizePolicy::Minimum, QSizePolicy::Expanding );
    Layout6_2->addItem( spacer_3 );

    PixmapLabel5_2 = new QLabel( partParametersPage, "PixmapLabel5_2" );
    PixmapLabel5_2->setPixmap( uic_load_pixmap_NewPartWizard( "machines.png" ) );
    PixmapLabel5_2->setScaledContents( FALSE );
    Layout6_2->addWidget( PixmapLabel5_2 );
    QSpacerItem* spacer_4 = new QSpacerItem( 0, 61, QSizePolicy::Minimum, QSizePolicy::Expanding );
    Layout6_2->addItem( spacer_4 );

    partParametersPageLayout->addLayout( Layout6_2, 0, 0 );

    partParameterFrame = new QGroupBox( partParametersPage, "partParameterFrame" );
    partParameterFrame->setTitle( trUtf8( "&Parameters" ) );
    partParameterFrame->setColumnLayout(0, Qt::Vertical );
    partParameterFrame->layout()->setSpacing( 6 );
    partParameterFrame->layout()->setMargin( 11 );
    partParameterFrameLayout = new QGridLayout( partParameterFrame->layout() );
    partParameterFrameLayout->setAlignment( Qt::AlignTop );

    partParametersPageLayout->addWidget( partParameterFrame, 0, 1 );
    addPage( partParametersPage, trUtf8( "Enter the parameters for the part" ) );

    // signals and slots connections
    connect( partLibraryListView, SIGNAL( currentChanged(QListViewItem*) ), this, SLOT( partLibraryListView_currentChanged(QListViewItem*) ) );
    connect( this, SIGNAL( selected(const QString&) ), this, SLOT( NewPartWizard_selected(const QString&) ) );
    connect( this, SIGNAL( helpClicked() ), this, SLOT( NewPartWizard_helpClicked() ) );

    // buddies
    nameLabel->setBuddy( nameEdit );
    init();
}

/*  
 *  Destroys the object and frees any allocated resources
 */
NewPartWizard::~NewPartWizard()
{
    // no need to delete child widgets, Qt does it all for us
}

