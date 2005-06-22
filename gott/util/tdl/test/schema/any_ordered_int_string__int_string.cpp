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
struct schema_any_ordered_integer_string__integer_string : tut::schema_basic {
   schema_any_ordered_integer_string__integer_string() {
    context.begin(L"document", 
                  RA(wstring(L"doc")));
      context.begin(L"any",
                    RA(wstring(L"any")));
        context.begin(L"ordered",
                      RA(wstring(L"ord")));
          context.begin(L"integer",
                      RA(wstring(L"int")));
          context.end();
          context.begin(L"string",
                      RA(wstring(L"string")));
          context.end();
        context.end();
        context.begin(L"integer",
                      RA(wstring(L"int2")));
        context.end();
        context.begin(L"string",
                      RA(wstring(L"string2")));
        context.end();
      context.end();
    context.end();
  }
};
}

namespace tut {
typedef test_group<schema_any_ordered_integer_string__integer_string> tf;
typedef tf::object object;
}

namespace {
  tut::tf test_any_ordered_integer_string__integer_string("schema::any(ordered(integer,string),integer,string)");
}

namespace tut {
template<> template<>
void object::test<1>(int) {
  run_test(L"4\nx");
  stru::cf::nd_list c;
  c.push_back(S(Xany(4), L"int"));
  c.push_back(S(Xany(L"x"), L"string"));
  C(C(M(c, L"ord"), L"any"), L"doc").write_to(xp);
  ensure_equals("single ordered_integer_string entity", tree, xp);
}

template<> template<>
void object::test<2>(int) {
  run_test(L"d7");
  C(C(S(Xany(L"d7"), L"string2"), L"any"), L"doc").write_to(xp);
  ensure_equals("just string", tree, xp);
}

template<> template<>
void object::test<3>(int) {
  try {
    run_test(L"");
    fail("empty");
  } catch (schema::mismatch const &mm) {
    ensure_equals("correct error", 
        std::string(mm.what()), "0:1 : mismatch after token ");
  }
}

template<> template<>
void object::test<4>(int) {
  try {
    run_test(L"foo bar");
    fail("string following string");
  } catch (schema::mismatch const &mm) {
    ensure_equals("correct error", 
        std::string(mm.what()), "1:1 : mismatch after token foo");
  }
}

template<> template<>
void object::test<5>(int) {
  run_test(L"4");
  C(C(S(Xany(4), L"int2"), L"any"), L"doc").write_to(xp);
  ensure_equals("just integer", tree, xp);
}

template<> template<>
void object::test<6>(int) {
  try {
    run_test(L"4,x,y");
  } catch (schema::mismatch const &mm) {
    ensure_equals("correct error", 
        std::string(mm.what()), "1:5 : mismatch at token y");
  }
}

template<> template<>
void object::test<7>(int) {
  try {
    run_test(L"732 bar");
    fail("string following integer");
  } catch (schema::mismatch const &mm) {
    ensure_equals("correct error", 
        std::string(mm.what()), "1:1 : mismatch after token 732");
  }
}

template<> template<>
void object::test<8>(int) {
  no_test();
}

// further tests
}