/****************************************************************************
** Form implementation generated from reading ui file 'newmodelwizard.ui'
**
** Created: Wed Dec 18 08:54:54 2002
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#include "newmodelwizard.h"

#include <qvariant.h>
#include <lcfilechooser.h>
#include <qapplication.h>
#include <qbuttongroup.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qspinbox.h>
#include <qtextedit.h>
#include <qvaluevector.h>
#include <qwidget.h>
#include <qmime.h>
#include <qdragobject.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qimage.h>
#include <qpixmap.h>

#include "pagefactory.h"
#include "newmodelwizard.ui.h"
static QPixmap uic_load_pixmap_NewModelWizard( const QString &name )
{
    const QMimeSource *m = QMimeSourceFactory::defaultFactory()->data( name );
    if ( !m )
	return QPixmap();
    QPixmap pix;
    QImageDrag::decode( m, pix );
    return pix;
}
/* 
 *  Constructs a NewModelWizard which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f'.
 *
 *  The wizard will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal wizard.
 */
NewModelWizard::NewModelWizard( QWidget* parent,  const char* name, bool modal, WFlags fl )
    : QWizard( parent, name, modal, fl )
{
    if ( !name )
	setName( "NewModelWizard" );
    resize( 581, 402 ); 
    setCaption( trUtf8( "New Model Wizard" ) );
    setIcon( uic_load_pixmap_NewModelWizard( "lignumCAD.png" ) );

    NewModelPage = new QWidget( this, "NewModelPage" );
    NewModelPageLayout = new QGridLayout( NewModelPage, 1, 1, 11, 6, "NewModelPageLayout"); 

    modelNameEdit = new QLineEdit( NewModelPage, "modelNameEdit" );
    QWhatsThis::add( modelNameEdit, trUtf8( "<p><b>Model Name</b></p>\n"
"Enter a name for the model." ) );

    NewModelPageLayout->addWidget( modelNameEdit, 0, 2 );

    fileNameLabel = new QLabel( NewModelPage, "fileNameLabel" );
    fileNameLabel->setText( trUtf8( "&File Name:" ) );

    NewModelPageLayout->addWidget( fileNameLabel, 1, 1 );

    versionLabel = new QLabel( NewModelPage, "versionLabel" );
    versionLabel->setText( trUtf8( "&Version:" ) );

    NewModelPageLayout->addWidget( versionLabel, 0, 3 );

    versionSpinBox = new QSpinBox( NewModelPage, "versionSpinBox" );
    versionSpinBox->setMaxValue( 1000 );
    versionSpinBox->setMinValue( 1 );
    QWhatsThis::add( versionSpinBox, trUtf8( "<p><b>Version Number</b></p>\n"
"<p>A change in version number is usually used to represent major changes to the model. For example, if Version 1 proved too impractical to construct and you decided to simplify the model, it might become Version 2.</p>" ) );

    NewModelPageLayout->addWidget( versionSpinBox, 0, 4 );

    modificationLabel = new QLabel( NewModelPage, "modificationLabel" );
    modificationLabel->setText( trUtf8( "Modification Time:" ) );

    NewModelPageLayout->addWidget( modificationLabel, 4, 1 );

    descriptionLabel = new QLabel( NewModelPage, "descriptionLabel" );
    descriptionLabel->setText( trUtf8( "&Description:" ) );
    descriptionLabel->setAlignment( int( QLabel::AlignTop ) );

    NewModelPageLayout->addWidget( descriptionLabel, 2, 1 );

    descriptionEdit = new QTextEdit( NewModelPage, "descriptionEdit" );
    QWhatsThis::add( descriptionEdit, trUtf8( "<p><b>Description</b></p>\n"
"<p>Enter a brief description of the model which is being constructed. This information is not used anywhere in <i>lignumCAD</i>; it is purely to aid you in identifying the model.</p>" ) );

    NewModelPageLayout->addMultiCellWidget( descriptionEdit, 2, 2, 2, 4 );

    revisionLabel = new QLabel( NewModelPage, "revisionLabel" );
    revisionLabel->setText( trUtf8( "&Revision:" ) );

    NewModelPageLayout->addWidget( revisionLabel, 1, 3 );

    modelFileChooser = new lCFileChooser( NewModelPage, "modelFileChooser" );
    modelFileChooser->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)1, 0, 0, modelFileChooser->sizePolicy().hasHeightForWidth() ) );
    QWhatsThis::add( modelFileChooser, trUtf8( "<p><b>File Name</b></p>\n"
"<p>Enter the name of the file in which to store\n"
"the model. Normally, this is automatically completed\n"
"when you enter the model name.</p>\n"
"<p>You can also click the <b>...</b> button to bring up a\n"
"standard file requestor if you want to overwrite an existing file.</p>" ) );

    NewModelPageLayout->addWidget( modelFileChooser, 1, 2 );

    revisionSpinBox = new QSpinBox( NewModelPage, "revisionSpinBox" );
    QWhatsThis::add( revisionSpinBox, trUtf8( "<p><b>Revision Number</b></p>\n"
"<p>A change in revision number is usually used to represent minor changes to the model. For example, if you need to change a few dimensions, you might increment the revision number.</p>" ) );

    NewModelPageLayout->addWidget( revisionSpinBox, 1, 4 );

    creationLabel = new QLabel( NewModelPage, "creationLabel" );
    creationLabel->setText( trUtf8( "Creation Time:" ) );

    NewModelPageLayout->addWidget( creationLabel, 3, 1 );

    Layout4 = new QVBoxLayout( 0, 0, 6, "Layout4"); 
    QSpacerItem* spacer = new QSpacerItem( 0, 20, QSizePolicy::Minimum, QSizePolicy::Expanding );
    Layout4->addItem( spacer );

    newWizardPixmap = new QLabel( NewModelPage, "newWizardPixmap" );
    newWizardPixmap->setPixmap( uic_load_pixmap_NewModelWizard( "shop.png" ) );
    newWizardPixmap->setScaledContents( TRUE );
    QToolTip::add( newWizardPixmap, trUtf8( "The author struggles with gluing edge banding on a piece of oak plywood." ) );
    QWhatsThis::add( newWizardPixmap, trUtf8( "The author struggles with gluing edge banding on a piece of oak plywood." ) );
    Layout4->addWidget( newWizardPixmap );
    QSpacerItem* spacer_2 = new QSpacerItem( 0, 20, QSizePolicy::Minimum, QSizePolicy::Expanding );
    Layout4->addItem( spacer_2 );

    NewModelPageLayout->addMultiCellLayout( Layout4, 0, 4, 0, 0 );

    modelNameLabel = new QLabel( NewModelPage, "modelNameLabel" );
    modelNameLabel->setText( trUtf8( "&Model Name:" ) );

    NewModelPageLayout->addWidget( modelNameLabel, 0, 1 );

    creationDateLabel = new QLabel( NewModelPage, "creationDateLabel" );
    creationDateLabel->setFrameShape( QLabel::Box );
    creationDateLabel->setFrameShadow( QLabel::Sunken );
    creationDateLabel->setText( trUtf8( "TextLabel2" ) );

    NewModelPageLayout->addMultiCellWidget( creationDateLabel, 3, 3, 2, 4 );

    modificationDateLabel = new QLabel( NewModelPage, "modificationDateLabel" );
    modificationDateLabel->setFrameShape( QLabel::Box );
    modificationDateLabel->setFrameShadow( QLabel::Sunken );
    modificationDateLabel->setText( trUtf8( "TextLabel2" ) );

    NewModelPageLayout->addMultiCellWidget( modificationDateLabel, 4, 4, 2, 4 );
    addPage( NewModelPage, trUtf8( "Enter the Information about the New Model" ) );

    InitialPagePage = new QWidget( this, "InitialPagePage" );
    InitialPagePageLayout = new QGridLayout( InitialPagePage, 1, 1, 11, 6, "InitialPagePageLayout"); 
    QSpacerItem* spacer_3 = new QSpacerItem( 0, 20, QSizePolicy::Minimum, QSizePolicy::Expanding );
    InitialPagePageLayout->addItem( spacer_3, 0, 0 );

    newWizardPixmap_2 = new QLabel( InitialPagePage, "newWizardPixmap_2" );
    newWizardPixmap_2->setPixmap( uic_load_pixmap_NewModelWizard( "shop.png" ) );
    newWizardPixmap_2->setScaledContents( TRUE );
    QToolTip::add( newWizardPixmap_2, trUtf8( "The author struggles with gluing edge banding on a piece of oak plywood." ) );

    InitialPagePageLayout->addMultiCellWidget( newWizardPixmap_2, 1, 3, 0, 0 );
    QSpacerItem* spacer_4 = new QSpacerItem( 0, 20, QSizePolicy::Minimum, QSizePolicy::Expanding );
    InitialPagePageLayout->addItem( spacer_4, 4, 0 );
    QSpacerItem* spacer_5 = new QSpacerItem( 20, 0, QSizePolicy::Expanding, QSizePolicy::Minimum );
    InitialPagePageLayout->addItem( spacer_5, 2, 1 );
    QSpacerItem* spacer_6 = new QSpacerItem( 20, 0, QSizePolicy::Expanding, QSizePolicy::Minimum );
    InitialPagePageLayout->addItem( spacer_6, 2, 3 );
    QSpacerItem* spacer_7 = new QSpacerItem( 0, 20, QSizePolicy::Minimum, QSizePolicy::Expanding );
    InitialPagePageLayout->addMultiCell( spacer_7, 0, 1, 2, 2 );
    QSpacerItem* spacer_8 = new QSpacerItem( 0, 20, QSizePolicy::Minimum, QSizePolicy::Expanding );
    InitialPagePageLayout->addMultiCell( spacer_8, 3, 4, 2, 2 );

    initialPageGroupBox = new QGroupBox( InitialPagePage, "initialPageGroupBox" );
    initialPageGroupBox->setTitle( trUtf8( "Page Types" ) );
    initialPageGroupBox->setColumnLayout(0, Qt::Vertical );
    initialPageGroupBox->layout()->setSpacing( 6 );
    initialPageGroupBox->layout()->setMargin( 11 );
    initialPageGroupBoxLayout = new QVBoxLayout( initialPageGroupBox->layout() );
    initialPageGroupBoxLayout->setAlignment( Qt::AlignTop );

    InitialPagePageLayout->addWidget( initialPageGroupBox, 2, 2 );
    addPage( InitialPagePage, trUtf8( "Select an Initial Page for Model" ) );

    // signals and slots connections
    connect( modelNameEdit, SIGNAL( textChanged(const QString&) ), this, SLOT( modelNameEdit_textChanged(const QString&) ) );
    connect( this, SIGNAL( selected(const QString&) ), this, SLOT( NewModelWizard_selected(const QString&) ) );
    connect( this, SIGNAL( helpClicked() ), this, SLOT( NewModelWizard_helpClicked() ) );

    // tab order
    setTabOrder( modelNameEdit, versionSpinBox );
    setTabOrder( versionSpinBox, modelFileChooser );
    setTabOrder( modelFileChooser, revisionSpinBox );
    setTabOrder( revisionSpinBox, descriptionEdit );

    // buddies
    fileNameLabel->setBuddy( modelFileChooser );
    versionLabel->setBuddy( versionSpinBox );
    descriptionLabel->setBuddy( descriptionEdit );
    revisionLabel->setBuddy( revisionSpinBox );
    modelNameLabel->setBuddy( modelNameEdit );
    init();
}

/*  
 *  Destroys the object and frees any allocated resources
 */
NewModelWizard::~NewModelWizard()
{
    // no need to delete child widgets, Qt does it all for us
}

