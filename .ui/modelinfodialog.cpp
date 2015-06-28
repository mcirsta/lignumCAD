/****************************************************************************
** Form implementation generated from reading ui file 'modelinfodialog.ui'
**
** Created: Wed Dec 18 08:54:53 2002
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#include "modelinfodialog.h"

#include <qvariant.h>
#include <lcfilechooser.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qspinbox.h>
#include <qtextedit.h>
#include <qmime.h>
#include <qdragobject.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qimage.h>
#include <qpixmap.h>

#include "modelinfodialog.ui.h"
static QPixmap uic_load_pixmap_ModelInfoDialog( const QString &name )
{
    const QMimeSource *m = QMimeSourceFactory::defaultFactory()->data( name );
    if ( !m )
	return QPixmap();
    QPixmap pix;
    QImageDrag::decode( m, pix );
    return pix;
}
/* 
 *  Constructs a ModelInfoDialog which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
ModelInfoDialog::ModelInfoDialog( QWidget* parent,  const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
	setName( "ModelInfoDialog" );
    resize( 367, 294 ); 
    setCaption( trUtf8( "Model Information" ) );
    setIcon( uic_load_pixmap_ModelInfoDialog( "lignumCAD.png" ) );
    setSizeGripEnabled( FALSE );
    ModelInfoDialogLayout = new QGridLayout( this, 1, 1, 11, 6, "ModelInfoDialogLayout"); 

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

    ModelInfoDialogLayout->addMultiCellLayout( Layout1, 5, 5, 0, 3 );

    modelNameEdit = new QLineEdit( this, "modelNameEdit" );
    QWhatsThis::add( modelNameEdit, trUtf8( "<p><b>Model Name</b></p>\n"
"Enter a name for the model." ) );

    ModelInfoDialogLayout->addWidget( modelNameEdit, 0, 1 );

    versionLabel = new QLabel( this, "versionLabel" );
    versionLabel->setText( trUtf8( "&Version:" ) );

    ModelInfoDialogLayout->addWidget( versionLabel, 0, 2 );
    QSpacerItem* spacer_2 = new QSpacerItem( 20, 0, QSizePolicy::Expanding, QSizePolicy::Minimum );
    ModelInfoDialogLayout->addMultiCell( spacer_2, 3, 3, 2, 3 );

    revisionSpinBox = new QSpinBox( this, "revisionSpinBox" );
    QWhatsThis::add( revisionSpinBox, trUtf8( "<p><b>Revision Number</b></p>\n"
"<p>A change in revision number is usually used to represent minor changes to the model. For example, if you need to change a few dimensions, you might increment the revision number.</p>" ) );

    ModelInfoDialogLayout->addWidget( revisionSpinBox, 1, 3 );
    QSpacerItem* spacer_3 = new QSpacerItem( 20, 0, QSizePolicy::Expanding, QSizePolicy::Minimum );
    ModelInfoDialogLayout->addMultiCell( spacer_3, 4, 4, 2, 3 );

    fileNameLabel = new QLabel( this, "fileNameLabel" );
    fileNameLabel->setText( trUtf8( "&File Name:" ) );

    ModelInfoDialogLayout->addWidget( fileNameLabel, 1, 0 );

    versionSpinBox = new QSpinBox( this, "versionSpinBox" );
    versionSpinBox->setMaxValue( 1000 );
    versionSpinBox->setMinValue( 1 );
    QWhatsThis::add( versionSpinBox, trUtf8( "<p><b>Version Number</b></p>\n"
"<p>A change in version number is usually used to represent major changes to the model. For example, if Version 1 proved too impractical to construct and you decided to simplify the model, it might become Version 2.</p>" ) );

    ModelInfoDialogLayout->addWidget( versionSpinBox, 0, 3 );

    creationDateLabel = new QLabel( this, "creationDateLabel" );
    creationDateLabel->setFrameShape( QLabel::Box );
    creationDateLabel->setFrameShadow( QLabel::Sunken );
    creationDateLabel->setText( trUtf8( "TextLabel2" ) );

    ModelInfoDialogLayout->addWidget( creationDateLabel, 3, 1 );

    modificationLabel = new QLabel( this, "modificationLabel" );
    modificationLabel->setText( trUtf8( "Modification Time:" ) );

    ModelInfoDialogLayout->addWidget( modificationLabel, 4, 0 );

    modificationDateLabel = new QLabel( this, "modificationDateLabel" );
    modificationDateLabel->setFrameShape( QLabel::Box );
    modificationDateLabel->setFrameShadow( QLabel::Sunken );
    modificationDateLabel->setText( trUtf8( "TextLabel2" ) );

    ModelInfoDialogLayout->addWidget( modificationDateLabel, 4, 1 );

    modelNameLabel = new QLabel( this, "modelNameLabel" );
    modelNameLabel->setText( trUtf8( "&Model Name:" ) );

    ModelInfoDialogLayout->addWidget( modelNameLabel, 0, 0 );

    creationLabel = new QLabel( this, "creationLabel" );
    creationLabel->setText( trUtf8( "Creation Time:" ) );

    ModelInfoDialogLayout->addWidget( creationLabel, 3, 0 );

    descriptionEdit = new QTextEdit( this, "descriptionEdit" );
    QWhatsThis::add( descriptionEdit, trUtf8( "<p><b>Description</b></p>\n"
"<p>Enter a brief description of the model which is being constructed. This information is not used anywhere in <i>lignumCAD</i>; it is purely to aid you in identifying the model.</p>" ) );

    ModelInfoDialogLayout->addMultiCellWidget( descriptionEdit, 2, 2, 1, 3 );

    descriptionLabel = new QLabel( this, "descriptionLabel" );
    descriptionLabel->setText( trUtf8( "&Description:" ) );
    descriptionLabel->setAlignment( int( QLabel::AlignTop ) );

    ModelInfoDialogLayout->addWidget( descriptionLabel, 2, 0 );

    revisionLabel = new QLabel( this, "revisionLabel" );
    revisionLabel->setText( trUtf8( "&Revision:" ) );

    ModelInfoDialogLayout->addWidget( revisionLabel, 1, 2 );

    modelFileChooser = new lCFileChooser( this, "modelFileChooser" );
    modelFileChooser->setLineWidth( 2 );
    QWhatsThis::add( modelFileChooser, trUtf8( "<p><b>File Name</b></p>\n"
"<p>Enter the name of the file in which to store\n"
"the model. Normally, this is automatically completed\n"
"when you enter the model name.</p>\n"
"<p>You can also click the <b>...</b> button to bring up a\n"
"standard file requestor if you want to overwrite an existing file.</p>" ) );

    ModelInfoDialogLayout->addWidget( modelFileChooser, 1, 1 );

    // signals and slots connections
    connect( buttonOk, SIGNAL( clicked() ), this, SLOT( accept() ) );
    connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
    connect( modelNameEdit, SIGNAL( textChanged(const QString&) ), this, SLOT( modelNameEdit_textChanged(const QString&) ) );
    connect( buttonHelp, SIGNAL( clicked() ), this, SLOT( buttonHelp_clicked() ) );

    // tab order
    setTabOrder( modelNameEdit, versionSpinBox );
    setTabOrder( versionSpinBox, modelFileChooser );
    setTabOrder( modelFileChooser, revisionSpinBox );
    setTabOrder( revisionSpinBox, descriptionEdit );
    setTabOrder( descriptionEdit, buttonHelp );
    setTabOrder( buttonHelp, buttonOk );
    setTabOrder( buttonOk, buttonCancel );

    // buddies
    versionLabel->setBuddy( versionSpinBox );
    fileNameLabel->setBuddy( modelFileChooser );
    modelNameLabel->setBuddy( modelNameEdit );
    descriptionLabel->setBuddy( descriptionEdit );
    revisionLabel->setBuddy( revisionSpinBox );
}

/*  
 *  Destroys the object and frees any allocated resources
 */
ModelInfoDialog::~ModelInfoDialog()
{
    // no need to delete child widgets, Qt does it all for us
}

