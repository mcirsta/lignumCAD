/* -*- c++ -*-
 * dburl.h
 *
 * Header for the DBURL class
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
#ifndef DBURL_H
#define DBURL_H

#include <qurl.h>

/*!
 * The DBURL class is just a thin wrapper on the QUrl class. It's
 * helpful to be able to extract paths other than the last in
 * the URL representation of object paths, such as to extract the
 * parent path of an object. Also, makes the access
 * functions look less like URL components (e.g., name() instead of file()).
 */
class DBURL : public QUrl
{
public:
  /*!
   * In cases where the path is not known at construction time.
   */
  DBURL ( void )
  {}
  /*!
   * Constructor takes a URL-like path:
   * protocol:///model-dburl/parent-name.type/.../object-name.type
   * \param path URL-like descriptor of object.
   */
  DBURL ( const QString& path ) : QUrl ( path )
  {}
  /*!
   * The two part constructor (hopefully, will not try to ENCODE
   * the protocol:/// part of the url!)
   * \param url some rooted part of the URL (like just the protocol).
   * \param relURL rest of the URL, relative to first argument.
   */
  DBURL ( const QString& url, const QString& relURL )
  {
      QUrl rURL(relURL);
      if(rURL.isRelative()) {
          QUrl(url + relURL);
      }
      else {
          QUrl(relURL);
      }
  }
  /*!
   * \return the object-name (without the type).
   */
  QString name ( void ) const;
  /*!
   * \return the type of the object-name.type.
   */
  QString type ( void ) const;
  /*!
   * \return the DBURL of the object's parent.
   */
  DBURL parent ( void ) const;
};

#endif // DBURL_H
