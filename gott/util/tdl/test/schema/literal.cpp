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
#include <gott/util/tdl/schema/types/literal.hpp>

namespace u = gott::util;
namespace schema = u::tdl::schema;
namespace stru = u::tdl::structure;
namespace simple = u::tdl::simple;
using u::xany::Xany;
using std::wstring;
using stru::cf::S;
using stru::cf::C;

namespace {
struct schema_literal : tut::schema_basic {
  schema_literal() {
    context.begin(L"document", schema::rule::attributes(wstring(L"doc")));
      context.begin(L"literal", schema::match_literal::attributes(L"foobar"));
      context.end();
    context.end();
  }
};
}

namespace tut {
typedef test_group<schema_literal> tf;
typedef tf::object object;
}

namespace {
  tut::tf literal_test("schema::literal");
}

namespace tut {
template<> template<>
void object::test<1>(int) {
  run_test(L"foobar");
  C(S(Xany(L"foobar")), L"doc").write_to(xp);
  ensure_equals("single foobar entity", tree, xp);
}

template<> template<>
void object::test<2>(int) {
  try {
    run_test(L"d7");
    fail("non-foobar");
  } catch (schema::mismatch const &mm) {
    ensure_equals("correct error", 
        std::string(mm.what()), "1:1 : mismatch in document at token d7");
  }
}

template<> template<>
void object::test<3>(int) {
  try {
    run_test(L"");
    fail("empty");
  } catch (schema::mismatch const &mm) {
    ensure_equals("correct error", 
        std::string(mm.what()), "0:1 : mismatch in document after token ");
  }
}

template<> template<>
void object::test<4>(int) {
  try {
    run_test(L"foobar bar");
    fail("overfilled #1");
  } catch (schema::mismatch const &mm) {
    ensure_equals("correct error", 
        std::string(mm.what()), "1:1 : mismatch in document after token foobar");
  }
}

template<> template<>
void object::test<5>(int) {
  try {
    run_test(L"foo\nbar");
    fail("overfilled #2");
  } catch (schema::mismatch const &mm) {
    ensure_equals("correct error", 
        std::string(mm.what()), "1:1 : mismatch in document at token foo");
  }
}

template<> template<>
void object::test<6>(int) {
  no_test();
}

// further tests
}
