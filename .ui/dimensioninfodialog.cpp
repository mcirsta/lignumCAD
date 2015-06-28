/****************************************************************************
** Form implementation generated from reading ui file 'dimensioninfodialog.ui'
**
** Created: Wed Dec 18 08:54:55 2002
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#include "dimensioninfodialog.h"

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
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qimage.h>
#include <qpixmap.h>

#include "dimensioninfodialog.ui.h"
static const char* const image0_data[] = { 
"22 22 131 2",
"Qt c None",
"#Q c #392810",
"#r c #413010",
"#x c #413018",
"#7 c #4a3818",
"#T c #523818",
"#v c #523c18",
"a. c #5a3c18",
"#M c #5a3c20",
"#D c #5a4020",
"#e c #6a4c20",
".M c #6a716a",
".4 c #734c20",
"#p c #735020",
"#5 c #735029",
".x c #73716a",
".N c #73756a",
"#k c #737573",
"#f c #737973",
"#q c #7b7162",
"#P c #7b7573",
"#C c #7b757b",
".y c #7b7973",
"#u c #7b797b",
"#w c #7b7d73",
".j c #7b7d7b",
"#0 c #83797b",
"#L c #837d7b",
"#B c #837d83",
"#a c #83817b",
"#i c #838183",
".z c #83857b",
".W c #838583",
"#H c #8b7d6a",
"#o c #8b8583",
"#Z c #8b858b",
".k c #8b8983",
"#d c #8b898b",
"#l c #8b8d83",
"## c #8b8d8b",
"#F c #8b918b",
"#s c #948573",
"#A c #948d8b",
".X c #94918b",
".A c #949194",
".c c #94958b",
"#h c #949594",
".l c #9c9994",
"#m c #9c999c",
"#N c #9c9d94",
".B c #9c9d9c",
"#c c #9ca19c",
"#y c #a47131",
".Y c #a49d9c",
".5 c #a4a19c",
".d c #a4a1a4",
".m c #a4a5a4",
"#z c #a4aaa4",
".O c #aca5a4",
"#n c #acaaa4",
".Z c #acaaac",
"#G c #acaea4",
".e c #acaeac",
".P c #acb2ac",
".C c #b4aeac",
".6 c #b4b2ac",
".n c #b4b2b4",
".D c #b4b6b4",
".o c #b4bab4",
".0 c #bdb6b4",
".f c #bdbab4",
".Q c #bdbabd",
"#W c #bdbeb4",
".g c #bdbebd",
"#3 c #bdc2bd",
"#j c #c58d39",
"#1 c #c58d41",
".E c #c5bebd",
"#. c #c5c2bd",
".R c #c5c2c5",
".p c #c5c6c5",
"#9 c #cd9139",
".U c #cd9141",
"#4 c #cd9541",
"#O c #cdc6c5",
"#J c #cdc6cd",
"#b c #cdcac5",
".T c #cdcacd",
"#Y c #cdcad5",
".F c #cdcecd",
"#V c #cdced5",
"#6 c #d5954a",
".q c #d5cecd",
".J c #d5ced5",
".7 c #d5d2cd",
".h c #d5d2d5",
".1 c #d5d6d5",
"a# c #de9d41",
"aa c #de9d4a",
"#E c #dea14a",
".G c #ded6d5",
"#2 c #ded6de",
".r c #dedad5",
".S c #dedade",
".H c #dedede",
".K c #e6a141",
".i c #e6a14a",
".t c #e6a54a",
"#R c #e6a552",
".2 c #e6dede",
".8 c #e6e2de",
"#g c #e6e2e6",
".s c #e6e6e6",
"#S c #eee6e6",
"#t c #eee6ee",
".I c #eeeaee",
".9 c #eeeeee",
".u c #f6b24a",
"#8 c #f6b252",
"#U c #f6b652",
"#I c #f6ca94",
"#K c #f6eeee",
".3 c #f6eef6",
".V c #ffb24a",
"#X c #ffb252",
".# c #ffb64a",
".b c #ffb652",
".L c #ffb65a",
".a c #ffba52",
".w c #ffba5a",
".v c #ffbe5a",
"Qt.#.a.aQt.b.a.c.d.e.f.g.h.i.b.aQt.#Qt.b.a.a",
".b.a.b.a.b.j.k.l.m.n.o.p.q.r.s.t.u.b.a.b.b.v",
".a.#.w.x.y.z.A.B.C.D.E.F.G.H.I.I.J.K.L.#.a.a",
".b.#.M.N.y.k.c.O.P.Q.R.h.r.s.I.S.T.R.U.V.#Qt",
".a.b.N.N.W.X.Y.Z.0.g.F.1.2.I.3.F.T.g.4.KQtQt",
".b.N.M.y.k.l.5.6.D.R.7.S.8.9.H.J#..6##.U.VQt",
".a.x.y#a.X.l.e.D.E#b.G.S.I.I.h.T.g#c#d#e.t.a",
".M.N#f.k.c.5.e.Q.g.h.1#g.I.s.T.p.P#h.W#i#j.L",
"#k.N#a#l#m#n.D.Q.T.7.S.s.3.1.T.g.O#d#o.j#p.t",
".x.y.z#h#c.C.D.E.F#q#r#s#t.T.R.D#h#o.j#u#v.i",
"#k#w##.l#n.6.g.p.h#x#y.U.S.T.R#z#A#i#B#C#D#E",
".N.W#F.5#G.D.g.F.1#H#j#I.T#J.o#m.W#i#u#C#v#E",
".j.k.l.m.n.f.p.h.2.s#K.S.J#..e###o#L#u#C#M#E",
".K.X#N.e.D.g#b.1.r.I.I.J#O.g.B#o.j#L#P#Q#e#R",
".b#h.O.6.Q#..h.r#S.I#g.F.p.P.A.W#i#u#C#T.U#U",
".#.i#n.D.f.T.7.H.s.I#V.T#W.5#o#i#u#C#Q#e.iQt",
".a#X.6.f.R.h.S.8.3.s#Y.R.D.A#Z#a#0#C#v#1.bQt",
".b.a.K.g.p.h.r#S.I#2.T#3.d#d#i#L#P#Q#e.t.#.v",
"Qt.#.b#4#5.1#g.I.I.T#J.D#m.W#i#Q#v#e#6.V.a.a",
".b.a.b.b.K.U#e.9.1.T.g#n#d#Q#7.4#j.K#8.b.#.v",
"Qt.b.w.a.w#X.t#9#p#va.#v#D#e.U.t#X.#.w.#.aQt",
".b.b.a.a.a.a.b.b.K#Ea##Eaa.t#8.a.#.b.a.b.#Qt"};


/* 
 *  Constructs a DimensionInfoDialog which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
DimensionInfoDialog::DimensionInfoDialog( QWidget* parent,  const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    QPixmap image0( ( const char** ) image0_data );
    if ( !name )
	setName( "DimensionInfoDialog" );
    resize( 333, 389 ); 
    setCaption( trUtf8( "Dimension Information" ) );
    setIcon( image0 );
    DimensionInfoDialogLayout = new QGridLayout( this, 1, 1, 11, 6, "DimensionInfoDialogLayout"); 

    nameLabel = new QLabel( this, "nameLabel" );
    nameLabel->setFrameShape( QLabel::MShape );
    nameLabel->setFrameShadow( QLabel::MShadow );
    nameLabel->setText( trUtf8( "&Name:" ) );

    DimensionInfoDialogLayout->addWidget( nameLabel, 0, 0 );

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

    DimensionInfoDialogLayout->addMultiCellLayout( Layout1, 3, 3, 0, 2 );

    GroupBox1 = new QGroupBox( this, "GroupBox1" );
    GroupBox1->setTitle( trUtf8( "Annotation Side" ) );
    QToolTip::add( GroupBox1, trUtf8( "Select the annotation side." ) );
    GroupBox1->setColumnLayout(0, Qt::Vertical );
    GroupBox1->layout()->setSpacing( 6 );
    GroupBox1->layout()->setMargin( 11 );
    GroupBox1Layout = new QGridLayout( GroupBox1->layout() );
    GroupBox1Layout->setAlignment( Qt::AlignTop );

    Layout2 = new QGridLayout( 0, 1, 1, 0, 6, "Layout2"); 

    belowButton = new QRadioButton( GroupBox1, "belowButton" );
    belowButton->setText( trUtf8( "Below" ) );
    QToolTip::add( belowButton, trUtf8( "Dimension will be drawn below references." ) );

    Layout2->addWidget( belowButton, 2, 1 );

    rightButton = new QRadioButton( GroupBox1, "rightButton" );
    rightButton->setText( trUtf8( "Right" ) );
    QToolTip::add( rightButton, trUtf8( "Dimension will be drawn to the right of the references." ) );

    Layout2->addWidget( rightButton, 1, 2 );

    leftButton = new QRadioButton( GroupBox1, "leftButton" );
    leftButton->setText( trUtf8( "Left" ) );
    QToolTip::add( leftButton, trUtf8( "Dimension will be drawn to the left of the references." ) );

    Layout2->addWidget( leftButton, 1, 0 );

    aboveButton = new QRadioButton( GroupBox1, "aboveButton" );
    aboveButton->setText( trUtf8( "Above" ) );
    QToolTip::add( aboveButton, trUtf8( "Dimension will be drawn above references." ) );

    Layout2->addWidget( aboveButton, 0, 1 );

    GroupBox1Layout->addLayout( Layout2, 0, 0 );

    DimensionInfoDialogLayout->addMultiCellWidget( GroupBox1, 2, 2, 0, 1 );

    orientationGroup = new QButtonGroup( this, "orientationGroup" );
    orientationGroup->setEnabled( TRUE );
    orientationGroup->setTitle( trUtf8( "Orientation" ) );
    QToolTip::add( orientationGroup, trUtf8( "Orientation of dimension line." ) );
    orientationGroup->setColumnLayout(0, Qt::Vertical );
    orientationGroup->layout()->setSpacing( 6 );
    orientationGroup->layout()->setMargin( 11 );
    orientationGroupLayout = new QGridLayout( orientationGroup->layout() );
    orientationGroupLayout->setAlignment( Qt::AlignTop );

    horizontalButton = new QRadioButton( orientationGroup, "horizontalButton" );
    horizontalButton->setEnabled( FALSE );
    horizontalButton->setText( trUtf8( "Horizontal" ) );

    orientationGroupLayout->addWidget( horizontalButton, 0, 0 );

    verticalButton = new QRadioButton( orientationGroup, "verticalButton" );
    verticalButton->setEnabled( FALSE );
    verticalButton->setText( trUtf8( "Vertical" ) );

    orientationGroupLayout->addWidget( verticalButton, 1, 0 );

    otherButton = new QRadioButton( orientationGroup, "otherButton" );
    otherButton->setEnabled( FALSE );
    otherButton->setText( trUtf8( "Other" ) );

    orientationGroupLayout->addWidget( otherButton, 2, 0 );

    DimensionInfoDialogLayout->addWidget( orientationGroup, 2, 2 );

    referencesGroupBox = new QGroupBox( this, "referencesGroupBox" );
    referencesGroupBox->setTitle( trUtf8( "References" ) );
    referencesGroupBox->setColumnLayout(0, Qt::Vertical );
    referencesGroupBox->layout()->setSpacing( 6 );
    referencesGroupBox->layout()->setMargin( 11 );
    referencesGroupBoxLayout = new QGridLayout( referencesGroupBox->layout() );
    referencesGroupBoxLayout->setAlignment( Qt::AlignTop );

    toReferenceLabel = new QLabel( referencesGroupBox, "toReferenceLabel" );
    toReferenceLabel->setText( trUtf8( "To:" ) );

    referencesGroupBoxLayout->addWidget( toReferenceLabel, 0, 0 );

    toReferenceLineEdit = new QLineEdit( referencesGroupBox, "toReferenceLineEdit" );
    QToolTip::add( toReferenceLineEdit, trUtf8( "Primary dimension reference." ) );
    QWhatsThis::add( toReferenceLineEdit, trUtf8( "<p><b>To Reference</b></p>\n"
"<p>This widget shows the geometry item which is the primary reference for the dimension. This value is not editable in this dialog; you must use the regular interface to adjust constraint references.</p>" ) );

    referencesGroupBoxLayout->addWidget( toReferenceLineEdit, 0, 1 );

    fromReferenceLabel = new QLabel( referencesGroupBox, "fromReferenceLabel" );
    fromReferenceLabel->setText( trUtf8( "From:" ) );

    referencesGroupBoxLayout->addWidget( fromReferenceLabel, 1, 0 );

    fromReferenceLineEdit = new QLineEdit( referencesGroupBox, "fromReferenceLineEdit" );
    QToolTip::add( fromReferenceLineEdit, trUtf8( "Secondary reference." ) );
    QWhatsThis::add( fromReferenceLineEdit, trUtf8( "<p><b>From Reference</b></p>\n"
"<p>This widget shows the geometry item which the primary reference is offset from (i.e., the secondary reference). This value is not editable in this dialog; you must use the regular interface to adjust constraint references.</p>" ) );

    referencesGroupBoxLayout->addWidget( fromReferenceLineEdit, 1, 1 );

    offsetButtonGroup = new QButtonGroup( referencesGroupBox, "offsetButtonGroup" );
    offsetButtonGroup->setTitle( trUtf8( "Offset" ) );
    offsetButtonGroup->setColumnLayout(0, Qt::Vertical );
    offsetButtonGroup->layout()->setSpacing( 6 );
    offsetButtonGroup->layout()->setMargin( 11 );
    offsetButtonGroupLayout = new QGridLayout( offsetButtonGroup->layout() );
    offsetButtonGroupLayout->setAlignment( Qt::AlignTop );

    specifiedRadioButton = new QRadioButton( offsetButtonGroup, "specifiedRadioButton" );
    specifiedRadioButton->setText( trUtf8( "Specified" ) );
    specifiedRadioButton->setChecked( TRUE );
    QToolTip::add( specifiedRadioButton, trUtf8( "Use a specified offset." ) );
    QWhatsThis::add( specifiedRadioButton, trUtf8( "<p><b>Specified Offset</b></p>\n"
"<p>Select this option if you want the dimension to have an independent, specified offset.</p>" ) );

    offsetButtonGroupLayout->addWidget( specifiedRadioButton, 0, 0 );

    offsetLengthSpinBox = new lCDefaultLengthSpinBox( offsetButtonGroup, "offsetLengthSpinBox" );
    QToolTip::add( offsetLengthSpinBox, trUtf8( "Enter the offset." ) );
    QWhatsThis::add( offsetLengthSpinBox, trUtf8( "<p><b>Specified Offset</b></p>\n"
"<p>Enter the offset of the dimension (from the From Reference to the To Reference). The units of the value are given in the default units specified in the application Preferences (so, you do not have to enter the units abbreviation).</p>\n"
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

    referencesGroupBoxLayout->addMultiCellWidget( offsetButtonGroup, 2, 2, 0, 1 );

    DimensionInfoDialogLayout->addMultiCellWidget( referencesGroupBox, 1, 1, 0, 2 );

    nameEdit = new QLineEdit( this, "nameEdit" );
    QToolTip::add( nameEdit, trUtf8( "Edit the dimension's name." ) );
    QWhatsThis::add( nameEdit, trUtf8( "<p><b>Name</b></p>\n"
"<p>Edit the name of the dimension. The dimension's name will be updated when the dialog is accepted.</p>" ) );

    DimensionInfoDialogLayout->addMultiCellWidget( nameEdit, 0, 0, 1, 2 );

    // signals and slots connections
    connect( buttonOk, SIGNAL( clicked() ), this, SLOT( accept() ) );
    connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
    connect( buttonHelp, SIGNAL( clicked() ), this, SLOT( buttonHelp_clicked() ) );

    // tab order
    setTabOrder( nameEdit, specifiedRadioButton );
    setTabOrder( specifiedRadioButton, offsetLengthSpinBox );
    setTabOrder( offsetLengthSpinBox, importedRadioButton );
    setTabOrder( importedRadioButton, constraintChooser );
    setTabOrder( constraintChooser, aboveButton );
    setTabOrder( aboveButton, rightButton );
    setTabOrder( rightButton, belowButton );
    setTabOrder( belowButton, leftButton );
    setTabOrder( leftButton, horizontalButton );
    setTabOrder( horizontalButton, verticalButton );
    setTabOrder( verticalButton, otherButton );
    setTabOrder( otherButton, buttonOk );
    setTabOrder( buttonOk, buttonHelp );
    setTabOrder( buttonHelp, buttonCancel );
    setTabOrder( buttonCancel, toReferenceLineEdit );
    setTabOrder( toReferenceLineEdit, fromReferenceLineEdit );

    // buddies
    nameLabel->setBuddy( nameEdit );
}

/*  
 *  Destroys the object and frees any allocated resources
 */
DimensionInfoDialog::~DimensionInfoDialog()
{
    // no need to delete child widgets, Qt does it all for us
}

