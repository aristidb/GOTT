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
#include <gott/tdl/structure/repatchers/integer.hpp>


using namespace gott::tdl::schema;
namespace stru = gott::tdl::structure;
namespace simple = gott::tdl::simple;
using gott::xany::Xany;
using gott::string;
using stru::cf::S;
using stru::cf::C;

namespace {
struct schema_multi_footype : tut::schema_basic {
  schema_multi_footype() 
  : tut::schema_basic(
    rule("document", rule_attr_t(), Vector<rule_t>() <<
      rule("ordered", rule_attr_t(), Vector<rule_t>() <<
        rule("list", rule_attr_t("s"), Vector<rule_t>() <<
          rule("list", 
            rule_attr_t(std::vector<string>(1, "t"), true, Xany(), 0,
              slotcfg(), slotcfg(slotcfg::list)),
            Vector<rule_t>() <<
            rule("node",
              rule_attr_t(std::vector<string>(1, "ii"), true, Xany(), 
                new stru::repatch_integer(),
                slotcfg(), slotcfg(slotcfg::list))))) <<
        rule("node", rule_attr_t("xx"))))) {}
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
  run_test("a_string");
  stru::cf::nd_list c;
  c.push_back(S(Xany(), "s"));
  c.push_back(S(Xany("a_string"), "xx"));
  C(M(c)).write_to(xp);
  ensure_equals("string only", tree, xp);
}

template<> template<>
void object::test<2>(int) {
  run_test("1,2,a");
  stru::cf::nd_list i;
  i.push_back(S(Xany(1), "ii"));
  i.push_back(S(Xany(2), "ii"));
  stru::cf::nd_list c;
  c.push_back(C(M(i, L"t"), "s"));
  c.push_back(S(Xany(L"a"), "xx"));
  C(M(c)).write_to(xp);
  ensure_equals("ints+string", tree, xp);
}

template<> template<>
void object::test<3>(int) {
  run_test("7000\n(4)");
  stru::cf::nd_list c;
  c.push_back(C(C(S(Xany(7000),"ii"),"t"),"s"));
  c.push_back(S(Xany(L"(4)"), "xx"));
  C(M(c)).write_to(xp);
  ensure_equals("int+string", tree, xp);
}

template<> template<>
void object::test<4>(int) {
  try {
    run_test("");
    fail("empty");
  } catch (mismatch const &m) {
    ensure_equals("correct error", gott::string(m.what()),
       "0:1 : mismatch in document>ordered>list(s)>list(t)>node(ii)"
       " after token ");
  }
}

template<> template<>
void object::test<5>(int) {
  try {
    run_test("44");
    fail("should be greedy");
  } catch (mismatch const &m) {
    ensure_equals("correct error", gott::string(m.what()),
        "1:1 : mismatch in document>ordered>list(s)>list(t)>node(ii)"
        " after token 44");
  }
}

template<> template<>
void object::test<6>(int) {
  no_test();
}

// further tests
}
