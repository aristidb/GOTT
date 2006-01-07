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
#include <gott/tdl/structure/repatchers/enumeration.hpp>
#include <gott/tdl/structure/repatchers/integer.hpp>

namespace schema = gott::tdl::schema;
namespace stru = gott::tdl::structure;
namespace simple = gott::tdl::simple;
using gott::xany::Xany;
using gott::string;

using stru::cf::S;
using stru::cf::C;
using schema::rule_t;
typedef schema::rule_attr_t RA;

namespace {
struct schema_unordered_foo_integer_string : tut::schema_basic {
  schema_unordered_foo_integer_string() 
  : tut::schema_basic(
      rule("document", RA(), Vector<rule_t>() <<
        rule("unordered", RA(), Vector<rule_t>() <<
          rule("node", RA("foo", true, 
               new stru::repatch_enumeration(std::vector<string>(1,"foo")))) <<
          rule("node", RA(RA::simple, true, new stru::repatch_integer())) <<
          rule("node", RA())))) {}
};
}

namespace tut {
typedef test_group<schema_unordered_foo_integer_string> tf;
typedef tf::object object;
}

namespace {
  tut::tf ordered_integer_string_test("schema::unordered_foo_integer_string");
}

namespace tut {
template<> template<>
void object::test<1>(int) {
  run_test("foo\n4\nx");
  stru::cf::nd_list c;
  c.push_back(S(Xany(0), "foo"));
  c.push_back(S(Xany(4)));
  c.push_back(S(Xany("x")));
  C(M(c)).write_to(xp);
  ensure_equals("foo,int,string", tree, xp);
}

template<> template<>
void object::test<2>(int) {
  try {
    run_test("d7");
    fail("just string");
  } catch (schema::mismatch const &mm) {
    ensure_equals("correct error", gott::string(mm.what()), 
        "1:1 : mismatch in document>unordered>node(foo) after token d7");
  }
}

template<> template<>
void object::test<3>(int) {
  try {
    run_test("");
    fail("empty");
  } catch (schema::mismatch const &mm) {
    ensure_equals("correct error", gott::string(mm.what()), 
        "0:1 : mismatch in document>unordered>node(foo) after token ");
  }
}

template<> template<>
void object::test<4>(int) {
  try {
    run_test("foo bar");
    fail("string following string");
  } catch (schema::mismatch const &mm) {
    ensure_equals("correct error", gott::string(mm.what()), 
        "1:1 : mismatch in document>unordered>node after token foo");
  }
}

template<> template<>
void object::test<5>(int) {
  try {
    run_test("foo");
    fail("just foo");
  } catch (schema::mismatch const &mm) {
    ensure_equals("correct error", gott::string(mm.what()), 
        "1:1 : mismatch in document>unordered>node after token foo");
  }
}

template<> template<>
void object::test<6>(int) {
  try {
    run_test("4,x,y");
  } catch (schema::mismatch const &mm) {
    ensure_equals("correct error", gott::string(mm.what()), 
        "1:5 : mismatch in document>unordered>node(foo) at token y");
  }
}

template<> template<>
void object::test<7>(int) {
  try {
    run_test("732 bar");
    fail("string following integer");
  } catch (schema::mismatch const &mm) {
    ensure_equals("correct error", gott::string(mm.what()), 
        "1:1 : mismatch in document>unordered>node(foo) after token 732");
  }
}

template<> template<>
void object::test<8>(int) {
  run_test("77,foo,foo");
  stru::cf::nd_list c;
  c.push_back(S(Xany(77)));
  c.push_back(S(Xany(0), "foo"));
  c.push_back(S(Xany("foo")));
  C(M(c)).write_to(xp);
  ensure_equals("reordered #1", tree, xp);
}

template<> template<>
void object::test<9>(int) {
  run_test("hallo\n-4,foo");
  stru::cf::nd_list c;
  c.push_back(S(Xany("hallo")));
  c.push_back(S(Xany(-4)));
  c.push_back(S(Xany(0), "foo"));
  C(M(c)).write_to(xp);
  ensure_equals("reordered #2", tree, xp);
}

template<> template<>
void object::test<10>(int) {
  no_test();
}

// further tests
}
