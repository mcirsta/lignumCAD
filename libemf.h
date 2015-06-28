/* -*- c++ -*-
 * EMF: A library for generating ECMA-234 Enhanced Metafiles
 * Copyright (C) 2002 lignum Computing, Inc. <libemf@lignumcomputing.com>
 * $Id: libemf.h,v 1.7 2002/03/28 17:14:51 allen Exp $
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */
#ifndef _LIBEMF_H
#define _LIBEMF_H 1

#include <cmath>
#include <vector>
#include <map>
#include <functional>
#include <algorithm>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <emf.h>

#include <wine/w16.h>

namespace EMF {
  /*!
   * The maximum number of pixels in the X direction. Effectively
   * the horizontal resolution of the metafile.
   */
#if 0
  const int XMAX_PIXELS = 1024; /*(INT_MAX)*/
#else
  const int XMAX_PIXELS = 1280; /*(INT_MAX)*/
#endif
  /*!
   * The maximum number of pixels in the Y direction. Effectively
   * the vertical resolution of the metafile.
   */
#if 0
  const int YMAX_PIXELS = 768; /*(INT_MAX)*/
#else
  const int YMAX_PIXELS = 1024; /*(INT_MAX)*/
#endif
  /*!
   * The number of millimeters to which the XMAX_PIXELS corresponds.
   * The default horizontal size of the metafile (can be changed by
   * the call to CreateEnhMetaFile).
   */
  const int XMAX_MM = 320;
  /*!
   * The number of millimeters to which the YMAX_PIXELS corresponds.
   * The default vertical size of the metafile (can be changed by
   * the call to CreateEnhMetaFile).
   */
  const int YMAX_MM = 240;
  /*!
   * The default resolution for the viewport.
   */
  const int RESOLUTION = 96;
  /*!
   * Rounds up a byte count to a multiple of four bytes.
   */
  static inline int ROUND_TO_LONG ( int n ) { return ((n+3)/4)*4; }

  //! Represent a wide (UNICODE) character string in a simple way.
  /*!
   * Even (widechar) strings have to be byte swapped. This structure
   * allows us to provide a uniform stream-like interface for writing out
   * all the components of metafiles.
   */
  struct WCHARSTR {
    WCHAR *const string_;	//!< String of WCHARs.
    const int length_;		//!< Number of WCHARs in string.
    /*!
     * Simple constructor.
     * \param string pointer to string of WCHARS.
     * \param n number of WCHARS in string.
     */
    WCHARSTR ( WCHAR *const string, const int length )
      : string_( string ), length_( length ) {}
  };

  //! Represent an ASCII character string in a simple way.
  /*!
   * ASCII strings don't have to be byte swapped, but this structure
   * allows us to provide a uniform stream-like interface for writing out
   * all the components of metafiles.
   */
  struct CHARSTR {
    CHAR *const string_;	//!< Array of single byte characters.
    const int length_;		//!< Number of single byte characers in array.
    /*!
     * Simple constructor.
     * \param array pointer to array of single byte characters.
     * \param n number of bytes in array.
     */
    CHARSTR ( CHAR *const string, const int length )
      : string_( string ), length_( length ) {}
  };

  //! Represent a byte array in a simple way.
  /*!
   * Evidently, an unsigned array of bytes with no particular encoding
   * implied.
   */
  struct BYTEARRAY {
    BYTE *const array_;		//!< Array of unsigned bytes.
    const int n_;		//!< Number of bytes in array.
    /*!
     * Simple constructor.
     * \param array pointer to array of bytes
     * \param n number of bytes in array
     */
    BYTEARRAY ( BYTE *const array, const int n )
      : array_( array ), n_( n ) {}
  };

  //! Represent an array of points in a simple way.
  /*!
   * Allow an array of POINTL's to be written out at once.
   */
  struct POINTLARRAY {
    POINTL *const points_;	//!< Array of POINTLs.
    const DWORD n_;		//!< Number of POINTLs in array.
    /*!
     * Simple constructor.
     * \param array pointer to array of POINTLs.
     * \param n number POINTLs in array.
     */
    POINTLARRAY ( POINTL *const points, const DWORD n )
      : points_( points ), n_( n ) {}
  };

  //! Represent an array of 16-bit point in a simple way.
  /*!
   * Allow an array of POINT16's to be written out at once.
   */
  struct POINT16ARRAY {
    POINT16 *const points_;	//!< Array of POINT16s.
    const DWORD n_;		//!< Number of POINT16s in array.
    /*!
     * Simple constructor.
     * \param array pointer to array of POINT16s.
     * \param n number POINT16s in array.
     */
    POINT16ARRAY ( POINT16 *const points, const DWORD n )
      : points_( points ), n_( n ) {}
  };

  //! Represent an array of integers in a simple way.
  /*!
   * Allow an array of INT's to be written out at once.
   */
  struct INTARRAY {
    INT *const ints_;		//!< Array of ints.
    const DWORD n_;		//!< Number of ints in array.
    /*!
     * simple constructor.
     * \param array pointer to ints.
     * \param n number ints in array.
     */
    INTARRAY ( INT *const ints, const DWORD n )
      : ints_( ints ), n_( n ) {}
  };

  //! Represent an array of double word integers in a simple way.
  /*!
   * Allow an array of DWORD's to be written out at once.
   */
  struct DWORDARRAY {
    DWORD *const dwords_;	//!< Array of double words.
    const DWORD n_;		//!< Number of double words in array.
    /*!
     * simple constructor.
     * \param array pointer to double words.
     * \param n number double words in array.
     */
    DWORDARRAY ( DWORD *const dwords, const DWORD n )
      : dwords_( dwords ), n_( n ) {}
  };

  //! All metafile records must be padded out to a multiple of 4 bytes.
  /*!
   * Write out a few bytes of padding if necessary.
   */
  struct PADDING {
    static const char padding_[4]; //!< Pad with '\0's.
    const int size_;		//!< Number of bytes of padding.
    /*!
     * simple constructor.
     * \param size number of bytes of padding to use.
     */
    PADDING ( const int size ) : size_( size ) {}
  };
  
  //! Support different endian modes when reading and writing the metafile
  /*!
   * To support different endian modes, rather than just writing the
   * structures directly to a file via fwrite( &emr, ...), we have to
   * write each element of the structure separately, swapping bytes
   * as necessary. datastream supports this. Remarkably similar to
   * the QDataStream class from Qt. So, too, for reading.
   */
  class DATASTREAM {
    bool swap_;
    ::FILE* fp_;

    static bool bigEndian ( void );
  public:
    /*!
     * Constructor for DATASTREAM.
     * \param fp optional file pointer (but must be assigned before
     * any output occurs.)
     */
    DATASTREAM ( ::FILE* fp = 0 ) : swap_( bigEndian() ), fp_( fp ) {}
    /*!
     * Use the given FILE stream as the input/output destination.
     * \param fp file point for i/o.
     */
    void setStream ( ::FILE* fp ) { fp_ = fp; }
    /*!
     * Output a byte to the stream (not swabbed or anything).
     * \param byte byte to output.
     */
    DATASTREAM& operator<< ( const BYTE& byte )
    {
      fwrite( &byte, sizeof(BYTE), 1, fp_ );
      return *this;
    }
    /*!
     * Input a byte from the stream (not swabbed or anything).
     * \param byte destination for input byte.
     */
    DATASTREAM& operator>> ( BYTE& byte )
    {
      fread( &byte, sizeof(BYTE), 1, fp_ );
      return *this;
    }
    /*!
     * Output a (short) word to the stream (swabbed).
     * \param word (short) word to output.
     */
    DATASTREAM& operator<< ( const WORD& word )
    {
      if ( swap_ ) {
	unsigned char const * p = (unsigned char const*)&word;
	fwrite( &p[1], sizeof(unsigned char), 1, fp_ );
	fwrite( &p[0], sizeof(unsigned char), 1, fp_ );
      }
      else
	fwrite( &word, sizeof(WORD), 1, fp_ );
      return *this;
    }
    /*!
     * Input a (short) word from the stream (swabbed).
     * \param word destination for (short) word.
     */
    DATASTREAM& operator>> ( WORD& word )
    {
      if ( swap_ ) {
	unsigned char* p = (unsigned char*)&word;
	fread( &p[1], sizeof(unsigned char), 1, fp_ );
	fread( &p[0], sizeof(unsigned char), 1, fp_ );
      }
      else
	fread( &word, sizeof(WORD), 1, fp_ );
      return *this;
    }
    /*!
     * Output a (short, 16-bit) word to the stream (swabbed).
     * \param word (short, 16-bit) word to output.
     */
    DATASTREAM& operator<< ( const INT16& word )
    {
      if ( swap_ ) {
	unsigned char const * p = (unsigned char const*)&word;
	fwrite( &p[1], sizeof(unsigned char), 1, fp_ );
	fwrite( &p[0], sizeof(unsigned char), 1, fp_ );
      }
      else
	fwrite( &word, sizeof(INT16), 1, fp_ );
      return *this;
    }
    /*!
     * Input a (short, 16-bit) word from the stream (swabbed).
     * \param word destination for (short, 16-bit) word.
     */
    DATASTREAM& operator>> ( INT16& word )
    {
      if ( swap_ ) {
	unsigned char* p = (unsigned char*)&word;
	fread( &p[1], sizeof(unsigned char), 1, fp_ );
	fread( &p[0], sizeof(unsigned char), 1, fp_ );
      }
      else
	fread( &word, sizeof(INT16), 1, fp_ );
      return *this;
    }
    /*!
     * Output a double word (long) to the stream (swabbed).
     * \param word word (long) to output.
     */
    DATASTREAM& operator<< ( const DWORD& dword )
    {
      if ( swap_ ) {
	unsigned char const* p = (unsigned char const*)&dword;
	fwrite( &p[3], sizeof(unsigned char), 1, fp_ );
	fwrite( &p[2], sizeof(unsigned char), 1, fp_ );
	fwrite( &p[1], sizeof(unsigned char), 1, fp_ );
	fwrite( &p[0], sizeof(unsigned char), 1, fp_ );
      }
      else
	fwrite( &dword, sizeof(DWORD), 1, fp_ );
      return *this;
    }
    /*!
     * Input a double word (long) from the stream (swabbed).
     * \param word destination for double word (long).
     */
    DATASTREAM& operator>> ( DWORD& dword )
    {
      if ( swap_ ) {
	unsigned char* p = (unsigned char*)&dword;
	fread( &p[3], sizeof(unsigned char), 1, fp_ );
	fread( &p[2], sizeof(unsigned char), 1, fp_ );
	fread( &p[1], sizeof(unsigned char), 1, fp_ );
	fread( &p[0], sizeof(unsigned char), 1, fp_ );
      }
      else
	fread( &dword, sizeof(DWORD), 1, fp_ );
      return *this;
    }
    /*!
     * Output a long int to the stream (swabbed).
     * \param long long int to output.
     */
    DATASTREAM& operator<< ( const LONG& long_ )
    {
      if ( swap_ ) {
	unsigned char const* p = (unsigned char const*)&long_;
	fwrite( &p[3], sizeof(unsigned char), 1, fp_ );
	fwrite( &p[2], sizeof(unsigned char), 1, fp_ );
	fwrite( &p[1], sizeof(unsigned char), 1, fp_ );
	fwrite( &p[0], sizeof(unsigned char), 1, fp_ );
      }
      else
	fwrite( &long_, sizeof(LONG), 1, fp_ );
      return *this;
    }
    /*!
     * Input a long int from the stream (swabbed).
     * \param word destination for long int.
     */
    DATASTREAM& operator>> ( LONG& long_ )
    {
      if ( swap_ ) {
	unsigned char* p = (unsigned char*)&long_;
	fread( &p[3], sizeof(unsigned char), 1, fp_ );
	fread( &p[2], sizeof(unsigned char), 1, fp_ );
	fread( &p[1], sizeof(unsigned char), 1, fp_ );
	fread( &p[0], sizeof(unsigned char), 1, fp_ );
      }
      else
	fread( &long_, sizeof(LONG), 1, fp_ );
      return *this;
    }
    /*!
     * Output a (long) int to the stream (swabbed).
     * \param int_ (long) int to output.
     */
    DATASTREAM& operator<< ( const INT& int_ )
    {
      if ( swap_ ) {
	unsigned char const* p = (unsigned char const*)&int_;
	fwrite( &p[3], sizeof(unsigned char), 1, fp_ );
	fwrite( &p[2], sizeof(unsigned char), 1, fp_ );
	fwrite( &p[1], sizeof(unsigned char), 1, fp_ );
	fwrite( &p[0], sizeof(unsigned char), 1, fp_ );
      }
      else
	fwrite( &int_, sizeof(INT), 1, fp_ );
      return *this;
    }
    /*!
     * Input a (long) int from the stream (swabbed).
     * \param int_ destination for (long) int.
     */
    DATASTREAM& operator>> ( INT& int_ )
    {
      if ( swap_ ) {
	unsigned char* p = (unsigned char*)&int_;
	fread( &p[3], sizeof(unsigned char), 1, fp_ );
	fread( &p[2], sizeof(unsigned char), 1, fp_ );
	fread( &p[1], sizeof(unsigned char), 1, fp_ );
	fread( &p[0], sizeof(unsigned char), 1, fp_ );
      }
      else
	fread( &int_, sizeof(INT), 1, fp_ );
      return *this;
    }
    /*!
     * Output a (long) unsigned int to the stream (swabbed).
     * \param uint (long) unsigned int to output.
     */
    DATASTREAM& operator<< ( const UINT& uint )
    {
      if ( swap_ ) {
	unsigned char const* p = (unsigned char const*)&uint;
	fwrite( &p[3], sizeof(unsigned char), 1, fp_ );
	fwrite( &p[2], sizeof(unsigned char), 1, fp_ );
	fwrite( &p[1], sizeof(unsigned char), 1, fp_ );
	fwrite( &p[0], sizeof(unsigned char), 1, fp_ );
      }
      else
	fwrite( &uint, sizeof(UINT), 1, fp_ );
      return *this;
    }
    /*!
     * Input a (long) unsigned int from the stream (swabbed).
     * \param uint destination for (long) unsigned int.
     */
    DATASTREAM& operator>> ( UINT& uint )
    {
      if ( swap_ ) {
	unsigned char* p = (unsigned char*)&uint;
	fread( &p[3], sizeof(unsigned char), 1, fp_ );
	fread( &p[2], sizeof(unsigned char), 1, fp_ );
	fread( &p[1], sizeof(unsigned char), 1, fp_ );
	fread( &p[0], sizeof(unsigned char), 1, fp_ );
      }
      else
	fread( &uint, sizeof(UINT), 1, fp_ );
      return *this;
    }
    /*!
     * Output a single precision float to the stream (swabbed).
     * \param float_ single precision float to output.
     */
    DATASTREAM& operator<< ( const FLOAT& float_ )
    {
      if ( swap_ ) {
	unsigned char const* p = (unsigned char const*)&float_;
	fwrite( &p[3], sizeof(unsigned char), 1, fp_ );
	fwrite( &p[2], sizeof(unsigned char), 1, fp_ );
	fwrite( &p[1], sizeof(unsigned char), 1, fp_ );
	fwrite( &p[0], sizeof(unsigned char), 1, fp_ );
      }
      else
	fwrite( &float_, sizeof(FLOAT), 1, fp_ );
      return *this;
    }
    /*!
     * Input a single precision float from the stream (swabbed).
     * \param uint destination for single precision float.
     */
    DATASTREAM& operator>> ( FLOAT& float_ )
    {
      if ( swap_ ) {
	unsigned char* p = (unsigned char*)&float_;
	fread( &p[3], sizeof(unsigned char), 1, fp_ );
	fread( &p[2], sizeof(unsigned char), 1, fp_ );
	fread( &p[1], sizeof(unsigned char), 1, fp_ );
	fread( &p[0], sizeof(unsigned char), 1, fp_ );
      }
      else
	fread( &float_, sizeof(FLOAT), 1, fp_ );
      return *this;
    }
    /*!
     * Output a series of '\0's to pad out a record.
     * \param padding simple padding structure (length and number of nulls).
     */
    DATASTREAM& operator<< ( const PADDING& padding )
    {
      if ( padding.size_ != 0 )
	fwrite( &padding.padding_, sizeof(CHAR), padding.size_, fp_ );
      return *this;
    }
    /*!
     * Output a RECTL structure.
     * \param rectl structure to output.
     */
    DATASTREAM& operator<< ( const RECTL& rectl )
    {
      *this << rectl.left << rectl.top << rectl.right << rectl.bottom;
      return *this;
    }
    /*!
     * Input a RECTL structure.
     * \param rectl destination of input RECTL.
     */
    DATASTREAM& operator>> ( RECTL& rectl )
    {
      *this >> rectl.left >> rectl.top >> rectl.right >> rectl.bottom;
      return *this;
    }
    /*!
     * Output a SIZEL structure.
     * \param sizel structure to output.
     */
    DATASTREAM& operator<< ( const SIZEL& sizel )
    {
      *this << sizel.cx << sizel.cy;
      return *this;
    }
    /*!
     * Input a SIZEL structure.
     * \param sizel destination of input SIZEL.
     */
    DATASTREAM& operator>> ( SIZEL& sizel )
    {
      *this >> sizel.cx >> sizel.cy;
      return *this;
    }
    /*!
     * Output a WCHAR string (note: the individual characters are swabbed).
     * \param wcharstr structure to output.
     */
    DATASTREAM& operator<< ( const WCHARSTR& wcharstr )
    {
      for ( int i = 0; i < wcharstr.length_; i++ )
	*this << wcharstr.string_[i];
      return *this;
    }
    /*!
     * Input a WCHAR string (note: the individual characters are swabbed.)
     * \param wcharstr destination of input WCHAR string.
     */
    DATASTREAM& operator>> ( WCHARSTR& wcharstr )
    {
      for ( int i = 0; i < wcharstr.length_; i++ )
	*this >> wcharstr.string_[i];
      return *this;
    }
    /*!
     * Output a single byte character string.
     * \param charstr structure to output.
     */
    DATASTREAM& operator<< ( const CHARSTR& charstr )
    {
      fwrite( charstr.string_, sizeof(CHAR), charstr.length_, fp_ );
      return *this;
    }
    /*!
     * Input a single byte character string.
     * \param charstr destination of input CHAR string.
     */
    DATASTREAM& operator>> ( CHARSTR& charstr )
    {
      fread( charstr.string_, sizeof(CHAR), charstr.length_, fp_ );
      return *this;
    }
    /*!
     * Output an Enhanced Metafile Record header.
     * \param emr Enhanced Metafile Record header to output.
     */
    DATASTREAM& operator<< ( const ::EMR& emr )
    {
      *this << emr.iType << emr.nSize;
      return *this;
    }
    /*!
     * Input an Enhanced Metafile Record header.
     * \param emr destination of Enhanced Metafile Record header.
     */
    DATASTREAM& operator>> ( ::EMR& emr )
    {
      *this >> emr.iType >> emr.nSize;
      return *this;
    }
    /*!
     * Output a POINT structure.
     * \param point POINT to output.
     */
    DATASTREAM& operator<< ( const POINT& point )
    {
      *this << point.x << point.y;
      return *this;
    }
    /*!
     * Input a POINT structure.
     * \param point destination of input POINT.
     */
    DATASTREAM& operator>> ( POINT& point )
    {
      *this >> point.x >> point.y;
      return *this;
    }
    /*!
     * Output a POINTL structure.
     * \param pointl POINTL to output.
     */
    DATASTREAM& operator<< ( const POINTL& pointl )
    {
      *this << pointl.x << pointl.y;
      return *this;
    }
    /*!
     * Input a POINTL structure.
     * \param pointl destination of input POINTL.
     */
    DATASTREAM& operator>> ( POINTL& pointl )
    {
      *this >> pointl.x >> pointl.y;
      return *this;
    }
    /*!
     * Output a POINT16 structure.
     * \param point POINT16 to output.
     */
    DATASTREAM& operator<< ( const POINT16& point )
    {
      *this << point.x << point.y;
      return *this;
    }
    /*!
     * Input a POINT16 structure.
     * \param point destination of input POINT16.
     */
    DATASTREAM& operator>> ( POINT16& point )
    {
      *this >> point.x >> point.y;
      return *this;
    }
    /*!
     * Output an XFORM structure.
     * \param xform XFORM to output.
     */
    DATASTREAM& operator<< ( const XFORM& xform )
    {
      *this << xform.eM11 << xform.eM12 << xform.eM21 << xform.eM22
	    << xform.eDx << xform.eDy;
      return *this;
    }
    /*!
     * Input an XFORM structure.
     * \param xfrom destination of input XFORM.
     */
    DATASTREAM& operator>> ( XFORM& xform )
    {
      *this >> xform.eM11 >> xform.eM12 >> xform.eM21 >> xform.eM22
	    >> xform.eDx >> xform.eDy;
      return *this;
    }
    /*!
     * Output an array of BYTEs.
     * \param array array of BYTEs to output.
     */
    DATASTREAM& operator<< ( const BYTEARRAY& array )
    {
      fwrite( array.array_, sizeof(BYTE), array.n_, fp_ );
      return *this;
    }
    /*!
     * Input an array of BYTEs.
     * \param array destination of array of input BYTEs.
     */
    DATASTREAM& operator>> ( BYTEARRAY& array )
    {
      fread( array.array_, sizeof(BYTE), array.n_, fp_ );
      return *this;
    }
    /*!
     * Output an array of POINTLs.
     * \param array array of POINTLs to output.
     */
    DATASTREAM& operator<< ( const POINTLARRAY& array )
    {
      for ( unsigned int i = 0; i < array.n_; i++ )
	*this << array.points_[i];
      return *this;
    }
    /*!
     * Input an array of POINTLs.
     * \param array destination of array of input POINTLs.
     */
    DATASTREAM& operator>> ( POINTLARRAY& array )
    {
      for ( unsigned int i = 0; i < array.n_; i++ )
	*this >> array.points_[i];
      return *this;
    }
    /*!
     * Output an array of POINT16s.
     * \param array array of POINT16s to output.
     */
    DATASTREAM& operator<< ( const POINT16ARRAY& array )
    {
      for ( unsigned int i = 0; i < array.n_; i++ )
	*this << array.points_[i];
      return *this;
    }
    /*!
     * Input an array of POINT16s.
     * \param array destination of array of input POINT16s.
     */
    DATASTREAM& operator>> ( POINT16ARRAY& array )
    {
      for ( unsigned int i = 0; i < array.n_; i++ )
	*this >> array.points_[i];
      return *this;
    }
    /*!
     * Output an array of (long) ints.
     * \param array array of (long) ints to output.
     */
    DATASTREAM& operator<< ( const INTARRAY& array )
    {
      for ( unsigned int i = 0; i < array.n_; i++ )
	*this << array.ints_[i];
      return *this;
    }
    /*!
     * Input an array of (long) ints.
     * \param array destination of array of input (long) ints.
     */
    DATASTREAM& operator>> ( INTARRAY& array )
    {
      for ( unsigned int i = 0; i < array.n_; i++ )
	*this >> array.ints_[i];
      return *this;
    }
    /*!
     * Output an array of double words (longs).
     * \param array array of double words (longs) to output.
     */
    DATASTREAM& operator<< ( const DWORDARRAY& array )
    {
      for ( unsigned int i = 0; i < array.n_; i++ )
	*this << array.dwords_[i];
      return *this;
    }
    /*!
     * Input an array of double words (longs).
     * \param array destination of array of input double words (longs).
     */
    DATASTREAM& operator>> ( DWORDARRAY& array )
    {
      for ( unsigned int i = 0; i < array.n_; i++ )
	*this >> array.dwords_[i];
      return *this;
    }
    /*!
     * Output an Enhanced Metafile Text Record.
     * \param text Enhanced Metafile Text Record to output.
     */
    DATASTREAM& operator<< ( const ::EMRTEXT& text )
    {
      *this << text.ptlReference << text.nChars << text.offString << text.fOptions
	 << text.rcl << text.offDx;
      return *this;
    }
    /*!
     * Input an Enhanced Metafile Text Record.
     * \param text destination of Enhanced Metafile Text Record.
     */
    DATASTREAM& operator>> ( ::EMRTEXT& text )
    {
      *this >> text.ptlReference >> text.nChars >> text.offString >> text.fOptions
	    >> text.rcl >> text.offDx;
      return *this;
    }
    /*!
     * Output a Logical Pen definition.
     * \param pen Logical Pen definition to output.
     */
    DATASTREAM& operator<< ( const LOGPEN& pen )
    {
      *this << pen.lopnStyle << pen.lopnWidth << pen.lopnColor;
      return *this;
    }
    /*!
     * Input a Logical Pen definition.
     * \param pen destination of Logical Pen definition.
     */
    DATASTREAM& operator>> ( LOGPEN& pen )
    {
      *this >> pen.lopnStyle >> pen.lopnWidth >> pen.lopnColor;
      return *this;
    }
    /*!
     * Output an Extended Logical Pen definition.
     * \param pen Extended Logical Pen definition to output.
     */
    DATASTREAM& operator<< ( const EXTLOGPEN& pen )
    {
      // *** How big is this structure if there are no style entries? ***
      *this << pen.elpPenStyle << pen.elpWidth << pen.elpBrushStyle << pen.elpColor
	    << pen.elpHatch << pen.elpNumEntries;
      return *this;
    }
    /*!
     * Input an Extended Logical Pen definition.
     * \param pen destination of Extended Logical Pen definition.
     */
    DATASTREAM& operator>> ( EXTLOGPEN& pen )
    {
      // *** How big is this structure if there are no style entries? ***
      *this >> pen.elpPenStyle >> pen.elpWidth >> pen.elpBrushStyle >> pen.elpColor
	    >> pen.elpHatch >> pen.elpNumEntries;
      return *this;
    }
    /*!
     * Output a Logical Brush definition.
     * \param brush Logical Brush definition to output.
     */
    DATASTREAM& operator<< ( const LOGBRUSH& brush )
    {
      *this << brush.lbStyle << brush.lbColor << brush.lbHatch;
      return *this;
    }
    /*!
     * Input a Logical Brush definition.
     * \param brush destination of Logical Brush definition.
     */
    DATASTREAM& operator>> ( LOGBRUSH& brush )
    {
      *this >> brush.lbStyle >> brush.lbColor >> brush.lbHatch;
      return *this;
    }
    /*!
     * Output a Logical Font definition (using WCHAR strings).
     * \param font Logical Font definition to output.
     */
    DATASTREAM& operator<< ( const LOGFONTW& font )
    {
      *this << font.lfHeight << font.lfWidth << font.lfEscapement
	    << font.lfOrientation << font.lfWeight << font.lfItalic
	    << font.lfUnderline << font.lfStrikeOut << font.lfCharSet
	    << font.lfOutPrecision << font.lfClipPrecision << font.lfQuality
	    << font.lfPitchAndFamily
	    << WCHARSTR( const_cast<WCHAR*const>(font.lfFaceName), LF_FACESIZE );
      return *this;
    }
    /*!
     * Input a Logical Font definition (using WCHAR strings).
     * \param font destination of Logical Font definition.
     */
    DATASTREAM& operator>> ( LOGFONTW& font )
    {
      WCHARSTR wFaceName( font.lfFaceName, LF_FACESIZE );

      *this >> font.lfHeight >> font.lfWidth >> font.lfEscapement
	    >> font.lfOrientation >> font.lfWeight >> font.lfItalic
	    >> font.lfUnderline >> font.lfStrikeOut >> font.lfCharSet
	    >> font.lfOutPrecision >> font.lfClipPrecision >> font.lfQuality
	    >> font.lfPitchAndFamily
	    >> wFaceName;
      return *this;
    }
    /*!
     * Output a Panose structure.
     * \param panose Panose structure to output.
     */
    DATASTREAM& operator<< ( const PANOSE& panose )
    {
      fwrite( &panose, sizeof(PANOSE), 1, fp_ );
      return *this;
    }
    /*!
     * Input a Panose structure.
     * \param panose destinatino of input Panose structure.
     */
    DATASTREAM& operator>> ( PANOSE& panose )
    {
      fread( &panose, sizeof(PANOSE), 1, fp_ );
      return *this;
    }
    /*!
     * Output an Extended Logical Font definition (using WCHAR strings).
     * \param font Extended Logical Font definition to output.
     */
    DATASTREAM& operator<< ( const EXTLOGFONTW& font )
    {
      *this << font.elfLogFont
	    << WCHARSTR( const_cast<WCHAR*const>(font.elfFullName),
			 LF_FULLFACESIZE )
	    << WCHARSTR( const_cast<WCHAR*const>(font.elfStyle), LF_FACESIZE )
	    << font.elfVersion << font.elfStyleSize << font.elfMatch
	    << font.elfReserved
	    << BYTEARRAY( const_cast<BYTE*const>(font.elfVendorId),
			  ELF_VENDOR_SIZE )
	    << font.elfCulture << font.elfPanose;
      return *this;
    }
    /*!
     * Input an Extended Logical Font definition (using WCHAR strings).
     * \param font destination of Extended Logical Font definition.
     */
    DATASTREAM& operator>> ( EXTLOGFONTW& font )
    {
      WCHARSTR wFullName( font.elfFullName, LF_FULLFACESIZE );
      WCHARSTR wStyle( font.elfStyle, LF_FACESIZE );
      BYTEARRAY bVendorId( font.elfVendorId, ELF_VENDOR_SIZE );
      *this >> font.elfLogFont
	    >> wFullName >> wStyle
	    >> font.elfVersion >> font.elfStyleSize >> font.elfMatch
	    >> font.elfReserved >> bVendorId
	    >> font.elfCulture >> font.elfPanose;
      return *this;
    }
    /*!
     * Output a Logical Palette.
     * \param palette Logical Palette to output.
     */
    DATASTREAM& operator<< ( const LOGPALETTE& palette )
    {
      // *** How big is this structure if the palette is empty? ***
      *this << palette.palVersion << palette.palNumEntries;
      return *this;
    }
    /*!
     * Input a Logical Palette.
     * \param palette destination of input Logical Palette.
     */
    DATASTREAM& operator>> ( LOGPALETTE& palette )
    {
      // *** How big is this structure if the palette is empty? ***
      *this >> palette.palVersion >> palette.palNumEntries;
      return *this;
    }
  };

  class METAFILEDEVICECONTEXT;

  //! The base class of all metafile records
  /*!
   * A metafile consists off a sequence of graphics records "executed"
   * in order. This is a common base class that allows each, different,
   * record to be stored in a common list. An interface is specified
   * for each record to write itself to a file.
   */
  class METARECORD {
  public:
    /*!
     * Execute the graphics command in the given context. Used by
     * PlayEnhMetaFile to "copy" one metafile into another.
     * \param source the device context from which this record is taken.
     * \param dc the destination context.
     */
    virtual void execute ( METAFILEDEVICECONTEXT* source, HDC dc ) const = 0;
    /*!
     * Write yourself to the given file. This is virtual since some records
     * are of arbitrary length and need to write additional information
     * after their EMR structure.
     * \param ds the datastream to write oneself to.
     */
    virtual bool serialize ( DATASTREAM ds ) = 0;
    /*!
     * The header record of a metafile records the total size of the metafile
     * in bytes, so as each record is added to the list, it updates the
     * total size.
     */
    virtual int size ( void ) const = 0;
    /*!
     * The virtual destructor allows records which allocated additional memory
     * to release it when they are deleted. Simple records just use the default
     * destructor defined here.
     */
    virtual ~METARECORD( ) { }
#ifdef ENABLE_EDITING
    /*!
     * This is an optional element of the METARECORD: print yourself to
     * stdout.
     */
    virtual void edit ( void ) const {}
#endif
  };

#ifdef ENABLE_EDITING
  /* Miscellaneous editing routines */
  inline void edit_rectl ( const char* tag, const RECTL& rectl )
  {
    printf( "\t%s\t: (%ld, %ld) - (%ld, %ld)\n", tag, rectl.left, rectl.top,
	    rectl.right, rectl.bottom );
  }

  inline void edit_xform ( const char* tag, const XFORM& xform )
  {
    printf( "\t%s.eM11\t: %f\n", tag, xform.eM11 );
    printf( "\t%s.eM12\t: %f\n", tag, xform.eM12 );
    printf( "\t%s.eM21\t: %f\n", tag, xform.eM21 );
    printf( "\t%s.eM22\t: %f\n", tag, xform.eM22 );
    printf( "\t%s.eDx\t: %f\n", tag, xform.eDx );
    printf( "\t%s.eDy\t: %f\n", tag, xform.eDy );
  }

  inline void edit_color ( const char* tag, const COLORREF& color )
  {
    printf( "\t%s\t: R(0x%02lx) G(0x%02lx) B(0x%02lx)\n", tag,
	    GetRValue( color ), GetGValue( color ), GetBValue( color ) );
  }

  inline void edit_sizel ( const char* tag, const SIZEL& size )
  {
    printf( "\t%s\t: (%ld, %ld)\n", tag, size.cx, size.cy );
  }

  inline void edit_pointl ( const char* tag, const POINTL& point )
  {
    printf( "\t%s\t: (%ld, %ld)\n", tag, point.x, point.y );
  }

  inline void edit_pointlarray ( const char* tag, const DWORD cptl,
				  const POINTL* points )
  {
    printf( "\tcptl%s\t: %ld\n", tag, cptl );
    printf( "\taptl%s\t: ", tag );
    if ( cptl > 0 )
      printf( "%ld, %ld\n", points[0].x, points[0].y );
    else
      puts( "" );
    for ( DWORD i = 1; i < cptl; i++ )
      printf( "\t\t%s  %ld, %ld\n", tag, points[i].x, points[i].y );
  }

  inline void edit_point16array ( const char* tag, const unsigned int cpts,
				  const POINT16* points )
  {
    printf( "\tcpts%s\t: %d\n", tag, cpts );
    printf( "\tapts%s\t: ", tag );
    if ( cpts > 0 )
      printf( "%d, %d\n", points[0].x, points[0].y );
    else
      puts( "" );
    for ( unsigned int i = 1; i < cpts; i++ )
      printf( "\t\t%s  %d, %d\n", tag, points[i].x, points[i].y );
  }

  inline void edit_pen_style ( const char* tag, DWORD style )
  {
    printf( "\t%s\t: ", tag );
    switch ( style & PS_STYLE_MASK ) {
    case PS_SOLID: printf( "PS_SOLID" ); break;
    case PS_DASH: printf( "PS_DASH" ); break;
    case PS_DOT: printf( "PS_DOT" ); break;
    case PS_DASHDOT: printf( "PS_DASHDOT" ); break;
    case PS_DASHDOTDOT: printf( "PS_DASHDOTDOT" ); break;
    case PS_NULL: printf( "PS_NULL" ); break;
    case PS_INSIDEFRAME: printf( "PS_INSIDEFRAME" ); break;
    case PS_USERSTYLE: printf( "PS_USERSTYLE" ); break;
    case PS_ALTERNATE: printf( "PS_ALTERNATE" ); break;
    }
    switch ( style & PS_ENDCAP_MASK ) {
    case PS_ENDCAP_ROUND: printf( " | PS_ENDCAP_ROUND" ); break;
    case PS_ENDCAP_SQUARE: printf( " | PS_ENDCAP_SQUARE" ); break;
    case PS_ENDCAP_FLAT: printf( " | PS_ENDCAP_FLAT" ); break;
    }
    switch ( style & PS_JOIN_MASK ) {
    case PS_JOIN_ROUND: printf( " | PS_JOIN_ROUND" ); break;
    case PS_JOIN_BEVEL: printf( " | PS_JOIN_BEVEL" ); break;
    case PS_JOIN_MITER: printf( " | PS_JOIN_MITER" ); break;
    }
    switch ( style & PS_TYPE_MASK ) {
    case PS_COSMETIC: printf( " | PS_COSMETIC" ); break;
    case PS_GEOMETRIC: printf( " | PS_GEOMETRIC" ); break;
    }
    printf( "\n" );
  }

  inline void edit_brush_style ( const char* tag, DWORD style )
  {
    printf( "\t%s\t: ", tag );
    switch ( style ) {
    case BS_SOLID: printf( "BS_SOLID" ); break;
    case BS_NULL: printf( "BS_NULL" ); break;
    case BS_HATCHED: printf( "BS_HATCHED" ); break;
    case BS_PATTERN: printf( "BS_PATTERN" ); break;
    case BS_INDEXED: printf( "BS_INDEXED" ); break;
    case BS_DIBPATTERN: printf( "BS_DIBPATTERN" ); break;
    case BS_DIBPATTERNPT: printf( "BS_DIBPATTERNPT" ); break;
    case BS_PATTERN8X8: printf( "BS_PATTERN8X8" ); break;
    case BS_DIBPATTERN8X8: printf( "BS_DIBPATTERN8X8" ); break;
    case BS_MONOPATTERN: printf( "BS_DIBPATTERN8X8" ); break;
    default: printf( "unknown(%ld)", style );
    }
    printf( "\n" );
  }

  inline void edit_brush_hatch ( const char* tag, DWORD hatch )
  {
    printf( "\t%s\t: ", tag );
    switch ( hatch ) {
    case HS_HORIZONTAL: printf( "HS_HORIZONTAL" ); break;
    case HS_VERTICAL: printf( "HS_VERTICAL" ); break;
    case HS_FDIAGONAL: printf( "HS_FDIAGONAL" ); break;
    case HS_BDIAGONAL: printf( "HS_BDIAGONAL" ); break;
    case HS_CROSS: printf( "HS_CROSS" ); break;
    case HS_DIAGCROSS: printf( "HS_DIAGCROSS" ); break;
    default: printf( "unknown(%ld)", hatch );
    }
    printf( "\n" );
  }
#endif
  /*!
   * The W32 interface defines objects in terms of handles (rather
   * than pointers).  In order to emulate this, each object is placed
   * into a global list and the index in that list becomes their handle.
   * Objects return this type when asked. The values for this enum are
   * taken from the WINE definitions.
   */
  enum OBJECTTYPE { O_METAFILEDEVICECONTEXT	= OBJ_METADC,
		    O_FONT 			= OBJ_FONT,
		    O_PEN			= OBJ_PEN,
		    O_EXTPEN			= OBJ_EXTPEN,
		    O_BRUSH			= OBJ_BRUSH,
		    O_PALETTE			= OBJ_PAL };
#if 0
  /*!
   * For debugging purposes, it's handy to have a text description of the object.
   */
  static char* typStr ( OBJECTTYPE type )
  {
    switch (type) {
    case O_METAFILEDEVICECONTEXT:
      return "metafile device context";
    case O_FONT:
      return "font";
    case O_PEN:
      return "pen";
    case O_EXTPEN:
      return "extended pen";
    case O_BRUSH:
      return "brush";
    case O_PALETTE:
      return "palette";
    }
    return "unknown object";
  }
#endif
  //! Global GDI object
  /*!
   * The GDI interface defines objects in terms of handles (rather
   * than pointers).  In order to emulate this, each object is placed
   * into a global list and the index in that list becomes their handle.
   */
  class OBJECT {
  public:
    HGDIOBJ handle;		//!< The handle of a GDI object.
    //! OBJECTs have a virtual destructor.
    virtual ~OBJECT () {}
    /*!
     * Create a new object. It's up to a subclass to create a real
     * handle for this object.
     */
    OBJECT ( void ) : handle( 0 ) {}
    /*!
     * Return the type of the object.
     */
    virtual OBJECTTYPE getType ( void ) const = 0;
  };

  //! A global graphics object
  /*!
   * Graphics objects have some additional properties: When an object is
   * Select'ed into a device context, the handle for that context is
   * added to the list of context's in which this object is used.
   */
  class GRAPHICSOBJECT : public OBJECT {
  public:
    //! GRAPHICSOBJECTs has a virtual destructor.
    virtual ~GRAPHICSOBJECT () {}
    /*!
     * A set of all the contexts into which this object has been selected and
     * the associated metafile handle for the object.
     */
    std::map< HDC, HGDIOBJ > contexts;
    /*!
     * Create a new metarecord which describes this object.
     * \param handle (appears not to used. Note the handle is really
     * assigned at serialization time.)
     */
    virtual METARECORD* newEMR ( HDC dc, HGDIOBJ handle ) = 0;
  };

  typedef METARECORD*(*METARECORDCTOR)(DATASTREAM&);

  class GLOBALOBJECTS {
    /*!
     * A vector of all objects created by the program.
     */
    std::vector<OBJECT*> objects;

    /*!
     * (The code for) reading a metafile is somewhat simplified by
     * having the constructors for each record type stored in this
     * map. The "virtual" constructors can then be invoked via
     * indexing.
     */
    std::map< DWORD, METARECORDCTOR > new_records;

  public:
    GLOBALOBJECTS ( void );
    ~GLOBALOBJECTS ( void );
    HGDIOBJ add ( OBJECT* object );
    OBJECT* find ( const HGDIOBJ handle );
    void remove ( const OBJECT* object );

    std::vector<EMF::OBJECT*>::const_iterator begin ( void ) const
    { return objects.begin(); }
    std::vector<EMF::OBJECT*>::const_iterator end ( void ) const
    { return objects.end(); }

    METARECORDCTOR newRecord ( DWORD iType ) const;

    static EMF::METARECORD* new_eof ( DATASTREAM& ds );
    static EMF::METARECORD* new_setviewportorgex ( DATASTREAM& ds );
    static EMF::METARECORD* new_setwindoworgex ( DATASTREAM& ds );
    static EMF::METARECORD* new_setviewportextex ( DATASTREAM& ds );
    static EMF::METARECORD* new_setwindowextex ( DATASTREAM& ds );
    static EMF::METARECORD* new_scaleviewportextex ( DATASTREAM& ds );
    static EMF::METARECORD* new_scalewindowextex ( DATASTREAM& ds );
    static EMF::METARECORD* new_modifyworldtransform ( DATASTREAM& ds );
    static EMF::METARECORD* new_setworldtransform ( DATASTREAM& ds );
    static EMF::METARECORD* new_settextalign ( DATASTREAM& ds );
    static EMF::METARECORD* new_settextcolor ( DATASTREAM& ds );
    static EMF::METARECORD* new_setbkcolor ( DATASTREAM& ds );
    static EMF::METARECORD* new_setbkmode ( DATASTREAM& ds );
    static EMF::METARECORD* new_setpolyfillmode ( DATASTREAM& ds );
    static EMF::METARECORD* new_setmapmode ( DATASTREAM& ds );
    static EMF::METARECORD* new_selectobject ( DATASTREAM& ds );
    static EMF::METARECORD* new_deleteobject ( DATASTREAM& ds );
    static EMF::METARECORD* new_movetoex ( DATASTREAM& ds );
    static EMF::METARECORD* new_lineto ( DATASTREAM& ds );
    static EMF::METARECORD* new_arc ( DATASTREAM& ds );
    static EMF::METARECORD* new_arcto ( DATASTREAM& ds );
    static EMF::METARECORD* new_rectangle ( DATASTREAM& ds );
    static EMF::METARECORD* new_ellipse ( DATASTREAM& ds );
    static EMF::METARECORD* new_polyline ( DATASTREAM& ds );
    static EMF::METARECORD* new_polyline16 ( DATASTREAM& ds );
    static EMF::METARECORD* new_polygon ( DATASTREAM& ds );
    static EMF::METARECORD* new_polygon16 ( DATASTREAM& ds );
    static EMF::METARECORD* new_polypolygon ( DATASTREAM& ds );
    static EMF::METARECORD* new_polypolygon16 ( DATASTREAM& ds );
    static EMF::METARECORD* new_polybezier ( DATASTREAM& ds );
    static EMF::METARECORD* new_polybezier16 ( DATASTREAM& ds );
    static EMF::METARECORD* new_polybezierto ( DATASTREAM& ds );
    static EMF::METARECORD* new_polybezierto16 ( DATASTREAM& ds );
    static EMF::METARECORD* new_polylineto ( DATASTREAM& ds );
    static EMF::METARECORD* new_polylineto16 ( DATASTREAM& ds );
    static EMF::METARECORD* new_exttextouta ( DATASTREAM& ds );
    static EMF::METARECORD* new_setpixelv ( DATASTREAM& ds );
    static EMF::METARECORD* new_createpen ( DATASTREAM& ds );
    static EMF::METARECORD* new_extcreatepen ( DATASTREAM& ds );
    static EMF::METARECORD* new_createbrushindirect ( DATASTREAM& ds );
    static EMF::METARECORD* new_extcreatefontindirectw ( DATASTREAM& ds );
    static EMF::METARECORD* new_fillpath ( DATASTREAM& ds );
    static EMF::METARECORD* new_strokepath ( DATASTREAM& ds );
    static EMF::METARECORD* new_strokeandfillpath ( DATASTREAM& ds );
    static EMF::METARECORD* new_beginpath ( DATASTREAM& ds );
    static EMF::METARECORD* new_endpath ( DATASTREAM& ds );
    static EMF::METARECORD* new_closefigure ( DATASTREAM& ds );
    static EMF::METARECORD* new_savedc ( DATASTREAM& ds );
    static EMF::METARECORD* new_restoredc ( DATASTREAM& ds );
    static EMF::METARECORD* new_setmetargn ( DATASTREAM& ds );
  };

  extern GLOBALOBJECTS globalObjects;

  //! Enhanced Metafile Header Record
  /**
   * The ENHMETAHEADER serves two purposes in this library: it keeps track
   * of the size of the metafile (in physical dimensions) and number of records
   * and handles that are ultimately to be written to the disk file. It is
   * also a real record that must be written out.
   */
  class ENHMETAHEADER : public METARECORD, public ::ENHMETAHEADER {

    LPWSTR description_w;
    int description_size;

  public:
    /*!
     * \param description an optional description argument is a UNICODE-like
     * string with the following format: "some text\0some more text\0\0".
     * The W32 interface defines UNICODE characters to be two-byte
     * (unsigned short strings). The constructor makes a copy of the argument.
     */
    ENHMETAHEADER ( LPCWSTR description = 0 )
      : description_w( 0 ), description_size( 0 )
    {
      iType = EMR_HEADER;
      nSize = sizeof( ::ENHMETAHEADER );

      // Compute the bounds
      RECTL default_bounds = { 0, 0, 0, 0 };
      rclBounds = default_bounds;
      RECTL default_frame = { 0, 0, 0, 0 };
      rclFrame = default_frame;
      dSignature = ENHMETA_SIGNATURE;
      nVersion = 0x10000;
      nBytes = nSize;
      nRecords = 1;
      nHandles = 0;
      sReserved = 0;
      nDescription = 0;
      offDescription = 0;
      nPalEntries = 0;
      szlDevice.cx = XMAX_PIXELS;
      szlDevice.cy = YMAX_PIXELS;
      szlMillimeters.cx = XMAX_MM;
      szlMillimeters.cy = YMAX_MM;
      //
      cbPixelFormat = 0;
      offPixelFormat = 0;
      bOpenGL = FALSE;
      //
#if 0
      szlMicrometers.cx = 1000 * szlMillimeters.cx;
      szlMicrometers.cy = 1000 * szlMillimeters.cy;
#endif
      if ( description ) {
	// Count the number of characters in the description
	int description_count = 0, nulls = 0;
	LPCWSTR description_p = description;
	while ( nulls < 3 ) {
	  description_count++;
	  if ( (*description_p++) == 0 ) nulls++;
	}

	// Make sure that the TOTAL record length will be a multiple of 4

	int record_size = ROUND_TO_LONG( sizeof( ::ENHMETAHEADER ) +
					 sizeof( WCHAR ) * description_count );
	description_size =
	  (record_size - sizeof( ::ENHMETAHEADER )) / sizeof( WCHAR );

	description_w = new WCHAR[ description_size ];

	memset( description_w, 0, sizeof(WCHAR) * description_size );

	for ( int i=0; i<description_count; i++ )
	  description_w[i] = *description++;

	nSize = nBytes = record_size;
	nDescription = description_count;
	offDescription = sizeof( ::ENHMETAHEADER );
      }
    }
    
    /*!
     * Destructor deletes memory allocated for description.
     */
    ~ENHMETAHEADER ( )
    {
      if ( description_w ) delete[] description_w;
    }
    /*!
     * Serializing the header is an example of an extended record.
     * \param ds metafile datastream.
     */
    bool serialize ( DATASTREAM ds )
    {
      ds << iType << nSize
	 << rclBounds << rclFrame
	 << dSignature << nVersion << nBytes << nRecords << nHandles << sReserved
	 << nDescription << offDescription << nPalEntries
	 << szlDevice << szlMillimeters
	 << cbPixelFormat << offPixelFormat << bOpenGL
#if 0
	 << szlMicrometers
#endif
	 << WCHARSTR( description_w, description_size );
      return true;
    }
    /*!
     * Read a header record from the datastream.
     */
    bool unserialize ( DATASTREAM ds )
    {
      ds >> iType >> nSize
	 >> rclBounds >> rclFrame
	 >> dSignature >> nVersion >> nBytes >> nRecords >> nHandles >> sReserved
	 >> nDescription >> offDescription >> nPalEntries
	 >> szlDevice >> szlMillimeters;

      // Some elements of the metafile header were added at later dates

#define OffsetOf( a, b ) ((unsigned int)(((char*)&(((::ENHMETAHEADER*)a)->b)) - \
(char*)((::ENHMETAHEADER*)a)))
#if 0
      if ( OffsetOf( this, szlMicrometers ) <= offDescription )
	ds >> cbPixelFormat >> offPixelFormat >> bOpenGL;
#else
      if ( sizeof(::ENHMETAHEADER) <= offDescription )
	ds >> cbPixelFormat >> offPixelFormat >> bOpenGL;
#endif
#undef OffsetOf
#if 0
      if ( sizeof(::ENHMETAHEADER) <= offDescription )
	ds >> szlMicrometers;
#endif
      // Should now probably check that the offset is correct...

      description_size = ( nSize - offDescription ) / sizeof(WCHAR);
      description_w = new WCHAR[ description_size ];

      WCHARSTR description( description_w, description_size );

      ds >> description;

      return true;
    }
    /*!
     * Internally computed size of this record.
     */
    int size ( void ) const { return nSize; }
    /*!
     * Execute this record in the context of the given device context.
     * \param source the device context from which this record is taken.
     * \param dc device context for execute.
     */
    void execute ( METAFILEDEVICECONTEXT* /*source*/, HDC /*dc*/ ) const
    {
      // Actually handled by the destination device context.
    }
#ifdef ENABLE_EDITING
    /*!
     * Print it to stdout.
     */
    void edit ( void ) const
    {
      printf( "*HEADER*\n" );
      printf( "\tiType\t\t\t: %ld\n", iType );
      printf( "\tnSize\t\t\t: %ld\n", nSize );
      edit_rectl( "rclBounds\t", rclBounds );
      edit_rectl( "rclFrame\t", rclFrame );
      printf( "\tdSignature\t\t: %.4s\n", (const char*)&dSignature );
      printf( "\tnVersion\t\t: 0x%x\n", (unsigned int)nVersion );
      printf( "\tnBytes\t\t\t: %ld\n", nBytes );
      printf( "\tnRecords\t\t: %ld\n", nRecords );
      printf( "\tnHandles\t\t: %d\n", nHandles );
      printf( "\tnDescription\t\t: %ld\n", nDescription );
      printf( "\toffDescription\t\t: %ld\n", offDescription );
      printf( "\tnPalEntries\t\t: %ld\n", nPalEntries );
      edit_sizel( "szlDevice\t", szlDevice );
      edit_sizel( "szlMillimeters\t", szlMillimeters );

      /* Make a crude guess as to the age of this file */
#define OffsetOf( a, b ) ((unsigned int)(((const char*)&(((const ::ENHMETAHEADER*)a)->b)) - \
(const char*)((const ::ENHMETAHEADER*)a)))

      if ( OffsetOf( this, cbPixelFormat ) <= offDescription ) {
	printf( "\tcbPixelFormat\t\t: %ld\n", cbPixelFormat );
	printf( "\toffPixelFormat\t\t: %ld\n", offPixelFormat );
	printf( "\tbOpenGL\t\t\t: %ld\n", bOpenGL );
#if 0
	if ( sizeof(::ENHMETAHEADER) <= offDescription ) {
	  edit_sizel( "szlMicrometers\t", szlMicrometers );
	}
#endif
      }

#undef OffsetOf

      if ( nDescription != 0 ) {

	wchar_t last_w = 0;
	WCHAR* description = description_w;

	printf( "\tDescription:" );

	for ( DWORD i = 0; i < nDescription; i++ ) {

	  wchar_t w = *description++; /* This is not true, really. UNICODE is not
				       * glibc's wide character representation */

	  if ( w != 0 ) {
	    if ( last_w == 0 ) printf( "\n\t\t" );
	    putchar( w );
	  }
	  
	  last_w = w;
	}
	printf( "\n" );
      }
    }
#endif /* ENABLE_EDITING */
  };

  //! EMF End of File Record
  /*!
   * Every metafile must have an End of File record. A palette may also
   * be recorded in the EOF record, but it is currently unused by
   * this library (all colors are specified in full RGB coordinates).
   */
  class EMREOF : public METARECORD, ::EMREOF {
  public:
    /*!
     * Constructor contains no user serviceable parts.
     */
    EMREOF ( void )
    {
      emr.iType = EMR_EOF;
      emr.nSize = sizeof( ::EMREOF );
      nPalEntries = 0;
      offPalEntries = 0;
      nSizeLast = 0;
    }

    /*!
     * Construct an EOF record from the input stream
     * \param ds Metafile datastream.
     */
    EMREOF ( DATASTREAM& ds )
    {
      ds >> emr >> nPalEntries >> offPalEntries >> nSizeLast;
    }

    /*!
     * \param ds Metafile datastream.
     */
    bool serialize ( DATASTREAM ds )
    {
      ds << emr << nPalEntries << offPalEntries << nSizeLast;
      return true;
    }
    /*!
     * Internally computed size of this record.
     */
    int size ( void ) const { return emr.nSize; }
    /*!
     * Execute this record in the context of the given device context.
     * \param source the device context from which this record is taken.
     * \param dc device context for execute.
     */
    void execute ( METAFILEDEVICECONTEXT* /*source*/, HDC /*dc*/ ) const
    {
      // Actually handled by the destination device context.
    }
#ifdef ENABLE_EDITING
    /*!
     * Print it to stdout.
     */
    void edit ( void ) const
    {
      printf( "*EOF*\n" );
    }
#endif /* ENABLE_EDITING */
  };

  //! EMF Set Viewport Origin (ex)
  /*!
   * The viewport origin is a point in device coordinates (i.e., pixels)
   * where the viewport starts. (For example, if you want to put several
   * different views on the same page, you might use different viewports.)
   */
  class EMRSETVIEWPORTORGEX : public METARECORD, ::EMRSETVIEWPORTORGEX {
  public:
    /*!
     * \param x x position of the viewport in device coordinates
     * \param y y position of the viewport in device coordinates
     */
    EMRSETVIEWPORTORGEX ( INT x, INT y )
    {
      emr.iType = EMR_SETVIEWPORTORGEX;
      emr.nSize = sizeof( ::EMRSETVIEWPORTORGEX );
      ptlOrigin.x = x;
      ptlOrigin.y = y;
    }
    /*!
     * Construct a SetVieportOrgEx record from the input stream.
     * \param ds Metafile datastream.
     */
    EMRSETVIEWPORTORGEX ( DATASTREAM& ds )
    {
      ds >> emr >> ptlOrigin;
    }
    /*!
     * \param fp Metafile file handle.
     */
    bool serialize ( DATASTREAM ds )
    {
      ds << emr << ptlOrigin;
      return true;
    }
    /*!
     * Internally computed size of this record.
     */
    int size ( void ) const { return emr.nSize; }
    /*!
     * Execute this record in the context of the given device context.
     * \param source the device context from which this record is taken.
     * \param dc device context for execute.
     */
    void execute ( METAFILEDEVICECONTEXT* /*source*/, HDC dc ) const
    {
      SetViewportOrgEx( dc, ptlOrigin.x, ptlOrigin.y, 0 );
    }
#ifdef ENABLE_EDITING
    /*!
     * Print it to stdout.
     */
    void edit ( void ) const
    {
      printf( "*SETVIEWPORTORGEX*\n" );
      edit_pointl( "ptlOrigin", ptlOrigin );
    }
#endif /* ENABLE_EDITING */
  };

  //! EMF Set Window Origin (ex)
  /*!
   * The window origin specifies the logical (i.e., real)
   * coordinates of the upper, left corner of the viewport. (For
   * example, if you want your XY plot's axis origin to be in the
   * middle of the viewport, you'd set the window origin to something
   * like [-1,-1].)
   */
  class EMRSETWINDOWORGEX : public METARECORD, ::EMRSETWINDOWORGEX {
  public:
    /*!
     * \param x x coordinate of window origin in logical coordinates
     * \param y y coordinate of window origin in logical coordinates
     */
    EMRSETWINDOWORGEX ( INT x, INT y )
    {
      emr.iType = EMR_SETWINDOWORGEX;
      emr.nSize = sizeof( ::EMRSETWINDOWORGEX );
      ptlOrigin.x = x;
      ptlOrigin.y = y;
    }
    /*!
     * Construct a SetWindowOrgEx record from the input stream.
     * \param ds Metafile datastream.
     */
    EMRSETWINDOWORGEX ( DATASTREAM& ds )
    {
      ds >> emr >> ptlOrigin;
    }
    /*!
     * \param fp Metafile file handle.
     */
    bool serialize ( DATASTREAM ds )
    {
      ds << emr << ptlOrigin;
      return true;
    }
    /*!
     * Internally computed size of this record.
     */
    int size ( void ) const { return emr.nSize; }
    /*!
     * Execute this record in the context of the given device context.
     * \param source the device context from which this record is taken.
     * \param dc device context for execute.
     */
    void execute ( METAFILEDEVICECONTEXT* /*source*/, HDC dc ) const
    {
      SetWindowOrgEx( dc, ptlOrigin.x, ptlOrigin.y, 0 );
    }
#ifdef ENABLE_EDITING
    /*!
     * Print it to stdout.
     */
    void edit ( void ) const
    {
      printf( "*SETWINDOWORGEX*\n" );
      edit_pointl( "ptlOrigin", ptlOrigin );
    }
#endif /* ENABLE_EDITING */
  };

  //! EMF Set Viewport Extents (ex)
  /*!
   * The viewport extent is the device coordinate (i.e. pixels) size of the
   * viewport. Since W32 doesn't do any clipping, the purpose of this
   * is not clear.
   */
  class EMRSETVIEWPORTEXTEX : public METARECORD, ::EMRSETVIEWPORTEXTEX {
  public:
    /*!
     * \param cx width of viewport in device coordinates
     * \param cy height of viewport in device coordinates
     */
    EMRSETVIEWPORTEXTEX ( INT cx, INT cy )
    {
      emr.iType = EMR_SETVIEWPORTEXTEX;
      emr.nSize = sizeof( ::EMRSETVIEWPORTEXTEX );
      szlExtent.cx = cx;
      szlExtent.cy = cy;
    }
    /*!
     * Construct a SetViewportExtEx record from the input stream.
     * \param ds Metafile datastream.
     */
    EMRSETVIEWPORTEXTEX ( DATASTREAM& ds )
    {
      ds >> emr >> szlExtent;
    }
    /*!
     * \param fp Metafile file handle.
     */
    bool serialize ( DATASTREAM ds )
    {
      ds << emr << szlExtent;
      return true;
    }
    /*!
     * Internally computed size of this record.
     */
    int size ( void ) const { return emr.nSize; }
    /*!
     * Execute this record in the context of the given device context.
     * \param source the device context from which this record is taken.
     * \param dc device context for execute.
     */
    void execute ( METAFILEDEVICECONTEXT* /*source*/, HDC dc ) const
    {
      SetViewportExtEx( dc, szlExtent.cx, szlExtent.cy, 0 );
    }
#ifdef ENABLE_EDITING
    /*!
     * Print it to stdout.
     */
    void edit ( void ) const
    {
      printf( "*SETVIEWPORTEXTEX*\n" );
      edit_sizel( "szlExtent", szlExtent );
    }
#endif /* ENABLE_EDITING */
  };

  //! EMF Scale Viewport Extents (ex)
  /*!
   * The viewport extent is the device coordinate (i.e. pixels) size of the
   * viewport. Scale the viewport extents by the ratios of the given
   * values. (OpenOffice accepts this, but not SETVIEWPORTEXT(?))
   */
  class EMRSCALEVIEWPORTEXTEX : public METARECORD, ::EMRSCALEVIEWPORTEXTEX {
  public:
    /*!
     * \param x_num numerator of x scale
     * \param x_den denominator of x scale
     * \param y_num numerator of y scale
     * \param y_den denominator of y scale
     */
    EMRSCALEVIEWPORTEXTEX ( LONG x_num, LONG x_den, LONG y_num, LONG y_den )
    {
      emr.iType = EMR_SCALEVIEWPORTEXTEX;
      emr.nSize = sizeof( ::EMRSCALEVIEWPORTEXTEX );
      xNum = x_num;
      xDenom = x_den;
      yNum = y_num;
      yDenom = y_den;
    }
    /*!
     * Construct a ScaleViewportExtEx record from the input stream.
     * \param ds Metafile datastream.
     */
    EMRSCALEVIEWPORTEXTEX ( DATASTREAM& ds )
    {
      ds >> emr >> xNum >> xDenom >> yNum >> yDenom;
    }
    /*!
     * \param fp Metafile file handle.
     */
    bool serialize ( DATASTREAM ds )
    {
      ds << emr << xNum << xDenom << yNum << yDenom;
      return true;
    }
    /*!
     * Internally computed size of this record.
     */
    int size ( void ) const { return emr.nSize; }
    /*!
     * Execute this record in the context of the given device context.
     * \param source the device context from which this record is taken.
     * \param dc device context for execute.
     */
    void execute ( METAFILEDEVICECONTEXT* /*source*/, HDC dc ) const
    {
      ScaleViewportExtEx( dc, xNum, xDenom, yNum, yDenom, 0 );
    }
#ifdef ENABLE_EDITING
    /*!
     * Print it to stdout.
     */
    void edit ( void ) const
    {
      printf( "*SCALEVIEWPORTEXTEX*\n" );
      printf( "\txNum\t: %ld\n", xNum );
      printf( "\txDenom\t: %ld\n", xDenom );
      printf( "\tyNum\t: %ld\n", yNum );
      printf( "\tyDenom\t: %ld\n", yDenom );
    }
#endif /* ENABLE_EDITING */
  };

  //! EMF Set Window Extent (ex)
  /**
   * The window extents define the scale of the logical coordinates. For
   * example, if your XY plot is from [-10,-10] to [10,10], then
   * the window extents are [20,20].
   */
  class EMRSETWINDOWEXTEX : public METARECORD, ::EMRSETWINDOWEXTEX {
  public:
    /*!
     * \param cx width of window in logical coordinates.
     * \param cy height of window in logical coordinates.
     */
    EMRSETWINDOWEXTEX ( INT cx, INT cy )
    {
      emr.iType = EMR_SETWINDOWEXTEX;
      emr.nSize = sizeof( ::EMRSETWINDOWEXTEX );
      szlExtent.cx = cx;
      szlExtent.cy = cy;
    }
    /*!
     * Construct a SetWindowExtEx record from the input stream.
     * \param ds Metafile datastream.
     */
    EMRSETWINDOWEXTEX ( DATASTREAM& ds )
    {
      ds >> emr >> szlExtent;
    }
    /*!
     * \param fp Metafile file handle.
     */
    bool serialize ( DATASTREAM ds )
    {
      ds << emr << szlExtent;
      return true;
    }
    /*!
     * Internally computed size of this record.
     */
    int size ( void ) const { return emr.nSize; }
    /*!
     * Execute this record in the context of the given device context.
     * \param source the device context from which this record is taken.
     * \param dc device context for execute.
     */
    void execute ( METAFILEDEVICECONTEXT* /*source*/, HDC dc ) const
    {
      SetWindowExtEx( dc, szlExtent.cx, szlExtent.cy, 0 );
    }
#ifdef ENABLE_EDITING
    /*!
     * Print it to stdout.
     */
    void edit ( void ) const
    {
      printf( "*SETWINDOWEXTEX*\n" );
      edit_sizel( "szlExtent", szlExtent );
    }
#endif /* ENABLE_EDITING */
  };

  //! EMF Scale Window Extents (ex)
  /*!
   * The window extent is the logical coordinate size of the
   * window. Scale the window extents by the ratios of the given
   * values.
   */
  class EMRSCALEWINDOWEXTEX : public METARECORD, ::EMRSCALEWINDOWEXTEX {
  public:
    /*!
     * \param x_num numerator of x scale
     * \param x_den denominator of x scale
     * \param y_num numerator of y scale
     * \param y_den denominator of y scale
     */
    EMRSCALEWINDOWEXTEX ( LONG x_num, LONG x_den, LONG y_num, LONG y_den )
    {
      emr.iType = EMR_SCALEWINDOWEXTEX;
      emr.nSize = sizeof( ::EMRSCALEWINDOWEXTEX );
      xNum = x_num;
      xDenom = x_den;
      yNum = y_num;
      yDenom = y_den;
    }
    /*!
     * Construct a ScaleWindowExtEx record from the input stream.
     * \param ds Metafile datastream.
     */
    EMRSCALEWINDOWEXTEX ( DATASTREAM& ds )
    {
      ds >> emr >> xNum >> xDenom >> yNum >> yDenom;
    }
    /*!
     * \param fp Metafile file handle.
     */
    bool serialize ( DATASTREAM ds )
    {
      ds << emr << xNum << xDenom << yNum << yDenom;
      return true;
    }
    /*!
     * Internally computed size of this record.
     */
    int size ( void ) const { return emr.nSize; }
    /*!
     * Execute this record in the context of the given device context.
     * \param source the device context from which this record is taken.
     * \param dc device context for execute.
     */
    void execute ( METAFILEDEVICECONTEXT* /*source*/, HDC dc ) const
    {
      ScaleWindowExtEx( dc, xNum, xDenom, yNum, yDenom, 0 );
    }
#ifdef ENABLE_EDITING
    /*!
     * Print it to stdout.
     */
    void edit ( void ) const
    {
      printf( "*SCALEWINDOWEXTEX*\n" );
      printf( "\txNum\t: %ld\n", xNum );
      printf( "\txDenom\t: %ld\n", xDenom );
      printf( "\tyNum\t: %ld\n", yNum );
      printf( "\tyDenom\t: %ld\n", yDenom );
    }
#endif /* ENABLE_EDITING */
  };

  //! EMF Modify World Transform
  /*!
   * Enhanced metafiles have a Coordinate Transformation which allows
   * the contents to be rotated and transformed. Does not appear to work
   * properly in StarOffice (but it's also possible I don't understand
   * how it's supposed to work either).
   */
  class EMRMODIFYWORLDTRANSFORM : public METARECORD, ::EMRMODIFYWORLDTRANSFORM {
  public:
    /*!
     * \param transform the transformation to apply
     * \param mode the mode of the transformation application (namely,
     * pre- or post-multiply)
     */
    EMRMODIFYWORLDTRANSFORM ( const XFORM* transform, DWORD mode )
    {
      emr.iType = EMR_MODIFYWORLDTRANSFORM;
      emr.nSize = sizeof( ::EMRMODIFYWORLDTRANSFORM );
      xform = *transform;
      iMode = mode;
    }
    /*!
     * Construct a ModifyWorldTransform from the input datastream.
     * \param fp Metafile file handle.
     */
    EMRMODIFYWORLDTRANSFORM ( DATASTREAM& ds )
    {
      ds >> emr >> xform >> iMode;
    }
    /*!
     * \param fp Metafile file handle.
     */
    bool serialize ( DATASTREAM ds )
    {
      ds << emr << xform << iMode;
      return true;
    }
    /*!
     * Internally computed size of this record.
     */
    int size ( void ) const { return emr.nSize; }
    /*!
     * Execute this record in the context of the given device context.
     * \param source the device context from which this record is taken.
     * \param dc device context for execute.
     */
    void execute ( METAFILEDEVICECONTEXT* /*source*/, HDC dc ) const
    {
      ModifyWorldTransform( dc, &xform, iMode );
    }
#ifdef ENABLE_EDITING
    /*!
     * Print it to stdout.
     */
    void edit ( void ) const
    {
      printf( "*MODIFYWORLDTRANSFORM*\n" );
      edit_xform( "xform", xform );
      printf( "\tiMode\t\t: " );
      switch ( iMode ) {
      case MWT_IDENTITY: printf( "MWT_IDENTITY\n" ); break;
      case MWT_LEFTMULTIPLY: printf( "MWT_LEFTMULTIPLY\n" ); break;
      case MWT_RIGHTMULTIPLY: printf( "MWT_RIGHTMULTIPLY\n" ); break;
      default: printf( "unknown(%ld)\n", iMode );
      }
    }
#endif /* ENABLE_EDITING */
  };

  //! EMF Set World Transform
  /*!
   * Enhanced metafiles have a Coordinate Transformation which allows
   * the contents to be rotated and transformed. Does not appear to work
   * properly in StarOffice (but it's also possible I don't understand
   * how it's supposed to work either).
   */
  class EMRSETWORLDTRANSFORM : public METARECORD, ::EMRSETWORLDTRANSFORM {
  public:
    /*!
     * \param transform the new transformation
     */
    EMRSETWORLDTRANSFORM ( const XFORM* transform )
    {
      emr.iType = EMR_SETWORLDTRANSFORM;
      emr.nSize = sizeof( ::EMRSETWORLDTRANSFORM );
      xform = *transform;
    }
    /*!
     * Construct a SetWorldTransform record from the input stream.
     * \param fp Metafile file handle.
     */
    EMRSETWORLDTRANSFORM ( DATASTREAM& ds )
    {
      ds >> emr >> xform;
    }
    /*!
     * \param fp Metafile file handle.
     */
    bool serialize ( DATASTREAM ds )
    {
      ds << emr << xform;
      return true;
    }
    /*!
     * Internally computed size of this record.
     */
    int size ( void ) const { return emr.nSize; }
    /*!
     * Execute this record in the context of the given device context.
     * \param source the device context from which this record is taken.
     * \param dc device context for execute.
     */
    void execute ( METAFILEDEVICECONTEXT* /*source*/, HDC dc ) const
    {
      SetWorldTransform( dc, &xform );
    }
#ifdef ENABLE_EDITING
    /*!
     * Print it to stdout.
     */
    void edit ( void ) const
    {
      printf( "*SETWORLDTRANSFORM*\n" );
      edit_xform( "xform", xform );
    }
#endif /* ENABLE_EDITING */
  };

  //! EMF Set Text Alignment
  /*!
   * Determines the justification of the text with respect to its position.
   */
  class EMRSETTEXTALIGN : public METARECORD, ::EMRSETTEXTALIGN {
  public:
    /*!
     * \param mode text alignment mode.
     */
    EMRSETTEXTALIGN ( UINT mode )
    {
      emr.iType = EMR_SETTEXTALIGN;
      emr.nSize = sizeof( ::EMRSETTEXTALIGN );
      iMode = mode;
    }
    /*!
     * Construct a SetTextAlign record from the input datastream.
     * \param fp Metafile file handle.
     */
    EMRSETTEXTALIGN ( DATASTREAM& ds )
    {
      ds >> emr >> iMode;
    }
    /*!
     * \param fp Metafile file handle.
     */
    bool serialize ( DATASTREAM ds )
    {
      ds << emr << iMode;
      return true;
    }
    /*!
     * Internally computed size of this record.
     */
    int size ( void ) const { return emr.nSize; }
    /*!
     * Execute this record in the context of the given device context.
     * \param source the device context from which this record is taken.
     * \param dc device context for execute.
     */
    void execute ( METAFILEDEVICECONTEXT* /*source*/, HDC dc ) const
    {
      SetTextAlign( dc, iMode );
    }
#ifdef ENABLE_EDITING
    /*!
     * Print it to stdout.
     */
    void edit ( void ) const
    {
      unsigned int known_bits = TA_BASELINE+TA_CENTER+TA_UPDATECP+TA_RTLREADING;
      unsigned int unknown_bits = ~known_bits;

      printf( "*SETTEXTALIGN*\n" );
      printf( "\tiMode\t: " );
      if ( iMode & TA_UPDATECP )
	printf( "TA_UPDATECP" );
      else
	printf( "TA_NOUPDATECP" );
      if ( iMode & TA_CENTER )
	printf( " | TA_CENTER" );
      else if ( iMode & TA_RIGHT )
	printf( " | TA_RIGHT" );
      else
	printf( " | TA_LEFT" );
      if ( iMode & TA_BASELINE )
	printf( " | TA_BASELINE" );
      else if ( iMode & TA_BOTTOM )
	printf( " | TA_BOTTOM" );
      else
	printf( " | TA_TOP" );
      if ( iMode & TA_RTLREADING )
	printf( " | TA_RTLREADING" );
      if ( iMode & unknown_bits )
	printf( " | unknown bits(0x%lx)", iMode & unknown_bits );
      printf( "\n" );
    }
#endif /* ENABLE_EDITING */
  };

  //! EMF Set Text Color
  /*!
   * Sets the foreground color of text.
   */
  class EMRSETTEXTCOLOR : public METARECORD, ::EMRSETTEXTCOLOR {
  public:
    /*!
     * \param color text foreground color
     */
    EMRSETTEXTCOLOR ( COLORREF color )
    {
      emr.iType = EMR_SETTEXTCOLOR;
      emr.nSize = sizeof( ::EMRSETTEXTCOLOR );
      crColor = color;
    }
    /*!
     * Construct a SetTextColor record from the input stream.
     * \param ds Metafile datastream.
     */
    EMRSETTEXTCOLOR ( DATASTREAM& ds )
    {
      ds >> emr >> crColor;
    }
    /*!
     * \param ds Metafile datastream.
     */
    bool serialize ( DATASTREAM ds )
    {
      ds << emr << crColor;
      return true;
    }
    /*!
     * Internally computed size of this record.
     */
    int size ( void ) const { return emr.nSize; }
    /*!
     * Execute this record in the context of the given device context.
     * \param source the device context from which this record is taken.
     * \param dc device context for execute.
     */
    void execute ( METAFILEDEVICECONTEXT* /*source*/, HDC dc ) const
    {
      SetTextColor( dc, crColor );
    }
#ifdef ENABLE_EDITING
    /*!
     * Print it to stdout.
     */
    void edit ( void ) const
    {
      printf( "*SETTEXTCOLOR*\n" );
      edit_color( "crColor", crColor );
    }
#endif /* ENABLE_EDITING */
  };

  //! EMF Set Background Color
  /*!
   * Sets the background color.
   */
  class EMRSETBKCOLOR : public METARECORD, ::EMRSETBKCOLOR {
  public:
    /*!
     * \param color background color
     */
    EMRSETBKCOLOR ( COLORREF color )
    {
      emr.iType = EMR_SETBKCOLOR;
      emr.nSize = sizeof( ::EMRSETBKCOLOR );
      crColor = color;
    }
    /*!
     * Construct a SetBkColor record from the input datastream.
     * \param ds Metafile datastream.
     */
    EMRSETBKCOLOR ( DATASTREAM& ds )
    {
      ds >> emr >> crColor;
    }
    /*!
     * \param fp Metafile file handle.
     */
    bool serialize ( DATASTREAM ds )
    {
      ds << emr << crColor;
      return true;
    }
    /*!
     * Internally computed size of this record.
     */
    int size ( void ) const { return emr.nSize; }
    /*!
     * Execute this record in the context of the given device context.
     * \param source the device context from which this record is taken.
     * \param dc device context for execute.
     */
    void execute ( METAFILEDEVICECONTEXT* /*source*/, HDC dc ) const
    {
      SetBkColor( dc, crColor );
    }
#ifdef ENABLE_EDITING
    /*!
     * Print it to stdout.
     */
    void edit ( void ) const
    {
      printf( "*SETBKCOLOR*\n" );
      edit_color( "crColor", crColor );
    }
#endif /* ENABLE_EDITING */
  };

  //! EMF Set Background Mode
  /*!
   * Set the background mode: transparent or opaque. Seems to be ignored
   * by StarOffice. (Appears to work for text, though.)
   */
  class EMRSETBKMODE : public METARECORD, ::EMRSETBKMODE {
  public:
    /*!
     * \param mode background mode.
     */
    EMRSETBKMODE ( DWORD mode )
    {
      emr.iType = EMR_SETBKMODE;
      emr.nSize = sizeof( ::EMRSETBKMODE );
      iMode = mode;
    }
    /*!
     * Construct a SetBkMode record from the input datastream.
     * \param ds Metafile datastream.
     */
    EMRSETBKMODE ( DATASTREAM& ds )
    {
      ds >> emr >> iMode;
    }
    /*!
     * \param ds Metafile datastream.
     */
    bool serialize ( DATASTREAM ds )
    {
      ds << emr << iMode;
      return true;
    }
    /*!
     * Internally computed size of this record.
     */
    int size ( void ) const { return emr.nSize; }
    /*!
     * Execute this record in the context of the given device context.
     * \param source the device context from which this record is taken.
     * \param dc device context for execute.
     */
    void execute ( METAFILEDEVICECONTEXT* /*source*/, HDC dc ) const
    {
      SetBkMode( dc, iMode );
    }
#ifdef ENABLE_EDITING
    /*!
     * Print it to stdout.
     */
    void edit ( void ) const
    {
      printf( "*SETBKMODE*\n" );
      printf( "\tiMode\t: " );
      switch ( iMode ) {
      case TRANSPARENT: printf( "TRANSPARENT\n" ); break;
      case OPAQUE: printf( "OPAQUE\n" ); break;
      default: printf( "unknown(%ld)\n", iMode );
      }
    }
#endif /* ENABLE_EDITING */
  };

  //! EMF Set the Polygon Fill Mode
  /*!
   * Set the polygon fill mode: ALTERNATE or WINDING
   */
  class EMRSETPOLYFILLMODE : public METARECORD, ::EMRSETPOLYFILLMODE {
  public:
    /*!
     * \param mode background mode.
     */
    EMRSETPOLYFILLMODE ( DWORD mode )
    {
      emr.iType = EMR_SETPOLYFILLMODE;
      emr.nSize = sizeof( ::EMRSETPOLYFILLMODE );
      iMode = mode;
    }
    /*!
     * Construct a SetPolyFillMode record from the input stream.
     * \param ds Metafile datastream.
     */
    EMRSETPOLYFILLMODE ( DATASTREAM& ds )
    {
      ds >> emr >> iMode;
    }
    /*!
     * \param ds Metafile datastream.
     */
    bool serialize ( DATASTREAM ds )
    {
      ds << emr << iMode;
      return true;
    }
    /*!
     * Internally computed size of this record.
     */
    int size ( void ) const { return emr.nSize; }
    /*!
     * Execute this record in the context of the given device context.
     * \param source the device context from which this record is taken.
     * \param dc device context for execute.
     */
    void execute ( METAFILEDEVICECONTEXT* /*source*/, HDC dc ) const
    {
      SetPolyFillMode( dc, iMode );
    }
#ifdef ENABLE_EDITING
    /*!
     * Print it to stdout.
     */
    void edit ( void ) const
    {
      printf( "*SETPOLYFILLMODE*\n" );
      printf( "\tiMode: " );
      switch ( iMode ) {
      case ALTERNATE: printf( "ALTERNATE\n" ); break;
      case WINDING: printf( "WINDING\n" ); break;
      default: printf( "unknown(%ld)\n", iMode );
      }
    }
#endif /* ENABLE_EDITING */
  };

  //! EMF Set Mapping Mode
  /*!
   * Set the mapping mode: HI (X style), LO (OpenGL style). Totally ignored
   * by StarOffice as near as I can tell.
   */
  class EMRSETMAPMODE : public METARECORD, ::EMRSETMAPMODE {
  public:
    /*!
     * \param mode window mapping mode
     */
    EMRSETMAPMODE ( DWORD mode )
    {
      emr.iType = EMR_SETMAPMODE;
      emr.nSize = sizeof( ::EMRSETMAPMODE );
      iMode = mode;
    }
    /*!
     * Construct a SetMapMode record from the input stream.
     * \param ds Metafile datastream.
     */
    EMRSETMAPMODE ( DATASTREAM& ds )
    {
      ds >> emr >> iMode;
    }
    /*!
     * \param ds Metafile datastream.
     */
    bool serialize ( DATASTREAM ds )
    {
      ds << emr << iMode;
      return true;
    }
    /*!
     * \param fp Metafile file handle.
     */
    int size ( void ) const { return emr.nSize; }
    /*!
     * Execute this record in the context of the given device context.
     * \param source the device context from which this record is taken.
     * \param dc device context for execute.
     */
    void execute ( METAFILEDEVICECONTEXT* /*source*/, HDC dc ) const
    {
      SetMapMode( dc, iMode );
    }
#ifdef ENABLE_EDITING
    /*!
     * Print it to stdout.
     */
    void edit ( void ) const
    {
      printf( "*SETMAPMODE*\n" );
      printf( "\tiMode\t: " );
      switch ( iMode ) {
      case MM_TEXT: printf( "MM_TEXT\n" ); break;
      case MM_LOMETRIC: printf( "MM_LOMETRIC\n" ); break;
      case MM_HIMETRIC: printf( "MM_HIMETRIC\n" ); break;
      case MM_LOENGLISH: printf( "MM_LOENGLISH\n" ); break;
      case MM_HIENGLISH: printf( "MM_HIENGLISH\n" ); break;
      case MM_TWIPS: printf( "MM_TWIPS\n" ); break;
      case MM_ISOTROPIC: printf( "MM_ISOTROPIC\n" ); break;
      case MM_ANISOTROPIC: printf( "MM_ANISOTROPIC\n" ); break;
      default: printf( "unknown(%ld)\n", iMode );
      }
    }
#endif /* ENABLE_EDITING */
  };

  //! EMF Select Object
  /**
   * Activate (make current) the given object, such as a pen, brush or font.
   */
  class EMRSELECTOBJECT : public METARECORD, ::EMRSELECTOBJECT {
  public:
    /*!
     * \param object the object to make active.
     */
    EMRSELECTOBJECT ( HGDIOBJ object )
    {
      emr.iType = EMR_SELECTOBJECT;
      emr.nSize = sizeof( ::EMRSELECTOBJECT );
      ihObject = object;
    }
    /*!
     * Construct a SelectObject record from the input stream.
     * \param ds Metafile datastream.
     */
    EMRSELECTOBJECT ( DATASTREAM& ds )
    {
      ds >> emr >> ihObject;
    }
    /*!
     * \param ds Metafile datastream.
     */
    bool serialize ( DATASTREAM ds )
    {
      ds << emr << ihObject;
      return true;
    }
    /*!
     * Internally computed size of this record.
     */
    int size ( void ) const { return emr.nSize; }
    /*!
     * Execute this record in the context of the given device context.
     * \param source the device context from which this record is taken.
     * \param dc device context for execute.
     */
    void execute ( METAFILEDEVICECONTEXT* source, HDC dc ) const;
#ifdef ENABLE_EDITING
    /*!
     * Print it to stdout.
     */
    void edit ( void ) const
    {
      printf( "*SELECTOBJECT*\n" );
      printf( "\tihObject\t: 0x%lx\n", ihObject );
    }
#endif /* ENABLE_EDITING */
  };

  //! EMF Delete Object
  /**
   * Delete the given object, such as a pen, brush or font.
   */
  class EMRDELETEOBJECT : public METARECORD, ::EMRDELETEOBJECT {
  public:
    /*!
     * \param object the object to delete.
     */
    EMRDELETEOBJECT ( HGDIOBJ object )
    {
      emr.iType = EMR_DELETEOBJECT;
      emr.nSize = sizeof( ::EMRDELETEOBJECT );
      ihObject = object;
    }
    /*!
     * Construct a DeleteObject record from the input stream.
     * \param ds Metafile datastream.
     */
    EMRDELETEOBJECT ( DATASTREAM& ds )
    {
      ds >> emr >> ihObject;
    }
    /*!
     * \param ds Metafile datastream.
     */
    bool serialize ( DATASTREAM ds )
    {
      ds << emr << ihObject;
      return true;
    }
    /*!
     * Internally computed size of this record.
     */
    int size ( void ) const { return emr.nSize; }
    /*!
     * Execute this record in the context of the given device context.
     * \param source the device context from which this record is taken.
     * \param dc device context for execute.
     */
    void execute ( METAFILEDEVICECONTEXT* source, HDC dc ) const;
#ifdef ENABLE_EDITING
    /*!
     * Print it to stdout.
     */
    void edit ( void ) const
    {
      printf( "*DELETEOBJECT*\n" );
      printf( "\tihObject\t: 0x%lx\n", ihObject );
    }
#endif /* ENABLE_EDITING */
  };

  //! EMF MoveTo (ex)
  /*!
   * Move the drawing point to the given position.
   */
  class EMRMOVETOEX : public METARECORD, ::EMRMOVETOEX {
  public:
    /*!
     * \param x new x position in logical coordinates.
     * \param y new y position in logical coordinates.
     */
    EMRMOVETOEX ( INT x, INT y )
    {
      emr.iType = EMR_MOVETOEX;
      emr.nSize = sizeof( ::EMRMOVETOEX );
      ptl.x = x;
      ptl.y = y;
    }
    /*!
     * Construct a MoveToEx record from the input stream.
     * \param ds Metafile datastream.
     */
    EMRMOVETOEX ( DATASTREAM& ds )
    {
      ds >> emr >> ptl;
    }
    /*!
     * \param ds Metafile datastream.
     */
    bool serialize ( DATASTREAM ds )
    {
      ds << emr << ptl;
      return true;
    }
    /*!
     * Internally computed size of this record.
     */
    int size ( void ) const { return emr.nSize; }
    /*!
     * Execute this record in the context of the given device context.
     * \param source the device context from which this record is taken.
     * \param dc device context for execute.
     */
    void execute ( METAFILEDEVICECONTEXT* /*source*/, HDC dc ) const
    {
      MoveToEx( dc,  ptl.x, ptl.y, 0 );
    }
#ifdef ENABLE_EDITING
    /*!
     * Print it to stdout.
     */
    void edit ( void ) const
    {
      printf( "*MOVETOEX*\n" );
      edit_pointl( "ptl", ptl );
    }
#endif /* ENABLE_EDITING */
  };

  //! EMF Line To
  /*!
   * Draw a line using the current pen to the given position.
   */
  class EMRLINETO : public METARECORD, ::EMRLINETO {
  public:
    /*!
     * \param x x position to draw line to in logical coordinates.
     * \param y y position to draw line to in logical coordinates.
     */
    EMRLINETO ( INT x, INT y )
    {
      emr.iType = EMR_LINETO;
      emr.nSize = sizeof( ::EMRLINETO );
      ptl.x = x;
      ptl.y = y;
    }
    /*!
     * Construct a LineTo record from the input stream.
     * \param ds Metafile datastream
     */
    EMRLINETO ( DATASTREAM& ds )
    {
      ds >> emr >> ptl;
    }
    /*!
     * \param ds Metafile datastream
     */
    bool serialize ( DATASTREAM ds )
    {
      ds << emr << ptl;
      return true;
    }
    /*!
     * Internally computed size of this record.
     */
    int size ( void ) const { return emr.nSize; }
    /*!
     * Execute this record in the context of the given device context.
     * \param source the device context from which this record is taken.
     * \param dc device context for execute.
     */
    void execute ( METAFILEDEVICECONTEXT* /*source*/, HDC dc ) const
    {
      LineTo( dc,  ptl.x, ptl.y );
    }
#ifdef ENABLE_EDITING
    /*!
     * Print it to stdout.
     */
    void edit ( void ) const
    {
      printf( "*LINETO*\n" );
      edit_pointl( "ptl", ptl );
    }
#endif /* ENABLE_EDITING */
  };

  //! EMF Arc
  /*!
   * Draw an arc. Not sure what the specification here means, though.
   */
  class EMRARC : public METARECORD, ::EMRARC {
  public:
    /*!
     * Take these descriptions with a grain of salt...
     * \param left x position of left edge of arc box.
     * \param top y position of top edge of arc box.
     * \param right x position of right edge of arc box.
     * \param bottom y position bottom edge of arc box.
     * \param xstart x position of arc start.
     * \param ystart y position of arc start.
     * \param xend x position of arc end.
     * \param yend y position of arc end.
     */
    EMRARC ( INT left, INT top, INT right, INT bottom, INT xstart,
	     INT ystart, INT xend, INT yend )
    {
      emr.iType = EMR_ARC;
      emr.nSize = sizeof( ::EMRARC );
      rclBox.left = left;
      rclBox.right = right;
      rclBox.bottom = bottom;
      rclBox.top = top;
      ptlStart.x = xstart;
      ptlStart.y = ystart;
      ptlEnd.x = xend;
      ptlEnd.y = yend;
    }
    /*!
     * Construct an Arc record from the input datastream.
     * \param ds Metafile datastream.
     */
    EMRARC ( DATASTREAM& ds )
    {
      ds >> emr >> rclBox >> ptlStart >> ptlEnd;
    }
    /*!
     * \param fp Metafile file handle.
     */
    bool serialize ( DATASTREAM ds )
    {
      ds << emr << rclBox << ptlStart << ptlEnd;
      return true;
    }
    /*!
     * Internally computed size of this record.
     */
    int size ( void ) const { return emr.nSize; }
    /*!
     * Execute this record in the context of the given device context.
     * \param source the device context from which this record is taken.
     * \param dc device context for execute.
     */
    void execute ( METAFILEDEVICECONTEXT* /*source*/, HDC dc ) const
    {
      Arc( dc, rclBox.left, rclBox.top, rclBox.right, rclBox.bottom,
	   ptlStart.x, ptlStart.y, ptlEnd.x, ptlEnd.y );
    }
#ifdef ENABLE_EDITING
    /*!
     * Print it to stdout.
     */
    void edit ( void ) const
    {
      printf( "*ARC*\n" );
      edit_rectl( "rclBox\t", rclBox );
      edit_pointl( "ptlStart", ptlStart );
      edit_pointl( "ptlEnd\t", ptlEnd );
    }
#endif /* ENABLE_EDITING */
  };

  //! EMF Arc To
  /*!
   * Draw another arc. Not sure what the specification here means, though.
   */
  class EMRARCTO : public METARECORD, ::EMRARCTO {
  public:
    /*!
     * Take these descriptions with a grain of salt...
     * \param left x position of left edge of arc box.
     * \param top y position of top edge of arc box.
     * \param right x position of right edge of arc box.
     * \param bottom y position bottom edge of arc box.
     * \param xstart x position of arc start.
     * \param ystart y position of arc start.
     * \param xend x position of arc end.
     * \param yend y position of arc end.
     */
    EMRARCTO ( INT left, INT top, INT right, INT bottom, INT xstart,
	     INT ystart, INT xend, INT yend )
    {
      emr.iType = EMR_ARCTO;
      emr.nSize = sizeof( ::EMRARCTO );
      rclBox.left = left;
      rclBox.right = right;
      rclBox.bottom = bottom;
      rclBox.top = top;
      ptlStart.x = xstart;
      ptlStart.y = ystart;
      ptlEnd.x = xend;
      ptlEnd.y = yend;
    }
    /*!
     * Construct an ArcTo record from the input stream.
     * \param ds Metafile datastream.
     */
    EMRARCTO ( DATASTREAM& ds )
    {
      ds >> emr >> rclBox >> ptlStart >> ptlEnd;
    }
    /*!
     * \param fp Metafile file handle.
     */
    bool serialize ( DATASTREAM ds )
    {
      ds << emr << rclBox << ptlStart << ptlEnd;
      return true;
    }
    /*!
     * Internally computed size of this record.
     */
    int size ( void ) const { return emr.nSize; }
    /*!
     * Execute this record in the context of the given device context.
     * \param source the device context from which this record is taken.
     * \param dc device context for execute.
     */
    void execute ( METAFILEDEVICECONTEXT* /*source*/, HDC dc ) const
    {
      ArcTo( dc, rclBox.left, rclBox.top, rclBox.right, rclBox.bottom,
	     ptlStart.x, ptlStart.y, ptlEnd.x, ptlEnd.y );
    }
#ifdef ENABLE_EDITING
    /*!
     * Print it to stdout.
     */
    void edit ( void ) const
    {
      printf( "*ARCTO*\n" );
      edit_rectl( "rclBox\t", rclBox );
      edit_pointl( "ptlStart", ptlStart );
      edit_pointl( "ptlEnd\t", ptlEnd );
    }
#endif /* ENABLE_EDITING */
  };

  //! EMF Rectangle
  /*!
   * Draw a rectangle.
   */
  class EMRRECTANGLE : public METARECORD, ::EMRRECTANGLE {
  public:
    /*!
     * \param left x position of left side of rectangle.
     * \param top y position of top side of rectangle.
     * \param right x position of right edge of rectangle.
     * \param bottom y position of bottom edge of rectangle.
     */
    EMRRECTANGLE ( INT left, INT top, INT right, INT bottom )
    {
      emr.iType = EMR_RECTANGLE;
      emr.nSize = sizeof( ::EMRRECTANGLE );
      rclBox.left = left;
      rclBox.right = right;
      rclBox.bottom = bottom;
      rclBox.top = top;
    }
    /*!
     * Construct a Rectangle record from the input stream.
     * \param ds Metafile datastream.
     */
    EMRRECTANGLE ( DATASTREAM& ds )
    {
      ds >> emr >> rclBox;
    }
    /*!
     * \param ds Metafile datastream.
     */
    bool serialize ( DATASTREAM ds )
    {
      ds << emr << rclBox;
      return true;
    }
    /*!
     * Internally computed size of this record.
     */
    int size ( void ) const { return emr.nSize; }
    /*!
     * Execute this record in the context of the given device context.
     * \param source the device context from which this record is taken.
     * \param dc device context for execute.
     */
    void execute ( METAFILEDEVICECONTEXT* /*source*/, HDC dc ) const
    {
      Rectangle( dc, rclBox.left, rclBox.top, rclBox.right, rclBox.bottom );
    }
#ifdef ENABLE_EDITING
    /*!
     * Print it to stdout.
     */
    void edit ( void ) const
    {
      printf( "*RECTANGLE*\n" );
      edit_rectl( "rclBox", rclBox );
    }
#endif /* ENABLE_EDITING */
  };

  //! EMF Ellipse
  /*!
   * Draw an ellipse. (I have no idea how the ellipse is defined!)
   */
  class EMRELLIPSE : public METARECORD, ::EMRELLIPSE {
  public:
    /*!
     * Take these descriptions with a grain of salt...
     * \param left x position of left extrema of ellipse.
     * \param top y position of top extrema of ellipse.
     * \param right x position of right extrema of ellipse.
     * \param bottom y position of bottom extrema of ellipse.
     */
    EMRELLIPSE ( INT left, INT top, INT right, INT bottom )
    {
      emr.iType = EMR_ELLIPSE;
      emr.nSize = sizeof( ::EMRELLIPSE );
      rclBox.left = left;
      rclBox.right = right;
      rclBox.bottom = bottom;
      rclBox.top = top;
    }
    /*!
     * Construct an Ellipse record from the input stream.
     * \param ds Metafile datastream.
     */
    EMRELLIPSE ( DATASTREAM& ds )
    {
      ds >> emr >> rclBox;
    }
    /*!
     * \param ds Metafile datastream.
     */
    bool serialize ( DATASTREAM ds )
    {
      ds << emr << rclBox;
      return true;
    }
    /*!
     * Internally computed size of this record.
     */
    int size ( void ) const { return emr.nSize; }
    /*!
     * Execute this record in the context of the given device context.
     * \param source the device context from which this record is taken.
     * \param dc device context for execute.
     */
    void execute ( METAFILEDEVICECONTEXT* /*source*/, HDC dc ) const
    {
      Ellipse( dc, rclBox.left, rclBox.top, rclBox.right, rclBox.bottom );
    }
#ifdef ENABLE_EDITING
    /*!
     * Print it to stdout.
     */
    void edit ( void ) const
    {
      printf( "*ELLIPSE*\n" );
      edit_rectl( "rclBox", rclBox );
    }
#endif /* ENABLE_EDITING */
  };

  //! EMF Polyline
  /*!
   * Draw a series of connected lines.
   */
  class EMRPOLYLINE : public METARECORD, ::EMRPOLYLINE {
    POINTL* lpoints;
  public:
    /*!
     * \param bounds overall bounding box of polyline.
     * \param points array of polyline vertices.
     * \param n number of vertices in points.
     */
    EMRPOLYLINE ( const RECTL* bounds, const POINT* points, INT n )
    {
      cptl = n;
      aptl[0].x = 0;		// Really unused
      aptl[0].y = 0;

      emr.iType = EMR_POLYLINE;
      // The (cptl - 1) below is to account for aptl, which isn't written out
      emr.nSize = sizeof( ::EMRPOLYLINE ) + sizeof( POINTL ) * ( cptl - 1);

      lpoints = new POINTL[cptl];

      for (int i=0; i<n; i++) {
	lpoints[i].x = points[i].x;
	lpoints[i].y = points[i].y;
      }

      rclBounds = *bounds;
    }
    /*!
     * Destructor frees a copy of the points it buffered.
     */
    ~EMRPOLYLINE ( )
    {
      if ( lpoints ) delete[] lpoints;
    }
    /*!
     * Construct a Polyline record from the input stream.
     * \param ds Metafile datastream.
     */
    EMRPOLYLINE ( DATASTREAM& ds )
    {
      ds >> emr >> rclBounds >> cptl;

      lpoints = new POINTL[cptl];

      POINTLARRAY points( lpoints, cptl );

      ds >> points;
    }
    /*!
     * \param fp Metafile file handle.
     */
    bool serialize ( DATASTREAM ds )
    {
      ds << emr << rclBounds << cptl << POINTLARRAY( lpoints, cptl );
      return true;
    }
    /*!
     * \param fp Metafile file handle.
     */
    int size ( void ) const { return emr.nSize; }
    /*!
     * Execute this record in the context of the given device context.
     * \param source the device context from which this record is taken.
     * \param dc device context for execute.
     */
    void execute ( METAFILEDEVICECONTEXT* /*source*/, HDC dc ) const
    {
      // According to the wine windef.h header, POINT and POINTL are equivalent
      Polyline( dc, (POINT*)lpoints, cptl );
    }
#ifdef ENABLE_EDITING
    /*!
     * Print it to stdout.
     */
    void edit ( void ) const
    {
      printf( "*POLYLINE*\n" );
      edit_rectl( "rclBounds", rclBounds );
#if 0
      printf( "\tcptl              : %ld\n", cptl );
      printf( "\taptl->\n" );
      for ( unsigned int i = 0; i < cptl; i++ )
	printf( "\t\t%ld, %ld\n", lpoints[i].x, lpoints[i].y );
#else
      edit_pointlarray( "\t", cptl, lpoints );
#endif
    }
#endif /* ENABLE_EDITING */
  };

  //! EMF Polyline16
  /*!
   * Draw a series of connected lines using 16-bit points.
   */
  class EMRPOLYLINE16 : public METARECORD, ::EMRPOLYLINE16 {
    POINT16* lpoints;
  public:
    /*!
     * \param bounds overall bounding box of polyline.
     * \param points array of polyline vertices.
     * \param n number of vertices in points.
     */
    EMRPOLYLINE16 ( const RECTL* bounds, const POINT16* points, INT n )
    {
      cpts = n;
      apts[0].x = 0;		// Really unused
      apts[0].y = 0;

      emr.iType = EMR_POLYLINE16;
      // The (cptl - 1) below is to account for aptl, which isn't written out
      emr.nSize = sizeof( ::EMRPOLYLINE16 ) + sizeof( POINT16 ) * ( cpts - 1);

      lpoints = new POINT16[cpts];

      for (int i=0; i<n; i++) {
	lpoints[i].x = points[i].x;
	lpoints[i].y = points[i].y;
      }

      rclBounds = *bounds;
    }
    /*!
     * Constructor with POINTs.
     * \param bounds overall bounding box of polyline.
     * \param points array of polyline vertices.
     * \param n number of vertices in points.
     */
    EMRPOLYLINE16 ( const RECTL* bounds, const POINT* points, INT n )
    {
      cpts = n;
      apts[0].x = 0;		// Really unused
      apts[0].y = 0;

      emr.iType = EMR_POLYLINE16;
      // The (cptl - 1) below is to account for aptl, which isn't written out
      emr.nSize = sizeof( ::EMRPOLYLINE16 ) + sizeof( POINT16 ) * ( cpts - 1);

      lpoints = new POINT16[cpts];

      for (int i=0; i<n; i++) {
	lpoints[i].x = points[i].x;
	lpoints[i].y = points[i].y;
      }

      rclBounds = *bounds;
    }
    /*!
     * Destructor frees a copy of the points it buffered.
     */
    ~EMRPOLYLINE16 ( )
    {
      if ( lpoints ) delete[] lpoints;
    }
    /*!
     * Construct a Polyline record from the input stream.
     * \param ds Metafile datastream.
     */
    EMRPOLYLINE16 ( DATASTREAM& ds )
    {
      ds >> emr >> rclBounds >> cpts;

      lpoints = new POINT16[cpts];

      POINT16ARRAY points( lpoints, cpts );

      ds >> points;
    }
    /*!
     * \param fp Metafile file handle.
     */
    bool serialize ( DATASTREAM ds )
    {
      ds << emr << rclBounds << cpts << POINT16ARRAY( lpoints, cpts );
      return true;
    }
    /*!
     * \param fp Metafile file handle.
     */
    int size ( void ) const { return emr.nSize; }
    /*!
     * Execute this record in the context of the given device context.
     * \param source the device context from which this record is taken.
     * \param dc device context for execute.
     */
    void execute ( METAFILEDEVICECONTEXT* /*source*/, HDC dc ) const
    {
      // According to the wine windef.h header, POINT and POINTL are equivalent
      Polyline16( dc, lpoints, cpts );
    }
#ifdef ENABLE_EDITING
    /*!
     * Print it to stdout.
     */
    void edit ( void ) const
    {
      printf( "*POLYLINE16*\n" );
      edit_rectl( "rclBounds", rclBounds );
      edit_point16array( "\t", cpts, lpoints );
    }
#endif /* ENABLE_EDITING */
  };

  //! EMF Filled Polygon
  /*!
   * Draw a filled polygon.
   */
  class EMRPOLYGON : public METARECORD, ::EMRPOLYGON {
    POINTL* lpoints;
  public:
    /*!
     * \param bounds overall bounding box of polygon.
     * \param points array of polygon vertices.
     * \param n number of vertices in points.
     */
    EMRPOLYGON ( const RECTL* bounds, const POINT* points, INT n )
    {
      cptl = n;
      aptl[0].x = 0;		// Really unused
      aptl[0].y = 0;

      emr.iType = EMR_POLYGON;
      // The (cptl-1) below is to account for aptl, which isn't written out
      emr.nSize = sizeof( ::EMRPOLYGON ) + sizeof( POINTL ) * (cptl-1);

      lpoints = new POINTL[cptl];

      for (int i=0; i<n; i++) {
	lpoints[i].x = points[i].x;
	lpoints[i].y = points[i].y;
      }

      rclBounds = *bounds;
    }
    /*!
     * Construct a Polygon record from the input stream.
     * \param ds Metafile datastream.
     */
    EMRPOLYGON ( DATASTREAM& ds )
    {
      ds >> emr >> rclBounds >> cptl;

      lpoints = new POINTL[cptl];

      POINTLARRAY points( lpoints, cptl );

      ds >> points;
    }
    /*!
     * Destructor frees a copy of the points it buffered.
     */
    ~EMRPOLYGON ( )
    {
      if ( lpoints ) delete[] lpoints;
    }
    /*!
     * \param fp Metafile file handle.
     */
    bool serialize ( DATASTREAM ds )
    {
      ds << emr << rclBounds << cptl << POINTLARRAY( lpoints, cptl );
      return true;
    }
    /*!
     * Internally computed size of this record.
     */
    int size ( void ) const { return emr.nSize; }
    /*!
     * Execute this record in the context of the given device context.
     * \param source the device context from which this record is taken.
     * \param dc device context for execute.
     */
    void execute ( METAFILEDEVICECONTEXT* /*source*/, HDC dc ) const
    {
      // According to the wine windef.h header, POINT and POINTL are equivalent
      Polygon( dc, (POINT*)lpoints, cptl );
    }
#ifdef ENABLE_EDITING
    /*!
     * Print it to stdout.
     */
    void edit ( void ) const
    {
      printf( "*POLYGON*\n" );
      edit_rectl( "rclBounds", rclBounds );
#if 0
      printf( "\tcptl              : %ld\n", cptl );
      printf( "\taptl->\n" );
      for ( unsigned int i = 0; i < cptl; i++ )
	printf( "\t\t%ld, %ld\n", lpoints[i].x, lpoints[i].y );
#else
      edit_pointlarray( "\t", cptl, lpoints );
#endif
    }
#endif /* ENABLE_EDITING */
  };

  //! EMF Filled Polygon16
  /*!
   * Draw a filled polygon (with 16-bit points).
   */
  class EMRPOLYGON16 : public METARECORD, ::EMRPOLYGON16 {
    POINT16* lpoints;
  public:
    /*!
     * \param bounds overall bounding box of polygon.
     * \param points array of polygon vertices.
     * \param n number of vertices in points.
     */
    EMRPOLYGON16 ( const RECTL* bounds, const POINT* points, INT16 n )
    {
      cpts = n;
      apts[0].x = 0;		// Really unused
      apts[0].y = 0;

      emr.iType = EMR_POLYGON16;
      // The (cptl-1) below is to account for aptl, which isn't written out
      emr.nSize = sizeof( ::EMRPOLYGON16 ) + sizeof( POINT16 ) * (cpts-1);

      lpoints = new POINT16[cpts];

      for (int i=0; i<n; i++) {
	lpoints[i].x = points[i].x;
	lpoints[i].y = points[i].y;
      }

      rclBounds = *bounds;
    }
    /*!
     * Additional constructor which takes a POINT16 array.
     * \param bounds overall bounding box of polygon.
     * \param points array of polygon vertices.
     * \param n number of vertices in points.
     */
    EMRPOLYGON16 ( const RECTL* bounds, const POINT16* points, INT16 n )
    {
      cpts = n;
      apts[0].x = 0;		// Really unused
      apts[0].y = 0;

      emr.iType = EMR_POLYGON16;
      // The (cptl-1) below is to account for aptl, which isn't written out
      emr.nSize = sizeof( ::EMRPOLYGON16 ) + sizeof( POINT16 ) * (cpts-1);

      lpoints = new POINT16[cpts];

      for (int i=0; i<n; i++) {
	lpoints[i].x = points[i].x;
	lpoints[i].y = points[i].y;
      }

      rclBounds = *bounds;
    }
    /*!
     * Construct a Polygon record from the input stream.
     * \param ds Metafile datastream.
     */
    EMRPOLYGON16 ( DATASTREAM& ds )
    {
      ds >> emr >> rclBounds >> cpts;

      lpoints = new POINT16[cpts];

      POINT16ARRAY points( lpoints, cpts );

      ds >> points;
    }
    /*!
     * Destructor frees a copy of the points it buffered.
     */
    ~EMRPOLYGON16 ( )
    {
      if ( lpoints ) delete[] lpoints;
    }
    /*!
     * \param fp Metafile file handle.
     */
    bool serialize ( DATASTREAM ds )
    {
      ds << emr << rclBounds << cpts << POINT16ARRAY( lpoints, cpts );
      return true;
    }
    /*!
     * Internally computed size of this record.
     */
    int size ( void ) const { return emr.nSize; }
    /*!
     * Execute this record in the context of the given device context.
     * \param source the device context from which this record is taken.
     * \param dc device context for execute.
     */
    void execute ( METAFILEDEVICECONTEXT* /*source*/, HDC dc ) const
    {
      // According to the wine windef.h header, POINT and POINTL are equivalent
      Polygon16( dc, lpoints, cpts );
    }
#ifdef ENABLE_EDITING
    /*!
     * Print it to stdout.
     */
    void edit ( void ) const
    {
      printf( "*POLYGON16*\n" );
      edit_rectl( "rclBounds", rclBounds );
      edit_point16array( "\t", cpts, lpoints );
    }
#endif /* ENABLE_EDITING */
  };

  //! EMF Poly Polygon
  /*!
   * Draw several filled polygons.
   */
  class EMRPOLYPOLYGON : public METARECORD, ::EMRPOLYPOLYGON {
    DWORD* lcounts;
    POINTL* lpoints;
  public:
    /*!
     * \param bounds overall bounding box of polygon.
     * \param points array of polygon vertices.
     * \param counts array of number of vertices in each polygon.
     * \param polygons number of polygons.
     */
    EMRPOLYPOLYGON ( const RECTL* bounds, const POINT* points, const INT* counts,
		     UINT polygons )
    {
      nPolys = polygons;
      // Count the number of points in points
      int n = 0;
      for ( unsigned int i = 0; i < nPolys; i++ )
	n += counts[i];

      cptl = n;
      aPolyCounts[0] = 0;	// Really unused
      aptl[0].x = 0;
      aptl[0].y = 0;

      emr.iType = EMR_POLYPOLYGON;
      // The (#-1)'s below are to account for aPolyCounts[0] and aptl[0], which
      // aren't directly written out
      emr.nSize = sizeof( ::EMRPOLYPOLYGON ) + sizeof( POINTL ) * (cptl-1)
	+ sizeof( DWORD ) * (nPolys-1);

      lcounts = new DWORD[nPolys];

      for ( unsigned int i = 0; i < nPolys; i++ )
	lcounts[i] = counts[i];

      lpoints = new POINTL[cptl];

      for (int i=0; i<n; i++) {
	lpoints[i].x = points[i].x;
	lpoints[i].y = points[i].y;
      }

      rclBounds = *bounds;
    }
    /*!
     * Destructor frees a copy of the counts and points it buffered.
     */
    ~EMRPOLYPOLYGON ( )
    {
      if ( lcounts ) delete[] lcounts;
      if ( lpoints ) delete[] lpoints;
    }
    /*!
     * Construct a Polygon record from the input stream.
     * \param ds Metafile datastream.
     */
    EMRPOLYPOLYGON ( DATASTREAM& ds )
    {
      ds >> emr >> rclBounds >> nPolys >> cptl;

      lcounts = new DWORD[nPolys];

      DWORDARRAY counts( lcounts, nPolys );

      ds >> counts;

      lpoints = new POINTL[cptl];

      POINTLARRAY points( lpoints, cptl );

      ds >> points;
    }
    /*!
     * \param fp Metafile file handle.
     */
    bool serialize ( DATASTREAM ds )
    {
      ds << emr << rclBounds << nPolys << cptl << DWORDARRAY( lcounts, nPolys )
	 << POINTLARRAY( lpoints, cptl );
      return true;
    }
    /*!
     * Internally computed size of this record.
     */
    int size ( void ) const { return emr.nSize; }
    /*!
     * Execute this record in the context of the given device context.
     * \param source the device context from which this record is taken.
     * \param dc device context for execute.
     */
    void execute ( METAFILEDEVICECONTEXT* /*source*/, HDC dc ) const
    {
      // According to the wine windef.h header, POINT and POINTL are equivalent
      // (but DWORD and INT are not)
      INT* counts = new INT[nPolys];
      for ( unsigned int i = 0; i < nPolys; i++ )
	counts[i] = lcounts[i];

      PolyPolygon( dc, (POINT*)lpoints, counts, nPolys );

      delete[] counts;
    }
#ifdef ENABLE_EDITING
    /*!
     * Print it to stdout.
     */
    void edit ( void ) const
    {
      printf( "*POLYPOLYGON*\n" );
      edit_rectl( "rclBounds", rclBounds );
      printf( "\tnPolys\t\t: %ld\n", nPolys );
      printf( "\tcptl\t\t: %ld\n", cptl );
      printf( "\taPolyCounts\t: " );
      if ( nPolys > 0 )
	printf( "%ld\n", lcounts[0] );
      else
	puts( "" );
      for ( unsigned int i = 1; i < nPolys; i++ )
	printf( "\t\t\t  %ld\n", lcounts[i] );
      printf( "\tapts\t\t: " );
      if ( cptl > 0 )
	printf( "%ld, %ld\n", lpoints[0].x, lpoints[0].y );
      else
	puts( "" );
      for ( unsigned int i = 1; i < cptl; i++ )
	printf( "\t\t\t  %ld, %ld\n", lpoints[i].x, lpoints[i].y );
    }
#endif /* ENABLE_EDITING */
  };

  //! EMF Poly Polygon16
  /*!
   * Draw several filled polygons (with 16-bit points).
   */
  class EMRPOLYPOLYGON16 : public METARECORD, ::EMRPOLYPOLYGON16 {
    DWORD* lcounts;
    POINT16* lpoints;
  public:
    /*!
     * \param bounds overall bounding box of polygon.
     * \param points array of polygon vertices.
     * \param counts array of number of vertices in each polygon.
     * \param polygons number of polygons.
     */
    EMRPOLYPOLYGON16 ( const RECTL* bounds, const POINT* points,
		       const INT* counts, UINT polygons )
    {
      nPolys = polygons;
      // Count the number of points in points
      int n = 0;
      for ( unsigned int i = 0; i < nPolys; i++ )
	n += counts[i];

      cpts = n;
      aPolyCounts[0] = 0;	// Really unused
      apts[0].x = 0;
      apts[0].y = 0;

      emr.iType = EMR_POLYPOLYGON16;
      // The (#-1)'s below are to account for aPolyCounts[0] and aptl[0], which
      // aren't directly written out
      emr.nSize = sizeof( ::EMRPOLYPOLYGON16 ) + sizeof( POINT16 ) * (cpts-1)
	+ sizeof( DWORD ) * (nPolys-1);

      lcounts = new DWORD[nPolys];

      for ( unsigned int i = 0; i < nPolys; i++ )
	lcounts[i] = counts[i];

      lpoints = new POINT16[cpts];

      for (int i=0; i<n; i++) {
	lpoints[i].x = points[i].x;
	lpoints[i].y = points[i].y;
      }

      rclBounds = *bounds;
    }
    /*!
     * Additional constructor which takes a POINT16 structure.
     * \param bounds overall bounding box of polygon.
     * \param points array of polygon vertices.
     * \param counts array of number of vertices in each polygon.
     * \param polygons number of polygons.
     */
    EMRPOLYPOLYGON16 ( const RECTL* bounds, const POINT16* points,
		       const INT* counts, UINT16 polygons )
    {
      nPolys = polygons;
      // Count the number of points in points
      int n = 0;
      for ( unsigned int i = 0; i < nPolys; i++ )
	n += counts[i];

      cpts = n;
      aPolyCounts[0] = 0;	// Really unused
      apts[0].x = 0;
      apts[0].y = 0;

      emr.iType = EMR_POLYPOLYGON16;
      // The (#-1)'s below are to account for aPolyCounts[0] and aptl[0], which
      // aren't directly written out
      emr.nSize = sizeof( ::EMRPOLYPOLYGON16 ) + sizeof( POINT16 ) * (cpts-1)
	+ sizeof( DWORD ) * (nPolys-1);

      lcounts = new DWORD[nPolys];

      for ( unsigned int i = 0; i < nPolys; i++ )
	lcounts[i] = counts[i];

      lpoints = new POINT16[cpts];

      for (int i=0; i<n; i++) {
	lpoints[i].x = points[i].x;
	lpoints[i].y = points[i].y;
      }

      rclBounds = *bounds;
    }
    /*!
     * Destructor frees a copy of the counts and points it buffered.
     */
    ~EMRPOLYPOLYGON16 ( )
    {
      if ( lcounts ) delete[] lcounts;
      if ( lpoints ) delete[] lpoints;
    }
    /*!
     * Construct a Polygon record from the input stream.
     * \param ds Metafile datastream.
     */
    EMRPOLYPOLYGON16 ( DATASTREAM& ds )
    {
      ds >> emr >> rclBounds >> nPolys >> cpts;

      lcounts = new DWORD[nPolys];

      DWORDARRAY counts( lcounts, nPolys );

      ds >> counts;

      lpoints = new POINT16[cpts];

      POINT16ARRAY points( lpoints, cpts );

      ds >> points;
    }
    /*!
     * \param fp Metafile file handle.
     */
    bool serialize ( DATASTREAM ds )
    {
      ds << emr << rclBounds << nPolys << cpts << DWORDARRAY( lcounts, nPolys )
	 << POINT16ARRAY( lpoints, cpts );
      return true;
    }
    /*!
     * Internally computed size of this record.
     */
    int size ( void ) const { return emr.nSize; }
    /*!
     * Execute this record in the context of the given device context.
     * \param source the device context from which this record is taken.
     * \param dc device context for execute.
     */
    void execute ( METAFILEDEVICECONTEXT* /*source*/, HDC dc ) const
    {
      // According to the wine windef.h header, POINT and POINTL are equivalent
      // (but DWORD and INT are not)
      INT* counts = new INT[nPolys];
      for ( unsigned int i = 0; i < nPolys; i++ )
	counts[i] = lcounts[i];

      PolyPolygon16( dc, lpoints, counts, nPolys );

      delete[] counts;
    }
#ifdef ENABLE_EDITING
    /*!
     * Print it to stdout.
     */
    void edit ( void ) const
    {
      printf( "*POLYPOLYGON16*\n" );
      edit_rectl( "rclBounds", rclBounds );
      printf( "\tnPolys\t\t: %ld\n", nPolys );
      printf( "\tcpts\t\t: %ld\n", cpts );
      printf( "\taPolyCounts\t: " );
      if ( nPolys > 0 )
	printf( "%ld\n", lcounts[0] );
      else
	puts( "" );
      for ( unsigned int i = 1; i < nPolys; i++ )
	printf( "\t\t\t  %ld\n", lcounts[i] );
      printf( "\tapts\t\t: " );
      if ( cpts > 0 )
	printf( "%d, %d\n", lpoints[0].x, lpoints[0].y );
      else
	puts( "" );
      for ( unsigned int i = 1; i < cpts; i++ )
	printf( "\t\t\t  %d, %d\n", lpoints[i].x, lpoints[i].y );
    }
#endif /* ENABLE_EDITING */
  };

  //! EMF Polybezier
  /*!
   * Draw a polygonal Bezier curve to (what?)
   */
  class EMRPOLYBEZIER : public METARECORD, ::EMRPOLYBEZIER {
    POINTL* lpoints;
  public:
    /*!
     * \param bounds overall bounding box of polybezier curve.
     * \param points array of polybezier vertices.
     * \param n number of vertices in points.
     */
    EMRPOLYBEZIER ( const RECTL* bounds, const POINT* points, INT n )
    {
      cptl = n;
      aptl[0].x = 0;		// Really unused
      aptl[0].y = 0;

      emr.iType = EMR_POLYBEZIER;
      // The (cptl-1) below is to account for aptl, which isn't written out
      emr.nSize = sizeof( ::EMRPOLYBEZIER ) + sizeof( POINTL ) * (cptl-1);

      lpoints = new POINTL[cptl];

      for (int i=0; i<n; i++) {
	lpoints[i].x = points[i].x;
	lpoints[i].y = points[i].y;
      }

      rclBounds = *bounds;
    }
    /*!
     * Construct a PolyBezier record from the input stream.
     * \param ds Metafile datastream.
     */
    EMRPOLYBEZIER ( DATASTREAM& ds )
    {
      ds >> emr >> rclBounds >> cptl;

      lpoints = new POINTL[cptl];

      POINTLARRAY points( lpoints, cptl );

      ds >> points;
    }
    /*!
     * Destructor frees a copy of the points it buffered.
     */
    ~EMRPOLYBEZIER ( )
    {
      if ( lpoints ) delete[] lpoints;
    }
    /*!
     * \param ds Metafile datastream.
     */
    bool serialize ( DATASTREAM ds )
    {
      ds << emr << rclBounds << cptl << POINTLARRAY( lpoints, cptl );
      return true;
    }
    /*!
     * Internally computed size of this record.
     */
    int size ( void ) const { return emr.nSize; }
    /*!
     * Execute this record in the context of the given device context.
     * \param source the device context from which this record is taken.
     * \param dc device context for execute.
     */
    void execute ( METAFILEDEVICECONTEXT* /*source*/, HDC dc ) const
    {
      // According to the wine windef.h header, POINT and POINTL are equivalent
      PolyBezier( dc, (POINT*)lpoints, cptl );
    }
#ifdef ENABLE_EDITING
    /*!
     * Print it to stdout.
     */
    void edit ( void ) const
    {
      printf( "*POLYBEZIER*\n" );
      edit_rectl( "rclBounds", rclBounds );
#if 0
      printf( "\tcptl              : %ld\n", cptl );
      printf( "\taptl->\n" );
      for ( unsigned int i = 0; i < cptl; i++ )
	printf( "\t\t%ld, %ld\n", lpoints[i].x, lpoints[i].y );
#else
      edit_pointlarray( "\t", cptl, lpoints );
#endif
    }
#endif /* ENABLE_EDITING */
  };

  //! EMF Polybezier16
  /*!
   * Draw a polygonal Bezier curve to (what?) using 16-bit points.
   */
  class EMRPOLYBEZIER16 : public METARECORD, ::EMRPOLYBEZIER16 {
    POINT16* lpoints;
  public:
    /*!
     * \param bounds overall bounding box of polybezier curve.
     * \param points array of polybezier vertices.
     * \param n number of vertices in points.
     */
    EMRPOLYBEZIER16 ( const RECTL* bounds, const POINT16* points, INT n )
    {
      cpts = n;
      apts[0].x = 0;		// Really unused
      apts[0].y = 0;

      emr.iType = EMR_POLYBEZIER16;
      // The (cptl-1) below is to account for aptl, which isn't written out
      emr.nSize = sizeof( ::EMRPOLYBEZIER16 ) + sizeof( POINT16 ) * (cpts-1);

      lpoints = new POINT16[cpts];

      for (int i=0; i<n; i++) {
	lpoints[i].x = points[i].x;
	lpoints[i].y = points[i].y;
      }

      rclBounds = *bounds;
    }
    /*!
     * Convenience constructor with POINTs.
     * \param bounds overall bounding box of polybezier curve.
     * \param points array of polybezier vertices.
     * \param n number of vertices in points.
     */
    EMRPOLYBEZIER16 ( const RECTL* bounds, const POINT* points, INT n )
    {
      cpts = n;
      apts[0].x = 0;		// Really unused
      apts[0].y = 0;

      emr.iType = EMR_POLYBEZIER16;
      // The (cptl-1) below is to account for aptl, which isn't written out
      emr.nSize = sizeof( ::EMRPOLYBEZIER16 ) + sizeof( POINT16 ) * (cpts-1);

      lpoints = new POINT16[cpts];

      for (int i=0; i<n; i++) {
	lpoints[i].x = points[i].x;
	lpoints[i].y = points[i].y;
      }

      rclBounds = *bounds;
    }
    /*!
     * Construct a PolyBezier record from the input stream.
     * \param ds Metafile datastream.
     */
    EMRPOLYBEZIER16 ( DATASTREAM& ds )
    {
      ds >> emr >> rclBounds >> cpts;

      lpoints = new POINT16[cpts];

      POINT16ARRAY points( lpoints, cpts );

      ds >> points;
    }
    /*!
     * Destructor frees a copy of the points it buffered.
     */
    ~EMRPOLYBEZIER16 ( )
    {
      if ( lpoints ) delete[] lpoints;
    }
    /*!
     * \param ds Metafile datastream.
     */
    bool serialize ( DATASTREAM ds )
    {
      ds << emr << rclBounds << cpts << POINT16ARRAY( lpoints, cpts );
      return true;
    }
    /*!
     * Internally computed size of this record.
     */
    int size ( void ) const { return emr.nSize; }
    /*!
     * Execute this record in the context of the given device context.
     * \param source the device context from which this record is taken.
     * \param dc device context for execute.
     */
    void execute ( METAFILEDEVICECONTEXT* /*source*/, HDC dc ) const
    {
      // According to the wine windef.h header, POINT and POINTL are equivalent
      PolyBezier16( dc, lpoints, cpts );
    }
#ifdef ENABLE_EDITING
    /*!
     * Print it to stdout.
     */
    void edit ( void ) const
    {
      printf( "*POLYBEZIER16*\n" );
      edit_rectl( "rclBounds", rclBounds );
      edit_point16array( "\t", cpts, lpoints );
    }
#endif /* ENABLE_EDITING */
  };

  //! EMF PolyBezierTo
  /*!
   * Draw a polygonal Bezier curve to (what?)
   */
  class EMRPOLYBEZIERTO : public METARECORD, ::EMRPOLYBEZIER {
    POINTL* lpoints;
  public:
    /*!
     * \param bounds overall bounding box of polybezier curve.
     * \param points array of polybezier vertices.
     * \param n number of vertices in points.
     */
    EMRPOLYBEZIERTO ( const RECTL* bounds, const POINT* points, INT n )
    {
      cptl = n;
      aptl[0].x = 0;		// Really unused
      aptl[0].y = 0;

      emr.iType = EMR_POLYBEZIERTO;
      // The (cptl-1) below is to account for aptl, which isn't written out
      emr.nSize = sizeof( ::EMRPOLYBEZIERTO ) + sizeof( POINTL ) * (cptl-1);

      lpoints = new POINTL[cptl];

      for (int i=0; i<n; i++) {
	lpoints[i].x = points[i].x;
	lpoints[i].y = points[i].y;
      }

      rclBounds = *bounds;
    }
    /*!
     * Construct a PolyBezier record from the input stream.
     * \param ds Metafile datastream.
     */
    EMRPOLYBEZIERTO ( DATASTREAM& ds )
    {
      ds >> emr >> rclBounds >> cptl;

      lpoints = new POINTL[cptl];

      POINTLARRAY points( lpoints, cptl );

      ds >> points;
    }
    /*!
     * Destructor frees a copy of the points it buffered.
     */
    ~EMRPOLYBEZIERTO ( )
    {
      if ( lpoints ) delete[] lpoints;
    }
    /*!
     * \param fp Metafile file handle.
     */
    bool serialize ( DATASTREAM ds )
    {
      ds << emr << rclBounds << cptl << POINTLARRAY( lpoints, cptl );
      return true;
    }
    /*!
     * Internally computed size of this record.
     */
    int size ( void ) const { return emr.nSize; }
    /*!
     * Execute this record in the context of the given device context.
     * \param source the device context from which this record is taken.
     * \param dc device context for execute.
     */
    void execute ( METAFILEDEVICECONTEXT* /*source*/, HDC dc ) const
    {
      // According to the wine windef.h header, POINT and POINTL are equivalent
      PolyBezierTo( dc, (POINT*)lpoints, cptl );
    }
#ifdef ENABLE_EDITING
    /*!
     * Print it to stdout.
     */
    void edit ( void ) const
    {
      printf( "*POLYBEZIERTO*\n" );
      edit_rectl( "rclBounds", rclBounds );
#if 0
      printf( "\tcptl              : %ld\n", cptl );
      printf( "\taptl->\n" );
      for ( unsigned int i = 0; i < cptl; i++ )
	printf( "\t\t%ld, %ld\n", lpoints[i].x, lpoints[i].y );
#else
      edit_pointlarray( "\t", cptl, lpoints );
#endif
    }
#endif /* ENABLE_EDITING */
  };

  //! EMF PolyBezierTo16
  /*!
   * Draw a polygonal Bezier curve to (what?) using 16-bit points
   */
  class EMRPOLYBEZIERTO16 : public METARECORD, ::EMRPOLYBEZIER16 {
    POINT16* lpoints;
  public:
    /*!
     * \param bounds overall bounding box of polybezier curve.
     * \param points array of polybezier vertices.
     * \param n number of vertices in points.
     */
    EMRPOLYBEZIERTO16 ( const RECTL* bounds, const POINT16* points, INT n )
    {
      cpts = n;
      apts[0].x = 0;		// Really unused
      apts[0].y = 0;

      emr.iType = EMR_POLYBEZIERTO16;
      // The (cptl-1) below is to account for aptl, which isn't written out
      emr.nSize = sizeof( ::EMRPOLYBEZIERTO16 ) + sizeof( POINT16 ) * (cpts-1);

      lpoints = new POINT16[cpts];

      for (int i=0; i<n; i++) {
	lpoints[i].x = points[i].x;
	lpoints[i].y = points[i].y;
      }

      rclBounds = *bounds;
    }
    /*!
     * Convenience constructor with POINTs.
     * \param bounds overall bounding box of polybezier curve.
     * \param points array of polybezier vertices.
     * \param n number of vertices in points.
     */
    EMRPOLYBEZIERTO16 ( const RECTL* bounds, const POINT* points, INT n )
    {
      cpts = n;
      apts[0].x = 0;		// Really unused
      apts[0].y = 0;

      emr.iType = EMR_POLYBEZIERTO16;
      // The (cptl-1) below is to account for aptl, which isn't written out
      emr.nSize = sizeof( ::EMRPOLYBEZIERTO16 ) + sizeof( POINT16 ) * (cpts-1);

      lpoints = new POINT16[cpts];

      for (int i=0; i<n; i++) {
	lpoints[i].x = points[i].x;
	lpoints[i].y = points[i].y;
      }

      rclBounds = *bounds;
    }
    /*!
     * Construct a PolyBezier record from the input stream.
     * \param ds Metafile datastream.
     */
    EMRPOLYBEZIERTO16 ( DATASTREAM& ds )
    {
      ds >> emr >> rclBounds >> cpts;

      lpoints = new POINT16[cpts];

      POINT16ARRAY points( lpoints, cpts );

      ds >> points;
    }
    /*!
     * Destructor frees a copy of the points it buffered.
     */
    ~EMRPOLYBEZIERTO16 ( )
    {
      if ( lpoints ) delete[] lpoints;
    }
    /*!
     * \param fp Metafile file handle.
     */
    bool serialize ( DATASTREAM ds )
    {
      ds << emr << rclBounds << cpts << POINT16ARRAY( lpoints, cpts );
      return true;
    }
    /*!
     * Internally computed size of this record.
     */
    int size ( void ) const { return emr.nSize; }
    /*!
     * Execute this record in the context of the given device context.
     * \param source the device context from which this record is taken.
     * \param dc device context for execute.
     */
    void execute ( METAFILEDEVICECONTEXT* /*source*/, HDC dc ) const
    {
      // According to the wine windef.h header, POINT and POINTL are equivalent
      PolyBezierTo16( dc, lpoints, cpts );
    }
#ifdef ENABLE_EDITING
    /*!
     * Print it to stdout.
     */
    void edit ( void ) const
    {
      printf( "*POLYBEZIERTO16*\n" );
      edit_rectl( "rclBounds", rclBounds );
      edit_point16array( "\t", cpts, lpoints );
    }
#endif /* ENABLE_EDITING */
  };

  //! EMF PolylineTo
  /*!
   * Draw a polygonal line curve to (what?)
   */
  class EMRPOLYLINETO : public METARECORD, ::EMRPOLYLINETO {
    POINTL* lpoints;
  public:
    /*!
     * \param bounds overall bounding box of polybezier curve.
     * \param points array of polybezier vertices.
     * \param n number of vertices in points.
     */
    EMRPOLYLINETO ( const RECTL* bounds, const POINT* points, INT n )
    {
      cptl = n;
      aptl[0].x = 0;
      aptl[0].y = 0;

      emr.iType = EMR_POLYLINETO;
      // The (cptl-1) below is to account for aptl, which isn't written out
      emr.nSize = sizeof( ::EMRPOLYLINETO ) + sizeof( POINTL ) * (cptl-1);

      lpoints = new POINTL[cptl];

      for (int i=0; i<n; i++) {
	lpoints[i].x = points[i].x;
	lpoints[i].y = points[i].y;
      }

      rclBounds = *bounds;
    }
    /*!
     * Construct a PolylineTo record from the input stream.
     * \param ds Metafile datastream.
     */
    EMRPOLYLINETO ( DATASTREAM& ds )
    {
      ds >> emr >> rclBounds >> cptl;

      lpoints = new POINTL[cptl];

      POINTLARRAY points( lpoints, cptl );

      ds >> points;
    }
    /*!
     * Destructor frees a copy of the points it buffered.
     */
    ~EMRPOLYLINETO ( )
    {
      if ( lpoints ) delete[] lpoints;
    }
    /*!
     * \param fp Metafile file handle.
     */
    bool serialize ( DATASTREAM ds )
    {
      ds << emr << rclBounds << cptl << POINTLARRAY( lpoints, cptl );
      return true;
    }
    /*!
     * Internally computed size of this record.
     */
    int size ( void ) const { return emr.nSize; }
    /*!
     * Execute this record in the context of the given device context.
     * \param source the device context from which this record is taken.
     * \param dc device context for execute.
     */
    void execute ( METAFILEDEVICECONTEXT* /*source*/, HDC dc ) const
    {
      // According to the wine windef.h header, POINT and POINTL are equivalent
      PolylineTo( dc, (POINT*)lpoints, cptl );
    }
#ifdef ENABLE_EDITING
    /*!
     * Print it to stdout.
     */
    void edit ( void ) const
    {
      printf( "*POLYLINETO*\n" );
      edit_rectl( "rclBounds", rclBounds );
#if 0
      printf( "\tcptl              : %ld\n", cptl );
      printf( "\taptl->\n" );
      for ( unsigned int i = 0; i < cptl; i++ )
	printf( "\t\t%ld, %ld\n", lpoints[i].x, lpoints[i].y );
#else
      edit_pointlarray( "\t", cptl, lpoints );
#endif
    }
#endif /* ENABLE_EDITING */
  };

  //! EMF PolylineTo16
  /*!
   * Draw a polygonal line curve to (what?)
   */
  class EMRPOLYLINETO16 : public METARECORD, ::EMRPOLYLINETO16 {
    POINT16* lpoints;
  public:
    /*!
     * \param bounds overall bounding box of polybezier curve.
     * \param points array of polybezier vertices.
     * \param n number of vertices in points.
     */
    EMRPOLYLINETO16 ( const RECTL* bounds, const POINT16* points, INT n )
    {
      cpts = n;
      apts[0].x = 0;
      apts[0].y = 0;

      emr.iType = EMR_POLYLINETO16;
      // The (cptl-1) below is to account for aptl, which isn't written out
      emr.nSize = sizeof( ::EMRPOLYLINETO16 ) + sizeof( POINT16 ) * (cpts-1);

      lpoints = new POINT16[cpts];

      for (int i=0; i<n; i++) {
	lpoints[i].x = points[i].x;
	lpoints[i].y = points[i].y;
      }

      rclBounds = *bounds;
    }
    /*!
     * Convenience constructor with POINTs.
     * \param bounds overall bounding box of polybezier curve.
     * \param points array of polybezier vertices.
     * \param n number of vertices in points.
     */
    EMRPOLYLINETO16 ( const RECTL* bounds, const POINT* points, INT n )
    {
      cpts = n;
      apts[0].x = 0;
      apts[0].y = 0;

      emr.iType = EMR_POLYLINETO16;
      // The (cptl-1) below is to account for aptl, which isn't written out
      emr.nSize = sizeof( ::EMRPOLYLINETO16 ) + sizeof( POINT16 ) * (cpts-1);

      lpoints = new POINT16[cpts];

      for (int i=0; i<n; i++) {
	lpoints[i].x = points[i].x;
	lpoints[i].y = points[i].y;
      }

      rclBounds = *bounds;
    }
    /*!
     * Construct a PolylineTo record from the input stream.
     * \param ds Metafile datastream.
     */
    EMRPOLYLINETO16 ( DATASTREAM& ds )
    {
      ds >> emr >> rclBounds >> cpts;

      lpoints = new POINT16[cpts];

      POINT16ARRAY points( lpoints, cpts );

      ds >> points;
    }
    /*!
     * Destructor frees a copy of the points it buffered.
     */
    ~EMRPOLYLINETO16 ( )
    {
      if ( lpoints ) delete[] lpoints;
    }
    /*!
     * \param fp Metafile file handle.
     */
    bool serialize ( DATASTREAM ds )
    {
      ds << emr << rclBounds << cpts << POINT16ARRAY( lpoints, cpts );
      return true;
    }
    /*!
     * Internally computed size of this record.
     */
    int size ( void ) const { return emr.nSize; }
    /*!
     * Execute this record in the context of the given device context.
     * \param source the device context from which this record is taken.
     * \param dc device context for execute.
     */
    void execute ( METAFILEDEVICECONTEXT* /*source*/, HDC dc ) const
    {
      // According to the wine windef.h header, POINT and POINTL are equivalent
      PolylineTo16( dc, lpoints, cpts );
    }
#ifdef ENABLE_EDITING
    /*!
     * Print it to stdout.
     */
    void edit ( void ) const
    {
      printf( "*POLYLINETO16*\n" );
      edit_rectl( "rclBounds", rclBounds );
      edit_point16array( "\t", cpts, lpoints );
    }
#endif /* ENABLE_EDITING */
  };

  //! EMF Extended Text Output ASCII
  /*!
   * Draw this text string with the current font, in the color of the current
   * pen and with the given text background color. Individual character positioning
   * can be given in the dx array.
   */
  class EMREXTTEXTOUTA : public METARECORD, ::EMREXTTEXTOUTA {
    PSTR string_a;
    int string_size;

    INT* dx_i;
  public:
    /*!
     * \param bounds bounding box of text string.
     * \param graphicsMode (not entirely sure?)
     * \param xScale width scale factor (of what?)
     * \param yScale height scale factor (of what?)
     * \param text a text metarecord containing the rendering style.
     * \param string the text to render
     * \param dx an array of positions for each character in string.
     */
    EMREXTTEXTOUTA ( const RECTL* bounds, DWORD graphicsMode, FLOAT xScale,
		     FLOAT yScale, const PEMRTEXT text, LPCSTR string,
		     const INT* dx )
    {
      emr.iType = EMR_EXTTEXTOUTA;
      emr.nSize = sizeof( ::EMREXTTEXTOUTA );

      rclBounds = *bounds;

      iGraphicsMode = graphicsMode;
      exScale = xScale;
      eyScale = yScale;

      emrtext = *text;

      string_size = ROUND_TO_LONG( emrtext.nChars );

      string_a = new CHAR[ string_size ];

      memset( string_a, 0, sizeof(CHAR) * string_size );

      for ( unsigned int i=0; i<emrtext.nChars; i++ )
	string_a[i] = *string++;

      emrtext.offString = emr.nSize;
      emr.nSize += string_size * sizeof(CHAR);

      if ( dx ) {

	dx_i = new INT[ emrtext.nChars ];

	for ( unsigned int i=0; i<emrtext.nChars; i++ )
	  dx_i[i] = *dx++;

	emrtext.offDx = emr.nSize;
	emr.nSize += emrtext.nChars * sizeof(INT);
      }
      else {
	emrtext.offDx = 0;
	dx_i = 0;
      }
    }
    /*!
     * Construct a ExtTextOutA record from the input stream.
     * \param ds Metafile datastream.
     */
    EMREXTTEXTOUTA ( DATASTREAM& ds )
    {
      ds >> emr >> rclBounds >> iGraphicsMode >> exScale >> eyScale >> emrtext;

      if ( emrtext.offString != 0 ) {
	string_size = ROUND_TO_LONG( emrtext.nChars );

	string_a = new CHAR[ string_size ];

	memset( string_a, 0, sizeof(CHAR) * string_size );

	CHARSTR string( string_a, string_size );

	ds >> string;
      }
      else
	string_a = 0;

      if ( emrtext.offDx ) {
	dx_i = new INT[ emrtext.nChars ];

	INTARRAY dx_is( dx_i, emrtext.nChars );

	ds >> dx_is;
      }
      else
	dx_i = 0;
    }
    /*!
     * Destructor frees its copy of the string and its character
     * offset array
     */
    ~EMREXTTEXTOUTA ( )
    {
      if ( string_a ) delete[] string_a;
      if ( dx_i ) delete[] dx_i;
    }
    /*!
     * \param fp Metafile file handle.
     */
    bool serialize ( DATASTREAM ds )
    {
      ds << emr << rclBounds << iGraphicsMode << exScale << eyScale
	 << emrtext << CHARSTR( string_a, string_size );
      if ( dx_i )
	ds << INTARRAY( dx_i, emrtext.nChars );
      return true;
    }
    /*!
     * Internally computed size of this record.
     */
    int size ( void ) const { return emr.nSize; }
    /*!
     * Execute this record in the context of the given device context.
     * \param source the device context from which this record is taken.
     * \param dc device context for execute.
     */
    void execute ( METAFILEDEVICECONTEXT* /*source*/, HDC dc ) const
    {
      RECT rect;
      rect.left = emrtext.rcl.left;
      rect.top = emrtext.rcl.top;
      rect.right = emrtext.rcl.right;
      rect.bottom = emrtext.rcl.bottom;

      ExtTextOutA( dc, emrtext.ptlReference.x, emrtext.ptlReference.y,
		   emrtext.fOptions, &rect, string_a, emrtext.nChars,
		   dx_i );
    }
#ifdef ENABLE_EDITING
    /*!
     * Print it to stdout.
     */
    void edit ( void ) const
    {
      printf( "*EXTTEXTOUTA*\n" );
      edit_rectl( "rclBounds", rclBounds );
      printf( "\tiGraphicsMode\t: " );
      switch ( iGraphicsMode ) {
      case GM_COMPATIBLE: printf( "GM_COMPATIBLE\n" ); break;
      case GM_ADVANCED: printf( "GM_ADVANCED\n" ); break;
      default: printf( "unknown(%ld)\n", iGraphicsMode );
      }
      printf( "\texScale\t\t: %f\n", exScale );
      printf( "\teyScale\t\t: %f\n", eyScale );
      printf( "\tptlReference\t: (%ld,%ld)\n", emrtext.ptlReference.x,
	      emrtext.ptlReference.y );
      printf( "\tnChars\t\t: %ld\n", emrtext.nChars );
      printf( "\toffString\t: %ld\n", emrtext.offString );
      printf( "\tfOptions\t: " );
      if ( emrtext.fOptions == 0 )
	printf( "None" );
      else {
	if ( emrtext.fOptions & ETO_GRAYED ) {
	  printf( "ETO_GRAYED" );
	  if ( emrtext.fOptions & ~ETO_GRAYED )
	    printf( " | " );
	}
	if ( emrtext.fOptions & ETO_OPAQUE ) {
	  printf( "ETO_OPAQUE" );
	  if ( emrtext.fOptions & ~(ETO_GRAYED | ETO_OPAQUE) )
	    printf( " | " );
	}
	if ( emrtext.fOptions & ETO_CLIPPED ) {
	  printf( "ETO_CLIPPED" );
	  if ( emrtext.fOptions & ~(ETO_GRAYED | ETO_OPAQUE | ETO_CLIPPED ) )
	    printf( " | " );
	}
	if ( emrtext.fOptions & ETO_GLYPH_INDEX ) {
	  printf( "ETO_GLYPH_INDEX" );
	  if ( emrtext.fOptions &
	       ~(ETO_GRAYED | ETO_OPAQUE | ETO_CLIPPED | ETO_GLYPH_INDEX) )
	    printf( " | " );
	}
	if ( emrtext.fOptions & ETO_RTLREADING ) {
	  printf( "ETO_RTLREADING" );
	  if ( emrtext.fOptions &
	       ~(ETO_GRAYED | ETO_OPAQUE | ETO_CLIPPED | ETO_GLYPH_INDEX |
		 ETO_RTLREADING) )
	    printf( " | " );
	}
	if ( emrtext.fOptions & ETO_IGNORELANGUAGE )
	  printf( "ETO_IGNORELANGUAGE" );
      }
      printf( "\n" );
      edit_rectl( "rcl\t", emrtext.rcl );
      printf( "\toffDx\t\t: %ld\n", emrtext.offDx );
      printf( "\tString:\n\t\t%s\n",  string_a );

      if ( emrtext.offDx != 0 ) {
	printf( "\tOffsets:\n\t\t" );
	for ( unsigned int i = 0; i < emrtext.nChars; i++ )
	  printf( "%d ", dx_i[i] );
	printf( "\n" );
      }
    }
#endif /* ENABLE_EDITING */
  };

  //! EMF Set Pixel
  /*!
   * Set the given pixel to the given color.
   */
  class EMRSETPIXELV : public METARECORD, ::EMRSETPIXELV {
  public:
    /*!
     * \param x x position at which to draw pixel.
     * \param y y position at which to draw pixel.
     * \param color color of pixel.
     */
    EMRSETPIXELV ( INT x, INT y, COLORREF color )
    {
      emr.iType = EMR_SETPIXELV;
      emr.nSize = sizeof( ::EMRSETPIXELV );
      ptlPixel.x = x;
      ptlPixel.y = y;
      crColor = color;
    }
    /*!
     * Construct a SetPixelV record from the input stream.
     * \param ds Metafile datastream.
     */
    EMRSETPIXELV ( DATASTREAM& ds )
    {
      ds >> emr >> ptlPixel >> crColor;
    }
    /*!
     * \param ds Metafile datastream.
     */
    bool serialize ( DATASTREAM ds )
    {
      ds << emr << ptlPixel << crColor;
      return true;
    }
    /*!
     * Internally computed size of this record.
     */
    int size ( void ) const { return emr.nSize; }
    /*!
     * Execute this record in the context of the given device context.
     * \param source the device context from which this record is taken.
     * \param dc device context for execute.
     */
    void execute ( METAFILEDEVICECONTEXT* /*source*/, HDC dc ) const
    {
      SetPixel( dc, ptlPixel.x, ptlPixel.y, crColor );
    }
#ifdef ENABLE_EDITING
    /*!
     * Print it to stdout.
     */
    void edit ( void ) const
    {
      printf( "*SETPIXELV*\n" );
      edit_pointl( "ptlPixel", ptlPixel );
      edit_color( "crColor\t", crColor );
    }
#endif /* ENABLE_EDITING */
  };

  class PEN;
  class EXTPEN;
  class BRUSH;
  class FONT;
  class PALETTE;

  //! EMF Pen
  /*!
   * Create a new pen (used for drawing lines, arcs, rectangles, etc.).
   */
  class EMRCREATEPEN : public METARECORD, public ::EMRCREATEPEN
  {
  public:
    /*!
     * \param pen an instance of a PEN object.
     * \param handle the PEN object's handle.
     */
    EMRCREATEPEN ( PEN* pen, HGDIOBJ handle );
    /*!
     * Construct a CreatePen record from the input stream.
     * \param ds Metafile datastream.
     */
    EMRCREATEPEN ( DATASTREAM& ds );
    /*!
     * \param fp Metafile file handle.
     */
    bool serialize ( DATASTREAM ds )
    {
      ds << emr << ihPen << lopn;
      return true;
    }
    /*!
     * Internally computed size of this record.
     */
    int size ( void ) const { return emr.nSize; }
    /*!
     * Execute this record in the context of the given device context.
     * \param source the device context from which this record is taken.
     * \param dc device context for execute.
     */
    void execute ( METAFILEDEVICECONTEXT* source, HDC dc ) const;
#ifdef ENABLE_EDITING
    /*!
     * Print it to stdout.
     */
    void edit ( void ) const
    {
      printf( "*CREATEPEN*\n" );
      printf( "\tihPen\t\t: 0x%lx\n", ihPen );
      edit_pen_style( "lopn.lopnStyle", lopn.lopnStyle );
      printf( "\tlopn.lopnWidth\t: %ld, %ld\n", lopn.lopnWidth.x, lopn.lopnWidth.y );
      edit_color( "lopn.lopnColor", lopn.lopnColor );
    }
#endif /* ENABLE_EDITING */
  };

  //! EMF Extended Pen
  /*!
   * Create a new pen (used for drawing lines, arcs, rectangles, etc.).
   * Apparently uses extended attributes such as a bitmap mask.
   */
  class EMREXTCREATEPEN : public METARECORD, public ::EMREXTCREATEPEN
  {
  public:
    /*!
     * \param pen an instance of a PEN object.
     * \param handle the PEN object's handle.
     */
    EMREXTCREATEPEN ( EXTPEN* pen, HGDIOBJ handle );
    /*!
     * Construct a ExtCreatePen record from the input stream.
     * \param ds Metafile datastream.
     */
    EMREXTCREATEPEN ( DATASTREAM& ds );
    /*!
     * \param ds Metafile datastream.
     */
    bool serialize ( DATASTREAM ds )
    {
      ds << emr << ihPen << offBmi << cbBmi << offBits << cbBits << elp;
      return true;
    }
    /*!
     * Internally computed size of this record.
     */
    int size ( void ) const { return emr.nSize; }
    /*!
     * Execute this record in the context of the given device context.
     * \param source the device context from which this record is taken.
     * \param dc device context for execute.
     */
    void execute ( METAFILEDEVICECONTEXT* source, HDC dc ) const;
#ifdef ENABLE_EDITING
    /*!
     * Print it to stdout.
     */
    void edit ( void ) const
    {
      printf( "*EXTCREATEPEN*\n" );
      printf( "\tihPen\t\t\t: 0x%lx\n", ihPen );
      printf( "\toffBmi\t\t\t: %ld\n",  offBmi );
      printf( "\tcbBmi\t\t\t: %ld\n", cbBmi );
      printf( "\toffBits\t\t\t: %ld\n", offBits );
      printf( "\tcbBits\t\t\t: %ld\n", cbBits );
      edit_pen_style( "elp.elpPenStyle\t", elp.elpPenStyle );
      printf( "\telp.elpWidth\t\t: %ld\n", elp.elpWidth );
      edit_brush_style( "elp.elpBrushStyle", elp.elpBrushStyle );
      edit_color( "elp.elpColor\t", elp.elpColor );
      edit_brush_hatch( "elp.elpHatch\t", elp.elpHatch );
      printf( "\telp.elpNumEntries\t: %ld\n", elp.elpNumEntries );
    }
#endif /* ENABLE_EDITING */
  };

  //! EMF Brush
  /*!
   * Create a new brush (used for filling shapes).
   */
  class EMRCREATEBRUSHINDIRECT : public METARECORD, public ::EMRCREATEBRUSHINDIRECT
  {
  public:
    /*!
     * \param brush an instance of a BRUSH object.
     * \param handle the BRUSH object's handle.
     */
    EMRCREATEBRUSHINDIRECT ( BRUSH* brush, HGDIOBJ handle );
    /*!
     * Create a CreateBrushIndirect record from the input stream.
     * \param ds Metafile datastream.
     */
    EMRCREATEBRUSHINDIRECT ( DATASTREAM& ds );
    /*!
     * \param ds Metafile datastream.
     */
    bool serialize ( DATASTREAM ds )
    {
      ds << emr << ihBrush << lb;
      return true;
    }
    /*!
     * Internally computed size of this record.
     */
    int size ( void ) const { return emr.nSize; }
    /*!
     * Execute this record in the context of the given device context.
     * \param source the device context from which this record is taken.
     * \param dc device context for execute.
     */
    void execute ( METAFILEDEVICECONTEXT* source, HDC dc ) const;
#ifdef ENABLE_EDITING
    /*!
     * Print it to stdout.
     */
    void edit ( void ) const
    {
      printf( "*CREATEBRUSHINDIRECT*\n" );
      printf( "\tihBrush\t\t: 0x%lx\n", ihBrush );
      edit_brush_style( "lb.lbStyle", lb.lbStyle );
      edit_color( "lb.lbColor", lb.lbColor );
      edit_brush_hatch( "lb.lbHatch", lb.lbHatch );
    }
#endif /* ENABLE_EDITING */
  };

  //! EMF Font
  /*!
   * Create a new font.
   */
  class EMREXTCREATEFONTINDIRECTW : public METARECORD, public ::EMREXTCREATEFONTINDIRECTW
  {
  public:
    /*!
     * \param font an instance of a FONT object.
     * \param handle the FONT object's handle.
     */
    EMREXTCREATEFONTINDIRECTW ( FONT* font, HGDIOBJ handle );
    /*!
     * Construct a CreateFontIndirectW record from the input stream.
     * \param ds Metafile datastream.
     */
    EMREXTCREATEFONTINDIRECTW ( DATASTREAM& ds );
    /*!
     * \param ds Metafile datastream.
     */
    bool serialize ( DATASTREAM ds )
    {
      // Since EMF records have to be multiples of 4 bytes, this
      // should perhaps be a general thing, but we know it's currently
      // only a problem for this structure.

      ds << emr << ihFont << elfw << PADDING( 2 );

      return true;
    }
    /*!
     * \param fp Metafile file handle.
     */
    int size ( void ) const { return emr.nSize; }
    /*!
     * Execute this record in the context of the given device context.
     * \param source the device context from which this record is taken.
     * \param dc device context for execute.
     */
    void execute ( METAFILEDEVICECONTEXT* source, HDC dc ) const;
#ifdef ENABLE_EDITING
    /*!
     * Print it to stdout.
     */
    void edit ( void ) const
    {
      printf( "*EXTCREATEFONTINDIRECTW*\n" );
      printf( "\tihFont\t\t\t: %ld\n", ihFont );
      printf( "\tlfHeight\t\t: %ld\n", elfw.elfLogFont.lfHeight );
      printf( "\tlfWidth\t\t\t: %ld\n", elfw.elfLogFont.lfWidth );
      printf( "\tlfEscapement\t\t: %ld\n", elfw.elfLogFont.lfEscapement );
      printf( "\tlfOrientation\t\t: %ld\n", elfw.elfLogFont.lfOrientation );
      printf( "\tlfWeight\t\t: " );
      switch ( elfw.elfLogFont.lfWeight ) {
      case FW_DONTCARE: printf( "FW_DONTCARE\n" ); break;
      case FW_THIN: printf( "FW_THIN\n" ); break;
      case FW_EXTRALIGHT: printf( "FW_EXTRALIGHT\n" ); break;
      case FW_LIGHT: printf( "FW_LIGHT\n" ); break;
      case FW_NORMAL: printf( "FW_NORMAL\n" ); break;
      case FW_MEDIUM: printf( "FW_MEDIUM\n" ); break;
      case FW_SEMIBOLD: printf( "FW_SEMIBOLD\n" ); break;
      case FW_BOLD: printf( "FW_BOLD\n" ); break;
      case FW_EXTRABOLD: printf( "FW_EXTRABOLD\n" ); break;
      case FW_BLACK: printf( "FW_BLACK\n" ); break;
      }
      printf( "\tlfItalic\t\t: %d\n", elfw.elfLogFont.lfItalic );
      printf( "\tlfUnderline\t\t: %d\n", elfw.elfLogFont.lfUnderline );
      printf( "\tlfStrikeOut\t\t: %d\n", elfw.elfLogFont.lfStrikeOut );
      printf( "\tlfCharSet\t\t: %d\n", elfw.elfLogFont.lfCharSet );
      printf( "\tlfOutPrecision\t\t: %d\n", elfw.elfLogFont.lfOutPrecision );
      printf( "\tlfClipPrecision\t\t: %d\n", elfw.elfLogFont.lfClipPrecision );
      printf( "\tlfQuality\t\t: %d\n", elfw.elfLogFont.lfQuality );
      printf( "\tlfPitchAndFamily\t: %d\n", elfw.elfLogFont.lfPitchAndFamily );
      int i = 0;
      printf( "\tlfFaceName\t\t: '" );
      while ( elfw.elfLogFont.lfFaceName[i] != 0 && i < LF_FACESIZE ) {
	putchar( elfw.elfLogFont.lfFaceName[i] );
	i++;
      }
      puts( "'" );

      i = 0;
      printf( "\telfFullName\t\t: '" );
      while ( elfw.elfFullName[i] != 0 && i < LF_FULLFACESIZE ) {
	putchar( elfw.elfFullName[i] );
	i++;
      }
      puts( "'" );

      i = 0;
      printf( "\telfStyle\t\t: '" );
      while ( elfw.elfStyle[i] != 0 && i < LF_FACESIZE ) {
	putchar( elfw.elfStyle[i] );
	i++;
      }
      puts( "'" );

      printf( "\telfVersion\t\t: %ld\n", elfw.elfVersion );
      printf( "\telfStyleSize\t\t: %ld\n", elfw.elfStyleSize );
      printf( "\telfMatch\t\t: %ld\n", elfw.elfMatch );
      printf( "\telfVendorId\t\t: '%s'\n", elfw.elfVendorId );
      printf( "\telfCulture\t\t: %ld\n", elfw.elfCulture );
      printf( "\telfPanose\t\t:\n" );
      printf( "\t\tbFamilyType\t\t: %d\n", elfw.elfPanose.bFamilyType );
      printf( "\t\tbSerifStyle\t\t: %d\n", elfw.elfPanose.bSerifStyle );
      printf( "\t\tbWeight\t\t\t: %d\n", elfw.elfPanose.bWeight );
      printf( "\t\tbProportion\t\t: %d\n", elfw.elfPanose.bProportion );
      printf( "\t\tbContrast\t\t: %d\n", elfw.elfPanose.bContrast );
      printf( "\t\tbStrokeVariation\t: %d\n", elfw.elfPanose.bStrokeVariation );
      printf( "\t\tbArmStyle\t\t: %d\n", elfw.elfPanose.bArmStyle );
      printf( "\t\tbLetterform\t\t: %d\n", elfw.elfPanose.bLetterform );
      printf( "\t\tbMidline\t\t: %d\n", elfw.elfPanose.bMidline );
      printf( "\t\tbXHeight\t\t: %d\n", elfw.elfPanose.bXHeight );
    }
#endif /* ENABLE_EDITING */
  };

  //! EMF Palette
  /*!
   * Create a new palette.
   */
  class EMRCREATEPALETTE : public METARECORD, public ::EMRCREATEPALETTE
  {
  public:
    /*!
     * \param palette an instance of a PALETTE object.
     * \param handle the PALETTE object's handle.
     */
    EMRCREATEPALETTE ( PALETTE* palette, HGDIOBJ handle );
    /*!
     * Construct a CreatePalette record from the input stream.
     * \param ds Metafile datastream.
     */
    EMRCREATEPALETTE ( DATASTREAM& ds );
    /*!
     * \param fp Metafile file handle.
     */
    bool serialize ( DATASTREAM ds )
    {
      ds << emr << ihPal << lgpl;
      return true;
    }
    /*!
     * Internally computed size of this record.
     */
    int size ( void ) const { return emr.nSize; }
    /*!
     * Execute this record in the context of the given device context.
     * \param source the device context from which this record is taken.
     * \param dc device context for execute.
     */
    void execute ( METAFILEDEVICECONTEXT* source, HDC dc ) const;
#ifdef ENABLE_EDITING
    /*!
     * Print it to stdout.
     */
    void edit ( void ) const
    {
      printf( "*CREATEPALETTE* (not really handled by libEMF)\n" );
    }
#endif /* ENABLE_EDITING */
  };

  //! EMF Fill path
  /*!
   * Fill the path.
   */
  class EMRFILLPATH : public METARECORD, ::EMRFILLPATH {
  public:
    /*!
     * \param bounds overall bounding box of polygon.
     * \param n number of vertices in points.
     */
    EMRFILLPATH ( const RECTL* bounds )
    {
      emr.iType = EMR_FILLPATH;
      emr.nSize = sizeof( ::EMRFILLPATH );
      rclBounds = *bounds;
    }
    /*!
     * Create a FillPath record from input stream.
     * \param ds Metafile datastream.
     */
    EMRFILLPATH ( DATASTREAM& ds )
    {
      ds >> emr >> rclBounds;
    }
    /*!
     * \param ds Metafile datastream.
     */
    bool serialize ( DATASTREAM ds )
    {
      ds << emr << rclBounds;
      return true;
    }
    /*!
     * Internally computed size of this record.
     */
    int size ( void ) const { return emr.nSize; }
    /*!
     * Execute this record in the context of the given device context.
     * \param source the device context from which this record is taken.
     * \param dc device context for execute.
     */
    void execute ( METAFILEDEVICECONTEXT* /*source*/, HDC dc ) const
    {
      FillPath( dc );
    }
#ifdef ENABLE_EDITING
    /*!
     * Print it to stdout.
     */
    void edit ( void ) const
    {
      printf( "*FILLPATH*\n" );
      edit_rectl( "rclBounds", rclBounds );
    }
#endif /* ENABLE_EDITING */
  };
  //! EMF Stroke path
  /*!
   * Stroke the path.
   */
  class EMRSTROKEPATH : public METARECORD, ::EMRSTROKEPATH {
  public:
    /*!
     * \param bounds overall bounding box of polygon.
     * \param n number of vertices in points.
     */
    EMRSTROKEPATH ( const RECTL* bounds )
    {
      emr.iType = EMR_STROKEPATH;
      emr.nSize = sizeof( ::EMRSTROKEPATH );
      rclBounds = *bounds;
    }
    /*!
     * Create a StrokePath record from input stream.
     * \param ds Metafile datastream.
     */
    EMRSTROKEPATH ( DATASTREAM& ds )
    {
      ds >> emr >> rclBounds;
    }
    /*!
     * \param fp Metafile file handle.
     */
    bool serialize ( DATASTREAM ds )
    {
      ds << emr << rclBounds;
      return true;
    }
    /*!
     * Internally computed size of this record.
     */
    int size ( void ) const { return emr.nSize; }
    /*!
     * Execute this record in the context of the given device context.
     * \param source the device context from which this record is taken.
     * \param dc device context for execute.
     */
    void execute ( METAFILEDEVICECONTEXT* /*source*/, HDC dc ) const
    {
      StrokePath( dc );
    }
#ifdef ENABLE_EDITING
    /*!
     * Print it to stdout.
     */
    void edit ( void ) const
    {
      printf( "*STROKEPATH*\n" );
      edit_rectl( "rclBounds", rclBounds );
    }
#endif /* ENABLE_EDITING */
  };
  //! EMF Stroke and Fill path
  /*!
   * Stroke and Fill the path.
   */
  class EMRSTROKEANDFILLPATH : public METARECORD, ::EMRSTROKEANDFILLPATH {
  public:
    /*!
     * \param bounds overall bounding box of polygon.
     * \param n number of vertices in points.
     */
    EMRSTROKEANDFILLPATH ( const RECTL* bounds )
    {
      emr.iType = EMR_STROKEANDFILLPATH;
      emr.nSize = sizeof( ::EMRSTROKEANDFILLPATH );
      rclBounds = *bounds;
    }
    /*!
     * Create a StrokeandfillPath record from input stream.
     * \param ds Metafile datastream.
     */
    EMRSTROKEANDFILLPATH ( DATASTREAM& ds )
    {
      ds >> emr >> rclBounds;
    }
    /*!
     * \param fp Metafile file handle.
     */
    bool serialize ( DATASTREAM ds )
    {
      ds << emr << rclBounds;
      return true;
    }
    /*!
     * Internally computed size of this record.
     */
    int size ( void ) const { return emr.nSize; }
    /*!
     * Execute this record in the context of the given device context.
     * \param source the device context from which this record is taken.
     * \param dc device context for execute.
     */
    void execute ( METAFILEDEVICECONTEXT* /*source*/, HDC dc ) const
    {
      StrokeAndFillPath( dc );
    }
#ifdef ENABLE_EDITING
    /*!
     * Print it to stdout.
     */
    void edit ( void ) const
    {
      printf( "*STROKEANDFILLPATH*\n" );
      edit_rectl( "rclBounds", rclBounds );
    }
#endif /* ENABLE_EDITING */
  };
  //! EMF Begin Path
  /*!
   * Begin the current path definition.
   */
  class EMRBEGINPATH : public METARECORD, ::EMRBEGINPATH {
  public:
    /*!
     * Create a Begin Path record.
     */
    EMRBEGINPATH ( void )
    {
      emr.iType = EMR_BEGINPATH;
      emr.nSize = sizeof( ::EMRBEGINPATH );
    }
    /*!
     * Construct a BeginPath record from the input stream.
     * \param ds Metafile datastream.
     */
    EMRBEGINPATH ( DATASTREAM& ds )
    {
      ds >> emr;
    }
    /*!
     * \param fp Metafile file handle.
     */
    bool serialize ( DATASTREAM ds )
    {
      ds << emr;
      return true;
    }
    /*!
     * Internally computed size of this record.
     */
    int size ( void ) const { return emr.nSize; }
    /*!
     * Execute this record in the context of the given device context.
     * \param source the device context from which this record is taken.
     * \param dc device context for execute.
     */
    void execute ( METAFILEDEVICECONTEXT* /*source*/, HDC dc ) const
    {
      BeginPath( dc );
    }
#ifdef ENABLE_EDITING
    /*!
     * Print it to stdout.
     */
    void edit ( void ) const
    {
      printf( "*BEGINPATH*\n" );
    }
#endif /* ENABLE_EDITING */
  };
  //! EMF End Path
  /*!
   * End the current path definition.
   */
  class EMRENDPATH : public METARECORD, ::EMRENDPATH {
  public:
    /*!
     * Create an End Path record.
     */
    EMRENDPATH ( void )
    {
      emr.iType = EMR_ENDPATH;
      emr.nSize = sizeof( ::EMRENDPATH );
    }
    /*!
     * Construct an EndPath record from the input stream.
     * \param ds Metafile datastream.
     */
    EMRENDPATH ( DATASTREAM& ds )
    {
      ds >> emr;
    }
    /*!
     * \param fp Metafile file handle.
     */
    bool serialize ( DATASTREAM ds )
    {
      ds << emr;
      return true;
    }
    /*!
     * Internally computed size of this record.
     */
    int size ( void ) const { return emr.nSize; }
    /*!
     * Execute this record in the context of the given device context.
     * \param source the device context from which this record is taken.
     * \param dc device context for execute.
     */
    void execute ( METAFILEDEVICECONTEXT* /*source*/, HDC dc ) const
    {
      EndPath( dc );
    }
#ifdef ENABLE_EDITING
    /*!
     * Print it to stdout.
     */
    void edit ( void ) const
    {
      printf( "*ENDPATH*\n" );
    }
#endif /* ENABLE_EDITING */
  };
  //! EMF Close Figure
  /*!
   * Close the current figure.
   */
  class EMRCLOSEFIGURE : public METARECORD, ::EMRCLOSEFIGURE {
  public:
    /*!
     * Create a Close Figure record.
     */
    EMRCLOSEFIGURE ( void )
    {
      emr.iType = EMR_CLOSEFIGURE;
      emr.nSize = sizeof( ::EMRCLOSEFIGURE );
    }
    /*!
     * Construct a CloseFigure record from the input stream.
     * \param ds Metafile datastream.
     */
    EMRCLOSEFIGURE ( DATASTREAM& ds )
    {
      ds >> emr;
    }
    /*!
     * \param fp Metafile file handle.
     */
    bool serialize ( DATASTREAM ds )
    {
      ds << emr;
      return true;
    }
    /*!
     * Internally computed size of this record.
     */
    int size ( void ) const { return emr.nSize; }
    /*!
     * Execute this record in the context of the given device context.
     * \param source the device context from which this record is taken.
     * \param dc device context for execute.
     */
    void execute ( METAFILEDEVICECONTEXT* /*source*/, HDC dc ) const
    {
      CloseFigure( dc );
    }
#ifdef ENABLE_EDITING
    /*!
     * Print it to stdout.
     */
    void edit ( void ) const
    {
      printf( "*CLOSEFIGURE*\n" );
    }
#endif /* ENABLE_EDITING */
  };
  //! EMF Save DC
  /*!
   * Save the device context (i.e., push contents on a stack of
   * some variety?)
   */
  class EMRSAVEDC : public METARECORD, ::EMRSAVEDC {
  public:
    /*!
     * Create a Save DC record.
     */
    EMRSAVEDC ( void )
    {
      emr.iType = EMR_SAVEDC;
      emr.nSize = sizeof( ::EMRSAVEDC );
    }
    /*!
     * Construct an Savedc record from the input stream.
     * \param ds Metafile datastream.
     */
    EMRSAVEDC ( DATASTREAM& ds )
    {
      ds >> emr;
    }
    /*!
     * \param fp Metafile file handle.
     */
    bool serialize ( DATASTREAM ds )
    {
      ds << emr;
      return true;
    }
    /*!
     * Internally computed size of this record.
     */
    int size ( void ) const { return emr.nSize; }
    /*!
     * Execute this record in the context of the given device context.
     * \param source the device context from which this record is taken.
     * \param dc device context for execute.
     */
    void execute ( METAFILEDEVICECONTEXT* /*source*/, HDC dc ) const
    {
      SaveDC( dc );
    }
#ifdef ENABLE_EDITING
    /*!
     * Print it to stdout.
     */
    void edit ( void ) const
    {
      printf( "*SAVEDC*\n" );
    }
#endif /* ENABLE_EDITING */
  };
  //! EMF Restore DC
  /*!
   * Use the stored device context in this context(?)
   */
  class EMRRESTOREDC : public METARECORD, ::EMRRESTOREDC {
  public:
    /*!
     * Create a Restore DC record.
     */
    EMRRESTOREDC ( INT n )
    {
      emr.iType = EMR_RESTOREDC;
      emr.nSize = sizeof( ::EMRRESTOREDC );
      iRelative = n;
    }
    /*!
     * Construct an Restoredc record from the input stream.
     * \param ds Metafile datastream.
     */
    EMRRESTOREDC ( DATASTREAM& ds )
    {
      ds >> emr >> iRelative;
    }
    /*!
     * \param fp Metafile file handle.
     */
    bool serialize ( DATASTREAM ds )
    {
      ds << emr << iRelative;
      return true;
    }
    /*!
     * Internally computed size of this record.
     */
    int size ( void ) const { return emr.nSize; }
    /*!
     * Execute this record in the context of the given device context.
     * \param source the device context from which this record is taken.
     * \param dc device context for execute.
     */
    void execute ( METAFILEDEVICECONTEXT* /*source*/, HDC dc ) const
    {
      RestoreDC( dc, iRelative );
    }
#ifdef ENABLE_EDITING
    /*!
     * Print it to stdout.
     */
    void edit ( void ) const
    {
      printf( "*RESTOREDC*\n" );
      printf( "\tiRelative: %ld\n", iRelative );
    }
#endif /* ENABLE_EDITING */
  };
  //! EMF Set Meta Region
  /*!
   * I really have no idea.
   */
  class EMRSETMETARGN : public METARECORD, ::EMRSETMETARGN {
  public:
    /*!
     * Create a Set Meta Rgn record.
     */
    EMRSETMETARGN ( void )
    {
      emr.iType = EMR_SETMETARGN;
      emr.nSize = sizeof( ::EMRSETMETARGN );
    }
    /*!
     * Construct an Setmetargn record from the input stream.
     * \param ds Metafile datastream.
     */
    EMRSETMETARGN ( DATASTREAM& ds )
    {
      ds >> emr;
    }
    /*!
     * \param fp Metafile file handle.
     */
    bool serialize ( DATASTREAM ds )
    {
      ds << emr;
      return true;
    }
    /*!
     * Internally computed size of this record.
     */
    int size ( void ) const { return emr.nSize; }
    /*!
     * Execute this record in the context of the given device context.
     * \param source the device context from which this record is taken.
     * \param dc device context for execute.
     */
    void execute ( METAFILEDEVICECONTEXT* /*source*/, HDC dc ) const
    {
      SetMetaRgn( dc );
    }
#ifdef ENABLE_EDITING
    /*!
     * Print it to stdout.
     */
    void edit ( void ) const
    {
      printf( "*SETMETARGN*\n" );
    }
#endif /* ENABLE_EDITING */
  };

  //! Graphics Pen
  /*!
   * Pens are used for drawing lines, arc, rectangles, etc.
   */
  class PEN : public GRAPHICSOBJECT, public LOGPEN {
  public:
    /*!
     * \param lpen the (logical?) pen definition.
     */
    PEN ( const LOGPEN* lpen )
    {
      lopnStyle = lpen->lopnStyle;
      lopnWidth = lpen->lopnWidth;
      lopnColor = lpen->lopnColor;
    }
    /*!
     * Return the type of this object (could probably do better with RTTI()).
     */
    OBJECTTYPE getType ( void ) const { return O_PEN; }
    /*!
     * Return a new metarecord for this object. And record its selection
     * into the given device context.
     * \param dc handle of device context into which this object is being selected.
     * \param emf_handle the EMF handle associated with the PEN.
     */
    METARECORD* newEMR ( HDC dc, HGDIOBJ emf_handle )
    {
      contexts[dc] = emf_handle;
      return new EMRCREATEPEN( this, emf_handle );
    }
  };

  //! Extended Graphics Pen
  /*!
   * Pens are used for drawing lines, arc, rectangles, etc.
   */
  class EXTPEN : public GRAPHICSOBJECT, public EXTLOGPEN {
  public:
    /*!
     * \param lpen the (logical?) pen definition.
     */
    EXTPEN ( const EXTLOGPEN* lpen )
    {
      elpPenStyle = lpen->elpPenStyle;
      elpWidth = lpen->elpWidth;
      elpBrushStyle = lpen->elpBrushStyle;
      elpColor = lpen->elpColor;
      elpHatch = lpen->elpHatch;
      elpNumEntries = 0;
      elpStyleEntry[0] = 0;
    }
    /*!
     * Return the type of this object (could probably do better with RTTI()).
     */
    OBJECTTYPE getType ( void ) const { return O_EXTPEN; }
    /*!
     * Return a new metarecord for this object. And record its selection
     * into the given device context.
     * \param dc handle of device context into which this object is being selected.
     * \param emf_handle the EMF handle associated with the PEN.
     */
    METARECORD* newEMR ( HDC dc, HGDIOBJ emf_handle )
    {
      contexts[dc] = emf_handle;
      return new EMREXTCREATEPEN( this, emf_handle );
    }
  };

  //! Graphics Brush
  /*!
   * Brushes are used for filling shapes.
   */
  class BRUSH : public GRAPHICSOBJECT, public LOGBRUSH {
  public:
    /*!
     * \param lbrush the (logical?) brush definition.
     */
    BRUSH ( const LOGBRUSH* lbrush )
    {
      lbStyle = lbrush->lbStyle;
      lbColor = lbrush->lbColor;
      lbHatch = lbrush->lbHatch;
    }
    /*!
     * Return the type of this object (could probably do better with RTTI()).
     */
    OBJECTTYPE getType ( void ) const { return O_BRUSH; }
    /*!
     * Return a new metarecord for this object. And record its selection
     * into the given device context.
     * \param dc handle of device context into which this object is being selected.
     * \param emf_handle the EMF handle associated with the BRUSH.
     */
    METARECORD* newEMR ( HDC dc, HGDIOBJ emf_handle )
    {
      contexts[dc] = emf_handle;
      return new EMRCREATEBRUSHINDIRECT( this, emf_handle );
    }
  };

  //! Graphics Font
  /*!
   * Fonts are used for drawing text (obviously).
   */
  class FONT : public GRAPHICSOBJECT, public EXTLOGFONTW {
  public:
    /*!
     * \param lfont the (logical?) font definition.
     */
    FONT ( const LOGFONTW* lfont )
    {
      this->elfLogFont = *lfont;
      // There are a lot more entries in the EXTLOGFONTW structure than
      // the API has values for, so we invent them here
      memset( &elfFullName, 0, sizeof elfFullName );
      memset( &elfStyle, 0, sizeof elfStyle );
      elfVersion = ELF_VERSION;
      elfStyleSize = 0;
      elfMatch = 0;
      elfReserved = 0;
      memset( &elfVendorId, 0, sizeof elfVendorId );
      elfCulture = ELF_CULTURE_LATIN;
      memset( &elfPanose, 1, sizeof(PANOSE) );
    }
    /*!
     * Return the type of this object (could probably do better with RTTI()).
     */
    OBJECTTYPE getType ( void ) const { return O_FONT; }
    /*!
     * Return a new metarecord for this object. And record its selection
     * into the given device context.
     * \param dc handle of device context into which this object is being selected.
     * \param emf_handle the EMF handle associated with the FONT.
     */
    METARECORD* newEMR ( HDC dc, HGDIOBJ emf_handle )
    {
      contexts[dc] = emf_handle;
      return new EMREXTCREATEFONTINDIRECTW( this, emf_handle );
    }
  };

  //! Graphics Palette
  /*!
   * Not entirely sure how palettes are used in general.
   */
  class PALETTE : public GRAPHICSOBJECT, public LOGPALETTE {
  public:
    /*!
     * \param lpalette the (logical?) palette definition.
     */
    PALETTE ( const LOGPALETTE* lpalette )
    {
      (void)lpalette;
      palVersion = 0;
      palNumEntries = 0;
      PALETTEENTRY zero_entry = { 0, 0, 0, 0 };
      palPalEntry[0] = zero_entry;
    }
    /*!
     * Return the type of this object (could probably do better with RTTI()).
     */
    OBJECTTYPE getType ( void ) const { return O_PALETTE; }
    /*!
     * Return a new metarecord for this object. And record its selection
     * into the given device context.
     * \param dc handle of device context into which this object is being selected.
     * \param emf_handle the EMF handle associated with the FONT.
     */
    METARECORD* newEMR ( HDC dc, HGDIOBJ emf_handle )
    {
      contexts[dc] = emf_handle;
      return new EMRCREATEPALETTE( this, emf_handle );
    }
  };

  //! Graphics Device Context
  /*!
   * Almost all GDI graphics calls require a device context (except those which
   * create graphics objects such as pens and fonts). This is a specific context
   * which renders to a metafile. There is a one-to-one correspondence between
   * the device context and the metafile.
   */
  class METAFILEDEVICECONTEXT : public OBJECT {
    /*!
     * We're only set up to render to a metafile, so initializing a device context
     * wants the metafile metadata.
     * \param size the left, top, right, and bottom limits of the metafile.
     * \param description_w the description of the metafile. See the constructor
     * for a description of the description.
     */
    void init ( const RECT* size, LPCWSTR description_w ) {

      // Evidently, metafile handles are numbered from 1, so don't
      // ever use 0.

      handles.push_back( true );

      // Keep some of our graphics state in a header record

      header = new ENHMETAHEADER ( description_w );
      records.push_back( header );

      // Compute the size and position of the metafile on the "page"

      if ( size ) {
	update_frame = false;

	header->rclFrame.left = size->left;
	header->rclFrame.top = size->top;
	header->rclFrame.right = size->right;
	header->rclFrame.bottom = size->bottom;

	header->rclBounds.left =
	  size->left * header->szlDevice.cx / ( header->szlMillimeters.cx * 100 );
	header->rclBounds.top =
	  size->top * header->szlDevice.cy / ( header->szlMillimeters.cy * 100 );
	header->rclBounds.right =
	  size->right * header->szlDevice.cx / ( header->szlMillimeters.cx * 100 );
	header->rclBounds.bottom =
	  size->bottom * header->szlDevice.cy / ( header->szlMillimeters.cy * 100 );
      }
      else {
	update_frame = true;

	header->rclBounds.left = -10;
	header->rclBounds.top = -10;
	header->rclBounds.right = 10;
	header->rclBounds.bottom = 10;

	header->rclFrame.left = (LONG)floor( (float)header->rclBounds.left *
	  header->szlMillimeters.cx * 100 / header->szlDevice.cx );
	header->rclFrame.top = (LONG)floor( (float)header->rclBounds.top *
	  header->szlMillimeters.cy * 100 / header->szlDevice.cy );
	header->rclFrame.right = (LONG)ceil( (float)header->rclBounds.right *
	  header->szlMillimeters.cx * 100 / header->szlDevice.cx );
	header->rclFrame.bottom = (LONG)ceil( (float)header->rclBounds.bottom *
	  header->szlMillimeters.cy * 100 / header->szlDevice.cy );
      }

      // Some default graphics state (are they really, though?)

      SIZEL default_resolution = { RESOLUTION, RESOLUTION };
      resolution = default_resolution;
      SIZEL default_viewport_ext = { 1, 1 };
      viewport_ext = default_viewport_ext;
      POINT default_viewport_org = { 0, 0 };
      viewport_org = default_viewport_org;
      SIZEL default_window_ext = { 1, 1 };
      window_ext = default_window_ext;
      POINT default_window_org = { 0, 0 };
      window_org = default_window_org;

      min_device_point = viewport_org;
      max_device_point = viewport_org;

      pen = (PEN*)globalObjects.find( BLACK_PEN | ENHMETA_STOCK_OBJECT );
      brush = (BRUSH*)globalObjects.find( BLACK_BRUSH | ENHMETA_STOCK_OBJECT );
      font = (FONT*)globalObjects.find( DEVICE_DEFAULT_FONT | ENHMETA_STOCK_OBJECT);
      palette = (PALETTE*)globalObjects.find( DEFAULT_PALETTE|ENHMETA_STOCK_OBJECT);

      text_alignment = TA_BASELINE;
      text_color = RGB(0,0,0);
      bk_color = RGB(0xff,0xff,0xff);
      bk_mode = OPAQUE;
      polyfill_mode = ALTERNATE;
      map_mode = MM_TEXT;

      handle = globalObjects.add( this );
    }

  public:
    /*!
     * If it is a file-based metafile, then this pointer is not null.
     */
    ::FILE* fp;
    /*!
     * All i/o to the metafile is wrapped by this class so that
     * byte swapping on big-endian machines is transparent.
     */
    DATASTREAM ds;
    /*!
     * Serves double duty as the physical device description.
     */
    ENHMETAHEADER* header;
    /*!
     * All of the metafile records are stored in memory.
     */
    std::vector< EMF::METARECORD* > records;

    // Keep a small set of graphics state information
    SIZEL resolution;		//!< The resolution in DPI of the *reference* DC.
    SIZEL viewport_ext;		//!< The extent of the viewport.
    POINT viewport_org;		//!< The origin of the viewport.
    SIZEL window_ext;		//!< The extent of the window.
    POINT window_org;		//!< The origin of the window.
    bool update_frame;		//!< Update the frame automatically?
    POINT min_device_point;	//!< The lft/top-most painted point in device units.
    POINT max_device_point;	//!< The rgt/btm-most painted point in device units.
    POINT point;		//!< The current point.
    PEN* pen;			//!< The current pen.
    BRUSH* brush;		//!< The current brush.
    FONT* font;			//!< The current font.
    PALETTE* palette;		//!< The current palette.
    UINT text_alignment;	//!< The current text alignment.
    COLORREF text_color;	//!< The current text foreground color.
    COLORREF bk_color;		//!< The current background color.
    INT bk_mode;		//!< The current background mode.
    INT polyfill_mode;		//!< The current polygon fill mode.
    INT map_mode;		//!< The current mapping mode.

    /*!
     * For compatibility, it appears that metafile handles are reused as
     * objects are deleted. Attempt to emulate that behavior with a
     * bit vector of used metafile handles.
     */
    std::vector< bool > handles;

    /*!
     * This map holds the *current* mapping between EMF handles and
     * global object handles as a metafile is played back (with
     * PlayEnhMetaFile).
     */
    std::map< HGDIOBJ, HGDIOBJ > emf_handles;

    /*!
     * Most graphics programs seem to want to handle the opening and closing
     * of files themselves, so this is an extension to the w32 interface.
     *
     * \param fp stdio pointer to an open file. May be null.
     * \param size the rectangle describing the position and size of the metafile
     * on the "page". May be null.
     * \param description_w a UNICODE string describing the metafile. The format
     * must be "some text\0some more text\0\0". May be null.
     */
      METAFILEDEVICECONTEXT ( FILE* fp_, const RECT* size,
			      LPCWSTR description_w )
	: fp(fp_), ds( fp_ )
    {
	  init( size, description_w );
    }
    /*!
     * Destructor frees all the graphics objects which may have been allocated.
     * Now, it also frees any metarecords which it might hold, too.
     */
    virtual ~METAFILEDEVICECONTEXT ( )
    {
      // Purge all the metarecords (if there are any) {this include the
      // header record, too}
      if ( records.size() > 0 )
	deleteMetafile();
    }
    /*!
     * Return the type of this object (could probably do better with RTTI()).
     */
    OBJECTTYPE getType ( void ) const { return O_METAFILEDEVICECONTEXT; }
    /*!
     * Scan the bit vector of used handles and return the index of the
     * first free bit as this objects metafile handle.
     */
    DWORD nextHandle ( void )
    {
      for ( unsigned int i = 1; i < handles.size(); i++ ) {
	if ( !handles[i] ) {
	  handles[i] = true;
	  return i;
	}
      }
      handles.push_back( true );
      // Well, it appears that even StockObject handles count for something.
      // Not sure what the right value here is, then.
      header->nHandles = handles.size();
      return handles.size()-1;
    }
    /*!
     * Clear the usage of this handle
     */
    void clearHandle ( DWORD handle )
    {
      handles[handle] = false;
    }
    /*!
     * Add this record to the metafile.
     *
     * \param record standard graphics record
     */
    void appendRecord ( METARECORD* record )
    {
      records.push_back( record );

      header->nBytes += record->size();
      header->nRecords++;
    }
    /*!
     * Add this record to the metafile.
     *
     * \param record this record is an object so it increments the handle count as well.
     */
    void appendHandle ( METARECORD* record )
    {
      records.push_back( record );

      header->nBytes += record->size();
      header->nRecords++;
    }
    /*!
     * Delete all the records from the metafile. This would seem to include deleting
     * the header record as well.
     */
    void deleteMetafile ( void )
    {
      for ( std::vector<METARECORD*>::const_iterator r = records.begin();
	    r != records.end();
	    r++ ) {
	delete *r;
      }
      records.clear();
    }
    /*!
     * Somewhat superfluous, except checker doesn't understand
     * the initialization of automatic structures in the declaration.
     */
    void mergePoint ( const LONG& x, const LONG& y )
    {
      POINT p;
      p.x = x;
      p.y = y;
      mergePoint( p );
    }
    /*!
     * Take the given point and determine if it enlarges the "painted"
     * area of the device.
     */
    void mergePoint( const POINT& p )
    {
      POINT device_point;

      // *** Note, it's possible for the global transformation matrix to
      // affect this too. ***

      device_point.x = (LONG)( (float)( p.x - window_org.x ) / window_ext.cx *
	viewport_ext.cx + viewport_org.x );

      device_point.y = (LONG)( (float)( p.y - window_org.y ) / window_ext.cy *
	viewport_ext.cy + viewport_org.y );

      // If the user didn't specify a bounding rectangle in the constructor,
      // compute one from this data, too.
      if ( device_point.x < min_device_point.x ) {
	min_device_point.x = device_point.x;
	if ( update_frame ) {
	  header->rclBounds.left = min_device_point.x - 10;
	  header->rclFrame.left = (LONG)floor( (float)header->rclBounds.left *
	    header->szlMillimeters.cx * 100 / header->szlDevice.cx );
	}
      }
      else if ( device_point.x > max_device_point.x ) {
	max_device_point.x = device_point.x;
	if ( update_frame ) {
	  header->rclBounds.right = max_device_point.x + 10;
	  header->rclFrame.right = (LONG)ceil( (float)header->rclBounds.right *
	    header->szlMillimeters.cx * 100 / header->szlDevice.cx );
	}
      }

      if ( device_point.y < min_device_point.y ) {
	min_device_point.y = device_point.y;
	if ( update_frame ) {
	  header->rclBounds.top = min_device_point.y - 10;
	  header->rclFrame.top = (LONG)floor( (float)header->rclBounds.top *
	    header->szlMillimeters.cy * 100 / header->szlDevice.cy );
	}
      }
      else if ( device_point.y > max_device_point.y ) {
	max_device_point.y = device_point.y;
	if ( update_frame ) {
	  header->rclBounds.bottom = max_device_point.y + 10;
	  header->rclFrame.bottom = (LONG)ceil( (float)header->rclBounds.bottom *
	    header->szlMillimeters.cy * 100 / header->szlDevice.cy );
	}
      }
    }
  };

} // close EMF namespace

#endif /* _LIBEMF_H */
