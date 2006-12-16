/* ***** BEGIN LICENSE BLOCK *****
 * Version: MPL 1.1/GPL 2.0/LGPL 2.1
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code is a set of geometry classes.
 *
 * The Initial Developer of the Original Code is
 * Andreas Pokorny (andreas.pokorny@gmail.com)
 * Portions created by the Initial Developer are Copyright (C) 2005-2006
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *  Andreas Pokorny (andreas.pokorny@gmail.com)
 *  Aristid Breitkreuz (aribrei@arcor.de)
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 2 or later (the "GPL"), or
 * the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
 * in which case the provisions of the GPL or the LGPL are applicable instead
 * of those above. If you wish to allow use of your version of this file only
 * under the terms of either the GPL or the LGPL, and not to allow others to
 * use your version of this file under the terms of the MPL, indicate your
 * decision by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL or the LGPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of any one of the MPL, the GPL or the LGPL.
 *
 * ***** END LICENSE BLOCK ***** */

#ifndef GOTT_UTIL_GEOMETRY_HPP_INCLUDED
#define GOTT_UTIL_GEOMETRY_HPP_INCLUDED

#include <gott/visibility.hpp>
#include <cstddef>
#include <iostream>

// move into subdir if this file grows to much 
//! \todo Reimplement these structure cleanly as soon as all 
//! requirements are known, but update all users of this file, 
//! before deletion!
namespace gott {

/**
 * \brief Small coordinate structure.
 */
struct coord {
  coord() : x(0), y(0) {}
  coord(int x_, int y_) : x(x_), y(y_) {}
  int x, y;
  inline coord& operator +=(coord const& r)  { x+=r.x;y+=r.y; return *this; }
};

inline coord operator +(coord const& l, coord const& r) { 
  coord t(l); 
  t += r; 
  return t; 
}

/** 
 * \brief Small rectangle structure
 */
struct rect 
{
  long left, top; 
  size_t width, height;
  rect(long l, long t, size_t w, size_t h) 
  : left(l), top(t), width(w), height(h) {}

  rect() 
  : left(0), top(0), width(1), height(1) {}

  /**
   * \returns true if coordinate is inside the rectangle.
   */
  inline bool is_inside(coord const& c) const {
    return c.x >= left && c.x <= left + long(width)  
      &&  c.y >= top && c.y <= top + long(height);
  }

  /**
   * \brief Turns *this into a rectangle containing *this and other
   */
  GOTT_EXPORT void add_region(rect const& other);

  /**
   * \brief Turns *this into a rectangle containing parts of *this that are not in other
   */
  GOTT_EXPORT void subtract_region(rect const& other);


  /**
   * \see add_region
   */
  inline rect &operator+=(rect const &other) {
    add_region(other);
    return *this;
  }
};

inline rect operator+(rect r1, rect const &r2) { return r1 += r2; }
inline bool operator== (rect const& l, rect const& r) { return l.left==r.left && l.top == r.top && l.width==r.width && l.height == r.height;} 
inline bool operator!= (rect const& l, rect const& r) { return !(l==r);}

inline std::ostream& operator << ( std::ostream& lhs, rect const &rhs )
{
  return lhs << rhs.left << 'x' << rhs.top << "->"<< rhs.width << "," << rhs.height;
}

}

#endif

