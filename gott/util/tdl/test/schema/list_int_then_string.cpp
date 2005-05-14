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
using stru::cf::M;

typedef schema::rule::attributes RA;

namespace {
struct schema_list_int_then_string : tut::schema_basic {
  schema_list_int_then_string() {
    context.begin(schema::match_document::factory::index(), 
                  RA(wstring(L"doc")));
      context.begin(schema::match_ordered::factory::index(),
                    RA(wstring(L"ord")));
        context.begin(schema::match_list::factory::index(),
                      RA(wstring(L"list")));
          context.begin(schema::match_integer::factory::index(),
                        RA(wstring(L"int")));
          context.end();
        context.end();
        context.begin(schema::match_string::factory::index(),
                    RA(wstring(L"string")));
        context.end();
      context.end();
    context.end();
  }
};
}

namespace tut {
typedef test_group<schema_list_int_then_string> tf;
typedef tf::object object;
}

namespace {
  tut::tf list_int_then_string_test("schema::list_int_then_string");
}

namespace tut {
template<> template<>
void object::test<1>() {
  run_test(L"4\nx");
  stru::cf::nd_list c;
  c.push_back(C(S(Xany(4), L"int"), L"list"));
  c.push_back(S(Xany(L"x"), L"string"));
  C(M(c, L"ord"), L"doc").write_to(xp);
  ensure_equals("single integer, then string", tree, xp);
}

template<> template<>
void object::test<2>() {
  run_test(L"d7");
  stru::cf::nd_list c;
  c.push_back(S(Xany(), L"list"));
  c.push_back(S(Xany(L"d7"), L"string"));
  C(M(c, L"ord"), L"doc").write_to(xp);
  ensure_equals("just string", tree, xp);
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
    run_test(L"foo bar");
    fail("following");
  } catch (schema::mismatch const &mm) {
    ensure_equals("correct error", 
        std::string(mm.what()), "1:1 : mismatch after token foo");
  }
}

template<> template<>
void object::test<5>() {
  try {
    run_test(L"4");
    fail("just integer");
  } catch (schema::mismatch const &mm) {
    ensure_equals("correct error", 
        std::string(mm.what()), "1:1 : mismatch after token 4");
  }
}

template<> template<>
void object::test<6>() {
  try {
    run_test(L"4,x,y");
    fail("too many strings");
  } catch (schema::mismatch const &mm) {
    ensure_equals("correct error", 
        std::string(mm.what()), "1:5 : mismatch at token y");
  }
}

template<> template<>
void object::test<7>() {
  run_test(L"4\n\n5\n6,7\n(zzz doink)");
  stru::cf::nd_list ll;
  for (int i = 4; i <= 7; ++i)
    ll.push_back(S(Xany(i), L"int"));
  stru::cf::nd_list c;
  c.push_back(M(ll, L"list"));
  c.push_back(S(Xany(L"(zzz doink)"), L"string"));
  C(M(c, L"ord"), L"doc").write_to(xp);
  ensure_equals("many thingies", tree, xp);
}

template<> template<>
void object::test<8>() {
  no_test();
}

// further tests
}
