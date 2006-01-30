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

#include <gott/visibility.hpp>
#include <boost/numeric/ublas/matrix.hpp>

namespace gott {
namespace graphics {

/**
 * Transformations that can be applied to any graphics::object.
 */
class transformations {
public:
  /// Default-Constructor.
  GOTT_EXPORT transformations();

  /// Copy-Constructor.
  GOTT_EXPORT transformations(transformations const &);

  /// Destructor.
  GOTT_EXPORT ~transformations();
  
  /**
   * Rotate.
   * \param rot Rotation (counter-clockwise) in radian.
   */
  GOTT_EXPORT void rotate(double rot);

  /**
   * Translate.
   * \param dx Shift to the right.
   * \param dy Shift upwards.
   */
 GOTT_EXPORT void translate(double dx, double dy);

  /**
   * Scale. 1.0 means no resize.
   * \param rx Ratio in x direction.
   * \param ry Ratio in y direction.
   */
  GOTT_EXPORT void scale(double rx, double ry);

  /**
   * Apply all transformations to the point (x,y).
   * \param x [inout] The x-coordinate of the point.
   * \param y [inout] The y-coordinate of the point.
   */
  GOTT_EXPORT void apply(double &x, double &y);

  GOTT_EXPORT static transformations const identity;

private:
  typedef
    boost::numeric::ublas::matrix<
      double,
      boost::numeric::ublas::row_major,
      boost::numeric::ublas::bounded_array<double, 9>
    >
    matrix_t;
  matrix_t mtx;
};

/**
 * Rotate.
 * \see transformations::rotate
 */
inline 
transformations rotate(double rot, transformations t = transformations()) {
  t.rotate(rot);
  return t;
}

/**
 * Translate.
 * \see transformations::translate
 */
inline transformations translate(double dx, double dy, 
    transformations t = transformations()) {
  t.translate(dx, dy);
  return t;
}

/**
 * Scale.
 * \see transformations::scale
 */
inline transformations scale(double rx, double ry,
    transformations t = transformations()) {
  t.scale(rx, ry);
  return t;
}

}}

#endif
