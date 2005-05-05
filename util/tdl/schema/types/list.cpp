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

#include "list.hpp"

namespace schema = gott::util::tdl::schema;
namespace ev = gott::util::tdl::schema::ev;
using schema::rule;
using schema::slotcfg;
using schema::match_list;

match_list::match_list(rule::factory const &r, slotcfg const &c,
                                 rule::attributes const &a, match &m)
: rule(need, a, m), sub(r), cfg(c) {
  last = m.pos().current();
  expectation = cfg.expectation();
  matcher().add(sub);
}

match_list::~match_list() {
  matcher().pos().forget(last);
}

bool match_list::play(ev::child_fail const &) {
  matcher().pos().seek(last);
  return true;
}

bool match_list::play(ev::child_succeed const &) {
  if (matcher().pos().proceeded(last))
    return full();
  else
    return empty();
}

bool match_list::full() {
  cfg.add();
  expectation = cfg.expectation();

  if (expects() != nothing) {
    matcher().pos().forget(last);
    last = matcher().pos().current();
    matcher().add(sub);
  }

  return true;
}

bool match_list::empty() {
  matcher().pos().seek(last);
  if (expectation != need)
    expectation = nothing;

  return true;
}
