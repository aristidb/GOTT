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

#include <gott/util/tdl/schema/parse.hpp>
#include <gott/util/tdl/schema/parse_position.hpp>
#include <gott/util/tdl/structure/structure.hpp>
#include <gott/util/tdl/simple/parse/parser.hpp>
#include <gott/util/tdl/schema/context.hpp>
#include <gott/util/tut/tut.h>
#include <gott/util/nstring/nstring.hpp>

namespace tdl = gott::tdl;

namespace {
struct ignore_struct : tdl::structure::revocable_structure {
  void data(gott::xany::Xany const &) {}
  void begin() {}
  void end() {}
  void add_tag(gott::nstring const &) {}

  pth point() { return pth(); }
  void revert(pth const &) {}
  void get_rid_of(pth const &) {}
};

struct test_base {
  ignore_struct no_stru;
  tdl::schema::match match;
  tdl::schema::positioning &pos;
  tdl::simple::parser &parse;

  test_base() 
  : match(no_stru), pos(match.pos()), parse(match) {
    tdl::schema::context c;
    c.begin("document");
    c.end();
    match.add(c.get());
  }
};

typedef tut::test_group<test_base> tf;
typedef tf::object object;

tf X("schema::parse_position");
}

namespace tut {
template<> template<>
void object::test<1>(int) {
  parse.begin_parse();
  ensure_equals("first token index", pos.current().first, 0U);
}

template<> template<>
void object::test<2>(int) {
  no_test();
}
}
