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

#include "any.hpp"

namespace schema = gott::tdl::schema;
namespace ev = schema::ev;
using schema::match_any;
using schema::item;

match_any::match_any(rule_attr_t const &a, Vector<rule_t> const &vv, match &m) 
: happy_once(a, m), v(vv), pos(v.begin()) {
  if (pos != v.end()) {
    begin = matcher().pos().current();
    matcher().add(*pos);
  }
}

match_any::~match_any() {
  if (expectation() != nothing)
    matcher().pos().forget(begin);
}

bool match_any::play(ev::child_fail const &) {
  matcher().pos().seek(begin);
  
  if (++pos != v.end()) {
    matcher().add(*pos);
    return true;
  }

  return false;
}

bool match_any::play(ev::child_succeed const &) {
  matcher().pos().forget(begin);
  be_happy();
  return true;
}

bool match_any::accept_empty(rule_attr_t const &, Vector<rule_t> const &choices) {
  for (Vector<rule_t>::const_iterator it = choices.begin(); 
       it != choices.end(); ++it)
    if (it->accept_empty())
      return true;
  return false;
}

gott::string match_any::name() const {
  return "any";
}
