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

#include "follow_ordered.hpp"
#include "../event.hpp"

namespace schema = gott::tdl::schema;
namespace ev = gott::tdl::schema::ev;
using schema::item;
using schema::match_follow_ordered;

match_follow_ordered::match_follow_ordered(rule_attr_t const &a, 
                                           std::vector<rule_t> const &c, 
                                           match &m)
: item(a, m), opened(0), saw_up(false), last(m.pos().current()), 
    unhappy(false) {
  for (std::vector<rule_t>::const_iterator it = c.begin(); it != c.end(); ++it)
    children.push_back(*it);
  pos = children.begin();
  init_accept_empty();
  if (expectation() == nothing)
    return;
  last = matcher().pos().current();
  matcher().add(pos->generator);
}

void match_follow_ordered::init_accept_empty() {
  bool rest_accept_empty = true;
  for (int i = children.size() - 1; i >= 0; --i) {
    active_element &e = children[i];
    e.rest_accept_empty = rest_accept_empty;
    e.accept_empty = e.generator.accept_empty();
    rest_accept_empty &= (e.slot.prefix_optional() || e.accept_empty);
  }
}

bool match_follow_ordered::play(ev::child_succeed const &) {
  if (matcher().pos().proceeded(last)) {
    pos->slot.add();
    matcher().pos().forget(last);
    last = matcher().pos().current();
  } else {
    ++pos; // don't repeat empty
    if (!search_insertible())
      return true;
    matcher().pos().forget(last);
    last = matcher().pos().current();
    matcher().add(pos->generator);
  }
  return true;
}

bool match_follow_ordered::play(ev::child_fail const &) {
  bool this_flee = !pos->accept_empty;
  unhappy |= this_flee && pos->slot.expectation() == need;
  ++pos;
  if (unhappy || !search_insertible()) {
    if (!this_flee)
      return true;
    matcher().pos().seek(last);
    return false;
  }
  matcher().pos().seek(last);
  if (--opened < 0) {
    opened = 0;
    matcher().add(pos->generator);
  }
  return true;
}

bool match_follow_ordered::play(ev::down const &) {
  if (saw_up || !search_insertible())
    return false;
  ++opened;
  matcher().add(pos->generator);
  return true;
}

bool match_follow_ordered::play(ev::up const &) {
  saw_up = true;
  if (--opened < 0) {
    opened = 0;
    return false;
  }
  return true;
}

bool match_follow_ordered::search_insertible() const {
  while (pos != children.end() && !accept_more(pos->slot.expectation()))
    ++pos;
  return pos != children.end();
}

item::expect match_follow_ordered::expectation() const {
  if (unhappy) 
    return need;
  if (!search_insertible()) 
    return opened != 0 ? maybe : nothing;
  if (pos->rest_accept_empty)
    if (pos->slot.expectation() != need || pos->accept_empty) 
      return maybe;
  return need;
}

bool match_follow_ordered::accept_empty(rule_attr_t const &,
                                        std::vector<rule_t> const &children) {
  bool accept = true;
  for (std::vector<rule_t>::const_iterator it = children.begin(); 
       it != children.end(); ++it)
    accept &= it->attributes().outer().prefix_optional() || it->accept_empty();
  return accept;
}

gott::string match_follow_ordered::name() const {
  return "follow";
}

match_follow_ordered::~match_follow_ordered() {
  matcher().pos().forget(last);
  if (expectation() != need && opened > 0) 
    matcher().parental_requirement(ev::up(), opened);
}

bool match_follow_ordered::miss_events(ev::event const &event, unsigned count) {
  if (typeid(event) != typeid(ev::up))
    return false;
  opened += count;
  return true;
}
