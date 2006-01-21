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

#ifndef GOTT_UTIL_TDL_SCHEMA_ANY_HPP
#define GOTT_UTIL_TDL_SCHEMA_ANY_HPP

#include "../match.hpp"
#include "../parse_position.hpp"
#include "../happy_once.hpp"
#include "../rule.hpp"

namespace gott {
namespace tdl {
namespace schema {

// Matcher any
// Matches the first applicable of an arbitrary number of rule_ts.
class match_any : public happy_once {
public:
  match_any(rule_attr_t const &, std::vector<rule_t> const &, match &);
  ~match_any();

  static bool accept_empty(rule_attr_t const &, std::vector<rule_t> const &);

private:
  std::vector<rule_t> v;
  std::vector<rule_t>::iterator pos;
  positioning::id begin;

  bool play(ev::child_fail const &);
  bool play(ev::child_succeed const &);
  string name() const;
};

}}}

#endif
