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

using gott::atom;
using gott::string;

static string atomize(string const &n) {
  static Index<string> table;
  int pos = table.FindAdd(n);
  return table[pos];
}

atom::atom(string const &n) : string(atomize(n)) {}

atom::atom(atom const &o) : string(o) {}

atom::~atom() {}

bool gott::operator==(atom const &a, atom const &b) {
  return a.as_utf8().begin() == b.as_utf8().begin(); // same memory
}

