/* -*- c++ -*-
 * openglbase.h
 *
 * Header for the OpenGLBase classes.
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

#ifndef OPENGLBASE_H
#define OPENGLBASE_H

#include <qgl.h>
#include <qmap.h>

#include "ratio.h"
#include "drawingdefaults.h"

// Forward declarations
namespace Space2D {
  class Point;
}
namespace Space3D {
  class Point;
  class Vector;
}
namespace OGLFT {
  class Face;
}

/*!
 * An OGLFT Face will clear its cache of display lists when an attribute
 * of the Face is changed. So, for efficiency, the application caches
 * each of the requested fonts in a map based on this complicated key.
 */
struct FaceData {
  //! Use Qt's string representation as the basic description
  //! of the font (QFont::toString())
  QString font_;

  //! The string rotation of the face in degrees. Default is 0. (What
  //! does this mean: Horizontal or natural for the user's language?)
  uint orientation_;

  //! Color of rendered text. Default is black.
  QRgb color_;

  //! Horizontal justification of rendered text. Default is ORIGIN.
  lC::HorizontalJustification horizontal_justification_;

  //! Vertical justification of rendered text. Default is BASELINE.
  lC::VerticalJustification vertical_justification_;

  //! Scale of the view (matters to outline faces).
  Ratio scale_;

  /*
   *! Construct a Face lookup key.
   * \param font The Qt string representation of a font. If
   * specified as QString::null (or the empty string, ""), the Face
   * will use the Qt application default font.
   * \param color The of the rendered text. Default is black.
   * \param horizontal_justification The horizontal justification of the
   * rendered text. Default is for the text to be drawn starting at the
   * origin of the first character.
   * \param vertical_justification The vertical justification of the
   * rendered text. Default is for the text to be drawn along the baseline
   * of the string.
   * \param scale scale of the OpenGL view.
   */
  FaceData ( const QString& font = QString::null,
	     uint orientation = 0,
	     QRgb color = 0,
	     lC::HorizontalJustification horizontal_justification = lC::ORIGIN,
	     lC::VerticalJustification vertical_justification = lC::BASELINE,
	     Ratio scale = Ratio(0) )
    : font_( font ), orientation_( orientation ), color_( color ),
      horizontal_justification_( horizontal_justification ),
      vertical_justification_( vertical_justification ), scale_( scale )
  {}

  /*!
   * Determine if one key is "less than" another. This is required by
   * the QMap used to cache OGLFT Face objects.
   * \param b The FaceData key to compare to this one.
   */
  bool operator< ( const FaceData& b ) const
  {
    if ( font_ < b.font_ ) return true;
    else if ( font_ == b.font_ ) {
      if ( scale_ < b.scale_ ) return true;
      else if ( scale_ == b.scale_ ) {
	if ( orientation_ < b.orientation_ ) return true;
	else if ( orientation_ == b.orientation_ ) {
	  if ( color_ < b.color_ ) return true;
	  else if ( color_ == b.color_ ) {
	    if ( horizontal_justification_ < b.horizontal_justification_ )
	      return true;
	    else if ( horizontal_justification_ == b.horizontal_justification_ ) {
	      if ( vertical_justification_ < b.vertical_justification_ )
		return true;
	    }
	  }
	}
      }
    }
    return false;
  }
};

/*!
 * Interface definition of all the OpenGL views used in lignumCAD.
 * The services required by all OpenGL views are specified here.
 * The general services defined here are confined to those which the
 * the simplest drawing routines need, namely view size and scale
 * information and the textures and fonts defined in this view. More
 * complicated services, such as input and selection and printing
 * are defined by subclasses.
 */
class OpenGLBase : public QGLWidget {
Q_OBJECT

private:
  //! Each of the texture objects required by the graphics elements
  //! which draw in this widget are cached here.
  QMap< QString, GLuint > textures_;

  //! The background texture (if there is one) is special since its
  //! colors are computed rather than read from a file.
  GLuint background_texture_;

  //! The source (grayscale) image of the background texture.
  QImage background_image_;

protected:
  //! Each of the text Faces required by the graphics elements which
  //! draw in this widget are cached here. Accessible for subclasses
  //! which want to change the source of the default face (e.g. the
  //! OpenGLExample class).
  QMap< FaceData, OGLFT::Face* > faces_;

public:
  /*!
   * Construct an OpenGLBase widget.
   * \param parent parent widget.
   * \param name name of widget.
   * \param share_widget optional reference to widget with which to share display
   * lists.
   */
  OpenGLBase ( QWidget* parent, const char* name, QGLWidget* share_widget = 0 );
  /*!
   * Construct an OpenGLBase widget. This constructor takes an explicit
   * QGLFormat object for special applications.
   * \param parent parent widget.
   * \param name name of widget.
   */
  OpenGLBase ( const QGLFormat& format, QWidget* parent, const char* name );
  /*!
   * Destruct an OpenGLBase widget. Will free the OGLFT::Face objects
   * held in the font cache.
   */
  ~OpenGLBase ( void );

  /*!
   * Retrieve the scale of the OpenGL view. The scale factor
   * is the ratio of the resolution of the display to the
   * effective resolution of the viewport. For example, this monitor
   * is about 100 DPI. If my desktop OpenGL window is 800x800 and I
   * want to display a 10"x10" (in model units) area, then the scale
   * is 100 DPI / ( 800 pixels / 10" ) = 1.25. Of course, this really
   * computed the other way around in the program: the user specifies
   * the scale and the figures are drawn accordingly.
   * \return the scale factor of the current viewport.
   */
  virtual double scale ( void ) const = 0;

  /*!
   * \return lower left znear corner of the current viewing volume
   * in model units.
   */
  virtual Space3D::Point llCorner ( void ) const = 0;

  /*!
   * \return upper right zfar corner of the current viewing volume
   * in model units.
   */
  virtual Space3D::Point urCorner ( void ) const = 0;
  /*!
   * \return the view normal. Defaults to the default OpenGL direction: +Z
   */
  virtual Space3D::Vector viewNormal ( void ) const;
  /*!
   * The view must supply a formatter for numbers. There is a global
   * UnitsBasis which contains the default length representation, but
   * when changes are made in a configuration dialog, each view needs
   * to have a local, transient, formatter.
   * \param value number to format.
   * \return formatted string representation of value.
   */
  virtual QString format ( double value ) const = 0;

  /*!
   * Get the OGLFT Face corresponding to the supplied key. If there
   * is no such face, one is created. Normally, if the face or
   * point size elements of face_data are empty, the default from
   * OpenGLGlobals is used, however, a class might want to override
   * this, so alternative implementations are allowed.
   * \param face_data key to look up Face object.
   * \return selected face object.
   */
  virtual OGLFT::Face* font ( const FaceData& face_data );

  /*!
   * Take a QPoint in screen coordinates (usually from a QMouseEvent)
   * and determine where it is in a 2D model view.
   * \param p a point in screen coordinates.
   * \return the coordinates in the 2D model view.
   */
  virtual Space2D::Point unproject2D ( const QPoint& p ) const;
  /*!
   * Take a QPoint in screen coordinates (usually from a QMouseEvent)
   * and determine where it is in a 3D model view.
   * \param p a point in screen coordinates.
   * \return the coordinates in the 3D model view.
   */
  virtual Space3D::Point unproject3D ( const QPoint& p ) const;
  /*!
   * Take a Point in pixel coordinates and determine where it is in
   * a 3D model view. Here, pixel coordinates can be fractional.
   * \param p a point in (sub)pixel coordinates.
   * \return the coordinates in the 3D model view.
   */
  virtual Space3D::Point unproject3D ( const Space2D::Point& p ) const;

  /*!
   * I guess every attribute which might be used by a drawing routine
   * will have to be reported through here, too. This is the geometry
   * outline color.
   * \return the geometry outline color.
   */
  virtual QColor geometryColor ( void ) const = 0;

  /*!
   * \return the annotation color.
   */
  virtual QColor annotationColor ( void ) const = 0;

  /*!
   * \return the sketch view grid color.
   */
  virtual QColor gridColor ( void ) const = 0;

  /*!
   * The constraint primary color.
   * \return the constraint primary color.
   */
  virtual QColor constraintPrimaryColor ( void ) const = 0;

  /*!
   * The constraint secondary color.
   * \return the constraint secondary color.
   */
  virtual QColor constraintSecondaryColor ( void ) const = 0;

  /*!
   * Get the texture object corresponding to the supplied key. If
   * there is no such texture object, one is created.
   * \param texture_name the file name containing the texture image.
   * \return OpenGL texture name.
   */
  GLuint texture ( const QString& texture_name );

  /*!
   * Create a texture based on the given file and the two colors.
   * (Details of the computation are in the source file.)
   * \param file name of a file containing a source image.
   * \param color0 the color corresponding to the 0 value in the image.
   * \param color1 the color corresponding to the 255 value in the image.
   */
  GLuint createBackgroundTexture ( const QString& file, const QColor& color0,
				   const QColor& color1 );
  /*!
   * \return the background texture's OpenGL name. 0 if undefined.
   */
  GLuint backgroundTexture ( void ) const { return background_texture_; }

  /*!
   * \return the source (grayscale) background image.
   */
  const QImage& backgroundImage ( void ) const { return background_image_; }

  /*!
   * \return the dimension font.
   */
  virtual QString dimensionFont ( void ) const = 0;

  /*!
   * \return the arrow head length.
   */
  virtual double arrowHeadLength ( void ) const = 0;

  /*!
   * \return the arrow head width ratio.
   */
  virtual Ratio arrowHeadWidthRatio ( void ) const = 0;

  /*!
   * \return the arrow head style.
   */
  virtual lC::ArrowHeadStyle arrowHeadStyle ( void ) const = 0;

  /*!
   * \return the dimension clearance length.
   */
  virtual double clearance ( void ) const = 0;

  /*!
   * \return the dimension extension line offset.
   */
  virtual double extensionLineOffset ( void ) const = 0;

  /*!
   * \return the annotation font.
   */
  virtual QString annotationFont ( void ) const = 0;

  /*!
   * \return the dimension extension line offset.
   */
  virtual double handleSize ( void ) const = 0;

protected:
  /*!
   * Clear the font cache. Normally invoked if the global default font
   * is changed.
   */
  void clearFontCache ( void );

signals:
  /*!
   * Emitted if any font changes, but it is up to the subclass
   * to determine the source of font change signals.
   */
  void fontChanged ( const QString& font );

private slots:
  /*!
   * Update the background color.
   * \param color the new background color.
   */
  void updateBackgroundColor ( const QColor& color );
  /*!
   * Update the default font file. Basically just clears out the cache
   * and causes a redraw.
   * \param file new font file for default face.
   */
  void updateFontFile ( const QString& file );
  /*!
   * Update the default face point size. Basically just clears out the cache
   * and causes a redraw.
   * \param point_size new default face point size.
   */
  void updateFontPointSize ( double point_size );
private:
  /*!
   * Since OpenGL texture images have to be powers of 2 in size, find
   * the smallest power of two which is greater than or equal to the
   * input value.
   * \param a value to convert.
   * \return smallest power of two which is greater than or equal to a.
   */
  uint nearestPowerCeil ( unsigned int a );
};

/*!
 * The set of rendering attributes for an OpenGL view. For now, this
 * also has the duty of clearing the screen.
 */
class OpenGLAttributes {
  //! The color scheme and background style for the page view.
  PageColorScheme color_scheme_;

  //! The dimension drawing attributes.
  DimensionAttributes dimension_attributes_;

  //! The annotation drawing attribute(s).
  AnnotationAttributes annotation_attributes_;

  //! The resize handle drawing attribute(s).
  HandleAttributes handle_attributes_;
  
public:
  /*!
   * Constructor automatically gets all the defaults from constants.h
   */
  OpenGLAttributes ( void );

  /*!
   * Set the display scheme.
   * \param scheme the new display scheme.
   */
  void setColorScheme ( const PageColorScheme& scheme ) { color_scheme_ = scheme; }

  /*!
   * \return the color scheme as a modifiable reference.
   */
  PageColorScheme& scheme ( void ) { return color_scheme_; }

  /*!
   * \return a non-modifiable color scheme.
   */
  const PageColorScheme& scheme ( void ) const { return color_scheme_; }

  /*!
   * Set the dimension attributes.
   * \param dimension the new dimension attributes.
   */
  void setDimension ( const DimensionAttributes& dimension ) { dimension_attributes_ = dimension; }

  /*!
   * \return the dimension attribute object as a modifiable reference.
   */
  DimensionAttributes& dimension ( void ) { return dimension_attributes_; }

  /*!
   * \return a non-modifiable dimension attribute object.
   */
  const DimensionAttributes& dimension ( void ) const { return dimension_attributes_; }

  /*!
   * Set the annotation attributes.
   * \param annotation the new annotation attributes.
   */
  void setAnnotation ( const AnnotationAttributes& annotation ) { annotation_attributes_ = annotation; }

  /*!
   * \return the annotation attribute object as a modifiable reference.
   */
  AnnotationAttributes& annotation ( void ) { return annotation_attributes_; }

  /*!
   * \return a non-modifiable annotation attribute object.
   */
  const AnnotationAttributes& annotation ( void ) const { return annotation_attributes_; }

  /*!
   * Set the resize handle attributes.
   * \param resize handle the new resize handle attributes.
   */
  void setHandle ( const HandleAttributes& handle ) { handle_attributes_ = handle; }

  /*!
   * \return the resize handle attribute object as a modifiable reference.
   */
  HandleAttributes& handle ( void ) { return handle_attributes_; }

  /*!
   * \return a non-modifiable resize handle attribute object.
   */
  const HandleAttributes& handle ( void ) const { return handle_attributes_; }


  /*!
   * Clear the screen in the manner specified by this object.
   * \param view OpenGL view to clear.
   * \param clear_depth_buffer flag to clear the depth buffer.
   */
  void clear ( OpenGLBase* view, bool clear_depth_buffer ) const;
};

/*!
 * Another singleton which stores the global attributes for the OpenGL views.
 * If one is changed, this object emits the appropriate signal.
 * Also stores the program compiled default attributes.
 */
class OpenGLGlobals : public QObject {
  Q_OBJECT
  //! These are the default attributes programmed into lignumCAD.
  OpenGLAttributes default_;

  //! Available display schemes.
  QList< PageColorScheme > color_schemes_;

  //! Has the user selected a predefined scheme or customized it?
  bool predefined_scheme_;

  //! The dimension spin boxes have a limit on the range of values
  //! which can be selected by the user. This structure contains
  //! the minimum values.
  DimensionAttributes dimension_minimums_;

  //! The maximum values which the dimension size spin boxes can attain.
  DimensionAttributes dimension_maximums_;

  //! The resize handle minimum attributes.
  HandleAttributes handle_minimums_;

  //! The resize handle maximum attributes.
  HandleAttributes handle_maximums_;

  //! These are the current attributes selected by the user.
  OpenGLAttributes current_;

  /*!
   * Basically a database of OpenGL attributes. Note that this is a
   * QObject so it can emit signals when the properties change
   * (also means that it really has to be allocated dynamically).
   */
  static OpenGLGlobals* opengl_globals_;

protected:
  /*!
   * Per singleton design pattern, the constructor is hidden and may
   * only be called by instance().
   */
  OpenGLGlobals ( void );
  /*!
   * Destroys the actually QObject at program termination.
   */
  ~OpenGLGlobals ( void );

public:
  /*!
   * \return pointer to instance of global database.
   */
  static OpenGLGlobals* instance ( void );

  /*!
   * \return a list of scheme names. Generates a copy of the names in
   * display_schemes_, so ask for a new list each time it's needed.
   */
  QStringList schemeStrings ( void ) const;

  /*!
   * Set whether or not the current scheme is one of the predefined ones.
   * A separate indication is necessary because there is always a current
   * item in the scheme list box.
   * \param predefined whether or not the current scheme is one of
   * the predefined ones.
   */
  void setPredefinedScheme ( bool predefined );

  /*!
   * \return whether or not the current display scheme is one of
   * the predefined ones or a custom scheme.
   */
  bool isPredefinedScheme ( void ) const { return predefined_scheme_; }

  /*!
   * Update the default scheme to the given one.
   * \param index the index of the new default scheme.
   */
  void setDefaultScheme ( int index );

  /*!
   * Update the default scheme to the given one (based on the scheme names).
   * \param name the name of the new default scheme.
   */
  void setDefaultScheme ( const QString& name );

  /*!
   * \return the default scheme.
   */
  const PageColorScheme& defaultScheme ( void ) const { return default_.scheme(); }

  /*!
   * Update the current scheme to the given one.
   * \param index the index of the new current scheme.
   */
  void setScheme ( int index );

  /*!
   * \return the current scheme.
   */
  const PageColorScheme& scheme ( void ) const { return current_.scheme(); }

  /*!
   * Retrieve the index of the current default scheme. Note that this
   * may not necessarily be active since the user could have specified
   * completely different colors, but this is still the source of
   * the defaults.
   * \return index of the current default scheme.
   */

  //TODO getDefaultScheme instead of 0
  int at () const { return  0; }

  /*!
   * Retrieve the i-th predefined scheme without modifying the current
   * scheme.
   * \param index the index of the scheme to retrieve.
   * \return selected scheme.
   */
  const PageColorScheme& scheme ( int index ) const;

  /*!
   * \return the current dimension attributes.
   */
  const DimensionAttributes& dimension ( void ) const { return current_.dimension(); }

  /*!
   * \return the current annotation attributes.
   */
  const AnnotationAttributes& annotation ( void ) const { return current_.annotation(); }

  /*!
   * \return the current handle attributes.
   */
  const HandleAttributes& handle ( void ) const { return current_.handle(); }

  /*!
   * Forward the clearing function.
   * \param view the view to clear.
   * \param clear_depth_buffer clear the depth buffer too?
   */
  void clear ( OpenGLBase* view, bool clear_depth_buffer ) { current_.clear( view, clear_depth_buffer ); }

  /*!
   * \return the default geometry outline color.
   */
  QColor defaultGeometryColor ( void ) const { return default_.scheme().geometryColor(); }

  /*!
   * Set the geometry outline color.
   * \param color the new geometry outline color.
   */
  void setGeometryColor ( const QColor& color );

  /*!
   * \return the current geometry outline color.
   */
  QColor geometryColor ( void ) const { return current_.scheme().geometryColor(); }

  /*!
   * \return the default annotation color.
   */
  QColor defaultAnnotationColor ( void ) const { return default_.scheme().annotationColor(); }

  /*!
   * Set the annotation color.
   * \param color the new annotation color.
   */
  void setAnnotationColor ( const QColor& color );

  /*!
   * \return the current annotation color.
   */
  QColor annotationColor ( void ) const { return current_.scheme().annotationColor(); }

  /*!
   * \return the default sketch view grid color.
   */
  QColor defaultGridColor ( void ) const { return default_.scheme().gridColor(); }

  /*!
   * Set the sketch view grid color.
   * \param color the new sketch view grid color.
   */
  void setGridColor ( const QColor& color );

  /*!
   * \return the current sketch view grid color.
   */
  QColor gridColor ( void ) const { return current_.scheme().gridColor(); }

  /*!
   * \return the default constraint primary color.
   */
  QColor defaultConstraintPrimaryColor ( void ) const { return default_.scheme().constraintPrimaryColor(); }

  /*!
   * Set the constraint primary color.
   * \param color the new constraint primary color.
   */
  void setConstraintPrimaryColor ( const QColor& color );

  /*!
   * \return the current constraint primary color.
   */
  QColor constraintPrimaryColor ( void ) const { return current_.scheme().constraintPrimaryColor(); }

  /*!
   * \return the default constraint secondary color.
   */
  QColor defaultConstraintSecondaryColor ( void ) const { return default_.scheme().constraintSecondaryColor(); }

  /*!
   * Set the constraint secondary color.
   * \param color the new constraint secondary color.
   */
  void setConstraintSecondaryColor ( const QColor& color );

  /*!
   * \return the current constraint secondary color.
   */
  QColor constraintSecondaryColor ( void ) const { return current_.scheme().constraintSecondaryColor(); }

  /*!
   * \return the default background style.
   */
  enum lC::Background::Style defaultBackgroundStyle ( void ) const { return default_.scheme().backgroundStyle(); }

  /*!
   * Set the background style.
   * \param style the new background style.
   */
  void setBackgroundStyle ( enum lC::Background::Style style );

  /*!
   * \return the current background style.
   */
  enum lC::Background::Style backgroundStyle ( void ) const { return current_.scheme().backgroundStyle(); }

  /*!
   * \return the default solid background color.
   */
  QColor defaultBackgroundColor ( void ) const { return default_.scheme().backgroundColor(); }

  /*!
   * Set the solid background color.
   * \param color the new solid background color.
   */
  void setBackgroundColor ( const QColor& color );

  /*!
   * \return the current solid background color.
   */
  QColor backgroundColor ( void ) const { return current_.scheme().backgroundColor(); }

  /*!
   * \return the default gradient background color.
   */
  QColor defaultGradientColor ( void ) const { return default_.scheme().gradientColor(); }

  /*!
   * Set the gradient background color.
   * \param color the new gradient background color.
   */
  void setGradientColor ( const QColor& color );

  /*!
   * \return the current gradient background color.
   */
  QColor gradientColor ( void ) const { return current_.scheme().gradientColor(); }

  /*!
   * \return the default pattern background image file name.
   */
  QString defaultPatternFile ( void ) const { return default_.scheme().patternFile(); }

  /*!
   * Set the pattern background image file name.
   * \param file the new pattern background image file name.
   */
  void setPatternFile ( const QString& file );

  /*!
   * \return the current pattern background image file name.
   */
  QString patternFile ( void ) const { return current_.scheme().patternFile(); }

  /*!
   * \return the default geometry texture image file name.
   */
  QString defaultTextureFile ( void ) const { return default_.scheme().textureFile(); }

  /*!
   * Set the geometry texture image file name.
   * \param file the new geometry texture image file name.
   */
  void setTextureFile ( const QString& file );

  /*!
   * \return the current geometry texture image file name.
   */
  QString textureFile ( void ) const { return current_.scheme().textureFile(); }

  /*!
   * \return the default dimension font identifier.
   */
  QString defaultDimensionFont ( void ) const { return default_.dimension().font();}

  /*!
   * Set the dimension font.
   * \param font the new dimension font.
   */
  void setDimensionFont ( const QString& font );

  /*!
   * \return the current dimension font.
   */
  QString dimensionFont ( void ) const { return current_.dimension().font(); }

  /*!
   * \return the default dimension arrow head style.
   */
  enum lC::ArrowHeadStyle defaultArrowHeadStyle ( void ) const { return default_.dimension().arrowHeadStyle(); }

  /*!
   * Set the dimension arrow head style.
   * \param style the new dimension arrow head style.
   */
  void setArrowHeadStyle ( enum lC::ArrowHeadStyle style );

  /*!
   * \return the current dimension arrow head style.
   */
  enum lC::ArrowHeadStyle arrowHeadStyle ( void ) const { return current_.dimension().arrowHeadStyle(); }

  /*!
   * \return the minimum dimension arrow head length in inches.
   */
  double minimumArrowHeadLength ( void ) const { return dimension_minimums_.arrowHeadLength(); }

  /*!
   * \return the maximum dimension arrow head length in inches.
   */
  double maximumArrowHeadLength ( void ) const { return dimension_maximums_.arrowHeadLength(); }

  /*!
   * \return the default dimension arrow head length in inches.
   */
  double defaultArrowHeadLength ( void ) const { return default_.dimension().arrowHeadLength(); }

  /*!
   * Set the dimension arrow head length.
   * \param style the new dimension arrow head length in inches.
   */
  void setArrowHeadLength ( double length );

  /*!
   * \return the current dimension arrow head length in inches.
   */
  double arrowHeadLength ( void ) const { return current_.dimension().arrowHeadLength(); }

  /*!
   * \return the default dimension arrow head length to width ratio.
   */
  Ratio defaultArrowHeadWidthRatio ( void ) const { return default_.dimension().arrowHeadWidthRatio(); }

  /*!
   * Set the dimension arrow head length to width ratio.
   * \param width_ratio the new dimension arrow head length to width ratio.
   */
  void setArrowHeadWidthRatio ( const Ratio& ratio );

  /*!
   * \return the current dimension arrow head length to width ratio.
   */
  Ratio arrowHeadWidthRatio ( void ) const { return current_.dimension().arrowHeadWidthRatio(); }

  /*!
   * \return the minimum dimension clearance length in inches.
   */
  double minimumClearance ( void ) const { return dimension_minimums_.clearance(); }

  /*!
   * \return the maximum dimension clearance length in inches.
   */
  double maximumClearance ( void ) const { return dimension_maximums_.clearance(); }

  /*!
   * \return the default dimension number clearance in inches.
   */
  double defaultClearance ( void ) const { return default_.dimension().clearance(); }

  /*!
   * Set the dimension number clearance in inches.
   * \param clearance the new dimension number clearance in inches.
   */
  void setClearance ( double clearance );

  /*!
   * \return the current dimension number clearance in inches.
   */
  double clearance ( void ) const { return current_.dimension().clearance(); }

  /*!
   * \return the minimum line thickness in inches.
   */
  double minimumLineThickness ( void ) const { return dimension_minimums_.lineThickness(); }

  /*!
   * \return the maximum line thickness in inches.
   */
  double maximumLineThickness ( void ) const { return dimension_maximums_.lineThickness(); }

  /*!
   * \return the default dimension line thickness in inches.
   */
  double defaultLineThickness ( void ) const { return default_.dimension().lineThickness(); }

  /*!
   * Set the dimension line thickness in inches.
   * \param thickness the new dimension line thickness in inches.
   */
  void setLineThickness ( double thickness );

  /*!
   * \return the current dimension line thickness in inches.
   */
  double lineThickness ( void ) const { return current_.dimension().lineThickness(); }

  /*!
   * \return the minimum dimension extension line offset in inches.
   */
  double minimumExtensionLineOffset ( void ) const { return dimension_minimums_.extensionLineOffset(); }

  /*!
   * \return the maximum dimension extension line offset in inches.
   */
  double maximumExtensionLineOffset ( void ) const { return dimension_maximums_.extensionLineOffset(); }

  /*!
   * \return the default dimension extension line offset in inches.
   */
  double defaultExtensionLineOffset ( void ) const { return default_.dimension().extensionLineOffset(); }

  /*!
   * Set the dimension extension line offset in inches.
   * \param offset the new dimension extension line offset in inches.
   */
  void setExtensionLineOffset ( double offset );

  /*!
   * \return the current dimension extension line offset in inches.
   */
  double extensionLineOffset ( void ) const { return current_.dimension().extensionLineOffset(); }

  /*!
   * \return the default annotation font identifier.
   */
  QString defaultAnnotationFont ( void ) const { return default_.annotation().font();}

  /*!
   * Set the annotation font.
   * \param font the new annotation font.
   */
  void setAnnotationFont ( const QString& font );

  /*!
   * \return the current annotation font.
   */
  QString annotationFont ( void ) const { return current_.annotation().font(); }

  /*!
   * \return the minimum resize handle size in inches.
   */
  double minimumHandleSize ( void ) const { return handle_minimums_.size(); }

  /*!
   * \return the maximum resize handle size in inches.
   */
  double maximumHandleSize ( void ) const { return handle_maximums_.size(); }

  /*!
   * \return the default resize handle size in inches.
   */
  double defaultHandleSize ( void ) const { return default_.handle().size(); }

  /*!
   * Set the resize handle size (edge length) in inches.
   * \param size the new resize handle size (edge length) in inches.
   */
  void setHandleSize ( double size );

  /*!
   * \return the current resize handle size in inches.
   */
  double handleSize ( void ) const { return current_.handle().size(); }

signals:
  /*!
   * Emitted when (foreground) attribute is changed. (Why so broad? Well,
   * the Dimension depends on almost all of these attributes and
   * everything else depends on almost none of them! So, distinguishing
   * among the different attributes is particularly useful.)
   */
  void attributeChanged();
  /*!
   * Emitted when a background attribute changes. These attributes
   * don't really need to be propagated to the views graphics object.
   */
  void backgroundAttributeChanged();
};
#endif // OPENGLBASE_H
