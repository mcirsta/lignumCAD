/****************************************************************************
** Form implementation generated from reading ui file 'referencelineinfodialog.ui'
**
** Created: Wed Dec 18 08:54:54 2002
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#include "referencelineinfodialog.h"

#include <qvariant.h>
#include <lcconstraintchooser.h>
#include <lcdefaultlengthspinbox.h>
#include <qbuttongroup.h>
#include <qframe.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qmime.h>
#include <qdragobject.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qimage.h>
#include <qpixmap.h>

#include "referencelineinfodialog.ui.h"
static QPixmap uic_load_pixmap_ReferenceLineInfoDialog( const QString &name )
{
    const QMimeSource *m = QMimeSourceFactory::defaultFactory()->data( name );
    if ( !m )
	return QPixmap();
    QPixmap pix;
    QImageDrag::decode( m, pix );
    return pix;
}
/* 
 *  Constructs a ReferenceLineInfoDialog which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
ReferenceLineInfoDialog::ReferenceLineInfoDialog( QWidget* parent,  const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
	setName( "ReferenceLineInfoDialog" );
    resize( 400, 248 ); 
    setCaption( trUtf8( "Reference Line Information" ) );
    setIcon( uic_load_pixmap_ReferenceLineInfoDialog( "lignumCAD.png" ) );
    ReferenceLineInfoDialogLayout = new QGridLayout( this, 1, 1, 11, 6, "ReferenceLineInfoDialogLayout"); 

    Layout15 = new QHBoxLayout( 0, 0, 6, "Layout15"); 

    referenceLineNameLabel = new QLabel( this, "referenceLineNameLabel" );
    referenceLineNameLabel->setText( trUtf8( "&Name:" ) );
    Layout15->addWidget( referenceLineNameLabel );

    nameEdit = new QLineEdit( this, "nameEdit" );
    QToolTip::add( nameEdit, trUtf8( "Edit the reference line's name." ) );
    QWhatsThis::add( nameEdit, trUtf8( "<p><b>Name</b></p>\n"
"<p>Edit the name of the reference line. The reference line's name will be updated when the dialog is accepted. If there is already a reference line on this page which has the same name, the name change will be rejected.</p>" ) );
    Layout15->addWidget( nameEdit );

    ReferenceLineInfoDialogLayout->addLayout( Layout15, 0, 0 );

    definitionGroupBox = new QGroupBox( this, "definitionGroupBox" );
    definitionGroupBox->setTitle( trUtf8( "Definition" ) );
    definitionGroupBox->setColumnLayout(0, Qt::Vertical );
    definitionGroupBox->layout()->setSpacing( 6 );
    definitionGroupBox->layout()->setMargin( 11 );
    definitionGroupBoxLayout = new QGridLayout( definitionGroupBox->layout() );
    definitionGroupBoxLayout->setAlignment( Qt::AlignTop );

    referenceLabel = new QLabel( definitionGroupBox, "referenceLabel" );
    referenceLabel->setText( trUtf8( "Reference:" ) );

    definitionGroupBoxLayout->addWidget( referenceLabel, 0, 0 );

    referenceLineEdit = new QLineEdit( definitionGroupBox, "referenceLineEdit" );
    referenceLineEdit->setReadOnly( TRUE );
    QToolTip::add( referenceLineEdit, trUtf8( "Offset reference." ) );
    QWhatsThis::add( referenceLineEdit, trUtf8( "<p><b>Offset Reference</b></p>\n"
"<p>This widget shows the geometry item which the reference line is offset from. This value is not editable in this dialog; you must use the regular interface to adjust constraint references.</p>" ) );

    definitionGroupBoxLayout->addWidget( referenceLineEdit, 0, 1 );

    offsetButtonGroup = new QButtonGroup( definitionGroupBox, "offsetButtonGroup" );
    offsetButtonGroup->setTitle( trUtf8( "Offset" ) );
    offsetButtonGroup->setColumnLayout(0, Qt::Vertical );
    offsetButtonGroup->layout()->setSpacing( 6 );
    offsetButtonGroup->layout()->setMargin( 11 );
    offsetButtonGroupLayout = new QGridLayout( offsetButtonGroup->layout() );
    offsetButtonGroupLayout->setAlignment( Qt::AlignTop );

    specifiedRadioButton = new QRadioButton( offsetButtonGroup, "specifiedRadioButton" );
    specifiedRadioButton->setText( trUtf8( "&Specified" ) );
    specifiedRadioButton->setChecked( TRUE );
    QToolTip::add( specifiedRadioButton, trUtf8( "Use a specified offset." ) );
    QWhatsThis::add( specifiedRadioButton, trUtf8( "<p><b>Specified Offset</b></p>\n"
"<p>Select this option if you want the reference line to have an independent, specified offset.</p>" ) );

    offsetButtonGroupLayout->addWidget( specifiedRadioButton, 0, 0 );

    offsetLengthSpinBox = new lCDefaultLengthSpinBox( offsetButtonGroup, "offsetLengthSpinBox" );
    QToolTip::add( offsetLengthSpinBox, trUtf8( "Enter the offset." ) );
    QWhatsThis::add( offsetLengthSpinBox, trUtf8( "<p><b>Specified Offset</b></p>\n"
"<p>Enter the offset of the reference line (from the reference). The units of the value are given in the default units specified in the application Preferences (so, you do not have to enter the units abbreviation).</p>\n"
"<p>If the default units format is DECIMAL, then the entered value can have the usual floating point representation, e.g.:</p>\n"
"<p><code>1.234</code></p>\n"
"<p>If the default units format is FRACTIONAL, then, in addition to the decimal format, any of the following representations can be typed in:</p>\n"
"<p><code>1</code> (equals 1.0)</p>\n"
"<p><code>1/2</code> (equals 0.5)</p>\n"
"<p><code>1 1/2</code> (equals 1.5, note the blank separating the whole number and the fraction)</p>\n"
"<p>If you modify the value, you can always go back to the default value when the dialog was opened by clicking the <img src=\"default_active.png\"> button</p>" ) );

    offsetButtonGroupLayout->addWidget( offsetLengthSpinBox, 0, 1 );

    Line1 = new QFrame( offsetButtonGroup, "Line1" );
    Line1->setFrameShape( QFrame::HLine );
    Line1->setFrameShadow( QFrame::Sunken );
    Line1->setFrameShape( QFrame::HLine );

    offsetButtonGroupLayout->addMultiCellWidget( Line1, 1, 1, 0, 1 );

    importedRadioButton = new QRadioButton( offsetButtonGroup, "importedRadioButton" );
    importedRadioButton->setEnabled( FALSE );
    importedRadioButton->setText( trUtf8( "Imported" ) );

    offsetButtonGroupLayout->addWidget( importedRadioButton, 2, 0 );

    constraintChooser = new lCConstraintChooser( offsetButtonGroup, "constraintChooser" );
    constraintChooser->setEnabled( FALSE );

    offsetButtonGroupLayout->addWidget( constraintChooser, 2, 1 );

    definitionGroupBoxLayout->addMultiCellWidget( offsetButtonGroup, 1, 1, 0, 1 );

    ReferenceLineInfoDialogLayout->addWidget( definitionGroupBox, 1, 0 );

    orientationButtonGroup = new QButtonGroup( this, "orientationButtonGroup" );
    orientationButtonGroup->setTitle( trUtf8( "Orientation" ) );
    QToolTip::add( orientationButtonGroup, trUtf8( "Orientation of the reference line." ) );
    orientationButtonGroup->setColumnLayout(0, Qt::Vertical );
    orientationButtonGroup->layout()->setSpacing( 6 );
    orientationButtonGroup->layout()->setMargin( 11 );
    orientationButtonGroupLayout = new QGridLayout( orientationButtonGroup->layout() );
    orientationButtonGroupLayout->setAlignment( Qt::AlignTop );

    horizontalButton = new QRadioButton( orientationButtonGroup, "horizontalButton" );
    horizontalButton->setEnabled( FALSE );
    horizontalButton->setText( trUtf8( "Horizontal" ) );

    orientationButtonGroupLayout->addWidget( horizontalButton, 0, 0 );

    verticalButton = new QRadioButton( orientationButtonGroup, "verticalButton" );
    verticalButton->setEnabled( FALSE );
    verticalButton->setText( trUtf8( "Vertical" ) );

    orientationButtonGroupLayout->addWidget( verticalButton, 1, 0 );

    ReferenceLineInfoDialogLayout->addWidget( orientationButtonGroup, 1, 1 );

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

    ReferenceLineInfoDialogLayout->addMultiCellLayout( Layout1, 2, 2, 0, 1 );

    // signals and slots connections
    connect( buttonOk, SIGNAL( clicked() ), this, SLOT( accept() ) );
    connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
    connect( buttonHelp, SIGNAL( clicked() ), this, SLOT( buttonHelp_clicked() ) );

    // tab order
    setTabOrder( nameEdit, referenceLineEdit );
    setTabOrder( referenceLineEdit, specifiedRadioButton );
    setTabOrder( specifiedRadioButton, offsetLengthSpinBox );
    setTabOrder( offsetLengthSpinBox, constraintChooser );
    setTabOrder( constraintChooser, horizontalButton );
    setTabOrder( horizontalButton, verticalButton );
    setTabOrder( verticalButton, buttonHelp );
    setTabOrder( buttonHelp, buttonOk );
    setTabOrder( buttonOk, buttonCancel );

    // buddies
    referenceLineNameLabel->setBuddy( nameEdit );
}

/*  
 *  Destroys the object and frees any allocated resources
 */
ReferenceLineInfoDialog::~ReferenceLineInfoDialog()
{
    // no need to delete child widgets, Qt does it all for us
}

