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

lCConstraintDialog::lCConstraintDialog ( QWidget* parent, const char* name,
					 bool modal )
  : QDialog( parent, name, modal )
{
  QGridLayout* dialog_layout_ = new QGridLayout( this, 1, 1, 11, 6, "grid_layout" );

  QHBoxLayout* layout = new QHBoxLayout( 0, 0, 6, "button_layout" );

  help_button_ = new QPushButton( this, "helpButton" );
  help_button_->setText( tr( "Help" ) );
  layout->addWidget( help_button_ );

  QSpacerItem* spacer = new QSpacerItem( 0, 0, QSizePolicy::Expanding,
					 QSizePolicy::Minimum );
  layout->addItem( spacer );

  ok_button_ = new QPushButton( this, "okButton" );
  ok_button_->setText( tr( "OK" ) );
  layout->addWidget( ok_button_ );

  cancel_button_ = new QPushButton( this, "cancelButton" );
  cancel_button_->setText( tr( "Cancel" ) );
  layout->addWidget( cancel_button_ );

  dialog_layout_->addLayout( layout, 1, 0 );

  constraint_list_ = new QListView( this, "constraint_list" );
  constraint_list_->addColumn( tr( "Name" ) );

  dialog_layout_->addWidget( constraint_list_, 0, 0 );

  connect( ok_button_, SIGNAL( clicked() ), this, SLOT( accept() ) );
  connect( cancel_button_, SIGNAL( clicked() ), this, SLOT( reject() ) );
}

QString lCConstraintDialog::constraint ( void ) const
{
  return "Feegle";
}

lCConstraintChooser::lCConstraintChooser( QWidget *parent, const char* name )
  : QHBox( parent, name )
{
  setSpacing( 0 );
  setFrameStyle( Panel | Sunken );
  setLineWidth( 2 );

  line_edit_ = new QLineEdit( this, "constraintchooser_lineedit" );
  line_edit_->setFrame( false );

  button_ = new QPushButton( "...", this, "constraintchooser_button" );
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
  return line_edit_->edited();
}

void lCConstraintChooser::setEdited ( bool edited ) const
{
  line_edit_->setEdited( edited );
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
	line_edit_->setEdited( true );

	emit constraintChanged( constraint );
      }
    }
  }
}
