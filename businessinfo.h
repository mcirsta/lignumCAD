/* -*- c++ -*-
 * businessinfo.h
 *
 * Header for the BusinessInfo class.
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
#ifndef BUSINESSINFO_H
#define BUSINESSINFO_H

#include <qstring.h>

/*!
 * This is a global (singleton) object which contains the business
 * information entered in the Preferences dialog.
 */
class BusinessInfo {
  //! The name of the business (or the user)
  QString name_;
  //! The location of the business (usually city, state, but otherwise
  //! not parsed).
  QString location_;
  //! A logo for the business (SVG would be the best but we'll accomodate
  //! a pixmap, too.)
  QString logo_;
  //! The global Businessinfo object.
  static BusinessInfo business_info_;

protected:
  /*!
   * All of the defaults are just null strings.
   */
  BusinessInfo () {}

public:
  /*!
   * Access the business information through this global instance.
   * \return reference to the global business info object.
   */
  static BusinessInfo& instance ( void );
  /*!
   * Set the business name.
   * \param name business name.
   */
  void setName ( const QString& name ) { name_ = name; }
  /*!
   * \return the name of the business.
   */
  QString name ( void ) const { return name_; }
  /*!
   * Set the business location.
   * \param name business location.
   */
  void setLocation ( const QString& location ) { location_ = location; }
  /*!
   * \return the location of the business.
   */
  QString location ( void ) const { return location_; }
  /*!
   * Set the file name containing the business logo.
   * \param file file name containing the business logo.
   */
  void setLogo ( const QString& logo ) { logo_ = logo; }
  /*!
   * \return the business logo as a string.
   */
  QString logo ( void ) const { return logo_; }
};

#endif // BUSINESSINFO_H
