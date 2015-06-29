/* -*- c++ -*-
 * ratio.h
 *
 * Header for the Ratio class.
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

#ifndef RATIO_H
#define RATIO_H
#include <cmath>
/*!
 * This is a very simple class which represents small, positive,
 * rational numbers from 65535 to 1 / 65535. Its algrebra is
 * non-standard: 1/1 + 1 = 2/1, but 1/1 - 1 = 1/2 and 1/2 - 1 = 1/3.
 * It is intended for use mostly as a scale factor in viewing.
 *
 * Update: Here is space of values covered by incrementing:
 *  ... 3/1 5/2 2/1 3/2 1/1 2/3 1/2 2/5 1/3 ...
 * or, as it is stored internally:
 *  ... 6/2 5/2 4/2 3/2 2/2 2/3 2/4 2/5 2/6 ...
 */
class Ratio {
  ushort numerator_;
  ushort denominator_;
public:
  Ratio ( ushort n = 2, ushort d = 2 )
    : numerator_( n ), denominator_( d )
  {}

  Ratio ( int serial )
  {
    if ( serial >= 0 ) {
      numerator_ = serial + 2;
      denominator_ = 2;
    }
    else {
      numerator_ = 2;
      denominator_ = 2 - serial;
    }
  }

  ushort numerator ( void ) const {
    if ( numerator_ >= denominator_ ) {
      if ( numerator_ % denominator_ != 0 )
	return numerator_;
      else
	return numerator_ / denominator_;
    }
    else {
      if ( denominator_ % numerator_ != 0 )
	return numerator_;
      else
	return 1;
    }
  }

  ushort denominator ( void ) const {
    if ( numerator_ >= denominator_ ) {
      if ( numerator_ % denominator_ != 0 )
	return denominator_;
      else
	return 1;
    }
    else {
      if ( denominator_ % numerator_ != 0 )
	return denominator_;
      else
	return denominator_ / numerator_;
    }
  }

  operator double ( void ) const { return (double)numerator_/denominator_; }

  Ratio& operator++ ( void ) {
    if ( denominator_ == 2 ) numerator_++;
    else denominator_--;
    return *this;
  }

  Ratio& operator-- ( void ) {
    if ( numerator_ == 2 ) denominator_++;
    else numerator_--;
    return *this;
  }

  bool operator== ( const Ratio& b ) const
  {
    return numerator_ == b.numerator_ && denominator_ == b.denominator_;
  }

  Ratio& operator= ( double value )
  {
    if ( value < 1 ) {
      numerator_ = 2;
      denominator_ = (ushort)rint( 2. / value );
    }
    else {
      denominator_ = 2;
      numerator_ = (ushort)rint( 2. * value );
    }
    return *this;
  }

  int serial ( void ) const
  {
    return numerator_ - denominator_;
  }
};
#endif // RATIO_H
