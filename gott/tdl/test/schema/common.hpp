// Copyright (C) 2004-2005 by Aristid Breitkreuz (aribrei@arcor.de)
// Content: TDL Testing
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

#include <gott/tdl/schema/match.hpp>
#include <gott/tdl/schema/rule.hpp>
#include <gott/tdl/schema/rule_attr.hpp>
#include <gott/tdl/exceptions.hpp>
#include <gott/tdl/structure/container.hpp>
#include <gott/tdl/structure/comfort.hpp>
#include <gott/tdl/structure/print.hpp>
#include <gott/tdl/structure/revocable_adapter.hpp>
#include <gott/string/string.hpp>
#include <gott/tut/tut.h>

namespace tut {
struct schema_basic {
  gott::tdl::structure::container tree, xp;
  gott::tdl::structure::revocable_adapter rtree;
  gott::tdl::schema::match match;
  gott::tdl::schema::rule_t grammar;
  std::string rest;
  schema_basic() : rtree(tree), match(rtree) {}
  schema_basic(gott::tdl::schema::rule_t const &r) 
    : rtree(tree), match(rtree), grammar(r) {}
  void run_test(std::string const &input) {
    match.add(grammar);
    std::istringstream stream(input);
    match.parse(stream);
    rest = stream.str().c_str() + stream.tellg();
  }
};
}

