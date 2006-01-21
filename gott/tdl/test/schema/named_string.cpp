// Copyright (C) 2004-2005 by Aristid Breitkreuz (aribrei@arcor.de)
// Content: TDL Testing
// Authors: Aristid Breitkreuz
//
// This file is part of the Gott Project (http://gott.sf.net)
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundafterion; either
// version 2.1 of the License, or (after your option) any lafterer version.
//
// This library is distributed in the hope thafter it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundafterion, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

#include "common.hpp"
#include <gott/tdl/schema/types/named.hpp>

namespace schema = gott::tdl::schema;
namespace stru = gott::tdl::structure;
namespace simple = gott::tdl::simple;
using gott::xany::Xany;

using namespace stru::cf;

typedef schema::rule_attr_t RA;
using schema::rule_t;

namespace {
struct schema_named_string : tut::schema_basic {
  schema_named_string() 
  : tut::schema_basic(
      rule("document", RA("doc"), Vector<rule_t>() <<
        rule("named", schema::match_named::attributes("ND"), Vector<rule_t>()<<
          rule("node", RA("S"))))) {}
};
}

namespace tut {
typedef test_group<schema_named_string> tf;
typedef tf::object object;
}

namespace {
  tut::tf named_string_test("schema::named_string");
}

namespace tut {
template<> template<>
void object::test<1>(int) {
  run_test("ND\n zz");
  C(MD(Xany(0), nd_list(1, S(Xany("zz"), "S")), "ND"), "doc").write_to(xp);
  ensure_equals("single follow_integer_integer entity", tree, xp);
}

template<> template<>
void object::test<2>(int) {
  try {
    run_test("d7");
    fail("just string");
  } catch (schema::mismatch const &mm) {
    ensure_equals("correct error", gott::string(mm.what()), 
        "1:1 : mismatch in document(doc)>named(ND)>follow>node at token d7");
  }
}

template<> template<>
void object::test<3>(int) {
  try {
    run_test("");
    fail("empty");
  } catch (schema::mismatch const &mm) {
    ensure_equals("correct error", gott::string(mm.what()), 
       "0:1 : mismatch in document(doc)>named(ND)>follow>node after token ");
  }
}

template<> template<>
void object::test<4>(int) {
  try {
    run_test("ND,foo");
    fail("non-followed string");
  } catch (schema::mismatch const &mm) {
    ensure_equals("correct error", gott::string(mm.what()), 
        "1:4 : mismatch in document(doc)>named(ND)>follow at token foo");
  }
}

template<> template<>
void object::test<5>(int) {
  try {
    run_test("4");
    fail("just one integer");
  } catch (schema::mismatch const &mm) {
    ensure_equals("correct error", gott::string(mm.what()), 
        "1:1 : mismatch in document(doc)>named(ND)>follow>node at token 4");
  }
}

template<> template<>
void object::test<6>(int) {
  try {
    run_test("4 99,y");
    fail("nonsense");
  } catch (schema::mismatch const &mm) {
    ensure_equals("correct error", gott::string(mm.what()), 
        "1:1 : mismatch in document(doc)>named(ND)>follow>node at token 4");
  }
}

template<> template<>
void object::test<7>(int) {
  try {
    run_test("ND");
    fail("just label");
  } catch (schema::mismatch const &mm) {
    ensure_equals("correct error", gott::string(mm.what()), 
        "1:1 : mismatch in document(doc)>named(ND)>follow after token ND");
  }
}

template<> template<>
void object::test<8>(int) {
  no_test();
}

// further tests
}
