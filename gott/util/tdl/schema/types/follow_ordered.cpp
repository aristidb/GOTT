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

#include "follow_ordered.hpp"

using std::vector;

namespace schema = gott::util::tdl::schema;
namespace ev = gott::util::tdl::schema::ev;
using schema::rule;
using schema::match_follow_ordered;

match_follow_ordered::match_follow_ordered(std::vector<element> const &c, 
    rule::attributes const &a, match &m)
: rule(need, a, m), children(c.begin(), c.end()), 
    pos(children.begin()), opened(0), state(downwards) {
  init_rest_accept_empty();
  if (pos == children.end() || !update()) {
    expectation = nothing;
    return;
  }
  pos->before = matcher().pos().current();
  matcher().add(*pos->generator);
}

void match_follow_ordered::init_rest_accept_empty() {
  bool rest_accept_empty = true;
  for (container::reverse_iterator it = children.rbegin(); 
       it != children.rend(); ++it)
    it->rest_accept_empty = (rest_accept_empty &= it->accept_empty);
}

match_follow_ordered::~match_follow_ordered() {}

bool match_follow_ordered::play(ev::child_succeed const &) {
  if (matcher().pos().proceeded(pos->before)) {
    pos->slot.add();
    if (pos->rest_accept_empty)
      expectation = maybe;
    if (!update())
      state = upwards;
    return true;
  } else {
    matcher().pos().seek(pos->before);
    opened = pos->opened;
    if (pos->slot.expectation() != need) {
      ++pos;
      update();
    }
    return true;
  }
}

bool match_follow_ordered::play(ev::down const &) {
  ++opened;
  if (state == downwards && update()) {
    pos->opened = opened;
    pos->before = matcher().pos().current();
    matcher().add(*pos->generator);
    return true;
  } else 
    return false;
}

bool match_follow_ordered::play(ev::up const &) {
  if (state != upwards)
    return false;
  if (--opened == 0)
    expectation = nothing;
  return true;
}

bool match_follow_ordered::update() {
  if (!accept_more(pos->slot.expectation()))
    ++pos;
  return pos != children.end();
}

bool match_follow_ordered::accept_empty(vector<element> const &children) {
  bool accept = true;
  for (vector<element>::const_iterator it = children.begin(); 
       it != children.end(); ++it)
    accept &= it->second.prefix_optional() || it->first->accept_empty();
  return accept;
}

wchar_t const *match_follow_ordered::name() const {
//  return L"follow_ordered";
  return L"follow";
}