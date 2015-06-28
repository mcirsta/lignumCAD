/****************************************************************************
** Form implementation generated from reading ui file 'newpartdialog.ui'
**
** Created: Tue Oct 8 08:07:34 2002
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#include "newpartdialog.h"

#include <qvariant.h>
#include <lcconstraintchooser.h>
#include <lcdefaultlengthspinbox.h>
#include <qbuttongroup.h>
#include <qframe.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qmime.h>
#include <qdragobject.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qimage.h>
#include <qpixmap.h>

static QPixmap uic_load_pixmap_NewPartDialog( const QString &name )
{
    const QMimeSource *m = QMimeSourceFactory::defaultFactory()->data( name );
    if ( !m )
	return QPixmap();
    QPixmap pix;
    QImageDrag::decode( m, pix );
    return pix;
}
/* 
 *  Constructs a NewPartDialog which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
NewPartDialog::NewPartDialog( QWidget* parent,  const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
	setName( "NewPartDialog" );
    resize( 306, 389 ); 
    setCaption( trUtf8( "Create a Part" ) );
    setIcon( uic_load_pixmap_NewPartDialog( "lignumCAD.png" ) );
    setSizeGripEnabled( FALSE );
    NewPartDialogLayout = new QGridLayout( this, 1, 1, 11, 6, "NewPartDialogLayout"); 

    TextLabel1 = new QLabel( this, "TextLabel1" );
    TextLabel1->setFrameShape( QLabel::NoFrame );
    TextLabel1->setFrameShadow( QLabel::Plain );
    TextLabel1->setText( trUtf8( "Enter the initial size of the stock." ) );

    NewPartDialogLayout->addWidget( TextLabel1, 0, 0 );

    widthButtonGroup = new QButtonGroup( this, "widthButtonGroup" );
    widthButtonGroup->setTitle( trUtf8( "Width" ) );
    widthButtonGroup->setColumnLayout(0, Qt::Vertical );
    widthButtonGroup->layout()->setSpacing( 6 );
    widthButtonGroup->layout()->setMargin( 11 );
    widthButtonGroupLayout = new QGridLayout( widthButtonGroup->layout() );
    widthButtonGroupLayout->setAlignment( Qt::AlignTop );

    widthSpecifiedRadioButton = new QRadioButton( widthButtonGroup, "widthSpecifiedRadioButton" );
    widthSpecifiedRadioButton->setText( trUtf8( "Specified" ) );
    widthSpecifiedRadioButton->setChecked( TRUE );
    QToolTip::add( widthSpecifiedRadioButton, trUtf8( "Use a specified width." ) );
    QWhatsThis::add( widthSpecifiedRadioButton, trUtf8( "<p><b>Specified Width</b></p>\n"
"<p>Select this option if you want the rectangle to have an independent, specified width.</p>" ) );

    widthButtonGroupLayout->addWidget( widthSpecifiedRadioButton, 0, 0 );

    widthLengthSpinBox = new lCDefaultLengthSpinBox( widthButtonGroup, "widthLengthSpinBox" );
    QToolTip::add( widthLengthSpinBox, trUtf8( "Enter the width." ) );
    QWhatsThis::add( widthLengthSpinBox, trUtf8( "<p><b>Specified Width</b></p>\n"
"<p>Enter the width of the rectangle. The units of the value are given in the default units specified in the application Preferences (so, you do not have to enter the units abbreviation).</p>\n"
"<p>If the default units format is DECIMAL, then the entered value can have the usual floating point representation, e.g.:</p>\n"
"<p><code>1.234</code></p>\n"
"<p>If the default units format is FRACTIONAL, then, in addition to the decimal format, any of the following representations can be typed in:</p>\n"
"<p><code>1</code> (equals 1.0)</p>\n"
"<p><code>1/2</code> (equals 0.5)</p>\n"
"<p><code>1 1/2</code> (equals 1.5, note the blank separating the whole number and the fraction)</p>\n"
"<p>If you modify the value, you can always go back to the default value when the dialog was opened by clicking the <img src=\"default_active.png\"> button</p>" ) );

    widthButtonGroupLayout->addWidget( widthLengthSpinBox, 0, 1 );

    Line1 = new QFrame( widthButtonGroup, "Line1" );
    Line1->setFrameShape( QFrame::HLine );
    Line1->setFrameShadow( QFrame::Sunken );
    Line1->setFrameShape( QFrame::HLine );

    widthButtonGroupLayout->addMultiCellWidget( Line1, 1, 1, 0, 1 );

    widthImportedRadioButton = new QRadioButton( widthButtonGroup, "widthImportedRadioButton" );
    widthImportedRadioButton->setEnabled( FALSE );
    widthImportedRadioButton->setText( trUtf8( "Imported" ) );

    widthButtonGroupLayout->addWidget( widthImportedRadioButton, 2, 0 );

    widthConstraintChooser = new lCConstraintChooser( widthButtonGroup, "widthConstraintChooser" );
    widthConstraintChooser->setEnabled( FALSE );

    widthButtonGroupLayout->addWidget( widthConstraintChooser, 2, 1 );

    NewPartDialogLayout->addWidget( widthButtonGroup, 1, 0 );

    heightButtonGroup = new QButtonGroup( this, "heightButtonGroup" );
    heightButtonGroup->setTitle( trUtf8( "Height" ) );
    heightButtonGroup->setColumnLayout(0, Qt::Vertical );
    heightButtonGroup->layout()->setSpacing( 6 );
    heightButtonGroup->layout()->setMargin( 11 );
    heightButtonGroupLayout = new QGridLayout( heightButtonGroup->layout() );
    heightButtonGroupLayout->setAlignment( Qt::AlignTop );

    heightSpecifiedRadioButton = new QRadioButton( heightButtonGroup, "heightSpecifiedRadioButton" );
    heightSpecifiedRadioButton->setText( trUtf8( "Specified" ) );
    heightSpecifiedRadioButton->setChecked( TRUE );
    QToolTip::add( heightSpecifiedRadioButton, trUtf8( "Use a specified width." ) );
    QWhatsThis::add( heightSpecifiedRadioButton, trUtf8( "<p><b>Specified Width</b></p>\n"
"<p>Select this option if you want the rectangle to have an independent, specified width.</p>" ) );

    heightButtonGroupLayout->addWidget( heightSpecifiedRadioButton, 0, 0 );

    heightLengthSpinBox = new lCDefaultLengthSpinBox( heightButtonGroup, "heightLengthSpinBox" );
    QToolTip::add( heightLengthSpinBox, trUtf8( "Enter the width." ) );
    QWhatsThis::add( heightLengthSpinBox, trUtf8( "<p><b>Specified Width</b></p>\n"
"<p>Enter the width of the rectangle. The units of the value are given in the default units specified in the application Preferences (so, you do not have to enter the units abbreviation).</p>\n"
"<p>If the default units format is DECIMAL, then the entered value can have the usual floating point representation, e.g.:</p>\n"
"<p><code>1.234</code></p>\n"
"<p>If the default units format is FRACTIONAL, then, in addition to the decimal format, any of the following representations can be typed in:</p>\n"
"<p><code>1</code> (equals 1.0)</p>\n"
"<p><code>1/2</code> (equals 0.5)</p>\n"
"<p><code>1 1/2</code> (equals 1.5, note the blank separating the whole number and the fraction)</p>\n"
"<p>If you modify the value, you can always go back to the default value when the dialog was opened by clicking the <img src=\"default_active.png\"> button</p>" ) );

    heightButtonGroupLayout->addWidget( heightLengthSpinBox, 0, 1 );

    Line1_2 = new QFrame( heightButtonGroup, "Line1_2" );
    Line1_2->setFrameShape( QFrame::HLine );
    Line1_2->setFrameShadow( QFrame::Sunken );
    Line1_2->setFrameShape( QFrame::HLine );

    heightButtonGroupLayout->addMultiCellWidget( Line1_2, 1, 1, 0, 1 );

    heightImportedRadioButton = new QRadioButton( heightButtonGroup, "heightImportedRadioButton" );
    heightImportedRadioButton->setEnabled( FALSE );
    heightImportedRadioButton->setText( trUtf8( "Imported" ) );

    heightButtonGroupLayout->addWidget( heightImportedRadioButton, 2, 0 );

    heightConstraintChooser = new lCConstraintChooser( heightButtonGroup, "heightConstraintChooser" );
    heightConstraintChooser->setEnabled( FALSE );

    heightButtonGroupLayout->addWidget( heightConstraintChooser, 2, 1 );

    NewPartDialogLayout->addWidget( heightButtonGroup, 2, 0 );

    depthButtonGroup = new QButtonGroup( this, "depthButtonGroup" );
    depthButtonGroup->setTitle( trUtf8( "Depth" ) );
    depthButtonGroup->setColumnLayout(0, Qt::Vertical );
    depthButtonGroup->layout()->setSpacing( 6 );
    depthButtonGroup->layout()->setMargin( 11 );
    depthButtonGroupLayout = new QGridLayout( depthButtonGroup->layout() );
    depthButtonGroupLayout->setAlignment( Qt::AlignTop );

    depthSpecifiedRadioButton = new QRadioButton( depthButtonGroup, "depthSpecifiedRadioButton" );
    depthSpecifiedRadioButton->setText( trUtf8( "Specified" ) );
    depthSpecifiedRadioButton->setChecked( TRUE );
    QToolTip::add( depthSpecifiedRadioButton, trUtf8( "Use a specified width." ) );
    QWhatsThis::add( depthSpecifiedRadioButton, trUtf8( "<p><b>Specified Width</b></p>\n"
"<p>Select this option if you want the rectangle to have an independent, specified width.</p>" ) );

    depthButtonGroupLayout->addWidget( depthSpecifiedRadioButton, 0, 0 );

    depthLengthSpinBox = new lCDefaultLengthSpinBox( depthButtonGroup, "depthLengthSpinBox" );
    QToolTip::add( depthLengthSpinBox, trUtf8( "Enter the width." ) );
    QWhatsThis::add( depthLengthSpinBox, trUtf8( "<p><b>Specified Width</b></p>\n"
"<p>Enter the width of the rectangle. The units of the value are given in the default units specified in the application Preferences (so, you do not have to enter the units abbreviation).</p>\n"
"<p>If the default units format is DECIMAL, then the entered value can have the usual floating point representation, e.g.:</p>\n"
"<p><code>1.234</code></p>\n"
"<p>If the default units format is FRACTIONAL, then, in addition to the decimal format, any of the following representations can be typed in:</p>\n"
"<p><code>1</code> (equals 1.0)</p>\n"
"<p><code>1/2</code> (equals 0.5)</p>\n"
"<p><code>1 1/2</code> (equals 1.5, note the blank separating the whole number and the fraction)</p>\n"
"<p>If you modify the value, you can always go back to the default value when the dialog was opened by clicking the <img src=\"default_active.png\"> button</p>" ) );

    depthButtonGroupLayout->addWidget( depthLengthSpinBox, 0, 1 );

    Line1_3 = new QFrame( depthButtonGroup, "Line1_3" );
    Line1_3->setFrameShape( QFrame::HLine );
    Line1_3->setFrameShadow( QFrame::Sunken );
    Line1_3->setFrameShape( QFrame::HLine );

    depthButtonGroupLayout->addMultiCellWidget( Line1_3, 1, 1, 0, 1 );

    depthImportedRadioButton = new QRadioButton( depthButtonGroup, "depthImportedRadioButton" );
    depthImportedRadioButton->setEnabled( FALSE );
    depthImportedRadioButton->setText( trUtf8( "Imported" ) );

    depthButtonGroupLayout->addWidget( depthImportedRadioButton, 2, 0 );

    depthConstraintChooser = new lCConstraintChooser( depthButtonGroup, "depthConstraintChooser" );
    depthConstraintChooser->setEnabled( FALSE );

    depthButtonGroupLayout->addWidget( depthConstraintChooser, 2, 1 );

    NewPartDialogLayout->addWidget( depthButtonGroup, 3, 0 );

    Layout1 = new QHBoxLayout( 0, 0, 6, "Layout1"); 

    buttonHelp = new QPushButton( this, "buttonHelp" );
    buttonHelp->setText( trUtf8( "Help" ) );
    buttonHelp->setAccel( 4144 );
    buttonHelp->setAutoDefault( TRUE );
    Layout1->addWidget( buttonHelp );
    QSpacerItem* spacer = new QSpacerItem( 20, 0, QSizePolicy::Expanding, QSizePolicy::Minimum );
    Layout1->addItem( spacer );

    buttonOk = new QPushButton( this, "buttonOk" );
    buttonOk->setText( trUtf8( "OK" ) );
    buttonOk->setAccel( 0 );
    buttonOk->setAutoDefault( TRUE );
    buttonOk->setDefault( TRUE );
    Layout1->addWidget( buttonOk );

    buttonCancel = new QPushButton( this, "buttonCancel" );
    buttonCancel->setText( trUtf8( "Cancel" ) );
    buttonCancel->setAccel( 0 );
    buttonCancel->setAutoDefault( TRUE );
    Layout1->addWidget( buttonCancel );

    NewPartDialogLayout->addLayout( Layout1, 4, 0 );

    // signals and slots connections
    connect( buttonOk, SIGNAL( clicked() ), this, SLOT( accept() ) );
    connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
}

/*  
 *  Destroys the object and frees any allocated resources
 */
NewPartDialog::~NewPartDialog()
{
    // no need to delete child widgets, Qt does it all for us
}

