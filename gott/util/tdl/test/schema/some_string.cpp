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
using stru::cf::M;
using schema::slotcfg;

typedef schema::rule::attributes RA;

namespace {
struct schema_some_string : tut::schema_basic {
  schema_some_string() {
    context.begin(schema::match_document::factory::index());
      context.begin(schema::match_list::factory::index());
        context.begin(schema::match_string::factory::index(), 
                      RA(wstring(L"el")), slotcfg(slotcfg::some));
        context.end();
      context.end();
    context.end();
  }
};
}

namespace tut {
typedef test_group<schema_some_string> tf;
typedef tf::object object;
}

namespace {
  tut::tf list_int_then_string_test("schema::some_string");
}

namespace tut {
template<> template<>
void object::test<1>() {
  run_test(L"aa\nbb\ncc");
  stru::cf::nd_list c;
  c.push_back(S(Xany(L"aa"), L"el"));
  c.push_back(S(Xany(L"bb"), L"el"));
  c.push_back(S(Xany(L"cc"), L"el"));
  C(M(c)).write_to(xp);
  ensure_equals("three strings", tree, xp);
}

template<> template<>
void object::test<2>() {
  try {
    run_test(L"");
    fail("empty");
  } catch (schema::mismatch const &m) {
    ensure_equals("correct error", std::string(m.what()), 
                  "0:1 : mismatch after token ");
  }
}

template<> template<>
void object::test<3>() {
  try {
    run_test(L"1 2 3");
    fail("going down");
  } catch (schema::mismatch const &m) {
    ensure_equals("correct error", std::string(m.what()),
                  "1:1 : mismatch after token 1");
  }
}

template<> template<>
void object::test<4>() {
  run_test(L"zzzz");
  C(C(S(Xany(L"zzzz"), L"el"))).write_to(xp);
  ensure_equals("one string", tree, xp);
}

template<> template<>
void object::test<5>() {
  no_test();
}

// further tests
}
