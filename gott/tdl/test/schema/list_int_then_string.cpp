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
#include <gott/tdl/structure/repatchers/integer.hpp>
#include <gott/string/string.hpp>

namespace schema = gott::tdl::schema;
namespace stru = gott::tdl::structure;
namespace simple = gott::tdl::simple;
using gott::xany::Xany;

using stru::cf::S;
using stru::cf::C;
using stru::cf::M;

typedef schema::rule_attr_t RA;
using schema::rule_t;
using schema::slotcfg;

namespace {
struct schema_list_int_then_string : tut::schema_basic {
  schema_list_int_then_string() 
  : tut::schema_basic(
      rule("document", RA(), Vector<rule_t>() <<
        rule("ordered", RA("ord"), Vector<rule_t>() <<
          rule("list", RA("list"), Vector<rule_t>() <<
            rule("node", 
              RA(std::vector<gott::string>(1, "int"), true, Xany(),
                new stru::repatch_integer(),
                slotcfg(), slotcfg(slotcfg::list)))) <<
          rule("node", RA("string"))))) {}
};
}

namespace tut {
typedef test_group<schema_list_int_then_string> tf;
typedef tf::object object;
}

namespace {
  tut::tf list_int_then_string_test("schema::list_int_then_string");
}

namespace tut {
template<> template<>
void object::test<1>(int) {
  run_test("4\nx");
  stru::cf::nd_list c;
  c.push_back(C(S(Xany(4), "int"), "list"));
  c.push_back(S(Xany(L"x"), "string"));
  C(M(c, "ord")).write_to(xp);
  ensure_equals("single integer, then string", tree, xp);
}

template<> template<>
void object::test<2>(int) {
  run_test("d7");
  stru::cf::nd_list c;
  c.push_back(S(Xany(), "list"));
  c.push_back(S(Xany(L"d7"), "string"));
  C(M(c, "ord")).write_to(xp);
  ensure_equals("just string", tree, xp);
}

template<> template<>
void object::test<3>(int) {
  try {
    run_test("");
    fail("empty");
  } catch (schema::mismatch const &mm) {
    ensure_equals("correct error", gott::string(mm.what()), 
        "0:1 : mismatch in document>ordered(ord)>list(list)>node(int)"
        " after token ");
  }
}

template<> template<>
void object::test<4>(int) {
  try {
    run_test("foo bar");
    fail("following");
  } catch (schema::mismatch const &mm) {
    ensure_equals("correct error", gott::string(mm.what()), 
        "1:1 : mismatch in document after token foo");
  }
}

template<> template<>
void object::test<5>(int) {
  try {
    run_test("4");
    fail("just integer");
  } catch (schema::mismatch const &mm) {
    ensure_equals("correct error", gott::string(mm.what()), 
        "1:1 : mismatch in document>ordered(ord)>list(list)>node(int)"
        " after token 4");
  }
}

template<> template<>
void object::test<6>(int) {
  try {
    run_test("4,x,y");
    fail("too many strings");
  } catch (schema::mismatch const &mm) {
    ensure_equals("correct error", 
        gott::string(mm.what()), "1:5 : mismatch in document at token y");
  }
}

template<> template<>
void object::test<7>(int) {
  run_test("4\n\n5\n6,7\n(zzz doink)");
  stru::cf::nd_list ll;
  for (int i = 4; i <= 7; ++i)
    ll.push_back(S(Xany(i), "int"));
  stru::cf::nd_list c;
  c.push_back(M(ll, "list"));
  c.push_back(S(Xany("(zzz doink)"), "string"));
  C(M(c, "ord")).write_to(xp);
  ensure_equals("many thingies", tree, xp);
}

template<> template<>
void object::test<8>(int) {
  no_test();
}

// further tests
}
