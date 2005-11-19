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

#ifndef GOTT_BASE_CONFIG_READ_TDL_HPP
#define GOTT_BASE_CONFIG_READ_TDL_HPP

#include <gott/tdl/schema/item.hpp>
#include <gott/tdl/schema/rule.hpp>
#include <gott/tdl/schema/rule_attr.hpp>

namespace gott {
namespace config {

/**
 * Matcher for a single config item.
 * Matches one TDL item (which may configure more than one item).
 * user-data: VectorMap<QID, rule_t>
 * format:
 * sub-qid {%follow% {%ordered% sub-qid}} data
 */
class match_config : tdl::schema::item {
public:
  match_config(tdl::schema::rule_attr_t const &, 
      Vector<tdl::schema::rule_t> const &, 
      tdl::schema::match &);

  static bool accept_empty(
      tdl::schema::rule_attr_t const &, 
      Vector<tdl::schema::rule_t> const &)
  { return true; }

private:
  bool play(tdl::schema::ev::down const &);
  bool play(tdl::schema::ev::up const &);
  bool play(tdl::schema::ev::node const &);
  bool play(tdl::schema::ev::child_succeed const &);
  string name() const;
};

}}

#endif
