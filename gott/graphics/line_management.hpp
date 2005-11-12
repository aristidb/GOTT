// Copyright (C) 2005 by Aristid Breitkreuz (aribrei@arcor.de)
// Content: Non-immediate graphics library
// Authors: Aristid Breitkreuz
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

#ifndef GOTT_GRAPHICS_LINE_MANAGEMENT_HPP
#define GOTT_GRAPHICS_LINE_MANAGEMENT_HPP

namespace gott {
namespace graphics {

/**
 * How a line is drawn.
 */
class line_management {
public:
  double line_width;

  enum join {
    join_miter,
    join_round,
    join_bevel
  };

  /// Style of line-endings.
  enum cap {
    /// End exactly at the end point.
    cap_butt,
    /// Round ending with the center of the circle being the end point.
    cap_round,
    // Square ending with the center of the square being the end point.
    cap_square
  };

  // missing: dash
};

}}

#endif