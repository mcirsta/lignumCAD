/* -*- c++ -*-
 * newpartwizard.ui.h
 *
 * Header for NewPartWizard class
 * Copyright (C) 2002 lignum Computing, Inc. <lignumcad@lignumcomputing.com>
 * Copyright (C) 2026 Marius Cirsta <mforce2@gmail.com>
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
#include <QAbstractButton>
#include <QCursor>
#include <QGridLayout>
#include <QIcon>
#include <QScrollArea>
#include <QTreeWidgetItem>
#include <QVBoxLayout>
#include <QWidget>
#include <QWhatsThis>
#include <QWizardPage>

#include <vector>

void NewPartWizard::init()
{    
    scroll_area_ = 0;
    scroll_widget_ = 0;
    scroll_layout_ = 0;

    setOption( QWizard::HaveHelpButton, true );
    setStartId( pageId( initialPartPage ) );
    partParametersPage->setFinalPage( true );

    button( QWizard::NextButton )->setEnabled( false );
    button( QWizard::FinishButton )->setEnabled( false );
    
    const PartMetadataList& parts = PartFactory::instance()->parts();
    for ( PartMetadata* part : parts ) {
	QTreeWidgetItem* group = groups_.value( trC( part->group() ), 0 );
	if ( group == 0 ) {
	    group = new QTreeWidgetItem( partLibraryListView, QStringList( trC( part->group() ) ) );
	    group->setExpanded( true );
	    group->setFlags( group->flags() & ~Qt::ItemIsSelectable );
	    groups_.insert( trC( part->group() ), group);
	}
	QTreeWidgetItem* item = new QTreeWidgetItem( group, QStringList() << trC( part->name() ) << QString() );
	item->setExpanded( true );
	item->setIcon( 1, QIcon( part->icon() ) );
	parts_.insert( item, part );
    }
}

void NewPartWizard::partLibraryListView_currentChanged( QTreeWidgetItem* item )
{
    button( QWizard::NextButton )->setEnabled( item != 0 && parts_.contains( item ) );
}


void NewPartWizard::NewPartWizard_currentIdChanged( int )
{
    if ( currentPage() == initialPartPage )
	focusInitialPartPage();
    else if ( currentPage() == partParametersPage )
	focusPartParametersPage();
}


void NewPartWizard::focusInitialPartPage()
{
    partLibraryListView_currentChanged( partLibraryListView->currentItem() );
    partLibraryListView->setFocus();
}


void NewPartWizard::focusPartParametersPage()
{
    PartMetadata* part = selectedPart();
    if ( part == 0 ) {
	button( QWizard::FinishButton )->setEnabled( false );
	return;
    }

    button( QWizard::FinishButton )->setEnabled( false );
    partParameterFrame->setTitle( tr( "&Parameters for %1::%2" ).
				  arg( trC( part->group() ) ).
				  arg( lC::formatName( part->name() ) ) );

    uint n_parameters = part->parameterCount();
    if ( scroll_area_ == 0 ) {
	QGridLayout* layout = static_cast<QGridLayout*>( partParameterFrame->layout() );
	scroll_area_ = new QScrollArea( partParameterFrame );
	scroll_area_->setObjectName( "parameterScrollArea" );
	scroll_area_->setWidgetResizable( true );
	layout->addWidget( scroll_area_, 0, 0 );

	scroll_widget_ = new QWidget( scroll_area_ );
	scroll_widget_->setObjectName( "parameterWidget" );
	scroll_layout_ = new QVBoxLayout( scroll_widget_ );
	scroll_layout_->setContentsMargins( 0, 0, 0, 0 );
	scroll_layout_->setSpacing( 6 );
	scroll_area_->setWidget( scroll_widget_ );
    }

    for ( size_t n = labels_.size(); n < n_parameters; n++ ) {
	lCDefaultLengthConstraint* length_constraint =
		new lCDefaultLengthConstraint( scroll_widget_, "parameterLabel" );
	scroll_layout_->addWidget( length_constraint );
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
	labels_.push_back( length_constraint );
    }
    QStringList::const_iterator parameter = part->parameters();
    std::vector<lCDefaultLengthConstraint*>::iterator label = labels_.begin();
    parameter_labels_.clear();
    for ( uint n = 0; n < n_parameters; n++ ) {
	lCDefaultLengthConstraint* parameter_label = *label;
	parameter_label->setTitle( trC( *parameter ) );
	parameter_label->setDefaultLength( 0 );
	parameter_label->setLength( 0 );
	parameter_label->show();
	parameter_labels_.insert( *parameter, parameter_label );
	++parameter;
	++label;
    }
    for ( ; label != labels_.end(); ++label )
	(*label)->hide();

    scroll_area_->ensureVisible( 0, 0 );
    if ( n_parameters > 0 && !labels_.empty() )
	labels_.front()->setFocus();
}


const PartParameterMap& NewPartWizard::parameters( void )
{
    return parameter_labels_;
}


const PartMetadata* NewPartWizard::part( void )
{
    return selectedPart();
}


void NewPartWizard::updateValidity( double )
{
    // One of the parameter boxes was changed, so recheck the validity of the input.
    if ( part()->valid(parameter_labels_) )
	button( QWizard::FinishButton )->setEnabled( true );
}


void NewPartWizard::NewPartWizard_helpClicked()
{
    if ( currentPage() == initialPartPage ) {
	QWhatsThis::showText( QCursor::pos(), tr( "<p><b>Initial Part Page</b></p>\
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
no Part will be generated.</p>" ), this );
    }
    else if ( currentPage() == partParametersPage ) {
	QWhatsThis::showText( QCursor::pos(), tr( "<p><b>Part Parameters Page</b></p>\
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
no new Part will be created.</p></p>" ), this );
    }
}

bool NewPartWizard::validateCurrentPage()
{
    if ( currentPage() != initialPartPage )
	return QWizard::validateCurrentPage();

    // Check that no part already has this name.
    int ret = part_view_->parent()->uniquePageName( part_view_, nameEdit->text(), lC::STR::PART );
    switch ( ret ) {
	case lC::OK:
	    return true;
	case lC::Redo:
	    focusInitialPartPage();
	    return false;
	case lC::Rejected:
	    reject();
	    return false;
    }

    return false;
}


void NewPartWizard::showInitialPartPage()
{
    setStartId( pageId( initialPartPage ) );
    restart();
    NewPartWizard_currentIdChanged( currentId() );
}


void NewPartWizard::setPartView( PartView * part_view )
{
    part_view_ = part_view;
}


int NewPartWizard::pageId( const QWizardPage* page ) const
{
    const QList<int> ids = pageIds();
    for ( int id : ids ) {
	if ( this->page( id ) == page )
	    return id;
    }

    return -1;
}


PartMetadata* NewPartWizard::selectedPart() const
{
    return parts_.value( partLibraryListView->currentItem(), 0 );
}


QString NewPartWizard::trC( const QString & string )
{
  return qApp->translate( "Constants", string.toUtf8().constData() );
}
