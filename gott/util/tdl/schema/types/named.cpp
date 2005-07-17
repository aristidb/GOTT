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

#include "named.hpp"
#include "../event.hpp"

using std::list;
using std::wstring;
using gott::util::xany::Xany;
using gott::util::xany::Xany_cast;

namespace schema = gott::util::tdl::schema;
namespace ev = gott::util::tdl::schema::ev;
using schema::rule;
using schema::match_named;

rule::attributes match_named::attributes(wstring const &s, bool cc) {
  return rule::attributes(std::list<wstring>(1, s), cc, s);
}

match_named::match_named(rule::factory const &s, slotcfg const &c,
                         rule::attributes const &a, match &m) 
: rule(need, a, m), sub(s), optional(c.get_mode() == slotcfg::optional),
state(read_none) {
  if (a.user().type() != typeid(wstring))
    throw std::bad_exception();
}

bool match_named::play(ev::node const &n) {
  if (state != read_none)
    return false;

  if (n.get_data() == Xany_cast<wstring>(get_attributes().user())) {
    state = read_node;
    if (optional) {
      expectation = maybe;
      // WE ARE OPTIONAL, MIGHT HAVE TO RECOVER THE INCOMING up EVENT
    }
    return true;
  }
  
  return false;
}

bool match_named::play(ev::down const &) {
  if (state == read_node) {
    state = read_down;
    matcher().add(sub);
    return true;
  }
  return false;
}

bool match_named::play(ev::child_succeed const &) {
  state = read_sub;
  if (optional) {
    // TODO? WE NEED NO LONGER GO BACK
  }
  return true;
}

bool match_named::play(ev::child_fail const &) {
  if (optional) {
    // TODO? GO BACK
    return true;
  }
  return false;
}

bool match_named::play(ev::up const &) {
  if (state == read_sub) {
    state = read_up;
    expectation = nothing;
    return true;
  }
  return false;
}

wchar_t const *match_named::name() const {
  return L"named";
}
