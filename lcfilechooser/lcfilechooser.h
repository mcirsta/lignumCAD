/* -*- c++ -*-
 * lcfilechooser.h
 *
 * Header for the lCFileChooser classes
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
#ifndef LCFILECHOOSER_H
#define LCFILECHOOSER_H

class QLineEdit;
class QPushButton;

class lCFileChooser : public QHBox
{
    Q_OBJECT

    Q_PROPERTY( QString fileName READ fileName WRITE setFileName )
    Q_PROPERTY( bool edited READ edited )

public:
    lCFileChooser( QWidget *parent = 0, const char *name = 0);

    QString fileName() const;
    bool edited() const;
    void setEdited ( bool edited ) const;

public slots:
    void setFileName( const QString &fn );

signals:
    void fileNameChanged( const QString & );

private slots:
    void chooseFile();

private:
    QLineEdit *lineEdit;
    QPushButton *button;
};

#endif // LCFILECHOOSER_H
