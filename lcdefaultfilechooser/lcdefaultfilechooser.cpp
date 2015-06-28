/*
 * lcdefaultfilechooser.cpp
 *
 * Implementation of lCDefaultFileChooser class
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
#include <qpushbutton.h>
#include <qtoolbutton.h>
#include <qfiledialog.h>
#include <qtooltip.h>

#include "constants.h"
#include "lcdefaultfilechooser.h"

lCDefaultFileChooser::lCDefaultFileChooser( QWidget *parent, const char *name )
  : QHBox( parent, name )
{
  setSpacing( 0 );
  setFrameStyle( Panel | Sunken );
  setLineWidth( 2 );

  line_edit_ = new QLineEdit( this, "lineedit" );
  line_edit_->setReadOnly( true );
  line_edit_->setFrame( false );

  button_ = new QPushButton( tr( "..." ), this, "button" );
  button_->setFixedWidth( button_->fontMetrics().width( "ABC" ) );

  default_ = new QToolButton( this, "default" );

  QToolTip::add( default_,
		 tr( "Click this button to restore the color to the default" ) );

  QIconSet icon( lC::lookupPixmap( "default_active.png" ) );
  icon.setPixmap( lC::lookupPixmap( "default_inactive.png" ),
		  QIconSet::Automatic, QIconSet::Disabled );
  default_->setIconSet( icon );

  default_->setFixedWidth( default_->sizeHint().width() );
  default_->setFixedHeight( button_->sizeHint().height()-2 );

  line_edit_->setFixedHeight( button_->sizeHint().height() );
#if 0
  connect( line_edit_, SIGNAL( textChanged( const QString & ) ),
	   this, SIGNAL( fileNameChanged( const QString & ) ) );
#endif
  connect( button_, SIGNAL( clicked() ), this, SLOT( chooseFile() ) );
  connect( default_, SIGNAL( clicked() ), this, SLOT( chooseDefault() ) );

  setFocusProxy( button_ );
}

bool lCDefaultFileChooser::edited ( void ) const
{
  return line_edit_->edited();
}

void lCDefaultFileChooser::setEdited ( bool edited )
{
  line_edit_->setEdited( edited );
}

void lCDefaultFileChooser::setFileName( const QString &file )
{
  line_edit_->setText( file );

  if ( file == default_file_ )
    default_->setEnabled( false );
}

void lCDefaultFileChooser::setFilter( const QString &filter )
{
  filter_ = filter;
}

void lCDefaultFileChooser::setDefaultFileName( const QString &file )
{
  default_file_ = file;

  line_edit_->setText( default_file_ );
}

QString lCDefaultFileChooser::fileName() const
{
  return line_edit_->text();
}

QString lCDefaultFileChooser::filter() const
{
  return filter_;
}

void lCDefaultFileChooser::chooseFile()
{
  QString file_name =
    QFileDialog::getOpenFileName( line_edit_->text(), tr( filter_ ), this );

  if ( !file_name.isEmpty() ) {
    if ( file_name != line_edit_->text() ) {

      line_edit_->setText( file_name );
      line_edit_->setEdited( true );

      if ( file_name != default_file_ )
	default_->setEnabled( true );
      else
	default_->setEnabled( false );

      emit fileNameChanged( file_name );
    }
  }
}

void lCDefaultFileChooser::chooseDefault ( void )
{
  line_edit_->setText( default_file_ );

  line_edit_->setEdited( true );

  default_->setEnabled( false );

  emit fileNameChanged( default_file_ );
}
