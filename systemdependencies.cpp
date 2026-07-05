/*
 * systemdependencies.cpp
 *
 * Functions which are dependent on operating system, graphics system, etc.
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
#include <qapplication.h>
#include <qfont.h>
#include <qstringlist.h>
#include <qprocess.h>
#include <qdir.h>
#include <qfileinfo.h>
#include <qmessagebox.h>
#include <QLocale>

#include "configuration.h"
#include "systemdependencies.h"

namespace System {

  // Find the file which contains the user's desired font.

  bool findFontFile ( const QString& font, QString& file, double& point_size )
  {
    QFont qfont;
    qfont.fromString( font );

    point_size = qfont.pointSizeF();
    if ( point_size <= 0 )
      point_size = 12;

    // TODO: Once the Qt6 port is running, replace this command-line fallback
    // with a proper font discovery path for OGLFT/FreeType.
    QProcess fc_match;
    fc_match.start( "fc-match", QStringList{ "-f", "%{file}", qfont.family() } );
    if ( !fc_match.waitForFinished( 3000 ) )
      return false;

    file = QString::fromLocal8Bit( fc_match.readAllStandardOutput() ).trimmed();

    return !file.isEmpty();
  }

  // Show the manual in some way.

  void showManual ( const QDir& home_dir )
  {
    // Find the manual for the current language (or English otherwise).
    QString locale = QLocale::system().name();
    QString doc_dir = home_dir.absolutePath() + QDir::separator() + "doc" +
      QDir::separator() + locale;
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
	int k = doc_dir.lastIndexOf( delimiters[i] );
	if ( k > rightmost )
	  rightmost = k;
      }

      if ( rightmost == 0 )
	break;

      doc_dir.truncate( rightmost );
    }

    // Well, maybe the English version is available.

    if ( help_file.isEmpty() ) {
      doc_dir = home_dir.absolutePath() + QDir::separator() + "doc" + QDir::separator() +
	"en" + QDir::separator() + "HTML" + QDir::separator() + "index.html";
      QFileInfo manual( doc_dir );

      if ( manual.isReadable() )
	help_file = manual.filePath();
    }

    if ( help_file.isEmpty() ) {
      QMessageBox mb( QMessageBox::Warning,
		      lC::STR::LIGNUMCAD,
		      qApp->translate( "Messages",
				       "lignumCAD cannot access the documentation.\n"
				       "It should be installed in the directory\n"
				       "%1\n"
				       "Please check your installation." ).
		      arg( home_dir.absolutePath() + QDir::separator() + "doc" +
			   QDir::separator() + locale ),
		      QMessageBox::Ok );
      mb.setObjectName( "no_documentation" );
      mb.exec();
      return;
    }

#if defined(Q_OS_UNIX)
    QProcess::startDetached( "kfmclient", QStringList{ "exec", help_file } );
#else
#error "System::showManual() must be defined for this system"
#endif
  }

  // Find the full path to the executable.

  QString executablePath ( void )
  {
#if defined(Q_OS_LINUX)
    QFileInfo proc_exe_entry( "/proc/self/exe" );
    return proc_exe_entry.readSymLink();
#else
#error "System::executablePath() must be defined for this system"
#endif
  }
} // end of System namespace 
