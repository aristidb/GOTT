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
struct schema_some_string : tut::schema_basic {
  schema_some_string()
  : tut::schema_basic(
      rule_one("document", RA(),
        rule_one("list", RA(),
          rule("node", 
            RA(std::vector<string>(1, "el"), true, Xany(), 0, 
               slotcfg(), slotcfg(slotcfg::some)))))) {}
};
}

namespace tut {
typedef test_group<schema_some_string> tf;
typedef tf::object object;
}

namespace {
  tut::tf list_int_then_string_test("schema::some_string");
}

namespace tut {
template<> template<>
void object::test<1>(int) {
  run_test("aa\nbb\ncc");
  stru::cf::nd_list c;
  c.push_back(S(Xany("aa"), "el"));
  c.push_back(S(Xany("bb"), "el"));
  c.push_back(S(Xany("cc"), "el"));
  C(M(c)).write_to(xp);
  ensure_equals("three strings", tree, xp);
}

template<> template<>
void object::test<2>(int) {
  try {
    run_test("");
    fail("empty");
  } catch (schema::mismatch const &m) {
    ensure_equals("correct error", gott::string(m.what()), 
                  "0:1 : mismatch in document>list>node(el) after token ");
  }
}

template<> template<>
void object::test<3>(int) {
  try {
    run_test("1 2 3");
    fail("going down");
  } catch (schema::mismatch const &m) {
    ensure_equals("correct error", gott::string(m.what()),
                  "1:1 : mismatch in document>list>node(el) after token 1");
  }
}

template<> template<>
void object::test<4>(int) {
  run_test("zzzz");
  C(C(S(Xany("zzzz"), "el"))).write_to(xp);
  ensure_equals("one string", tree, xp);
}

template<> template<>
void object::test<5>(int t) {
  int n = t - 3; // minimum: 2 elements
  std::ostringstream w;
  for (int i = 0; i < n; ++i)
    w << char('A' + i) << '\n';
  run_test(w.str());
  stru::cf::nd_list c;
  for (int i = 0; i < n; ++i)
    c.push_back(S(Xany(std::string(1, 'A'+i)), "el"));
  C(M(c)).write_to(xp);
  ensure_equals("many", tree, xp);
}

template<> template<>
void object::test<15>(int) {
  no_test();
}

}
