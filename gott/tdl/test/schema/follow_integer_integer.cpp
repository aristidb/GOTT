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

namespace schema = gott::tdl::schema;
namespace stru = gott::tdl::structure;
namespace simple = gott::tdl::simple;
using gott::xany::Xany;

using stru::cf::S;
using stru::cf::C;

typedef schema::rule_attr_t RA;
typedef stru::repatch_integer I;
using schema::rule_t;

namespace {
struct schema_follow_integer_integer : tut::schema_basic {
  schema_follow_integer_integer() 
  : tut::schema_basic(
      rule("document", RA("doc"), Vector<rule_t>() <<
        rule("follow", RA("foll"), Vector<rule_t>() <<
          rule("node", RA("int1", true, new I())) <<
          rule("node", RA("int2", true, new I()))))) {}
};
}

namespace tut {
typedef test_group<schema_follow_integer_integer> tf;
typedef tf::object object;
}

namespace {
  tut::tf follow_integer_integer_test("schema::follow_integer_integer");
}

namespace tut {
template<> template<>
void object::test<1>(int) {
  run_test("4\n 5");
  stru::cf::nd_list c;
  c.push_back(S(Xany(4), L"int1"));
  c.push_back(S(Xany(5), L"int2"));
  C(M(c, L"foll"), L"doc").write_to(xp);
  ensure_equals("single follow_integer_integer entity", tree, xp);
}

template<> template<>
void object::test<2>(int) {
  try {
    run_test("d7");
    fail("just string");
  } catch (schema::mismatch const &mm) {
    ensure_equals("correct error", std::string(mm.what()),
      "1:1 : mismatch in document(doc)>follow(foll)>node(int1) at token d7");
  }
}

template<> template<>
void object::test<3>(int) {
  try {
    run_test("");
    fail("empty");
  } catch (schema::mismatch const &mm) {
    ensure_equals("correct error", std::string(mm.what()), 
     "0:1 : mismatch in document(doc)>follow(foll)>node(int1) after token ");
  }
}

template<> template<>
void object::test<4>(int) {
  try {
    run_test("-77 foo");
    fail("followed string");
  } catch (schema::mismatch const &mm) {
    ensure_equals("correct error", std::string(mm.what()), 
     "1:5 : mismatch in document(doc)>follow(foll)>node(int2) at token foo");
  }
}

template<> template<>
void object::test<5>(int) {
  try {
    run_test("4");
    fail("just one integer");
  } catch (schema::mismatch const &mm) {
    ensure_equals("correct error", std::string(mm.what()), 
        "1:1 : mismatch in document(doc)>follow(foll) after token 4");
  }
}

template<> template<>
void object::test<6>(int) {
  try {
    run_test("4 99,y");
    fail("follows");
  } catch (schema::mismatch const &mm) {
    ensure_equals("correct error", std::string(mm.what()), 
        "1:6 : mismatch in document(doc)>follow(foll) at token y");
  }
}

template<> template<>
void object::test<7>(int) {
  no_test();
}

// further tests
}
