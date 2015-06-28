/* -*- c++ -*-
 * oglpaintdevice.h
 *
 * Header for OpenGLPaintDevice
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
#ifndef OGLPAINTDEVICE_H
#define OGLPAINTDEVICE_H

#include <qpaintdevice.h>

#include "openglbase.h"

/*!
 * This class implements a very narrow QPaintDevice. It is solely
 * for rendering RichText into the current OpenGL context. It goes
 * without saying that this depends a great deal on undocumented
 * internals of Qt.
 */
class OGLPaintDevice : public QPaintDevice
{
  OpenGLBase* view_;
  FaceData face_data_;
public:
  OGLPaintDevice ( OpenGLBase* view )
    : QPaintDevice( QInternal::ExternalDevice ), view_( view )
  {}

  void setView ( OpenGLBase* view );
  bool cmd ( int command, QPainter* painter, QPDevCmdParam* params );
  int metric ( int n ) const;
};

#endif // OGLPAINTDEVICE_H
