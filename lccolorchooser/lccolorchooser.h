/* -*- c++ -*-
 * lccolorchooser.h
 *
 * Header for the lCColorChooser class
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
#ifndef LCCOLORCHOOSER_H
#define LCCOLORCHOOSER_H

#include <qhbox.h>
#include <qcolor.h>

class QLabel;
class QPushButton;
class QToolButton;

class lCColorChooser : public QHBox
{
  Q_OBJECT

  Q_PROPERTY( QColor color READ color WRITE setColor )
  Q_PROPERTY( bool edited READ edited )

public:
  lCColorChooser( QWidget* parent = 0, const char* name = 0);

  QColor color ( void ) const;
  bool edited ( void ) const;
  void setEdited ( bool edited );
  void setDefaultColor ( QColor default_color );

public slots:
  void setColor( const QColor& color );

signals:
  void colorChanged( const QColor& );

private slots:
  void chooseColor();
  void chooseDefault();

private:
  QLabel* color_label_;
  QPushButton* button_;
  QToolButton* default_;

  QColor color_;
  QColor default_color_;
  bool edited_;
};

#endif // LCCOLORCHOOSER_H
