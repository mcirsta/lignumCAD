/* -*- c++ -*-
 * newmodelwizard.h
 *
 * Header for the NewModelWizard classes
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
void NewModelWizard::init()
{
    // Alright, admittedly this is kind of convoluted. The purpose of this complexity is
    // to allow page types to be added or deleted without having to look a dozen places
    // in the code for where to add or delete switches. So, we dynamically construct
    // radio buttons for each page type. The (invisible) button group makes them exclusive.
    
    initialPageGroupBox->setColumnLayout( 0, Qt::Vertical );
    
    initialPageButtonGroup = new QButtonGroup( this, "pageButtonGroup" );
    initialPageButtonGroup->hide();
    // Aquire the list of id's which identify the various page types. This list should be ordered
    // in a "logical" order.
    QValueVector<uint> page_ids = PageFactory::instance()->pageIDs();
    
    QValueVector<uint>::const_iterator id = page_ids.begin();
    for ( ; id != page_ids.end(); ++id ) {
	QString text = qApp->translate( "lignumCADMainWindow",
					PageFactory::instance()->selection( *id ) );
	// Note: radio buttons are a child of the GroupBox
	QRadioButton* button = new QRadioButton( text, initialPageGroupBox );
	// But their exclusivity is controlled by the ButtonGroup (this is entirely so
	// that they can use their own id's).
	initialPageButtonGroup->insert( button, *id );
	QToolTip::add( button,
		       qApp->translate( "lignumCADMainWindow",
					PageFactory::instance()->toolTip( *id ) ) );
	QWhatsThis::add( button,
			 qApp->translate( "lignumCADMainWindow",
					  PageFactory::instance()->whatsThis( *id ) ) );
	initialPageGroupBox->layout()->add( button );
	// Keep a list of these around since QButtonGroup doesn't have an iterator and our
	// ids are not necessarily sequential.
	initialPageRadioButtons.append( button );
    }
    
    connect( initialPageButtonGroup, SIGNAL( clicked(int) ), SLOT(initialPageSelected() ) );
}
/*!
  * This is a little gloss on entering a new model name. The file name string is
  * updated as the model name is typed.
  * \param text current text of model name widget.
  */
void NewModelWizard::modelNameEdit_textChanged( const QString & text )
{
  modelFileChooser->setFileName( text + ".lcad" );
  modelFileChooser->setEdited( true );
}

/*!
 * Allow the user to finish the new model wizard now.
 */
void NewModelWizard::initialPageSelected( void )
{
  finishButton()->setEnabled( true );
  finishButton()->setDefault( true );
}

/*!
 * Improve the response to the keyboard by manipulating the focus.
 * The focus can get stuck in the description TextEdit widget. (The page
 * title parameter is ignored.)
 */
void NewModelWizard::NewModelWizard_selected( const QString & )
{
  if ( currentPage() == NewModelPage ) {
    modelNameEdit->setFocus();
    nextButton()->setDefault( true );
  }
  else if ( currentPage() == InitialPagePage ) {
    initialPageButtonGroup->setFocus();
    finishButton()->setDefault( true );
  }
}


void NewModelWizard::NewModelWizard_helpClicked()
{
    if ( currentPage() == NewModelPage ) {
	QWhatsThis::display( tr( "<p><b>New Model Page</b></p>\
<p>Enter the basic information about the model. \
(This is sometimes called the <i>metadata</i>.) \
The metadata includes:\
<ul>\
<li>Model name</li>\
<li>File to save the model in (defaults to <i>ModelName</i>.lcad)</li>\
<li>Initial version and revision numbers</li>\
<li>Optional description of the model</li>\
<li>Date and time of creation</li>\
<li>Date and time of last modification</li>\
</ul>\
The creation and last modification times \
are updated automatically.</p>\
<p>When the fields are filled in to your satisfaction, \
click the <b>Next</b> button \
(or press <b>Enter</b> or <b>Alt+N</b>) to \
proceed to the next page.</p>\
<p>If you click the <b>Cancel</b> button \
(or press <b>ESC</b> or <b>Alt+C</b>), \
no new model will be created.</p>" ) );
    }
    else if ( currentPage() == InitialPagePage ) {
	QWhatsThis::display( tr( "<p><b>Initial Page</b></p>\
<p><i>lignumCAD</i> requires that you start your \
model with at least one page. Typically, you would \
start with a few sketches to establish the overall \
dimensions of the model; but, you are free to \
start with any page type.</p>\
<p>After you choose a page type \
from the list, the <b>Finish</b> button will be activated. \
You can click that \
(or press <b>Enter</b> or <b>Alt+F</b>) and \
the new model with the specified initial page \
will be created.</p>\
<p>You can also click <b>Back</b> \
(or press <b>Alt+B</b>) \
to edit the model information some more.</p>\
<p>If you click <b>Cancel</b> \
(or press <b>ESC</b> or <b>Alt+C</b>), \
no new model will be created.</p>" ) );
    }
}

/*!
  * Evidently, this is the only way to introduce a public method into a Designer
  * dialog. Would prefer to return the value, but we'll have to settle for the
  * side-effect instead.
  * \param type set to the name of the selected initial page type.
  */
void NewModelWizard::selectedPage( uint & type )
{
    QPtrListIterator<QRadioButton> rb( initialPageRadioButtons );
    for ( ; rb.current() != 0; ++rb ) {
	if ( (*rb)->isChecked() ) {
	    type = initialPageButtonGroup->id( *rb );
	    return;
	}
    }
    // Should not happen.
   type = 0;
}

/*!
  * Turn off any page radio buttons which happen to be on.
  */
void NewModelWizard::unsetInitialPages( void )
{
    QPtrListIterator<QRadioButton> rb( initialPageRadioButtons );
    for ( ; rb.current() != 0; ++rb )
	(*rb)->setChecked( false );   
}
