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

namespace schema = gott::tdl::schema;
namespace ev = gott::tdl::schema::ev;
using schema::rule;
using schema::match_follow_ordered;

match_follow_ordered::match_follow_ordered(Vector<element> const &c, 
    rule_attr const &a, match &m)
: rule(a, m), opened(0), saw_up(false), last(m.pos().current()), 
    unhappy(false) {
  for (Vector<element>::const_iterator it = c.begin(); it != c.end(); ++it)
    children.Add(*it);
  pos = children.begin();
  init_accept_empty();
  if (expectation() == nothing)
    return;
  last = matcher().pos().current();
  matcher().add(*pos->generator);
}

void match_follow_ordered::init_accept_empty() {
  bool rest_accept_empty = true;
  for (int i = children.GetCount() - 1; i >= 0; --i) {
    active_element &e = children[i];
    e.rest_accept_empty = rest_accept_empty;
    e.accept_empty = e.generator->accept_empty();
    rest_accept_empty &= (e.slot.prefix_optional() || e.accept_empty);
  }
}

match_follow_ordered::~match_follow_ordered() {
  matcher().pos().forget(last);
}

bool match_follow_ordered::play(ev::child_succeed const &) {
  if (matcher().pos().proceeded(last)) {
    pos->slot.add();
  } else {
    ++pos; // don't repeat empty
    if (!search_insertible())
      return true;
    matcher().pos().forget(last);
    last = matcher().pos().current();
    matcher().add(*pos->generator);
  }
  return true;
}

bool match_follow_ordered::play(ev::child_fail const &) {
  bool this_flee = !pos->accept_empty;
  unhappy |= this_flee && pos->slot.expectation() == need;
  ++pos; // Skip the undoable
  if (!search_insertible()) {
    if (!this_flee)
      return true;
    matcher().pos().seek(last);
    return false;
  }
  matcher().pos().seek(last);
  matcher().add(*pos->generator);
  return true;
}

bool match_follow_ordered::play(ev::down const &) {
  if (saw_up || !search_insertible())
    return false;
  ++opened;
  matcher().pos().forget(last);
  last = matcher().pos().current();
  matcher().add(*pos->generator);
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

rule::expect match_follow_ordered::expectation() const {
  if (unhappy) 
    return need;
  if (opened != 0) 
    return need;
  if (!search_insertible()) 
    return nothing;
  if (pos->rest_accept_empty)
    if (pos->slot.expectation() != need || pos->accept_empty) 
      return maybe;
  return need;
}

bool match_follow_ordered::accept_empty(Vector<element> const &children) {
  bool accept = true;
  for (Vector<element>::const_iterator it = children.begin(); 
       it != children.end(); ++it)
    accept &= it->second.prefix_optional() || it->first->accept_empty();
  return accept;
}

wchar_t const *match_follow_ordered::name() const {
  return L"follow";
}
