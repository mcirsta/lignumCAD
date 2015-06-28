/* -*- c++ -*-
 * constants.h
 *
 * Constants in lignumCAD
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
#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <GL/gl.h>

#include <qstring.h>
#include <qdragobject.h>
#include <qmime.h>
#include <qvaluevector.h>

#include "ratio.h"

// Manifest constants, conversion routines and other homeless items.
// In a namespace because of their common names.

namespace lC {
  //! A value no program should be without
  const double EPSILON = 1e-6;

  //! The maximum size of any dimension in inches. Hopefully no one will want to
  //! build something larger than a mile...
  const double MAXIMUM_DIMENSION = 63360.;

  //! The minimum size of any dimension in inches. Actually, if you're careful,
  //! you can build something two miles wide.
  const double MINIMUM_DIMESION = -MAXIMUM_DIMENSION;

  //! The Version number of the LIGNUMCAD model file
  const uint FILE_VERSION = 2;

  //! The default background color for the Page views.
  const QString DEFAULT_BACKGROUND_COLOR = "#004040";

  //! The default background gradient color for the Page views.
  const QString DEFAULT_GRADIENT_COLOR = "#aaaaaa";

  //! The default background pattern for the Page views.
  const QString DEFAULT_PATTERN_FILE = "paper.png";

  //! The default geometry texture image.
  const QString DEFAULT_TEXTURE_FILE = "cherry.png";

  //! The default color for geometry edges
  const QString DEFAULT_GEOMETRY_COLOR = "#c9ce29";

  //! The default color for annotations
  const QString DEFAULT_ANNOTATION_COLOR = "#00aaaa";

  //! The default color for the sketch view grid
  const QString DEFAULT_GRID_COLOR = "#555555";

  //! A constraint's primary reference is drawn in this color.
  const QString DEFAULT_CONSTRAINT_PRIMARY_COLOR = "#bc25bc";

  //! A constraint's secondary reference is drawn in this color.
  const QString DEFAULT_CONSTRAINT_SECONDARY_COLOR = "#5fbc25";

  //! By default, we'll try to use the users UI font.
  const QString DEFAULT_FONT = QString::null;

  //! Arrow head style.
  enum ArrowHeadStyle { OPEN, HOLLOW, FILLED };

  //! The default arrow head style.
  const ArrowHeadStyle DEFAULT_ARROW_HEAD_STYLE = FILLED;

  //! The minimum arrow head length (in inches) the user can specify.
  const double MINIMUM_ARROW_HEAD_LENGTH = 1. / 64.;

  //! The default arrow head length (in inches).
  const double DEFAULT_ARROW_HEAD_LENGTH = 3. / 16.;

  //! The maximum arrow head length (in inches) the user can specify.
  const double MAXIMUM_ARROW_HEAD_LENGTH = 2.;

  //! The minimum arrow head width to length ratio the user can specify.
  const Ratio MINIMUM_ARROW_HEAD_WIDTH_RATIO(2,16);

  //! The default arrow head width to length ratio.
  const Ratio DEFAULT_ARROW_HEAD_WIDTH_RATIO(2,8);

  //! The maximum arrow head width to length ratio the user can specify.
  const Ratio MAXIMUM_ARROW_HEAD_WIDTH_RATIO(4,2);

  // I think a useful addition to C++ would be the automatic generation
  // of enumerated type values as strings.

  /*!
   * Convert an ArrowHeadStyle enumeration into a string. Mostly for
   * serialization since this value is not translated.
   * \param style an arrow head style.
   * \return the string representation of the arrow head style.
   */
  inline QString arrowHeadStyleText ( ArrowHeadStyle style )
  {
    if ( style == OPEN )
      return "OPEN";
    else if ( style == HOLLOW )
      return "HOLLOW";

    return "FILLED";
  }

  /*!
   * Convert a string into an ArrowHeadStyle enumeration. Mostly for
   * serialization since this value is not translated.
   * \param text string containing an arrow head style.
   * \return corresponding ArrowHeadStyle enumeration.
   */
  inline ArrowHeadStyle arrowHeadStyle ( const QString& text )
  {
    if ( text == "OPEN" )
      return OPEN;
    else if ( text == "HOLLOW" )
      return HOLLOW;

    return FILLED;
  }

  // Resize handle defaults
  const double MINIMUM_HANDLE_SIZE = 1. / 64.; // Inches, 1/2 edge length
  const double DEFAULT_HANDLE_SIZE = 1. / 16.; // Inches, 1/2 edge length
  const double MAXIMUM_HANDLE_SIZE = 1. / 2.; // Inches, 1/2 edge length

  // Dimension defaults
  const double MINIMUM_DIMENSION_CLEARANCE = 1. / 64.; // Inches
  const double DEFAULT_DIMENSION_CLEARANCE = 1. / 8.; // Inches
  const double MAXIMUM_DIMENSION_CLEARANCE = 1.; // Inches
  const double MINIMUM_DIMENSION_LINE_THICKNESS = 1. / 64.; // Inches
  const double DEFAULT_DIMENSION_LINE_THICKNESS = 1. / 64.; // Inches
  const double MAXIMUM_DIMENSION_LINE_THICKNESS = 1. / 8.; // Inches
  const double MINIMUM_EXTENSION_LINE_OFFSET = 0.; // Inches
  const double DEFAULT_EXTENSION_LINE_OFFSET = 1. / 16.; // Inches
  const double MAXIMUM_EXTENSION_LINE_OFFSET = 1. / 8.; // Inches

  // Location of UNICODE private code page
  const ushort UNICODE_PRIVATE_PAGE = 0xE000;

  // Background styles. For now this is in an interior namespace
  // due to the (re)use of SOLID.
  namespace Background {
    enum Style { SOLID, GRADIENT, PATTERN };

    inline QString backgroundStyleText ( Style style )
    {
      if ( style == SOLID )
	return "SOLID";
      else if ( style == GRADIENT )
	return "GRADIENT";

      return "PATTERN";
    }

    inline Style backgroundStyle ( const QString& text )
    {
      if ( text == "SOLID" )
	return SOLID;
      else if ( text == "GRADIENT" )
	return GRADIENT;

      return PATTERN;
    }
  }

  // An annotation (usually a dimension) can be oriented above or below
  // its reference object, which can be either an edge or an entire figure
  typedef enum { ABOVE, BELOW } AnnotationSide;

  inline QString annotationSideText ( AnnotationSide annotation_side )
  {
    if ( annotation_side == ABOVE )
      return "ABOVE";

    return "BELOW";
  }

  inline AnnotationSide annotationSide ( const QString& text )
  {
    if ( text == "ABOVE" )
      return ABOVE;

    return BELOW;
  }

  // The orientation of various entities
  typedef enum { HORIZONTAL, VERTICAL, OTHER } Orientation;

  inline QString orientationText ( Orientation orientation )
  {
    switch ( orientation ) {
    case HORIZONTAL:
      return "HORIZONTAL";
    case VERTICAL:
      return "VERTICAL";
    default:
      return "OTHER";
    }
  }

  inline Orientation orientation ( const QString& text )
  {
    if ( text == "HORIZONTAL" )
      return HORIZONTAL;
    else if ( text == "VERTICAL" )
      return VERTICAL;

    return OTHER;
  }

  // Well, SOLID is overworked...
  namespace Render {
    //! Rendering styles for geometry. PARENT is for when the View overrides
    //! the rendering of its children.
    typedef enum {
      PARENT,			//!< Style inherited from page default.
      STIPPLE,			//!< Use a (possibly) stippled line.
      WIREFRAME,		//!< Outline, or hollow, mode.
      HIDDEN,			//!< (3D) Hidden lines removed (or translucent).
      SOLID,			//!< Filled with a single color.
      TEXTURED,			//!< Fill with a texture.
      HIGHLIGHTS		//!< Draw highlights only (internal).
    } Style;

    /*!
     * Convert a RenderStyle enumeration into a string. Mostly for
     * serialization since this value is not translated.
     * \param style a rendering style.
     * \return the string representation of the rendering style.
     */
    inline QString styleText ( Style style )
    {
      switch ( style ) {
      case STIPPLE:
	return "STIPPLE";
      case WIREFRAME:
	return "WIREFRAME";
      case HIDDEN:
	return "HIDDEN";
      case SOLID:
	return "SOLID";
      case TEXTURED:
	return "TEXTURED";
      }

      return "PARENT";
    }

    /*!
     * Convert a string into an RenderStyle enumeration. Mostly for
     * serialization since this value is not translated.
     * \param text string containing a rendering style.
     * \return corresponding RenderStyle enumeration.
     */
    inline Style style ( const QString& text )
    {
      if ( text == "STIPPLE" )
	return STIPPLE;
      else if ( text == "WIREFRAME" )
	return WIREFRAME;
      else if ( text == "HIDDEN" )
	return HIDDEN;
      else if ( text == "SOLID" )
	return SOLID;
      else if ( text == "TEXTURED" )
	return TEXTURED;

      return PARENT;
    }

    //! Various modes in which elements can be rendered
    typedef enum {
      INVISIBLE,		//<! Not drawn (usually).
      REGULAR,			//<! Standard representation.
      HIGHLIGHTED,		//<! Transiently emphasized.
      ACTIVATED			//<! Strong, fixed hightlight.
    } Mode;
  } // End of Render namespace

  namespace Edge {
    //! Predefined stipple patterns for edges.
    enum Style {
      SOLID,			//!< solid line.
      DASH,			//!< dashed line.
      DOT,			//!< dotted line.
      DASHDOT,			//!< dash-dotted line.
      DASHDOTDOT		//!< dash-dot-dotted line.
    };

    /*!
     * Retrieve the string representation of the Edge::Style enum.
     * \param edge Edge::Style enum to convert to string representation.
     * \return string representation of edge style.
     */
    inline QString styleText ( enum Style edge )
    {
      switch ( edge ) {
      case DASH:
	return "DASH";
      case DOT:
	return "DOT";
      case DASHDOT:
	return "DASHDOT";
      case DASHDOTDOT:
	return "DASHDOTDOT";
      }
      return "SOLID";
    }

    /*!
     * Convert string into Edge::Style enum.
     * \param text string representation of Edge::Style enum.
     * \return Edge::Style enum described by text.
     */
    inline enum Style style ( const QString& text )
    {
      if ( text == "DASH" )
	return DASH;
      else if ( text == "DOT" )
	return DOT;
      else if ( text == "DASHDOT" )
	return DASHDOT;
      else if ( text == "DASHDOTDOT" )
	return DASHDOTDOT;

      return SOLID;
    }
  } // End of Edge namespace

  // Text justification values. (Identical to the values in OGLFT.)

  typedef enum { LEFT, ORIGIN, CENTER, RIGHT } HorizontalJustification;
  typedef enum { BOTTOM, BASELINE, MIDDLE, TOP } VerticalJustification;
  

  // Look up the images which are loaded at compile time
  inline QPixmap lookupPixmap ( const QString name )
  {
    const QMimeSource* mime_source =
      QMimeSourceFactory::defaultFactory()->data( name );

    if ( mime_source == 0 )
      return QPixmap();

    QPixmap pixmap;
    QImageDrag::decode( mime_source, pixmap );
    return pixmap;
  }

  // Look up the images which are loaded at compile time
  inline QImage lookupImage ( const QString name )
  {
    const QMimeSource* mime_source =
      QMimeSourceFactory::defaultFactory()->data( name );

    if ( mime_source == 0 )
      return QImage();

    QImage image;
    QImageDrag::decode( mime_source, image );
    return image;
  }

  // The columns of the Model Hierarchy View (why use a single digit
  // when you can use a 10 character name?)
  typedef enum { NAME, TYPE, DETAIL } HierarchyListColumns;

  // As above, this should be configurable, both in units and value...
  const double MINIMUM_SIZE = 1./64.;

  // Convert a QColor into something acceptable to OpenGL directly

  inline GLubyte* qCubv ( const QColor& color )
  {
    static GLubyte ubv[4];
    ubv[0] = qRed( color.rgb() );
    ubv[1] = qGreen( color.rgb() );
    ubv[2] = qBlue( color.rgb() );
    ubv[3] = qAlpha( color.rgb() );
    return ubv;
  }
 
  inline GLfloat* qCfv ( const QColor& color )
  {
    static GLfloat fv[4];
    fv[0] = qRed( color.rgb() ) / 255.;
    fv[1] = qGreen( color.rgb() ) / 255.;
    fv[2] = qBlue( color.rgb() ) / 255.;
    fv[3] = qAlpha( color.rgb() ) / 255.;
    return fv;
  }

  /*!
   * Interestingly, the default conversion of a double precision
   * number to a QString is only 6 significant digits. We need a
   * lot more!
   * \param value floating point value to convert to a string.
   * \return high-precision QString representation of value.
   */
  inline QString format ( double value )
  {
    return QString::number( value, 'g', 13 );
  }
  /*!
   * Convert a name which may contain special characters into something
   * which looks reasonable when displayed.
   * \param name encoded model item name.
   * \return decoded model item name.
   */
  inline QString formatName ( const QString& name )
  {
    QString formatted_name;

    // This is pretty complicated.
    for ( uint i = 0; i < name.length(); ) {
      // The quoting character (thanks to the definition of the URI) is
      // the '%'. Let's look at each case:
      if ( name[i] == '%' ) {
	if ( ( i + 2 ) < name.length() ) {
	  i++;
	  formatted_name += QChar( name.mid(i,2).toUShort( 0, 16 ) );
	  i += 2;
	}
	else {
	  i++;
	  formatted_name += '%';
	}
      }
      else
	formatted_name += name[i++];
    }

    return formatted_name;
  }
  /*!
   * Convert a name which may contain special characters into something
   * which looks reasonable when displayed as QTab text.
   * \param name encoded model item name.
   * \return decoded model item name.
   */
  inline QString formatTabName ( const QString& name )
  {
    bool found_amp = false;
    QString formatted_name;

    // Unquote %xx values and double up &'s.
    for ( uint i = 0; i < name.length(); ) {
      if ( name[i] == '%' ) {
	if ( ( i + 2 ) < name.length() ) {
	  i++;
	  formatted_name += QChar( name.mid(i,2).toUShort( 0, 16 ) );
	  i += 2;
	}
	else {
	  i++;
	  formatted_name += '%';
	}
	found_amp = false;
      }
      else if ( name[i] == '&' && !found_amp ) {
	formatted_name += "&&";
	found_amp = true;
	i++;
      }
      else {
	if ( name[i] != '&' )
	  found_amp = false;
	formatted_name += name[i++];
      }
    }

    return formatted_name;
  }
  /*!
   * Convert an ID array into a string.
   * \param ID Full id path of object.
   */
  inline QString idToString ( const QValueVector<uint>& ID )
  {
    QString id_string;

    if ( ID.size() < 1 ) return QString::null;

    id_string = QString::number( ID[0] );

    for ( uint i = 1; i < ID.size(); ++i )
      id_string += QString( " : %1" ).arg( ID[i] );

    return id_string;
  }
  // Data structure shared between Items and Views to determine
  // if resizing operation is allowed.
  struct ValidDelta {
    bool dx_ok_, dy_ok_, dz_ok_;
    ValidDelta ( bool dx_ok = true, bool dy_ok = true, bool dz_ok = true )
      : dx_ok_( dx_ok ), dy_ok_( dy_ok ), dz_ok_( dz_ok )
    {}
  };

  //! Must prevent the user from naming two model items with the same
  //! name. These are the possible return values from uniqueness checkers.
  typedef enum {
    OK,				//!< New name is unique.
    Rejected,			//!< Not unique; user doesn't want the change.
    Redo			//!< Not unique; user wants to try again.
  } RenameStatus;

  // These are frequently used constant strings which can be shared
  // but are not necessarily shown to the user (so some are not translated).
  // These are more or less enumeration types.
  namespace STR {
    const QString ALIGN               = QT_TRANSLATE_NOOP( "Constants", "align" );
    const QString ALIGN_OFFSET        = QT_TRANSLATE_NOOP( "Constants", "align-offset" );
    const QString ANNOTATION          = QT_TRANSLATE_NOOP( "Constants", "annotation" );
    const QString ANNOTATION_SIDE     = "annotation-side";
    const QString ANNOTATION_VIEW     = "annotation-view";
    const QString ARBITRARY_CONSTRAINT = "arbitrary-constraint";
    const QString ASSEMBLY            = QT_TRANSLATE_NOOP( "Constants", "assembly" );
    const QString ASSEMBLY_VIEW       = "assembly-view";
    const QString AXIS                = QT_TRANSLATE_NOOP( "Constants", "axis" );
    const QString BACK                = "back";
    const QString BASE                = "base";
    const QString BASE_DIAMETER       = QT_TRANSLATE_NOOP( "Constants", "base diameter" );
    const QString BASE_LENGTH         = QT_TRANSLATE_NOOP( "Constants", "base length" );
    const QString BLANKS              = QT_TRANSLATE_NOOP( "Constants", "blanks" );
    const QString BOARD               = QT_TRANSLATE_NOOP( "Constants", "board" );
    const QString BOTTOM              = "bottom";
    const QString BOTTOM_WIDTH        = QT_TRANSLATE_NOOP( "Constants", "bottom width" );
    const QString BOUNDARY            = "boundary";
    const QString BSPCYL              = "bspcyl";
    const QString BSP_DIAMETER        = "bsp-diameter";
    const QString CENTERED_CONSTRAINT = "centered-constraint";
    const QString CENTERLINE          = QT_TRANSLATE_NOOP( "Constants", "centerline" );
    const QString CENTERLINE_VIEW     = "centerline-view";
    const QString CHANGE_ANNOTATION   = "change-annotation";
    const QString CHANGE_MATERIAL     = "change-material";
    const QString CHANGE_OFFSET       = "change-offset";
    const QString CHANGE_VIEW         = "change-view";
    const QString COINCIDENT_CONSTRAINT = "coincident-constraint";
    const QString CON_BOTTOM_DIAMETER = "con-bottom-diameter";
    const QString CON_LENGTH          = "con-length";
    const QString CON_TOP_DIAMETER    = "con-top-diameter";
    const QString CONSTRAINT          = QT_TRANSLATE_NOOP( "Constants", "constraint" );
    const QString CONSTRAINTS         = "constraints";
    const QString CONSTRAINED_LINE    = QT_TRANSLATE_NOOP( "Constants", "constrained-line" );
    const QString CONSTRAINED_PLANE   = "constrained-plane";
    const QString CONSTRAINT_UNDEFINED = QT_TRANSLATE_NOOP( "Constants", "constraint undefined" );
    const QString CONTOUR             = "contour";
    const QString CREATE              = "create";
    const QString CREATE_CONSTRAINT   = "create-constraint";
    const QString CREATED             = "created";
    const QString CYL                 = "cyl";
    const QString CYL_LENGTH          = "cyl-length";
    const QString CYL_DIAMETER        = "cyl-diameter";
    const QString DB_URL_PATTERN      = "%1/%2.%3";
    const QString DB_PREFIX           = "db:///";
    const QString DELETE              = "delete";
    const QString DELETE_CONSTRAINT   = "delete-constraint";
    const QString DESCRIPTION         = "description";
    const QString DIAMETER            = QT_TRANSLATE_NOOP( "Constants", "diameter");
    const QString DIMENSION           = "dimension";
    const QString DIMENSION_OFFSET    = "dimension-offset";
    const QString DIMENSION_VIEW      = "dimension-view";
    const QString DISPLAY_OPTIONS     = "display-options";
    const QString DRAWING             = QT_TRANSLATE_NOOP( "Constants", "drawing" );
    const QString DRAWING_VIEW        = "drawing-view";
    const QString EDGE                = "edge";
    const QString EXTENSION_LINE_OFFSET = "extension-line-offset";
    const QString FACE                = "face";
    const QString FIGURE              = "figure";
    const QString FROM                = "from";
    const QString FRONT               = "front";
    const QString FRONT_TAPER         = "front-taper";
    const QString HEIGHT              = "height";
    const QString HEIGHT_ANNOTATION_SIDE = "height-annotation-side";
    const QString ID                  = "id";
    const QString IMAGES              = "images";
    const QString KNOBS               = QT_TRANSLATE_NOOP( "Constants", "knobs" );
    const QString LCAD_FILE_EXT       = ".lcad";
    const QString LEFT                = "left";
    const QString LEGS                = QT_TRANSLATE_NOOP( "Constants", "legs" );
    const QString LENGTH              = "length";
    const QString LENGTH_GRAIN        = QT_TRANSLATE_NOOP( "Constants", "length (in grain direction)" );
    const QString LINE                = QT_TRANSLATE_NOOP( "Constants", "line" );
    const QString MATE                = QT_TRANSLATE_NOOP( "Constants", "mate" );
    const QString MATE_OFFSET         = QT_TRANSLATE_NOOP( "Constants", "mate-offset" );
    const QString MATERIAL            = "material";
    const QString MATERIALS           = "materials";
    const QString MEMENTO             = "memento";
    const QString MODEL               = QT_TRANSLATE_NOOP( "Constants", "model" );
    const QString MODIFIED            = "modified";
    const QString MODIFY_SOLID        = "modify-solid";
    const QString MOVE_ANNOTATION     = "move-annotation";
    const QString MOVE_DIMENSION      = "move-dimension";
    const QString MOVE_LINE           = "move-line";
    const QString MOVE_LINES          = "move-lines";
    const QString MOVE_RECTANGLE      = "move-rectangle";
    const QString NAME                = "name";
    const QString NAME_ID             = "%1<%2>";
    const QString NEW_ANNOTATION_SIDE = "new-annotation-side";
    const QString NEW_CONSTRAINTS     = "new-constraints";
    const QString NEW_DIMENSION_OFFSET = "new-dimension-offset";
    const QString NEW_EXTENSION_OFFSET = "new-extension-offset";
    const QString NEW_HEIGHT          = "new-height";
    const QString NEW_MATERIAL        = "new-material";
    const QString NEW_OFFSET          = "new-offset";
    const QString NEW_PLACEMENT       = "new-placement";
    const QString NEW_STYLE           = "new-style";
    const QString NEW_TEXT            = "new-text";
    const QString NEW_URL             = "new-url";
    const QString NEW_VALUE           = "new-value";
    const QString NEW_VIEW            = "new-view";
    const QString NEW_WIDTH           = "new-width";
    const QString NEW_X               = "new-x";
    const QString NEW_Y               = "new-y";
    const QString NONE                = QT_TRANSLATE_NOOP( "Constants", "none" );
    const QString OFFSET              = "offset";
    const QString OFFSET_CONSTRAINT   = "offset-constraint";
    const QString OLD_ANNOTATION_SIDE = "old-annotation-side";
    const QString OLD_CONSTRAINTS     = "old-constraints";
    const QString OLD_DIMENSION_OFFSET = "old-dimension-offset";
    const QString OLD_EXTENSION_OFFSET = "old-extension-offset";
    const QString OLD_HEIGHT          = "old-height";
    const QString OLD_MATERIAL        = "old-material";
    const QString OLD_OFFSET          = "old-offset";
    const QString OLD_PLACEMENT       = "old-placement";
    const QString OLD_STYLE           = "old-style";
    const QString OLD_TEXT            = "old-text";
    const QString OLD_URL             = "old-url";
    const QString OLD_VALUE           = "old-value";
    const QString OLD_VIEW            = "old-view";
    const QString OLD_WIDTH           = "old-width";
    const QString OLD_X               = "old-x";
    const QString OLD_Y               = "old-y";
    const QString ORIENTATION         = "orientation";
    const QString ORIGIN_CSYS         = "origin-csys";
    const QString PAGE                = "page";
    const QString PAGES               = "pages";
    const QString PGP                 = "pgp";
    const QString PARAMETER           = "parameter";
    const QString PARENT              = "parent";
    const QString PART                = QT_TRANSLATE_NOOP( "Constants", "part" );
    const QString PART_VIEW           = "part-view";
    const QString PATH_PATTERN        = "%1/%2";
    const QString PHASE               = "phase";
    const QString PLACEMENT           = "placement";
    const QString PLANE               = QT_TRANSLATE_NOOP( "Constants", "plane" );
    const QString PYR_WIDTH           = "pyramidal-width";
    const QString RATIO_INDEX         = "ratio-index";
    const QString REC_LENGTH          = "rectangular-length";
    const QString REC_WIDTH           = "rectangular-width";
    const QString RECONSTRAINTS       = "reconstraints";
    const QString RECTANGLE           = QT_TRANSLATE_NOOP( "Constants", "rectangle" );
    const QString RECTANGLE_FMT       = QT_TRANSLATE_NOOP( "Constants", "origin (%1,%2), width %3, height %4" );
    const QString RECTANGLE_VIEW      = "rectangle-view";
    const QString REFERENCE_LINE      = QT_TRANSLATE_NOOP( "Constants", "reference-line" );
    const QString REFERENCE_LINE_FMT  = QT_TRANSLATE_NOOP( "Constants", "origin (%1,%2), direction (%3,%4)" );
    const QString REFERENCE_LINE_VIEW = "reference-line-view";
    const QString RENAME              = "rename";
    const QString RENDER_STYLE        = "render-style";
    const QString RESTYLE_RECTANGLE   = "restyle-rectangle";
    const QString REVISION            = "revision";
    const QString RIGHT               = "right";
    const QString RIGHT_TAPER         = "right-taper";
    const QString ROLE                = "role";
    const QString RPP                 = "rpp";
    const QString RPPCON              = "rppcon";
    const QString RPPPYR              = "rpppyr";
    const QString SCALE               = "scale";
    const QString SHANK               = "shank";
    const QString SKETCH              = QT_TRANSLATE_NOOP( "Constants", "sketch" );
    const QString SKETCH_VIEW         = "sketch-view";
    const QString SOLID               = QT_TRANSLATE_NOOP( "Constants", "solid" );
    const QString SOLID_VIEW          = "solid-view";
    const QString SPACE2D             = "SPACE2D";
    const QString STYLE               = "style";
    const QString SUBASSEMBLY         = QT_TRANSLATE_NOOP( "Constants", "subassembly" );
    const QString SUBASSEMBLY_VIEW    = "subassembly-view";
    const QString SURFACE             = "surface";
    const QString SURFACE0            = "surface0";
    const QString SURFACE1            = "surface1";
    const QString TAPERED             = QT_TRANSLATE_NOOP( "Constants", "tapered" );
    const QString TEXTURE             = "texture";
    const QString THICKNESS           = QT_TRANSLATE_NOOP( "Constants","thickness");
    const QString TO                  = "to";
    const QString TOP                 = "top";
    const QString TOP_LENGTH          = QT_TRANSLATE_NOOP( "Constants", "top length" );
    const QString TOP_WIDTH           = QT_TRANSLATE_NOOP( "Constants", "top width" );
    const QString TRANSISTION         = "transistion";
    const QString TURNED              = QT_TRANSLATE_NOOP( "Constants", "turned" );
    const QString TURNING             = QT_TRANSLATE_NOOP( "Constants", "turning" );
    const QString TURNING_LENGTH      = QT_TRANSLATE_NOOP( "Constants", "turning length");
    const QString TURNING_BOTTOM_DIAMETER= QT_TRANSLATE_NOOP( "Constants", "turning bottom diameter");
    const QString TURNING_TOP_DIAMETER= QT_TRANSLATE_NOOP( "Constants", "turning top diameter");
    const QString TYPE                = "type";
    const QString UI_SELECTION        = "UI-SELECTION";
    const QString UNCONSTRAINTS       = "unconstraints";
    const QString UNDEFINED           = QT_TRANSLATE_NOOP( "Constants", "undefined" );
    const QString URL                 = "url";
    const QString VALUE               = "value";
    const QString VALUE_UNIT          = "%1%2";
    const QString VERSION             = "version";
    const QString VIEW_DATA           = "view-data";
    const QString VIEW_POINT          = "view-point";
    const QString VIEW                = "view";
    const QString VIEW_EXT            = "-view";
    const QString VIEWS               = "views";
    const QString WIDTH               = QT_TRANSLATE_NOOP( "Constants", "width" );
    const QString WIDTH_ANNOTATION_SIDE = "width-annotation-side";
    const QString WIREFRAME           = "wireframe";
    const QString X                   = "x";
    const QString X_AXIS              = "X_AXIS";
    const QString X0                  = "x0";
    const QString X1                  = "x1";
    const QString XML                 = "xml";
    const QString XML_FILE_PATERN     = "*.xml";
    const QString Y                   = "y";
    const QString Y_AXIS              = "Y_AXIS";
    const QString Y0                  = "y0";
    const QString Y0_ANGLE            = "y0-angle";
    const QString Y1                  = "y1";
    const QString Z                   = "z";
    const QString Z0_ANGLE            = "z0-angle";
    const QString Z1_ANGLE            = "z1-angle";
  }
};

#endif // CONSTANTS_H
