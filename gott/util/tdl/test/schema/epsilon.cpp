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

namespace u = gott::util;
namespace schema = u::tdl::schema;
namespace stru = u::tdl::structure;
namespace simple = u::tdl::simple;
using gott::xany::Xany;
using std::wstring;
using stru::cf::S;
using stru::cf::C;
using stru::cf::M;
using schema::slotcfg;

typedef schema::rule_attr RA;

namespace {
struct schema_epsilon : tut::schema_basic {
  schema_epsilon() {
    context.begin(L"document");
      context.begin(L"list");
        context.begin(L"node", RA(), slotcfg(slotcfg::exact, 0));
        context.end();
      context.end();
    context.end();
  }
};
}

namespace tut {
typedef test_group<schema_epsilon> tf;
typedef tf::object object;
}

namespace {
  tut::tf list_int_then_string_test("schema::epsilon");
}

namespace tut {
template<> template<>
void object::test<1>(int) {
  try {
    run_test(L"77\n102342\n9");
    fail("content");
  } catch (schema::mismatch const &m) {
    ensure_equals("correct error", std::string(m.what()),
                  "1:1 : mismatch in document at token 77");
  }
}

template<> template<>
void object::test<2>(int) {
  run_test(L"");
  C(S(Xany())).write_to(xp);
  ensure_equals("empty", tree, xp);
}

template<> template<>
void object::test<3>(int) {
  try {
    run_test(L"1,2,3,4");
    fail("too many");
  } catch (schema::mismatch const &m) {
    ensure_equals("correct error", std::string(m.what()),
                  "1:1 : mismatch in document at token 1");
  }
}

template<> template<>
void object::test<4>(int) {
  try {
    run_test(L"1 2 3");
    fail("going down");
  } catch (schema::mismatch const &m) {
    ensure_equals("correct error", std::string(m.what()),
                  "1:1 : mismatch in document at token 1");
  }
}

template<> template<>
void object::test<5>(int) {
  no_test();
}

// further tests
}
