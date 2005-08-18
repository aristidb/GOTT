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

#include "named.hpp"
#include "../event.hpp"
#include <gott/util/string/string.hpp>
#include <gott/util/tdl/structure/types/enumeration.hpp>

using gott::xany::Xany;
using gott::xany::Xany_cast;

namespace schema = gott::tdl::schema;
namespace ev = gott::tdl::schema::ev;
using schema::rule;
using schema::rule_attr;
using schema::match_named;

rule_attr match_named::attributes(string const &s, bool cc) {
  return rule_attr(Vector<string>() << s, cc, Xany(s));
}

match_named::match_named(rule_factory const &s, rule_attr const &a, 
                         match &m) 
: happy_once(a, m), 
  tag(Xany_cast<string>(a.user())),
  outer(rule_attr(rule_attr::simple, false)),
  inner_name(rule_attr(rule_attr::simple, false, 
        new structure::repatch_enumeration(Vector<string>() << tag))) {
  outer.add(inner_name);
  outer.add(s);
  matcher().add(outer);
}

match_named::~match_named() {}

bool match_named::play(ev::child_succeed const &) {
  be_happy();
  return true;
}

gott::string match_named::name() const {
  return "named";
}
