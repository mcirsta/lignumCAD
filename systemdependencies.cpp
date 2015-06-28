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
#include <qapplication.h>
#include <qfont.h>
#include <qstringlist.h>
#include <qregexp.h>
#include <qprocess.h>
#include <qdir.h>
#include <qfileinfo.h>
#include <qmessagebox.h>
#include <qtextcodec.h>

#include "configuration.h"
#include "systemdependencies.h"

namespace System {

  // Find the file which contains the user's desired font.

  bool findFontFile ( const QString& font, QString& file, double& point_size )
  {
    QFont qfont;
    qfont.fromString( font );

    bool found_file = false, found_point_size = false;

#if defined( Q_WS_X11 )
    // This depends on so many things. Currently it will only work
    // with my modification to Xft. Non-X based implementations are
    // out of luck for now...

    // Try to extract the name of the file in which the font is stored.
    // This can fail for a number of reasons, but most especially if the
    // user has selected a file which is only identified by its XLFD id.

    QStringList elements = QStringList::split( ":", qfont.rawName() );

    for ( QStringList::Iterator element = elements.begin();
	  element != elements.end();
	  ++element ) {
      QStringList values = QStringList::split( "=", *element );
      if ( values.front() == "file" ) {
	// The Xft file string seems to include some extra backquotes
	file = values.back().replace( QRegExp("\\\\"), "" );
	found_file = true;
      }
      // The value that Xft passes to FT_Set_Char_Size is:
      // size = point size / 72 * screen Y DPI.
      // Not entirely sure this is the correct value, but we have
      // to duplicate it in order for the font requestor and OpenGL
      // view to show the font at the same size.
      else if ( values.front() == "pixelsize" ) {
	point_size = values.back().toDouble();
	found_point_size = true;
      }
    }
#else
#error "System::findFontFile() must be defined for this system"
#endif
    return found_file && found_point_size;
  }

  // Show the manual in some way.

  void showManual ( const QDir& home_dir )
  {
    // Find the manual for the current language (or English otherwise).
    QString doc_dir = home_dir.absPath() + QDir::separator() + "doc" +
      QDir::separator() + QTextCodec::locale();
    QString delimiters = "_.@";
    QString help_file;
    while ( true ) {

      QFileInfo manual( doc_dir + QDir::separator() + "HTML" + QDir::separator() +
			"index.html" );

      if ( manual.isReadable() ) {
	help_file = manual.filePath();
	break;
      }

      int rightmost = 0;
      for ( int i = 0; i < (int)delimiters.length(); i++ ) {
	int k = doc_dir.findRev( delimiters[i] );
	if ( k > rightmost )
	  rightmost = k;
      }

      if ( rightmost == 0 )
	break;

      doc_dir.truncate( rightmost );
    }

    // Well, maybe the English version is available.

    if ( help_file.isEmpty() ) {
      doc_dir = home_dir.absPath() + QDir::separator() + "doc" + QDir::separator() +
	"en" + QDir::separator() + "HTML" + QDir::separator() + "index.html";
      QFileInfo manual( doc_dir );

      if ( manual.isReadable() )
	help_file = manual.filePath();
    }

    if ( help_file.isEmpty() ) {
      QMessageBox mb( qApp->translate( "Constants", lC::STR::LIGNUMCAD ),
	qApp->translate( "Messages",
			 "lignumCAD cannot access the documentation.\n"
			 "It should be installed in the directory\n"
			 "%1\n"
			 "Please check your installation." ).
		      arg( home_dir.absPath() + QDir::separator() + "doc" +
			   QDir::separator() + QTextCodec::locale() ),
			QMessageBox::Warning,
			QMessageBox::Ok,
			QMessageBox::NoButton, QMessageBox::NoButton,
			0, "no_documentation" );
      mb.exec();
      return;
    }

#if defined(Q_OS_UNIX)
    QProcess show_manual( QString( "kfmclient" ) );
    show_manual.addArgument( "exec" );
    show_manual.addArgument( help_file );
    show_manual.launch( QByteArray() );
#else
#error "System::showManual() must be defined for this system"
#endif
  }

  // Find the full path to the executable.

  QString executablePath ( void )
  {
#if defined(Q_OS_LINUX)
    QFileInfo proc_exe_entry( "/proc/self/exe" );
    return proc_exe_entry.readLink();
#else
#error "System::executablePath() must be defined for this system"
#endif
  }
} // end of System namespace 
