/* -*- c++ -*-
 * usersettings.h
 *
 * Strings for user preferences.
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
#ifndef USERSETTINGS_H
#define USERSETTINGS_H
// These are the strings which key the user's preferences in QSettings.
namespace lC {
  namespace Setting {

    const QString HOME = "/lignumComputing/lignumCAD/Home";

    const QString DOCK_GEOMETRIES = "/lignumComputing/lignumCAD/Geometries/Layout";

    namespace Business {
      const QString NAME = "/lignumComputing/lignumCAD/Default/Business/Name";
      const QString LOCATION="/lignumComputing/lignumCAD/Default/Business/Location";
      const QString LOGO = "/lignumComputing/lignumCAD/Default/Business/Logo";
    } // end of Business namespace

    const QString LENGTH_UNIT = "/lignumComputing/lignumCAD/Default/LengthUnit";

    namespace ColorScheme {
      const QString BASE = "/lignumComputing/lignumCAD/Default/ColorScheme/Base";
      const QString CUSTOM ="/lignumComputing/lignumCAD/Default/ColorScheme/Custom";
      const QString GEOMETRY =
	"/lignumComputing/lignumCAD/Default/ColorScheme/Geomtry";
      const QString ANNOTATION =
	"/lignumComputing/lignumCAD/Default/ColorScheme/Annotation";
      const QString GRID ="/lignumComputing/lignumCAD/Default/ColorScheme/Grid";
      const QString CONSTRAINT_PRIMARY =
	"/lignumComputing/lignumCAD/Default/ColorScheme/ConstraintPrimary";
      const QString CONSTRAINT_SECONDARY =
	"/lignumComputing/lignumCAD/Default/ColorScheme/ConstraintSecondary";
      const QString BACKGROUND_STYLE =
	"/lignumComputing/lignumCAD/Default/ColorScheme/BackgroundStyle";
      const QString BACKGROUND =
	"/lignumComputing/lignumCAD/Default/ColorScheme/Background";
      const QString GRADIENT =
	"/lignumComputing/lignumCAD/Default/ColorScheme/Gradient";
      const QString PATTERN_FILE =
	"/lignumComputing/lignumCAD/Default/ColorScheme/PatternFile";
    } // end of ColorScheme namespace

    namespace Dimension {
      const QString FONT = "/lignumComputing/lignumCAD/Default/Dimension/Font";
      const QString ARROW_HEAD_LENGTH =
	"/lignumComputing/lignumCAD/Default/Dimension/ArrowHeadLength";
      const QString ARROW_HEAD_WIDTH_RATIO =
	"/lignumComputing/lignumCAD/Default/Dimension/ArrowHeadWidthRatio";
      const QString ARROW_HEAD_STYLE =
	"/lignumComputing/lignumCAD/Default/Dimension/ArrowHeadStyle";
      const QString CLEARANCE =
	"/lignumComputing/lignumCAD/Default/Dimension/Clearance";
      const QString LINE_THICKNESS =
	"/lignumComputing/lignumCAD/Default/Dimension/LineThickness";
      const QString EXTENSION_LINE_OFFSET =
	"/lignumComputing/lignumCAD/Default/Dimension/ExtensionLineOffset";
    } // end of Dimension namespace

    namespace Annotation {
      const QString FONT = "/lignumComputing/lignumCAD/Default/Annotation/Font";
    } // end of Annotation namespace

    namespace Handle {
      const QString SIZE = "/lignumComputing/lignumCAD/Default/Handle/Size";
    }
  } // end of Setting namespace
} // end of lC namespace
#endif // USERSETTINGS_H
