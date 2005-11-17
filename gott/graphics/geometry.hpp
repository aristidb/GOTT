#ifndef GOTT_UTIL_GEOMETRY_HPP_INCLUDED
#define GOTT_UTIL_GEOMETRY_HPP_INCLUDED
// Copyright (C) 2004-2005 by Andreas Pokorny andreas.pokorny@gmail.com
// Content: GOTT simple geometry utility classes 
// Authors: Andreas Pokorny
//
// This file is part of the Gott Project (http://gott.sf.net)
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

#include <gott/visibility.hpp>
#include <cstddef>

// move into subdir if this file grows to much 
namespace gott{

/**
 * \brief Small coordinate structure.
 */
struct coord
{
  coord() : x(0), y(0) {}
  coord( int x_, int y_ ) : x(x_), y(y_){}
  int x, y;
  inline coord& operator +=(coord const& r)  { x+=r.x;y+=r.y; return *this; }
};

inline coord operator +(coord const& l, coord const& r)  { 
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
  rect( long l, long t, size_t w, size_t h) 
  : left(l), top(t), width(w), height(h) {}

  rect() 
  : left(0), top(0), width(1), height(1) {}

  /**
   * \returns true if coordinate is inside the rectangle.
   */
  inline bool is_inside( coord const& c ) const{
    return c.x >= left && c.x <= left + long(width)  
      &&  c.y >= top && c.y <= top + long(height);
  }

  /**
   * \brief Turns *this into a rectangle containing *this and other
   */
  GOTT_EXPORT void add_region( rect const& other );

  /**
   * \brief Turns *this into a rectangle containing parts of *this that are not in other
   */
  GOTT_EXPORT void subtract_region( rect const& other );


  /**
   * \see add_region
   */
  inline rect &operator+=(rect const &other) {
    add_region(other);
    return *this;
  }
};

inline rect operator+(rect r1, rect const &r2) { return r1 += r2; }
inline bool operator== ( rect const& l, rect const& r) { return l.left==r.left && l.top == r.top && l.width==r.width && l.height == r.height;} 
inline bool operator!= ( rect const& l, rect const& r) { return !(l==r);}

}

#endif

