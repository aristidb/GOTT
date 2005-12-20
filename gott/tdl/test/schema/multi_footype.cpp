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
#include <gott/tdl/schema/types/named.hpp>
#include <gott/tdl/structure/repatchers/integer.hpp>

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
struct schema_multi_footype : tut::schema_basic {
  rule_t multi, footype;

  schema_multi_footype() 
  : tut::schema_basic(rule("document", RA("--doc--"), 
        Vector<rule_t>() << rule_t(&footype))) {
    footype = rule("named", schema::match_named::attributes("a"),
        Vector<rule_t>() << rule_t(&multi));

    multi =
      rule("unordered", RA("--unordered--"), Vector<rule_t>() <<
        rule("named", schema::match_named::attributes("plugin"), 
          Vector<rule_t>() << 
          rule("list", RA(RA::simple, false), Vector<rule_t>() <<
            rule("node", 
              RA(Vector<string>() << "plugin-data", true, Xany(), 0,
                slotcfg(), slotcfg(slotcfg::list))))) <<
        rule("named", schema::match_named::attributes("sum"),
          Vector<rule_t>() <<
          rule("list", RA(RA::simple, false), Vector<rule_t>() <<
            rule("node", 
              RA(Vector<string>() << "sum-data", true, Xany(),
                 new stru::repatch_integer(),
                 slotcfg(), slotcfg(slotcfg::some))))) <<
        rule("node", 
          RA(Vector<string>() << "--other--", true, Xany(), 
             new stru::repatch_integer(),
             slotcfg(), slotcfg(slotcfg::some))));
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
    run_test("");
    fail("empty");
  } catch (schema::mismatch const &m) {
    ensure_equals("correct error", gott::string(m.what()),
      "0:1 : mismatch in document(--doc--)>named(a)>follow>node after token ");
  }
}

template<> template<>
void object::test<2>(int) {
  run_test("a\n plugin x\n sum 7\n 77");
  nd_list c;
  c.push_back(MD(Xany(0), nd_list() << S(Xany("x"), "plugin-data"), "plugin"));
  c.push_back(MD(Xany(0), nd_list() << S(Xany(7), "sum-data"), "sum"));
  c.push_back(S(Xany(77), "--other--"));
  C(MD(Xany(0), nd_list() << M(c, "--unordered--"), "a"), "--doc--")
    .write_to(xp);
  ensure_equals("simple", tree, xp);
}

template<> template<>
void object::test<3>(int) {
  run_test(
      "a\n"
      " 4\n"
      " sum\n"
      "     -1220\n"
      " plugin\n"
      "                     x,y,z\n"
  );
  nd_list c, p;
  p.push_back(S(Xany("x"), "plugin-data"));
  p.push_back(S(Xany("y"), "plugin-data"));
  p.push_back(S(Xany("z"), "plugin-data"));

  c.push_back(S(Xany(4), "--other--"));
  c.push_back(MD(Xany(0), nd_list() << S(Xany(-1220), "sum-data"), "sum"));
  c.push_back(MD(Xany(0), p, "plugin"));
  C(MD(Xany(0), nd_list() << M(c, "--unordered--"), "a"), "--doc--")
    .write_to(xp);
  ensure_equals("reordered #1", tree, xp);
}

template<> template<>
void object::test<4>(int) {
  run_test(
      "a\n"
      " 4,7,-9\n"
      " sum\n"
      "     -1220\n"
      " plugin\n"
      "                     x,y,z\n"
  );
  nd_list c, p;
  p.push_back(S(Xany("x"), "plugin-data"));
  p.push_back(S(Xany("y"), "plugin-data"));
  p.push_back(S(Xany("z"), "plugin-data"));
  c.push_back(S(Xany(4), "--other--"));
  c.push_back(S(Xany(7), "--other--"));
  c.push_back(S(Xany(-9), "--other--"));
  c.push_back(MD(Xany(0), nd_list() << S(Xany(-1220), "sum-data"), "sum"));
  c.push_back(MD(Xany(0), p, L"plugin"));
  C(MD(Xany(0), nd_list() << M(c, "--unordered--"), "a"), "--doc--")
    .write_to(xp);
  ensure_equals("reordered #1", tree, xp);
}

template<> template<>
void object::test<5>(int) {
  run_test(
      "a\n"
      " 4,7,-9\n"
      " sum\n"
      "     -1220\n"
      " 22\n"
      " plugin\n"
      "                     x,y,z\n"
      " -1234"
  );
  nd_list c, p;
  p.push_back(S(Xany("x"), "plugin-data"));
  p.push_back(S(Xany("y"), "plugin-data"));
  p.push_back(S(Xany("z"), "plugin-data"));
  c.push_back(S(Xany(4), "--other--"));
  c.push_back(S(Xany(7), "--other--"));
  c.push_back(S(Xany(-9), "--other--"));
  c.push_back(MD(Xany(0), nd_list() << S(Xany(-1220), "sum-data"), "sum"));
  c.push_back(S(Xany(22), "--other--"));
  c.push_back(MD(Xany(0), p, "plugin"));
  c.push_back(S(Xany(-1234), "--other--"));
  C(MD(Xany(0), nd_list() << M(c, "--unordered--"), "a"), "--doc--")
    .write_to(xp);
  ensure_equals("reordered #1", tree, xp);
}

template<> template<>
void object::test<6>(int) {
  no_test();
}

// further tests
}
