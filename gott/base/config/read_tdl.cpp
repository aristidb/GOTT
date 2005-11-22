// Copyright (C) 2005 by Aristid Breitkreuz (aribrei@arcor.de)
// Content: Program configuration library
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

#include "read_tdl.hpp"
#include <gott/tdl/schema/by_name.hpp>
#include <gott/tdl/schema/event.hpp>
#include <gott/tdl/schema/match.hpp>
#include <boost/algorithm/string.hpp>
#include <gott/range.hpp>

#include <iostream>

using gott::string;
namespace config = gott::config;
namespace schema = gott::tdl::schema;
namespace ev = schema::ev;
using config::match_config_tdl;

namespace {
struct config_type {
  config_type() {
    schema::by_name().add<match_config_tdl>("config");
  }
} auto_reg;
}

match_config_tdl::match_config_tdl(schema::rule_attr_t const &ra, 
    Vector<schema::rule_t> const &o_children, 
    schema::match &m) 
: schema::item(ra, m), next_child(-1) {
  for (Vector<schema::rule_t>::const_iterator it = o_children.begin(); 
      it != o_children.end(); ++it)
    children.Add(it->attributes().tags()[0], *it);
}

bool match_config_tdl::play(ev::down const &) {
  if (next_child >= 0 && !dirty)
    matcher().add(children[next_child]);
  else {
    add_len.Top() += 2;
    current_id = current_id + "::";
  }

  std::cout << "down => " << current_id << std::endl;

  return true;
}

bool match_config_tdl::play(ev::up const &) {
  std::cout << "up => ";
  if (next_child < 0) {
    std::cout << '$' << current_id << std::endl;
    return false;
  }

  dirty = false;

  current_id = offset(current_id.as_utf8(), 0, -add_len.Top());
  add_len.Pop();
  
  std::cout << current_id << std::endl;
  
  return true;
}

bool match_config_tdl::play(ev::node const &n) {
  add_len.Add(n.get_data().size());
  if (current_id == string()) 
    current_id = n.get_data();
  else
    current_id = current_id + n.get_data();

  std::cout << "node => " << current_id << std::endl;
  
  next_child = children.Find(current_id);

  return true;
}

bool match_config_tdl::play(ev::child_succeed const &) {
  dirty = true;
  std::cout << "child_succeed => " << current_id << std::endl;
  return true;
}

schema::item::expect match_config_tdl::expectation() const {
  if (next_child < 0)
    if (current_id == string())
      return maybe;
    else
      return need;
  if (current_id == string())
    return nothing;
  return maybe;
}

string match_config_tdl::name() const {
  return "config";
}
