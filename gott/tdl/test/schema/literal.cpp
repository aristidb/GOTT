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
#include <gott/tdl/structure/types/enumeration.hpp>

namespace schema = gott::tdl::schema;
namespace stru = gott::tdl::structure;
namespace simple = gott::tdl::simple;
using gott::xany::Xany;

using gott::string;
using stru::cf::S;
using stru::cf::C;
using schema::rule_attr_t;

namespace {
struct schema_literal : tut::schema_basic {
  schema_literal() 
  : tut::schema_basic(
      rule("document", rule_attr_t(), Vector<schema::rule_t>() <<
        rule("node", 
          rule_attr_t(L"foobar", true, 
            new stru::repatch_enumeration(Vector<string>() | "foobar"))))) {}
};
}

namespace tut {
typedef test_group<schema_literal> tf;
typedef tf::object object;
}

namespace {
  tut::tf literal_test("schema::literal");
}

namespace tut {
template<> template<>
void object::test<1>(int) {
  run_test("foobar");
  C(S(Xany(0), "foobar")).write_to(xp);
  ensure_equals("single foobar entity", tree, xp);
}

template<> template<>
void object::test<2>(int) {
  try {
    run_test("d7");
    fail("non-foobar");
  } catch (schema::mismatch const &mm) {
    ensure_equals("correct error", std::string(mm.what()), 
        "1:1 : mismatch in document>node(foobar) at token d7");
  }
}

template<> template<>
void object::test<3>(int) {
  try {
    run_test("");
    fail("empty");
  } catch (schema::mismatch const &mm) {
    ensure_equals("correct error", std::string(mm.what()), 
        "0:1 : mismatch in document>node(foobar) after token ");
  }
}

template<> template<>
void object::test<4>(int) {
  try {
    run_test("foobar bar");
    fail("overfilled #1");
  } catch (schema::mismatch const &mm) {
    ensure_equals("correct error", std::string(mm.what()), 
        "1:1 : mismatch in document after token foobar");
  }
}

template<> template<>
void object::test<5>(int) {
  try {
    run_test("foo\nbar");
    fail("overfilled #2");
  } catch (schema::mismatch const &mm) {
    ensure_equals("correct error", std::string(mm.what()), 
        "1:1 : mismatch in document>node(foobar) at token foo");
  }
}

template<> template<>
void object::test<6>(int) {
  no_test();
}

// further tests
}
