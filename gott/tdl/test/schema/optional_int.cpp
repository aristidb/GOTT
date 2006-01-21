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

#include "common.hpp"
#include <gott/tdl/schema/slot.hpp>
#include <gott/tdl/structure/repatchers/integer.hpp>

namespace schema = gott::tdl::schema;
namespace stru = gott::tdl::structure;
namespace simple = gott::tdl::simple;
using gott::xany::Xany;
using gott::string;

using stru::cf::S;
using stru::cf::C;
using stru::cf::M;
using schema::slotcfg;
using schema::rule_t;

typedef schema::rule_attr_t RA;

namespace {
struct schema_optional_int : tut::schema_basic {
  schema_optional_int() 
  : tut::schema_basic(
      rule_one("document",
         rule_one("list",
           rule("node",
             RA(std::vector<string>(1, "el"), true, Xany(), 
                new stru::repatch_integer(), 
                slotcfg(), slotcfg(slotcfg::optional)))))) {}
};
}

namespace tut {
typedef test_group<schema_optional_int> tf;
typedef tf::object object;
}

namespace {
  tut::tf list_int_then_string_test("schema::optional_int");
}

namespace tut {
template<> template<>
void object::test<1>(int) {
  run_test("1\n");
  C(C(S(Xany(1), "el"))).write_to(xp);
  ensure_equals("optional int", tree, xp);
}

template<> template<>
void object::test<2>(int) {
  run_test("");
  C(S(Xany())).write_to(xp);
  ensure_equals("optional int", tree, xp);
}

template<> template<>
void object::test<3>(int) {
  try {
    run_test("1 2 3");
    fail("going down");
  } catch (schema::mismatch const &m) {
    ensure_equals("correct error", std::string(m.what()),
                  "1:1 : mismatch in document after token 1");
  }
}

template<> template<>
void object::test<4>(int) {
  try {
    run_test("zzzz");
    fail("string");
  } catch (schema::mismatch const &m) {
    ensure_equals("correct error", std::string(m.what()),
                  "1:1 : mismatch in document>list>node(el) at token zzzz");
  }
}

template<> template<>
void object::test<5>(int t) {
  int n = t - 3; // minimum: 2 elements
  std::ostringstream w;
  for (int i = 1; i <= n; ++i)
    w << i << '\n';
  try {
    run_test(w.str());
    fail("many");
  } catch(schema::mismatch const &m) {
    ensure_equals("correct error", std::string(m.what()),
                  "2:1 : mismatch in document at token 2");
  }
}

template<> template<>
void object::test<9>(int) {
  no_test();
}

}
