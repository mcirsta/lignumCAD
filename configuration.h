/* -*- c++ -*-
 * configuration.h
 *
 * Program source configuration control
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
#ifndef CONFIGURATION_H
#define CONFIGURATION_H

namespace lC {
  namespace STR {
    const QString LIGNUMCOMPUTINGINC = "lignum Computing, Inc.";
    const QString LIGNUMCOMPUTINGURL = "www.lignumcomputing.com";
    const QString LIGNUMCOMPUTINGEMAIL = "lignumcad@lignumcomputing.com";
    
    const char cLIGNUMCAD[] = "lignumCAD";
    const QString LIGNUMCAD = QT_TRANSLATE_NOOP( "Constants", cLIGNUMCAD );
    const QString VERSION_MAJOR = "0";
    const QString VERSION_MINOR = "2";
    const QString COPYRIGHT_DATES = "2002";
#if defined(Q_OS_UNIX)
    const QString HOME                = "/opt/lignumCAD";
#else
#error "Must define installation directory for this operating system"
#endif
  } // End of STR namespace
} // End of lC namespace


#endif /* CONFIGURATION_H */
