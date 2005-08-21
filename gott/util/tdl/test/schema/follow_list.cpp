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

using stru::cf::S;
using stru::cf::C;
using schema::slotcfg;

typedef schema::rule_attr RA;

namespace {
struct schema_follow_list : tut::schema_basic {
  schema_follow_list() {
    context.begin(L"document");
      context.begin(L"follow");
        context.begin(L"node", RA(L"i", true, new stru::repatch_integer()), 
                      slotcfg(slotcfg::list));
        context.end();
        context.begin(L"node", RA(L"s"));
        context.end();
      context.end();
    context.end();
  }
};
}

namespace tut {
typedef test_group<schema_follow_list> tf;
typedef tf::object object;
}

namespace {
  tut::tf follow_list_test("schema::follow_list");
}

namespace tut {
template<> template<>
void object::test<1>(int) {
  run_test(L"4\n 5 x");
  stru::cf::nd_list c;
  c.push_back(S(Xany(4), L"i"));
  c.push_back(S(Xany(5), L"i"));
  c.push_back(S(Xany(L"x"), L"s"));
  C(M(c)).write_to(xp);
  ensure_equals("single follow_list entity", tree, xp);
}

template<> template<>
void object::test<2>(int) {
  run_test(L"d7");
  C(C(S(Xany(L"d7"), L"s"))).write_to(xp);
  ensure_equals("just string", tree, xp);
}

template<> template<>
void object::test<3>(int) {
  try {
    run_test(L"");
    fail("empty");
  } catch (schema::mismatch const &mm) {
    ensure_equals("correct error", std::string(mm.what()), 
        "0:1 : mismatch in document>follow>node(i) after token ");
  }
}

template<> template<>
void object::test<4>(int) {
  run_test(L"-77 foo");
  stru::cf::nd_list c;
  c.push_back(S(Xany(-77), L"i"));
  c.push_back(S(Xany(L"foo"), L"s"));
  C(M(c)).write_to(xp);
  ensure_equals("followed string", tree, xp);
}

template<> template<>
void object::test<5>(int) {
  try {
    run_test(L"4");
    fail("just one integer");
  } catch (schema::mismatch const &mm) {
    ensure_equals("correct error", std::string(mm.what()), 
        "1:1 : mismatch in document>follow after token 4");
  }
}

template<> template<>
void object::test<6>(int) {
  try {
    run_test(L"4 99,y");
    fail("stuff");
  } catch (schema::mismatch const &mm) {
    ensure_equals("correct error", 
        std::string(mm.what()), "1:6 : mismatch in document>follow at token y");
  }
}

template<> template<>
void object::test<7>(int) {
  run_test(L"1 2 3 4 5 6 7 8 9 10 foo");
  stru::cf::nd_list c;
  for (int i = 1; i <= 10; ++i)
    c.Add(S(Xany(i), "i"));
  c.Add(S(Xany("foo"), "s"));
  C(M(c)).write_to(xp);
  ensure_equals(tree, xp);
}

template<> template<>
void object::test<8>(int) {
  no_test();
}

// further tests
}
