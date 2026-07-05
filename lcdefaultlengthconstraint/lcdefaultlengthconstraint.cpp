/*
 * lcdefaultlengthconstraint.cpp
 *
 * Implementation of lCDefaultLengthConstraint class
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
#include <QButtonGroup>
#include <QFrame>
#include <QGridLayout>
#include <QRadioButton>

#include "lcdefaultlengthspinbox/lcdefaultlengthspinbox.h"
#include "lcconstraintchooser/lcconstraintchooser.h"
#include "lcdefaultlengthconstraint.h"

lCDefaultLengthConstraint::lCDefaultLengthConstraint( QWidget *parent,
					      const char *name )
  : QGroupBox( parent ), button_group_( new QButtonGroup( this ) ),
    edited_( false )
{
  if ( name != 0 )
    setObjectName( QString::fromLatin1( name ) );

  setTitle( "Temporary label" );

  QGridLayout* g_layout = new QGridLayout( this );
  g_layout->setSpacing( 6 );
  g_layout->setContentsMargins( 11, 11, 11, 11 );

  specified_button_ = new QRadioButton( this );
  specified_button_->setText( tr( "Specified" ) );
  button_group_->addButton( specified_button_, 0 );

  specified_spin_box_ = new lCDefaultLengthSpinBox( this, "specifiedLength" );

  imported_button_ = new QRadioButton( this );
  imported_button_->setText( tr( "Imported" ) );
  button_group_->addButton( imported_button_, 1 );

  imported_constraint_chooser_ = new lCConstraintChooser( this, "constrainedLength" );

  QFrame* separator = new QFrame( this );
  separator->setObjectName( "separator" );
  separator->setFrameShape( QFrame::HLine );
  separator->setFrameShadow( QFrame::Sunken );

  g_layout->addWidget( specified_button_, 0, 0 );
  g_layout->addWidget( specified_spin_box_, 0, 1 );
  g_layout->addWidget( separator, 1, 0, 1, 2 );
  g_layout->addWidget( imported_button_, 2, 0 );
  g_layout->addWidget( imported_constraint_chooser_, 2, 1 );

  button_group_->setExclusive( true );
  connect( button_group_, SIGNAL( idClicked(int) ), SLOT( updateChooser(int) ) );
  connect( specified_spin_box_, SIGNAL( valueChanged(double) ),
	   this, SIGNAL( valueChanged(double ) ) );

  specified_button_->setChecked( true );

  // Only temporary...
  imported_button_->setEnabled( false );
  imported_constraint_chooser_->setEnabled( false );
}

bool lCDefaultLengthConstraint::edited ( void ) const
{
  if ( specified_button_->isChecked() )
    return specified_spin_box_->edited();
  else
    return imported_constraint_chooser_->edited();
}

void lCDefaultLengthConstraint::setEdited ( bool edited )
{
  specified_spin_box_->setEdited( edited );
  imported_constraint_chooser_->setEdited( edited );
}

bool lCDefaultLengthConstraint::isSpecified ( void ) const
{
  return specified_button_->isChecked();
}

void lCDefaultLengthConstraint::updateChooser ( int _id )
{
  if ( _id == button_group_->id( specified_button_ ) ) {
    specified_spin_box_->setEnabled( true );
    imported_constraint_chooser_->setEnabled( false );
  }
  else if ( _id == button_group_->id( imported_button_ ) ) {
    specified_spin_box_->setEnabled( false );
    imported_constraint_chooser_->setEnabled( true );
  }
}

void lCDefaultLengthConstraint::setLengthUnit ( const LengthUnit* length_unit,
						enum UnitFormat format )
{
  specified_spin_box_->setLengthUnit( length_unit, format );
}

void lCDefaultLengthConstraint::setLengthLimits ( const LengthUnit* length_unit,
						  enum UnitFormat format,
						  double min, double max,
						  double _default )
{
  specified_spin_box_->setLengthLimits( length_unit, format, min, max, _default );
}

void lCDefaultLengthConstraint::setLengthLimits ( const LengthUnit* length_unit,
						  enum UnitFormat format,
						  int precision,
						  double min, double max,
						  double _default )
{
  specified_spin_box_->setLengthLimits( length_unit, format, precision, min, max,
					_default );
}

void lCDefaultLengthConstraint::setLength ( double length )
{
  specified_spin_box_->setLength( length );
}

void lCDefaultLengthConstraint::setDefaultLength( double length )
{
  specified_spin_box_->setDefaultLength( length );
}

double lCDefaultLengthConstraint::specifiedLength() const
{
  return specified_spin_box_->length();
}

QString lCDefaultLengthConstraint::importedLength() const
{
  return imported_constraint_chooser_->constraint();
}

QString lCDefaultLengthConstraint::specifiedButtonToolTip ( void ) const
{
  return specified_button_->toolTip();
}

void lCDefaultLengthConstraint::setSpecifiedButtonToolTip ( const QString& tooltip )
{
  specified_button_->setToolTip( tooltip );
}

QString lCDefaultLengthConstraint::specifiedButtonWhatsThis ( void ) const
{
  return specified_button_->whatsThis();
}

void lCDefaultLengthConstraint::setSpecifiedButtonWhatsThis ( const QString& whatsthis )
{
  specified_button_->setWhatsThis( whatsthis );
}

QString lCDefaultLengthConstraint::specifiedSpinBoxToolTip ( void ) const
{
  return specified_spin_box_->toolTip();
}

void lCDefaultLengthConstraint::setSpecifiedSpinBoxToolTip ( const QString& tooltip)
{
  specified_spin_box_->setToolTip( tooltip );
}

QString lCDefaultLengthConstraint::specifiedSpinBoxWhatsThis ( void ) const
{
  return specified_spin_box_->whatsThis();
}

void lCDefaultLengthConstraint::setSpecifiedSpinBoxWhatsThis ( const QString& whatsthis )
{
  specified_spin_box_->setWhatsThis( whatsthis );
}
