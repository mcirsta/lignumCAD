/*
 * units.cpp
 *
 * Unit classes
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
#include <qapplication.h>
#include <qregexp.h>

#include "constants.h"
#include "vectoralgebra.h"
#include "units.h"

// Some manifest constants and conversion factors.
namespace {
  const double INCHES_PER_FOOT = 12.;
  const double FEET_PER_YARD   = 3.;
  const double INCHES_PER_YARD = ( INCHES_PER_FOOT * FEET_PER_YARD );
  const double MM_PER_INCH     = 25.4;
  const double CM_PER_INCH     = ( MM_PER_INCH / 10. );
  const double M_PER_INCH      = ( CM_PER_INCH / 100. );

  // Do these need to be translated...?
  const QString INCH_QUOTE_ABBREV = "\"";
  const QString INCH_IN_ABBREV    = "in";
  const QString FOOT_QUOTE_ABBREV = "′";
  const QString FOOT_FT_ABBREV    = "ft";
  const QString YARD_ABBREV       = "yd";
  const QString MILLIMETER_ABBREV = "mm";
  const QString CENTIMETER_ABBREV = "cm";
  const QString METER_ABBREV      = "m";
};

// The tables of valid precisions.

class EnglishFractionalQuote : public PrecisionTable {
public:
  EnglishFractionalQuote ( void )
    : PrecisionTable( "English Fractional Quote" )
  {
    precisions_.append( std::shared_ptr<lCPrecision>(new lCPrecision( 1., lC::STR::VALUE_UNIT.arg("1").arg( INCH_QUOTE_ABBREV ) )) );
    precisions_.append( std::shared_ptr<lCPrecision>(new lCPrecision( 2., lC::STR::VALUE_UNIT.arg("1/2").arg( INCH_QUOTE_ABBREV )) ) );
    precisions_.append( std::shared_ptr<lCPrecision>(new lCPrecision( 4., lC::STR::VALUE_UNIT.arg("1/4").arg( INCH_QUOTE_ABBREV )) ) );
    precisions_.append( std::shared_ptr<lCPrecision>(new lCPrecision( 8., lC::STR::VALUE_UNIT.arg("1/8").arg( INCH_QUOTE_ABBREV )) ) );
    precisions_.append( std::shared_ptr<lCPrecision>(new lCPrecision( 16., lC::STR::VALUE_UNIT.arg("1/16").arg( INCH_QUOTE_ABBREV )) ) );
    precisions_.append( std::shared_ptr<lCPrecision>(new lCPrecision( 32., lC::STR::VALUE_UNIT.arg("1/32").arg( INCH_QUOTE_ABBREV )) ) );
    precisions_.append( std::shared_ptr<lCPrecision>(new lCPrecision( 64., lC::STR::VALUE_UNIT.arg("1/64").arg( INCH_QUOTE_ABBREV )) ) );
  }

  // Default precision is 1/8"
  int defaultPrecision ( void ) const { return 3; }
};

class EnglishFractionalIn : public PrecisionTable {
public:
  EnglishFractionalIn ( void )
    : PrecisionTable( "English Fractional In" )
  {
    precisions_.append( std::shared_ptr<lCPrecision>(new lCPrecision( 1., lC::STR::VALUE_UNIT.arg("1").arg(INCH_IN_ABBREV) )) );
    precisions_.append( std::shared_ptr<lCPrecision>(new lCPrecision( 2., lC::STR::VALUE_UNIT.arg("1/2").arg(INCH_IN_ABBREV )) ));
    precisions_.append( std::shared_ptr<lCPrecision>(new lCPrecision( 4., lC::STR::VALUE_UNIT.arg("1/4").arg(INCH_IN_ABBREV )) ));
    precisions_.append( std::shared_ptr<lCPrecision>(new lCPrecision( 8., lC::STR::VALUE_UNIT.arg("1/8").arg(INCH_IN_ABBREV )) ));
    precisions_.append( std::shared_ptr<lCPrecision>(new lCPrecision( 16., lC::STR::VALUE_UNIT.arg("1/16").arg(INCH_IN_ABBREV))));
    precisions_.append( std::shared_ptr<lCPrecision>(new lCPrecision( 32., lC::STR::VALUE_UNIT.arg("1/32").arg(INCH_IN_ABBREV))));
    precisions_.append( std::shared_ptr<lCPrecision>(new lCPrecision( 64., lC::STR::VALUE_UNIT.arg("1/64").arg(INCH_IN_ABBREV))));
  }

  // Default precision is 1/8"
  int defaultPrecision ( void ) const { return 3; }
};

class EnglishDecimalQuote : public PrecisionTable {
public:
  EnglishDecimalQuote ( void )
    : PrecisionTable( "English Decimal Quote" )
  {
    precisions_.append( std::shared_ptr<lCPrecision>(new lCPrecision( 1., lC::STR::VALUE_UNIT.arg("1").arg(INCH_QUOTE_ABBREV)) ) );
    precisions_.append( std::shared_ptr<lCPrecision>(new lCPrecision( 10., lC::STR::VALUE_UNIT.arg("0.1").arg(INCH_QUOTE_ABBREV ))));
    precisions_.append( std::shared_ptr<lCPrecision>(new lCPrecision( 100.,lC::STR::VALUE_UNIT.arg("0.01").arg(INCH_QUOTE_ABBREV))));
    precisions_.append( std::shared_ptr<lCPrecision>(new lCPrecision(1000.,lC::STR::VALUE_UNIT.arg("0.001").arg(INCH_QUOTE_ABBREV))));
  }

  // Default precision is 0.1"
  int defaultPrecision ( void ) const { return 1; }
};

class EnglishDecimalIn : public PrecisionTable {
public:
  EnglishDecimalIn ( void )
    : PrecisionTable( "English Decimal In" )
  {
    precisions_.append( std::shared_ptr<lCPrecision>(new lCPrecision( 1., lC::STR::VALUE_UNIT.arg("1").arg(INCH_IN_ABBREV) )) );
    precisions_.append( std::shared_ptr<lCPrecision>(new lCPrecision( 10., lC::STR::VALUE_UNIT.arg("0.1").arg(INCH_IN_ABBREV ))));
    precisions_.append( std::shared_ptr<lCPrecision>(new lCPrecision( 100.,lC::STR::VALUE_UNIT.arg("0.01").arg(INCH_IN_ABBREV))));
    precisions_.append( std::shared_ptr<lCPrecision>(new lCPrecision(1000.,lC::STR::VALUE_UNIT.arg("0.001").arg(INCH_IN_ABBREV))));
  }

  // Default precision is 0.1"
  int defaultPrecision ( void ) const { return 1; }
};

class MetricDecimal : public PrecisionTable {
public:
  MetricDecimal ( void )
    : PrecisionTable( "Metric Decimal" )
  {
    precisions_.append( std::shared_ptr<lCPrecision>( new lCPrecision( MM_PER_INCH / 100.,
                       lC::STR::VALUE_UNIT.arg("10").arg(CENTIMETER_ABBREV) )));
    precisions_.append( std::shared_ptr<lCPrecision>(new lCPrecision( MM_PER_INCH / 10.,
                       lC::STR::VALUE_UNIT.arg("1").arg(CENTIMETER_ABBREV) )));
    precisions_.append( std::shared_ptr<lCPrecision>(new lCPrecision( MM_PER_INCH,
                       lC::STR::VALUE_UNIT.arg("1").arg(MILLIMETER_ABBREV) )));
    precisions_.append( std::shared_ptr<lCPrecision>(new lCPrecision( MM_PER_INCH * 10.,
                       lC::STR::VALUE_UNIT.arg("0.1").arg(MILLIMETER_ABBREV))));
  }

  // Default precision is 1mm
  int defaultPrecision ( void ) const { return 2; }
};


// Instantiations of each precision table

static EnglishFractionalQuote english_fractional_quote;
static EnglishFractionalIn english_fractional_in;
static EnglishDecimalQuote english_decimal_quote;
static EnglishDecimalIn english_decimal_in;
static MetricDecimal metric_decimal;

/*!
 * The inch unit of length.
 */
class Inch : public LengthUnit
{
public:
  Inch ( const QString& abbreviation,
	 const PrecisionTable* fractional_precision_table,
	 const PrecisionTable* decimal_precision_table )
    : LengthUnit( ENGLISH, qApp->translate( "Precision", "Inches" ),
		  abbreviation, FRACTIONAL, 1, true, fractional_precision_table,
		  decimal_precision_table )
  {}

  QString format ( enum UnitFormat format, int precision, double value,
		   bool use_unicode ) const
  {
    switch ( format ) {
    case FRACTIONAL:
      {
	// The limit of fractional values to 1/256-ths is hardcoded.
	QString formatted_value;
	if ( fabs( value ) < 1./256. )
	  formatted_value = "0";
	else {
	  // Extract the integral part
	  int a = (int)value;

	  if ( a != 0 )
	    formatted_value = QString::number( a );

	  // Extract the fractional part: NOTE: THIS IS LIMITED TO
	  // REPRESENTING ALL FRACTIONS AS n/256!
	  int b = (int)rint( 256 * fabs( value - a ) );

	  // If b is exactly 256, then the original number was essentially
	  // an integer (to withing 1/256-th)
	  if ( b == 256 )
	    formatted_value = QString::number( rint( value ) );

	  else if ( b != 0 ) {
	    int c = 256;

	    if ( !formatted_value.isEmpty() )
	      formatted_value += QChar( 0xa0 );
	    else if ( value < 0 )
	      formatted_value = "-";

	    // If required, shift to the 0xE000 private UNICODE
	    // page using the old trick of subtracting the character '0'
	    // from another numeric character to find the integral value.
	    ushort num_unicode_offset =
	      lC::UNICODE_PRIVATE_PAGE - QChar('0').unicode();
	    ushort den_unicode_offset =
	      lC::UNICODE_PRIVATE_PAGE + 0x10 - QChar('0').unicode();

	    if ( !use_unicode ) {
	      num_unicode_offset = 0;
	      den_unicode_offset = 0;
	    }

	    // Remove common factors of two from the numerator and denominator
	    // (i.e. (b&1) == 0 implies b is even)
	    for ( ; ( b & 1 ) == 0; b >>= 1, c >>= 1 );

	    // Format the numerator and shift to the 0xE000 sequence
	    QString numerator = QString::number( b );
        for ( int i = 0; i < numerator.length(); i++ )
          numerator[i] = QChar( numerator.at(i).unicode() +
				       num_unicode_offset );
	    formatted_value += numerator;
	    formatted_value += use_unicode ?
	      QChar( 0xE00a ) // The custom '/' character
	      : QChar( '/' );
	    // Format the denominator and shift to the 0xE010 sequence
	    QString denominator = QString::number( c );
        for ( int i = 0; i < denominator.length(); i++ )
          denominator[i] = QChar( denominator.at(i).unicode() +
					 den_unicode_offset );
	    formatted_value += denominator;
	  }
	}
	// Finally, tack on the abbreviation
	return lC::STR::VALUE_UNIT.arg( formatted_value ).arg( abbreviation() );
      }
    case DECIMAL:
      {
	return lC::STR::VALUE_UNIT.arg( value, 0, 'f', precision).arg(abbreviation());
      }
    }
    return QString::null;
  }

  double parse ( const QString& text, enum UnitFormat format, int precision ) const
  {
    double value = 0;
    switch ( format ) {
    case FRACTIONAL:
      {
	// Note: any unit indicator (e.g., " or in) is ignored for the inch.
	QString unit_regexp( "\\s*(?:" + INCH_QUOTE_ABBREV + "|" + INCH_IN_ABBREV +
			     ")?\\s*$" );
	// Note: accept a decimal input anyway.
	QRegExp decimal_regexp( "^\\s*[+-]?(?:\\d+(?:\\.(?:\\d+)?)?|\\.\\d+)" +
				unit_regexp );
	QRegExp fraction_regexp( "^\\s*([+-]?)(\\d+)/(\\d+)" + unit_regexp );
	QRegExp mixed_fraction_regexp( "^\\s*([+-]?)(\\d+)(?:[ -](\\d+)/(\\d+))?" +
				       unit_regexp );

	// First, check to see if it is a regular decimal number
    int pos = decimal_regexp.indexIn( text );
	if ( pos >= 0 ) {
	  value = text.toDouble(); // Just use the regular Qt conversion.
	  break;
	}
	// Next, check to see if it is a pure fraction.
    pos = fraction_regexp.indexIn( text );
	if ( pos >= 0 ) {
	  int numerator = fraction_regexp.cap( 2 ).toInt();
	  int denominator = fraction_regexp.cap( 3 ).toInt();
	  if ( denominator != 0 )
	    value = (double)numerator / denominator;
	  if ( fraction_regexp.cap( 1 ) == "-" )
	    value = -value;
	  break;
	}
	// If not a pure fraction, maybe it's a mixed fraction.
    pos = mixed_fraction_regexp.indexIn( text );
	if ( pos >= 0 ) {
	  int whole = mixed_fraction_regexp.cap( 2 ).toInt();
	  int numerator = mixed_fraction_regexp.cap( 3 ).toInt();
	  int denominator = mixed_fraction_regexp.cap( 4 ).toInt();
	  value = whole;
	  if ( denominator != 0 )
	    value += (double)numerator / denominator;
	  if ( mixed_fraction_regexp.cap( 1 ) == "-" )
	    value = -value;
	}
      }
      break;
    case DECIMAL:
      value = text.toDouble();
      break;
    }
    return round( format, precision, value );
  }
};

/*!
 * The inch unit of length, specifically the one using "in" as its
 * abbreviation.
 */
class InchIn : public Inch
{
public:
  InchIn ( void ) : Inch( qApp->translate( "Precision", INCH_IN_ABBREV.toLatin1() ),
              &english_fractional_in, &english_decimal_in )
  {}
};

/*!
 * The inch unit of length, specifically the one using " as its
 * abbreviation.
 */
class InchQuote : public Inch
{
public:
  InchQuote ( void ) : Inch( qApp->translate( "Precision", INCH_QUOTE_ABBREV.toLatin1() ),
			     &english_fractional_quote, &english_decimal_quote )
  {}
};

/*!
 * The foot unit of length. Note: the program stores everything
 * in units of inches, so this is strictly a formating step.
 */
class Foot : public LengthUnit
{
public:
  Foot ( const QString& abbreviation,
	 const PrecisionTable* fractional_precision_table,
	 const PrecisionTable* decimal_precision_table )
    : LengthUnit( ENGLISH, qApp->translate( "Precision", "Feet" ),
		  abbreviation, FRACTIONAL, INCHES_PER_FOOT, true,
		  fractional_precision_table, decimal_precision_table )
  {}

  //! Used to format the inch part of the value.
  virtual Inch* inch ( void ) const = 0;

  QString format ( enum UnitFormat format, int precision, double value,
		   bool use_unicode ) const
  {
    switch ( format ) {
    case FRACTIONAL:
      {
	// The limit to fractional values to 1/256-th is fixed.
	QString formatted_value;
	if ( fabs( value ) < 1./256. )
	  formatted_value = "0";
	else {
	  // Convert to feet
	  value /= INCHES_PER_FOOT;
	  // Extract the integral part.
	  int a = (int)value;

	  if ( a != 0 )
	    formatted_value = lC::STR::VALUE_UNIT.arg( a ).arg( abbreviation() );

	  // Extract the inches and fractional part. NOTE: STILL LIMITED
	  // TO 1/256-th inch PRECISION.
	  int b = (int)rint( INCHES_PER_FOOT * 256 * fabs( value - a ) );

	  // If b is exactly 12*256, then the original number was essentially
	  // an integer (to within 1/(12*256)-th of a foot)
	  if ( b == (int)( INCHES_PER_FOOT * 256 ) )
	    formatted_value =
	      lC::STR::VALUE_UNIT.arg( rint( value ) ).arg( abbreviation() );

	  else if ( b != 0 ) {
	    value = INCHES_PER_FOOT * fabs( value - a );
	    // Avoid leading blank if value is less than a foot
	    if ( !formatted_value.isEmpty() )
	      formatted_value += QChar( 0xa0 );
	    formatted_value += inch()->format( format, precision,value,use_unicode);
	  }
	}
	return formatted_value;
      }
    case DECIMAL:
      {
	// In this case, for example, 1" is 1/12' = 0.0833..', so we need
	// so extra significance in the representation.
	int significant_digits = precision + 2;
      return lC::STR::VALUE_UNIT.
	arg( value / INCHES_PER_FOOT, 0, 'f', significant_digits ).
	arg( abbreviation() );
      }
    }
    return QString::null;
  }

  double parse ( const QString& text, enum UnitFormat format, int precision ) const
  {
    double value = 0;
    switch ( format ) {
    case FRACTIONAL:
      {
	// Note: To indicate a length in feet, you have to use a unit
	// indicator, otherwise it is assumed to be inches. For example:
	// 1' 2, 2ft 1 1/2
	// note: accept decimal representation, too.
	QRegExp decimal_regexp( "^\\s*[+-]?(?:\\d+(?:\\.(?:\\d+)?)?|\\.\\d+)$" );
	QRegExp foot_regexp( "^\\s*([+-]?)(\\d+)(?:'|ft?)" );

	// First,check to see if is in decimal notation
    int pos = decimal_regexp.indexIn( text );
	if ( pos >= 0 ) {
	  value = INCHES_PER_FOOT * text.toDouble();
	  break;
	}
	// Next, check to see if it has a specified foot length
    pos = foot_regexp.indexIn( text );
	if ( pos >= 0 ) {
	  value = INCHES_PER_FOOT * foot_regexp.cap( 2 ).toDouble();
	  // Parse the rest of the string as an integer by chopping
	  // off the matched part and passing the rest to the inch parser.
	  QString inch_text = text.mid( foot_regexp.cap(0).length() );
	  value += inch()->parse( inch_text, format, precision );

	  if ( foot_regexp.cap( 1 ) == "-" )
	    value = -value;
	  break;
	}
	// Otherwise, treat as an inch value
	else
	  value = inch()->parse( text, format, precision );
      }
      break;
    case DECIMAL:
      value = INCHES_PER_FOOT * text.toDouble();
      break;
    }
    return round( format, precision, value );
  }
};

/*!
 * The foot unit of length, specifically the one using "ft" as its
 * abbreviation.
 */
class FootFt : public Foot
{
  //! Use for formatting the inch part in fractional mode.
  InchIn* inch_;
public:
  FootFt ( void ) : Foot( qApp->translate( "Precision", FOOT_FT_ABBREV.toLatin1() ),
			  &english_fractional_in, &english_decimal_in )
  {
    inch_ = new InchIn;
  }

  ~FootFt ( void )
  {
    delete inch_;
  }

  Inch* inch ( void ) const { return inch_; }
};

/*!
 * The foot unit of length, specifically the one using ' as its
 * abbreviation.
 */
class FootQuote : public Foot
{
  //! Use for formatting the inch part in fractional mode.
  InchQuote* inch_;
public:
  FootQuote ( void ) : Foot( qApp->translate( "Precision", FOOT_QUOTE_ABBREV.toLatin1() ),
			     &english_fractional_quote, &english_decimal_quote )
  {
    inch_ = new InchQuote;
  }
  ~FootQuote ( void )
  {
    delete inch_;
  }

  Inch* inch ( void ) const { return inch_; }
};

/*!
 * The yard unit of length.
 */
class Yard : public LengthUnit
{
  FootFt* foot_;
public:
  Yard ( void ) : LengthUnit( ENGLISH, qApp->translate( "Precision", "Yards" ),
			      qApp->translate( "Precision", "yd" ),
			      FRACTIONAL, INCHES_PER_YARD, true,
			      &english_fractional_in, &english_decimal_in )
  {
    foot_ = new FootFt;
  }

  ~Yard ( void )
  {
    delete foot_;
  }

  QString format ( enum UnitFormat format, int precision, double value,
		   bool use_unicode ) const
  {
    switch ( format ) {
    case FRACTIONAL:
      {
	// The limit to fractional values to 1/256-th is fixed.
	QString formatted_value;
	if ( fabs( value ) < 1./256. )
	  formatted_value = "0";
	else {
	  // Convert to yards
	  value /= INCHES_PER_YARD;
	  // Extract the integral part.
	  int a = (int)value;

	  if ( a != 0 )
	    formatted_value = lC::STR::VALUE_UNIT.arg( a ).arg( abbreviation() );

	  // Extract the feet, inches and fractional part. NOTE: STILL LIMITED
	  // TO 1/256-th inch PRECISION.
	  int b = (int)rint( INCHES_PER_YARD * 256 * fabs(value - a));

	  // If b is exactly 3*12*256, then the original number was essentially
	  // an integer (to within 1/(3*12*256)-th of a yard)
	  if ( b == (int)( INCHES_PER_YARD * 256 ) )
	    formatted_value =
	      lC::STR::VALUE_UNIT.arg( rint( value ) ).arg( abbreviation() );

	  else if ( b != 0 ) {
	    value = INCHES_PER_YARD * fabs( value - a );
	    // Avoid the leading blank if less than a yard
	    if ( !formatted_value.isEmpty() )
	      formatted_value += QChar( 0xa0 );
	    formatted_value += foot_->format( format, precision, value,use_unicode);
	  }
	}
	return formatted_value;
      }
    case DECIMAL:
      // Since, for example, 1" = 1/36yd = 0.0277...yd, we add some extra
      // signficance to the precision.
      int significant_digits = precision + 3;
      return lC::STR::VALUE_UNIT.
	arg( value / ( INCHES_PER_YARD), 0, 'f', significant_digits ).
	arg( abbreviation() );
    }
    return QString::null;
  }

  double parse ( const QString& text, enum UnitFormat format, int precision ) const
  {
    double value = 0;
    switch ( format ) {
    case FRACTIONAL:
      {
	// Note: To indicate a length in yards, you have to use a unit
	// indicator, otherwise it is assumed to be inches. For example:
	// 1y 2' 2, 2yd 2ft 1 1/2
	// note: accept decimal representation, too.
	QRegExp decimal_regexp( "^\\s*[+-]?(?:\\d+(?:\\.(?:\\d+)?)?|\\.\\d+)$" );
	QRegExp yard_regexp( "^\\s*([+-]?)(\\d+)yd?" );

	// First, check to see if it is just decimal.
    int pos = decimal_regexp.indexIn( text );
	if ( pos >= 0 ) {
	  value = INCHES_PER_YARD * text.toDouble();
	  break;
	}
	// Next, check to see if it has a specified yard length
    pos = yard_regexp.indexIn( text );
	if ( pos >= 0 ) {
	  value = INCHES_PER_YARD * yard_regexp.cap( 2 ).toDouble();
	  // Parse the rest of the string as a foot length by chopping
	  // off the matched part and passing the rest to the foot parser.
	  QString foot_text = text.mid( yard_regexp.cap(0).length() );
	  value += foot_->parse( foot_text, format, precision );

	  if ( yard_regexp.cap( 1 ) == "-" )
	    value = -value;
	  break;
	}
	// Otherwise, treat as a foot value
	else
	  value = foot_->parse( text, format, precision );
      }
      break;
    case DECIMAL:
      value = INCHES_PER_YARD * text.toDouble();
      break;
    }
    return round( format, precision, value );
  }
};

/*!
 * The millimeter unit of length.
 */
class Millimeter : public LengthUnit
{
public:
  Millimeter ( void ) : LengthUnit( METRIC,
				    qApp->translate( "Precision", "Millimeters" ),
                    qApp->translate( "Precision", MILLIMETER_ABBREV.toLatin1() ),
				    DECIMAL, MM_PER_INCH, false,
				    0, &metric_decimal )
  {}

  QString format ( enum UnitFormat format, int precision, double value,
		   bool /*use_unicode*/ ) const
  {
    switch ( format ) {
    case FRACTIONAL:
      break;
    case DECIMAL:
      // Since mm are so small, the number of useful significant digits
      // to the right of the decimal point is much fewer than for the other
      // units.
      int significant_digits = precision < 3 ? 0 : 1;
      return lC::STR::VALUE_UNIT.
	arg( MM_PER_INCH * value, 0, 'f', significant_digits ).
	arg( abbreviation() );
    }
    return QString::null;
  }

  double parse ( const QString& text, enum UnitFormat format, int precision ) const
  {
    double value = 0;
    switch ( format ) {
    case FRACTIONAL:
      break;
    case DECIMAL:
      value = text.toDouble() / MM_PER_INCH;
      break;
    }
    return round( format, precision, value );
  }
};

/*!
 * The centimeter unit of length.
 */
class Centimeter : public LengthUnit
{
public:
  Centimeter ( void ) : LengthUnit( METRIC,
				    qApp->translate( "Precision", "Centimeters" ),
                    qApp->translate( "Precision", CENTIMETER_ABBREV.toLatin1() ),
				    DECIMAL, CM_PER_INCH, false,
				    0, &metric_decimal )
  {}

  QString format ( enum UnitFormat format, int precision, double value,
		   bool /*use_unicode*/ ) const
  {
    switch ( format ) {
    case FRACTIONAL:
      break;
    case DECIMAL:
      // cm...inches...they're about the same, but both the 10cm and 1cm
      // precision steps don't require any signicant digits to the right
      // of the decimal point
      int significant_digits = precision < 2 ? 0 : precision - 1;
      return lC::STR::VALUE_UNIT.
	arg( CM_PER_INCH * value, 0, 'f', significant_digits ).
	arg( abbreviation() );
    }
    return QString::null;
  }

  double parse ( const QString& text, enum UnitFormat format, int precision ) const
  {
    double value = 0;
    switch ( format ) {
    case FRACTIONAL:
      break;
    case DECIMAL:
      value = text.toDouble() / CM_PER_INCH;
      break;
    }
    return round( format, precision, value );
  }
};

/*!
 * The meter unit of length.
 */
class Meter : public LengthUnit
{
public:
  Meter ( void ) : LengthUnit( METRIC,
			       qApp->translate( "Precision", "Meters" ),
                   qApp->translate( "Precision", METER_ABBREV.toLatin1() ),
			       DECIMAL, M_PER_INCH, false,
			       0, &metric_decimal )
  {}

  QString format ( enum UnitFormat format, int precision, double value,
		   bool /*use_unicode*/ ) const
  {
    switch ( format ) {
    case FRACTIONAL:
      break;
    case DECIMAL:
      // Of course, meters are largest unit we have here, so they need
      // the most significant digits.
      int significant_digits = precision + 1;
      return lC::STR::VALUE_UNIT.
	arg( M_PER_INCH * value, 0, 'f', significant_digits ).arg(abbreviation());
    }
    return QString::null;
  }

  double parse ( const QString& text, enum UnitFormat format, int precision ) const
  {
    double value = 0;
    switch ( format ) {
    case FRACTIONAL:
      break;
    case DECIMAL:
      value = text.toDouble() / M_PER_INCH;
      break;
    }
    return round( format, precision, value );
  }
};


UnitsBasis* UnitsBasis::units_basis_;

UnitsBasis* UnitsBasis::instance ( void )      
{
  if ( units_basis_ == 0 )
    units_basis_ = new UnitsBasis();

  return units_basis_;
}

UnitsBasis::UnitsBasis ( void )
  : QObject( 0 ), currentLenghtUnit( 0 )
{
  setObjectName( "unitsBasis" );

  length_units_.append( std::shared_ptr<LengthUnit>(new InchIn) );
  length_units_.append( std::shared_ptr<LengthUnit>(new InchQuote) );
  length_units_.append( std::shared_ptr<LengthUnit>(new FootFt) );
  length_units_.append( std::shared_ptr<LengthUnit>(new FootQuote) );
  length_units_.append( std::shared_ptr<LengthUnit>(new Yard) );
  length_units_.append( std::shared_ptr<LengthUnit>(new Millimeter) );
  length_units_.append( std::shared_ptr<LengthUnit>(new Centimeter) );
  length_units_.append( std::shared_ptr<LengthUnit>(new Meter) );

  QListIterator< std::shared_ptr<LengthUnit> > i( length_units_ );
  while ( i.hasNext() )
    length_unit_strings_.append( qApp->translate( "Precision", "%1 [ %2 ]" ).
                 arg( i.peekNext()->name() ).
                 arg( i.next()->abbreviation() ) );

  // The default default is Centimeter
  currentLenghtUnit = 6 ;
  format_ = length_units_[currentLenghtUnit]->defaultFormat();
  setPrecision( length_units_[currentLenghtUnit]->defaultPrecision( format_ ) );
}

UnitsBasis::~UnitsBasis ( void )
{}

LengthUnit* UnitsBasis::lengthUnit ( int index ) const
{
  return length_units_[index].get();
}

LengthUnit* UnitsBasis::lengthUnit ( ) const
{
  return length_units_[currentLenghtUnit].get();
}

void UnitsBasis::setLengthUnit ( int index )
{
  currentLenghtUnit = index;

  format_ = length_units_[index]->defaultFormat();
  precision_index_ = length_units_[index]->defaultPrecision( format_ );

  emit unitsChanged();
}

// Set the current unit based on the string read from the default settings.

void UnitsBasis::setLengthUnit ( const QString& lText )
{
  //TODO check what the tokenizer is
  QStringList list = lText.split(' ');
  QStringList::const_iterator l = list.begin();

  if ( l == list.end() )
    return;

  QRegExp name_rx( "(.*)\\[(.*)\\]" );

  int position = name_rx.indexIn( lText );

  if ( position < 0 )
    return;

  // Don't disturb the current default in case this fails.
  QList< std::shared_ptr<LengthUnit> >::const_iterator lu;
  int index = 0;

  for ( lu = length_units_ .begin() ; lu != length_units_.end(); ++lu, ++index )
    if ( name_rx.cap(1) == lu->get()->name() &&
     name_rx.cap(2) == lu->get()->abbreviation() )
      break;

  if ( lu == length_units_.end() )
    return;

  // So, at least use the default attributes of this unit
  setLengthUnit( index );

  ++l;
  if ( l == list.end() ) {
    emit unitsChanged();
    return;
  }

  format_ = unitFormat( *l );

  ++l;
  if ( l == list.end() ) {
    emit unitsChanged();
    return;
  }

  precision_index_ = (*l).toInt();

  emit unitsChanged();
}

void UnitsBasis::setFormat ( enum UnitFormat format )
{
  format_ = format;

  emit unitsChanged();
}

void UnitsBasis::setPrecision ( int precision_index )
{
  precision_index_ = precision_index;

  emit unitsChanged();
}

double UnitsBasis::round ( double x ) const
{
  lCPrecision* precision = length_units_[currentLenghtUnit]->precision( format_,
                                 precision_index_ );
  double in = precision->unitsPerIn();
  double rounded = rint( in * x ) / in;

  return rounded;
}

void UnitsBasis::round ( Space2D::Point& point ) const
{
  point[Space2D::X] = round( point[Space2D::X] );
  point[Space2D::Y] = round( point[Space2D::Y] );
}

void UnitsBasis::round ( Space2D::Vector& vector ) const
{
  vector[Space2D::X] = round( vector[Space2D::X] );
  vector[Space2D::Y] = round( vector[Space2D::Y] );
}

void UnitsBasis::round ( Space3D::Point& point ) const
{
  point[Space3D::X] = round( point[Space3D::X] );
  point[Space3D::Y] = round( point[Space3D::Y] );
  point[Space3D::Z] = round( point[Space3D::Z] );
}

QString UnitsBasis::format( double value, bool use_unicode ) const
{
  return length_units_[currentLenghtUnit]->format( format_, precision_index_, value,
					  use_unicode );
}
