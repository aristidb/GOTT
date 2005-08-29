// Copyright (C) 2004-2005 by Aristid Breitkreuz (aribrei@arcor.de)
// Content: TDL Schema engine
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

#include "by_name.hpp"
#include "../exceptions.hpp"
#include <gott/util/string/string.hpp>

namespace tdl = gott::tdl;
namespace schema = tdl::schema;
using schema::name_manager_t;

name_manager_t &schema::name_manager() {
  static name_manager_t m;
  return m;
}

class name_manager_t::IMPL {
public:
  typedef VectorMap<string, unsigned> mapping; 
  mapping items;
};

name_manager_t::name_manager_t() : p(new IMPL) {
}

name_manager_t::~name_manager_t() {
}

void name_manager_t::add(string const &name, unsigned id) {
  p->items.Add(name, id);
}

unsigned name_manager_t::get(string const &name) const {
  int i = p->items.Find(name);
  if (i < 0)
    throw unregistered_type(name);
  return p->items[i];
}
