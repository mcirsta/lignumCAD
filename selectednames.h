/* -*- c++ -*-
 * selectednames.h
 *
 * SelectedNames class: A tree representation of OpenGL selections.
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
#ifndef SELECTEDNAMES_H
#define SELECTEDNAMES_H

#include <GL/gl.h>

#include <map>
#include <vector>
/*!
 * Here is an alternate implementation of SelectedNames which does away
 * with the tree and, instead, is just a C++ reinterpretation of the
 * OpenGL hit record. This is slightly improved on by sorting in
 * increasing distance from the viewpoint.
 *
 * There seems to be little reason to hide the nature of this structure
 * since every thing is essentially an automatic variable.
 */
class SelectedNames : public std::multimap< GLfloat, std::vector< GLuint > >
{
public:
  /*!
   * Insert an OpenGL hit record into the data struct. The hit record
   * consists of a count of the number of names on the stack at
   * the time of the hit, the nearest and furthest z values
   * of the primitives (times 2^32 - 1) and (count) number of selection names.
   * \param hit_record OpenGL selection hit record.
   */
  void addRecord ( GLuint* hit_record )
  {
    GLuint names = *hit_record++;
    GLfloat z0 = (GLfloat) (*hit_record++) / 0x7fffffff;
    hit_record++;		// For now, ignore the z far value.
    iterator record =
      insert( std::pair< GLfloat, std::vector< GLuint > >( z0, std::vector< GLuint >( names ) ) );
    for ( GLuint j = 0; j < names; j++ )
      (*record).second[j] = *hit_record++;
  }

  /*!
   * Append the contents of the given object into the current
   * object. (I think may just be syntactic sugar now.)
   * \param b object containing data to append to this object.
   */
  void operator+= ( const SelectedNames& b )
  {
    insert( b.begin(), b.end() );
  }
};

/*!
 * Compare two SelectedNames object for equality. Since the depth
 * values can change as the mouse wanders around, the standard
 * equivalence operator for the multimap is not exactly true.
 * (Well, it true of course within the framework for STL multimaps,
 * but we provide a different semantic here). Two SelectedNames are
 * equal if the same set of vector<GLunit>'s appear in both objects.
 */
inline bool operator== ( const SelectedNames& a, const SelectedNames& b )
{
  if ( a.size() != b.size() ) {
    return false;
  }

  if ( a.empty() ) {
    return true;
  }

  // This comparison depends on records being selected in the same
  // order on both scans. However, it is not wrong for this routine
  // to declare two equal objects to be different, just more inefficient
  // for the drawing routines.

  bool equal = true;

  SelectedNames::const_iterator ia = a.begin();

  for ( ; ia != a.end(); ++ia ) {
    SelectedNames::const_iterator ib = b.begin();

    bool equal_row = false;
    for ( ; ib != b.end(); ++ib ) {
      if ( (*ia).second == (*ib).second ) {
	equal_row = true;
	break;
      }
    }
    if ( !equal_row ) {		// I.e., The row of a has no corresponding row in b
      equal = false;		// so the objects are different.
      break;
    }
  }

  return equal;
}

inline std::ostream& operator<< ( std::ostream& o, const SelectedNames& sn )
{
  if ( sn.empty() )
    return o << "empty" << std::endl;

  SelectedNames::const_iterator i = sn.begin();

  for ( ; i != sn.end(); ++i ) {
    o << "Depth: " << (*i).first << ": ";
    std::vector<GLuint>::const_iterator j = (*i).second.begin();
    for ( ; j != (*i).second.end(); ++j ) {
      o << (*j) << " ";
    }
    o << std::endl;
  }

  return o;
}

#endif // SELECTEDNAMES_H
