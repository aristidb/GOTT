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

#include "atom.hpp"
#include <gott/util/my_hash_map.hpp>
#include HH_HASH_SET

using gott::atom;
using gott::nstring;

static nstring atomize(nstring const &n) {
  static hashd::hash_set<nstring> table;
  return *table.insert(n).first;
}

atom::atom(nstring const &n) : nstring(atomize(n)) {}

atom::atom(atom const &o) : nstring(o) {}

atom::~atom() {}

bool gott::operator==(atom const &a, atom const &b) {
  return a.begin_raw() == b.begin_raw(); // same memory
}

std::size_t hashd::hash<atom>::operator() (atom const &a) const {
  return std::size_t(a.begin_raw());
}
