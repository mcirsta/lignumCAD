/****************************************************************************
** Form implementation generated from reading ui file 'partinfodialog.ui'
**
** Created: Wed Dec 18 08:54:56 2002
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#include "partinfodialog.h"

#include <qvariant.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qmime.h>
#include <qdragobject.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qimage.h>
#include <qpixmap.h>

#include "partinfodialog.ui.h"
static QPixmap uic_load_pixmap_PartInfoDialog( const QString &name )
{
    const QMimeSource *m = QMimeSourceFactory::defaultFactory()->data( name );
    if ( !m )
	return QPixmap();
    QPixmap pix;
    QImageDrag::decode( m, pix );
    return pix;
}
/* 
 *  Constructs a PartInfoDialog which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
PartInfoDialog::PartInfoDialog( QWidget* parent,  const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
	setName( "PartInfoDialog" );
    resize( 308, 81 ); 
    setCaption( trUtf8( "Part Information" ) );
    setIcon( uic_load_pixmap_PartInfoDialog( "lignumCAD.png" ) );
    setSizeGripEnabled( TRUE );
    PartInfoDialogLayout = new QGridLayout( this, 1, 1, 11, 6, "PartInfoDialogLayout"); 

    Layout1 = new QHBoxLayout( 0, 0, 6, "Layout1"); 

    buttonHelp = new QPushButton( this, "buttonHelp" );
    buttonHelp->setText( trUtf8( "&Help" ) );
    buttonHelp->setAccel( 276824136 );
    buttonHelp->setAutoDefault( TRUE );
    Layout1->addWidget( buttonHelp );
    QSpacerItem* spacer = new QSpacerItem( 20, 0, QSizePolicy::Expanding, QSizePolicy::Minimum );
    Layout1->addItem( spacer );

    buttonOk = new QPushButton( this, "buttonOk" );
    buttonOk->setText( trUtf8( "&OK" ) );
    buttonOk->setAccel( 276824143 );
    buttonOk->setAutoDefault( TRUE );
    buttonOk->setDefault( TRUE );
    Layout1->addWidget( buttonOk );

    buttonCancel = new QPushButton( this, "buttonCancel" );
    buttonCancel->setText( trUtf8( "&Cancel" ) );
    buttonCancel->setAccel( 276824131 );
    buttonCancel->setAutoDefault( TRUE );
    Layout1->addWidget( buttonCancel );

    PartInfoDialogLayout->addMultiCellLayout( Layout1, 1, 1, 0, 1 );

    nameEdit = new QLineEdit( this, "nameEdit" );
    QToolTip::add( nameEdit, trUtf8( "Edit the part name." ) );
    QWhatsThis::add( nameEdit, trUtf8( "<p><b>Part Name</b></p>\n"
"<p>Enter a new name for the part.</p>" ) );

    PartInfoDialogLayout->addWidget( nameEdit, 0, 1 );

    nameLabel = new QLabel( this, "nameLabel" );
    nameLabel->setText( trUtf8( "&Name:" ) );

    PartInfoDialogLayout->addWidget( nameLabel, 0, 0 );

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
PartInfoDialog::~PartInfoDialog()
{
    // no need to delete child widgets, Qt does it all for us
}

