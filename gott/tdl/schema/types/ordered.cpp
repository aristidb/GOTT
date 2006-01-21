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

#include "ordered.hpp"
#include "../rule.hpp"

namespace schema = gott::tdl::schema;
namespace ev = gott::tdl::schema::ev;
using schema::item;
using schema::match_ordered;

match_ordered::match_ordered(rule_attr_t const &a, std::vector<rule_t> const&r,
                             match  &m)
: happy_once(a, m), subrules(deflatten(r)), pos(subrules.begin()) {
  if (pos != subrules.end())
    matcher().add(*pos);
  else
    be_happy();
}

std::vector<schema::rule_t> match_ordered::deflatten(
    std::vector<rule_t> const &in) {
  std::vector<rule_t> out(in.size());
  for (std::size_t i = 0; i < in.size(); ++i) {
    if (in[i].attributes().outer() == one())
      out[i] = in[i];
    else
      out[i] = rule_one("list", rule_attr(coat = false), in[i]);
  }
  return out;
}

match_ordered::~match_ordered() {}

bool match_ordered::play(ev::child_succeed const &) {
  if (++pos != subrules.end()) 
    matcher().add(*pos);
  else 
    be_happy();
  return true;
}

bool match_ordered::accept_empty(rule_attr_t const &, 
                                 std::vector<rule_t> const &children) {
  bool accept = true;
  for (std::vector<rule_t>::const_iterator it = children.begin(); 
       it != children.end(); ++it)
    accept &= it->accept_empty();
  return accept;
}

gott::string match_ordered::name() const {
  return "ordered";
}
