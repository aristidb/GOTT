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

#ifndef GOTT_GRAPHICS_TRANSFORMATIONS_HPP
#define GOTT_GRAPHICS_TRANSFORMATIONS_HPP

#include <gott/util/visibility.hpp>

namespace gott {
namespace graphics {

/**
 * Transformations that can be applied to any graphics::object.
 * \todo Please check if this is scientifically fine.
 */
class transformations {
  /**
   * Rotate.
   * \todo Select rotation unit (degree, radian).
   */
  GOTT_EXPORT void rotate(double rot);

  /**
   * Translate.
   * \todo Use double for this?
   */
 GOTT_EXPORT void translate(double dx, double dy);

  /**
   * Scale.
   * \todo Unit?
   */
  GOTT_EXPORT void scale(double rx, double ry);
};

/**
 * Rotate.
 * \see transformations::rotate
 */
inline transformations rotate(transformations t, double rot) {
  t.rotate(rot);
  return t;
}

/**
 * Translate.
 * \see transformations::translate
 */
inline transformations translate(transformations t, double dx, double dy) {
  t.translate(dx, dy);
  return t;
}

/**
 * Scale.
 * \see transformations::scale
 */
inline transformations scale(transformations t, double rx, double ry) {
  t.scale(rx, ry);
  return t;
}

}}

#endif
