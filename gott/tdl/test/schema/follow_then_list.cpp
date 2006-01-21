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
using schema::slotcfg;
using schema::rule_t;
typedef schema::rule_attr_t RA;

namespace {
struct schema_follow_then_list : tut::schema_basic {
  schema_follow_then_list() 
  : tut::schema_basic(
      rule("document", RA("doc"), Vector<rule_t>() <<
        rule("follow", RA("foll"), Vector<rule_t>() <<
          rule("node", RA("s")) <<
          rule("node", 
            RA(Vector<string>() << "i",true,Xany(),new stru::repatch_integer(),
               slotcfg(), slotcfg(slotcfg::list)))))) {}
};
}

namespace tut {
typedef test_group<schema_follow_then_list> tf;
typedef tf::object object;
}

namespace {
  tut::tf follow_then_list_test("schema::follow_then_list");
}

namespace tut {
template<> template<>
void object::test<1>(int) {
  run_test("4\n 5");
  stru::cf::nd_list c;
  c.push_back(S(Xany("4"), "s"));
  c.push_back(S(Xany(5), "i"));
  C(M(c, "foll"), "doc").write_to(xp);
  ensure_equals("single follow_then_list entity", tree, xp);
}

template<> template<>
void object::test<2>(int) {
  run_test("d7");
  C(C(S(Xany("d7"), "s"), "foll"), "doc").write_to(xp);
  ensure_equals("just string", tree, xp);
}

template<> template<>
void object::test<3>(int) {
  try {
    run_test("");
    fail("empty");
  } catch (schema::mismatch const &mm) {
    ensure_equals("correct error", std::string(mm.what()), 
      "0:1 : mismatch in document(doc)>follow(foll)>node(s) after token ");
  }
}

template<> template<>
void object::test<4>(int) {
  run_test("foo -77");
  stru::cf::nd_list c;
  c.push_back(S(Xany("foo"), "s"));
  c.push_back(S(Xany(-77), "i"));
  C(M(c, "foll"), "doc").write_to(xp);
  ensure_equals("followed string", tree, xp);
}

template<> template<>
void object::test<5>(int) {
  try {
    run_test("4 99,y");
    fail("stuff");
  } catch (schema::mismatch const &mm) {
    ensure_equals("correct error", std::string(mm.what()), 
     "1:6 : mismatch in document(doc)>follow(foll) at token y");
  }
}

template<> template<>
void object::test<6>(int) {
  run_test("x 1 2 3");
  stru::cf::nd_list c;
  c.push_back(S(Xany("x"), "s"));
  c.push_back(S(Xany(1), "i"));
  c.push_back(S(Xany(2), "i"));
  c.push_back(S(Xany(3), "i"));
  C(M(c, "foll"), "doc").write_to(xp);
  ensure_equals("multiple", tree, xp);
}

template<> template<>
void object::test<7>(int) {
  no_test();
}

// further tests
}
