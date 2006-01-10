// Copyright (C) 2006 by Aristid Breitkreuz (aribrei@arcor.de)
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

#ifndef GOTT_UTIL_TDL_SCHEMA_TREE_HPP
#define GOTT_UTIL_TDL_SCHEMA_TREE_HPP

#include "../match.hpp"
#include "../rule.hpp"

namespace gott {
namespace tdl {
namespace schema {

class match_tree : public item {
public:
  match_tree(rule_attr_t const &, Vector<rule_t> const &, match &);

  static bool accept_empty(rule_attr_t const &, Vector<rule_t> const &) 
  { return false; }
  
private:
  unsigned level;
  enum { fresh, titled, used } level_state;
  bool fresh_level;

  item::expect expectation() const;
  bool play(ev::node const &);
  bool play(ev::down const &);
  bool play(ev::up const &);
  string name() const;
};

}}}

#endif