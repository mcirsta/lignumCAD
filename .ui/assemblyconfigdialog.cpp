/****************************************************************************
** Form implementation generated from reading ui file 'assemblyconfigdialog.ui'
**
** Created: Wed Dec 18 08:54:56 2002
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#include "assemblyconfigdialog.h"

#include <qvariant.h>
#include <qheader.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qlistview.h>
#include <qpushbutton.h>
#include <qmime.h>
#include <qdragobject.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qimage.h>
#include <qpixmap.h>

#include "assemblyconfigdialog.ui.h"
static QPixmap uic_load_pixmap_AssemblyConfigDialog( const QString &name )
{
    const QMimeSource *m = QMimeSourceFactory::defaultFactory()->data( name );
    if ( !m )
	return QPixmap();
    QPixmap pix;
    QImageDrag::decode( m, pix );
    return pix;
}
/* 
 *  Constructs a AssemblyConfigDialog which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
AssemblyConfigDialog::AssemblyConfigDialog( QWidget* parent,  const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
	setName( "AssemblyConfigDialog" );
    resize( 308, 316 ); 
    setCaption( trUtf8( "Create Assembly" ) );
    setIcon( uic_load_pixmap_AssemblyConfigDialog( "lignumCAD.png" ) );
    setSizeGripEnabled( TRUE );
    AssemblyConfigDialogLayout = new QGridLayout( this, 1, 1, 11, 6, "AssemblyConfigDialogLayout"); 

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

    AssemblyConfigDialogLayout->addMultiCellLayout( Layout1, 3, 3, 0, 1 );

    nameLabel = new QLabel( this, "nameLabel" );
    nameLabel->setText( trUtf8( "&Name:" ) );

    AssemblyConfigDialogLayout->addWidget( nameLabel, 0, 0 );

    nameEdit = new QLineEdit( this, "nameEdit" );

    AssemblyConfigDialogLayout->addWidget( nameEdit, 0, 1 );

    modelListView = new QListView( this, "modelListView" );
    modelListView->addColumn( trUtf8( "Name" ) );
    modelListView->addColumn( trUtf8( "Type" ) );

    AssemblyConfigDialogLayout->addMultiCellWidget( modelListView, 2, 2, 0, 1 );

    TextLabel2 = new QLabel( this, "TextLabel2" );
    TextLabel2->setText( trUtf8( "&Select initial assembly model" ) );

    AssemblyConfigDialogLayout->addMultiCellWidget( TextLabel2, 1, 1, 0, 1 );

    // signals and slots connections
    connect( buttonOk, SIGNAL( clicked() ), this, SLOT( accept() ) );
    connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
    connect( modelListView, SIGNAL( selectionChanged(QListViewItem*) ), this, SLOT( modelListView_selectionChanged(QListViewItem*) ) );
    connect( buttonHelp, SIGNAL( clicked() ), this, SLOT( buttonHelp_clicked() ) );

    // buddies
    nameLabel->setBuddy( nameEdit );
    TextLabel2->setBuddy( modelListView );
    init();
}

/*  
 *  Destroys the object and frees any allocated resources
 */
AssemblyConfigDialog::~AssemblyConfigDialog()
{
    // no need to delete child widgets, Qt does it all for us
}

