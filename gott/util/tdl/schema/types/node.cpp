// Copyright (C) 2004-2005 by Aristid Breitkreuz (aribrei@arcor.de)
// Content: TDL Schema engine
// Authors: Aristid Breitkreuz
//
// This File is part of the Gott Project (http://gott.sf.net)
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

namespace schema = gott::util::tdl::schema;
namespace ev = gott::util::tdl::schema::ev;
using schema::rule;
using schema::match_node;

match_node::match_node(rule::attributes const &a, match &m)
: happy_once(a, m) {}

bool match_node::play(ev::node const &n) {
  if (expectation() == need) {
    matcher().direct_structure().data(gott::util::xany::Xany(n.get_data()));

    be_happy();
    return true;
  }

  return false;
}

wchar_t const *match_node::name() const {
  return L"node";
}
