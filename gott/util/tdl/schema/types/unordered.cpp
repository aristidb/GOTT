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

#include "unordered.hpp"

using std::vector;

namespace schema = gott::util::tdl::schema;
namespace ev = gott::util::tdl::schema::ev;
using schema::rule;
using schema::match_unordered;

match_unordered::match_unordered(vector<rule::factory const *> const &r, 
                                 rule::attributes const &a, match &m) 
: rule(need, a, m), last(m.pos().current()) {
  for (size_t i = 0; i < r.size(); ++i)
    children.push_back(std::make_pair(r[i], slotcfg::one));

  pos = children.begin();

  if (pos != children.end())
    matcher().add(*pos->first);
  else
    expectation = nothing;
}

match_unordered::~match_unordered() {
  matcher().pos().forget(last);
}

bool match_unordered::play(ev::child_succeed const &) {
  pos->second.add();
  if (pos->second.expectation() == rule::nothing) 
    children.erase(pos);

  if (children.empty())
    expectation = nothing;
  else {
    pos = children.begin();
    matcher().pos().forget(last);
    last = matcher().pos().current();
    matcher().add(*pos->first);
  }

  return true;
}

bool match_unordered::play(ev::child_fail const &) {
  matcher().pos().seek(last);

  if (++pos == children.end())
    return false;

  matcher().add(*pos->first);

  return true;
}
