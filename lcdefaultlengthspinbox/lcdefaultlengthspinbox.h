/* -*- c++ -*-
 * lcdefaultlengthspinbox.h
 *
 * Header for the lCDefaultLengthSpinBox classes
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
#ifndef LCDEFAULTLENGTHSPINBOX_H
#define LCDEFAULTLENGTHSPINBOX_H

#include <qspinbox.h>

#include "units.h"

class LengthUnit;
class QToolButton;

class lCLengthSpinBox : public QSpinBox
{
  Q_OBJECT

  const LengthUnit* length_unit_;
  double max_;
  double min_;
  enum UnitFormat format_;
  int precision_;

public:
  lCLengthSpinBox( QWidget* parent = 0, const char* name = 0);

  void setLengthUnit ( const LengthUnit* length_unit, enum UnitFormat format );
  void setLengthLimits ( const LengthUnit* length_unit, enum UnitFormat format,
			 double min, double max ); 
  void setLengthLimits ( const LengthUnit* length_unit, enum UnitFormat format,
			 int precision, double min, double max ); 
  void setLength ( double length );
  double length ( void ) const;
  double length ( int value ) const;
  int value ( double length ) const;

protected:
  QString mapValueToText ( int index );
  int mapTextToValue ( bool* ok );
};

class lCDefaultLengthSpinBox : public QHBox
{
  Q_OBJECT

  Q_PROPERTY( bool edited READ edited )

public:
  lCDefaultLengthSpinBox ( QWidget *parent = 0, const char *name = 0 );

  double length ( void ) const;
  bool edited ( void ) const;
  void setEdited ( bool edited );
  void setLengthUnit ( const LengthUnit* length_unit, enum UnitFormat format );
  void setLengthLimits ( const LengthUnit* length_unit, enum UnitFormat format,
			 double min, double max, double _default ); 
  void setLengthLimits ( const LengthUnit* length_unit, enum UnitFormat format,
			 int precision, double min, double max, double _default ); 

public slots:
  void setLength ( double length );
  void setDefaultLength ( double length );

signals:
  void valueChanged ( double length );

private slots:
  void chooseDefault ( void );
  void updateIndex ( int );

private:
  lCLengthSpinBox* spin_box_;
  QToolButton* default_;

  int default_index_;

  bool edited_;
};

#endif // LCDEFAULTSPINBOX_H
