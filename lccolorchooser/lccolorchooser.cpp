/*
 * lccolorchooser.cpp
 *
 * Implementation of lCColorChooser class
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
#include <qlabel.h>
#include <qpushbutton.h>
#include <qtoolbutton.h>
#include <qcolordialog.h>
#include <QHBoxLayout>
#include <QIcon>
#include <QPalette>
#include <QPixmap>

#include "constants.h"
#include "lccolorchooser.h"

lCColorChooser::lCColorChooser( QWidget *parent, const char *name )
  : QFrame( parent ), edited_( false )
{
  setObjectName( name );

  QHBoxLayout* layout = new QHBoxLayout( this );
  layout->setContentsMargins( 0, 0, 0, 0 );
  layout->setSpacing( 0 );

  setFrameStyle( Panel | Sunken );
  setLineWidth( 2 );

  color_label_ = new QLabel( this );
  color_label_->setObjectName( "label" );
  color_label_->setMinimumWidth( color_label_->fontMetrics().horizontalAdvance( "COLOR" ) );
  color_label_->setFixedHeight( color_label_->fontMetrics().lineSpacing() );
  color_label_->setFrameStyle( QFrame::Box | QFrame::Plain );

  button_ = new QPushButton( tr( "..." ), this );
  button_->setObjectName( "button" );
  button_->setFixedWidth( button_->fontMetrics().horizontalAdvance( "ABC" ) );

  default_ = new QToolButton( this );
  default_->setObjectName( "default" );

  default_->setToolTip( tr( "Click this button to restore the color to the default" ) );

  QIcon icon;
  icon.addPixmap( QPixmap( ":/images/default_active.png" ), QIcon::Normal );
  icon.addPixmap( QPixmap( ":/images/default_inactive.png" ), QIcon::Disabled );
  default_->setIcon( icon );

  default_->setFixedWidth( default_->sizeHint().width() );
  default_->setFixedHeight( button_->sizeHint().height()-2 );

  layout->addWidget( color_label_ );
  layout->addWidget( button_ );
  layout->addWidget( default_ );

  connect( button_, SIGNAL( clicked() ), this, SLOT( chooseColor() ) );
  connect( default_, SIGNAL( clicked() ), this, SLOT( chooseDefault() ) );

  setFocusProxy( button_ );
}

bool lCColorChooser::edited ( void ) const
{
  return edited_;
}

void lCColorChooser::setEdited ( bool edited )
{
  edited_ = edited;
}

void lCColorChooser::setColor( const QColor& color )
{
  color_ = color;
  updateColorLabel();

  edited_ = false;

  if ( color_ == default_color_ )
    default_->setEnabled( false );
}

void lCColorChooser::setDefaultColor ( QColor default_color )
{
  default_color_ = default_color;
}

QColor lCColorChooser::color() const
{
  return color_;
}

void lCColorChooser::chooseColor()
{
  QColor color = QColorDialog::getColor( color_, this );

  if ( color.isValid() ) {
    color_ = color;

    updateColorLabel();

    edited_ = true;

    if ( color_ != default_color_ )
      default_->setEnabled( true );
    else
      default_->setEnabled( false );

    emit colorChanged( color_ );
  }
}

void lCColorChooser::chooseDefault ( void )
{
  color_ = default_color_;

  updateColorLabel();
  
  edited_ = true;

  default_->setEnabled( false );

  emit colorChanged( color_ );
}

void lCColorChooser::updateColorLabel()
{
  QPalette palette = color_label_->palette();
  palette.setColor( QPalette::Window, color_ );
  color_label_->setAutoFillBackground( true );
  color_label_->setPalette( palette );
}
