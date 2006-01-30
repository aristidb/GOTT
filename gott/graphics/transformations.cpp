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

void transformations::rotate(double phi) {
  double sin_phi = std::sin(phi);
  double cos_phi = std::cos(phi);
  matrix_t helper(math::identity_matrix<double>(3));
  helper(0, 0) = cos_phi;  helper(0, 1) = sin_phi;
  helper(1, 0) = -sin_phi; helper(1, 1) = cos_phi;
  mtx = prod(mtx, helper);
}

void transformations::translate(double dx, double dy) {
  matrix_t helper(math::identity_matrix<double>(3));
  helper(2, 0) = dx; helper(2, 1) = dy;
  mtx = prod(mtx, helper);
}

void transformations::scale(double sx, double sy) {
  matrix_t helper(math::identity_matrix<double>(3));
  helper(0, 0) = sx; helper(1, 1) = sy;
  mtx = prod(mtx, helper);
}

void transformations::apply(double &x, double &y) {
  math::vector<double, math::bounded_array<double, 3> > input(3), output(3);
  input(0) = x; input(1) = y; input(2) = 1;
  output = prod(input, mtx);
  x = output(0); y = output(1);
}
