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

#include <gott/tdl/schema/rule.hpp>
#include <gott/tdl/structure/repatchers/integer.hpp>
#include <gott/tdl/schema/match.hpp>
#include <gott/tdl/structure/container.hpp>
#include <gott/tdl/structure/revocable_adapter.hpp>
#include <gott/tdl/structure/print.hpp>
#include <boost/assign/list_of.hpp>
#include <iostream>

using namespace gott;
using namespace gott::tdl;
using namespace gott::tdl::schema;
using namespace boost::assign;

int main() {
  std::cerr 
    <<
    "Simple config test: TDL configuration\n"
    "There are the following keys:\n"
    "  - conf::int : an integer\n"
    "  - conf::string : an arbitrary string\n"
    "  - get::more::stuff : a list of integers\n"
    ;
  
  rule_t conf = 
    rule_one("document",
      rule("list", rule_attr(tag = "container"),
        list_of(rule("config", rule_attr(outer = list(), tag = "element"),
          list_of
          (rule("node", rule_attr(tag = "conf::int", 
              repatcher = new structure::repatch_integer())))
          (rule("node", rule_attr(tag = "conf::string")))
          (rule("list", rule_attr(tag = "get::more::stuff"),
            list_of(rule("node", rule_attr(outer = list())))))))));

  structure::container out;
  structure::revocable_adapter r(out);
  match m(conf, r);
  m.parse(std::cin);
  std::cout << out << '\n';
}
