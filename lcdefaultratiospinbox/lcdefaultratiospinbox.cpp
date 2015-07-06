/*
 * lcdefaultratiospinbox.cpp
 *
 * Implementation of lCDefaultRatioSpinBox class
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
#include <qtoolbutton.h>
#include <qtooltip.h>

#include "constants.h"
#include "lcdefaultratiospinbox.h"

lCRatioSpinBox::lCRatioSpinBox( QWidget *parent, const char *name )
  : QSpinBox( parent )
{
    setObjectName( name );
  setMinimum( -20 );
  setMaximum( 20 );
  //updateDisplay();
}

void lCRatioSpinBox::setRatio ( const Ratio& ratio )
{
  setValue( ratio.serial() );
}

Ratio lCRatioSpinBox::ratio ( void ) const
{
  return Ratio( value() );
}

QString lCRatioSpinBox::mapValueToText ( int value )
{
  Ratio ratio( value );
  return QString( "%1 : %2" ).arg( ratio.numerator() ).arg( ratio.denominator() );
}

int lCRatioSpinBox::mapTextToValue ( bool* ok )
{
  if ( ok != 0 )
    *ok = true;

  return minimum();
}

lCDefaultRatioSpinBox::lCDefaultRatioSpinBox( QWidget *parent, const char *name )
  : QWidget( parent )
{
    setObjectName( name );
  //setSpacing( 0 );

  spin_box_ = new lCRatioSpinBox( this );
  spin_box_->setObjectName( "spinbox" );

  default_ = new QToolButton( this );
  default_->setObjectName( "default" );

  default_->setObjectName(tr( "Click this button to restore the default value" ) );

  QIcon icon( lC::lookupPixmap( "default_active.png" ) );
  icon.addPixmap( lC::lookupPixmap( "default_inactive.png" ),
          QIcon::Disabled, QIcon::Off );
  default_->setIcon( icon );

  default_->setFixedWidth( default_->sizeHint().width() );
  default_->setFixedHeight( spin_box_->sizeHint().height()-2 );

  connect( default_, SIGNAL( clicked() ), this, SLOT( chooseDefault() ) );
  connect( spin_box_, SIGNAL( valueChanged(int) ), this,
	   SLOT( updateIndex(int) ) );

  setFocusProxy( spin_box_ );
}

void lCDefaultRatioSpinBox::setRatio ( const Ratio& ratio )
{
  spin_box_->setRatio( ratio );

  if ( spin_box_->ratio() == default_ratio_ )
    default_->setEnabled( false );
}

void lCDefaultRatioSpinBox::setDefaultRatio( const Ratio& ratio )
{
  default_ratio_ = ratio;

  spin_box_->setValue( default_ratio_.serial() );

  default_->setEnabled( false );
}

Ratio lCDefaultRatioSpinBox::ratio() const
{
  return spin_box_->ratio();
}

void lCDefaultRatioSpinBox::chooseDefault ( void )
{
  spin_box_->setValue( default_ratio_.serial() );

  default_->setEnabled( false );
}

void lCDefaultRatioSpinBox::updateIndex ( int /*value*/ )
{
  default_->setEnabled( true );

  emit valueChanged( spin_box_->ratio() );
}
