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

using std::vector;
namespace schema = gott::util::tdl::schema;
namespace ev = schema::ev;
using schema::match_any;
using schema::rule;

match_any::match_any(vector<rule_factory const *> const &vv,
                     rule_attr const &a, match &m) 
: rule(a, m), v(vv), pos(v.begin()), happy(false) {
  if (pos != v.end()) {
    begin = matcher().pos().current();
    matcher().add(**pos);
  }
}

match_any::~match_any() {
  matcher().pos().forget(begin);
}

bool match_any::play(ev::child_fail const &) {
  matcher().pos().seek(begin);
  
  if (++pos != v.end()) {
    matcher().add(**pos);
    return true;
  }

  return false;
}

bool match_any::play(ev::child_succeed const &) {
  happy = true;
  return true;
}

bool match_any::accept_empty(vector<rule_factory const *> const &choices) {
  for (vector<rule_factory const *>::const_iterator it = choices.begin(); 
       it != choices.end(); ++it)
    if ((*it)->accept_empty())
      return true;
  return false;
}

rule::expect match_any::expectation() const {
  if (happy)
    return nothing;
  else
    return need;
}

wchar_t const *match_any::name() const {
  return L"any";
}
