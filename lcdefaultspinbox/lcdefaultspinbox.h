/* -*- c++ -*-
 * lcdefaultspinbox.h
 *
 * Header for the lCDefaultSpinBox classes
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
#ifndef LCDEFAULTSPINBOX_H
#define LCDEFAULTSPINBOX_H

#include <qhbox.h>

class QSpinBox;
class QToolButton;

class lCDefaultSpinBox : public QHBox
{
  Q_OBJECT

  Q_PROPERTY( int value READ value WRITE setValue )
  Q_PROPERTY( int maxValue READ maxValue WRITE setMaxValue )
  Q_PROPERTY( int minValue READ minValue WRITE setMinValue )
  Q_PROPERTY( int defaultValue READ defaultValue WRITE setDefaultValue )

public:
  lCDefaultSpinBox( QWidget *parent = 0, const char *name = 0);

  int value ( void ) const;
  int maxValue ( void ) const;
  int minValue ( void ) const;
  int defaultValue ( void ) const;

public slots:
  void setValue ( int value );
  void setMaxValue ( int value );
  void setMinValue ( int value );
  void setDefaultValue ( int value );

signals:
  void valueChanged ( int value );

private slots:
  void chooseDefault ( void );
  void updateValue ( int );

private:
  QSpinBox* spin_box_;
  QToolButton* default_;

  int default_value_;
};

#endif // LCDEFAULTSPINBOX_H
