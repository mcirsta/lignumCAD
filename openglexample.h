/* -*- c++ -*-
 * openglexample.h
 *
 * Header for the OpenGLExample class.
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

#ifndef OPENGLEXAMPLE_H
#define OPENGLEXAMPLE_H

#include "vectoralgebra.h"
#include "units.h"
#include "openglbase.h"

using namespace Space3D;

namespace Space2D {
  class Dimension;
}

/*!
 * This class shows an example of the dimesion line. Has to keep a lot
 * of internal rendering state since changes can't be applied to the
 * application until the user selects OK.
 */

class OpenGLExample : public OpenGLBase {
Q_OBJECT
  //! This view has its own set of OpenGL attributes so that they
  //! can be changed in a dialog without affecting the global
  //! attribute database.
  OpenGLAttributes attributes_;

  //! Rendering mode (i.e., whether or not to draw resize handles, for example)
  lC::Render::Mode mode_;

  //! The example dimension line.
  Space2D::Dimension* dimension_;

  //! Save a copy of the scale of the viewport.
  double scale_;

  //! Save a copy of the lower left corner of the viewport.
  Point ll_corner_;

  //! Save a copy of the upper right corner of the viewport.
  Point ur_corner_;

  //! Keep a copy of the example's length unit object itself.
  LengthUnit* length_unit_;

  //! Keep a copy of the example's length unit format (FRACTIONAL or DECIMAL)
  enum UnitFormat format_;

  //! Keep a copy of the example's length unit precision.
  int precision_;

  //! Length of the example dimension.
  double dmn_length_;

public:
  /*!
   * You only get one chance to specify the QGLFormat of an OpenGL widget. In
   * general the examples don't need a depth buffer, so we can save a little
   * memory space by not allocating one (though, you could always get one
   * anyway).
   * \param length_unit default length unit.
   * \param format special OpenGL context requirements.
   * \param name QObject name of widget.
   * \param parent widget's parent.
   */
  OpenGLExample ( LengthUnit* length_unit, const QGLFormat& format,
		  const char* name, QWidget* parent );

  //! Destructor doesn't do much.
  ~OpenGLExample ( void );

  /*!
   * Retreive the scale of the example viewport.
   */
  double scale ( void ) const;

  /*!
   * \return lower left znear corner of the viewing volume
   * in model units.
   */
  Point llCorner ( void ) const;

  /*!
   * \return upper right zfar corner of the viewing volume
   * in model units.
   */
  Point urCorner ( void ) const;

  /*!
   * Set the attributes of the example length representation.
   * \param length_unit the length unit to display.
   * \param format format of the length unit representation.
   * \param precision precision of the length unit representation.
   */
  void setLengthUnit ( LengthUnit* length_unit, UnitFormat format, int precision );

  /*!
   * Experiment with a new color scheme.
   * \param color the new default color scheme for the example.
   */
  void setColorScheme ( const PageColorScheme& scheme );

  /*!
   * \return the geometry outline color.
   */
  QColor geometryColor ( void ) const { return attributes_.scheme().geometryColor(); }

  /*!
   * Experiment with a new geometry color.
   * \param color the new default geometry color for the example.
   */
  void setGeometryColor ( const QColor& color );

  /*!
   * Experiment with a new annotation color.
   * \param color the new default annotation color for the example.
   */
  void setAnnotationColor ( const QColor& color );

  /*!
   * \return the annotation color.
   */
  QColor annotationColor ( void ) const { return attributes_.scheme().annotationColor(); }

  /*!
   * Experiment with a new sketch view grid color.
   * \param color the new default sketch view grid color for the example.
   */
  void setGridColor ( const QColor& color );

  /*!
   * \return the sketch view grid color.
   */
  QColor gridColor ( void ) const { return attributes_.scheme().gridColor(); }

  /*!
   * \return the constraint primary color.
   */
  QColor constraintPrimaryColor ( void ) const { return attributes_.scheme().constraintPrimaryColor(); }

  /*!
   * Experiment with a new constraint primary color.
   * \param color the new default constraint primary color for the example.
   */
  void setConstraintPrimaryColor ( const QColor& color );

  /*!
   * \return the constraint secondary color.
   */
  QColor constraintSecondaryColor ( void ) const { return attributes_.scheme().constraintSecondaryColor(); }
  /*!
   * Experiment with a new constraint secondary color.
   * \param color the new default constraint secondary color for the example.
   */
  void setConstraintSecondaryColor ( const QColor& color );

  /*!
   * Experiment with a new background style.
   * \param style the new default background style for the example.
   */
  void setBackgroundStyle ( enum lC::Background::Style style );

  /*!
   * Experiment with a new solid background color.
   * \param color the new default solid background color for the example.
   */
  void setBackgroundColor ( const QColor& color );

  /*!
   * Experiment with a new gradient background color.
   * \param color the new default gradient background color for the example.
   */
  void setGradientColor ( const QColor& color );

  /*!
   * Experiment with a new background pattern.
   * \param file the file containing the new default background pattern
   * for the example.
   */
  void setPatternFile ( const QString& file );

  /*!
   * Experiment with a new dimension font.
   * \param color the new default dimension font for the example.
   */
  void setDimensionFont ( const QString& font );

  /*!
   * \return the current dimension font for the example.
   */
  QString dimensionFont ( void ) const { return attributes_.dimension().font(); }

  /*!
   * Experiment with a new arrow head length.
   * \param length the new arrow head length for the example.
   */
  void setArrowHeadLength ( double length );

  /*!
   * \return the current arrow head length for the example.
   */
  double arrowHeadLength ( void ) const { return attributes_.dimension().arrowHeadLength(); }

  /*!
   * Experiment with a new arrow head width ratio.
   * \param ratio the new arrow head width ratio for the example.
   */
  void setArrowHeadWidthRatio ( const Ratio& ratio );

  /*!
   * \return the current arrow head width ratio for the example.
   */
  Ratio arrowHeadWidthRatio ( void ) const { return attributes_.dimension().arrowHeadWidthRatio(); }

  /*!
   * Experiment with a new arrow head style.
   * \param style the new arrow head style for the example.
   */
  void setArrowHeadStyle ( enum lC::ArrowHeadStyle style );

  /*!
   * \return the current arrow head style for the example.
   */
  lC::ArrowHeadStyle arrowHeadStyle ( void ) const { return attributes_.dimension().arrowHeadStyle(); }

  /*!
   * Experiment with a new dimension clearance.
   * \param length the new dimension clearance for the example.
   */
  void setClearance ( double length );

  /*!
   * \return the current dimension clearance length for the example.
   */
  double clearance ( void ) const { return attributes_.dimension().clearance(); }

  /*!
   * Experiment with a new dimension extension line offset.
   * \param length the new dimension extension line offset for the example.
   */
  void setExtensionLineOffset ( double length );

  /*!
   * \return the current dimension extension line offset for the example.
   */
  double extensionLineOffset ( void ) const { return attributes_.dimension().extensionLineOffset(); }

  /*!
   * Experiment with a new annotation font.
   * \param color the new default annotation font for the example.
   */
  void setAnnotationFont ( const QString& font );

  /*!
   * \param the current annotation font for the example.
   */
  QString annotationFont ( void ) const { return attributes_.annotation().font(); }

  /*!
   * Experiment with a new handle size.
   * \param length the new handle size for the example.
   */
  void setHandleSize ( double length );

  /*!
   * \return the current handle size for the example.
   */
  double handleSize ( void ) const { return attributes_.handle().size(); }

  /*!
   * Get the OGLFT Face corresponding to the supplied key. This is
   * reimplemented so that this class can override the lookup of
   * the default Face and point size.
   * \param face_data key to look up Face object.
   * \return selected face object.
   */
  OGLFT::Face* font ( const FaceData& face_data );

  /*!
   * There are quite a few order of operation limitations due to the
   * fact that you can't call any OpenGL functions until the view's
   * initializeGL method has been called. We need to be able to compute
   * the projection corners before we can actually use them.
   */
  void computeLayout ( void );

  /*!
   * Format numbers according to the *model information dialog's* format.
   * \param value number to format.
   * \return formatted string representation of number.
   */
  QString format ( double value ) const;

protected:
  /*!
   * Set up any state needed in rendering.
   */
  void initializeGL ( void );
  /*!
   * Called when the window is resized.
   */
  void resizeGL ( int w, int h );
  /*!
   * Called when the window is exposed.
   */
  void paintGL ( void );
private slots:
  //! There seems to be a bug in nVidia's 28.80 OpenGL drivers which cause
  //! the initial exposure glXSwapBuffers to not work if the widget is not
  //! already mapped. So, fire a second chance redraw if the window resized.
  void redraw ( void );
};

#endif // EXAMPLE_H
