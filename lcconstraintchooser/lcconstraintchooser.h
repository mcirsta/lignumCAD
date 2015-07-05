/* -*- c++ -*-
 * lcconstraintchooser.h
 *
 * Header for the lCConstraintChooser classes
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
#ifndef LCCONSTRAINTCHOOSER_H
#define LCCONSTRAINTCHOOSER_H

#include <qdialog.h>

class QListView;
class QLineEdit;
class QPushButton;

class lCConstraintDialog : public QDialog 
{
  Q_OBJECT

  QListView* constraint_list_;
  QPushButton* help_button_;
  QPushButton* ok_button_;
  QPushButton* cancel_button_;
public:
  lCConstraintDialog ( QWidget* parent = 0, const char* name = 0, bool modal = true );
  QString constraint ( void ) const;
};

class lCConstraintChooser : public QWidget
{
  Q_OBJECT

  Q_PROPERTY( bool edited READ edited )

public:
  lCConstraintChooser( QWidget *parent = 0, const char *name = 0);

  QString constraint() const;
  bool edited() const;
  void setEdited ( bool edited ) const;

public slots:
  void setConstraint( const QString &constraint );

signals:
  void constraintChanged( const QString & );

private slots:
  void chooseConstraint();

private:
  QLineEdit *line_edit_;
  QPushButton *button_;
  lCConstraintDialog* constraint_dialog_;
};

#endif // LCCONSTRAINTCHOOSER_H
