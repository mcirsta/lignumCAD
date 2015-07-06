/* -*- c++ -*-
 * lcdefaultcombobox.h
 *
 * Header for the lCDefaultComboBox classes
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
#ifndef LCDEFAULTCOMBOBOX_H
#define LCDEFAULTCOMBOBOX_H


#include <QWidget>
#include <QComboBox>
#include <QToolButton>


//TODO horiz layout
class lCDefaultComboBox : public QWidget
{
  Q_OBJECT

public:
  lCDefaultComboBox( QWidget *parent = 0, const char *name = 0);

  void insertItem ( const QString& text );
  void insertItem ( const QPixmap& pixmap, const QString& text );
  void setStringList ( const QStringList& list );

  int value ( void ) const;

public slots:
  void setValue( int value );
  void setDefaultValue( int value );

signals:
  void valueChanged( int value );

private slots:
  void chooseDefault ( void ); 
  void updateIndex ( int ); 

private:
  QComboBox* combo_box_;
  QToolButton* default_;

  int default_value_;
};

#endif // LCDEFAULTCOMBOBOX_H
