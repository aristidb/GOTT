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

using gott::string;
using gott::xany::Xany;
using namespace stru::cf;
using schema::rule_t;
using schema::rule_one;

typedef schema::rule_attr_t RA;

namespace {
struct schema_any : tut::schema_basic {
  schema_any()
  : tut::schema_basic(
      rule_one("document",
         rule("any", RA(),
           list_of
           (rule("follow", RA(), 
             list_of
             (rule("node", RA("int3", true, new stru::repatch_integer())))
             (rule("node", RA("int4", true, new stru::repatch_integer())))))
           (rule("ordered", RA(),
             list_of
             (rule("node", RA("int", true, new stru::repatch_integer())))
             (rule("node", RA("string")))))
           (rule("ordered", RA(),
             list_of
             (rule("node", RA("string_1")))
             (rule("node", RA("string_2")))))
           (rule("node", RA("int2", true, new stru::repatch_integer())))
           (rule("node", RA("string2"))))))
  {}
};
}

namespace tut {
typedef test_group<schema_any> tf;
typedef tf::object object;
}

namespace {
  tut::tf test_any("schema::any");
}

namespace tut {
template<> template<>
void object::test<1>(int) {
  run_test("4\nx");
  nd_list c;
  c.push_back(S(Xany(4), "int"));
  c.push_back(S(Xany("x"), "string"));
  C(C(M(c))).write_to(xp);
  ensure_equals("single ordered_integer_string entity", tree, xp);
}

template<> template<>
void object::test<2>(int) {
  run_test("d7");
  C(C(S(Xany("d7"), "string2"))).write_to(xp);
  ensure_equals("just string", tree, xp);
}

template<> template<>
void object::test<3>(int) {
  try {
    run_test("");
    fail("empty");
  } catch (tdl::mismatch const &mm) {
    ensure_equals("correct error", string(mm.what()),
        "0:1 : mismatch in document>any>follow>node(int3) after token ");
  }
}

template<> template<>
void object::test<4>(int) {
  try {
    run_test("foo bar");
    fail("string following string");
  } catch (tdl::mismatch const &mm) {
    ensure_equals("correct error", string(mm.what()), 
      "1:1 : mismatch in document>any>ordered>node(string_2) after token foo");
  }
}

template<> template<>
void object::test<5>(int) {
  run_test("4");
  C(C(S(Xany(4), "int2"))).write_to(xp);
  ensure_equals("just integer", tree, xp);
}

template<> template<>
void object::test<6>(int) {
  try {
    run_test("4,x,y");
  } catch (tdl::mismatch const &mm) {
    ensure_equals("correct error", 
        string(mm.what()), "1:5 : mismatch in document at token y");
  }
}

template<> template<>
void object::test<7>(int) {
  try {
    run_test("732 bar");
    fail("string following integer");
  } catch (tdl::mismatch const &mm) {
    ensure_equals("correct error", string(mm.what()),
       "1:5 : mismatch in document>any>follow>node(int4) at token bar");
  }
}

template<> template<>
void object::test<8>(int) {
  run_test("xxx,yyyy");
  nd_list c;
  c.push_back(S(Xany("xxx"), "string_1"));
  c.push_back(S(Xany("yyyy"), "string_2"));
  C(C(M(c))).write_to(xp);
  ensure_equals("ordered_string_string", tree, xp);
}

template<> template<>
void object::test<9>(int) {
  run_test("4 5");
  nd_list c;
  c.push_back(S(Xany(4), "int3"));
  c.push_back(S(Xany(5), "int4"));
  C(C(M(c))).write_to(xp);
  ensure_equals("follow", tree, xp);
}

template<> template<>
void object::test<10>(int) {
  no_test();
}

// further tests
}