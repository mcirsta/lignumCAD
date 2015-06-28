/****************************************************************************
** Form implementation generated from reading ui file 'assemblyadddialog.ui'
**
** Created: Wed Dec 18 08:54:56 2002
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#include "assemblyadddialog.h"

#include <qvariant.h>
#include <qheader.h>
#include <qlistview.h>
#include <qpushbutton.h>
#include <qmime.h>
#include <qdragobject.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qimage.h>
#include <qpixmap.h>

#include "assemblyadddialog.ui.h"
static QPixmap uic_load_pixmap_AssemblyAddDialog( const QString &name )
{
    const QMimeSource *m = QMimeSourceFactory::defaultFactory()->data( name );
    if ( !m )
	return QPixmap();
    QPixmap pix;
    QImageDrag::decode( m, pix );
    return pix;
}
/* 
 *  Constructs a AssemblyAddDialog which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
AssemblyAddDialog::AssemblyAddDialog( QWidget* parent,  const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
	setName( "AssemblyAddDialog" );
    resize( 308, 225 ); 
    setCaption( trUtf8( "Add a Model to the Assembly" ) );
    setIcon( uic_load_pixmap_AssemblyAddDialog( "lignumCAD.png" ) );
    setSizeGripEnabled( TRUE );
    AssemblyAddDialogLayout = new QGridLayout( this, 1, 1, 11, 6, "AssemblyAddDialogLayout"); 

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

    AssemblyAddDialogLayout->addLayout( Layout1, 1, 0 );

    modelListView = new QListView( this, "modelListView" );
    modelListView->addColumn( trUtf8( "Name" ) );
    modelListView->addColumn( trUtf8( "Type" ) );

    AssemblyAddDialogLayout->addWidget( modelListView, 0, 0 );

    // signals and slots connections
    connect( buttonOk, SIGNAL( clicked() ), this, SLOT( accept() ) );
    connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
    connect( modelListView, SIGNAL( selectionChanged(QListViewItem*) ), this, SLOT( modelListView_selectionChanged(QListViewItem*) ) );
    connect( buttonHelp, SIGNAL( clicked() ), this, SLOT( buttonHelp_clicked() ) );
}

/*  
 *  Destroys the object and frees any allocated resources
 */
AssemblyAddDialog::~AssemblyAddDialog()
{
    // no need to delete child widgets, Qt does it all for us
}

