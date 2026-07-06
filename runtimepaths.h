/* -*- c++ -*-
 * runtimepaths.h
 *
 * Runtime data directory lookup helpers.
 * Copyright (C) 2026 Marius Cirsta <mforce2@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */
#ifndef RUNTIMEPATHS_H
#define RUNTIMEPATHS_H

#include <QDir>
#include <QString>
#include <QStringList>

namespace lC {
  namespace Runtime {
    QString versionedDataDir ( const QString& root );
    QStringList dataDirCandidates ( void );
    QString dataDirPath ( void );
    QDir dataDir ( void );
    QStringList translationDirs ( const QDir& data_dir );
  }
}

#endif // RUNTIMEPATHS_H
