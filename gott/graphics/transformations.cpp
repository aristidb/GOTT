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
: mtx(math::identity_matrix<double>(3)) {}
transformations::transformations(transformations const &o) : mtx(o.mtx) {}
transformations::~transformations() {}

void transformations::swap(transformations &x) {
  mtx.swap(x.mtx);
}

transformations &transformations::rotate(double phi) {
  double sin_phi = std::sin(phi);
  double cos_phi = std::cos(phi);
  matrix_t helper(math::identity_matrix<double>(3));
  helper(0, 0) = cos_phi;  helper(0, 1) = sin_phi;
  helper(1, 0) = -sin_phi; helper(1, 1) = cos_phi;
  mtx = prod(mtx, helper);
  return *this;
}

transformations &transformations::translate(double dx, double dy) {
  matrix_t helper(math::identity_matrix<double>(3));
  helper(2, 0) = dx; helper(2, 1) = dy;
  mtx = prod(mtx, helper);
  return *this;
}

transformations &transformations::scale(double sx, double sy) {
  matrix_t helper(math::identity_matrix<double>(3));
  helper(0, 0) = sx; helper(1, 1) = sy;
  mtx = prod(mtx, helper);
  return *this;
}

void transformations::apply(double &x, double &y) {
  math::vector<double, math::bounded_array<double, 3> > input(3), output(3);
  input(0) = x; input(1) = y; input(2) = 1;
  output = prod(input, mtx);
  x = output(0); y = output(1);
}
