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
using schema::item;
using schema::rule_t;
typedef schema::rule_attr_t RA;

item::expect odd(std::size_t c) {
  return c % 2 ? item::maybe : item::need;
}

namespace {
struct schema_odd_int : tut::schema_basic {
  schema_odd_int() 
  : tut::schema_basic(
      rule_one("document",
         rule_one("list",
           rule("node",
             RA(std::vector<string>(1, "el"), true, Xany(), 
                new stru::repatch_integer(), 
                slotcfg(), slotcfg(slotcfg::function, odd)))))) {}
};
}

namespace tut {
typedef test_group<schema_odd_int> tf;
typedef tf::object object;
}

namespace {
  tut::tf list_int_then_string_test("schema::odd_int");
}

namespace tut {
template<> template<>
void object::test<1>(int n) {
  try {
    std::ostringstream w;
    for (int i = 0; i < n; ++i)
      w << i%10 << ',' << 0 << '\n';
    run_test(w.str());
    fail("even");
  } catch(schema::mismatch const &m) {
    std::stringstream err;
    err << n << ":3 : mismatch in document>list>node(el) after token 0";
    ensure_equals("correct error", m.what(), err.str());
  }
}

template<> template<>
void object::test<12>(int) {
  try {
    run_test("1 2 3");
    fail("going down");
  } catch (schema::mismatch const &m) {
    ensure_equals("correct error", std::string(m.what()),
                  "1:1 : mismatch in document>list>node(el) after token 1");
  }
}

template<> template<>
void object::test<13>(int) {
  try {
    run_test("zzzz");
    fail("string");
  } catch (schema::mismatch const &m) {
    ensure_equals("correct error", std::string(m.what()),
                  "1:1 : mismatch in document>list>node(el) at token zzzz");
  }
}

template<> template<>
void object::test<14>(int t) {
  int n = t - 14;
  std::ostringstream w;
  for (int i = 0; i < n; ++i) 
    w << i << ',' << 1 << '\n';
  w << 0;
  run_test(w.str());

  stru::cf::nd_list c;
  for (int i = 0; i < n; ++i) {
    c.push_back(S(Xany(i), "el"));
    c.push_back(S(Xany(1), "el"));
  }
  c.push_back(S(Xany(0), "el"));
  C(M(c)).write_to(xp);

  ensure_equals("odd", tree, xp);
}

template<> template<>
void object::test<22>(int) {
  no_test();
}

}
