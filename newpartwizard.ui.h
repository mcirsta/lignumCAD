/* -*- c++ -*-
 * newpartwizard.ui.h
 *
 * Header for NewPartWizard class
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
void NewPartWizard::init()
{    
    scroll_view_ = 0;
    
    nextButton()->setEnabled( false );
    finishButton()->setEnabled( false );
    
    QPtrListIterator<PartMetadata> part = PartFactory::instance()->parts();
    for ( ; part.current() != 0; ++part ) {
	QListViewItem* group = groups_.find( trC( part.current()->group() ) );
	if ( group == 0 ) {
	    group = new QListViewItem( partLibraryListView, trC( part.current()->group() ) );
	    group->setOpen( true );
	    group->setSelectable( false );
	    groups_.insert( trC( part.current()->group() ), group);
	}
	QListViewItem* item = new QListViewItem( group, trC( part.current()->name() ) );
	item->setOpen( true );
	item->setPixmap( 1, lC::lookupPixmap( part.current()->icon() ) );
	parts_.insert( item, part.current() );
    }
    
    connect( nextButton(), SIGNAL( clicked() ), SLOT( validateName() ) );
}

void NewPartWizard::partLibraryListView_currentChanged( QListViewItem* item )
{
    if ( item != 0 ) {
	if ( groups_.find( item->text( 0 ) ) != 0 )
	    nextButton()->setEnabled( false );
	else
	    nextButton()->setEnabled( true );
    }
    else
	nextButton()->setEnabled( false );
}


void NewPartWizard::NewPartWizard_selected( const QString& /*page_name*/ )
{
    if ( currentPage() == initialPartPage ) {
	partLibraryListView_currentChanged( partLibraryListView->currentItem() );
	partLibraryListView->setFocus();
	nextButton()->setDefault( true );
    }
    else if ( currentPage() == partParametersPage ) {
	QListViewItem* item = partLibraryListView->currentItem();
#if 0	
	PartMetadata* part = PartFactory::instance()->part( item->parent()->text(0),
							    item->text( 0 ) );
#else
	PartMetadata* part = parts_[ item ];
#endif
	partParameterFrame->setTitle( tr( "&Parameters for %1::%2" ).
				      arg( trC( part->group() ) ).
				      arg( lC::formatName( part->name() ) ) );
	// Construct how ever many input parameter fields this part needs.
	uint n_parameters = part->parameterCount();
	if ( scroll_view_ == 0 ) {
	    QGridLayout* layout = new QGridLayout( partParameterFrame->layout() );
	    scroll_view_ = new QScrollView( partParameterFrame, "parameterScrollView" );
	    layout->addWidget( scroll_view_, 0, 0 );
	    scroll_vbox_ = new QVBox( scroll_view_->viewport(), "parameterVBox" );
	    scroll_view_->addChild( scroll_vbox_ );
	    scroll_view_->show();
	}
	
	for ( uint n = labels_.count(); n < n_parameters; n++ ) {
	    lCDefaultLengthConstraint* length_constraint =
		    new lCDefaultLengthConstraint( scroll_vbox_, "parameterLabel" );
	    length_constraint->setLengthLimits( UnitsBasis::instance()->lengthUnit(),
						UnitsBasis::instance()->format(),
						UnitsBasis::instance()->precision(),
						0,
						lC::MAXIMUM_DIMENSION, 0 );
	    length_constraint->setSpecifiedButtonToolTip( tr( "Use a specified size." ) );
	    length_constraint->setSpecifiedButtonWhatsThis( tr("<p><b>Specified Size</b></p> <p>Select this option if you want the parameter to have an independent, specified size.</p>" ) );
	    length_constraint->setSpecifiedSpinBoxToolTip( tr( "Enter the size." ) );
	    length_constraint->setSpecifiedSpinBoxWhatsThis( tr( "<p><b>Specified Size</b></p> <p>Enter the size of the parameter. The units of the value are given in the default units specified in the application Preferences (so, you do not have to enter the units abbreviation).</p> <p>If the default units format is DECIMAL, then the entered value can have the usual floating point representation, e.g.:</p> <p><code>1.234</code></p> <p>If the default units format is FRACTIONAL, then, in addition to the decimal format, any of the following representations can be typed in:</p> <p><code>1</code> (equals 1.0)</p> <p><code>1/2</code> (equals 0.5)</p> <p><code>1 1/2</code> (equals 1.5, note the blank separating the whole number and the fraction)</p> <p>If you modify the value, you can always go back to the default value when the dialog was opened by clicking the <img src=\"default_active.png\"> button</p>" ) );
	    connect( length_constraint, SIGNAL( valueChanged( double ) ),
		     this, SLOT(updateValidity( double ) ) );
	    labels_.append( length_constraint );
	}
	QStringList::const_iterator parameter = part->parameters();
	QPtrListIterator<lCDefaultLengthConstraint> label( labels_ );
	parameter_labels_.clear();
	for ( uint n = 0; n < n_parameters; n++ ) {
	    lCDefaultLengthConstraint* parameter_label = label.current();
	    parameter_label->setTitle( trC( *parameter ) );
	    parameter_label->setDefaultLength( 0 );
	    parameter_label->setLength( 0 );
	    parameter_label->show();
	    parameter_labels_.insert( *parameter, parameter_label );
	    ++parameter;
	    ++label;
	}
	for ( ; label.current() != 0; ++label ) {
	    label.current()->hide();
	}
	scroll_view_->ensureVisible( 0, 0 );
	if ( n_parameters > 0 && labels_.count() > 0 )
	    labels_.at(0)->setFocus();
	finishButton()->setDefault( true );
    }
}


const QDict<lCDefaultLengthConstraint>& NewPartWizard::parameters( void )
{
    return parameter_labels_;
}


const PartMetadata* NewPartWizard::part( void )
{
    QListViewItem* item = partLibraryListView->currentItem();
#if 0    
    return PartFactory::instance()->part( item->parent()->text(0), item->text( 0 ) );
#else
    return parts_[ item ];
#endif
}


void NewPartWizard::updateValidity( double )
{
    // One of the parameter boxes was changed, so recheck the validity of the input.
    if ( part()->valid(parameter_labels_) )
	finishButton()->setEnabled( true );
}


void NewPartWizard::NewPartWizard_helpClicked()
{
    if ( currentPage() == initialPartPage ) {
	QWhatsThis::display( tr( "<p><b>Initial Part Page</b></p>\
<p>Each Part is based on a three-dimensional solid geometry model. From this page, \
you can select the base solid geometry. The list shows the predefined solid templates. There \
are essentially two kinds of base solids: blanks and customized parts. The blanks \
include a regular rectangular parallelipiped, called a <em>board</em>, and a \
cylinder, called a <em>turning</em>. From these shapes, you can generate any \
other shape through the use of milling operations.</p>\
<p>The customized parts represent items which are you are more likely to buy pre-made. \
(Eventually, you will also be able to save your own creations as templates as well.)</p>\
<p>You should also set the name of the part (although you can change it later, too).</p>\
<p>When the fields are filled in to your satisfaction, \
click the <b>Next</b> button \
(or press <b>Enter</b> or <b>Alt+N</b>) to \
proceed to the next page.</p>\
<p>If you click the <b>Cancel</b> button \
(or press <b>ESC</b> or <b>Alt+C</b>), \
no Part will be generated.</p>" ) );
    }
    else if ( currentPage() == partParametersPage ) {
	QWhatsThis::display( tr( "<p><b>Part Parameters Page</b></p>\
<p>On this page, the parameters (dimensions or sizes) of the template solids are \
defined. When acceptable \
values are entered for all parameters, the <b>Finish</b> button will become \
active and the solid is ready to be generated. You can click that \
(or press <b>Enter</b> or <b>Alt+F</b>) and \
the new part will be created.</p>\
<p>You can also click <b>Back</b> \
(or press <b>Alt+B</b>) \
to change the base solid template. (Note that any values you have entered in \
the parameter input boxes will be forgotten when you return to this page, \
even if you select the same \
base solid template.)</p>\
<p>If you click <b>Cancel</b> \
(or press <b>ESC</b> or <b>Alt+C</b>), \
no new Part will be created.</p></p>" ) );
    }
}

void NewPartWizard::validateName( void )
{
    // Check that no part already has this name.
    int ret = part_view_->parent()->uniquePageName( part_view_, nameEdit->text(), lC::STR::PART );
    switch ( ret ) {
	case lC::Redo:
		showPage( initialPartPage );
	break;
	case lC::Rejected:
		reject();
    }
}


void NewPartWizard::setPartView( PartView * part_view )
{
    part_view_ = part_view;
}


QString NewPartWizard::trC( const QString & string )
{
  return qApp->translate( "Constants", string );
}
