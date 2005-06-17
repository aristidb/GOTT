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
#include <gott/util/tdl/schema/types/enumeration.hpp>

namespace u = gott::util;
namespace schema = u::tdl::schema;
namespace stru = u::tdl::structure;
namespace simple = u::tdl::simple;
using u::xany::Xany;
using std::vector;
using std::wstring;
using stru::cf::S;
using stru::cf::C;

namespace {
struct schema_enumeration : tut::schema_basic {
  schema_enumeration() {
    context.begin(L"document", schema::rule::attributes(wstring(L"doc")));
      vector<wstring> v(3);
      v[0] = L"first"; v[1] = L"second"; v[2] = L"third";
      context.begin(L"enumeration", schema::match_enumeration::attributes(v));
      context.end();
    context.end();
  }
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
  run_test(L"first");
  C(S(Xany(0)), L"doc").write_to(xp);
  ensure_equals("first", tree, xp);
}

template<> template<>
void object::test<2>(int) {
  run_test(L"second");
  C(S(Xany(1)), L"doc").write_to(xp);
  ensure_equals("second", tree, xp);
}

template<> template<>
void object::test<3>(int) {
  run_test(L"third");
  C(S(Xany(2)), L"doc").write_to(xp);
  ensure_equals("third", tree, xp);
}

template<> template<>
void object::test<4>(int) {
  try {
    run_test(L"d7");
    fail("out-of");
  } catch (schema::mismatch const &mm) {
    ensure_equals("correct error", 
        std::string(mm.what()), "1:1 : mismatch at token d7");
  }
}

template<> template<>
void object::test<5>(int) {
  try {
    run_test(L"");
    fail("empty");
  } catch (schema::mismatch const &mm) {
    ensure_equals("correct error", 
        std::string(mm.what()), "0:1 : mismatch after token ");  
  }
}

template<> template<>
void object::test<6>(int) {
  try {
    run_test(L"second bar");
    fail("overfilled #1");
  } catch (schema::mismatch const &mm) {
    ensure_equals("correct error", 
        std::string(mm.what()), "1:1 : mismatch after token second");  
  }
}

template<> template<>
void object::test<7>(int) {
  try {
    run_test(L"foo\nbar");
    fail("overfilled #2");
  } catch (schema::mismatch const &mm) {
    ensure_equals("correct error", 
        std::string(mm.what()), "1:1 : mismatch at token foo");
  }
}

template<> template<>
void object::test<8>(int) {
  no_test();
}

// further tests
}
