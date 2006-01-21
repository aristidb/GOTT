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
#include <gott/string/string.hpp>
#include <gott/tdl/structure/repatchers/enumeration.hpp>
#include <gott/debug/assert.hpp>
#include <boost/assign/list_of.hpp>

using gott::xany::Xany;
using gott::xany::Xany_cast;

namespace schema = gott::tdl::schema;
namespace ev = gott::tdl::schema::ev;
using schema::item;
using schema::rule_attr_t;
using schema::match_named;
using namespace boost::assign;

rule_attr_t match_named::attributes(string const &s, bool cc) {
  return rule_attr_t(std::vector<string>(1, s), cc, Xany(s));
}

match_named::match_named(rule_attr_t const &a, std::vector<rule_t> const &s, match &m) 
: happy_once(a, m), 
  tag(Xany_cast<string>(a.user())),
  rewritten(
      rule("follow", rule_attr_t(rule_attr_t::simple, false), list_of 
        (rule("node", rule_attr_t(rule_attr_t::simple, false, 
          new structure::repatch_enumeration(std::vector<string>(1, tag)))))
        (s[0]))) {
  GOTT_ASSERT_2(s.size(), 1, std::equal_to<int>(), "one parameter");
  matcher().add(rewritten);
}

match_named::~match_named() {}

bool match_named::play(ev::child_succeed const &) {
  be_happy();
  return true;
}

gott::string match_named::name() const {
  return "named";
}
