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

#include "node.hpp"
#include "../event.hpp"
#include <gott/debug/assert.hpp>

namespace schema = gott::tdl::schema;
namespace ev = gott::tdl::schema::ev;
using schema::item;
using schema::match_node;

match_node::match_node(rule_attr_t const &a, std::vector<rule_t> const &r, 
    match &m)
: happy_once(a, m) {
  GOTT_ASSERT_2(r.size(), 0, std::equal_to<int>(), "no children");
}

bool match_node::play(ev::node const &n) {
  if (expectation() != need)
    return false;

  matcher().direct_structure().data(gott::xany::Xany(n.get_data()));
  be_happy();
  return true;
}

gott::string match_node::name() const {
  return "node";
}
