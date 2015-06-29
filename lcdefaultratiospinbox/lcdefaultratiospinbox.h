/* -*- c++ -*-
 * lcdefaultratiospinbox.h
 *
 * Header for the lCDefaultRatioSpinBox classes
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
#ifndef LCDEFAULTRATIOSPINBOX_H
#define LCDEFAULTRATIOSPINBOX_H

#include <qspinbox.h>

#include "ratio.h"

class QToolButton;

class lCRatioSpinBox : public QSpinBox
{
  Q_OBJECT

public:
  lCRatioSpinBox( QWidget* parent = 0, const char* name = 0);

  void setRatio ( const Ratio& ratio );
  Ratio ratio ( void ) const;

protected:
  QString mapValueToText ( int index );
  int mapTextToValue ( bool* ok );
};

class lCDefaultRatioSpinBox : public QHBox
{
  Q_OBJECT

public:
  lCDefaultRatioSpinBox ( QWidget *parent = 0, const char *name = 0 );

public slots:
  void setRatio ( const Ratio& ratio );
  void setDefaultRatio ( const Ratio& ratio );
  Ratio ratio ( void ) const;

signals:
  void valueChanged ( const Ratio & ratio );

private slots:
  void chooseDefault ( void );
  void updateIndex ( int );

private:
  lCRatioSpinBox* spin_box_;
  QToolButton* default_;

  Ratio default_ratio_;
};

#endif // LCDEFAULTSPINBOX_H
