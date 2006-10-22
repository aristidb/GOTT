// vim:ts=2:sw=2:expandtab:autoindent:filetype=cpp:
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

#include "transformations.hpp"
#include <algorithm>
#include <cmath>

namespace graphics = gott::graphics;
using graphics::transformations;

namespace math = boost::numeric::ublas;

transformations const transformations::identity;

transformations::transformations() 
: mtx(math::identity_matrix<number>(3)) {}
transformations::transformations(transformations const &o) : mtx(o.mtx) {}
transformations::~transformations() {}

transformations::transformations(number a, number b, number c, number d, 
    number t_x, number t_y)
: mtx(math::identity_matrix<number>(3)) {
  matrix(a, b, c, d, t_x, t_y);
}

void transformations::swap(transformations &x) {
  mtx.swap(x.mtx);
}

transformations &transformations::rotate(angle phi) {
  number sin_phi = std::sin(phi);
  number cos_phi = std::cos(phi);
  return matrix(cos_phi, sin_phi, -sin_phi, cos_phi, 0, 0);
}

transformations &transformations::translate(distance dx, distance dy) {
  return matrix(1, 0, 0, 1, dx, dy);
}

transformations &transformations::scale(number sx, number sy) {
  return matrix(sx, 0, 0, sy, 0, 0);
}

transformations &transformations::skew_x(angle a) {
  return matrix(1, 0, std::tan(a), 1, 0, 0);
}

transformations &transformations::skew_y(angle a) {
  return matrix(1, std::tan(a), 0, 1, 0, 0);
}

transformations &transformations::matrix(number a, number b, number c, 
    number d, number t_x, number t_y) {
  matrix_t helper(math::identity_matrix<number>(3));
  helper(0, 0) = a;   helper(0, 1) = b;
  helper(1, 0) = c;   helper(1, 1) = d;
  helper(2, 0) = t_x; helper(2, 1) = t_y;
  mtx = math::prod(mtx, helper);
  return *this;
}

transformations &transformations::append(transformations const &o) {
  mtx = math::prod(mtx, o.mtx);
  return *this;
}

transformations &transformations::prepend(transformations const &o) {
  mtx = math::prod(o.mtx, mtx);
  return *this;
}

graphics::point transformations::apply(graphics::point const &p) {
  math::vector<number, math::bounded_array<number, 3> > input(3), output(3);
  input(0) = p.x; input(1) = p.y; input(2) = 1;
  output = math::prod(input, mtx);
  return graphics::point(output(0), output(1));
}
