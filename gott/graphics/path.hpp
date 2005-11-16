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

#ifndef GOTT_GRAPHICS_PATH_HPP
#define GOTT_GRAPHICS_PATH_HPP

#include <gott/util/visibility.hpp>
#include "point.hpp"

namespace gott {

class string;
class font;

namespace graphics {

/**
 * A graphical path.
 */
class path {
public:
  /// Empty path.
  GOTT_EXPORT path();

  /// Destructor.
  GOTT_EXPORT ~path();

  /// Copy-Constructor.
  GOTT_EXPORT path(path const &);
  
  /**
   * Append other path.
   * \param o Path to be appended.
   */
  GOTT_EXPORT path &operator +=(path const &o);

  /**
   * Adds a circular arc. Positional version. Connects the current point with 
   * start of the arc with a line. The new current point will be the end of
   * the arc.
   * \param center The coordinates of the center of the arc.
   * \param radius The radius of the arc.
   * \param angle1 The starting angle of the arc.
   * \param angle2 The stopping angle of the arc. If angle2 < angle1, angle2 
   *               will be increased by 2 * PI until angle2 >= angle1.
   */
  GOTT_EXPORT void arc_p(point const &center, double radius, 
      double angle1, double angle2);

  /**
   * Adds a circular arc in reverse direction. Positional version.
   * \see arc_p
   * \param center The coordinates of the center of the arc.
   * \param radius The radius of the arc.
   * \param angle1 The starting angle of the arc.
   * \param angle2 The stopping angle of the arc. If angle2 > angle1, angle2 
   *               will be decreased by 2 * PI until angle2 <= angle1.
   */
  GOTT_EXPORT void arc_negative_p(point const &center, double radius, 
      double angle1, double angle2);

  /**
   * Add a cubic Bezier spline to the path. Positional version.
   * Starts at the current point. The end point will be the new current point.
   * \param c1 The first control point.
   * \param c2 The scond control point.
   * \param e The end point.
   */
  GOTT_EXPORT void curve_to_p(point const &c1, point const &c2, point const &e);

  /**
   * Adds a line from the old current point to the new.
   * \param p The new current point.
   */
  GOTT_EXPORT void line_to(point const &p);

  /**
   * Change the current point. This will create a new subpath if necessary.
   * \param p The new current point.
   */
  GOTT_EXPORT void move_to(point const &p);

  /**
   * Add the outlines of a text to the path.
   * \param txt The text to get the outline from.
   * \param style The text's font.
   */
  GOTT_EXPORT void text(string const &txt, font const &style);

  // GOTT_EXPORT void glyphs(...);
};

}}

#endif
