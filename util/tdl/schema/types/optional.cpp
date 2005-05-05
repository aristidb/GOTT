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

#include "optional.hpp"

namespace schema = gott::util::tdl::schema;
namespace ev = gott::util::tdl::schema::ev;
using schema::rule;
using schema::match_optional;

match_optional::match_optional(rule::factory const &x, 
                               rule::attributes const &a, match &m) 
: rule(maybe, a, m), sub(x), start(m.pos().current()) {
  matcher().add(sub);
}

match_optional::~match_optional() {
  matcher().pos().forget(start);
}

bool match_optional::play(ev::child_succeed const &) {
  expectation = nothing;
  return true;
}

bool match_optional::play(ev::child_fail const &) {
  matcher().pos().seek(start);
  expectation = nothing;
  return true;
}
