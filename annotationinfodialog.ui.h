/* -*- c++ -*-
 * annotationinfodialog.ui.h
 *
 * Header for the AnnotationInfoDialog class
 * Copyright (C) 2002 lignum Computing, Inc. <lignumcad@lignumcomputing.com>
 * $Id$
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */
/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename slots use Qt Designer which will
** update this file, preserving your code. Create an init() slot in place of
** a constructor, and a destroy() slot in place of a destructor.
*****************************************************************************/
/*!
 * Initialize the Annotation information dialog.
 */
void AnnotationInfoDialog::init()
{
    int width = boldButton->sizeHint().width();
    width = QMAX( width, boldButton->sizeHint().height() );
    width = QMAX( width, italicButton->sizeHint().width() );
    width = QMAX( width, italicButton->sizeHint().height() );
    width = QMAX( width, underlineButton->sizeHint().width() );
    width = QMAX( width, underlineButton->sizeHint().height() );
    
    boldButton->setFixedSize( width, width );
    italicButton->setFixedSize( width, width );
    underlineButton->setFixedSize( width, width );
    
    QFont font;
    if ( !OpenGLGlobals::instance()->annotationFont().isEmpty() )
	font.fromString( OpenGLGlobals::instance()->annotationFont() );
    
    annotationTextEdit->setFont( font );
    
    QValueList<int> sizes = QFontDatabase::standardSizes();
    QValueList<int>::Iterator it = sizes.begin();
    for ( ; it != sizes.end(); ++it )
        sizesComboBox->insertItem( QString::number( *it ) );
    
    QPixmap sample( 24, 24 );
    sample.fill( annotationTextEdit->colorGroup().text() );
    colorButton->setPixmap( sample );   
}

/*!
 * Set the bold attribute of the selected text. Also makes this the current
 * state of the bold  attribute.
 * \param bold new bold attribute.
 */
void AnnotationInfoDialog::boldButton_toggled( bool bold )
{
    annotationTextEdit->setBold( bold );
}

/*!
 * Set the italic attribute of the selected text. Also makes this the current
 * state of the italic attribute.
 * \param italic new italic attribute.
 */
void AnnotationInfoDialog::italicButton_toggled( bool italic )
{
    annotationTextEdit->setItalic( italic );
}

/*!
 * Set the underline attribute of the selected text. Also makes this the
 * current state of the underline attribute.
 * \param underline new underline attribute.
 */
void AnnotationInfoDialog::underlineButton_toggled( bool underline )
{
    annotationTextEdit->setUnderline( underline );
}

/*!
 * Set the point size of the selected text. Also makes this the current
 * state of the font size.
 * \param value new point size.
 */
void AnnotationInfoDialog::pointSizeSpinBox_valueChanged( int value )
{
    annotationTextEdit->setPointSize(value);
}

/*!
 * Set the color of the selected text. Runs the standard Qt color selector.
 * Also makes this the current state of the color attribute.
 */
void AnnotationInfoDialog::colorButton_clicked()
{
    QColor color = QColorDialog::getColor( annotationTextEdit->color(), this );
    annotationTextEdit->setColor( color );
    QPixmap sample( 24, 24);
    sample.fill( color );
    colorButton->setPixmap( sample );   
}

/*!
 * This is invoked when the text edit cursor enters a region of text
 * with a different color from the current color.
 * \param color current color at cursor.
 */
void AnnotationInfoDialog::annotationTextEdit_currentColorChanged( const QColor & color )
{
    QPixmap sample( 24, 24 );
    sample.fill( color );
    colorButton->setPixmap( sample );
}


/*!
 * This is invoked when the text edit cursor enters a region of
 * different attributes from the current attributes.
 * \param font current font at cursor.
 */
void AnnotationInfoDialog::annotationTextEdit_currentFontChanged( const QFont & font )
{
    boldButton->setOn( font.bold() );
    italicButton->setOn( font.italic() );
    underlineButton->setOn( font.underline() );
    sizesComboBox->setCurrentText( QString::number( font.pointSize() ) );
}

/*!
 * This is invoked when the user changes the font point size.
 * \param text point size text from combo box.
 */
void AnnotationInfoDialog::sizesComboBox_activated( const QString & text )
{
    annotationTextEdit->setPointSize( text.toInt() );
}

/*!
 * Display brief help message for annotation text editor.
 */
void AnnotationInfoDialog::buttonHelp_clicked()
{
   QWhatsThis::display( tr("<p><b>Annotation Information</b></p>\
<p>This dialog allows you to edit selected properties of a \
text annotation. These include:\
<ul>\
<li>Name</li>\
<li>Text (including font size, color and style)</li>\
</ul></p>\
<p>After the properties are adjusted as desired, click on the \
<b>OK</b> button (or press <b>Enter</b> or <b>Alt+O</b>) \
to accept your changes. Clicking the <b>Cancel</b> button \
(or pressing <b>ESC</b> or <b>Alt+C</b>) will exit the \
dialog without applying the changes.</p>" ) );

}
