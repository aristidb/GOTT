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
#include <boost/assign/list_of.hpp>

using boost::assign::list_of;
using std::vector;

namespace schema = gott::tdl::schema;
namespace stru = gott::tdl::structure;
namespace simple = gott::tdl::simple;
using gott::string;
using gott::xany::Xany;

using stru::cf::S;
using stru::cf::C;
using schema::rule_attr_t;
using schema::rule_t;
using stru::repatch_enumeration;

namespace {
struct schema_enumeration : tut::schema_basic {
  schema_enumeration() 
  : tut::schema_basic(
    rule("document", rule_attr_t(), Vector<rule_t>() <<
      rule("node", rule_attr_t(std::vector<string>(), true, Xany(),
             new repatch_enumeration(
               list_of("first")("second")("third").operator vector<string>())))))
  {}
};
}

namespace tut {
typedef test_group<schema_enumeration> tf;
typedef tf::object object;
}

namespace {
  tut::tf enumeration_test("schema::enumeration");
}

namespace tut {
template<> template<>
void object::test<1>(int) {
  run_test("first");
  C(S(Xany(0))).write_to(xp);
  ensure_equals("first", tree, xp);
}

template<> template<>
void object::test<2>(int) {
  run_test("second");
  C(S(Xany(1))).write_to(xp);
  ensure_equals("second", tree, xp);
}

template<> template<>
void object::test<3>(int) {
  run_test("third");
  C(S(Xany(2))).write_to(xp);
  ensure_equals("third", tree, xp);
}

template<> template<>
void object::test<4>(int) {
  try {
    run_test("d7");
    fail("out-of");
  } catch (schema::mismatch const &mm) {
    ensure_equals("correct error", 
        std::string(mm.what()), "1:1 : mismatch in document>node at token d7");
  }
}

template<> template<>
void object::test<5>(int) {
  try {
    run_test("");
    fail("empty");
  } catch (schema::mismatch const &mm) {
    ensure_equals("correct error", 
        std::string(mm.what()), "0:1 : mismatch in document>node after token ");  
  }
}

template<> template<>
void object::test<6>(int) {
  try {
    run_test("second bar");
    fail("overfilled #1");
  } catch (schema::mismatch const &mm) {
    ensure_equals("correct error", 
        std::string(mm.what()), "1:1 : mismatch in document after token second");  
  }
}

template<> template<>
void object::test<7>(int) {
  try {
    run_test("foo\nbar");
    fail("overfilled #2");
  } catch (schema::mismatch const &mm) {
    ensure_equals("correct error", 
        std::string(mm.what()), "1:1 : mismatch in document>node at token foo");
  }
}

template<> template<>
void object::test<8>(int) {
  no_test();
}

// further tests
}
