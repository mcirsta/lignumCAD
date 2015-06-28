/****************************************************************************
** Form implementation generated from reading ui file 'offsetinfodialog.ui'
**
** Created: Wed Dec 18 08:54:56 2002
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#include "offsetinfodialog.h"

#include <qvariant.h>
#include <lcdefaultlengthconstraint.h>
#include <qpushbutton.h>
#include <qmime.h>
#include <qdragobject.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qimage.h>
#include <qpixmap.h>

#include "offsetinfodialog.ui.h"
static QPixmap uic_load_pixmap_OffsetInfoDialog( const QString &name )
{
    const QMimeSource *m = QMimeSourceFactory::defaultFactory()->data( name );
    if ( !m )
	return QPixmap();
    QPixmap pix;
    QImageDrag::decode( m, pix );
    return pix;
}
/* 
 *  Constructs a OffsetInfoDialog which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
OffsetInfoDialog::OffsetInfoDialog( QWidget* parent,  const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
	setName( "OffsetInfoDialog" );
    resize( 308, 157 ); 
    setCaption( trUtf8( "Offset Information" ) );
    setSizeGripEnabled( TRUE );
    OffsetInfoDialogLayout = new QGridLayout( this, 1, 1, 11, 6, "OffsetInfoDialogLayout"); 

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

    OffsetInfoDialogLayout->addLayout( Layout1, 1, 0 );

    offsetLengthConstraint = new lCDefaultLengthConstraint( this, "offsetLengthConstraint" );
    offsetLengthConstraint->setTitle( trUtf8( "Temporary label" ) );
    offsetLengthConstraint->setSpecifiedButtonToolTip( trUtf8( "Use a specified size." ) );
    offsetLengthConstraint->setSpecifiedButtonWhatsThis( trUtf8( "<p><b>Specified Size</b></p> <p>Select this option if you want the parameter to have an independent, specified size.</p>" ) );
    offsetLengthConstraint->setSpecifiedSpinBoxToolTip( trUtf8( "Enter the size." ) );
    offsetLengthConstraint->setSpecifiedSpinBoxWhatsThis( trUtf8( "<p><b>Specified Size</b></p> <p>Enter the size of the parameter. The units of the value are given in the default units specified in the application Preferences (so, you do not have to enter the units abbreviation).</p> <p>If the default units format is DECIMAL, then the entered value can have the usual floating point representation, e.g.:</p> <p><code>1.234</code></p> <p>If the default units format is FRACTIONAL, then, in addition to the decimal format, any of the following representations can be typed in:</p> <p><code>1</code> (equals 1.0)</p> <p><code>1/2</code> (equals 0.5)</p> <p><code>1 1/2</code> (equals 1.5, note the blank separating the whole number and the fraction)</p> <p>If you modify the value, you can always go back to the default value when the dialog was opened by clicking the <img src=\"default_active.png\"> button</p>" ) );

    OffsetInfoDialogLayout->addWidget( offsetLengthConstraint, 0, 0 );

    // signals and slots connections
    connect( buttonOk, SIGNAL( clicked() ), this, SLOT( accept() ) );
    connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
    connect( buttonHelp, SIGNAL( clicked() ), this, SLOT( buttonHelp_clicked() ) );

    // tab order
    setTabOrder( buttonOk, buttonCancel );
    setTabOrder( buttonCancel, buttonHelp );
}

/*  
 *  Destroys the object and frees any allocated resources
 */
OffsetInfoDialog::~OffsetInfoDialog()
{
    // no need to delete child widgets, Qt does it all for us
}

