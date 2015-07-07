/*
 * lcconstraintchooser.cpp
 *
 * Implementation of the lCConstraintChooser classes
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
#include <qlayout.h>
#include <qlistview.h>

#include "lcconstraintchooser.h"

#include <QListWidget>

lCConstraintDialog::lCConstraintDialog ( QWidget* parent, const char* name,
					 bool modal )
  : QDialog( parent )
{
  setObjectName( name );
  setModal( modal );
  QGridLayout* dialog_layout_ = new QGridLayout( this);  //1, 1 );
  dialog_layout_->setSpacing(  6 );
  dialog_layout_->setMargin( 11 );
  dialog_layout_->setObjectName( "grid_layout" );

  QHBoxLayout* layout = new QHBoxLayout( 0 );
  layout->setMargin( 0 );
  layout->setSpacing( 6 );
  layout->setObjectName( "button_layout" );

  help_button_ = new QPushButton( this );
  help_button_->setText( "helpButton" );
  help_button_->setText( tr( "Help" ) );
  layout->addWidget( help_button_ );

  QSpacerItem* spacer = new QSpacerItem( 0, 0, QSizePolicy::Expanding,
					 QSizePolicy::Minimum );
  layout->addItem( spacer );

  ok_button_ = new QPushButton( this );
  ok_button_->setText( "okButton" );
  ok_button_->setText( tr( "OK" ) );
  layout->addWidget( ok_button_ );

  cancel_button_ = new QPushButton( this );
  cancel_button_->setText( "cancelButton" );
  cancel_button_->setText( tr( "Cancel" ) );
  layout->addWidget( cancel_button_ );

  dialog_layout_->addLayout( layout, 1, 0 );

  constraint_list_ = new QListWidget( this );
  constraint_list_->setObjectName( "constraint_list" );
  //TODO
  //constraint_list_->addColumn( tr( "Name" ) );

  dialog_layout_->addWidget( constraint_list_, 0, 0 );

  connect( ok_button_, SIGNAL( clicked() ), this, SLOT( accept() ) );
  connect( cancel_button_, SIGNAL( clicked() ), this, SLOT( reject() ) );
}

QString lCConstraintDialog::constraint ( void ) const
{
  return "Feegle";
}

lCConstraintChooser::lCConstraintChooser( QWidget *parent, const char* name )
  : QWidget( parent ) // TODO QHbox
{
  //setSpacing( 0 );
 // setFrameStyle( Panel | Sunken );
 // setLineWidth( 2 );

    setObjectName( name );
  line_edit_ = new QLineEdit( this );
  line_edit_->setObjectName( "constraintchooser_lineedit" );
  line_edit_->setFrame( false );

  button_ = new QPushButton( this );
  button_->setObjectName( "constraintchooser_button" );
  button_->setText ( "..." );
  button_->setFixedWidth( button_->fontMetrics().width( "ABC" ) );

  line_edit_->setFixedHeight( button_->sizeHint().height() );

  constraint_dialog_ = new lCConstraintDialog( this, "constraint_dialog" );

  connect( line_edit_, SIGNAL( textChanged( const QString & ) ),
	   this, SIGNAL( constraintChanged( const QString & ) ) );
  connect( button_, SIGNAL( clicked() ), this, SLOT( chooseConstraint() ) );

  setFocusProxy( button_ );
}

bool lCConstraintChooser::edited ( void ) const
{
  return line_edit_->isModified();
}

void lCConstraintChooser::setEdited ( bool edited ) const
{
  line_edit_->setModified( edited );
}

void lCConstraintChooser::setConstraint( const QString &constraint )
{
  line_edit_->setText( constraint );
}

QString lCConstraintChooser::constraint() const
{
  return line_edit_->text();
}

void lCConstraintChooser::chooseConstraint()
{
  // Here is where we should populate the list view by some traversal of the
  // model tree...

  int result = constraint_dialog_->exec();

  if ( result == QDialog::Accepted ) {
    QString constraint = constraint_dialog_->constraint();

    if ( !constraint.isEmpty() ) {
      if ( constraint != line_edit_->text() ) {

	line_edit_->setText( constraint );
    line_edit_->setModified( true );

	emit constraintChanged( constraint );
      }
    }
  }
}
