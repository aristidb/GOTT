// vim:ts=2:sw=2:expandtab:autoindent:filetype=cpp:
/* ***** BEGIN LICENSE BLOCK *****
 * Version: MPL 1.1/GPL 2.0/LGPL 2.1
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code is A C++ Library for Dealing with the TDL Language.
 *
 * The Initial Developer of the Original Code is
 * Aristid Breitkreuz (aribrei@arcor.de).
 * Portions created by the Initial Developer are Copyright (C) 2004-2006
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *  Aristid Breitkreuz (aribrei@arcor.de)
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 2 or later (the "GPL"), or
 * the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
 * in which case the provisions of the GPL or the LGPL are applicable instead
 * of those above. If you wish to allow use of your version of this file only
 * under the terms of either the GPL or the LGPL, and not to allow others to
 * use your version of this file under the terms of the MPL, indicate your
 * decision by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL or the LGPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of any one of the MPL, the GPL or the LGPL.
 *
 * ***** END LICENSE BLOCK ***** */

#include "common.hpp"
#include <gott/tdl/structure/repatchers/integer.hpp>
#include <gott/string/string.hpp>
#include <boost/assign/list_of.hpp>

using namespace boost::assign;

namespace schema = tdl::schema;
namespace stru = tdl::structure;
using gott::xany::Xany;

using stru::cf::S;
using stru::cf::C;
using stru::cf::M;

typedef schema::rule_attr_t RA;
using schema::rule_t;
using schema::slotcfg;

namespace {
struct schema_list_int_then_string : tut::schema_basic {
  schema_list_int_then_string() 
  : tut::schema_basic(
      rule_one("tdl::schema::document", RA(),
        rule("tdl::schema::ordered", RA("ord"), list_of
          (rule_one("tdl::schema::list", RA("tdl::schema::list"),
            rule("tdl::schema::node", 
              RA(std::vector<gott::string>(1, "int"), true, Xany(),
                new stru::repatch_integer(),
                slotcfg(), slotcfg(slotcfg::list)))))
          (rule("tdl::schema::node", RA("string")))))) {}
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
void object::test<1>(int) {
  run_test("4\nx");
  stru::cf::nd_list c;
  c.push_back(C(S(Xany(4), "int"), "tdl::schema::list"));
  c.push_back(S(Xany("x"), "string"));
  C(M(c, "ord")).write_to(xp);
  ensure_equals("single integer, then string", tree, xp);
}

template<> template<>
void object::test<2>(int) {
  run_test("d7");
  stru::cf::nd_list c;
  c.push_back(S(Xany(), "tdl::schema::list"));
  c.push_back(S(Xany("d7"), "string"));
  C(M(c, "ord")).write_to(xp);
  ensure_equals("just string", tree, xp);
}

template<> template<>
void object::test<3>(int) {
  try {
    run_test("");
    fail("empty");
  } catch (tdl::tdl_error const &m) {
    ensure_equals(m.module(), "TDL Schema matcher");
  }
}

template<> template<>
void object::test<4>(int) {
  try {
    run_test("foo bar");
    fail("following");
  } catch (tdl::tdl_error const &m) {
    ensure_equals(m.module(), "TDL Schema matcher");
  }
}

template<> template<>
void object::test<5>(int) {
  try {
    run_test("4");
    fail("just integer");
  } catch (tdl::tdl_error const &m) {
    ensure_equals(m.module(), "TDL Schema matcher");
  }
}

template<> template<>
void object::test<6>(int) {
  try {
    run_test("4,x,y");
    fail("too many strings");
  } catch (tdl::tdl_error const &m) {
    ensure_equals(m.module(), "TDL Schema matcher");
  }
}

template<> template<>
void object::test<7>(int) {
  run_test("4\n\n5\n6,7\n(zzz doink)");
  stru::cf::nd_list ll;
  for (int i = 4; i <= 7; ++i)
    ll.push_back(S(Xany(i), "int"));
  stru::cf::nd_list c;
  c.push_back(M(ll, "tdl::schema::list"));
  c.push_back(S(Xany("(zzz doink)"), "string"));
  C(M(c, "ord")).write_to(xp);
  ensure_equals("many thingies", tree, xp);
}

template<> template<>
void object::test<8>(int) {
  no_test();
}

// further tests
}
