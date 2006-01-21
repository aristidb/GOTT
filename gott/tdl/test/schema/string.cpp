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

namespace schema = gott::tdl::schema;
namespace stru = gott::tdl::structure;
namespace simple = gott::tdl::simple;
using gott::xany::Xany;

using stru::cf::S;
using stru::cf::C;

namespace {
struct schema_string : tut::schema_basic {
  schema_string() 
  : tut::schema_basic(
      rule_one("document", schema::rule_attr_t("doc"),
        rule("node", schema::rule_attr_t()))) {}
};
}

namespace tut {
typedef test_group<schema_string> tf;
typedef tf::object object;
}

namespace {
  tut::tf string_test("schema::string");
}

namespace tut {
template<> template<>
void object::test<1>(int) {
  run_test("data");
  C(S(Xany("data")), "doc").write_to(xp);
  ensure_equals("single entity", tree, xp);
}

template<> template<>
void object::test<2>(int) {
  try {
    run_test("");
    fail("empty");
  } catch (schema::mismatch const &) {}
}

template<> template<>
void object::test<3>(int) {
  try {
    run_test("foo bar");
    fail("overfilled #1");
  } catch (schema::mismatch const &mm) {
    ensure_equals("correct error", gott::string(mm.what()),  
                  "1:1 : mismatch in document(doc) after token foo");
  }
}

template<> template<>
void object::test<4>(int) {
  try {
    run_test("foo\nbar");
    fail("overfilled #2");
  } catch (schema::mismatch const &mm) {
    ensure_equals("correct error", gott::string(mm.what()), 
                  "2:1 : mismatch in document(doc) at token bar");
  }
}

template<> template<>
void object::test<5>(int) {
  no_test();
}

// further tests
}
