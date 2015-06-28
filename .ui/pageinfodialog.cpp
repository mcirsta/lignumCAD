/****************************************************************************
** Form implementation generated from reading ui file 'pageinfodialog.ui'
**
** Created: Wed Dec 18 08:54:53 2002
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#include "pageinfodialog.h"

#include <qvariant.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qwhatsthis.h>
#include <qmime.h>
#include <qdragobject.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qimage.h>
#include <qpixmap.h>

#include "pageinfodialog.ui.h"
static QPixmap uic_load_pixmap_PageInfoDialog( const QString &name )
{
    const QMimeSource *m = QMimeSourceFactory::defaultFactory()->data( name );
    if ( !m )
	return QPixmap();
    QPixmap pix;
    QImageDrag::decode( m, pix );
    return pix;
}
/* 
 *  Constructs a PageInfoDialog which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
PageInfoDialog::PageInfoDialog( QWidget* parent,  const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
	setName( "PageInfoDialog" );
    resize( 308, 81 ); 
    setCaption( trUtf8( "Rename Page" ) );
    setIcon( uic_load_pixmap_PageInfoDialog( "lignumCAD.png" ) );
    setSizeGripEnabled( FALSE );
    PageInfoDialogLayout = new QGridLayout( this, 1, 1, 11, 6, "PageInfoDialogLayout"); 

    Layout1 = new QHBoxLayout( 0, 0, 6, "Layout1"); 

    buttonHelp = new QPushButton( this, "buttonHelp" );
    buttonHelp->setText( trUtf8( "&Help" ) );
    buttonHelp->setAccel( 276824136 );
    buttonHelp->setAutoDefault( TRUE );
    QToolTip::add( buttonHelp, trUtf8( "Get the Help message for this dialog." ) );
    Layout1->addWidget( buttonHelp );
    QSpacerItem* spacer = new QSpacerItem( 20, 0, QSizePolicy::Expanding, QSizePolicy::Minimum );
    Layout1->addItem( spacer );

    buttonOk = new QPushButton( this, "buttonOk" );
    buttonOk->setText( trUtf8( "&OK" ) );
    buttonOk->setAccel( 276824143 );
    buttonOk->setAutoDefault( TRUE );
    buttonOk->setDefault( TRUE );
    QToolTip::add( buttonOk, trUtf8( "Accept the page name change." ) );
    Layout1->addWidget( buttonOk );

    buttonCancel = new QPushButton( this, "buttonCancel" );
    buttonCancel->setText( trUtf8( "&Cancel" ) );
    buttonCancel->setAccel( 276824131 );
    buttonCancel->setAutoDefault( TRUE );
    QToolTip::add( buttonCancel, trUtf8( "Cancel changing the page name." ) );
    Layout1->addWidget( buttonCancel );

    PageInfoDialogLayout->addMultiCellLayout( Layout1, 1, 1, 0, 1 );

    nameLabel = new QLabel( this, "nameLabel" );
    nameLabel->setText( trUtf8( "&Name:" ) );

    PageInfoDialogLayout->addWidget( nameLabel, 0, 0 );

    nameEdit = new QLineEdit( this, "nameEdit" );
    QToolTip::add( nameEdit, trUtf8( "Edit page name." ) );
    QWhatsThis::add( nameEdit, trUtf8( "<p><b>Page Name</b></p>\n"
"<p>Enter a new name for the page.</p>" ) );

    PageInfoDialogLayout->addWidget( nameEdit, 0, 1 );

    // signals and slots connections
    connect( buttonOk, SIGNAL( clicked() ), this, SLOT( accept() ) );
    connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
    connect( buttonHelp, SIGNAL( clicked() ), this, SLOT( buttonHelp_clicked() ) );

    // buddies
    nameLabel->setBuddy( nameEdit );
}

/*  
 *  Destroys the object and frees any allocated resources
 */
PageInfoDialog::~PageInfoDialog()
{
    // no need to delete child widgets, Qt does it all for us
}

