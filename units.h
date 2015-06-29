/* -*- c++ -*-
 * units.h
 *
 * Header for the Units classes
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
#ifndef UNITS_H
#define UNITS_H
/*!
 * You get a choice of units in which measurements are displayed in
 * lignumCAD: English or Metric. The English units have a further
 * subdivision into fractional or decimal representation. Internally,
 * length dimensions are stored as inches (believe it or not). The UnitsBasis
 * class knows what the current display units are and provides formating
 * and quantization services. Note that the display units are
 * a global attribute.
 */
#include <cmath>
#include <qstringlist.h>
#include <qobject.h>
#include <memory>

//! Unfortunately, there are profound differences between the
//! *presentation* of length as English or Metric units.
enum UnitSystem {
  ENGLISH,			//!< Unit is from the the Imperial system
  METRIC			//!< Unit is from the Metric system
};

//! How measurements are displayed.
enum UnitFormat {
  FRACTIONAL,			//!< Display as fraction.
  DECIMAL			//!< Display as floating point.
};

/*!
 * Retrieve the string representation of the UnitFormat enum.
 * \param format UnitFormat enum to convert to string representation.
 * \return string representation of UnitFormat.
 */
inline QString unitFormatText ( enum UnitFormat format )
{
  if ( format == FRACTIONAL )
    return "FRACTIONAL";

  return "DECIMAL";
}

/*!
 * Convert string into UnitFormat enum.
 * \param text string representation of UnitFormat enum.
 * \return UnitFormat enum described by text.
 */
inline enum UnitFormat unitFormat ( const QString& text )
{
  if ( text == "FRACTIONAL" )
    return FRACTIONAL;

  return DECIMAL;
}

/*!
 * A table of objects of this class describe the precision of lignumCAD
 * *input*, that is, how finely can you describe a point in space through
 * user input. Internally, all geometry calculations are made in double
 * precision. This just limits, or maybe a better word is: quantizes,
 * the number of significant digits you have to worry about typing
 * or mousing in.
 */
class lCPrecision {
  //! lCPrecision is determined by converting the input, which is in
  //! continuous inches, into an integer multiple of this basis,
  //! which is given in terms of numbers of the unit in an inch.
  //! For example, there are 25.4mm in an inch, so the quantized
  //! values will always be multiples of 1/25.4".
  double units_per_in_;
  //! A label suitable for spin box choices.
  QString label_;

public:
  /*!
   * Simple constructor for this class.
   */
  lCPrecision ( double units_per_in, QString label )
    : units_per_in_( units_per_in ), label_( label )
  {}

  /*!
   * \return the precision basis.
   */
  double unitsPerIn ( void ) const { return units_per_in_; }

  /*!
   * \return the precision's label.
   */
  QString label ( void ) const { return label_; }
};

/*!
 * There is one of these tables for each combination of
 * unit system, length format (except there is no METRIC/FRACTIONAL
 * entry) and abbreviation.
 */
class PrecisionTable {
  //! Name of this table (used anywhere?)
  QString name_;

protected:
  //! List of available precisions.
  QList< std::shared_ptr<lCPrecision> > precisions_;

public:
  /*!
   * Subclasses should add the required precisions in their own
   * constructor.
   */
  PrecisionTable ( const QString& name )
    : name_( name )
  {
  }

  //! Clean-up handled by list autodelete
  virtual ~PrecisionTable ( void ) {}

  /*!
   * \return the name of the table.
   */
  QString name ( void ) const { return name_; }

  /*!
   * \return number of available precisions.
   */
  int count ( void ) const { return precisions_.count(); }

  /*!
   * Retrieve the given precision without modifying the current
   * precision.
   * \param index index of the precision to return.
   * \return requested precision.
   */
  lCPrecision* precision ( int index ) const
  {
    QList< std::shared_ptr<lCPrecision> > p( precisions_ );
    return (p.value(index)).get();
  }

  /*!
   * \return index of the default precision for this table.
   */
  virtual int defaultPrecision ( void ) const = 0;

  /*!
   * \return the various available precision's labels in a list
   */
  QStringList precisionList ( void ) const
  {
    QStringList labels;

    QList< std::shared_ptr<lCPrecision> > p( precisions_ );

    for (int i=0;i<p.count();i++)
      labels << p[i]->label();

    return labels;
  }

  /*!
   * The text in the spin box is taken directly from the Precision
   * label strings.
   * \param integer value stored in spin box.
   * \return given precision string representation.
   */
  QString mapValueToText ( int value ) const { return precision(value)->label(); }

  /*!
   * If the user tries to type anything in the spin box, treat it like
   * a default entry.
   * \param text text shown in spin box (ignored).
   * \return default index.
   */
  int mapTextToValue ( const QString& /*text*/ ) const { return defaultIndex(); }
  /*!
   * \return default index for this range. Just uses the default precision.
   */
  int defaultIndex ( void ) const { return defaultPrecision(); }
  /*!
   * \return the spin box wrapping for this range. Precisions always wrap.
   */
  bool isWrapped ( void ) const { return true; }
};

/*!
 * Base class of the various length units. The primary purpose
 * of this representation is to format lengths (which are expressed
 * internally as inches) in the appropriate units for display.
 */
class LengthUnit {
  //! The system of the unit.
  const enum UnitSystem system_;
  //! The full name of the unit.
  const QString name_;
  //! An abbreviation for the unit.
  const QString abbreviation_;
  //! The default format in which the length is presented.
  const UnitFormat format_;
  //! The length of this unit in inches.
  const double in_;
  //! Whether or not it makes sense to display this unit as a fraction.
  const bool can_be_fraction_;
  //! The table of valid fractional precisions for this unit.
  const PrecisionTable* fractional_precision_table_;
  //! The table of valid decimal precisions for this unit.
  const PrecisionTable* decimal_precision_table_;

public:
  /*!
   * Make a new length unit.
   * \param system the system of the unit.
   * \param name the full name of the unit.
   * \param abbreviation an abbreviation for the unit.
   * \param format default format in which the length is presented.
   * \param in the length of the unit in inches.
   * \param can_be_fraction whether or not it makes sense to display this
   * unit as a fraction.
   * \param fractional_precision_table the table of valid fractional
   * precisions for this unit.
   * \param decimal_precision_table the table of valid decimal
   * precisions for this unit.
   */
  LengthUnit ( const enum UnitSystem system, const QString& name,
	       const QString& abbreviation,
	       enum UnitFormat format, double in, bool can_be_fraction,
	       const PrecisionTable* fractional_precision_table,
	       const PrecisionTable* decimal_precision_table )
    : system_( system ), name_( name ), abbreviation_( abbreviation ),
      format_( format ), in_( in ), can_be_fraction_( can_be_fraction ),
      fractional_precision_table_( fractional_precision_table ),
      decimal_precision_table_( decimal_precision_table )
  {}
  //! Destroy a length unit.
  virtual ~LengthUnit ( void ) {}

  /*!
   * \return the system of the unit.
   */
  enum UnitSystem system ( void ) const { return system_; }

  /*!
   * \return the full name of the unit.
   */
  QString name ( void ) const { return name_; }

  /*!
   * \return the abbreviation for the unit.
   */
  QString abbreviation ( void ) const { return abbreviation_; }

  /*!
   * \return the default format for the unit.
   */
  enum UnitFormat defaultFormat ( void ) const { return format_; }

  /*!
   * \return the length of the unit in inches.
   */
  double in ( void ) const { return in_; }

  /*!
   * Does it make sense to display the unit as a fraction?
   * \return whether or not it makes sense to display the unit as a fraction.
   */
  bool canBeFraction ( void ) const { return can_be_fraction_; }

  /*!
   * Retrieve the index of the default precision for the given format.
   * \param format format to retrieve the default precision for.
   * \return default precision index.
   */
  int defaultPrecision ( enum UnitFormat format ) const
  {
    switch ( format ) {
    case FRACTIONAL:
      if ( fractional_precision_table_ != 0 )
	return fractional_precision_table_->defaultPrecision();
      break;
    case DECIMAL:
      if ( decimal_precision_table_ != 0 )
	return decimal_precision_table_->defaultPrecision();
      break; 
    }

    return 0;
  }

  /*!
   * Retrieve the precision table for the given format.
   * \param format format to select.
   * \return selected precision table.
   */
  const PrecisionTable* precisionTable ( enum UnitFormat format ) const
  {
    switch ( format ) {
    case FRACTIONAL:
      return fractional_precision_table_;
    case DECIMAL:
      return decimal_precision_table_;
    }

    return 0;
  }

  /*!
   * Make a string list out of the precision labels for the given
   * format. Useful for combo boxes.
   * \param format format to select.
   * \return list of precision strings
   */
  QStringList precisionList ( enum UnitFormat format ) const
  {
    switch ( format ) {
    case FRACTIONAL:
      if ( fractional_precision_table_ != 0 )
	return fractional_precision_table_->precisionList();
    case DECIMAL:
      if ( decimal_precision_table_ != 0 )
	return decimal_precision_table_->precisionList();
    }
    return QStringList();
  }

  /*!
   * Retrieve the precision object corresponding to this format and
   * precision index.
   * \param format format to select.
   * \param precision index of selected precision.
   * \return selected precision entry.
   */
  lCPrecision* precision ( enum UnitFormat format, int precision ) const
  {
    switch ( format ) {
    case FRACTIONAL:
      if ( fractional_precision_table_ != 0 )
	return fractional_precision_table_->precision( precision );
      break;
    case DECIMAL:
      if ( decimal_precision_table_ != 0 )
	return decimal_precision_table_->precision( precision );
      break; 
    }

    return 0;
  }

  /*!
   * Round (quantize) the value in the given format at given precision.
   * \param format format to select the precision table.
   * \param precision index into the selected precision table.
   * \param value value to round.
   * \return rounded value.
   */
  double round ( enum UnitFormat format, int precision, double value ) const
  {
    switch ( format ) {
    case FRACTIONAL:
      if ( fractional_precision_table_ != 0 ) {
	double in = fractional_precision_table_->precision(precision)->unitsPerIn();
	return rint( in * value ) / in;
      }
      break;
    case DECIMAL:
      if ( decimal_precision_table_ != 0 ) {
	double in = decimal_precision_table_->precision( precision )->unitsPerIn();
	return rint( in * value ) / in;
      }
      break;
    }

    return value;
  }

  /*!
   * Format the given value as specified. Note: for the fractional
   * representation, these routines generate a special set of UNICODE
   * characters which OGLFT knows how to render. See OGLFT::Face::format
   * for details.
   * \param format format of generated representation.
   * \param precision requested precision of representation.
   * \param value value to format.
   * \param use_unicode select whether or not the fraction uses the
   * special UNICODE values. Default is to use them.
   * \return formatted string representation.
   */
  virtual QString format ( enum UnitFormat format, int precision,
			   double value, bool use_unicode = true ) const = 0;

  /*!
   * Take a string and return the closest value (in inches) to within
   * the given precision. Expects the number to be in the given
   * format, but we may relax this eventually...
   * \param text string to parse as a length.
   * \param format expect this format.
   * \param precision round to this precision.
   * \return the closest value in inches.
   */
  virtual double parse ( const QString& text, enum UnitFormat format,
			 int precision ) const = 0;
};

namespace Space2D {
  class Point;
  class Vector;
}
namespace Space3D {
  class Point;
}

/*!
 * The UnitsBasis class keeps track of the units the user has selected
 * for the presentation of measurements (again, internally, we use inches).
 * This class also provides the services of quantizing a value to
 * the current selected precision and is the interface through which
 * display values are formatted. This class is a singleton and is
 * accessed only through the instance() method.
 */
class UnitsBasis : public QObject {
  Q_OBJECT

  //! The list of available length unit representations.
  QList< std::shared_ptr<LengthUnit> > length_units_;

  //! Used in the Unit information dialog to select a length display unit.
  QStringList length_unit_strings_;

  //! The current format.
  enum UnitFormat format_;

  //! The precision index is an index into the PrecisionTable of the
  //! current length unit.
  int precision_index_;

  //! The singleton UnitsBasis object. Currently, this is an application
  //! wide setting even though it is saved in the model file. (So, there
  //! needs to be a default setting in the preferences dialog as well...)
  static UnitsBasis* units_basis_;

protected:
  //! Constructor makes a unit object for each possible display unit.
  UnitsBasis ( void );
  //! Destructor cleans up the units objects.
  ~UnitsBasis ( void );

public:
  /*!
   * \return the global units basis instance.
   */
  static UnitsBasis* instance ( void );

  /*!
   * Retrieve a list of formatted length unit strings. Used in the
   * list view unit selector in the model information dialog box.
   * \return list of formatted length unit strings.
   */
  QStringList lengthUnitStrings ( void ) const { return length_unit_strings_; }

  /*!
   * \return the index of the current length unit representation.
   */
  //int at ( void ) const { return length_units_.at(); }

  /*!
   * Retrieve the precision index of the current length unit representation.
   * Used as an index into the PrecisionTable of length units and as an
   * index into the lCDefaultComboBox list of choices.
   * \return index of the current length unit representation.
   */
  int precision ( void ) const { return precision_index_; }

  /*!
   * Query whether or not it makes sense to have a fractional representation
   * of the current unit.
   * \return whether or not a fractional representation makes sense for the
   * current unit.
   */
//  bool canBeFraction ( void ) const {
//    return length_units_.current()->canBeFraction();
//  }

  /*!
   * \return the full name of the current length unit.
   */
  QString name ( int i ) const { return length_units_[i]->name(); }

  /*!
   * \return the abbreviation of the current length unit.
   */
  QString abbreviation ( int i ) const
  { return length_units_[i]->abbreviation(); }

  /*!
   * \return the format of the current length unit.
   */
  enum UnitFormat format ( void ) const { return format_; }

  /*!
   * Get the index-th length unit. Does not update the current length
   * unit.
   * \param index position of length unit representation to retrieve.
   * \return selected length unit representation.
   */
  LengthUnit* lengthUnit ( int index ) const;

  /*!
   * Set the current length unit representation. Note: index is some
   * opaque value corresponding to the order of entries in the
   * length_units_ list.
   * \param index position of new length unit representation in length_units_
   * list.
   */
  void setLengthUnit ( int index );

  /*!
   * Restore the current length unit from its QSettings representation.
   * \param list QSettings representation of length unit (as a string list).
   */
  void setLengthUnit ( const QStringList& list );

  /*!
   * Select the format for the current length unit representation.
   * \param format current length unit format.
   */
  void setFormat ( enum UnitFormat format );

  /*!
   * Set the precision *index* for the current length unit representation.
   * Note, again, that this is the log-base-2 (fractional format) or
   * log-base-10 (decimal format) of the desired precision.
   * \param precision the precision *index* to use for the current
   * length unit representation.
   */
  void setPrecision ( int precision );

  /*!
   * Round (quantize) a value to be within the current precision.
   * \param x the value to quantize.
   * \return the quantized value.
   */
  double round ( double x ) const;

  /*!
   * Convenience routine to round (quantize) all the values in a 2D Point
   * to be within the current precision.
   * \param point the Point to quantize. The point itself is modified.
   */
  void round( Space2D::Point& point ) const;

  /*!
   * Convenience routine to round (quantize) all the values in a 2D Vector
   * to be within the current precision.
   * \param vector the Vector to quantize. The vector itself is modified.
   */
  void round( Space2D::Vector& vector ) const;

  /*!
   * Convenience routine to round (quantize) all the values in a 3D Point
   * to be within the current precision.
   * \param point the Point to quantize. The point itself is modified.
   */
  void round( Space3D::Point& point ) const;

  /*!
   * Format the given value using the current length unit representation.
   * \param value the value to format.
   * \param use_unicode select whether or not the fraction uses the
   * special UNICODE values. Default is to use them.
   * \return formatted string, suitable for use with OGLFT rendering if
   * use_unicode is true.
   */
  QString format ( double value, bool use_unicode = true ) const;

signals:
  /*!
   * Emitted when some aspect of the units representation changes.
   */
  void unitsChanged ( void );
};
#endif // UNITS_H
