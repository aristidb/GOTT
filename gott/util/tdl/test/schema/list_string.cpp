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

typedef schema::rule_attr RA;

namespace {
struct schema_list_string : tut::schema_basic {
  schema_list_string() {
    context.begin(L"document");
      context.begin(L"list");
        context.begin(L"node", RA(wstring(L"el")), slotcfg(slotcfg::list));
        context.end();
      context.end();
    context.end();
  }
};
}

namespace tut {
typedef test_group<schema_list_string> tf;
typedef tf::object object;
}

namespace {
  tut::tf list_int_then_string_test("schema::list_string");
}

namespace tut {
template<> template<>
void object::test<1>(int) {
  run_test(L"a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z");
  stru::cf::nd_list c;
  for (wchar_t ch = 'a'; ch <= 'z'; ++ch)
    c.push_back(S(Xany(wstring(1,ch)), L"el"));
  C(M(c)).write_to(xp);
  ensure_equals("alphabet", tree, xp);
}

template<> template<>
void object::test<2>(int) {
  run_test(L"");
  C(S(Xany())).write_to(xp);
  ensure_equals("empty", tree, xp);
}

template<> template<>
void object::test<3>(int) {
  run_test(L"\"nene ich geh dann mal\"");
  C(C(S(Xany(L"nene ich geh dann mal")))).write_to(xp);
}

template<> template<>
void object::test<4>(int) {
  try {
    run_test(L"a b");
    fail("following");
  } catch (schema::mismatch const &mm) {
    ensure_equals("correct error", std::string(mm.what()), 
        "1:1 : mismatch in document>list>node(el) after token a");
  }
}

template<> template<>
void object::test<5>(int) {
  no_test();
}

// further tests
}
