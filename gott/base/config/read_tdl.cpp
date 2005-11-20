// Copyright (C) 2005 by Aristid Breitkreuz (aribrei@arcor.de)
// Content: Non-immediate graphics library
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
: schema::happy_once(ra, m) {
  for (Vector<schema::rule_t>::const_iterator it = o_children.begin(); 
      it != o_children.end(); ++it)
    children.Add(it->attributes().tags()[0], *it);
}

bool match_config_tdl::play(ev::down const &) {
  current_id = current_id + "::";
  ++level;
  return true;
}

bool match_config_tdl::play(ev::up const &) {
  {
    range_t<utf8_t const *> in = current_id.as_utf8();
    utf8_t const *it = 
      boost::algorithm::find_last(in, "::").end();
    current_id = range(current_id.as_utf8().Begin, it);
  }
  --level;
  return true;
}

bool match_config_tdl::play(ev::node const &n) {
  current_id = current_id + n.get_data();
  int no = children.Find(current_id);
  if (no >= 0)
    matcher().add(children[no]);
  return true;
}

bool match_config_tdl::play(ev::child_succeed const &) {
  return true;
}

string match_config_tdl::name() const {
  return "config";
}
