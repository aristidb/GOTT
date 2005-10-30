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
#include <gott/util/tdl/schema/slot.hpp>
#include <gott/util/tdl/structure/types/integer.hpp>

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
struct schema_5to8int : tut::schema_basic {
  schema_5to8int()
  : tut::schema_basic(
      rule("document", RA(),
        Vector<rule_t>() <<
         rule("list", RA(),
           Vector<rule_t>() <<
           rule("node",
             RA(Vector<string>() << "el", true, Xany(), 
                new stru::repatch_integer(), 
                slotcfg(), slotcfg(slotcfg::range, 5, 8))))))
  {}
};
}

namespace tut {
typedef test_group<schema_5to8int> tf;
typedef tf::object object;
}

namespace {
  tut::tf list_int_then_string_test("schema::5to8int");
}

namespace tut {
template<> template<>
void object::test<1>(int c) {
  try {
    std::wostringstream w;
    for (int i = 1; i < c; ++i) // 0-4
      w << (77 + i) << '\n';
    run_test(w.str());
    fail("too few");
  } catch (schema::mismatch const &m) {
    std::ostringstream err;
    err << (c-1) << ":1 : mismatch in document>list>node(el) after token ";
    if (c > 1) err << 76 + c;
    ensure_equals("correct error", m.what(), err.str());
  }
}

template<> template<>
void object::test<6>(int t) {
  std::wostringstream w;
  for (int i = 1; i < t; ++i) // 5-8
    w << '\n' << -i;
  run_test(w.str());
  stru::cf::nd_list c;
  for (int i = 1; i < t; ++i)
    c.push_back(S(Xany(-i), L"el"));
  C(M(c)).write_to(xp);
  ensure_equals("in range", tree, xp);
}

template<> template<>
void object::test<10>(int c) {
  try {
    std::wostringstream w;
    for (int i = 1; i < c; ++i) // 9-...
      w << (77 + i) << '\n';
    run_test(w.str());
    fail("too few");
  } catch (schema::mismatch const &m) {
    ensure_equals("correct error", m.what(),
                  std::string("9:1 : mismatch in document at token 86"));
  }
}

template<> template<>
void object::test<16>(int) {
  try {
    run_test(L"1 2 3");
    fail("going down");
  } catch (schema::mismatch const &m) {
    ensure_equals("correct error", std::string(m.what()), 
                  "1:1 : mismatch in document>list>node(el) after token 1");
  }
}

template<> template<>
void object::test<17>(int) {
  try {
    run_test(L"lala");
    fail("string");
  } catch (schema::mismatch const &m) {
    ensure_equals("correct error", std::string(m.what()), 
                  "1:1 : mismatch in document>list>node(el) at token lala");
  }
}

template<> template<>
void object::test<18>(int) {
  no_test();
}

// further tests
}
