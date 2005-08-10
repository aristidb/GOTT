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

#include "named.hpp"
#include "../event.hpp"
#include <gott/util/nstring/nstring.hpp>
#include <list>

using std::list;
using std::wstring;
using gott::xany::Xany;
using gott::xany::Xany_cast;

namespace schema = gott::tdl::schema;
namespace ev = gott::tdl::schema::ev;
using schema::rule;
using schema::rule_attr;
using schema::match_named;

rule_attr match_named::attributes(wstring const &s, bool cc) {
  return rule_attr(std::list<wstring>(1, s), cc, Xany(s));
}

match_named::match_named(rule_factory const &s, rule_attr const &a, 
                         match &m) 
: rule(a, m), sub(s), state(read_none) {
  if (!a.user().compatible<nstring>())
    throw std::bad_exception();
}

bool match_named::play(ev::node const &n) {
  if (state != read_none)
    return false;

  if (n.get_data() == Xany_cast<nstring>(rule::attributes().user())) {
    state = read_node;
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
  return true;
}

bool match_named::play(ev::child_fail const &) {
  return false;
}

bool match_named::play(ev::up const &) {
  if (state == read_sub) {
    state = done;
    return true;
  }
  return false;
}

rule::expect match_named::expectation() const {
  return state == done ? nothing : need;
}

wchar_t const *match_named::name() const {
  return L"named";
}
