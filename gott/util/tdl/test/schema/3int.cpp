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
struct schema_3int : tut::schema_basic {
  schema_3int() {
    context.begin(schema::match_document::factory::index());
      context.begin(schema::match_list::factory::index());
        context.begin(schema::match_integer::factory::index(), 
                      RA(wstring(L"el")), slotcfg(slotcfg::exact, 3));
        context.end();
      context.end();
    context.end();
  }
};
}

namespace tut {
typedef test_group<schema_3int> tf;
typedef tf::object object;
}

namespace {
  tut::tf list_int_then_string_test("schema::3int");
}

namespace tut {
template<> template<>
void object::test<1>() {
  run_test(L"77\n102342\n9");
  stru::cf::nd_list c;
  c.push_back(S(Xany(77), L"el"));
  c.push_back(S(Xany(102342), L"el"));
  c.push_back(S(Xany(9), L"el"));
  C(M(c)).write_to(xp);
  ensure_equals("three integers", tree, xp);
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
    run_test(L"1,2,3,4");
    fail("too many");
  } catch (schema::mismatch const &m) {
    ensure_equals("correct error", std::string(m.what()),
                  "1:7 : mismatch at token 4");
  }
}

// further tests
}
