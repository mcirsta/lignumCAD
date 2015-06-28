/****************************************************************************
** Form implementation generated from reading ui file 'materialdialog.ui'
**
** Created: Thu Jan 2 13:42:17 2003
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#include "materialdialog.h"

#include <qvariant.h>
#include <qdir.h>
#include <qdom.h>
#include <qgroupbox.h>
#include <qheader.h>
#include <qlabel.h>
#include <qlistview.h>
#include <qpalette.h>
#include <qpushbutton.h>
#include <qtextedit.h>
#include <qwhatsthis.h>
#include <qmime.h>
#include <qdragobject.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qimage.h>
#include <qpixmap.h>

#include "constants.h"
#include "materialdialog.ui.h"
static QPixmap uic_load_pixmap_MaterialDialog( const QString &name )
{
    const QMimeSource *m = QMimeSourceFactory::defaultFactory()->data( name );
    if ( !m )
	return QPixmap();
    QPixmap pix;
    QImageDrag::decode( m, pix );
    return pix;
}
/* 
 *  Constructs a MaterialDialog which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
MaterialDialog::MaterialDialog( QWidget* parent,  const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
	setName( "MaterialDialog" );
    resize( 538, 468 ); 
    setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)5, 0, 0, sizePolicy().hasHeightForWidth() ) );
    setCaption( trUtf8( "Set the Material for the Part" ) );
    setIcon( uic_load_pixmap_MaterialDialog( "lignumCAD.png" ) );
    setSizeGripEnabled( TRUE );
    MaterialDialogLayout = new QGridLayout( this, 1, 1, 11, 6, "MaterialDialogLayout"); 

    buttonHelp = new QPushButton( this, "buttonHelp" );
    buttonHelp->setText( trUtf8( "&Help" ) );
    buttonHelp->setAccel( 276824136 );
    buttonHelp->setAutoDefault( TRUE );

    MaterialDialogLayout->addWidget( buttonHelp, 1, 0 );

    detailsGroupBox = new QGroupBox( this, "detailsGroupBox" );
    detailsGroupBox->setTitle( trUtf8( "Material Details" ) );
    detailsGroupBox->setColumnLayout(0, Qt::Vertical );
    detailsGroupBox->layout()->setSpacing( 6 );
    detailsGroupBox->layout()->setMargin( 11 );
    detailsGroupBoxLayout = new QHBoxLayout( detailsGroupBox->layout() );
    detailsGroupBoxLayout->setAlignment( Qt::AlignTop );

    MaterialText = new QTextEdit( detailsGroupBox, "MaterialText" );
    MaterialText->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)7, 100, 0, MaterialText->sizePolicy().hasHeightForWidth() ) );
    MaterialText->setMinimumSize( QSize( 250, 0 ) );
    MaterialText->setReadOnly( TRUE );
    detailsGroupBoxLayout->addWidget( MaterialText );

    Layout19 = new QVBoxLayout( 0, 0, 6, "Layout19"); 

    Layout10 = new QGridLayout( 0, 1, 1, 0, 6, "Layout10"); 

    SolidColorLabel = new QLabel( detailsGroupBox, "SolidColorLabel" );
    SolidColorLabel->setText( trUtf8( "Solid Color" ) );
    SolidColorLabel->setAlignment( int( QLabel::AlignCenter ) );

    Layout10->addMultiCellWidget( SolidColorLabel, 1, 1, 0, 2 );
    QSpacerItem* spacer = new QSpacerItem( 21, 21, QSizePolicy::Expanding, QSizePolicy::Minimum );
    Layout10->addItem( spacer, 0, 2 );

    SolidColor = new QLabel( detailsGroupBox, "SolidColor" );
    SolidColor->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, SolidColor->sizePolicy().hasHeightForWidth() ) );
    SolidColor->setMinimumSize( QSize( 64, 64 ) );
    SolidColor->setMaximumSize( QSize( 64, 64 ) );
    SolidColor->setFrameShape( QLabel::Box );
    SolidColor->setFrameShadow( QLabel::Sunken );
    SolidColor->setText( trUtf8( "" ) );
    SolidColor->setAlignment( int( QLabel::AlignCenter ) );

    Layout10->addWidget( SolidColor, 0, 1 );
    QSpacerItem* spacer_2 = new QSpacerItem( 16, 21, QSizePolicy::Minimum, QSizePolicy::Expanding );
    Layout10->addItem( spacer_2, 2, 1 );
    QSpacerItem* spacer_3 = new QSpacerItem( 21, 21, QSizePolicy::Expanding, QSizePolicy::Minimum );
    Layout10->addItem( spacer_3, 0, 0 );
    Layout19->addLayout( Layout10 );

    Layout16 = new QGridLayout( 0, 1, 1, 0, 6, "Layout16"); 
    QSpacerItem* spacer_4 = new QSpacerItem( 21, 21, QSizePolicy::Expanding, QSizePolicy::Minimum );
    Layout16->addItem( spacer_4, 0, 2 );

    FaceGrainLabel = new QLabel( detailsGroupBox, "FaceGrainLabel" );
    FaceGrainLabel->setText( trUtf8( "Face Grain" ) );
    FaceGrainLabel->setAlignment( int( QLabel::AlignCenter ) );

    Layout16->addMultiCellWidget( FaceGrainLabel, 1, 1, 0, 2 );

    FaceGrainPixmap = new QLabel( detailsGroupBox, "FaceGrainPixmap" );
    FaceGrainPixmap->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, FaceGrainPixmap->sizePolicy().hasHeightForWidth() ) );
    FaceGrainPixmap->setMinimumSize( QSize( 64, 64 ) );
    FaceGrainPixmap->setMaximumSize( QSize( 64, 64 ) );
    FaceGrainPixmap->setFrameShape( QLabel::Box );
    FaceGrainPixmap->setFrameShadow( QLabel::Sunken );
    FaceGrainPixmap->setScaledContents( FALSE );
    FaceGrainPixmap->setAlignment( int( QLabel::AlignCenter ) );

    Layout16->addWidget( FaceGrainPixmap, 0, 1 );
    QSpacerItem* spacer_5 = new QSpacerItem( 21, 21, QSizePolicy::Expanding, QSizePolicy::Minimum );
    Layout16->addItem( spacer_5, 0, 0 );
    QSpacerItem* spacer_6 = new QSpacerItem( 16, 21, QSizePolicy::Minimum, QSizePolicy::Expanding );
    Layout16->addItem( spacer_6, 2, 1 );
    Layout19->addLayout( Layout16 );

    Layout17 = new QGridLayout( 0, 1, 1, 0, 6, "Layout17"); 
    QSpacerItem* spacer_7 = new QSpacerItem( 16, 21, QSizePolicy::Minimum, QSizePolicy::Expanding );
    Layout17->addItem( spacer_7, 2, 1 );
    QSpacerItem* spacer_8 = new QSpacerItem( 21, 21, QSizePolicy::Expanding, QSizePolicy::Minimum );
    Layout17->addItem( spacer_8, 0, 2 );
    QSpacerItem* spacer_9 = new QSpacerItem( 21, 21, QSizePolicy::Expanding, QSizePolicy::Minimum );
    Layout17->addItem( spacer_9, 0, 0 );

    EndGrainLabel = new QLabel( detailsGroupBox, "EndGrainLabel" );
    EndGrainLabel->setText( trUtf8( "End Grain" ) );
    EndGrainLabel->setAlignment( int( QLabel::AlignCenter ) );

    Layout17->addMultiCellWidget( EndGrainLabel, 1, 1, 0, 2 );

    EndGrainPixmap = new QLabel( detailsGroupBox, "EndGrainPixmap" );
    EndGrainPixmap->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, EndGrainPixmap->sizePolicy().hasHeightForWidth() ) );
    EndGrainPixmap->setMinimumSize( QSize( 64, 64 ) );
    EndGrainPixmap->setMaximumSize( QSize( 64, 64 ) );
    EndGrainPixmap->setFrameShape( QLabel::Box );
    EndGrainPixmap->setFrameShadow( QLabel::Sunken );
    EndGrainPixmap->setScaledContents( FALSE );
    EndGrainPixmap->setAlignment( int( QLabel::AlignCenter ) );

    Layout17->addWidget( EndGrainPixmap, 0, 1 );
    Layout19->addLayout( Layout17 );

    Layout18 = new QGridLayout( 0, 1, 1, 0, 6, "Layout18"); 

    EdgeGrainLabel = new QLabel( detailsGroupBox, "EdgeGrainLabel" );
    EdgeGrainLabel->setText( trUtf8( "Edge Grain" ) );
    EdgeGrainLabel->setAlignment( int( QLabel::AlignCenter ) );

    Layout18->addMultiCellWidget( EdgeGrainLabel, 1, 1, 0, 2 );

    EdgeGrainPixmap = new QLabel( detailsGroupBox, "EdgeGrainPixmap" );
    EdgeGrainPixmap->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, EdgeGrainPixmap->sizePolicy().hasHeightForWidth() ) );
    EdgeGrainPixmap->setMinimumSize( QSize( 64, 64 ) );
    EdgeGrainPixmap->setMaximumSize( QSize( 64, 64 ) );
    EdgeGrainPixmap->setFrameShape( QLabel::Box );
    EdgeGrainPixmap->setFrameShadow( QLabel::Sunken );
    EdgeGrainPixmap->setScaledContents( FALSE );
    EdgeGrainPixmap->setAlignment( int( QLabel::AlignCenter ) );

    Layout18->addWidget( EdgeGrainPixmap, 0, 1 );
    QSpacerItem* spacer_10 = new QSpacerItem( 21, 21, QSizePolicy::Expanding, QSizePolicy::Minimum );
    Layout18->addItem( spacer_10, 0, 0 );
    QSpacerItem* spacer_11 = new QSpacerItem( 21, 21, QSizePolicy::Expanding, QSizePolicy::Minimum );
    Layout18->addItem( spacer_11, 0, 2 );
    Layout19->addLayout( Layout18 );
    detailsGroupBoxLayout->addLayout( Layout19 );

    MaterialDialogLayout->addMultiCellWidget( detailsGroupBox, 0, 0, 2, 4 );

    MaterialList = new QListView( this, "MaterialList" );
    MaterialList->addColumn( trUtf8( "Materials" ) );
    MaterialList->setMinimumSize( QSize( 150, 0 ) );
    MaterialList->setAllColumnsShowFocus( TRUE );

    MaterialDialogLayout->addMultiCellWidget( MaterialList, 0, 0, 0, 1 );
    QSpacerItem* spacer_12 = new QSpacerItem( 150, 16, QSizePolicy::Expanding, QSizePolicy::Minimum );
    MaterialDialogLayout->addMultiCell( spacer_12, 1, 1, 1, 2 );

    buttonOk = new QPushButton( this, "buttonOk" );
    buttonOk->setText( trUtf8( "&OK" ) );
    buttonOk->setAccel( 276824143 );
    buttonOk->setAutoDefault( TRUE );
    buttonOk->setDefault( TRUE );

    MaterialDialogLayout->addWidget( buttonOk, 1, 3 );

    buttonCancel = new QPushButton( this, "buttonCancel" );
    buttonCancel->setText( trUtf8( "&Cancel" ) );
    buttonCancel->setAccel( 276824131 );
    buttonCancel->setAutoDefault( TRUE );

    MaterialDialogLayout->addWidget( buttonCancel, 1, 4 );

    // signals and slots connections
    connect( buttonOk, SIGNAL( clicked() ), this, SLOT( accept() ) );
    connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
    connect( MaterialList, SIGNAL( selectionChanged(QListViewItem*) ), this, SLOT( MaterialList_selectionChanged(QListViewItem*) ) );
    connect( buttonHelp, SIGNAL( clicked() ), this, SLOT( buttonHelp_clicked() ) );

    // tab order
    setTabOrder( MaterialList, buttonOk );
    setTabOrder( buttonOk, buttonHelp );
    setTabOrder( buttonHelp, buttonCancel );
    setTabOrder( buttonCancel, MaterialText );
    init();
}

/*  
 *  Destroys the object and frees any allocated resources
 */
MaterialDialog::~MaterialDialog()
{
    // no need to delete child widgets, Qt does it all for us
}

