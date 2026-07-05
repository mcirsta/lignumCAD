/*
 * dburl.cpp
 *
 * DBURL class: A URL-like representation object names.
 * Copyright (C) 2002 lignum Computing, Inc. <lignumcad@lignumcomputing.com>
 * Copyright (C) 2026 Marius Cirsta <mforce2@gmail.com>
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
#include "dburl.h"

namespace {

bool isHexDigit ( QChar ch )
{
  return ch.isDigit() ||
    ( ch >= 'a' && ch <= 'f' ) ||
    ( ch >= 'A' && ch <= 'F' );
}

bool isPercentEscape ( const QString& text, qsizetype index )
{
  return index + 2 < text.size() &&
    text[index] == '%' &&
    isHexDigit( text[index + 1] ) &&
    isHexDigit( text[index + 2] );
}

void appendPercentEncodedText ( QString& encoded_path, QString& text )
{
  if ( text.isEmpty() )
    return;

  encoded_path += QString::fromLatin1( QUrl::toPercentEncoding( text ) );
  text.clear();
}

} // end of anonymous namespace

QString DBURL::encodedPath ( const QString& path )
{
  QString encoded_path;
  QString text;

  for ( qsizetype i = 0; i < path.size(); ++i ) {
    if ( path[i] == '/' ) {
      appendPercentEncodedText( encoded_path, text );
      encoded_path += '/';
    }
    else if ( isPercentEscape( path, i ) ) {
      appendPercentEncodedText( encoded_path, text );
      encoded_path += path.mid( i, 3 );
      i += 2;
    }
    else
      text += path[i];
  }

  appendPercentEncodedText( encoded_path, text );

  return encoded_path;
}

QString DBURL::toString ( bool encoded_path ) const
{
  if ( encoded_path )
    return QUrl::toString( QUrl::FullyEncoded );

  return QUrl::toString();
}

QString DBURL::name ( void ) const
{
  QString name = path().section( '/', -1 );
  return name.left( name.lastIndexOf( '.' ) );
}

QString DBURL::type ( void ) const
{
  QString name = path().section( '/', -1 );
  return name.right( name.length() - name.lastIndexOf( '.' ) - 1 );
}

DBURL DBURL::parent ( void ) const
{
  DBURL parent( *this );
  QString parent_path = path();
  parent_path.truncate( parent_path.lastIndexOf( '/' ) );
  parent.setPath( parent_path );
  return parent;
}
