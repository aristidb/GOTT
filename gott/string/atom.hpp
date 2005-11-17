// Copyright (C) 2005 by Aristid Breitkreuz (aribrei@arcor.de)
// Content: Text storage and manipulation
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

#ifndef GOTT_UTIL_ATOM_ATOM_HPP
#define GOTT_UTIL_ATOM_ATOM_HPP

#include <gott/string/string.hpp>

namespace gott {

/**
 * Atom class.
 * Atoms are strings with fast comparison and fast hashing but slow constructor.
 */
class atom : public string, Moveable<atom> {
public:
  /// Copy-Constructor (fast).
  GOTT_EXPORT atom(atom const &);

  /// Constructor (slow).
  GOTT_EXPORT atom(string const &);

  /// Destructor.
  GOTT_EXPORT ~atom();
};

/**
 * Compare two atoms for equality (fast).
 */
GOTT_EXPORT bool operator==(atom const &, atom const &);

/**
 * Compare two atoms for inequality (fast).
 */
inline bool operator!=(atom const &a, atom const &b) {
  return !(a == b);
}

}

template<>
inline unsigned GetHashValue(gott::atom const &a) {
  return GetHashValue(a.as_utf8().begin);
}

#endif
