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
#include <gott/tdl/schema/slot.hpp>

namespace schema = gott::tdl::schema;
namespace stru = gott::tdl::structure;
namespace simple = gott::tdl::simple;
using gott::xany::Xany;
using gott::string;

using namespace stru::cf;
using schema::slotcfg;
using schema::rule_t;

typedef schema::rule_attr_t RA;

namespace {
struct schema_list_string : tut::schema_basic {
  schema_list_string() 
  : tut::schema_basic(
      rule("document", RA(), Vector<rule_t>() <<
        rule("list", RA(), Vector<rule_t>() <<
          rule("node", 
            RA(std::vector<string>(1, "el"), true, Xany(), 0, 
               slotcfg(), slotcfg(slotcfg::list)))))) {}
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
  run_test("a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z");
  nd_list c;
  for (char ch = 'a'; ch <= 'z'; ++ch)
    c.push_back(S(Xany(std::string(1,ch)), "el"));
  C(M(c)).write_to(xp);
  ensure_equals("alphabet", tree, xp);
}

template<> template<>
void object::test<2>(int) {
  run_test("");
  C(S(Xany())).write_to(xp);
  ensure_equals("empty", tree, xp);
}

template<> template<>
void object::test<3>(int) {
  run_test("\"nene ich geh dann mal\"");
  C(C(S(Xany("nene ich geh dann mal")))).write_to(xp);
}

template<> template<>
void object::test<4>(int) {
  try {
    run_test("a b");
    fail("following");
  } catch (schema::mismatch const &mm) {
    ensure_equals("correct error", gott::string(mm.what()), 
        "1:1 : mismatch in document>list>node(el) after token a");
  }
}

template<> template<>
void object::test<5>(int) {
  no_test();
}

// further tests
}
