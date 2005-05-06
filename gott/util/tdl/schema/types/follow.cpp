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

#include "follow.hpp"

namespace schema = gott::util::tdl::schema;
namespace ev = gott::util::tdl::schema::ev;
using schema::rule;
using schema::match_follow;

match_follow::match_follow(rule::factory const * const a[2], 
                           rule::attributes const &at, match &m) 
: rule(need, at, m), pre(*a[0]), post(*a[1]), state(initial), 
    first_empty(false) {
  matcher().add(pre);
}

bool match_follow::play(ev::child_succeed const &) {
  if (state == initial)
    state = pre_parsed;
  else
    post_done();
  return true;
}

void match_follow::post_done() {
  if (first_empty) {
    state = none;
    expectation = nothing;
  } else
    state = post_parsed;
}

bool match_follow::play(ev::down const &) {
  if (state == pre_parsed) {
    matcher().add(post);
    return true;
  }
  return false;
}

bool match_follow::play(ev::up const &) {
  if (state == post_parsed) {
    expectation = nothing;
    return true;
  }
  return false;
}
