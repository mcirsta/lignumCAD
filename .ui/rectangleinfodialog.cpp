/****************************************************************************
** Form implementation generated from reading ui file 'rectangleinfodialog.ui'
**
** Created: Wed Dec 18 08:54:56 2002
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#include "rectangleinfodialog.h"

#include <qvariant.h>
#include <lccolorchooser.h>
#include <lcconstraintchooser.h>
#include <lcdefaultcombobox.h>
#include <lcdefaultfilechooser.h>
#include <lcdefaultlengthspinbox.h>
#include <lcdefaultspinbox.h>
#include <qbuttongroup.h>
#include <qframe.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qslider.h>
#include <qmime.h>
#include <qdragobject.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qimage.h>
#include <qpixmap.h>

#include "constants.h"
#include "rectangleinfodialog.ui.h"
static QPixmap uic_load_pixmap_RectangleInfoDialog( const QString &name )
{
    const QMimeSource *m = QMimeSourceFactory::defaultFactory()->data( name );
    if ( !m )
	return QPixmap();
    QPixmap pix;
    QImageDrag::decode( m, pix );
    return pix;
}
/* 
 *  Constructs a RectangleInfoDialog which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
RectangleInfoDialog::RectangleInfoDialog( QWidget* parent,  const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
	setName( "RectangleInfoDialog" );
    resize( 567, 568 ); 
    setCaption( trUtf8( "Rectangle Information" ) );
    setIcon( uic_load_pixmap_RectangleInfoDialog( "lignumCAD.png" ) );
    RectangleInfoDialogLayout = new QGridLayout( this, 1, 1, 11, 6, "RectangleInfoDialogLayout"); 

    nameLabel = new QLabel( this, "nameLabel" );
    nameLabel->setFrameShape( QLabel::MShape );
    nameLabel->setFrameShadow( QLabel::MShadow );
    nameLabel->setText( trUtf8( "&Name:" ) );

    RectangleInfoDialogLayout->addWidget( nameLabel, 0, 0 );

    xOriginGroupBox = new QGroupBox( this, "xOriginGroupBox" );
    xOriginGroupBox->setTitle( trUtf8( "X Origin" ) );
    xOriginGroupBox->setColumnLayout(0, Qt::Vertical );
    xOriginGroupBox->layout()->setSpacing( 6 );
    xOriginGroupBox->layout()->setMargin( 11 );
    xOriginGroupBoxLayout = new QGridLayout( xOriginGroupBox->layout() );
    xOriginGroupBoxLayout->setAlignment( Qt::AlignTop );

    xOriginReferenceLabel = new QLabel( xOriginGroupBox, "xOriginReferenceLabel" );
    xOriginReferenceLabel->setText( trUtf8( "Reference:" ) );

    xOriginGroupBoxLayout->addWidget( xOriginReferenceLabel, 0, 0 );

    xOriginReferenceLineEdit = new QLineEdit( xOriginGroupBox, "xOriginReferenceLineEdit" );
    xOriginReferenceLineEdit->setReadOnly( TRUE );
    QToolTip::add( xOriginReferenceLineEdit, trUtf8( "X origin offset reference." ) );
    QWhatsThis::add( xOriginReferenceLineEdit, trUtf8( "<p><b>X Origin Offset Reference</b></p>\n"
"<p>This widget shows the geometry item which the X0 edge of the rectangle is offset from. This value is not editable in this dialog; you must use the regular interface to adjust constraint references.</p>" ) );

    xOriginGroupBoxLayout->addWidget( xOriginReferenceLineEdit, 0, 1 );

    xOriginButtonGroup = new QButtonGroup( xOriginGroupBox, "xOriginButtonGroup" );
    xOriginButtonGroup->setTitle( trUtf8( "Offset" ) );
    xOriginButtonGroup->setColumnLayout(0, Qt::Vertical );
    xOriginButtonGroup->layout()->setSpacing( 6 );
    xOriginButtonGroup->layout()->setMargin( 11 );
    xOriginButtonGroupLayout = new QGridLayout( xOriginButtonGroup->layout() );
    xOriginButtonGroupLayout->setAlignment( Qt::AlignTop );

    xOriginSpecifiedRadioButton = new QRadioButton( xOriginButtonGroup, "xOriginSpecifiedRadioButton" );
    xOriginSpecifiedRadioButton->setText( trUtf8( "Specified" ) );
    QToolTip::add( xOriginSpecifiedRadioButton, trUtf8( "Use a specified x origin offset." ) );
    QWhatsThis::add( xOriginSpecifiedRadioButton, trUtf8( "<p><b>Specified X Origin Offset</b></p>\n"
"<p>Select this option if you want the rectangle to have an independent, specified X origin offset.</p>" ) );

    xOriginButtonGroupLayout->addWidget( xOriginSpecifiedRadioButton, 0, 0 );

    xOriginOffsetLengthSpinBox = new lCDefaultLengthSpinBox( xOriginButtonGroup, "xOriginOffsetLengthSpinBox" );
    QToolTip::add( xOriginOffsetLengthSpinBox, trUtf8( "Enter the x origin offset." ) );
    QWhatsThis::add( xOriginOffsetLengthSpinBox, trUtf8( "<p><b>Specified X Origin Offset</b></p>\n"
"<p>Enter the offset of the X origin of the rectangle (from the X origin reference). The units of the value are given in the default units specified in the application Preferences (so, you do not have to enter the units abbreviation).</p>\n"
"<p>If the default units format is DECIMAL, then the entered value can have the usual floating point representation, e.g.:</p>\n"
"<p><code>1.234</code></p>\n"
"<p>If the default units format is FRACTIONAL, then, in addition to the decimal format, any of the following representations can be typed in:</p>\n"
"<p><code>1</code> (equals 1.0)</p>\n"
"<p><code>1/2</code> (equals 0.5)</p>\n"
"<p><code>1 1/2</code> (equals 1.5, note the blank separating the whole number and the fraction)</p>\n"
"<p>If you modify the value, you can always go back to the default value when the dialog was opened by clicking the <img src=\"default_active.png\"> button</p>" ) );

    xOriginButtonGroupLayout->addWidget( xOriginOffsetLengthSpinBox, 0, 1 );

    Line1_3 = new QFrame( xOriginButtonGroup, "Line1_3" );
    Line1_3->setFrameShape( QFrame::HLine );
    Line1_3->setFrameShadow( QFrame::Sunken );
    Line1_3->setFrameShape( QFrame::HLine );

    xOriginButtonGroupLayout->addMultiCellWidget( Line1_3, 1, 1, 0, 1 );

    xOriginImportedRadioButton = new QRadioButton( xOriginButtonGroup, "xOriginImportedRadioButton" );
    xOriginImportedRadioButton->setEnabled( FALSE );
    xOriginImportedRadioButton->setText( trUtf8( "Imported" ) );

    xOriginButtonGroupLayout->addWidget( xOriginImportedRadioButton, 2, 0 );

    xOriginConstraintChooser = new lCConstraintChooser( xOriginButtonGroup, "xOriginConstraintChooser" );
    xOriginConstraintChooser->setEnabled( FALSE );

    xOriginButtonGroupLayout->addWidget( xOriginConstraintChooser, 2, 1 );

    xOriginGroupBoxLayout->addMultiCellWidget( xOriginButtonGroup, 1, 1, 0, 1 );

    RectangleInfoDialogLayout->addMultiCellWidget( xOriginGroupBox, 2, 2, 0, 1 );

    yOriginGroupBox = new QGroupBox( this, "yOriginGroupBox" );
    yOriginGroupBox->setTitle( trUtf8( "Y Origin" ) );
    yOriginGroupBox->setColumnLayout(0, Qt::Vertical );
    yOriginGroupBox->layout()->setSpacing( 6 );
    yOriginGroupBox->layout()->setMargin( 11 );
    yOriginGroupBoxLayout = new QGridLayout( yOriginGroupBox->layout() );
    yOriginGroupBoxLayout->setAlignment( Qt::AlignTop );

    yOriginReferenceLabel = new QLabel( yOriginGroupBox, "yOriginReferenceLabel" );
    yOriginReferenceLabel->setText( trUtf8( "Reference:" ) );

    yOriginGroupBoxLayout->addWidget( yOriginReferenceLabel, 0, 0 );

    yOriginReferenceLineEdit = new QLineEdit( yOriginGroupBox, "yOriginReferenceLineEdit" );
    yOriginReferenceLineEdit->setReadOnly( TRUE );
    QToolTip::add( yOriginReferenceLineEdit, trUtf8( "Y origin offset reference." ) );
    QWhatsThis::add( yOriginReferenceLineEdit, trUtf8( "<p><b>Y Origin Offset Reference</b></p>\n"
"<p>This widget shows the geometry item which the Y0 edge of the rectangle is offset from. This value is not editable in this dialog; you must use the regular interface to adjust constraint references.</p>" ) );

    yOriginGroupBoxLayout->addWidget( yOriginReferenceLineEdit, 0, 1 );

    yOriginButtonGroup = new QButtonGroup( yOriginGroupBox, "yOriginButtonGroup" );
    yOriginButtonGroup->setTitle( trUtf8( "Offset" ) );
    yOriginButtonGroup->setColumnLayout(0, Qt::Vertical );
    yOriginButtonGroup->layout()->setSpacing( 6 );
    yOriginButtonGroup->layout()->setMargin( 11 );
    yOriginButtonGroupLayout = new QGridLayout( yOriginButtonGroup->layout() );
    yOriginButtonGroupLayout->setAlignment( Qt::AlignTop );

    yOriginSpecifiedRadioButton = new QRadioButton( yOriginButtonGroup, "yOriginSpecifiedRadioButton" );
    yOriginSpecifiedRadioButton->setText( trUtf8( "Specified" ) );
    QToolTip::add( yOriginSpecifiedRadioButton, trUtf8( "Use a specified y origin offset." ) );

    yOriginButtonGroupLayout->addWidget( yOriginSpecifiedRadioButton, 0, 0 );

    yOriginOffsetLengthSpinBox = new lCDefaultLengthSpinBox( yOriginButtonGroup, "yOriginOffsetLengthSpinBox" );
    QToolTip::add( yOriginOffsetLengthSpinBox, trUtf8( "Enter the y origin offset" ) );
    QWhatsThis::add( yOriginOffsetLengthSpinBox, trUtf8( "<p><b>Specified X Origin Offset</b></p>\n"
"<p>Enter the offset of the X origin of the rectangle (from the X origin reference). The units of the value are given in the default units specified in the application Preferences (so, you do not have to enter the units abbreviation).</p>\n"
"<p>If the default units format is DECIMAL, then the entered value can have the usual floating point representation, e.g.:</p>\n"
"<p><code>1.234</code></p>\n"
"<p>If the default units format is FRACTIONAL, then, in addition to the decimal format, any of the following representations can be typed in:</p>\n"
"<p><code>1</code> (equals 1.0)</p>\n"
"<p><code>1/2</code> (equals 0.5)</p>\n"
"<p><code>1 1/2</code> (equals 1.5, note the blank separating the whole number and the fraction)</p>\n"
"<p>If you modify the value, you can always go back to the default value when the dialog was opened by clicking the <img src=\"default_active.png\"> button</p>" ) );

    yOriginButtonGroupLayout->addWidget( yOriginOffsetLengthSpinBox, 0, 1 );

    Line1_3_2 = new QFrame( yOriginButtonGroup, "Line1_3_2" );
    Line1_3_2->setFrameShape( QFrame::HLine );
    Line1_3_2->setFrameShadow( QFrame::Sunken );
    Line1_3_2->setFrameShape( QFrame::HLine );

    yOriginButtonGroupLayout->addMultiCellWidget( Line1_3_2, 1, 1, 0, 1 );

    yOriginImportedRadioButton = new QRadioButton( yOriginButtonGroup, "yOriginImportedRadioButton" );
    yOriginImportedRadioButton->setEnabled( FALSE );
    yOriginImportedRadioButton->setText( trUtf8( "Imported" ) );

    yOriginButtonGroupLayout->addWidget( yOriginImportedRadioButton, 2, 0 );

    yOriginConstraintChooser = new lCConstraintChooser( yOriginButtonGroup, "yOriginConstraintChooser" );
    yOriginConstraintChooser->setEnabled( FALSE );

    yOriginButtonGroupLayout->addWidget( yOriginConstraintChooser, 2, 1 );

    yOriginGroupBoxLayout->addMultiCellWidget( yOriginButtonGroup, 1, 1, 0, 1 );

    RectangleInfoDialogLayout->addMultiCellWidget( yOriginGroupBox, 2, 2, 2, 4 );

    heightButtonGroup = new QButtonGroup( this, "heightButtonGroup" );
    heightButtonGroup->setTitle( trUtf8( "Height" ) );
    heightButtonGroup->setColumnLayout(0, Qt::Vertical );
    heightButtonGroup->layout()->setSpacing( 6 );
    heightButtonGroup->layout()->setMargin( 11 );
    heightButtonGroupLayout = new QGridLayout( heightButtonGroup->layout() );
    heightButtonGroupLayout->setAlignment( Qt::AlignTop );

    heightSpecifiedRadioButton = new QRadioButton( heightButtonGroup, "heightSpecifiedRadioButton" );
    heightSpecifiedRadioButton->setText( trUtf8( "Specified" ) );
    QToolTip::add( heightSpecifiedRadioButton, trUtf8( "Use a specified height." ) );
    QWhatsThis::add( heightSpecifiedRadioButton, trUtf8( "<p><b>Specified Height</b></p>\n"
"<p>Select this option if you want the rectangle to have an independent, specified height.</p>" ) );

    heightButtonGroupLayout->addWidget( heightSpecifiedRadioButton, 0, 0 );

    heightLengthSpinBox = new lCDefaultLengthSpinBox( heightButtonGroup, "heightLengthSpinBox" );
    QToolTip::add( heightLengthSpinBox, trUtf8( "Enter the height." ) );
    QWhatsThis::add( heightLengthSpinBox, trUtf8( "<p><b>Specified Height</b></p>\n"
"<p>Enter the height of the rectangle. The units of the value are given in the default units specified in the application Preferences (so, you do not have to enter the units abbreviation).</p>\n"
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

    RectangleInfoDialogLayout->addMultiCellWidget( heightButtonGroup, 1, 1, 2, 4 );

    nameEdit = new QLineEdit( this, "nameEdit" );
    QToolTip::add( nameEdit, trUtf8( "Edit the rectangle's name." ) );
    QWhatsThis::add( nameEdit, trUtf8( "<p><b>Name</b></p>\n"
"<p>Edit the name of the rectangle. The rectangle's name will be updated when the dialog is accepted. If there is already a rectangle on this page which has the same name, the name change will be rejected.</p>" ) );

    RectangleInfoDialogLayout->addWidget( nameEdit, 0, 1 );

    dimDisplaySideGroupBox = new QGroupBox( this, "dimDisplaySideGroupBox" );
    dimDisplaySideGroupBox->setTitle( trUtf8( "Dimension Display Side" ) );
    dimDisplaySideGroupBox->setColumnLayout(0, Qt::Vertical );
    dimDisplaySideGroupBox->layout()->setSpacing( 6 );
    dimDisplaySideGroupBox->layout()->setMargin( 11 );
    dimDisplaySideGroupBoxLayout = new QGridLayout( dimDisplaySideGroupBox->layout() );
    dimDisplaySideGroupBoxLayout->setAlignment( Qt::AlignTop );

    Layout2 = new QGridLayout( 0, 1, 1, 0, 6, "Layout2"); 

    belowButton = new QRadioButton( dimDisplaySideGroupBox, "belowButton" );
    belowButton->setText( trUtf8( "&Below" ) );
    QToolTip::add( belowButton, trUtf8( "Display width dimension below rectangle." ) );
    QWhatsThis::add( belowButton, trUtf8( "<p><b>Display Width Dimension Below Rectangle</b></p>\n"
"<p>If the rectangle has a valid width dimension, then it will be displayed relative to the bottom edge of the rectangle. The side of the bottom edge on which the dimension appears is controlled by an attribute of the dimension itself.</p>\n"
"<p>When might a rectangle <i>not</i> have a valid width dimension? If both X edges of the rectangle are constrained to outside figures, then the width of the rectangle is not controlled by a single parameter.</p>" ) );

    Layout2->addWidget( belowButton, 2, 1 );

    rightButton = new QRadioButton( dimDisplaySideGroupBox, "rightButton" );
    rightButton->setText( trUtf8( "&Right" ) );
    QToolTip::add( rightButton, trUtf8( "Display height dimension to right of rectangle." ) );
    QWhatsThis::add( rightButton, trUtf8( "<p><b>Display Height Dimension to the Right of the Rectangle</b></p>\n"
"<p>If the rectangle has a valid height dimension, then it will be displayed relative to the right edge of the rectangle. The side of the right edge on which the dimension appears is controlled by an attribute of the dimension itself.</p>\n"
"<p>When might a rectangle <i>not</i> have a valid height dimension? If both Y edges of the rectangle are constrained to outside figures, then the height of the rectangle is not controlled by a single parameter.</p>" ) );

    Layout2->addWidget( rightButton, 1, 2 );

    leftButton = new QRadioButton( dimDisplaySideGroupBox, "leftButton" );
    leftButton->setText( trUtf8( "&Left" ) );
    QToolTip::add( leftButton, trUtf8( "Display height dimension to left of rectangle." ) );
    QWhatsThis::add( leftButton, trUtf8( "<p><b>Display Height Dimension to the Left of the Rectangle</b></p>\n"
"<p>If the rectangle has a valid height dimension, then it will be displayed relative to the left edge of the rectangle. The side of the left edge on which the dimension appears is controlled by an attribute of the dimension itself.</p>\n"
"<p>When might a rectangle <i>not</i> have a valid height dimension? If both Y edges of the rectangle are constrained to outside figures, then the height of the rectangle is not controlled by a single parameter.</p>" ) );

    Layout2->addWidget( leftButton, 1, 0 );

    aboveButton = new QRadioButton( dimDisplaySideGroupBox, "aboveButton" );
    aboveButton->setText( trUtf8( "&Above" ) );
    QToolTip::add( aboveButton, trUtf8( "Display width dimension above rectangle." ) );
    QWhatsThis::add( aboveButton, trUtf8( "<p><b>Display Width Dimension Above Rectangle</b></p>\n"
"<p>If the rectangle has a valid width dimension, then it will be displayed relative to the top edge of the rectangle. The side of the top edge on which the dimension appears is controlled by an attribute of the dimension itself.</p>\n"
"<p>When might a rectangle <i>not</i> have a valid width dimension? If both X edges of the rectangle are constrained to outside figures, then the width of the rectangle is not controlled by a single parameter.</p>" ) );

    Layout2->addWidget( aboveButton, 0, 1 );

    dimDisplaySideGroupBoxLayout->addLayout( Layout2, 0, 0 );

    RectangleInfoDialogLayout->addMultiCellWidget( dimDisplaySideGroupBox, 3, 3, 0, 1 );
    QSpacerItem* spacer = new QSpacerItem( 0, 20, QSizePolicy::Minimum, QSizePolicy::Expanding );
    RectangleInfoDialogLayout->addItem( spacer, 4, 1 );

    buttonOk = new QPushButton( this, "buttonOk" );
    buttonOk->setText( trUtf8( "&OK" ) );
    buttonOk->setAccel( 276824143 );
    buttonOk->setAutoDefault( TRUE );
    buttonOk->setDefault( TRUE );

    RectangleInfoDialogLayout->addWidget( buttonOk, 5, 3 );

    buttonCancel = new QPushButton( this, "buttonCancel" );
    buttonCancel->setText( trUtf8( "&Cancel" ) );
    buttonCancel->setAccel( 276824131 );
    buttonCancel->setAutoDefault( TRUE );

    RectangleInfoDialogLayout->addWidget( buttonCancel, 5, 4 );

    widthButtonGroup = new QButtonGroup( this, "widthButtonGroup" );
    widthButtonGroup->setTitle( trUtf8( "Width" ) );
    widthButtonGroup->setColumnLayout(0, Qt::Vertical );
    widthButtonGroup->layout()->setSpacing( 6 );
    widthButtonGroup->layout()->setMargin( 11 );
    widthButtonGroupLayout = new QGridLayout( widthButtonGroup->layout() );
    widthButtonGroupLayout->setAlignment( Qt::AlignTop );

    widthSpecifiedRadioButton = new QRadioButton( widthButtonGroup, "widthSpecifiedRadioButton" );
    widthSpecifiedRadioButton->setText( trUtf8( "Specified" ) );
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

    RectangleInfoDialogLayout->addMultiCellWidget( widthButtonGroup, 1, 1, 0, 1 );

    rectangleStyleGroupBox = new QGroupBox( this, "rectangleStyleGroupBox" );
    rectangleStyleGroupBox->setTitle( trUtf8( "Style" ) );
    rectangleStyleGroupBox->setColumnLayout(0, Qt::Vertical );
    rectangleStyleGroupBox->layout()->setSpacing( 6 );
    rectangleStyleGroupBox->layout()->setMargin( 11 );
    rectangleStyleGroupBoxLayout = new QGridLayout( rectangleStyleGroupBox->layout() );
    rectangleStyleGroupBoxLayout->setAlignment( Qt::AlignTop );

    defaultStyleLabel = new QLabel( rectangleStyleGroupBox, "defaultStyleLabel" );
    defaultStyleLabel->setText( trUtf8( "Default (use Page attributes)" ) );
    QToolTip::add( defaultStyleLabel, trUtf8( "Default (use page attributes)." ) );

    rectangleStyleGroupBoxLayout->addMultiCellWidget( defaultStyleLabel, 0, 0, 1, 2 );

    wireframeStyleLabel = new QLabel( rectangleStyleGroupBox, "wireframeStyleLabel" );
    wireframeStyleLabel->setText( trUtf8( "&Wireframe" ) );
    wireframeStyleLabel->setAlignment( int( QLabel::AlignBottom ) );
    QToolTip::add( wireframeStyleLabel, trUtf8( "Wireframe color." ) );

    rectangleStyleGroupBoxLayout->addWidget( wireframeStyleLabel, 2, 1 );

    wireframeStyleColorChooser = new lCColorChooser( rectangleStyleGroupBox, "wireframeStyleColorChooser" );
    QToolTip::add( wireframeStyleColorChooser, trUtf8( "Choose wireframe color." ) );
    QWhatsThis::add( wireframeStyleColorChooser, trUtf8( "<p><b>Wireframe Color</b></p>\n"
"<p>By default, rectangles are drawn using the default geometry color from the application Preferences. If you want to use a different color, you can choose one here.</p>\n"
"<p>If you modify the value, you can always go back to the default value when the dialog was opened by clicking the <img src=\"default_active.png\"> button</p>" ) );

    rectangleStyleGroupBoxLayout->addWidget( wireframeStyleColorChooser, 2, 2 );

    edgeStyleComboBox = new lCDefaultComboBox( rectangleStyleGroupBox, "edgeStyleComboBox" );
    QToolTip::add( edgeStyleComboBox, trUtf8( "Choose an edge stipple." ) );
    QWhatsThis::add( edgeStyleComboBox, trUtf8( "<p><b>Edge Stipple</b></p>\n"
"<p>By default, rectangles are drawn with a solid line. If you want to emulate a rectangle with hidden edges in a sketch, you can set the edge stipple to one of the broken line types, e.g. dashed.</p>\n"
"<p>If you modify the value, you can always go back to the default value when the dialog was opened by clicking the <img src=\"default_active.png\"> button</p>" ) );

    rectangleStyleGroupBoxLayout->addWidget( edgeStyleComboBox, 1, 2 );

    edgeStyleLabel = new QLabel( rectangleStyleGroupBox, "edgeStyleLabel" );
    edgeStyleLabel->setText( trUtf8( "&Edge" ) );
    edgeStyleLabel->setAlignment( int( QLabel::AlignBottom ) );
    QToolTip::add( edgeStyleLabel, trUtf8( "Edge stipple." ) );

    rectangleStyleGroupBoxLayout->addWidget( edgeStyleLabel, 1, 1 );

    rectangleStyleSlider = new QSlider( rectangleStyleGroupBox, "rectangleStyleSlider" );
    rectangleStyleSlider->setMaxValue( 5 );
    rectangleStyleSlider->setPageStep( 1 );
    rectangleStyleSlider->setOrientation( QSlider::Vertical );
    QToolTip::add( rectangleStyleSlider, trUtf8( "Select rectangle style." ) );
    QWhatsThis::add( rectangleStyleSlider, trUtf8( "<p><b>Select Rectangle Style</b></p>\n"
"<p>The rectangle style slider lets you set an increasingly complicated style for the rectangle. See the \"What's This?\" help for each of the individual options for more detail.</p>" ) );

    rectangleStyleGroupBoxLayout->addMultiCellWidget( rectangleStyleSlider, 0, 5, 0, 0 );

    hiddenStyleSpinBox = new lCDefaultSpinBox( rectangleStyleGroupBox, "hiddenStyleSpinBox" );
    hiddenStyleSpinBox->setEnabled( FALSE );
    hiddenStyleSpinBox->setMaxValue( 100 );
    hiddenStyleSpinBox->setDefaultValue( 50 );
    QToolTip::add( hiddenStyleSpinBox, trUtf8( "Hidden line removal is not applicable to 2D entities" ) );

    rectangleStyleGroupBoxLayout->addWidget( hiddenStyleSpinBox, 3, 2 );

    hiddenStyleLabel = new QLabel( rectangleStyleGroupBox, "hiddenStyleLabel" );
    hiddenStyleLabel->setEnabled( FALSE );
    hiddenStyleLabel->setText( trUtf8( "Hidden" ) );
    hiddenStyleLabel->setAlignment( int( QLabel::AlignBottom ) );
    QToolTip::add( hiddenStyleLabel, trUtf8( "Hidden line removal is not applicable to 2D entities" ) );

    rectangleStyleGroupBoxLayout->addWidget( hiddenStyleLabel, 3, 1 );

    solidStyleLabel = new QLabel( rectangleStyleGroupBox, "solidStyleLabel" );
    solidStyleLabel->setText( trUtf8( "&Solid" ) );
    solidStyleLabel->setAlignment( int( QLabel::AlignBottom ) );
    QToolTip::add( solidStyleLabel, trUtf8( "Solid color." ) );

    rectangleStyleGroupBoxLayout->addWidget( solidStyleLabel, 4, 1 );

    solidStyleColorChooser = new lCColorChooser( rectangleStyleGroupBox, "solidStyleColorChooser" );
    QToolTip::add( solidStyleColorChooser, trUtf8( "Choose solid color." ) );
    QWhatsThis::add( solidStyleColorChooser, trUtf8( "<p><b>Solid Color</b></p>\n"
"<p>By default, rectangles are drawn in wireframe. If you choose this style, then the rectangle will be drawn as a solid rectangle in the chosen color.</p>\n"
"<p>If you modify the value, you can always go back to the default value when the dialog was opened by clicking the <img src=\"default_active.png\"> button</p>" ) );

    rectangleStyleGroupBoxLayout->addWidget( solidStyleColorChooser, 4, 2 );

    textureStyleLabel = new QLabel( rectangleStyleGroupBox, "textureStyleLabel" );
    textureStyleLabel->setText( trUtf8( "&Texture" ) );
    textureStyleLabel->setAlignment( int( QLabel::AlignBottom ) );
    QToolTip::add( textureStyleLabel, trUtf8( "Texture pattern file." ) );

    rectangleStyleGroupBoxLayout->addWidget( textureStyleLabel, 5, 1 );

    textureStyleFileChooser = new lCDefaultFileChooser( rectangleStyleGroupBox, "textureStyleFileChooser" );
    QToolTip::add( textureStyleFileChooser, trUtf8( "Choose texture pattern file." ) );
    QWhatsThis::add( textureStyleFileChooser, trUtf8( "<p><b>Texture Pattern File</b></p>\n"
"<p>By default, rectangles are drawn in wireframe. If you choose this style, then the rectangle will be drawn as a solid, textured rectangle using the image from the given file.</p>\n"
"<p>The image must meet certain properties in order to be useful as a texture. <i>lignumCAD</i> will adjust the image size to meet the requirements of OpenGL textures, but choosing a very large image may yield an impractical texture. Also, the best resolution to use is one which is close to the ratio between the display and the default scale of <i>lignumCAD</i>. Currently, an image scanned at about 20 dots per inch and approximately 256 pixels x 256 pixels is the optimal size.</p>\n"
"<p>If you modify the value, you can always go back to the default value when the dialog was opened by clicking the <img src=\"default_active.png\"> button</p>" ) );

    rectangleStyleGroupBoxLayout->addWidget( textureStyleFileChooser, 5, 2 );

    RectangleInfoDialogLayout->addMultiCellWidget( rectangleStyleGroupBox, 3, 4, 2, 4 );

    buttonHelp = new QPushButton( this, "buttonHelp" );
    buttonHelp->setText( trUtf8( "&Help" ) );
    buttonHelp->setAccel( 276824136 );
    buttonHelp->setAutoDefault( TRUE );

    RectangleInfoDialogLayout->addWidget( buttonHelp, 5, 0 );
    QSpacerItem* spacer_2 = new QSpacerItem( 20, 0, QSizePolicy::Expanding, QSizePolicy::Minimum );
    RectangleInfoDialogLayout->addMultiCell( spacer_2, 5, 5, 1, 2 );

    // signals and slots connections
    connect( buttonOk, SIGNAL( clicked() ), this, SLOT( accept() ) );
    connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
    connect( rectangleStyleSlider, SIGNAL( valueChanged(int) ), this, SLOT( rectangleStyleSlider_valueChanged(int) ) );
    connect( buttonHelp, SIGNAL( clicked() ), this, SLOT( buttonHelp_clicked() ) );

    // tab order
    setTabOrder( nameEdit, widthSpecifiedRadioButton );
    setTabOrder( widthSpecifiedRadioButton, widthLengthSpinBox );
    setTabOrder( widthLengthSpinBox, heightSpecifiedRadioButton );
    setTabOrder( heightSpecifiedRadioButton, heightLengthSpinBox );
    setTabOrder( heightLengthSpinBox, xOriginSpecifiedRadioButton );
    setTabOrder( xOriginSpecifiedRadioButton, xOriginOffsetLengthSpinBox );
    setTabOrder( xOriginOffsetLengthSpinBox, yOriginSpecifiedRadioButton );
    setTabOrder( yOriginSpecifiedRadioButton, yOriginOffsetLengthSpinBox );
    setTabOrder( yOriginOffsetLengthSpinBox, aboveButton );
    setTabOrder( aboveButton, belowButton );
    setTabOrder( belowButton, leftButton );
    setTabOrder( leftButton, rightButton );
    setTabOrder( rightButton, rectangleStyleSlider );
    setTabOrder( rectangleStyleSlider, edgeStyleComboBox );
    setTabOrder( edgeStyleComboBox, wireframeStyleColorChooser );
    setTabOrder( wireframeStyleColorChooser, hiddenStyleSpinBox );
    setTabOrder( hiddenStyleSpinBox, solidStyleColorChooser );
    setTabOrder( solidStyleColorChooser, buttonOk );
    setTabOrder( buttonOk, buttonCancel );
    setTabOrder( buttonCancel, buttonHelp );
    setTabOrder( buttonHelp, widthImportedRadioButton );
    setTabOrder( widthImportedRadioButton, widthConstraintChooser );
    setTabOrder( widthConstraintChooser, xOriginReferenceLineEdit );
    setTabOrder( xOriginReferenceLineEdit, xOriginImportedRadioButton );
    setTabOrder( xOriginImportedRadioButton, xOriginConstraintChooser );
    setTabOrder( xOriginConstraintChooser, yOriginReferenceLineEdit );
    setTabOrder( yOriginReferenceLineEdit, yOriginImportedRadioButton );
    setTabOrder( yOriginImportedRadioButton, yOriginConstraintChooser );
    setTabOrder( yOriginConstraintChooser, heightImportedRadioButton );
    setTabOrder( heightImportedRadioButton, heightConstraintChooser );

    // buddies
    nameLabel->setBuddy( nameEdit );
    wireframeStyleLabel->setBuddy( wireframeStyleColorChooser );
    edgeStyleLabel->setBuddy( edgeStyleComboBox );
    solidStyleLabel->setBuddy( solidStyleColorChooser );
    textureStyleLabel->setBuddy( textureStyleFileChooser );
    init();
}

/*  
 *  Destroys the object and frees any allocated resources
 */
RectangleInfoDialog::~RectangleInfoDialog()
{
    // no need to delete child widgets, Qt does it all for us
}

