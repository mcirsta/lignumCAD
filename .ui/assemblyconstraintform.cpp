/****************************************************************************
** Form implementation generated from reading ui file 'assemblyconstraintform.ui'
**
** Created: Wed Dec 18 08:54:56 2002
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#include "assemblyconstraintform.h"

#include <qvariant.h>
#include <qbuttongroup.h>
#include <qframe.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qtextbrowser.h>
#include <qmime.h>
#include <qdragobject.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qimage.h>
#include <qpixmap.h>

#include "assemblyconstraintform.ui.h"
static QPixmap uic_load_pixmap_AssemblyConstraintForm( const QString &name )
{
    const QMimeSource *m = QMimeSourceFactory::defaultFactory()->data( name );
    if ( !m )
	return QPixmap();
    QPixmap pix;
    QImageDrag::decode( m, pix );
    return pix;
}
/* 
 *  Constructs a AssemblyConstraintForm which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f'.
 */
AssemblyConstraintForm::AssemblyConstraintForm( QWidget* parent,  const char* name, WFlags fl )
    : QWidget( parent, name, fl )
{
    if ( !name )
	setName( "AssemblyConstraintForm" );
    resize( 126, 448 ); 
    setCaption( trUtf8( "Assembly Constraints" ) );
    AssemblyConstraintFormLayout = new QVBoxLayout( this, 11, 6, "AssemblyConstraintFormLayout"); 

    TextLabel1 = new QLabel( this, "TextLabel1" );
    TextLabel1->setText( trUtf8( "Available\n"
"Constraints" ) );
    TextLabel1->setAlignment( int( QLabel::AlignCenter ) );
    AssemblyConstraintFormLayout->addWidget( TextLabel1 );

    constraintButtonGroup = new QButtonGroup( this, "constraintButtonGroup" );
    constraintButtonGroup->setLineWidth( 0 );
    constraintButtonGroup->setTitle( trUtf8( "" ) );
    constraintButtonGroup->setExclusive( TRUE );
    constraintButtonGroup->setColumnLayout(0, Qt::Vertical );
    constraintButtonGroup->layout()->setSpacing( 3 );
    constraintButtonGroup->layout()->setMargin( 0 );
    constraintButtonGroupLayout = new QVBoxLayout( constraintButtonGroup->layout() );
    constraintButtonGroupLayout->setAlignment( Qt::AlignTop );

    matePushButton = new QPushButton( constraintButtonGroup, "matePushButton" );
    matePushButton->setText( trUtf8( "&Mate" ) );
    matePushButton->setToggleButton( TRUE );
    QToolTip::add( matePushButton, trUtf8( "Create a mate constraint." ) );
    QWhatsThis::add( matePushButton, trUtf8( "<p><b>Mate</b></p>\n"
"<p>This constraint causes two surfaces to touch while facing one another. For example, imagine holding two blocks together:</p>\n"
"<img src=\"mate_example2.png\">\n"
"<p>Once this constraint is activated, pick one surface from the new model and one surface from the assembly. <i>lignumCAD</i> will calculate a new position (and orientation) of the new model.</p>\n"
"<p>You can cancel creating this constraint before it is complete by simply selecting a new constraint, selecting <b>Cancel constraint</b> from the context menu or by typing <b>ESC</b>.</p>" ) );
    constraintButtonGroupLayout->addWidget( matePushButton );

    alignPushButton = new QPushButton( constraintButtonGroup, "alignPushButton" );
    alignPushButton->setText( trUtf8( "&Align" ) );
    alignPushButton->setToggleButton( TRUE );
    QToolTip::add( alignPushButton, trUtf8( "Create an align constraint." ) );
    QWhatsThis::add( alignPushButton, trUtf8( "<p><b>Align</b></p>\n"
"<p>This constraint causes two surfaces to touch while facing in the same direction. For example, imagine lining up two blocks together:</p>\n"
"<img src=\"align_example.png\">\n"
"<p>Once this constraint is activated, pick one surface from the new model and one surface from the assembly. <i>lignumCAD</i> will calculate a new position (and orientation) of the new model.</p>\n"
"<p>You can cancel creating this constraint before it is complete by simply selecting a new constraint, selecting <b>Cancel constraint</b> from the context menu or by typing <b>ESC</b>.</p>" ) );
    constraintButtonGroupLayout->addWidget( alignPushButton );

    mateOffsetPushButton = new QPushButton( constraintButtonGroup, "mateOffsetPushButton" );
    mateOffsetPushButton->setText( trUtf8( "Mate &Offset" ) );
    mateOffsetPushButton->setToggleButton( TRUE );
    constraintButtonGroupLayout->addWidget( mateOffsetPushButton );

    alignOffsetPushButton = new QPushButton( constraintButtonGroup, "alignOffsetPushButton" );
    alignOffsetPushButton->setText( trUtf8( "Align Off&set" ) );
    alignOffsetPushButton->setToggleButton( TRUE );
    constraintButtonGroupLayout->addWidget( alignOffsetPushButton );
    AssemblyConstraintFormLayout->addWidget( constraintButtonGroup );

    Line1 = new QFrame( this, "Line1" );
    Line1->setFrameShape( QFrame::HLine );
    Line1->setFrameShadow( QFrame::Sunken );
    Line1->setFrameShape( QFrame::HLine );
    AssemblyConstraintFormLayout->addWidget( Line1 );

    helpPushButton = new QPushButton( this, "helpPushButton" );
    helpPushButton->setText( trUtf8( "He&lp" ) );
    QToolTip::add( helpPushButton, trUtf8( "See the short help message for this dialog." ) );
    AssemblyConstraintFormLayout->addWidget( helpPushButton );

    Line2 = new QFrame( this, "Line2" );
    Line2->setFrameShape( QFrame::HLine );
    Line2->setFrameShadow( QFrame::Sunken );
    Line2->setFrameShape( QFrame::HLine );
    AssemblyConstraintFormLayout->addWidget( Line2 );

    cancelPushButton = new QPushButton( this, "cancelPushButton" );
    cancelPushButton->setText( trUtf8( "&Cancel" ) );
    QToolTip::add( cancelPushButton, trUtf8( "Cancel the addition of the new model to the assembly." ) );
    QWhatsThis::add( cancelPushButton, trUtf8( "Clicking this button (or typing <b>Alt-C</b>) will cancel the addition of the new model to the assembly." ) );
    AssemblyConstraintFormLayout->addWidget( cancelPushButton );

    Line3 = new QFrame( this, "Line3" );
    Line3->setFrameShape( QFrame::HLine );
    Line3->setFrameShadow( QFrame::Sunken );
    Line3->setFrameShape( QFrame::HLine );
    AssemblyConstraintFormLayout->addWidget( Line3 );

    constraintsTextLabel = new QLabel( this, "constraintsTextLabel" );
    constraintsTextLabel->setText( trUtf8( "Defined\n"
"Constraints" ) );
    constraintsTextLabel->setAlignment( int( QLabel::AlignCenter ) );
    AssemblyConstraintFormLayout->addWidget( constraintsTextLabel );

    constraintListBrowser = new QTextBrowser( this, "constraintListBrowser" );
    AssemblyConstraintFormLayout->addWidget( constraintListBrowser );
    QSpacerItem* spacer = new QSpacerItem( 0, 30, QSizePolicy::Minimum, QSizePolicy::Expanding );
    AssemblyConstraintFormLayout->addItem( spacer );

    // signals and slots connections
    connect( helpPushButton, SIGNAL( clicked() ), this, SLOT( helpPushButton_clicked() ) );
}

/*  
 *  Destroys the object and frees any allocated resources
 */
AssemblyConstraintForm::~AssemblyConstraintForm()
{
    // no need to delete child widgets, Qt does it all for us
}

