/* -*- c++ -*-
 * lcdefaultfilechooser.h
 *
 * Header for the lCDefaultFileChooser classes
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
#ifndef LCDEFAULTFILECHOOSER_H
#define LCDEFAULTFILECHOOSER_H

#include <QLineEdit>


//TODO horiz layout
class lCDefaultFileChooser : public QWidget
{
  Q_OBJECT

  Q_PROPERTY( QString fileName READ fileName WRITE setFileName )
  Q_PROPERTY( QString filter READ filter WRITE setFilter )
  Q_PROPERTY( bool edited READ edited )

public:
  lCDefaultFileChooser( QWidget *parent = 0, const char *name = 0);

  QString fileName ( void ) const;
  QString filter ( void ) const;
  bool edited ( void ) const;

  void setFilter ( const QString& filter );
  void setEdited ( bool edited );
  void setDefaultFileName ( const QString& file );

public slots:
  void setFileName( const QString &file );

signals:
  void fileNameChanged( const QString& );

private slots:
  void chooseFile();
  void chooseDefault(); 

private:
  QLineEdit* line_edit_;
  QPushButton* button_;
  QToolButton* default_;

  QString filter_;
  QString default_file_;
};

#endif // LCDEFAULTFILECHOOSER_H
