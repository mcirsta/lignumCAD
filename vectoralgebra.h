/* -*- c++ -*-
 * vectoralgebra.h
 *
 * Header for the vector algebra classes
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
#ifndef VECTORALGEBRA_H
#define VECTORALGEBRA_H

#ifndef LC_NO_DEBUG
#include <iostream>
#endif
#include <cmath>

// This is divided into two namespaces, one for 2D entities and one
// for 3D entities. Saves some space in the 2D views.

namespace Space3D {
  class Vector;			// Forward declaration for conversion operator.
  class Point;			// Forward declaration for conversion operator.
}

namespace Space2D {

  enum CARTESIAN { X, Y };

  // A vector in 2D space

  struct Vector {
    double v_[2];
    Vector ( double x = 0, double y = 0 )
    {
      v_[X] = x;
      v_[Y] = y;
    }

    Vector ( const Space3D::Vector& v );

    operator const double* ( void ) const { return v_; }
    double& operator[] ( enum CARTESIAN i ) { return v_[i]; }

    Vector& operator*= ( double k );

    double orientation ( void ) const;
    double length ( void ) const;

    Vector& normalize ( void );
    Vector& rotate ( double degrees );
  };

#ifndef LC_NO_DEBUG
  inline std::ostream& operator<< ( std::ostream& o, const Space2D::Vector v )
  {
    return o << v[Space2D::X] << ", " << v[Space2D::Y];
  }
#endif

  inline Vector& Vector::operator*= ( double k )
  {
    v_[X] *= k;
    v_[Y] *= k;
    return *this;
  }

  inline double Vector::orientation ( void ) const
  {
    return (180./M_PI) * atan2( v_[Y], v_[X] );
  }

  inline double Vector::length ( void ) const
  {
    return hypot( v_[X], v_[Y] );
  }

  inline Vector& Vector::normalize ( void )
  {
    double l = length();
    if ( l != 0. ) {
      v_[X] /= l;
      v_[Y] /= l;
    }
    return *this;
  }

  inline Vector& Vector::rotate ( double degrees )
  {
    double theta = M_PI * degrees / 180.;
    double cost = cos( theta );
    double sint = sin( theta );
    double x = cost * v_[X] - sint * v_[Y];
    double y = sint * v_[X] + cost * v_[X];
    v_[X] = x;
    v_[Y] = y;
    return *this;
  }

  inline Vector operator+ ( const Vector& a, const Vector& b )
  {
    return Vector( a.v_[X] + b.v_[X], a.v_[Y] + b.v_[Y] );
  }

  inline Vector operator- ( const Vector& a, const Vector& b )
  {
    return Vector( a.v_[X] - b.v_[X], a.v_[Y] - b.v_[Y] );
  }

  inline Vector operator* ( const double k, const Vector& a )
  {
    return Vector( k * a.v_[X], k * a.v_[Y] );
  }

  inline Vector operator* ( const Vector& a, const double k )
  {
    return Vector( k * a.v_[X], k * a.v_[Y] );
  }

  inline double operator* ( const Vector& a, const Vector& b )
  {
    return a.v_[X] * b.v_[X] +  a.v_[Y] * b.v_[Y];
  }

  // A point in 2D space

  struct Point {
    double p_[2];
    Point ( double x = 0, double y = 0 )
    {
      p_[X] = x;
      p_[Y] = y;
    }

    Point ( const Space3D::Point& p );

    operator const double* ( void ) const { return p_; }
    double& operator[] ( enum CARTESIAN i ) { return p_[i]; }

    Point& operator+= ( const Vector& b );
    Point& operator-= ( const Vector& b );
    bool operator== ( const Point& b ) const;
    bool operator!= ( const Point& b ) const;
    double distance ( double x, double y ) const;
    double distance ( const Point& b ) const;
  };

#ifndef LC_NO_DEBUG
  inline std::ostream& operator<< ( std::ostream& o, const Space2D::Point p )
  {
    return o << p[Space2D::X] << ", " << p[Space2D::Y];
  }
#endif

  inline Point& Point::operator+= ( const Vector& b )
  {
    p_[X] += b.v_[X];
    p_[Y] += b.v_[Y];
    return *this;
  }

  inline Point& Point::operator-= ( const Vector& b )
  {
    p_[X] -= b.v_[X];
    p_[Y] -= b.v_[Y];
    return *this;
  }

  inline bool Point::operator== ( const Point& b ) const
  {
    return p_[X] == b.p_[X] && p_[Y] == b.p_[Y];
  }

  inline bool Point::operator!= ( const Point& b ) const
  {
    return p_[X] != b.p_[X] || p_[Y] != b.p_[Y];
  }

  inline double Point::distance ( double x, double y ) const
  {
    return sqrt( ( p_[X] - x ) * ( p_[X] - x ) +
		 ( p_[Y] - y ) * ( p_[Y] - y ) );
  }

  inline double Point::distance ( const Point& b ) const
  {
    return sqrt( ( p_[X] - b.p_[X] ) * ( p_[X] - b.p_[X] ) +
		 ( p_[Y] - b.p_[Y] ) * ( p_[Y] - b.p_[Y] ) );
  }

  inline Point floor ( const Point& p )
  {
    return Point( ::floor( p[X] ), ::floor( p[Y] ) );
  }

  inline Point ceil ( const Point& p )
  {
    return Point( ::ceil( p[X] ), ::ceil( p[Y] ) );
  }

  inline Vector operator- ( const Point& a, const Point& b )
  {
    return Vector( a.p_[X] - b.p_[X], a.p_[Y] - b.p_[Y] );
  }

  inline Point operator+ ( const Point& a, const Vector& b )
  {
    return Point( a.p_[X] + b.v_[X], a.p_[Y] + b.v_[Y] );
  }

  inline Point operator- ( const Point& a, const Vector& b )
  {
    return Point( a.p_[X] - b.v_[X], a.p_[Y] - b.v_[Y] );
  }
} // End of Space2D namespace

namespace Space3D {
  enum CARTESIAN { X, Y, Z, W };

  // A vector in 3D space

  struct Vector {
    double v_[3];
    Vector ( double x = 0, double y = 0, double z = 0 )
    {
      v_[X] = x;
      v_[Y] = y;
      v_[Z] = z;
    }

    operator const double* ( void ) const { return v_; }
    double& operator[] ( enum CARTESIAN i ) { return v_[i]; }

    Vector& operator*= ( double k );
    Vector cross ( const Vector& b ) const;

    double length ( void ) const;
    Vector& rotate ( double degrees, const Vector& about );
    Vector& normalize ( void );
  };

#ifndef LC_NO_DEBUG
  inline std::ostream& operator<< ( std::ostream& o, const Space3D::Vector v )
  {
    return o << v[Space3D::X] << ", " << v[Space3D::Y] << ", " << v[Space3D::Z];
  }
#endif

  inline Vector& Vector::operator*= ( double k )
  {
    v_[X] *= k;
    v_[Y] *= k;
    v_[Z] *= k;
    return *this;
  }

  inline Vector Vector::cross ( const Vector& b ) const
  {
    return Vector( v_[Y] * b.v_[Z] - b.v_[Y] * v_[Z],
		   v_[Z] * b.v_[X] - b.v_[Z] * v_[X],
		   v_[X] * b.v_[Y] - b.v_[X] * v_[Y] );
  }

  inline double operator* ( const Vector& a, const Vector& b )
  {
    return a.v_[X] * b.v_[X] +  a.v_[Y] * b.v_[Y] +  a.v_[Z] * b.v_[Z];
  }

  inline double Vector::length ( void ) const
  {
    return sqrt( *this * *this );
  }

  inline Vector& Vector::normalize ( void )
  {
    double l = length();
    if ( l != 0. ) {
      v_[X] /= l;
      v_[Y] /= l;
      v_[Z] /= l;
    }
    return *this;
  }

  inline Vector operator+ ( const Vector& a, const Vector& b )
  {
    return Vector( a.v_[X] + b.v_[X], a.v_[Y] + b.v_[Y], a.v_[Z] + b.v_[Z] );
  }

  inline Vector operator- ( const Vector& a, const Vector& b )
  {
    return Vector( a.v_[X] - b.v_[X], a.v_[Y] - b.v_[Y], a.v_[Z] - b.v_[Z] );
  }

  inline Vector operator* ( const double k, const Vector& a )
  {
    return Vector( k * a.v_[X], k * a.v_[Y], k * a.v_[Z] );
  }

  inline Vector operator* ( const Vector& a, const double k )
  {
    return Vector( k * a.v_[X], k * a.v_[Y], k * a.v_[Z] );
  }

  // A point in 3D space

  struct Point {
    double p_[3];
    Point ( double x = 0, double y = 0, double z = 0 )
    {
      p_[X] = x;
      p_[Y] = y;
      p_[Z] = z;
    }

    operator const double* ( void ) const { return p_; }
    operator double* ( void ) { return p_; }
    double& operator[] ( enum CARTESIAN i ) { return p_[i]; }

    Point& operator+= ( const Vector& b );
    Point& operator-= ( const Vector& b );

    bool operator== ( const Point& b ) const;

    double distance ( double x, double y, double z ) const;
    double distance ( const Point& b ) const;
    /*!
     * Project the point on to the plane.
     * \param o origin of the plane.
     * \param n normal of the plane.
     * \return projection of this point onto the plane.
     */
    Point project ( const Point& o, const Vector& n ) const;
  };

#ifndef LC_NO_DEBUG
  inline std::ostream& operator<< ( std::ostream& o, const Space3D::Point p )
  {
    return o << p[Space3D::X] << ", " << p[Space3D::Y] << ", " << p[Space3D::Z];
  }
#endif

  inline Point& Point::operator+= ( const Vector& b )
  {
    p_[X] += b.v_[X];
    p_[Y] += b.v_[Y];
    p_[Z] += b.v_[Z];
    return *this;
  }

  inline Point& Point::operator-= ( const Vector& b )
  {
    p_[X] -= b.v_[X];
    p_[Y] -= b.v_[Y];
    p_[Z] -= b.v_[Z];
    return *this;
  }

  inline bool Point::operator== ( const Point& b ) const
  {
    return p_[X] == b.p_[X] && p_[Y] == b.p_[Y] && p_[Z] == b.p_[Z];
  }

  inline double Point::distance ( double x, double y, double z ) const
  {
    return sqrt( ( p_[X] - x ) * ( p_[X] - x ) +
		 ( p_[Y] - y ) * ( p_[Y] - y ) +
		 ( p_[Z] - z ) * ( p_[Z] - z ) );
  }

  inline double Point::distance ( const Point& b ) const
  {
    return sqrt( ( p_[X] - b.p_[X] ) * ( p_[X] - b.p_[X] ) +
		 ( p_[Y] - b.p_[Y] ) * ( p_[Y] - b.p_[Y] ) +
		 ( p_[Z] - b.p_[Z] ) * ( p_[Z] - b.p_[Z] ) );
  }

  inline Point floor ( const Point& p )
  {
    return Point( ::floor( p[X] ), ::floor( p[Y] ), ::floor( p[Z] ) );
  }

  inline Point ceil ( const Point& p )
  {
    return Point( ::ceil( p[X] ), ::ceil( p[Y] ), ::ceil( p[Z] ) );
  }

  inline Vector operator- ( const Point& a, const Point& b )
  {
    return Vector( a.p_[X] - b.p_[X], a.p_[Y] - b.p_[Y], a.p_[Z] - b.p_[Z] );
  }

  inline Point operator+ ( const Point& a, const Vector& b )
  {
    return Point( a.p_[X] + b.v_[X], a.p_[Y] + b.v_[Y], a.p_[Z] + b.v_[Z] );
  }

  inline Point operator- ( const Point& a, const Vector& b )
  {
    return Point( a.p_[X] - b.v_[X], a.p_[Y] - b.v_[Y], a.p_[Z] - b.v_[Z] );
  }


  inline Point Point::project ( const Point& o, const Vector& n ) const
  {
    Vector d = *this - o;
    double l = d * n;

    return *this - l * n;
  }


  struct Matrix {
    enum Type { IDENTITY, TRANSLATION, XROTATION, YROTATION, ZROTATION, ROTATION };

    Vector m_[4];

    Matrix ( enum Type type = IDENTITY, double value0 = 0, double value1 = 0,
	     double value2 = 0, double value3 = 0 )
    {
      switch ( type ) {
      case IDENTITY:
	m_[X][X] = 1; m_[Y][X] = 0; m_[Z][X] = 0; m_[W][X] = 0;
	m_[X][Y] = 0; m_[Y][Y] = 1; m_[Z][Y] = 0; m_[W][Y] = 0;
	m_[X][Z] = 0; m_[Y][Z] = 0; m_[Z][Z] = 1; m_[W][Z] = 0;
	break;
      case TRANSLATION:
	m_[X][X] = 1; m_[Y][X] = 0; m_[Z][X] = 0; m_[W][X] = value0;
	m_[X][Y] = 0; m_[Y][Y] = 1; m_[Z][Y] = 0; m_[W][Y] = value1;
	m_[X][Z] = 0; m_[Y][Z] = 0; m_[Z][Z] = 1; m_[W][Z] = value2;
	break;
      case XROTATION:
	m_[X][X] = 1; m_[Y][X] = 0;      m_[Z][X] = 0;       m_[W][X] = 0;
	m_[X][Y] = 0; m_[Y][Y] = value0; m_[Z][Y] = -value1; m_[W][Y] = 0;
	m_[X][Z] = 0; m_[Y][Z] = value1; m_[Z][Z] =  value0; m_[W][Z] = 0;
	break;
      case YROTATION:
	m_[X][X] =  value0; m_[Y][X] = 0; m_[Z][X] = value1; m_[W][X] = 0;
	m_[X][Y] = 0;       m_[Y][Y] = 1; m_[Z][Y] = 0;      m_[W][Y] = 0;
	m_[X][Z] = -value1; m_[Y][Z] = 0; m_[Z][Z] = value0; m_[W][Z] = 0;
	break;
      case ZROTATION:
	m_[X][X] = value0; m_[Y][X] = -value1; m_[Z][X] = 0; m_[W][X] = 0;
	m_[X][Y] = value1; m_[Y][Y] =  value0; m_[Z][Y] = 0; m_[W][Y] = 0;
	m_[X][Z] = 0;      m_[Y][Z] = 0;       m_[Z][Z] = 1; m_[W][Z] = X;
	break;
      case ROTATION:
	{
	  double ct = cos( value3 );
	  double st = sin( value3 );
	  double v00 = value0 * value0;
	  double v01 = value0 * value1;
	  double v02 = value0 * value2;
	  double v10 = value1 * value0;
	  double v11 = value1 * value1;
	  double v12 = value1 * value2;
	  double v20 = value2 * value0;
	  double v21 = value2 * value1;
	  double v22 = value2 * value2;

	  m_[X][X] = v00 + ct * ( 1 - v00 );
	  m_[Y][X] = v01 + ct * (   - v01 ) + st * ( -value2 );
	  m_[Z][X] = v02 + ct * (   - v02 ) + st * value1;
	  m_[W][X] = 0;

	  m_[X][Y] = v10 + ct * (   - v10 ) + st * value2;
	  m_[Y][Y] = v11 + ct * ( 1 - v11 );
	  m_[Z][Y] = v12 + ct * (   - v12 ) + st * ( -value0 );
	  m_[W][Y] = 0;

	  m_[X][Z] = v20 + ct * (   - v20 ) + st * ( -value1 );
	  m_[Y][Z] = v21 + ct * (   - v21 ) + st * value0;
	  m_[Z][Z] = v22 + ct * ( 1 - v22 );
	  m_[W][Z] = 0;
	}
	break;	
      }
    }

    Vector& operator[] ( enum CARTESIAN i ) { return m_[i]; }
    const Vector& operator[] ( enum CARTESIAN i ) const { return m_[i]; }

    Matrix& operator*= ( const Matrix& m );
  };

  inline Vector operator* ( const Matrix& m, const Vector& v )
  {
    // Note: This is the transformation of v by the matrix m.
    // Specifically, this operation requires the inverse of m;
    // however, here we simply use the transpose of m. Therefore,
    // the upper left 3x3 part of m must be self-adjoint.
    return Vector ( m[X][X] * v[X] + m[X][Y] * v[Y] + m[X][Z] * v[Z],
		    m[Y][X] * v[X] + m[Y][Y] * v[Y] + m[Y][Z] * v[Z],
		    m[Z][X] * v[X] + m[Z][Y] * v[Y] + m[Z][Z] * v[Z] );
  }

  inline Point operator* ( const Matrix& m, const Point& p )
  {
    return Point( m[X][X] * p[X] + m[Y][X] * p[Y] + m[Z][X] * p[Z] + m[W][X],
		  m[X][Y] * p[X] + m[Y][Y] * p[Y] + m[Z][Y] * p[Z] + m[W][Y],
		  m[X][Z] * p[X] + m[Y][Z] * p[Y] + m[Z][Z] * p[Z] + m[W][Z] );
  }

  inline Vector& Vector::rotate ( double degrees, const Vector& about )
  {
    double angle = M_PI * degrees / 180.;

    Matrix r( Matrix::ROTATION, about[X], about[Y], about[Z], angle );

    *this = r * *this;

    return *this;
  }

  inline Matrix& Matrix::operator*= ( const Matrix& b )
  {
    Matrix a = *this;
    m_[X] = Vector( a[X][X]*b[X][X] + a[Y][X]*b[X][Y] + a[Z][X]*b[X][Z],
		    a[X][Y]*b[X][X] + a[Y][Y]*b[X][Y] + a[Z][Y]*b[X][Z],
		    a[X][Z]*b[X][X] + a[Y][Z]*b[X][Y] + a[Z][Z]*b[X][Z] );
    m_[Y] = Vector( a[X][X]*b[Y][X] + a[Y][X]*b[Y][Y] + a[Z][X]*b[Y][Z],
		    a[X][Y]*b[Y][X] + a[Y][Y]*b[Y][Y] + a[Z][Y]*b[Y][Z],
		    a[X][Z]*b[Y][X] + a[Y][Z]*b[Y][Y] + a[Z][Z]*b[Y][Z] );
    m_[Z] = Vector( a[X][X]*b[Z][X] + a[Y][X]*b[Z][Y] + a[Z][X]*b[Z][Z],
		    a[X][Y]*b[Z][X] + a[Y][Y]*b[Z][Y] + a[Z][Y]*b[Z][Z],
		    a[X][Z]*b[Z][X] + a[Y][Z]*b[Z][Y] + a[Z][Z]*b[Z][Z] );
    m_[W] = Vector( a[X][X]*b[W][X] + a[Y][X]*b[W][Y] + a[Z][X]*b[W][Z] + a[W][X],
		    a[X][Y]*b[W][X] + a[Y][Y]*b[W][Y] + a[Z][Y]*b[W][Z] + a[W][Y],
		    a[X][Z]*b[W][X] + a[Y][Z]*b[W][Y] + a[Z][Z]*b[W][Z] + a[W][Z] );

    return *this;
  }

  inline Matrix operator* ( const Matrix& a, const Matrix& b )
  {
    Matrix c;
    c[X] = Vector( a[X][X]*b[X][X] + a[Y][X]*b[X][Y] + a[Z][X]*b[X][Z],
		   a[X][Y]*b[X][X] + a[Y][Y]*b[X][Y] + a[Z][Y]*b[X][Z],
		   a[X][Z]*b[X][X] + a[Y][Z]*b[X][Y] + a[Z][Z]*b[X][Z] );
    c[Y] = Vector( a[X][X]*b[Y][X] + a[Y][X]*b[Y][Y] + a[Z][X]*b[Y][Z],
		   a[X][Y]*b[Y][X] + a[Y][Y]*b[Y][Y] + a[Z][Y]*b[Y][Z],
		   a[X][Z]*b[Y][X] + a[Y][Z]*b[Y][Y] + a[Z][Z]*b[Y][Z] );
    c[Z] = Vector( a[X][X]*b[Z][X] + a[Y][X]*b[Z][Y] + a[Z][X]*b[Z][Z],
		   a[X][Y]*b[Z][X] + a[Y][Y]*b[Z][Y] + a[Z][Y]*b[Z][Z],
		   a[X][Z]*b[Z][X] + a[Y][Z]*b[Z][Y] + a[Z][Z]*b[Z][Z] );
    c[W] = Vector( a[X][X]*b[W][X] + a[Y][X]*b[W][Y] + a[Z][X]*b[W][Z] + a[W][X],
		   a[X][Y]*b[W][X] + a[Y][Y]*b[W][Y] + a[Z][Y]*b[W][Z] + a[W][Y],
		   a[X][Z]*b[W][X] + a[Y][Z]*b[W][Y] + a[Z][Z]*b[W][Z] + a[W][Z] );

    return c;
  }

  inline std::ostream& operator<< ( std::ostream& o, const Matrix& m )
  {
    o << "[[ " << m[X][X] << ", " << m[Y][X] << ", " << m[Z][X] << ", " << m[W][X]
      << "]," << std::endl;
    o << " [ " << m[X][Y] << ", " << m[Y][Y] << ", " << m[Z][Y] << ", " << m[W][Y]
      << "]," << std::endl;
    o << " [ " << m[X][Z] << ", " << m[Y][Z] << ", " << m[Z][Z] << ", " << m[W][Z]
      << "]]" << std::endl;
    return o;
  }
} // End of Space3D namespace

namespace Space2D {
  inline Vector::Vector ( const Space3D::Vector& v )
  {
    // Implicitly a projection on to the XY plane.
    v_[X] = v.v_[Space3D::X];
    v_[Y] = v.v_[Space3D::Y];
  }
  inline Point::Point ( const Space3D::Point& p )
  {
    // Implicitly a projection on to the XY plane.
    p_[X] = p.p_[Space3D::X];
    p_[Y] = p.p_[Space3D::Y];
  }
} // End of Space2D namespace

#endif // VECTORALGEBRA_H
