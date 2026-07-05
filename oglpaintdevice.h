/* -*- c++ -*-
 * oglpaintdevice.h
 *
 * Header for OpenGLPaintDevice
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
#ifndef OGLPAINTDEVICE_H
#define OGLPAINTDEVICE_H

#include <memory>

#include <qpaintdevice.h>

#include "openglbase.h"

class OGLPaintEngine;

/*!
 * This class implements a very narrow QPaintDevice. It is solely
 * for rendering RichText into the current OpenGL context. It goes
 * through a small QPaintEngine which handles just the primitives
 * emitted by QTextDocument.
 */
class OGLPaintDevice : public QPaintDevice
{
  OpenGLBase* view_;
  FaceData face_data_;
  std::unique_ptr<OGLPaintEngine> paint_engine_;
public:
  OGLPaintDevice ( OpenGLBase* view );
  ~OGLPaintDevice ( void );

  void setView ( OpenGLBase* view );
  OpenGLBase* view ( void ) const { return view_; }
  FaceData& faceData ( void ) { return face_data_; }
  QPaintEngine* paintEngine ( void ) const override;
  int metric ( PaintDeviceMetric n ) const override;
};

#endif // OGLPAINTDEVICE_H
