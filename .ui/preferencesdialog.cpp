/****************************************************************************
** Form implementation generated from reading ui file 'preferencesdialog.ui'
**
** Created: Wed Dec 18 08:54:55 2002
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#include "preferencesdialog.h"

#include <qvariant.h>
#include <lccolorchooser.h>
#include <lcdefaultcombobox.h>
#include <lcdefaultfilechooser.h>
#include <lcdefaultlengthspinbox.h>
#include <lcdefaultratiospinbox.h>
#include <lcfontchooser.h>
#include <qbuttongroup.h>
#include <qfileinfo.h>
#include <qframe.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qlistbox.h>
#include <qpaintdevicemetrics.h>
#include <qpainter.h>
#include <qpicture.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qslider.h>
#include <qtabwidget.h>
#include <qwidget.h>
#include <qmime.h>
#include <qdragobject.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qimage.h>
#include <qpixmap.h>

#include "businessinfo.h"
#include "preferencesdialog.ui.h"
static QPixmap uic_load_pixmap_PreferencesDialog( const QString &name )
{
    const QMimeSource *m = QMimeSourceFactory::defaultFactory()->data( name );
    if ( !m )
	return QPixmap();
    QPixmap pix;
    QImageDrag::decode( m, pix );
    return pix;
}
/* 
 *  Constructs a PreferencesDialog which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
PreferencesDialog::PreferencesDialog( QWidget* parent,  const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
	setName( "PreferencesDialog" );
    resize( 694, 513 ); 
    setCaption( trUtf8( "Application Default Preferences" ) );
    setIcon( uic_load_pixmap_PreferencesDialog( "lignumCAD.png" ) );
    setSizeGripEnabled( TRUE );
    PreferencesDialogLayout = new QGridLayout( this, 1, 1, 11, 6, "PreferencesDialogLayout"); 

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

    PreferencesDialogLayout->addLayout( Layout1, 2, 0 );

    exampleBox = new QGroupBox( this, "exampleBox" );
    exampleBox->setTitle( trUtf8( "Example" ) );
    exampleBox->setColumnLayout(0, Qt::Vertical );
    exampleBox->layout()->setSpacing( 6 );
    exampleBox->layout()->setMargin( 11 );
    exampleBoxLayout = new QGridLayout( exampleBox->layout() );
    exampleBoxLayout->setAlignment( Qt::AlignTop );

    exampleFrame = new QFrame( exampleBox, "exampleFrame" );
    exampleFrame->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)7, 0, 0, exampleFrame->sizePolicy().hasHeightForWidth() ) );
    exampleFrame->setMinimumSize( QSize( 0, 100 ) );
    exampleFrame->setFrameShape( QFrame::StyledPanel );
    exampleFrame->setFrameShadow( QFrame::Raised );
    exampleFrame->setLineWidth( 1 );

    exampleBoxLayout->addWidget( exampleFrame, 0, 0 );

    PreferencesDialogLayout->addWidget( exampleBox, 1, 0 );

    preferencesTabWidget = new QTabWidget( this, "preferencesTabWidget" );

    identificationPage = new QWidget( preferencesTabWidget, "identificationPage" );
    identificationPageLayout = new QGridLayout( identificationPage, 1, 1, 11, 6, "identificationPageLayout"); 

    businessLocationLineEdit = new QLineEdit( identificationPage, "businessLocationLineEdit" );
    QToolTip::add( businessLocationLineEdit, trUtf8( "Change the business (or user) location." ) );
    QWhatsThis::add( businessLocationLineEdit, trUtf8( "<p><b>Edit Business (or User) Location</b></p>\n"
"<p>Change the location of the Business (or user). A typical format is \"City, State\" (at least in this USA).\n"
"This string appears on printed pages in the revision control box.</p>" ) );

    identificationPageLayout->addWidget( businessLocationLineEdit, 1, 1 );

    businessLocationLabel = new QLabel( identificationPage, "businessLocationLabel" );
    businessLocationLabel->setText( trUtf8( "&Location:" ) );
    QToolTip::add( businessLocationLabel, trUtf8( "Business (or user) location." ) );

    identificationPageLayout->addWidget( businessLocationLabel, 1, 0 );

    businessNameLabel = new QLabel( identificationPage, "businessNameLabel" );
    businessNameLabel->setText( trUtf8( "&Name:" ) );
    QToolTip::add( businessNameLabel, trUtf8( "Business or User name" ) );

    identificationPageLayout->addWidget( businessNameLabel, 0, 0 );

    businessNameLineEdit = new QLineEdit( identificationPage, "businessNameLineEdit" );
    QToolTip::add( businessNameLineEdit, trUtf8( "Edit the business (or user) name." ) );
    QWhatsThis::add( businessNameLineEdit, trUtf8( "<p><b>Edit Business (or User) Name</b></p>\n"
"<p>Change the name of the Business (or user). This string appears on printed pages in the revision control box.</p>" ) );

    identificationPageLayout->addWidget( businessNameLineEdit, 0, 1 );

    GroupBox10 = new QGroupBox( identificationPage, "GroupBox10" );
    GroupBox10->setEnabled( TRUE );
    GroupBox10->setTitle( trUtf8( "Logo (optional)" ) );
    GroupBox10->setColumnLayout(0, Qt::Vertical );
    GroupBox10->layout()->setSpacing( 6 );
    GroupBox10->layout()->setMargin( 11 );
    GroupBox10Layout = new QGridLayout( GroupBox10->layout() );
    GroupBox10Layout->setAlignment( Qt::AlignTop );

    logoFrame = new QFrame( GroupBox10, "logoFrame" );
    logoFrame->setMinimumSize( QSize( 170, 170 ) );
    logoFrame->setFrameShape( QFrame::StyledPanel );
    logoFrame->setFrameShadow( QFrame::Raised );
    logoFrameLayout = new QGridLayout( logoFrame, 1, 1, 11, 6, "logoFrameLayout"); 

    logoLabel = new QLabel( logoFrame, "logoLabel" );
    logoLabel->setEnabled( FALSE );
    QFont logoLabel_font(  logoLabel->font() );
    logoLabel_font.setPointSize( 24 );
    logoLabel->setFont( logoLabel_font ); 
    logoLabel->setText( trUtf8( "No Logo" ) );
    logoLabel->setAlignment( int( QLabel::AlignCenter ) );

    logoFrameLayout->addWidget( logoLabel, 0, 0 );

    GroupBox10Layout->addMultiCellWidget( logoFrame, 0, 2, 1, 1 );

    logoFileChooser = new lCDefaultFileChooser( GroupBox10, "logoFileChooser" );
    QToolTip::add( logoFileChooser, trUtf8( "Enter the file name of a graphical logo." ) );
    QWhatsThis::add( logoFileChooser, trUtf8( "<p><b>Edit the Logo File Name</b></p>\n"
"<p>Change the name of the file which contains a logo for the business (or user). This image appears on printed pages in the revision control box.</p>\n"
"<p>You have two choices here: You can enter either a raster image file name (e.g., PNG or JPEG; note that you are limited to the formats that Qt can recognize), or an SVG (Scalable Vector Graphics) file. The SVG file will probably look best when printed (unless the raster image is very high resolution). Note that with the SVG file, you are limited to Qt's ability to interpret it.</p>\n"
"<p>If you modify the value, you can always go back to the default value when the dialog was opened by clicking the <img src=\"default_active.png\"> button</p>" ) );

    GroupBox10Layout->addWidget( logoFileChooser, 1, 0 );
    QSpacerItem* spacer_2 = new QSpacerItem( 0, 20, QSizePolicy::Minimum, QSizePolicy::Expanding );
    GroupBox10Layout->addItem( spacer_2, 0, 0 );
    QSpacerItem* spacer_3 = new QSpacerItem( 0, 20, QSizePolicy::Minimum, QSizePolicy::Expanding );
    GroupBox10Layout->addItem( spacer_3, 2, 0 );

    identificationPageLayout->addMultiCellWidget( GroupBox10, 2, 2, 0, 1 );
    QSpacerItem* spacer_4 = new QSpacerItem( 20, 0, QSizePolicy::Expanding, QSizePolicy::Minimum );
    identificationPageLayout->addItem( spacer_4, 2, 2 );
    preferencesTabWidget->insertTab( identificationPage, trUtf8( "Identification" ) );

    unitsPage = new QWidget( preferencesTabWidget, "unitsPage" );
    unitsPageLayout = new QGridLayout( unitsPage, 1, 1, 11, 6, "unitsPageLayout"); 

    Layout8 = new QVBoxLayout( 0, 0, 6, "Layout8"); 

    formatButtonGroup = new QButtonGroup( unitsPage, "formatButtonGroup" );
    formatButtonGroup->setTitle( trUtf8( "Format" ) );
    formatButtonGroup->setExclusive( TRUE );
    formatButtonGroup->setColumnLayout(0, Qt::Vertical );
    formatButtonGroup->layout()->setSpacing( 6 );
    formatButtonGroup->layout()->setMargin( 11 );
    formatButtonGroupLayout = new QGridLayout( formatButtonGroup->layout() );
    formatButtonGroupLayout->setAlignment( Qt::AlignTop );

    fractionalRadioButton = new QRadioButton( formatButtonGroup, "fractionalRadioButton" );
    fractionalRadioButton->setText( trUtf8( "&Fractional" ) );
    QToolTip::add( fractionalRadioButton, trUtf8( "Display lengths as fractional values." ) );
    QWhatsThis::add( fractionalRadioButton, trUtf8( "<p><b>Fractional Representation</b></p>\n"
"<p>With this choice, lengths will be displayed as mixed fractions. For example, one and a half inches, will be shown as <code>1 1/2\"</code></p>" ) );

    formatButtonGroupLayout->addWidget( fractionalRadioButton, 0, 0 );

    decimalRadioButton = new QRadioButton( formatButtonGroup, "decimalRadioButton" );
    decimalRadioButton->setText( trUtf8( "&Decimal" ) );
    QToolTip::add( decimalRadioButton, trUtf8( "Display lengths as decimal values." ) );
    QWhatsThis::add( decimalRadioButton, trUtf8( "<p><b>Decimal Representation</b></p>\n"
"<p>With this choice, lengths will be displayed in the usual floating point notation. For example, one and a half inches, will be shown as <code>1.5\"</code></p>" ) );

    formatButtonGroupLayout->addWidget( decimalRadioButton, 1, 0 );
    Layout8->addWidget( formatButtonGroup );

    Layout7 = new QHBoxLayout( 0, 0, 6, "Layout7"); 

    precisionLabel = new QLabel( unitsPage, "precisionLabel" );
    precisionLabel->setText( trUtf8( "&Precision:" ) );
    QToolTip::add( precisionLabel, trUtf8( "Input and display precision." ) );
    Layout7->addWidget( precisionLabel );

    precisionComboBox = new lCDefaultComboBox( unitsPage, "precisionComboBox" );
    QToolTip::add( precisionComboBox, trUtf8( "Enter the input and display precision." ) );
    QWhatsThis::add( precisionComboBox, trUtf8( "<p><b>Input and Display Precision</b></p>\n"
"<p>The precision setting here determines two things:\n"
"<ol>\n"
"<li>The number of displayed significant digits (if the representation is in decimal mode).</li>\n"
"<li>The <i>quantization</i> of user input. For example, the default length unit display representation is inches, in fractional format, and a precision of 1/8 inch. In this mode, as you input lengths through the main user interface, every distance will be a multiple of 1/8 inch.</li>\n"
"</ol></p>\n"
"<p>In general, it is not a good idea to decrease the precision of a model after you have started. That is, do not start at 1/16\" precision and then change to 1/8\" later. However, it is OK to go to higher precisions.</p>\n"
"<p>If you modify the precision, you can always go back to the default value of the selected length unit representation by clicking the <img src=\"default_active.png\"> button</p>" ) );
    Layout7->addWidget( precisionComboBox );
    Layout8->addLayout( Layout7 );

    unitsPageLayout->addLayout( Layout8, 0, 1 );

    UnitsBox = new QGroupBox( unitsPage, "UnitsBox" );
    UnitsBox->setTitle( trUtf8( "Length Units [Abbreviation]" ) );
    UnitsBox->setColumnLayout(0, Qt::Vertical );
    UnitsBox->layout()->setSpacing( 6 );
    UnitsBox->layout()->setMargin( 11 );
    UnitsBoxLayout = new QGridLayout( UnitsBox->layout() );
    UnitsBoxLayout->setAlignment( Qt::AlignTop );

    unitsListBox = new QListBox( UnitsBox, "unitsListBox" );
    QToolTip::add( unitsListBox, trUtf8( "Length units and their abbreviations." ) );
    QWhatsThis::add( unitsListBox, trUtf8( "<p><b>Length Units</p></b>\n"
"<p>Select the length unit to be used in the presentation and input of distances. Each row lists a unit name and the abbreviation with which it will be displayed. For example, <code>Inch[in]</code> will display lengths as <code>1in</code>, whereas <code>Inch[\"]</code> will display length as <code>1\"</code>.</p>" ) );

    UnitsBoxLayout->addWidget( unitsListBox, 0, 0 );

    unitsPageLayout->addMultiCellWidget( UnitsBox, 0, 1, 0, 0 );
    QSpacerItem* spacer_5 = new QSpacerItem( 0, 20, QSizePolicy::Minimum, QSizePolicy::Expanding );
    unitsPageLayout->addItem( spacer_5, 1, 1 );
    QSpacerItem* spacer_6 = new QSpacerItem( 20, 0, QSizePolicy::Expanding, QSizePolicy::Minimum );
    unitsPageLayout->addItem( spacer_6, 0, 2 );
    preferencesTabWidget->insertTab( unitsPage, trUtf8( "Units" ) );

    colorSchemePage = new QWidget( preferencesTabWidget, "colorSchemePage" );
    colorSchemePageLayout = new QGridLayout( colorSchemePage, 1, 1, 11, 6, "colorSchemePageLayout"); 

    Layout7_3 = new QVBoxLayout( 0, 0, 6, "Layout7_3"); 

    predefinedRadioButton = new QRadioButton( colorSchemePage, "predefinedRadioButton" );
    predefinedRadioButton->setText( trUtf8( "&Predefined Schemes" ) );
    QToolTip::add( predefinedRadioButton, trUtf8( "Use predefined schemes." ) );
    QWhatsThis::add( predefinedRadioButton, trUtf8( "<p><b>Predefined schemes</b></p>\n"
"<p><i>lignumCAD</i> has several predefined color schemes compiled into the program. If you activate this option, you can only pick from the list below.</p>" ) );
    Layout7_3->addWidget( predefinedRadioButton );

    colorSchemeListBox = new QListBox( colorSchemePage, "colorSchemeListBox" );
    colorSchemeListBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)7, 0, 0, colorSchemeListBox->sizePolicy().hasHeightForWidth() ) );
    QToolTip::add( colorSchemeListBox, trUtf8( "Predefined schemes." ) );
    QWhatsThis::add( colorSchemeListBox, trUtf8( "<p><b>Predefined Schemes</b></p>\n"
"<p>This is a list of color schemes which are compiled into <i>lignumCAD</i>. If you want to add a new predefined scheme, look at the constructor for the <code>OpenGLGlobals</code> class in the source file <code>openglbase.cpp</code>. (A later version of <i>lignumCAD</i> will probably allow you to define these through the GUI.)</p>" ) );
    Layout7_3->addWidget( colorSchemeListBox );

    colorSchemePageLayout->addMultiCellLayout( Layout7_3, 0, 1, 0, 0 );

    Layout5 = new QHBoxLayout( 0, 0, 6, "Layout5"); 

    foregroundGroupBox = new QGroupBox( colorSchemePage, "foregroundGroupBox" );
    foregroundGroupBox->setTitle( trUtf8( "Foreground" ) );
    foregroundGroupBox->setColumnLayout(0, Qt::Vertical );
    foregroundGroupBox->layout()->setSpacing( 6 );
    foregroundGroupBox->layout()->setMargin( 11 );
    foregroundGroupBoxLayout = new QGridLayout( foregroundGroupBox->layout() );
    foregroundGroupBoxLayout->setAlignment( Qt::AlignTop );

    geometryColorChooser = new lCColorChooser( foregroundGroupBox, "geometryColorChooser" );
    geometryColorChooser->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)1, 0, 0, geometryColorChooser->sizePolicy().hasHeightForWidth() ) );
    QToolTip::add( geometryColorChooser, trUtf8( "Choose a color for geometry." ) );
    QWhatsThis::add( geometryColorChooser, trUtf8( "<p><b>Geometry Color</b></p>\n"
"<p>This is the default color for geometry items, such as rectangles. The geometry color can also be overridden on a figure-wise basis.</p>\n"
"<p>If you modify the color, you can always go back to the default value by clicking the <img src=\"default_active.png\"> button</p>" ) );

    foregroundGroupBoxLayout->addWidget( geometryColorChooser, 0, 1 );

    geometryColorLabel = new QLabel( foregroundGroupBox, "geometryColorLabel" );
    geometryColorLabel->setText( trUtf8( "G&eometry Color:" ) );

    foregroundGroupBoxLayout->addWidget( geometryColorLabel, 0, 0 );

    annotationColorLabel = new QLabel( foregroundGroupBox, "annotationColorLabel" );
    annotationColorLabel->setText( trUtf8( "&Annotation Color:" ) );

    foregroundGroupBoxLayout->addWidget( annotationColorLabel, 1, 0 );

    annotationColorChooser = new lCColorChooser( foregroundGroupBox, "annotationColorChooser" );
    annotationColorChooser->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)1, 0, 0, annotationColorChooser->sizePolicy().hasHeightForWidth() ) );
    QToolTip::add( annotationColorChooser, trUtf8( "Choose an annotation color." ) );
    QWhatsThis::add( annotationColorChooser, trUtf8( "<p><b>Geometry Color</b></p>\n"
"<p>This is the default color for annotation items, such as centerlines and text notes.</p>\n"
"<p>If you modify the color, you can always go back to the default value by clicking the <img src=\"default_active.png\"> button</p>" ) );

    foregroundGroupBoxLayout->addWidget( annotationColorChooser, 1, 1 );

    gridColorLabel = new QLabel( foregroundGroupBox, "gridColorLabel" );
    gridColorLabel->setText( trUtf8( "&Grid Color:" ) );

    foregroundGroupBoxLayout->addWidget( gridColorLabel, 2, 0 );

    gridColorChooser = new lCColorChooser( foregroundGroupBox, "gridColorChooser" );
    QToolTip::add( gridColorChooser, trUtf8( "Choose a color for the sketch page grid." ) );
    QWhatsThis::add( gridColorChooser, trUtf8( "<p><b>Grid Color</b></p>\n"
"<p>This is the default color for the grid shown on sketch pages.</p>\n"
"<p>If you modify the color, you can always go back to the default value by clicking the <img src=\"default_active.png\"> button</p>" ) );

    foregroundGroupBoxLayout->addWidget( gridColorChooser, 2, 1 );

    constraintColorGroupBox = new QGroupBox( foregroundGroupBox, "constraintColorGroupBox" );
    constraintColorGroupBox->setTitle( trUtf8( "Constraint Colors" ) );
    constraintColorGroupBox->setColumnLayout(0, Qt::Vertical );
    constraintColorGroupBox->layout()->setSpacing( 6 );
    constraintColorGroupBox->layout()->setMargin( 11 );
    constraintColorGroupBoxLayout = new QGridLayout( constraintColorGroupBox->layout() );
    constraintColorGroupBoxLayout->setAlignment( Qt::AlignTop );

    constraintPrimaryColorChooser = new lCColorChooser( constraintColorGroupBox, "constraintPrimaryColorChooser" );
    QToolTip::add( constraintPrimaryColorChooser, trUtf8( "Choose a color for the primary entity in a constraint relationship." ) );
    QWhatsThis::add( constraintPrimaryColorChooser, trUtf8( "<p><b>Constraint Primary Color</b></p>\n"
"<p>In <i>lignumCAD</i>, constraints are the used to relate two geometry entities to one another. For example, the width of a rectangle can be characterized by one X edge being offset from the other, fixed, X edge by an amount equal to the width of the rectangle. Such an offset is called a <i>dimension</i> constraint. There is also a directional sense to the constraint; one edge is the primary member and the other is the secondary member. In the width example, the offset edge is the primary member.</p>\n"
"<p>When constraints are rendered on the screen, the indicator of the constraint primary member is drawn in this color.</p>\n"
"<p>If you modify the color, you can always go back to the default value by clicking the <img src=\"default_active.png\"> button.</p>" ) );

    constraintColorGroupBoxLayout->addWidget( constraintPrimaryColorChooser, 0, 1 );

    constrainSecondaryColorLabel = new QLabel( constraintColorGroupBox, "constrainSecondaryColorLabel" );
    constrainSecondaryColorLabel->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)5, 0, 0, constrainSecondaryColorLabel->sizePolicy().hasHeightForWidth() ) );
    constrainSecondaryColorLabel->setText( trUtf8( "&Secondary:" ) );

    constraintColorGroupBoxLayout->addWidget( constrainSecondaryColorLabel, 1, 0 );

    constraintSecondaryColorChooser = new lCColorChooser( constraintColorGroupBox, "constraintSecondaryColorChooser" );
    QToolTip::add( constraintSecondaryColorChooser, trUtf8( "Choose a color for the secondary entity in a constraint relationship." ) );
    QWhatsThis::add( constraintSecondaryColorChooser, trUtf8( "<p><b>Constraint Secondary Color</b></p>\n"
"<p>In <i>lignumCAD</i>, constraints are the used to relate two geometry entities to one another. For example, the width of a rectangle can be characterized by one X edge being offset from the other, fixed, X edge, by an amount equal to the width of the rectangle. Such an offset is called a <i>dimension</i> constraint. There is also a directional sense to the constraint; one edge is the primary member and the other is the secondary member. \n"
"In the width example, the fixed edge is the secondary member.\n"
"<p>When constraints are rendered on the screen, the indicator of the constraint secondary member is drawn in this color.</p>\n"
"<p>If you modify the color, you can always go back to the default value by clicking the <img src=\"default_active.png\"> button.</p>" ) );

    constraintColorGroupBoxLayout->addWidget( constraintSecondaryColorChooser, 1, 1 );

    constraintPrimaryColorLabel = new QLabel( constraintColorGroupBox, "constraintPrimaryColorLabel" );
    constraintPrimaryColorLabel->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)5, 0, 0, constraintPrimaryColorLabel->sizePolicy().hasHeightForWidth() ) );
    constraintPrimaryColorLabel->setText( trUtf8( "Pr&imary:" ) );

    constraintColorGroupBoxLayout->addWidget( constraintPrimaryColorLabel, 0, 0 );

    foregroundGroupBoxLayout->addMultiCellWidget( constraintColorGroupBox, 3, 3, 0, 1 );
    Layout5->addWidget( foregroundGroupBox );

    backgroundGroupBox = new QGroupBox( colorSchemePage, "backgroundGroupBox" );
    backgroundGroupBox->setTitle( trUtf8( "Background" ) );
    backgroundGroupBox->setColumnLayout(0, Qt::Vertical );
    backgroundGroupBox->layout()->setSpacing( 6 );
    backgroundGroupBox->layout()->setMargin( 11 );
    backgroundGroupBoxLayout = new QGridLayout( backgroundGroupBox->layout() );
    backgroundGroupBoxLayout->setAlignment( Qt::AlignTop );

    backgroundSlider = new QSlider( backgroundGroupBox, "backgroundSlider" );
    backgroundSlider->setMaxValue( 2 );
    backgroundSlider->setPageStep( 1 );
    backgroundSlider->setOrientation( QSlider::Vertical );
    QToolTip::add( backgroundSlider, trUtf8( "Select page background style." ) );
    QWhatsThis::add( backgroundSlider, trUtf8( "<p><b>Select Page Background Style</b></p>\n"
"<p>The background style slider lets you set an increasingly complicated style for the page background. See the \"What's This?\" help for each of the individual options for more detail.</p>" ) );

    backgroundGroupBoxLayout->addMultiCellWidget( backgroundSlider, 0, 2, 0, 0 );

    solidBackgroundColorChooser = new lCColorChooser( backgroundGroupBox, "solidBackgroundColorChooser" );
    QToolTip::add( solidBackgroundColorChooser, trUtf8( "Choose a solid background color." ) );
    QWhatsThis::add( solidBackgroundColorChooser, trUtf8( "<p><b>Solid Background Color</b></p>\n"
"<p>This is the color used for a solid background. A solid background will give you the highest performance.</p>\n"
"<p>If you modify the color, you can always go back to the default value by clicking the <img src=\"default_active.png\"> button</p>" ) );

    backgroundGroupBoxLayout->addWidget( solidBackgroundColorChooser, 0, 2 );

    gradientBackgroundColorChooser = new lCColorChooser( backgroundGroupBox, "gradientBackgroundColorChooser" );
    QToolTip::add( gradientBackgroundColorChooser, trUtf8( "Choose a gradient background color." ) );
    QWhatsThis::add( gradientBackgroundColorChooser, trUtf8( "<p><b>Gradient Background Color</b></p>\n"
"<p>If you choose the gradient background style, then the background will be shaded from the top of the screen to bottom. The solid background color will be used at the top. The background will smoothly shade to the gradient color at the bottom of the screen. (This background style does not use the high-performance OpenGL screen clearing routine; therefore, there is some loss of performance.)</p>\n"
"<p>If you modify the color, you can always go back to the default value by clicking the <img src=\"default_active.png\"> button</p>" ) );

    backgroundGroupBoxLayout->addWidget( gradientBackgroundColorChooser, 1, 2 );

    patternBackgroundFileChooser = new lCDefaultFileChooser( backgroundGroupBox, "patternBackgroundFileChooser" );
    QToolTip::add( patternBackgroundFileChooser, trUtf8( "Choose a patterned background color." ) );
    QWhatsThis::add( patternBackgroundFileChooser, trUtf8( "<p><b>Pattern Background Image File</b></p>\n"
"<p>If you choose the pattern background style, then the background will be drawn as a tiled image filling the window. The tile image is computed by taking the input image, converting it to grayscale and then using the solid and gradient colors as end-points in the OpenGL pixel transfer functions. In other words, where pixels in the grayscale version of the input image are zero, the solid color will be used. Where they are 255, the gradient color will be used. Pixels with intermediate values will be some linearly interpolated color between the solid and gradient colors.</p>\n"
"<p>This background style does not use the high-performance OpenGL screen clearing routine. Also, it is probably not practical without hardware OpenGL acceleration of texture mapping.</p>\n"
"<p>If you modify the color, you can always go back to the default value by clicking the <img src=\"default_active.png\"> button</p>" ) );

    backgroundGroupBoxLayout->addWidget( patternBackgroundFileChooser, 2, 2 );

    solidBackgroundLabel = new QLabel( backgroundGroupBox, "solidBackgroundLabel" );
    solidBackgroundLabel->setText( trUtf8( "&Solid" ) );

    backgroundGroupBoxLayout->addWidget( solidBackgroundLabel, 0, 1 );

    gradientBackgoundLabel = new QLabel( backgroundGroupBox, "gradientBackgoundLabel" );
    gradientBackgoundLabel->setText( trUtf8( "G&radient" ) );

    backgroundGroupBoxLayout->addWidget( gradientBackgoundLabel, 1, 1 );

    patternBackgroundLabel = new QLabel( backgroundGroupBox, "patternBackgroundLabel" );
    patternBackgroundLabel->setText( trUtf8( "P&attern" ) );

    backgroundGroupBoxLayout->addWidget( patternBackgroundLabel, 2, 1 );
    Layout5->addWidget( backgroundGroupBox );

    colorSchemePageLayout->addLayout( Layout5, 1, 2 );

    Line5 = new QFrame( colorSchemePage, "Line5" );
    Line5->setFrameShape( QFrame::VLine );
    Line5->setFrameShadow( QFrame::Sunken );
    Line5->setFrameShape( QFrame::VLine );

    colorSchemePageLayout->addMultiCellWidget( Line5, 0, 1, 1, 1 );

    customRadioButton = new QRadioButton( colorSchemePage, "customRadioButton" );
    customRadioButton->setText( trUtf8( "C&ustom" ) );
    QToolTip::add( customRadioButton, trUtf8( "Use a custom color scheme." ) );
    QWhatsThis::add( customRadioButton, trUtf8( "<p><b>Custom Color Scheme</b></p>\n"
"<p>If you don't like the predefined color schemes, you can pick the <b>Custom</b> option and set all the values explicitly. Any choice in the <b>Predefined scheme</b> list will serve as the defaults for the custom scheme widget choices.</p>" ) );

    colorSchemePageLayout->addWidget( customRadioButton, 0, 2 );
    preferencesTabWidget->insertTab( colorSchemePage, trUtf8( "Color Scheme" ) );

    otherStylesPage = new QWidget( preferencesTabWidget, "otherStylesPage" );
    otherStylesPageLayout = new QGridLayout( otherStylesPage, 1, 1, 11, 6, "otherStylesPageLayout"); 

    dimensionGroupBox = new QGroupBox( otherStylesPage, "dimensionGroupBox" );
    dimensionGroupBox->setTitle( trUtf8( "Dimension Properties" ) );
    dimensionGroupBox->setColumnLayout(0, Qt::Vertical );
    dimensionGroupBox->layout()->setSpacing( 6 );
    dimensionGroupBox->layout()->setMargin( 11 );
    dimensionGroupBoxLayout = new QGridLayout( dimensionGroupBox->layout() );
    dimensionGroupBoxLayout->setAlignment( Qt::AlignTop );

    fontLabel = new QLabel( dimensionGroupBox, "fontLabel" );
    fontLabel->setText( trUtf8( "&Font:" ) );

    dimensionGroupBoxLayout->addWidget( fontLabel, 0, 0 );

    arrowHeadLengthLabel = new QLabel( dimensionGroupBox, "arrowHeadLengthLabel" );
    arrowHeadLengthLabel->setText( trUtf8( "Arrow head &length:" ) );

    dimensionGroupBoxLayout->addWidget( arrowHeadLengthLabel, 1, 0 );

    arrowHeadWidthLabel = new QLabel( dimensionGroupBox, "arrowHeadWidthLabel" );
    arrowHeadWidthLabel->setText( trUtf8( "Arrow head &width\n"
"to length ratio:" ) );

    dimensionGroupBoxLayout->addWidget( arrowHeadWidthLabel, 2, 0 );

    arrowHeadStyleLabel = new QLabel( dimensionGroupBox, "arrowHeadStyleLabel" );
    arrowHeadStyleLabel->setText( trUtf8( "Arrow head &style" ) );

    dimensionGroupBoxLayout->addWidget( arrowHeadStyleLabel, 3, 0 );

    clearanceLabel = new QLabel( dimensionGroupBox, "clearanceLabel" );
    clearanceLabel->setText( trUtf8( "&Dimension value\n"
"clearance:" ) );
    QToolTip::add( clearanceLabel, trUtf8( "This is the extra space above and below the area occupied by the dimension value text." ) );

    dimensionGroupBoxLayout->addMultiCellWidget( clearanceLabel, 4, 5, 0, 0 );

    extensionOffsetLabel = new QLabel( dimensionGroupBox, "extensionOffsetLabel" );
    extensionOffsetLabel->setText( trUtf8( "E&xtension line offset:" ) );

    dimensionGroupBoxLayout->addWidget( extensionOffsetLabel, 7, 0 );

    arrowHeadStyleComboBox = new lCDefaultComboBox( dimensionGroupBox, "arrowHeadStyleComboBox" );
    QToolTip::add( arrowHeadStyleComboBox, trUtf8( "Choose the arrow head style." ) );
    QWhatsThis::add( arrowHeadStyleComboBox, trUtf8( "<p><b>Dimension Arrow Head Style</b></p>\n"
"<p>Select the style of the arrow head. The current styles include solid, hollow and open.</p>\n"
"<p>If you modify the arrow head style, you can always go back to the default value by clicking the <img src=\"default_active.png\"> button</p>" ) );

    dimensionGroupBoxLayout->addWidget( arrowHeadStyleComboBox, 3, 1 );

    lineThicknessLabel = new QLabel( dimensionGroupBox, "lineThicknessLabel" );
    lineThicknessLabel->setText( trUtf8( "Line &thickness:" ) );
    QToolTip::add( lineThicknessLabel, trUtf8( "Line thickness is used during printing. It is not normally used for screen rendering." ) );

    dimensionGroupBoxLayout->addWidget( lineThicknessLabel, 6, 0 );

    dimensionFontChooser = new lCFontChooser( dimensionGroupBox, "dimensionFontChooser" );
    dimensionFontChooser->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)4, (QSizePolicy::SizeType)0, 0, 0, dimensionFontChooser->sizePolicy().hasHeightForWidth() ) );
    QToolTip::add( dimensionFontChooser, trUtf8( "Choose a font (face, size and style) for dimension values." ) );
    QWhatsThis::add( dimensionFontChooser, trUtf8( "<p><b>Dimension Font</b></p>\n"
"<p>Select the font used to display the value of dimensions. The font specification includes the face, the size and any additional styles (such as bold, italic or underlined).</p>\n"
"<h3><font color=\"red\">WARNING WARNING WARNING</font></h3>\n"
"<p> In the current version of <i>lignumCAD</i>, you <b><i>must</i></b> pick a Type 1 or TrueType font. Unfortunately, there is no way to tell from the Qt font requestor which type of font you are selecting. Hopefully both of these limitations will be corrected in a future version.</p>\n"
"<p>If you modify the font, you can always go back to the default value by clicking the <img src=\"default_active.png\"> button</p>" ) );

    dimensionGroupBoxLayout->addWidget( dimensionFontChooser, 0, 1 );

    arrowHeadLengthSpinBox = new lCDefaultLengthSpinBox( dimensionGroupBox, "arrowHeadLengthSpinBox" );
    QToolTip::add( arrowHeadLengthSpinBox, trUtf8( "Choose dimension arrow head length." ) );
    QWhatsThis::add( arrowHeadLengthSpinBox, trUtf8( "<p><b>Dimension Arrow Head Length</b></p>\n"
"<p>Set the length of the arrow heads which are used to display the dimension's extents. Note that the length specified here is in <i>paper</i> units. That is, arrow heads are always the same size regardless of the scale of the view.</p>\n"
"<p>If you modify the arrow head length, you can always go back to the default value by clicking the <img src=\"default_active.png\"> button</p>" ) );

    dimensionGroupBoxLayout->addWidget( arrowHeadLengthSpinBox, 1, 1 );

    arrowHeadWidthRatioSpinBox = new lCDefaultRatioSpinBox( dimensionGroupBox, "arrowHeadWidthRatioSpinBox" );
    QToolTip::add( arrowHeadWidthRatioSpinBox, trUtf8( "Choose the dimension arrow head width to length ratio." ) );
    QWhatsThis::add( arrowHeadWidthRatioSpinBox, trUtf8( "<p><b>Dimension Arrow Head Width to Length Ratio</b></p>\n"
"<p>The width of dimension arrow heads are given in terms of a ratio to the length of the arrow head. So, if the width to length ratio is 1:4, a one inch long arrow head will be a quarter of an inch wide.</p>\n"
"<p>If you modify the arrow head width to length ratio, you can always go back to the default value by clicking the <img src=\"default_active.png\"> button</p>" ) );

    dimensionGroupBoxLayout->addWidget( arrowHeadWidthRatioSpinBox, 2, 1 );

    clearanceLengthSpinBox = new lCDefaultLengthSpinBox( dimensionGroupBox, "clearanceLengthSpinBox" );
    QToolTip::add( clearanceLengthSpinBox, trUtf8( "Set a dimension value clearance length." ) );
    QWhatsThis::add( clearanceLengthSpinBox, trUtf8( "<p><b>Dimension Value Clearance Length</b></p>\n"
"<p>Dimensions are usually drawn as two arrows pointing to extension lines. The length of the extension lines is somewhat arbitrary. In <i>lignumCAD</i>, the minimum length of an extension line is equal to the height of the dimension value text plus a little bit more for aesthetic purposes. That little bit more is the clearance length.\n"
"Note that the clearance length specified here is in <i>paper</i> units. That is, the clearance length is always the same size regardless of the scale of the view.</p>\n"
"<p>If you modify the clearance length, you can always go back to the default value by clicking the <img src=\"default_active.png\"> button</p>" ) );

    dimensionGroupBoxLayout->addWidget( clearanceLengthSpinBox, 4, 1 );

    lineThicknessLengthSpinBox = new lCDefaultLengthSpinBox( dimensionGroupBox, "lineThicknessLengthSpinBox" );
    QToolTip::add( lineThicknessLengthSpinBox, trUtf8( "Choose a line thickness for printing dimension lines." ) );
    QWhatsThis::add( lineThicknessLengthSpinBox, trUtf8( "<p><b>Dimension Line Thickness</b></p>\n"
"<p>In technical drawing, dimension lines are usually drawn with a narrower line than geometry edges. Unfortunately, computer monitors haven't quite reached the high resolutions necessary to emulate this (so, <i>lignumCAD</i> uses color to distinguish between geometry and annotations). However, most modern printers are quite capable of drawing lines of distinguishable thicknesses. Therefore, this option will let you set the thickness of dimension lines when they are printed.</p>\n"
"<p>If you modify the arrow head width to length ratio, you can always go back to the default value by clicking the <img src=\"default_active.png\"> button</p>" ) );

    dimensionGroupBoxLayout->addMultiCellWidget( lineThicknessLengthSpinBox, 5, 6, 1, 1 );

    extensionOffsetLengthSpinBox = new lCDefaultLengthSpinBox( dimensionGroupBox, "extensionOffsetLengthSpinBox" );
    QToolTip::add( extensionOffsetLengthSpinBox, trUtf8( "Set the dimension extension line offset." ) );
    QWhatsThis::add( extensionOffsetLengthSpinBox, trUtf8( "<p><b>Dimension Extension Line Offset</b></p>\n"
"<p>Dimension extension lines are usually drawn with a small gap between the point in space which they reference and the end of the extension line. You can set the size of this offset here. Note that the offset specified here is in <i>paper</i> units. That is, the offset is always the same size regardless of the scale of the view.</p>\n"
"<p>If you modify the extension line offset, you can always go back to the default value by clicking the <img src=\"default_active.png\"> button</p>" ) );

    dimensionGroupBoxLayout->addWidget( extensionOffsetLengthSpinBox, 7, 1 );

    otherStylesPageLayout->addMultiCellWidget( dimensionGroupBox, 0, 1, 0, 0 );

    Layout9 = new QVBoxLayout( 0, 0, 6, "Layout9"); 

    annotationGroupBox = new QGroupBox( otherStylesPage, "annotationGroupBox" );
    annotationGroupBox->setTitle( trUtf8( "Annotation Properties" ) );
    annotationGroupBox->setColumnLayout(0, Qt::Vertical );
    annotationGroupBox->layout()->setSpacing( 6 );
    annotationGroupBox->layout()->setMargin( 11 );
    annotationGroupBoxLayout = new QGridLayout( annotationGroupBox->layout() );
    annotationGroupBoxLayout->setAlignment( Qt::AlignTop );

    annotationFontChooser = new lCFontChooser( annotationGroupBox, "annotationFontChooser" );
    QToolTip::add( annotationFontChooser, trUtf8( "Choose a font (face, size and style) for annotation text." ) );
    QWhatsThis::add( annotationFontChooser, trUtf8( "<p><b>Annotation Font</b></p>\n"
"<p>Select the default font used to display text annotations. The font specification includes the face, the size and any additional styles (such as bold, italic or underlined). You also have some additional control over the size and style of the font when you create the annotation.</p>\n"
"<h3><font color=\"red\">WARNING WARNING WARNING</font></h3>\n"
"<p> In the current version of <i>lignumCAD</i>, you <b><i>must</i></b> pick a Type 1 or TrueType font. Unfortunately, there is no way to tell from the Qt font requestor which type of font you are selecting. Hopefully both of these limitations will be corrected in a future version.</p>\n"
"<p>If you modify the font, you can always go back to the default value by clicking the <img src=\"default_active.png\"> button</p>" ) );

    annotationGroupBoxLayout->addWidget( annotationFontChooser, 0, 1 );

    annotationFontLabel = new QLabel( annotationGroupBox, "annotationFontLabel" );
    annotationFontLabel->setText( trUtf8( "Fo&nt:" ) );

    annotationGroupBoxLayout->addWidget( annotationFontLabel, 0, 0 );
    Layout9->addWidget( annotationGroupBox );

    handleGroupBox = new QGroupBox( otherStylesPage, "handleGroupBox" );
    handleGroupBox->setTitle( trUtf8( "Resize Handle Properties" ) );
    handleGroupBox->setColumnLayout(0, Qt::Vertical );
    handleGroupBox->layout()->setSpacing( 6 );
    handleGroupBox->layout()->setMargin( 11 );
    handleGroupBoxLayout = new QHBoxLayout( handleGroupBox->layout() );
    handleGroupBoxLayout->setAlignment( Qt::AlignTop );

    handleSizeLabel = new QLabel( handleGroupBox, "handleSizeLabel" );
    handleSizeLabel->setText( trUtf8( "&Resize handle size:" ) );
    handleGroupBoxLayout->addWidget( handleSizeLabel );

    handleLengthSpinBox = new lCDefaultLengthSpinBox( handleGroupBox, "handleLengthSpinBox" );
    QToolTip::add( handleLengthSpinBox, trUtf8( "Choose the size of the resize handles." ) );
    QWhatsThis::add( handleLengthSpinBox, trUtf8( "<p><b>Resize Handle Size</b></p>\n"
"<p>Set the edge length (size) of resize handles. Note that the size specified here is in <i>paper</i> units. That is, resize handles are always the same size regardless of the scale of the view.</p>\n"
"<p>If you modify the resize handle size, you can always go back to the default value by clicking the <img src=\"default_active.png\"> button</p>" ) );
    handleGroupBoxLayout->addWidget( handleLengthSpinBox );
    Layout9->addWidget( handleGroupBox );

    otherStylesPageLayout->addLayout( Layout9, 0, 1 );
    QSpacerItem* spacer_7 = new QSpacerItem( 0, 20, QSizePolicy::Minimum, QSizePolicy::Expanding );
    otherStylesPageLayout->addItem( spacer_7, 1, 1 );
    QSpacerItem* spacer_8 = new QSpacerItem( 20, 0, QSizePolicy::Expanding, QSizePolicy::Minimum );
    otherStylesPageLayout->addItem( spacer_8, 0, 2 );
    preferencesTabWidget->insertTab( otherStylesPage, trUtf8( "Other Styles" ) );

    PreferencesDialogLayout->addWidget( preferencesTabWidget, 0, 0 );

    // signals and slots connections
    connect( buttonOk, SIGNAL( clicked() ), this, SLOT( accept() ) );
    connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
    connect( logoFileChooser, SIGNAL( fileNameChanged(const QString&) ), this, SLOT( logoFileChooser_fileNameChanged(const QString&) ) );
    connect( unitsListBox, SIGNAL( currentChanged(QListBoxItem*) ), this, SLOT( unitsListBox_currentChanged(QListBoxItem*) ) );
    connect( formatButtonGroup, SIGNAL( clicked(int) ), this, SLOT( formatButtonGroup_clicked(int) ) );
    connect( precisionComboBox, SIGNAL( valueChanged(int) ), this, SLOT( precisionComboBox_valueChanged(int) ) );
    connect( colorSchemeListBox, SIGNAL( currentChanged(QListBoxItem*) ), this, SLOT( colorSchemeListBox_currentChanged(QListBoxItem*) ) );
    connect( geometryColorChooser, SIGNAL( colorChanged(const QColor&) ), this, SLOT( geometryColorChooser_colorChanged(const QColor&) ) );
    connect( annotationColorChooser, SIGNAL( colorChanged(const QColor&) ), this, SLOT( annotationColorChooser_colorChanged(const QColor&) ) );
    connect( gridColorChooser, SIGNAL( colorChanged(const QColor&) ), this, SLOT( gridColorChooser_colorChanged(const QColor&) ) );
    connect( constraintPrimaryColorChooser, SIGNAL( colorChanged(const QColor&) ), this, SLOT( constraintPrimaryColorChooser_colorChanged(const QColor&) ) );
    connect( constraintSecondaryColorChooser, SIGNAL( colorChanged(const QColor&) ), this, SLOT( constraintSecondaryColorChooser_colorChanged(const QColor&) ) );
    connect( backgroundSlider, SIGNAL( valueChanged(int) ), this, SLOT( backgroundSlider_valueChanged(int) ) );
    connect( solidBackgroundColorChooser, SIGNAL( colorChanged(const QColor&) ), this, SLOT( solidBackgroundColorChooser_colorChanged(const QColor&) ) );
    connect( gradientBackgroundColorChooser, SIGNAL( colorChanged(const QColor&) ), this, SLOT( gradientBackgroundColorChooser_colorChanged(const QColor&) ) );
    connect( patternBackgroundFileChooser, SIGNAL( fileNameChanged(const QString&) ), this, SLOT( patternBackgroundFileChooser_fileNameChanged(const QString&) ) );
    connect( dimensionFontChooser, SIGNAL( fontChanged(const QString&) ), this, SLOT( dimensionFontChooser_fontChanged(const QString&) ) );
    connect( arrowHeadLengthSpinBox, SIGNAL( valueChanged(double) ), this, SLOT( arrowHeadLengthSpinBox_valueChanged(double) ) );
    connect( arrowHeadStyleComboBox, SIGNAL( valueChanged(int) ), this, SLOT( arrowHeadStyleComboBox_valueChanged(int) ) );
    connect( clearanceLengthSpinBox, SIGNAL( valueChanged(double) ), this, SLOT( clearanceLengthSpinBox_valueChanged(double) ) );
    connect( lineThicknessLengthSpinBox, SIGNAL( valueChanged(double) ), this, SLOT( lineThicknessLengthSpinBox_valueChanged(double) ) );
    connect( extensionOffsetLengthSpinBox, SIGNAL( valueChanged(double) ), this, SLOT( extensionOffsetLengthSpinBox_valueChanged(double) ) );
    connect( annotationFontChooser, SIGNAL( fontChanged(const QString&) ), this, SLOT( annotationFontChooser_fontChanged(const QString&) ) );
    connect( handleLengthSpinBox, SIGNAL( valueChanged(double) ), this, SLOT( handleLengthSpinBox_valueChanged(double) ) );
    connect( arrowHeadWidthRatioSpinBox, SIGNAL( valueChanged(const Ratio&) ), this, SLOT( arrowHeadWidthRatioSpinBox_valueChanged(const Ratio&) ) );
    connect( buttonHelp, SIGNAL( clicked() ), this, SLOT( buttonHelp_clicked() ) );

    // tab order
    setTabOrder( preferencesTabWidget, businessNameLineEdit );
    setTabOrder( businessNameLineEdit, businessLocationLineEdit );
    setTabOrder( businessLocationLineEdit, logoFileChooser );
    setTabOrder( logoFileChooser, unitsListBox );
    setTabOrder( unitsListBox, fractionalRadioButton );
    setTabOrder( fractionalRadioButton, decimalRadioButton );
    setTabOrder( decimalRadioButton, precisionComboBox );
    setTabOrder( precisionComboBox, predefinedRadioButton );
    setTabOrder( predefinedRadioButton, colorSchemeListBox );
    setTabOrder( colorSchemeListBox, customRadioButton );
    setTabOrder( customRadioButton, geometryColorChooser );
    setTabOrder( geometryColorChooser, annotationColorChooser );
    setTabOrder( annotationColorChooser, gridColorChooser );
    setTabOrder( gridColorChooser, constraintPrimaryColorChooser );
    setTabOrder( constraintPrimaryColorChooser, constraintSecondaryColorChooser );
    setTabOrder( constraintSecondaryColorChooser, backgroundSlider );
    setTabOrder( backgroundSlider, solidBackgroundColorChooser );
    setTabOrder( solidBackgroundColorChooser, gradientBackgroundColorChooser );
    setTabOrder( gradientBackgroundColorChooser, patternBackgroundFileChooser );
    setTabOrder( patternBackgroundFileChooser, dimensionFontChooser );
    setTabOrder( dimensionFontChooser, arrowHeadLengthSpinBox );
    setTabOrder( arrowHeadLengthSpinBox, arrowHeadWidthRatioSpinBox );
    setTabOrder( arrowHeadWidthRatioSpinBox, arrowHeadStyleComboBox );
    setTabOrder( arrowHeadStyleComboBox, clearanceLengthSpinBox );
    setTabOrder( clearanceLengthSpinBox, lineThicknessLengthSpinBox );
    setTabOrder( lineThicknessLengthSpinBox, extensionOffsetLengthSpinBox );
    setTabOrder( extensionOffsetLengthSpinBox, annotationFontChooser );
    setTabOrder( annotationFontChooser, handleLengthSpinBox );
    setTabOrder( handleLengthSpinBox, buttonHelp );
    setTabOrder( buttonHelp, buttonOk );
    setTabOrder( buttonOk, buttonCancel );

    // buddies
    businessLocationLabel->setBuddy( businessLocationLineEdit );
    businessNameLabel->setBuddy( businessNameLineEdit );
    precisionLabel->setBuddy( precisionComboBox );
    geometryColorLabel->setBuddy( geometryColorChooser );
    annotationColorLabel->setBuddy( annotationColorChooser );
    gridColorLabel->setBuddy( gridColorChooser );
    constrainSecondaryColorLabel->setBuddy( constraintSecondaryColorChooser );
    constraintPrimaryColorLabel->setBuddy( constraintPrimaryColorChooser );
    solidBackgroundLabel->setBuddy( solidBackgroundColorChooser );
    gradientBackgoundLabel->setBuddy( gradientBackgroundColorChooser );
    patternBackgroundLabel->setBuddy( patternBackgroundFileChooser );
    fontLabel->setBuddy( dimensionFontChooser );
    arrowHeadLengthLabel->setBuddy( arrowHeadLengthSpinBox );
    arrowHeadWidthLabel->setBuddy( arrowHeadWidthRatioSpinBox );
    arrowHeadStyleLabel->setBuddy( arrowHeadStyleComboBox );
    clearanceLabel->setBuddy( clearanceLengthSpinBox );
    extensionOffsetLabel->setBuddy( extensionOffsetLengthSpinBox );
    lineThicknessLabel->setBuddy( lineThicknessLengthSpinBox );
    annotationFontLabel->setBuddy( annotationFontChooser );
    handleSizeLabel->setBuddy( handleLengthSpinBox );
    init();
}

/*  
 *  Destroys the object and frees any allocated resources
 */
PreferencesDialog::~PreferencesDialog()
{
    // no need to delete child widgets, Qt does it all for us
}

