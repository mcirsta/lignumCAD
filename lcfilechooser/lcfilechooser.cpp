/* 
 * lcfilechooser.cpp
 *
 * Header for the lCFileChooser classes
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
#include "lcfilechooser.h"
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qfiledialog.h>

lCFileChooser::lCFileChooser( QWidget *parent, const char *name )
  : QFrame( parent )
{
  
  setObjectName( name );
  setFrameStyle( Panel | Sunken );
  setLineWidth( 2 );

  lineEdit = new QLineEdit( this );
  lineEdit->setObjectName( "filechooser_lineedit" );
  lineEdit->setFrame( false );
  lineEdit->setSizePolicy( QSizePolicy( QSizePolicy::Preferred,
   					QSizePolicy::Maximum ) );

  connect( lineEdit, SIGNAL( textChanged( const QString & ) ),
	   this, SIGNAL( fileNameChanged( const QString & ) ) );

  button = new QPushButton(  this );
  button->setObjectName( "filechooser_button" );
  button->setText("...");
  //  button->setSizePolicy( QSizePolicy( QSizePolicy::Preferred,
  //				      QSizePolicy::Maximum ) );
  button->setFixedWidth( button->fontMetrics().width( " ... " ) );
  button->setFixedHeight( lineEdit->sizeHint().height() );

  resize( 50, button->fontMetrics().height() );

  connect( button, SIGNAL( clicked() ), this, SLOT( chooseFile() ) );

  setFocusProxy( lineEdit );
}

bool lCFileChooser::edited ( void ) const
{
  return lineEdit->isModified();
}

void lCFileChooser::setEdited ( bool edited ) const
{
  lineEdit->setModified( edited );
}

void lCFileChooser::setFileName( const QString &fn )
{
  lineEdit->setText( fn );
}

QString lCFileChooser::fileName() const
{
  return lineEdit->text();
}

void lCFileChooser::chooseFile()
{
  QString file_name =
    QFileDialog::getSaveFileName( this,
                                  tr( "Save File " ),
                                  tr( "lignumCAD (*.lcad);;All Files (*)" ));

  if ( !file_name.isEmpty() ) {
    if ( file_name != lineEdit->text() ) {
      if ( !file_name.endsWith( ".lcad" ) )
	file_name += ".lcad";

      lineEdit->setText( file_name );
      lineEdit->setModified( true );

      emit fileNameChanged( file_name );
    }
  }
}
