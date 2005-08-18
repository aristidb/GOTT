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

#ifndef GOTT_UTIL_TDL_SCHEMA_NAMED_HPP
#define GOTT_UTIL_TDL_SCHEMA_NAMED_HPP

#include "../match.hpp"
#include "../rule_factory.hpp"
#include "../rule_attr.hpp"
#include "../happy_once.hpp"
#include "follow_ordered.hpp"
#include "node.hpp"

namespace gott {
namespace tdl {
namespace schema {

class match_named : public happy_once {
public:
  static GOTT_EXPORT
  rule_attr attributes(string const &s, bool cc = true);
  
  typedef factory_template::onechild<match_named> factory;
  match_named(rule_factory const &, rule_attr const &, match &);

  ~match_named();

  static bool accept_empty(bool) { return false; }

private:
  virtual string name() const;
  string tag;
  match_follow_ordered::factory outer;
  match_node::factory inner_name;
  bool play(ev::child_succeed const &);
};
  
}}}

#endif
