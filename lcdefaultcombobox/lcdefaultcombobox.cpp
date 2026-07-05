/*
 * lcdefaultcombobox.cpp
 *
 * Implementation of lCDefaultComboBox class
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
#include <qcombobox.h>
#include <qtoolbutton.h>
#include <QHBoxLayout>
#include <QIcon>
#include <QPixmap>

#include "constants.h"
#include "lcdefaultcombobox.h"

lCDefaultComboBox::lCDefaultComboBox( QWidget *parent, const char *name )
  : QFrame( parent ), default_value_( 0 )
{
  setObjectName( name );

  QHBoxLayout* layout = new QHBoxLayout( this );
  layout->setContentsMargins( 0, 0, 0, 0 );
  layout->setSpacing( 0 );

  // A read only combo box (no reason for the user to type anything)
  combo_box_ = new QComboBox( this );
  combo_box_->setObjectName( "combobox" );
  combo_box_->setEditable( false );

  default_ = new QToolButton( this );
  default_->setObjectName( "default" );

  default_->setToolTip( tr( "Click this button to restore the default value" ) );

  QIcon icon;
  icon.addPixmap( QPixmap( ":/images/default_active.png" ), QIcon::Normal );
  icon.addPixmap( QPixmap( ":/images/default_inactive.png" ), QIcon::Disabled );
  default_->setIcon( icon );

  default_->setFixedWidth( default_->sizeHint().width() );
  default_->setFixedHeight( combo_box_->sizeHint().height()-2 );

  layout->addWidget( combo_box_ );
  layout->addWidget( default_ );

  connect( default_, SIGNAL( clicked() ), SLOT( chooseDefault() ) );
  connect( combo_box_, SIGNAL( activated(int) ), SLOT( updateIndex(int) ) );

  setFocusProxy( combo_box_ );
}

void lCDefaultComboBox::insertItem( const QString& text )
{
  combo_box_->addItem( text );

  if ( combo_box_->currentIndex() == default_value_ )
    default_->setEnabled( false );
}

void lCDefaultComboBox::insertItem( const QPixmap& pixmap, const QString& text )
{
  combo_box_->addItem( QIcon( pixmap ), text );

  if ( combo_box_->currentIndex() == default_value_ )
    default_->setEnabled( false );
}

void lCDefaultComboBox::setStringList( const QStringList& list )
{
  combo_box_->clear();

  combo_box_->addItems( list );

  if ( combo_box_->currentIndex() == default_value_ )
    default_->setEnabled( false );
}

void lCDefaultComboBox::setValue( int value )
{
  combo_box_->setCurrentIndex( value );

  if ( value == default_value_ )
    default_->setEnabled( false );
  else
    default_->setEnabled( true );
}

void lCDefaultComboBox::setDefaultValue( int value )
{
  default_value_ = value;

  combo_box_->setCurrentIndex( value );

  default_->setEnabled( false );
}

int lCDefaultComboBox::value() const
{
  return combo_box_->currentIndex();
}

void lCDefaultComboBox::chooseDefault ( void )
{
  combo_box_->setCurrentIndex( default_value_ );

  default_->setEnabled( false );

  emit valueChanged( default_value_ );
}

void lCDefaultComboBox::updateIndex ( int /*value*/ )
{
  default_->setEnabled( true );

  emit valueChanged( combo_box_->currentIndex() );
}
