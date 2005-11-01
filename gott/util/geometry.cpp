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

#include <utility>
#include <algorithm>
#include <gott/util/geometry.hpp>

namespace gott{
rect::rect( long l, long t, std::size_t w, std::size_t h)
  : left(l), top(t), width(w), height(h)
{
}
rect::rect()
  : left(0), top(0), width(1), height(1)
{}

void rect::add_region( rect const& other ) {
  width = std::max( left + width, other.left + other.width);
  left = std::min( left, other.left );
  height = std::max( top + height, other.top + other.height);
  top = std::min( top, other.top);
}

bool rect::is_inside( coord const& c  ) const {
  return c.x >= left && c.x <= left + long(width)  &&  c.y >= top && c.y <= top + long(height);
}

}

