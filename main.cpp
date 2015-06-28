/*
 * main.cpp
 *
 * Main routine for lignumCAD
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
#include <QApplication>
#include <qtextcodec.h>
#include <qstatusbar.h>
#include <qlabel.h>
#include <qtimer.h>
#include <qmessagebox.h>
#include <qsettings.h>
#include <qdir.h>

#if defined(Q_OS_UNIX)
#include <signal.h>
#include <stdlib.h>
#endif // Q_OS_UNIX

#include "configuration.h"
#include "systemdependencies.h"
#include "constants.h"
#include "usersettings.h"
#include "ui_lignumcadmainwindow.h"
#include "cursorfactory.h"
#include "command.h"

namespace {
  // A not especially effective segv handler.
#if defined(Q_OS_UNIX)
  void one_shot_segv_handler ( int )
  {
    // The state of the application is undefined so this can only do its
    // best.
    CommandHistory::instance().flushOnSegV();
    // Nothing else can really be done by this process
    exit(6);
  }
#endif
}

int main( int argc, char ** argv )
{
  QApplication app( argc, argv );

  // First, we have to locate our data directory
  QSettings settings;
  bool ok;
  QString home = settings.readEntry( lC::Setting::HOME, lC::STR::HOME, &ok );

  QDir home_dir( QString( "%1%2v%3.%4" ).arg(home).arg(QDir::separator()).
		 arg( lC::STR::VERSION_MAJOR ).arg( lC::STR::VERSION_MINOR ) );

  if ( !home_dir.exists() ) {
    QMessageBox mb( app.translate( "Constants", lC::STR::LIGNUMCAD ),
	    app.translate( "Messages",
			   "The data directory for lignumCAD does not exist in:\n"
			   "%1\n"
			   "Please check your installation.").
		    arg( home_dir.absPath() ),
		    QMessageBox::Critical,
		    QMessageBox::Ok,
		    QMessageBox::NoButton, QMessageBox::NoButton,
		    0, "no_data_dir", true, Qt::WType_Popup );
    mb.exec();
    return 1;
  }

  // Translations should be in the data directory

  QTranslator translator( 0 );
  if ( !translator.load( QString( "lignumCAD_" ) + QTextCodec::locale(), "." ) ) {
    if ( !translator.load( QString( "lignumCAD_" ) + QTextCodec::locale(),
			   home_dir.absPath() + QDir::separator() +
			   "translations" ) ) {
       
      QMessageBox mb( app.translate( "Constants", lC::STR::LIGNUMCAD ),
	      app.translate( "Messages",
			     "Could not find translation file for locale:\n"
			     "%1\n"
			     "Continuing with built-in strings.\n"
			     "Please check your installation.").
		      arg( QTextCodec::locale() ),
		      QMessageBox::Warning,
		      QMessageBox::Ok,
		      QMessageBox::NoButton, QMessageBox::NoButton,
		      0, "no_data_dir", true, Qt::WType_Popup );
      mb.exec();
    }
  }

  app.installTranslator( &translator );

  // There are also some Qt internal labels which need to be translated.
  // Only for non-English languages, however.

  QTranslator qt_translator( 0 );

  if ( ! QString( QTextCodec::locale() ).startsWith( "en" ) ) {

    if ( !qt_translator.load( QString( "qt_" ) + QTextCodec::locale(), "." ) ) {
      if ( !qt_translator.load( QString( "qt_" ) + QTextCodec::locale(),
				home_dir.absPath() + QDir::separator() +
				"translations" ) ) {
	
	QMessageBox mb( app.translate( "Constants", lC::STR::LIGNUMCAD ),
			app.translate( "Messages",
			       "Could not find Qt translation file for locale:\n"
			       "%1\n"
			       "Continuing with built-in strings.\n"
			       "Please check your installation.").
			arg( QTextCodec::locale() ),
			QMessageBox::Warning,
			QMessageBox::Ok,
			QMessageBox::NoButton, QMessageBox::NoButton,
			0, "no_data_dir", true, Qt::WType_Popup );
	mb.exec();
      }
    }

    app.installTranslator( &qt_translator );
  }

  // Must be able to write the history file into the current directory.

  if ( ! CommandHistory::instance().ready() ) {
    QMessageBox mb( app.translate( "Constants", lC::STR::LIGNUMCAD ),
	    app.translate( "Messages",
			   "lignumCAD cannot write the command history file\n"
			   "in the current directory. Please make sure that\n"
			   "you have write permission to the current directory\n"
			   "and that no old history.xml exists." ),
		    QMessageBox::Warning,
		    QMessageBox::Ok,
		    QMessageBox::NoButton, QMessageBox::NoButton,
		    0, "no_data_dir", true, Qt::WType_Popup );
    mb.exec();
    return 1;
  }

  lignumCADMainWindow *lCMW = new lignumCADMainWindow;

  lCMW->setHomeDir( home_dir );

  lCMW->show();

  // Make sure that we can actually work. In particular, the
  // default application font must be a Type1 or TrueType font.
  QString file;
  double point_size;
  if ( !System::findFontFile( app.font().toString(), file, point_size ) ) {
    QMessageBox::critical( lCMW, app.translate( "Constants", lC::STR::LIGNUMCAD ),
	   app.translate( "Messages",
			  "A current limitation of lignumCAD is\n"
			  "the requirement that the default application\n"
			  "font be either a Type1 or TrueType font.\n"
			  "Please run the qtconfig program from\n"
			  "Qt version 3.0 to set up the proper font. See\n"
			  "http://www.trolltech.com/developer/download/qt-x11.html" ),
			   QMessageBox::Ok,
			   QMessageBox::NoButton, QMessageBox::NoButton );
    return 1;
  }

  // Without command line arguments, treat the user to the splash
  // screen for a few seconds

  if ( argc == 1 )
    QTimer::singleShot( 1000, lCMW, SLOT( showView() ) );

  // Otherwise, load the given model and get going

  else
    lCMW->showView( argv[1] );

  app.connect( &app, SIGNAL( lastWindowClosed() ), lCMW, SLOT( fileExit() ) );

#if defined(Q_OS_UNIX)
  // Kind of a last resort thing:
  sigset_t sigset;
  sigemptyset( &sigset );

  struct sigaction segv_action;
  segv_action.sa_handler = one_shot_segv_handler;
  segv_action.sa_mask = sigset;
  segv_action.sa_flags = 0;
#if 0
  sigaction( SIGSEGV, &segv_action, 0 );
#endif
#endif // Q_OS_UNIX

  int ret_code = app.exec();

  CursorFactory::instance().clear();
  delete lCMW;
  return ret_code;
}