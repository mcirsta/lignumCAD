/*
 * systemdependencies.cpp
 *
 * Functions which are dependent on operating system, graphics system, etc.
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
#ifndef SYSTEMDEPENDENCIES_H
#define SYSTEMDEPENDENCIES_H

class QString;
class QDir;

namespace System {
 /*!
  * OGLFT uses FreeType. FreeType Faces are located by file name.
  * Therefore, we need to derive a font file from the Qt QFont
  * object.
  * \param font a Qt font string representation (from QFont::toString()).
  * \param file returns the file name for font.
  * \param point_size returns the point size of font.
  * \return true if successful.
  */
  bool findFontFile ( const QString& font, QString& file, double& point_size );
  /*!
   * Until we a more general help browsing system built into lignumCAD,
   * we'll just call out to some external HTML browser.
   */
  void showManual ( const QDir& home_dir );
  /*!
   * Return the full path to the executable.
   */
  QString executablePath ( void );
} // end of System namespace 
#endif /* SYSTEMDEPENDENCIES_H */
