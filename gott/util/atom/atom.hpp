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

#include <gott/util/nstring/nstring.hpp>

namespace gott {

class atom : public nstring {
public:
  GOTT_EXPORT atom(atom const &);
  GOTT_EXPORT atom(nstring const &);
  GOTT_EXPORT ~atom();
};

GOTT_EXPORT bool operator==(atom const &, atom const &);

inline bool operator!=(atom const &a, atom const &b) {
  return !(a == b);
}

}

#define PARAM_NO_BASIC_STRING
#include <gott/util/my_hash_map.hpp>
#undef PARAM_NO_BASIC_STRING

namespace HH_HASHD {
template<> struct hash<gott::atom> {
  std::size_t operator() (gott::atom const &) const;
};
}

#endif