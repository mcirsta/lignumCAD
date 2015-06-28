/****************************************************************************
** Form implementation generated from reading ui file 'annotationinfodialog.ui'
**
** Created: Wed Dec 18 08:54:55 2002
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#include "annotationinfodialog.h"

#include <qvariant.h>
#include <qcombobox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qtextedit.h>
#include <qtoolbutton.h>
#include <qmime.h>
#include <qdragobject.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qimage.h>
#include <qpixmap.h>

#include "openglbase.h"
#include "qcolordialog.h"
#include "qfontdatabase.h"
#include "annotationinfodialog.ui.h"
static QPixmap uic_load_pixmap_AnnotationInfoDialog( const QString &name )
{
    const QMimeSource *m = QMimeSourceFactory::defaultFactory()->data( name );
    if ( !m )
	return QPixmap();
    QPixmap pix;
    QImageDrag::decode( m, pix );
    return pix;
}
/* 
 *  Constructs a AnnotationInfoDialog which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
AnnotationInfoDialog::AnnotationInfoDialog( QWidget* parent,  const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
	setName( "AnnotationInfoDialog" );
    resize( 308, 229 ); 
    setCaption( trUtf8( "Annotation Information" ) );
    setIcon( uic_load_pixmap_AnnotationInfoDialog( "lignumCAD.png" ) );
    setSizeGripEnabled( TRUE );
    AnnotationInfoDialogLayout = new QGridLayout( this, 1, 1, 11, 6, "AnnotationInfoDialogLayout"); 

    nameTextLabel = new QLabel( this, "nameTextLabel" );
    nameTextLabel->setText( trUtf8( "&Name:" ) );

    AnnotationInfoDialogLayout->addWidget( nameTextLabel, 0, 0 );

    colorButton = new QPushButton( this, "colorButton" );
    colorButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, colorButton->sizePolicy().hasHeightForWidth() ) );
    colorButton->setMinimumSize( QSize( 32, 32 ) );
    colorButton->setMaximumSize( QSize( 32, 32 ) );
    colorButton->setText( trUtf8( "" ) );
    QToolTip::add( colorButton, trUtf8( "Choose the text color." ) );
    QWhatsThis::add( colorButton, trUtf8( "<p><b>Text Color</b></p>\n"
"<p>Set the current color of the annotation text. This works two ways: if some portion of the text is highlighted, it will be rendered in the given color. Otherwise, all text typed after the present position of the cursor will be drawn in the given color.</p>" ) );

    AnnotationInfoDialogLayout->addWidget( colorButton, 1, 2 );
    QSpacerItem* spacer = new QSpacerItem( 20, 0, QSizePolicy::Expanding, QSizePolicy::Minimum );
    AnnotationInfoDialogLayout->addItem( spacer, 1, 3 );

    annotationTextEdit = new QTextEdit( this, "annotationTextEdit" );
    annotationTextEdit->setTextFormat( QTextEdit::RichText );

    AnnotationInfoDialogLayout->addMultiCellWidget( annotationTextEdit, 2, 2, 0, 3 );

    Layout1 = new QHBoxLayout( 0, 0, 6, "Layout1"); 

    buttonHelp = new QPushButton( this, "buttonHelp" );
    buttonHelp->setText( trUtf8( "&Help" ) );
    buttonHelp->setAccel( 276824136 );
    buttonHelp->setAutoDefault( TRUE );
    Layout1->addWidget( buttonHelp );
    QSpacerItem* spacer_2 = new QSpacerItem( 20, 0, QSizePolicy::Expanding, QSizePolicy::Minimum );
    Layout1->addItem( spacer_2 );

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

    AnnotationInfoDialogLayout->addMultiCellLayout( Layout1, 3, 3, 0, 3 );

    nameEdit = new QLineEdit( this, "nameEdit" );
    QToolTip::add( nameEdit, trUtf8( "Choose a name for the annotation." ) );
    QWhatsThis::add( nameEdit, trUtf8( "<p><b>Name</b></p>\n"
"<p>Edit the name of the annotation. The annotation's name will be updated when the dialog is accepted. If there is already a annotation on this page which has the same name, the name change will be rejected.</p>" ) );

    AnnotationInfoDialogLayout->addMultiCellWidget( nameEdit, 0, 0, 1, 3 );

    Layout4 = new QHBoxLayout( 0, 0, 2, "Layout4"); 

    boldButton = new QToolButton( this, "boldButton" );
    QFont boldButton_font(  boldButton->font() );
    boldButton_font.setBold( TRUE );
    boldButton->setFont( boldButton_font ); 
    boldButton->setText( trUtf8( "B" ) );
    boldButton->setToggleButton( TRUE );
    boldButton->setAutoRaise( TRUE );
    QToolTip::add( boldButton, trUtf8( "Set the current selection to bold." ) );
    QWhatsThis::add( boldButton, trUtf8( "<p><b>Bold</b></p>\n"
"<p>Set (or unset) the current style of the annotation text to bold. This works two ways: if some portion of the text is highlighted, it will be rendered in bold face (or it will have the bold property removed). Otherwise, all text typed after the present position of the cursor will be bold (or  not).</p>" ) );
    Layout4->addWidget( boldButton );

    italicButton = new QToolButton( this, "italicButton" );
    italicButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)7, 0, 0, italicButton->sizePolicy().hasHeightForWidth() ) );
    QFont italicButton_font(  italicButton->font() );
    italicButton_font.setItalic( TRUE );
    italicButton->setFont( italicButton_font ); 
    italicButton->setText( trUtf8( "I" ) );
    italicButton->setToggleButton( TRUE );
    italicButton->setAutoRaise( TRUE );
    QToolTip::add( italicButton, trUtf8( "Set the current selection to italic." ) );
    QWhatsThis::add( italicButton, trUtf8( "<p><b>Italic</b></p>\n"
"<p>Set (or unset) the current style of the annotation text to italic. This works two ways: if some portion of the text is highlighted, it will be rendered in italic face (or it will have the italic property removed). Otherwise, all text typed after the present position of the cursor will be italic (or  not).</p>" ) );
    Layout4->addWidget( italicButton );

    underlineButton = new QToolButton( this, "underlineButton" );
    QFont underlineButton_font(  underlineButton->font() );
    underlineButton_font.setUnderline( TRUE );
    underlineButton->setFont( underlineButton_font ); 
    underlineButton->setText( trUtf8( "U" ) );
    underlineButton->setToggleButton( TRUE );
    underlineButton->setAutoRaise( TRUE );
    QToolTip::add( underlineButton, trUtf8( "Set the current selection to underlined." ) );
    QWhatsThis::add( underlineButton, trUtf8( "<p><b>Underline</b></p>\n"
"<p>Set (or unset) the current style of the annotation text to underline. This works two ways: if some portion of the text is highlighted, it will be rendered with underlining (or it will have the underlining property removed). Otherwise, all text typed after the present position of the cursor will be underlined (or  not).</p>" ) );
    Layout4->addWidget( underlineButton );

    AnnotationInfoDialogLayout->addLayout( Layout4, 1, 0 );

    sizesComboBox = new QComboBox( FALSE, this, "sizesComboBox" );
    QToolTip::add( sizesComboBox, trUtf8( "Choose the size of the text." ) );
    QWhatsThis::add( sizesComboBox, trUtf8( "<p><b>Character Size</b></p>\n"
"<p>Set the current point size of the annotation text. (A <i>point</i> is 1/72\". Note that these are effectively <i>paper</i> units; that is, text will be rendered at the same size regardless of the scale of the view.) This works two ways: if some portion of the text is highlighted, it will be rendered at the given point size. Otherwise, all text typed after the present position of the cursor will be drawn at that point size.</p>" ) );

    AnnotationInfoDialogLayout->addWidget( sizesComboBox, 1, 1 );

    // signals and slots connections
    connect( buttonOk, SIGNAL( clicked() ), this, SLOT( accept() ) );
    connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
    connect( boldButton, SIGNAL( toggled(bool) ), this, SLOT( boldButton_toggled(bool) ) );
    connect( italicButton, SIGNAL( toggled(bool) ), this, SLOT( italicButton_toggled(bool) ) );
    connect( underlineButton, SIGNAL( toggled(bool) ), this, SLOT( underlineButton_toggled(bool) ) );
    connect( colorButton, SIGNAL( clicked() ), this, SLOT( colorButton_clicked() ) );
    connect( annotationTextEdit, SIGNAL( currentColorChanged(const QColor&) ), this, SLOT( annotationTextEdit_currentColorChanged(const QColor&) ) );
    connect( annotationTextEdit, SIGNAL( currentFontChanged(const QFont&) ), this, SLOT( annotationTextEdit_currentFontChanged(const QFont&) ) );
    connect( sizesComboBox, SIGNAL( activated(const QString&) ), this, SLOT( sizesComboBox_activated(const QString&) ) );
    connect( buttonHelp, SIGNAL( clicked() ), this, SLOT( buttonHelp_clicked() ) );

    // tab order
    setTabOrder( nameEdit, annotationTextEdit );
    setTabOrder( annotationTextEdit, sizesComboBox );
    setTabOrder( sizesComboBox, colorButton );
    setTabOrder( colorButton, buttonHelp );
    setTabOrder( buttonHelp, buttonOk );
    setTabOrder( buttonOk, buttonCancel );

    // buddies
    nameTextLabel->setBuddy( nameEdit );
    init();
}

/*  
 *  Destroys the object and frees any allocated resources
 */
AnnotationInfoDialog::~AnnotationInfoDialog()
{
    // no need to delete child widgets, Qt does it all for us
}

