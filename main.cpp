/*
 * main.cpp
 *
 * Main routine for lignumCAD
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
#include <qstatusbar.h>
#include <qlabel.h>
#include <qtimer.h>
#include <qmessagebox.h>
#include <qdir.h>
#include <QCoreApplication>
#include <QLocale>
#include <QTranslator>

#if defined(Q_OS_UNIX)
#include <signal.h>
#include <stdlib.h>
#endif // Q_OS_UNIX

#include "configuration.h"
#include "systemdependencies.h"
#include "constants.h"
#include "runtimepaths.h"
#include "lignumcadmainwindow.h"
#include "cursorfactory.h"
#include "command.h"

namespace {
  QString trConstant ( const QString& text )
  {
    return QCoreApplication::translate( "Constants", text.toUtf8().constData() );
  }

  QString trMessage ( const char* text )
  {
    return QCoreApplication::translate( "Messages", text );
  }

  bool loadTranslation ( QTranslator& translator, const QString& name,
			 const QDir& data_dir )
  {
    if ( translator.load( name, "." ) )
      return true;

    for ( const QString& translation_dir :
	    lC::Runtime::translationDirs( data_dir ) ) {
      if ( translator.load( name, translation_dir ) )
	return true;
    }

    return false;
  }

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
  const QString data_dir_path = lC::Runtime::dataDirPath();
  if ( data_dir_path.isEmpty() ) {
    QMessageBox::critical( 0, trConstant( lC::STR::LIGNUMCAD ),
                           trMessage( "The data directory for lignumCAD was not found.\n"
				      "Checked:\n"
                                      "%1\n"
                                      "Please check your installation." ).
                           arg( lC::Runtime::dataDirCandidates().join( "\n" ) ) );
    return 1;
  }

  QDir home_dir( data_dir_path );

  // Translations should be in the data directory

  QString locale = QLocale::system().name();
  QTranslator translator;
  if ( !loadTranslation( translator, QString( "lignumCAD_" ) + locale,
			 home_dir ) ) {
    QMessageBox::warning( 0, trConstant( lC::STR::LIGNUMCAD ),
			  trMessage( "Could not find translation file for locale:\n"
				     "%1\n"
				     "Continuing with built-in strings.\n"
				     "Please check your installation." ).
			  arg( locale ) );
  }

  app.installTranslator( &translator );

  // There are also some Qt internal labels which need to be translated.
  // Only for non-English languages, however.

  QTranslator qt_translator;

  if ( !locale.startsWith( "en" ) ) {

    if ( !loadTranslation( qt_translator, QString( "qt_" ) + locale,
			   home_dir ) ) {
      QMessageBox::warning( 0, trConstant( lC::STR::LIGNUMCAD ),
			    trMessage( "Could not find Qt translation file for locale:\n"
				       "%1\n"
				       "Continuing with built-in strings.\n"
				       "Please check your installation." ).
			    arg( locale ) );
    }

    app.installTranslator( &qt_translator );
  }

  // Must be able to write the history file into the current directory.

  if ( ! CommandHistory::instance().ready() ) {
    QMessageBox::warning( 0, trConstant( lC::STR::LIGNUMCAD ),
                          trMessage( "lignumCAD cannot write the command history file\n"
                                     "in the current directory. Please make sure that\n"
                                     "you have write permission to the current directory\n"
                                     "and that no old history.xml exists." ) );
    return 1;
  }

  lignumCADMainWindow *lCMW = new lignumCADMainWindow;

  lCMW->setHomeDir( home_dir );

  lCMW->show();

  // OpenGL text rendering uses FreeType and needs a real font file for
  // the default application font.
  QString file;
  double point_size;
  if ( !System::findFontFile( app.font().toString(), file, point_size ) ) {
    QMessageBox::critical( lCMW, trConstant( lC::STR::LIGNUMCAD ),
			   trMessage( "A current limitation of lignumCAD is\n"
				      "the requirement that the default application\n"
				      "font has a file that FreeType can load.\n"
				      "Please choose an installed outline font\n"
				      "using your desktop font settings." ) );
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
