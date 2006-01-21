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
    std::vector<schema::rule_t> const &o_children, 
    schema::match &m) 
: schema::item(ra, m), dirty(false), peer(false), may_leave(false) {
  for (std::vector<schema::rule_t>::const_iterator it = o_children.begin(); 
      it != o_children.end(); ++it)
    children.insert(std::make_pair(it->attributes().tags()[0], *it));
  next_child = children.end();
}

bool match_config_tdl::play(ev::down const &) {
  peer = false;

  if (next_child != children.end() && !dirty)
    matcher().add(next_child->second);
  else {
    add_len.back() += 2;
    current_id = current_id + "::";
  }

  std::cout << "down => " << current_id << std::endl;

  return true;
}

bool match_config_tdl::play(ev::up const &) {
  if (!may_leave)
    return false;

  std::cout << "up => ";
  if (next_child == children.end()) {
    std::cout << '$' << current_id << std::endl;
    return false;
  }

  dirty = false;
  peer = false;

  current_id = offset(current_id.as_utf8(), 0, -add_len.back());
  add_len.pop_back();
  
  std::cout << current_id << std::endl;
  
  return true;
}

bool match_config_tdl::play(ev::node const &n) {
  if (peer)
    return false;

  peer = true;
  may_leave = false;
  
  add_len.push_back(n.get_data().size());
  if (current_id == string()) 
    current_id = n.get_data();
  else
    current_id = current_id + n.get_data();

  std::cout << "node => " << current_id << std::endl;

  next_child = children.find(current_id);

  return true;
}

bool match_config_tdl::play(ev::child_succeed const &) {
  dirty = true;
  peer = true;
  may_leave = true;
  std::cout << "child_succeed => " << current_id << std::endl;
  return true;
}

schema::item::expect match_config_tdl::expectation() const {
  if (next_child == children.end())
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
