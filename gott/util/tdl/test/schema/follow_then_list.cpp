// Copyright (C) 2004-2005 by Aristid Breitkreuz (aribrei@arcor.de)
// Content: TDL Testing
// Authors: Aristid Breitkreuz
//
// This File is part of the Gott Project (http://gott.sf.net)
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

namespace u = gott::util;
namespace schema = u::tdl::schema;
namespace stru = u::tdl::structure;
namespace simple = u::tdl::simple;
using u::xany::Xany;
using std::wstring;
using stru::cf::S;
using stru::cf::C;
using schema::slotcfg;

typedef schema::rule::attributes RA;

namespace {
struct schema_follow_then_list : tut::schema_basic {
  schema_follow_then_list() {
    context.begin(L"document", RA(wstring(L"doc")));
      context.begin(L"follow", RA(wstring(L"foll")));
        context.begin(L"string", RA(wstring(L"s")));
        context.end();
        context.begin(L"integer", RA(wstring(L"i")), 
                      slotcfg(slotcfg::list));
        context.end();
      context.end();
    context.end();
  }
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
  run_test(L"4\n 5");
  stru::cf::nd_list c;
  c.push_back(S(Xany(L"4"), L"s"));
  c.push_back(S(Xany(5), L"i"));
  C(M(c, L"foll"), L"doc").write_to(xp);
  ensure_equals("single follow_then_list entity", tree, xp);
}

template<> template<>
void object::test<2>(int) {
  run_test(L"d7");
  C(C(S(Xany(L"d7"), L"s"), L"foll"), L"doc").write_to(xp);
  ensure_equals("just string", tree, xp);
}

template<> template<>
void object::test<3>(int) {
  try {
    run_test(L"");
    fail("empty");
  } catch (schema::mismatch const &mm) {
    ensure_equals("correct error", 
        std::string(mm.what()), "0:1 : mismatch in document>follow>string after token ");
  }
}

template<> template<>
void object::test<4>(int) {
  run_test(L"foo -77");
  stru::cf::nd_list c;
  c.push_back(S(Xany(L"foo"), L"s"));
  c.push_back(S(Xany(-77), L"i"));
  C(M(c, L"foll"), L"doc").write_to(xp);
  ensure_equals("followed string", tree, xp);
}

template<> template<>
void object::test<5>(int) {
  try {
    run_test(L"4 99,y");
    fail("stuff");
  } catch (schema::mismatch const &mm) {
    ensure_equals("correct error", 
        std::string(mm.what()), "1:6 : mismatch in document>follow>list>integer at token y");
  }
}

template<> template<>
void object::test<6>(int) {
  no_test();
}

// further tests
}
