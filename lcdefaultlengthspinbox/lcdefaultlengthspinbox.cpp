/*
 * lcdefaultlengthspinbox.cpp
 *
 * Implementation of lCDefaultLengthSpinBox class
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
#include <qlineedit.h>
#include <qtoolbutton.h>
#include <qtooltip.h>

#include "constants.h"
#include "lcdefaultlengthspinbox.h"

#include "lcdebug.h"

lCLengthSpinBox::lCLengthSpinBox( QWidget *parent, const char *name )
  : QSpinBox( parent, name ), length_unit_( 0 )
{
  setValidator( 0 );
  updateDisplay();
}

void lCLengthSpinBox::setLengthUnit ( const LengthUnit* length_unit,
				      enum UnitFormat format )
{
  length_unit_ = length_unit;
  format_ = format;
  precision_ = length_unit_->precisionTable( format_ )->count() - 1;

  updateDisplay();
}

void lCLengthSpinBox::setLengthLimits ( const LengthUnit* length_unit,
					enum UnitFormat format,
					double min, double max )
{
  length_unit_ = length_unit;
  format_ = format;
  precision_ = length_unit_->precisionTable( format_ )->count() - 1;

  min_ = length_unit_->round( format_, precision_, min );
  max_ = length_unit_->round( format_, precision_, max );
  double units_per_in = length_unit_->precision( format_, precision_)->unitsPerIn();
  setMinValue( (int)rint( min_ * units_per_in ) );
  setMaxValue( (int)rint( max_ * units_per_in ) );

  updateDisplay();
}

void lCLengthSpinBox::setLengthLimits ( const LengthUnit* length_unit,
					enum UnitFormat format,
					int precision,
					double min, double max )
{
  length_unit_ = length_unit;
  format_ = format;
  precision_ = precision;

  min_ = length_unit_->round( format_, precision_, min );
  max_ = length_unit_->round( format_, precision_, max );
  double units_per_in = length_unit_->precision( format_, precision_)->unitsPerIn();
  setMinValue( (int)rint( min_ * units_per_in ) );
  setMaxValue( (int)rint( max_ * units_per_in ) );

  updateDisplay();
}

void lCLengthSpinBox::setLength ( double length )
{
  if ( length_unit_ == 0 ) return;

  setValue( (int)rint( ( length - min_ ) / ( max_ - min_ ) *
		       ( maxValue() - minValue() ) + minValue() ) );
}

double lCLengthSpinBox::length ( void ) const
{
  if ( length_unit_ == 0 ) return 0;

  return length_unit_->round( format_, precision_,
			      (double)( QSpinBox::value() - minValue() ) /
			      ( maxValue() - minValue() ) *
			      ( max_ - min_ ) + min_ );
}

double lCLengthSpinBox::length ( int value ) const
{
  if ( length_unit_ == 0 ) return 0;

  return length_unit_->round( format_, precision_,
			      (double)( value - minValue() ) /
			      ( maxValue() - minValue() ) *
			      ( max_ - min_ ) + min_ );
}

int lCLengthSpinBox::value ( double length ) const
{
  if ( length_unit_ == 0 ) return 0;

  return (int)rint( ( length - min_ ) / ( max_ - min_ ) *
		    ( maxValue() - minValue() ) + minValue() );
}

QString lCLengthSpinBox::mapValueToText ( int value )
{
  if ( length_unit_ == 0 ) return tr( "no units" );
  // Until we can figure how to get the lCSymbols into Qt, have to take
  // the regular characters for a fraction (if so requested)...
  return length_unit_->format( format_, precision_, length( value ), false );
}

int lCLengthSpinBox::mapTextToValue ( bool* ok )
{
  if ( length_unit_ == 0 ) {
    *ok = false;
    return 0;
  }

  *ok = true;

  double length = length_unit_->parse( text(), format_, precision_ );

  if ( length >= min_ && length <= max_ )
    return value( length );
  else
    *ok = false;

  return minValue();
}

lCDefaultLengthSpinBox::lCDefaultLengthSpinBox( QWidget *parent, const char *name )
  : QHBox( parent, name ), edited_( false )
{
  setSpacing( 0 );

  spin_box_ = new lCLengthSpinBox( this, "spinbox" );

  default_ = new QToolButton( this, "default" );

  QToolTip::add( default_,
		 tr( "Click this button to restore the default value" ) );

  QIconSet icon( lC::lookupPixmap( "default_active.png" ) );
  icon.setPixmap( lC::lookupPixmap( "default_inactive.png" ),
		  QIconSet::Automatic, QIconSet::Disabled );
  default_->setIconSet( icon );

  default_->setFixedWidth( default_->sizeHint().width() );
  default_->setFixedHeight( spin_box_->sizeHint().height()-2 );

  connect( default_, SIGNAL( clicked() ), this, SLOT( chooseDefault() ) );
  connect( spin_box_, SIGNAL( valueChanged(int) ), this,
	   SLOT( updateIndex(int) ) );

  setFocusProxy( spin_box_ );
}

bool lCDefaultLengthSpinBox::edited ( void ) const
{
  return edited_;
}

void lCDefaultLengthSpinBox::setEdited ( bool edited )
{
  edited_ = edited;
}

void lCDefaultLengthSpinBox::setLengthUnit ( const LengthUnit* length_unit,
					     enum UnitFormat format )
{
  double old_length = length();

  spin_box_->setLengthUnit( length_unit, format );

  setLength( old_length );
}

void lCDefaultLengthSpinBox::setLengthLimits ( const LengthUnit* length_unit,
					       enum UnitFormat format,
					       double min, double max,
					       double _default )
{
  spin_box_->setLengthLimits( length_unit, format, min, max );

  setDefaultLength( _default );
}

void lCDefaultLengthSpinBox::setLengthLimits ( const LengthUnit* length_unit,
					       enum UnitFormat format,
					       int precision,
					       double min, double max,
					       double _default )
{
  spin_box_->setLengthLimits( length_unit, format, precision, min, max );

  setDefaultLength( _default );
}

void lCDefaultLengthSpinBox::setLength ( double length )
{
  spin_box_->setLength( length );

  edited_ = false;

  if ( spin_box_->QSpinBox::value() == default_index_ )
    default_->setEnabled( false );
}

void lCDefaultLengthSpinBox::setDefaultLength( double length )
{
  default_index_ = spin_box_->value( length );

  spin_box_->setValue( default_index_ );

  default_->setEnabled( false );
}

double lCDefaultLengthSpinBox::length() const
{
  return spin_box_->length();
}

void lCDefaultLengthSpinBox::chooseDefault ( void )
{
  spin_box_->setValue( default_index_ );

  edited_ = true;

  default_->setEnabled( false );
}

void lCDefaultLengthSpinBox::updateIndex ( int /*value*/ )
{
  default_->setEnabled( true );

  edited_ = true;

  emit valueChanged( spin_box_->length() );
}
