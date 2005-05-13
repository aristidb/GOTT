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
struct schema_follow_integer_integer : tut::schema_basic {
  schema_follow_integer_integer() {
    context.begin(schema::match_document::factory::index(), 
                  RA(wstring(L"doc")));
      context.begin(schema::match_follow::factory::index(),
                    RA(wstring(L"foll")));
        context.begin(schema::match_integer::factory::index(),
                    RA(wstring(L"int1")));
        context.end();
        context.begin(schema::match_integer::factory::index(),
                    RA(wstring(L"int2")));
        context.end();
      context.end();
    context.end();
  }
};
}

namespace tut {
typedef test_group<schema_follow_integer_integer> tf;
typedef tf::object object;
}

namespace {
  tut::tf follow_integer_integer_test("schema::follow_integer_integer");
}

namespace tut {
template<> template<>
void object::test<1>() {
  run_test(L"4\n 5");
  stru::cf::nd_list c;
  c.push_back(S(Xany(4), L"int1"));
  c.push_back(S(Xany(5), L"int2"));
  C(M(c, L"foll"), L"doc").write_to(xp);
  ensure_equals("single follow_integer_integer entity", tree, xp);
}

template<> template<>
void object::test<2>() {
  try {
    run_test(L"d7");
    fail("just string");
  } catch (schema::mismatch const &mm) {
    ensure_equals("correct error", 
        std::string(mm.what()), "1:1 : mismatch at token d7");
  }
}

template<> template<>
void object::test<3>() {
  try {
    run_test(L"");
    fail("empty");
  } catch (schema::mismatch const &mm) {
    ensure_equals("correct error", 
        std::string(mm.what()), "0:1 : mismatch after token ");
  }
}

template<> template<>
void object::test<4>() {
  try {
//    run_test(L"-77 foo");
    fail("followed string");
  } catch (schema::mismatch const &mm) {
    ensure_equals("correct error", 
        std::string(mm.what()), "1:5 : mismatch at token foo");
  }
}

template<> template<>
void object::test<5>() {
  try {
    run_test(L"4");
    fail("just one integer");
  } catch (schema::mismatch const &mm) {
    ensure_equals("correct error", 
        std::string(mm.what()), "1:1 : mismatch after token 4");
  }
}

template<> template<>
void object::test<6>() {
  try {
    run_test(L"4 99,y");
  } catch (schema::mismatch const &mm) {
    ensure_equals("correct error", 
        std::string(mm.what()), "1:6 : mismatch at token y");
  }
}

template<> template<>
void object::test<7>() {
  no_test();
}

// further tests
}
