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
#include <gott/tdl/structure/types/integer.hpp>
#include <gott/tdl/schema/match.hpp>
#include <gott/tdl/structure/tree.hpp>
#include <gott/tdl/structure/print.hpp>
#include <iostream>

using namespace gott;
using namespace gott::tdl;

int main() {
  schema::rule_t conf = 
    schema::rule("document",
      schema::rule("config",
        Vector<schema::rule_t>() <<
        schema::rule("node", schema::rule_attr(schema::tag = "conf::int", 
            schema::repatcher = new structure::repatch_integer())) <<
        schema::rule("node", schema::rule_attr(schema::tag = "conf::string"))
  ));
  structure::tree out;
  schema::match m(conf, out);
  m.parse(std::wcin);
  structure::direct_print<char> wrt(std::cout);
  out.copy_to(wrt);
}
