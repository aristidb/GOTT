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
 * Portions created by the Initial Developer are Copyright (C) 2006
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

#include <iostream>
#include <cmath>
#include <gott/graphics/transformations.hpp>

namespace g = gott::graphics;

double const pi = 4 * std::atan(1.0);
double maximum_error = 0;

bool check(double isx, double isy, double expx, double expy, int test) {
  std::cout << isx << ' ' << isy << std::endl;
  double const epsilon = 1.0E-13;
  double dx = std::fabs(isx - expx);
  double dy = std::fabs(isy - expy);
  maximum_error = std::max(maximum_error, dx);
  maximum_error = std::max(maximum_error, dy);
  if (dx < epsilon && dy < epsilon)
    return true;
  std::cerr << "Test failed: " << test << '\n';
  std::cerr << "Error x: " << dx << '\n';
  std::cerr << "Error y: " << dy << '\n';
  return false;
}

int main() {
  bool happy = true;

  g::point p(7, -99);

  g::transformations t;
  p = t.apply(p);
  happy &= check(p.x, p.y, 7, -99, 1);

  p = g::translate(5, 5).apply(p);
  happy &= check(p.x, p.y, 12, -94, 2);

  g::scale(2, -0.2).apply_to(p);
  happy &= check(p.x, p.y, 24, 18.8, 3);

  g::transformations t2 = g::rotate(pi);
  t2.apply_to(p);
  happy &= check(p.x, p.y, -24, -18.8, 4);

  g::translate(25, 19.8).scale(8, 16).apply_to(p);
  happy &= check(p.x, p.y, 8, 16, 5);

  g::scale(1.0 / 8, 1.0 / 16).rotate(pi / 2).apply_to(p);
  happy &= check(p.x, p.y, -1.0, 1.0, 6);

  g::skew_y(-pi / 4).apply_to(p);
  happy &= check(p.x, p.y, -1.0, 2.0, 7);

  g::skew_y(pi / 4).skew_x(pi / 4).apply_to(p);
  happy &= check(p.x, p.y, 0, 1.0, 8);

  std::cout << "\nMaximum error: " << maximum_error << std::endl;

  return happy ? 0 : 1;
}
