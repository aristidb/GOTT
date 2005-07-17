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
struct schema_optional_int : tut::schema_basic {
  schema_optional_int() {
    context.begin(L"document");
      context.begin(L"list");
        context.begin(L"integer", 
                      RA(wstring(L"el")), slotcfg(slotcfg::optional));
        context.end();
      context.end();
    context.end();
  }
};
}

namespace tut {
typedef test_group<schema_optional_int> tf;
typedef tf::object object;
}

namespace {
  tut::tf list_int_then_string_test("schema::optional_int");
}

namespace tut {
template<> template<>
void object::test<1>(int) {
  run_test(L"1\n");
  C(C(S(Xany(1), L"el"))).write_to(xp);
  ensure_equals("optional int", tree, xp);
}

template<> template<>
void object::test<2>(int) {
  run_test(L"");
  C(S(Xany())).write_to(xp);
  ensure_equals("optional int", tree, xp);
}

template<> template<>
void object::test<3>(int) {
  try {
    run_test(L"1 2 3");
    fail("going down");
  } catch (schema::mismatch const &m) {
    ensure_equals("correct error", std::string(m.what()),
                  "1:1 : mismatch in document after token 1");
  }
}

template<> template<>
void object::test<4>(int) {
  try {
    run_test(L"zzzz");
    fail("string");
  } catch (schema::mismatch const &m) {
    ensure_equals("correct error", std::string(m.what()),
                  "1:1 : mismatch in document at token zzzz");
  }
}

template<> template<>
void object::test<5>(int t) {
  int n = t - 3; // minimum: 2 elements
  std::wostringstream w;
  for (int i = 1; i <= n; ++i)
    w << i << '\n';
  try {
    run_test(w.str());
    fail("many");
  } catch(schema::mismatch const &m) {
    ensure_equals("correct error", std::string(m.what()),
                  "2:1 : mismatch in document at token 2");
  }
}

template<> template<>
void object::test<9>(int) {
  no_test();
}

}
