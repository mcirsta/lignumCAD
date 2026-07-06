/* -*- c++ -*-
 * runtimepaths.cpp
 *
 * Runtime data directory lookup helpers.
 * Copyright (C) 2026 Marius Cirsta <mforce2@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */
#include "runtimepaths.h"

#include <QCoreApplication>
#include <QSettings>

#include "configuration.h"
#include "usersettings.h"

namespace {
  void appendCandidate ( QStringList& candidates, const QString& path )
  {
    if ( path.isEmpty() )
      return;

    const QString clean_path = QDir::cleanPath( path );
    if ( !candidates.contains( clean_path ) )
      candidates.append( clean_path );
  }
}

namespace lC {
  namespace Runtime {
    QString versionedDataDir ( const QString& root )
    {
      if ( root.isEmpty() )
	return QString();

      return QDir( root ).filePath( QString( "v%1.%2" ).
				    arg( lC::STR::VERSION_MAJOR ).
				    arg( lC::STR::VERSION_MINOR ) );
    }

    QStringList dataDirCandidates ( void )
    {
      QStringList candidates;

      QSettings settings;
      const QString configured_home =
	settings.value( lC::Setting::HOME ).toString();
      appendCandidate( candidates, versionedDataDir( configured_home ) );
      appendCandidate( candidates, configured_home );

      if ( QDir::isAbsolutePath( lC::STR::INSTALL_DATA_DIR ) )
	appendCandidate( candidates, lC::STR::INSTALL_DATA_DIR );
      else
	appendCandidate( candidates,
			 QDir( QCoreApplication::applicationDirPath() ).
			 filePath( QString( "../%1" ).
				   arg( lC::STR::INSTALL_DATA_DIR ) ) );

      appendCandidate( candidates, lC::STR::DATA_DIR );

      if ( lC::STR::ENABLE_DEVELOPMENT_DATA_DIR )
	appendCandidate( candidates, lC::STR::DEVELOPMENT_DATA_DIR );

      return candidates;
    }

    QString dataDirPath ( void )
    {
      for ( const QString& candidate : dataDirCandidates() ) {
	QDir dir( candidate );
	if ( dir.exists() )
	  return dir.absolutePath();
      }

      return QString();
    }

    QDir dataDir ( void )
    {
      return QDir( dataDirPath() );
    }

    QStringList translationDirs ( const QDir& data_dir )
    {
      QStringList dirs;
      appendCandidate( dirs, data_dir.filePath( "translations" ) );
      appendCandidate( dirs, data_dir.absolutePath() );
      return dirs;
    }
  }
}
