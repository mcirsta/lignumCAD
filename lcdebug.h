/* -*- c++ -*-
 * lcdebug.h
 *
 * Miscellaneous debugging routines
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

// This file contains miscellaneous debugging routines. Mostly
// output operators. File is empty if LC_NO_DEBUG is defined.
// In general, this file should be included as the last of the
// header files. There is some protection against improper order.


#ifndef LCDEBUG_H
#define LCDEBUG_H
#ifndef LC_NO_DEBUG

#include <iostream>

#include <qdatetime.h>

/*!
 * Return the current time to millisecond precision. Occasionally
 * useful as a prefix to an output string if the same thing is
 * printed over and over.
 * \return current time to ms precision.
 */
inline QString now ( void )
{
  return QTime::currentTime().toString( "hh:mm:ss.zzz " );
}

#ifdef VECTORALGEBRA_H
inline ostream& operator<< ( ostream& o, const Space2D::Point p )
{
  return o << p[Space2D::X] << ", " << p[Space2D::Y];
}

inline ostream& operator<< ( ostream& o, const Space2D::Vector v )
{
  return o << v[Space2D::X] << ", " << v[Space2D::Y];
}

inline ostream& operator<< ( ostream& o, const Space3D::Point p )
{
  return o << p[Space3D::X] << ", " << p[Space3D::Y] << ", " << p[Space3D::Z];
}

inline ostream& operator<< ( ostream& o, const Space3D::Vector v )
{
  return o << v[Space3D::X] << ", " << v[Space3D::Y] << ", " << v[Space3D::Z];
}
#endif // VECTORALGEBRA_H

#ifdef _gp_Pnt_HeaderFile
inline ostream& operator<< ( ostream& o, const gp_Pnt& p )
{
  return o << "[ " << p.X() << ", " << p.Y() << ", " << p.Z() << " ]";
}
#endif // _gp_Pnt_HeaderFile

#ifdef _gp_Dir_HeaderFile
inline ostream& operator<< ( ostream& o, const gp_Dir& d )
{
  return o << "[ " << d.X() << ", " << d.Y() << ", " << d.Z() << " ]";
}
#endif // _gp_Dir_HeaderFile

#ifdef _gp_Vec_HeaderFile
inline ostream& operator<< ( ostream& o, const gp_Vec& v )
{
  return o << "[ " << v.X() << ", " << v.Y() << ", " << v.Z() << " ]";
}
#endif // _gp_Vec_HeaderFile

#ifdef _gp_Ax2_HeaderFile
inline ostream& operator<< ( ostream& o, const gp_Ax2& a )
{
  return o << a.Location() << "; " << a.Direction();
}
#endif // _gp_Ax2_HeaderFile


#ifdef _gp_Ax3_HeaderFile
inline ostream& operator<< ( ostream& o, const gp_Ax3& a )
{
  return o << a.Location() << "; " << a.Direction();
}
#endif // _gp_Ax3_HeaderFile

#ifdef _gp_Pln_HeaderFile
inline ostream& operator<< ( ostream& o, const gp_Pln& p )
{
  return o << p.Axis().Location() << "; " << p.Axis().Direction();
}
#endif // _gp_Pln_HeaderFile

#ifdef _gp_Trsf_HeaderFile
inline ostream& operator<< ( ostream& o, const gp_Trsf& t )
{
  o << "[[ " << t.Value(1,1) << ", " << t.Value(1,2) << ", " << t.Value(1,3) << ", " << t.Value(1,4) << " ]," << endl;
  o << " [ " << t.Value(2,1) << ", " << t.Value(2,2) << ", " << t.Value(2,3) << ", " << t.Value(2,4) << " ]," << endl;
  o << " [ " << t.Value(3,1) << ", " << t.Value(3,2) << ", " << t.Value(3,3) << ", " << t.Value(3,4) << " ]]" << endl;
  return o;
}
#endif // _gp_Trsf_HeaderFile

#ifdef _TopAbs_ShapeEnum_HeaderFile
inline ostream& operator<< ( ostream& o, const TopAbs_ShapeEnum& shape_type )
{
  const char* types[] = { "COMPOUND","COMPSOLID", "SOLID", "SHELL", "FACE",
			 "WIRE", "EDGE", "VERTEX", "SHAPE" };
  return o << types[ shape_type ];
}
#endif // _TopAbs_ShapeEnum_HeaderFile

inline ostream& operator<< ( ostream& o, const QValueVector<uint>& id_path )
{
  if ( id_path.size() > 0 ) {
    o << id_path[0];
    for ( uint i = 1; i < id_path.size(); i++ )
      o << " : " << id_path[i];
  }
  return o;
}
#endif // LC_NO_DEBUG

#endif // LCDEBUG_H
