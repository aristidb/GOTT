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

typedef schema::rule::attributes RA;

namespace {
struct schema_unordered_list_integer_string : tut::schema_basic {
  schema_unordered_list_integer_string() {
    context.begin(L"document");
      context.begin(L"unordered");
        context.begin(L"integer", RA(), schema::slotcfg(schema::slotcfg::list));
        context.end();
        context.begin(L"string");
        context.end();
      context.end();
    context.end();
  }
};
}

namespace tut {
typedef test_group<schema_unordered_list_integer_string> tf;
typedef tf::object object;
}

namespace {
  tut::tf ordered_integer_string_test("schema::unordered_list_integer_string");
}

namespace tut {
template<> template<>
void object::test<1>(int) {
  run_test(L"4\nx");
  stru::cf::nd_list c;
  c.push_back(S(Xany(4)));
  c.push_back(S(Xany(L"x")));
  C(M(c)).write_to(xp);
  ensure_equals("int then string", tree, xp);
}

template<> template<>
void object::test<2>(int) {
  run_test(L"d7");
  C(C(S(Xany(L"d7")))).write_to(xp);
  ensure_equals("single string", tree, xp);
}

template<> template<>
void object::test<3>(int) {
  try {
    run_test(L"");
    fail("empty");
  } catch (schema::mismatch const &mm) {
    ensure_equals("correct error", std::string(mm.what()), 
        "0:1 : mismatch in document>unordered>integer after token ");
  }
}

template<> template<>
void object::test<4>(int) {
  try {
    run_test(L"list bar");
    fail("string following string");
  } catch (schema::mismatch const &mm) {
    ensure_equals("correct error", std::string(mm.what()), 
        "1:1 : mismatch in document>unordered>integer after token list");
  }
}

template<> template<>
void object::test<5>(int) {
  try {
    run_test(L"list,list");
    fail("two strings");
  } catch (schema::mismatch const &mm) {
    ensure_equals("correct error", std::string(mm.what()), 
        "1:6 : mismatch in document>unordered>integer at token list");
  }
}

template<> template<>
void object::test<6>(int) {
  try {
    run_test(L"4,x,y");
    fail("int then two strings");
  } catch (schema::mismatch const &mm) {
    ensure_equals("correct error", std::string(mm.what()), 
        "1:5 : mismatch in document>unordered>integer at token y");
  }
}

template<> template<>
void object::test<7>(int) {
  try {
    run_test(L"732 bar");
    fail("string following integer");
  } catch (schema::mismatch const &mm) {
    ensure_equals("correct error", std::string(mm.what()), 
        "1:1 : mismatch in document>unordered>integer after token 732");
  }
}

template<> template<>
void object::test<8>(int) {
  run_test(L"foo,77");
  stru::cf::nd_list c;
  c.push_back(S(Xany(L"foo")));
  c.push_back(S(Xany(77)));
  C(M(c)).write_to(xp);
  ensure_equals("reordered #1", tree, xp);
}

template<> template<>
void object::test<9>(int) {
  run_test(L"1,foo,77");
  stru::cf::nd_list c;
  c.push_back(S(Xany(1)));
  c.push_back(S(Xany(L"foo")));
  c.push_back(S(Xany(77)));
  C(M(c)).write_to(xp);
  ensure_equals("reordered #2", tree, xp);
}

template<> template<>
void object::test<10>(int) {
  no_test();
}

// further tests
}
