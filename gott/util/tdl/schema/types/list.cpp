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

#include "list.hpp"

namespace schema = gott::tdl::schema;
namespace ev = gott::tdl::schema::ev;
using schema::rule;
using schema::slotcfg;
using schema::match_list;

match_list::match_list(rule_factory const &r, slotcfg const &c,
                                 rule_attr const &a, match &m)
: rule(a, m), sub(r), cfg(c) {
  last = m.pos().current();
  if (accept_more(expectation()))
    matcher().add(sub);
}

match_list::~match_list() {
  if (expectation() != nothing)
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

  matcher().pos().forget(last);
  if (expectation() != nothing) {
    last = matcher().pos().current();
    matcher().add(sub);
  }

  return true;
}

bool match_list::empty() {
  matcher().pos().seek(last);
  if (expectation() != need)
    cfg.cancel();

  return true;
}

rule::expect match_list::expectation() const {
  return cfg.expectation();
}

wchar_t const *match_list::name() const {
  return L"list";
}
