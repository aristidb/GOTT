// vim:ts=2:sw=2:expandtab:autoindent:filetype=cpp:
// TODO: DECIDE: DELETE THIS FILE?
#if 0
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

#include <gott/graphics/geometry.hpp>
#include <utility>
#include <algorithm>

namespace gott{

void rect::add_region( rect const& other ) 
{
  width = std::max( left + width, other.left + other.width);
  left = std::min( left, other.left );
  height = std::max( top + height, other.top + other.height);
  top = std::min( top, other.top);
}

void rect::subtract_region( rect const& other ) {
  if( left <= other.left && top <= other.top && left+width <= other.left+other.width && top+height <= other.top+other.height) 
    left = top = width = height = 0 ;
  else {
    if( other.left <= left && (other.left + other.width) >= (left + width) ) {
      if( other.top <= top &&  (other.top+long(other.height)) >= top ){
        long old_tp = top + long(height);
        top = std::min( other.top + long(other.height), old_tp );
        height = old_tp - top;
      }
      else if ( other.top + other.height > (top+height) && other.top <= (top+long(height))  ) 
      {
        height = other.top - top;
      }
    else if( other.top <= top
        && (other.top + other.height) >= (top + height) ) {
      if ( other.left <= left &&  (other.left+long(other.width)) >= left ){
        long old_lw = left + long(width);
        left = std::min( other.left+long(other.width), old_lw );
        width = old_lw - left;
      }
      else if ( other.left+other.width> (left+width) && other.left <= (left+long(width))  ) 
        width = other.left-width;
      }
    }
  }
}
}
#endif
