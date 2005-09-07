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

#include "unordered.hpp"
#include <gott/util/range_algo.hpp>

namespace schema = gott::tdl::schema;
namespace ev = gott::tdl::schema::ev;
using schema::item;
using schema::match_unordered;

match_unordered::match_unordered(Vector<element> const &r, 
                                 rule_attr const &a, match &m) 
: item(a, m), last(m.pos().current()), all_happy(true) {
  copy(range(r), std::back_inserter(children));

  pos = children.begin();

  if (pos != children.end())
    matcher().add(*pos->first);
}

match_unordered::~match_unordered() {
  matcher().pos().forget(last);
}

bool match_unordered::play(ev::child_succeed const &) {
  pos->second.add();
  if (pos->second.expectation() == item::nothing) 
    children.Remove(pos - children.begin());

  if (!children.IsEmpty()) {
    pos = children.begin();
    matcher().pos().forget(last);
    last = matcher().pos().current();
    matcher().add(*pos->first);
  }

  return true;
}

bool match_unordered::play(ev::child_fail const &) {
  matcher().pos().seek(last);
  bool happy = pos->second.expectation() != need;
  if (++pos == children.end())
    if (happy) {
      children.clear();
      return true;
    } else {
      all_happy = false;
      return false;
    }
  matcher().add(*pos->first);
  return true;
}

item::expect match_unordered::expectation() const {
  if (children.IsEmpty() && all_happy)
    return nothing;
  return need;
}

bool match_unordered::accept_empty(Vector<element> const &children) {
  bool accept = true;
  for (Vector<element>::const_iterator it = children.begin(); 
       it != children.end(); ++it)
    ;//FIXME accept &= it->second.prefix_optional() || it->first->accept_empty();
  return accept;
}

gott::string match_unordered::name() const {
  return L"unordered";
}
