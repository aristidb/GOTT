// Copyright (C) 2006 by Aristid Breitkreuz (aribrei@arcor.de)
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

#include "tree.hpp"
#include "../event.hpp"
#include <gott/debug/assert.hpp>

using gott::tdl::schema::match_tree;
using gott::tdl::schema::item;
namespace ev = gott::tdl::schema::ev;

match_tree::match_tree(rule_attr_t const &a, std::vector<rule_t> const &c, match &m)
: item(a, m), level(0), fresh_level(true) {
  GOTT_ASSERT_2(c.size(), 0, std::equal_to<int>(), "no children");
}

gott::string match_tree::name() const {
  return "tree";
}

item::expect match_tree::expectation() const {
  if (level == 0)
    switch (level_state) {
    case fresh:
      return need;
    case titled:
      return maybe;
    case used: default:
      return nothing;
    }
  else
    return need;
}

bool match_tree::play(ev::node const &n) {
  if (expectation() == nothing)
    return false;
  if (level > 0 && level_state == titled) {
    matcher().direct_structure().end();
    matcher().direct_structure().begin();
  }
  matcher().direct_structure().data(gott::xany::Xany(n.get_data()));
  level_state = titled;
  return true;
}

bool match_tree::play(ev::down const &) {
  ++level;
  level_state = fresh;
  matcher().direct_structure().begin();
  return true;
}

bool match_tree::play(ev::up const &) {
  if (level == 0)
    return false;
  --level;
  level_state = used;
  matcher().direct_structure().end();
  return true;
}
