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
 * The Original Code is A Non-immediate Graphics Library.
 *
 * The Initial Developer of the Original Code is
 * Aristid Breitkreuz (aribrei@arcor.de).
 * Portions created by the Initial Developer are Copyright (C) 2005-2006
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
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

  /// Assignment.
  transformations &operator=(transformations x) {
    x.swap(*this);
    return *this;
  }

  /// Swapping.
  GOTT_EXPORT void swap(transformations &);
  
  /**
   * Rotate.
   * \param rot Rotation (counter-clockwise) in radian.
   */
  GOTT_EXPORT transformations &rotate(double rot);

  /**
   * Translate.
   * \param dx Shift to the right.
   * \param dy Shift upwards.
   */
 GOTT_EXPORT transformations &translate(double dx, double dy);

  /**
   * Scale. 1.0 means no resize.
   * \param rx Ratio in x direction.
   * \param ry Ratio in y direction.
   */
  GOTT_EXPORT transformations &scale(double rx, double ry);

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
