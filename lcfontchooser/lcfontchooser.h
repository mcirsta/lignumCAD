/* -*- c++ -*-
 * lcfontchooser.h
 *
 * Header for the lCFontChooser class
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
#ifndef LCFONTCHOOSER_H
#define LCFONTCHOOSER_H

#include <qfont.h>

class QLineEdit;
class QPushButton;
class QToolButton;

//TODO horiz layout
class lCFontChooser : public QWidget
{
  Q_OBJECT

  Q_PROPERTY( QString font READ font WRITE setFont )
  Q_PROPERTY( bool edited READ edited )

public:
  lCFontChooser( QWidget* parent = 0, const char* name = 0);

  QString font ( void ) const;
  bool edited ( void ) const;
  void setEdited ( bool edited );
  void setDefaultFont ( const QString& default_font );

public slots:
  void setFont( const QString& font );

signals:
  void fontChanged( const QString& );

private slots:
  void chooseFont();
  void chooseDefault();

private:
  QLineEdit* line_edit_;
  QPushButton* button_;
  QToolButton* default_;

  QString default_font_;
  QString font_;
};

#endif // LCFONTCHOOSER_H
