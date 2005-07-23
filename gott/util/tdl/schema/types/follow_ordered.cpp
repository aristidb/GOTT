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
: rule(need, a, m), children(c.begin(), c.end()), pos(children.begin()), 
    opened(0), saw_up(false), last(m.pos().current()) {
  init_rest_accept_empty();
  adjust_expectation();
  if (expectation == nothing)
    return;
  last = matcher().pos().current();
  matcher().add(*pos->generator);
}

void match_follow_ordered::init_rest_accept_empty() {
  bool rest_accept_empty = true;
  for (container::reverse_iterator it = children.rbegin(); 
       it != children.rend(); ++it) {
    it->rest_accept_empty = rest_accept_empty;
    rest_accept_empty &= it->generator->accept_empty();
  }
}

match_follow_ordered::~match_follow_ordered() {}

bool match_follow_ordered::play(ev::child_succeed const &) {
  if (matcher().pos().proceeded(last)) {
    pos->slot.add();
    adjust_expectation();
  } else if (search_insertible()) {
    last = matcher().pos().current();
    matcher().add(*pos->generator);
  } else
    return false;
  return true;
}

bool match_follow_ordered::play(ev::child_fail const &) {
  ++pos; // Skip the undoable
  if (!search_insertible())
    return false;
  matcher().pos().seek(last);
  matcher().add(*pos->generator);
  adjust_expectation();
  return true;
}

bool match_follow_ordered::play(ev::down const &) {
  ++opened;
  adjust_expectation();
  if (search_insertible()) {
    last = matcher().pos().current();
    matcher().add(*pos->generator);
    return true;
  } else 
    return false;
}

bool match_follow_ordered::play(ev::up const &) {
  saw_up = true;
  --opened;
  adjust_expectation();
  return opened >= 0;
}

bool match_follow_ordered::search_insertible() {
  while (pos != children.end() && !accept_more(pos->slot.expectation()))
    ++pos;
  return pos != children.end();
}

void match_follow_ordered::adjust_expectation() {
  if (opened > 0)
    expectation = need;
  else if (!search_insertible())
    expectation = nothing;
  else if (pos->slot.expectation() != need && pos->rest_accept_empty)
    expectation = maybe;
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
