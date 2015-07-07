/*
 * lcdefaultspinbox.cpp
 *
 * Implementation of lCDefaultSpinBox class
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
#include <qspinbox.h>
#include <qtoolbutton.h>
#include <qtooltip.h>

#include "constants.h"
#include "lcdefaultspinbox.h"

lCDefaultSpinBox::lCDefaultSpinBox( QWidget *parent, const char *name )
  : QWidget( parent )
{
    setObjectName( name );
  //setSpacing( 0 );

  spin_box_ = new QSpinBox( this );
  spin_box_->setObjectName( "spinbox" );

  default_ = new QToolButton( this );
  default_->setObjectName( "default" );

  default_->setToolTip( tr( "Click this button to restore the default value" ) );

  QIcon icon( lC::lookupPixmap( "default_active.png" ) );
  icon.addPixmap( lC::lookupPixmap( "default_inactive.png" ),
          QIcon::Disabled, QIcon::Off );
  default_->setIcon( icon );

  default_->setFixedWidth( default_->sizeHint().width() );
  default_->setFixedHeight( spin_box_->sizeHint().height()-2 );

  connect( default_, SIGNAL( clicked() ), this, SLOT( chooseDefault() ) );
  connect( spin_box_, SIGNAL( valueChanged( int ) ), this,
	   SLOT( updateValue( int ) ) );

  setFocusProxy( spin_box_ );
}

void lCDefaultSpinBox::setValue( int value )
{
  spin_box_->setValue( value );

  if ( spin_box_->value() == default_value_ )
    default_->setEnabled( false );
}

void lCDefaultSpinBox::setMinValue( int value )
{
  spin_box_->setMinimum( value );
}

void lCDefaultSpinBox::setMaxValue( int value )
{
  spin_box_->setMaximum( value );
}

void lCDefaultSpinBox::setDefaultValue( int value )
{
  default_value_ = value;

  spin_box_->setValue( default_value_ );

  default_->setEnabled( false );
}

int lCDefaultSpinBox::value() const
{
  return spin_box_->value();
}

int lCDefaultSpinBox::minValue() const
{
  return spin_box_->minimum();
}

int lCDefaultSpinBox::maxValue() const
{
  return spin_box_->maximum();
}

int lCDefaultSpinBox::defaultValue() const
{
  return default_value_;
}

void lCDefaultSpinBox::chooseDefault ( void )
{
  default_->setEnabled( false );

  spin_box_->setValue( default_value_ );
}

void lCDefaultSpinBox::updateValue ( int value )
{
  default_->setEnabled( true );

  emit valueChanged( value );
}
