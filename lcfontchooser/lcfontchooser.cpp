/*
 * lcfontchooser.cpp
 *
 * Implementation of lCFontChooser class
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
#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qtoolbutton.h>
#include <qtooltip.h>
#include <qfontdialog.h>

#include "constants.h"
#include "lcfontchooser.h"

#include <QFrame>

lCFontChooser::lCFontChooser(QWidget *parent, const char *name )
  : QWidget( parent )
{
  setObjectName( name );
  //TODO
//  setSpacing( 0 );
//  setFrameStyle( Panel | Sunken );
//  setLineWidth( 2 );

  line_edit_ = new QLineEdit( this );
  line_edit_->setObjectName( "lineedit" );
  line_edit_->setReadOnly( true );
  line_edit_->setFrame( false );
  // Just pick something for starters
  font_ = line_edit_->font().toString();

  line_edit_->setText( tr( "%1-%2" )
		       .arg( line_edit_->font().family() )
		       .arg( line_edit_->font().pointSize() ) );
  line_edit_->setCursorPosition( 0 );

  button_ = new QPushButton( this );
  button_->setText(  "..." );
  button_->setObjectName( "button" );
  button_->setFixedWidth( button_->fontMetrics().width( "ABC" ) );

  default_ = new QToolButton( this );
  default_->setObjectName( "default" );

  default_->setToolTip( tr( "Click this button to restore the font to the default" ) );

  QIcon icon( lC::lookupPixmap( "default_active.png" ) );
  icon.addPixmap( lC::lookupPixmap( "default_inactive.png" ),
          QIcon::Disabled, QIcon::Off );
  default_->setIcon( icon );

  default_->setFixedWidth( default_->sizeHint().width() );
  default_->setFixedHeight( button_->sizeHint().height()-2 );

  line_edit_->setFixedHeight( button_->sizeHint().height() );

  connect( button_, SIGNAL( clicked() ), this, SLOT( chooseFont() ) );
  connect( default_, SIGNAL( clicked() ), this, SLOT( chooseDefault() ) );

  setFocusProxy( button_ );
}

bool lCFontChooser::edited ( void ) const
{
  return line_edit_->isModified();
}

void lCFontChooser::setEdited ( bool edited )
{
  line_edit_->setModified( edited );
}

void lCFontChooser::setFont( const QString& font )
{
  font_ = font;

  QFont qfont;

  if ( !font.isEmpty() )
    qfont.fromString( font );

  line_edit_->setText( tr( "%1-%2" )
		       .arg( qfont.family() )
		       .arg( qfont.pointSize() ) );

  line_edit_->setCursorPosition( 0 );

  if ( font_ == default_font_ )
    default_->setEnabled( false );
}

void lCFontChooser::setDefaultFont ( const QString& default_font )
{
  default_font_ = default_font;
}

QString lCFontChooser::font() const
{
  return font_;
}

void lCFontChooser::chooseFont()
{
  bool ok;

  QFont qfont;

  if ( !font_.isEmpty() )
    qfont.fromString( font_ );

  qfont = QFontDialog::getFont( &ok, qfont, this );

  if ( ok ) {
    font_ = qfont.toString();

    line_edit_->setText( tr( "%1-%2" )
			 .arg( qfont.family() )
			 .arg( qfont.pointSize() ) );
    line_edit_->setCursorPosition( 0 );
    line_edit_->setModified( true );

    if ( font_ != default_font_ )
      default_->setEnabled( true );
    else
      default_->setEnabled( false );

    emit fontChanged( font_ );
  }
}

void lCFontChooser::chooseDefault ( void )
{
  font_ = default_font_;

  QFont qfont;

  if ( !font_.isEmpty() )
    qfont.fromString( font_ );

  line_edit_->setText( tr( "%1-%2" )
		       .arg( qfont.family() )
		       .arg( qfont.pointSize() ) );
  line_edit_->setCursorPosition( 0 );
  line_edit_->setModified( true );

  default_->setEnabled( false );

  emit fontChanged( font_ );
}
