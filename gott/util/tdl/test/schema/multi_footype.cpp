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
#include <gott/util/tdl/schema/context_template.hpp>
#include <gott/util/tdl/schema/slot.hpp>
#include <gott/util/tdl/schema/types/named.hpp>

namespace u = gott::util;
namespace schema = u::tdl::schema;
namespace stru = u::tdl::structure;
namespace simple = u::tdl::simple;
using u::xany::Xany;
using std::wstring;
using stru::cf::S;
using stru::cf::C;
using schema::slotcfg;

typedef schema::rule::attributes RA;

namespace {
struct schema_multi_footype : tut::schema_basic {
  schema::context c[2];
  schema_multi_footype() {
    schema::context_template document, footype, multi;

    document.begin(L"document", RA(wstring(L"--doc--")));
      document.param(0);
    document.end();

    footype.begin(L"named", schema::match_named::attributes(L"a"));
      footype.param(1);
    footype.end();

    multi.begin(L"unordered", RA(wstring(L"--unordered--")));
      multi.begin(L"named", schema::match_named::attributes(L"plugin"));
        multi.begin(L"string", RA(wstring(L"plugin-data")), 
                    slotcfg(slotcfg::list));
        multi.end();
      multi.end();
      multi.begin(L"named", schema::match_named::attributes(L"sum"));
        multi.begin(L"integer", RA(wstring(L"sum-data"))); // > 0
        multi.end();
      multi.end();
      multi.begin(L"integer", RA(wstring(L"--other--")), 
                  slotcfg(slotcfg::some));
      multi.end();
    multi.end();

    std::vector<schema::context*> v(2);
    for (size_t i = 0; i < 2; ++i)
      v[i] = &c[i];

    multi.instantiate(v, c[1]);
    footype.instantiate(v, c[0]);
    document.instantiate(v, context);
  }
};
}

namespace tut {
typedef test_group<schema_multi_footype> tf;
typedef tf::object object;
}

namespace {
  tut::tf multi_footype_test("schema::multi_footype");
}

namespace tut {
template<> template<>
void object::test<1>(int) {
  try {
    run_test(L"");
    fail("empty");
  } catch (schema::mismatch const &m) {
    ensure_equals("correct error", std::string(m.what()),
                  "0:1 : mismatch after token ");
  }
}

template<> template<>
void object::test<2>(int) {
  run_test(L"a\n plugin x\n sum 7\n 77");
  stru::cf::nd_list c;
  c.push_back(C(S(Xany(L"x"), L"plugin-data"), L"plugin"));
  c.push_back(C(S(Xany(7), L"sum-data"), L"sum"));
  c.push_back(S(Xany(77), L"--other--"));
  C(C(M(c, L"--unordered--"), L"a"), L"--doc--").write_to(xp);
  ensure_equals("simple", tree, xp);
}

template<> template<>
void object::test<3>(int) {
  run_test(
      L"a\n"
      " 4\n"
      " sum\n"
      "     -1220\n"
      " plugin\n"
      "                     x,y,z\n"
  );
  stru::cf::nd_list c, p;
  p.push_back(S(Xany(L"x"), L"plugin-data"));
  p.push_back(S(Xany(L"y"), L"plugin-data"));
  p.push_back(S(Xany(L"z"), L"plugin-data"));

  c.push_back(S(Xany(4), L"--other--"));
  c.push_back(C(S(Xany(-1220), L"sum-data"), L"sum"));
  c.push_back(M(p, L"plugin"));
  C(C(M(c, L"--unordered--"), L"a"), L"--doc--").write_to(xp);
  ensure_equals("reordered #1", tree, xp);
}

template<> template<>
void object::test<4>(int) {
  run_test(
      L"a\n"
      " 4,7,-9\n"
      " sum\n"
      "     -1220\n"
      " plugin\n"
      "                     x,y,z\n"
  );
  stru::cf::nd_list c, p;
  p.push_back(S(Xany(L"x"), L"plugin-data"));
  p.push_back(S(Xany(L"y"), L"plugin-data"));
  p.push_back(S(Xany(L"z"), L"plugin-data"));
  c.push_back(S(Xany(4), L"--other--"));
  c.push_back(S(Xany(7), L"--other--"));
  c.push_back(S(Xany(-9), L"--other--"));
  c.push_back(C(S(Xany(-1220), L"sum-data"), L"sum"));
  c.push_back(M(p, L"plugin"));
  C(C(M(c, L"--unordered--"), L"a"), L"--doc--").write_to(xp);
  ensure_equals("reordered #1", tree, xp);
}

template<> template<>
void object::test<5>(int) {
  no_test();
}

// further tests
}
