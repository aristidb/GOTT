// GOTT utilities
//
// Copyright (C) 2005 Aristid Breitkreuz
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

#include "xany.hpp"

using namespace std;
using namespace gott::xany;

operations_base::~operations_base() {}

bool gott::xany::operator==(Xany const &lhs, Xany const &rhs) {
  if (lhs.empty() || rhs.empty())
    return lhs.empty() && rhs.empty();
  if (lhs.type() != rhs.type())
    return false;
  return dynamic_cast<equality_comparable const &>(lhs.get_operations())
          .equals(lhs, rhs);
}

bool gott::xany::operator!=(Xany const &lhs, Xany const &rhs) {
  return !(lhs == rhs);
}

printable::~printable() {}

equality_comparable::~equality_comparable() {}
