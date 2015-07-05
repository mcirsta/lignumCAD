/* -*- c++ -*-
 * lcdefaultlengthconstraint.h
 *
 * Header for the lCDefaultLengthConstraint classes
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
#ifndef LCDEFAULTLENGTHCONSTRAINT_H
#define LCDEFAULTLENGTHCONSTRAINT_H

#include <QGroupBox>

#include "units.h"

class QRadioButton;
class lCDefaultLengthSpinBox;
class lCConstraintChooser;
class LengthUnit;
enum UnitFormat;

class lCDefaultLengthConstraint : public QGroupBox
{
  Q_OBJECT

  Q_PROPERTY( bool edited READ edited )
  Q_PROPERTY( QString specifiedButtonToolTip READ specifiedButtonToolTip
	      WRITE setSpecifiedButtonToolTip )
  Q_PROPERTY( QString specifiedButtonWhatsThis READ specifiedButtonWhatsThis
	      WRITE setSpecifiedButtonWhatsThis )
  Q_PROPERTY( QString specifiedSpinBoxToolTip READ specifiedSpinBoxToolTip
	      WRITE setSpecifiedSpinBoxToolTip )
  Q_PROPERTY( QString specifiedSpinBoxWhatsThis READ specifiedSpinBoxWhatsThis
	      WRITE setSpecifiedSpinBoxWhatsThis )

public:
  lCDefaultLengthConstraint ( QWidget *parent = 0, const char *name = 0 );

  bool edited ( void ) const;
  void setEdited ( bool edited );

  bool isSpecified ( void ) const;
  double specifiedLength ( void ) const;
  QString importedLength ( void ) const;
  void setLengthUnit ( const LengthUnit* length_unit, enum UnitFormat format );
  void setLengthLimits ( const LengthUnit* length_unit, enum UnitFormat format,
			 double min, double max, double _default ); 
  void setLengthLimits ( const LengthUnit* length_unit, enum UnitFormat format,
			 int precision, double min, double max, double _default ); 

  QString specifiedButtonToolTip ( void ) const;
  void setSpecifiedButtonToolTip ( const QString& text );
  QString specifiedButtonWhatsThis ( void ) const;
  void setSpecifiedButtonWhatsThis ( const QString& text );

  QString specifiedSpinBoxToolTip ( void ) const;
  void setSpecifiedSpinBoxToolTip ( const QString& text );
  QString specifiedSpinBoxWhatsThis ( void ) const;
  void setSpecifiedSpinBoxWhatsThis ( const QString& text );

public slots:
  void setLength ( double length );
  void setDefaultLength ( double length );

signals:
  void valueChanged ( double length );

private slots:
    void updateChooser ( int );

private:
  QRadioButton* specified_button_;
  QRadioButton* imported_button_;
  lCDefaultLengthSpinBox* specified_spin_box_;
  lCConstraintChooser* imported_constraint_chooser_;

  bool edited_;
};

#endif // LCDEFAULTLENGTHCONSTRAINT_H
