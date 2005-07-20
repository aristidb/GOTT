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
using namespace u::tdl::schema;
namespace stru = u::tdl::structure;
namespace simple = u::tdl::simple;
namespace schema = u::tdl::schema;
using u::xany::Xany;
using std::wstring;
using stru::cf::S;
using stru::cf::C;

namespace {
struct schema_multi_footype : tut::schema_basic {
  schema_multi_footype() {
    context.begin(L"document");
      context.begin(L"ordered");
        context.begin(L"list", rule::attributes(wstring(L"s")));
          context.begin(L"list", rule::attributes(wstring(L"t")));
            context.begin(L"integer", rule::attributes(wstring(L"ii")));
            ;context.end();
          ;context.end();
        ;context.end();
        context.begin(L"string", rule::attributes(wstring(L"xx")));
      ;context.end();
    ;context.end();
  }
};
}

namespace tut {
typedef test_group<schema_multi_footype> tf;
typedef tf::object object;
}

namespace {
  tut::tf multi_footype_test("schema::list_list");
}

namespace tut {
template<> template<>
void object::test<1>(int) {
  run_test(L"a_string");
  stru::cf::nd_list c;
  c.push_back(S(Xany(), L"s"));
  c.push_back(S(Xany(L"a_string"), L"xx"));
  C(M(c)).write_to(xp);
  ensure_equals("string only", tree, xp);
}

template<> template<>
void object::test<2>(int) {
  run_test(L"1,2,a");
  stru::cf::nd_list i;
  i.push_back(S(Xany(1), L"ii"));
  i.push_back(S(Xany(2), L"ii"));
  stru::cf::nd_list c;
  c.push_back(C(M(i, L"t"), L"s"));
  c.push_back(S(Xany(L"a"), L"xx"));
  C(M(c)).write_to(xp);
  ensure_equals("ints+string", tree, xp);
}

template<> template<>
void object::test<3>(int) {
  run_test(L"7000\n(4)");
  stru::cf::nd_list c;
  c.push_back(C(C(S(Xany(7000),L"ii"),L"t"),L"s"));
  c.push_back(S(Xany(L"(4)"), L"xx"));
  C(M(c)).write_to(xp);
  ensure_equals("int+string", tree, xp);
}

template<> template<>
void object::test<4>(int) {
  try {
    run_test(L"");
    fail("empty");
  } catch (schema::mismatch const &m) {
    ensure_equals("correct error", std::string(m.what()),
       "0:1 : mismatch in document>ordered>list(s)>list(t)>integer(ii)"
       " after token ");
  }
}

template<> template<>
void object::test<5>(int) {
  try {
    run_test(L"44");
    fail("should be greedy");
  } catch (schema::mismatch const &m) {
    ensure_equals("correct error", std::string(m.what()),
        "1:1 : mismatch in document>ordered>list(s)>list(t)>integer(ii)"
        " after token 44");
  }
}

template<> template<>
void object::test<6>(int) {
  no_test();
}

// further tests
}
