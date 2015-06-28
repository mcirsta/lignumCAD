/****************************************************************************
** Form implementation generated from reading ui file 'aboutdialog.ui'
**
** Created: Wed Dec 18 08:54:53 2002
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#include "aboutdialog.h"

#include <qvariant.h>
#include <Standard_Version.hxx>
#include <qframe.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qmime.h>
#include <qdragobject.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qimage.h>
#include <qpixmap.h>

#include "OGLFT.h"
#include "configuration.h"
#include "aboutdialog.ui.h"
static QPixmap uic_load_pixmap_aboutDialog( const QString &name )
{
    const QMimeSource *m = QMimeSourceFactory::defaultFactory()->data( name );
    if ( !m )
	return QPixmap();
    QPixmap pix;
    QImageDrag::decode( m, pix );
    return pix;
}
/* 
 *  Constructs a aboutDialog which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
aboutDialog::aboutDialog( QWidget* parent,  const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
	setName( "aboutDialog" );
    resize( 500, 760 ); 
    setPaletteBackgroundColor( QColor( 0, 63, 64 ) );
    setCaption( trUtf8( "About lignumCAD" ) );
    setIcon( uic_load_pixmap_aboutDialog( "lignumCAD.png" ) );
    setSizeGripEnabled( FALSE );
    aboutDialogLayout = new QVBoxLayout( this, 11, 6, "aboutDialogLayout"); 

    Layout3 = new QHBoxLayout( 0, 0, 6, "Layout3"); 
    QSpacerItem* spacer = new QSpacerItem( 20, 0, QSizePolicy::Expanding, QSizePolicy::Minimum );
    Layout3->addItem( spacer );

    PixmapLabel1 = new QLabel( this, "PixmapLabel1" );
    PixmapLabel1->setPixmap( uic_load_pixmap_aboutDialog( "splash.png" ) );
    PixmapLabel1->setScaledContents( TRUE );
    Layout3->addWidget( PixmapLabel1 );
    QSpacerItem* spacer_2 = new QSpacerItem( 20, 0, QSizePolicy::Expanding, QSizePolicy::Minimum );
    Layout3->addItem( spacer_2 );
    aboutDialogLayout->addLayout( Layout3 );

    programVersionLabel = new QLabel( this, "programVersionLabel" );
    programVersionLabel->setPaletteForegroundColor( QColor( 255, 255, 0 ) );
    QFont programVersionLabel_font(  programVersionLabel->font() );
    programVersionLabel_font.setFamily( "Arial [Xft]" );
    programVersionLabel_font.setPointSize( 12 );
    programVersionLabel_font.setBold( TRUE );
    programVersionLabel->setFont( programVersionLabel_font ); 
    programVersionLabel->setText( trUtf8( "lignumCAD Version x.y" ) );
    programVersionLabel->setAlignment( int( QLabel::AlignCenter ) );
    aboutDialogLayout->addWidget( programVersionLabel );

    copyrightLabel = new QLabel( this, "copyrightLabel" );
    copyrightLabel->setPaletteForegroundColor( QColor( 255, 255, 0 ) );
    QFont copyrightLabel_font(  copyrightLabel->font() );
    copyrightLabel_font.setFamily( "Arial [Xft]" );
    copyrightLabel_font.setPointSize( 12 );
    copyrightLabel_font.setBold( TRUE );
    copyrightLabel->setFont( copyrightLabel_font ); 
    copyrightLabel->setText( trUtf8( "Copyright © 2002 lignum Computing, Inc." ) );
    copyrightLabel->setAlignment( int( QLabel::AlignCenter ) );
    aboutDialogLayout->addWidget( copyrightLabel );

    contactLabel = new QLabel( this, "contactLabel" );
    contactLabel->setPaletteForegroundColor( QColor( 255, 255, 0 ) );
    QFont contactLabel_font(  contactLabel->font() );
    contactLabel_font.setPointSize( 12 );
    contactLabel_font.setBold( TRUE );
    contactLabel->setFont( contactLabel_font ); 
    contactLabel->setText( trUtf8( "www.lignumcomputing.com\n"
"e-mail: lignumcad@lignumcomputing.com" ) );
    contactLabel->setAlignment( int( QLabel::AlignCenter ) );
    aboutDialogLayout->addWidget( contactLabel );

    GPLLabel = new QLabel( this, "GPLLabel" );
    GPLLabel->setPaletteForegroundColor( QColor( 255, 255, 0 ) );
    GPLLabel->setText( trUtf8( "This program is licensed to you under the terms of the GNU General Public License Version 2 as published by the Free Software Foundation. This gives you legal permission to copy, distribute and/or modify this software under certain conditions. For details, see the file 'COPYING' that came with this software distribution. If you did not get the file, send email to lignumcad@lignumcomputing.com.\n"
"\n"
"The program is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE." ) );
    GPLLabel->setAlignment( int( QLabel::WordBreak | QLabel::AlignVCenter ) );
    aboutDialogLayout->addWidget( GPLLabel );

    Line1 = new QFrame( this, "Line1" );
    Line1->setPaletteForegroundColor( QColor( 255, 255, 0 ) );
    Line1->setPaletteBackgroundColor( QColor( 255, 255, 0 ) );
    Line1->setFrameShape( QFrame::HLine );
    Line1->setFrameShadow( QFrame::Sunken );
    Line1->setFrameShape( QFrame::HLine );
    aboutDialogLayout->addWidget( Line1 );

    Layout5 = new QGridLayout( 0, 1, 1, 0, 6, "Layout5"); 

    freeTypeVersionLabel = new QLabel( this, "freeTypeVersionLabel" );
    freeTypeVersionLabel->setPaletteForegroundColor( QColor( 255, 255, 0 ) );
    freeTypeVersionLabel->setText( trUtf8( "Version x.y.z" ) );

    Layout5->addWidget( freeTypeVersionLabel, 2, 1 );

    openCASCADELabel = new QLabel( this, "openCASCADELabel" );
    openCASCADELabel->setPaletteForegroundColor( QColor( 255, 255, 0 ) );
    openCASCADELabel->setText( trUtf8( "OpenCASCADE <www.opencascde.org>:" ) );

    Layout5->addWidget( openCASCADELabel, 1, 0 );

    acknowledgementsLabel = new QLabel( this, "acknowledgementsLabel" );
    acknowledgementsLabel->setPaletteForegroundColor( QColor( 255, 255, 0 ) );
    acknowledgementsLabel->setText( trUtf8( "lignum Computing, Inc. is pleased to acknowledge the following Open Source components of lignumCAD:" ) );
    acknowledgementsLabel->setAlignment( int( QLabel::WordBreak | QLabel::AlignVCenter ) );

    Layout5->addMultiCellWidget( acknowledgementsLabel, 0, 0, 0, 1 );

    openGLGLUVersionLabel = new QLabel( this, "openGLGLUVersionLabel" );
    openGLGLUVersionLabel->setPaletteForegroundColor( QColor( 255, 255, 0 ) );
    openGLGLUVersionLabel->setText( trUtf8( "Version x.y.z" ) );

    Layout5->addWidget( openGLGLUVersionLabel, 4, 1 );

    qtLabel = new QLabel( this, "qtLabel" );
    qtLabel->setPaletteForegroundColor( QColor( 255, 255, 0 ) );
    qtLabel->setText( trUtf8( "Qt <www.trolltech.com>:" ) );

    Layout5->addWidget( qtLabel, 3, 0 );

    openCASCADEVersionLabel = new QLabel( this, "openCASCADEVersionLabel" );
    openCASCADEVersionLabel->setPaletteForegroundColor( QColor( 255, 255, 0 ) );
    openCASCADEVersionLabel->setText( trUtf8( "Version x.y.z" ) );

    Layout5->addWidget( openCASCADEVersionLabel, 1, 1 );

    freeTypeLabel = new QLabel( this, "freeTypeLabel" );
    freeTypeLabel->setPaletteForegroundColor( QColor( 255, 255, 0 ) );
    freeTypeLabel->setText( trUtf8( "FreeType <www.freetype.org>:" ) );

    Layout5->addWidget( freeTypeLabel, 2, 0 );

    openGLGLULabel = new QLabel( this, "openGLGLULabel" );
    openGLGLULabel->setPaletteForegroundColor( QColor( 255, 255, 0 ) );
    openGLGLULabel->setText( trUtf8( "OpenGL GLU <www.sgi.com>:" ) );

    Layout5->addWidget( openGLGLULabel, 4, 0 );

    qtVersionLabel = new QLabel( this, "qtVersionLabel" );
    qtVersionLabel->setPaletteForegroundColor( QColor( 255, 255, 0 ) );
    qtVersionLabel->setText( trUtf8( "Version x.y.z" ) );

    Layout5->addWidget( qtVersionLabel, 3, 1 );
    aboutDialogLayout->addLayout( Layout5 );

    Line1_2 = new QFrame( this, "Line1_2" );
    Line1_2->setPaletteForegroundColor( QColor( 255, 255, 0 ) );
    Line1_2->setPaletteBackgroundColor( QColor( 255, 255, 0 ) );
    Line1_2->setFrameShape( QFrame::HLine );
    Line1_2->setFrameShadow( QFrame::Sunken );
    Line1_2->setFrameShape( QFrame::HLine );
    aboutDialogLayout->addWidget( Line1_2 );

    Layout6 = new QHBoxLayout( 0, 0, 6, "Layout6"); 

    openGLLabel = new QLabel( this, "openGLLabel" );
    openGLLabel->setPaletteForegroundColor( QColor( 255, 255, 0 ) );
    openGLLabel->setText( trUtf8( "OpenGL Driver" ) );
    Layout6->addWidget( openGLLabel );

    openGLVersionLabel = new QLabel( this, "openGLVersionLabel" );
    openGLVersionLabel->setPaletteForegroundColor( QColor( 255, 255, 0 ) );
    openGLVersionLabel->setText( trUtf8( "Version x.y.z" ) );
    Layout6->addWidget( openGLVersionLabel );
    aboutDialogLayout->addLayout( Layout6 );

    Layout2 = new QHBoxLayout( 0, 0, 6, "Layout2"); 
    QSpacerItem* spacer_3 = new QSpacerItem( 20, 0, QSizePolicy::Expanding, QSizePolicy::Minimum );
    Layout2->addItem( spacer_3 );

    buttonOk = new QPushButton( this, "buttonOk" );
    buttonOk->setText( trUtf8( "OK" ) );
    buttonOk->setAccel( 0 );
    buttonOk->setAutoDefault( TRUE );
    buttonOk->setDefault( TRUE );
    Layout2->addWidget( buttonOk );
    QSpacerItem* spacer_4 = new QSpacerItem( 20, 0, QSizePolicy::Expanding, QSizePolicy::Minimum );
    Layout2->addItem( spacer_4 );
    aboutDialogLayout->addLayout( Layout2 );

    // signals and slots connections
    connect( buttonOk, SIGNAL( clicked() ), this, SLOT( accept() ) );
    init();
}

/*  
 *  Destroys the object and frees any allocated resources
 */
aboutDialog::~aboutDialog()
{
    // no need to delete child widgets, Qt does it all for us
}

