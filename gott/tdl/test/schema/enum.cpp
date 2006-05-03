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
#include <gott/tdl/structure/repatchers/enumeration.hpp>
#include <boost/assign/list_of.hpp>

using boost::assign::list_of;
using std::vector;

namespace schema = tdl::schema;
namespace stru = tdl::structure;
using gott::string;
using gott::xany::Xany;

using stru::cf::S;
using stru::cf::C;
using schema::rule_attr_t;
using schema::rule_t;
using stru::repatch_enumeration;

namespace {
struct schema_enumeration : tut::schema_basic {
  schema_enumeration() 
  : tut::schema_basic(
    rule_one("tdl::schema::document",
      rule("tdl::schema::node", rule_attr_t(std::vector<string>(), true, Xany(),
             new repatch_enumeration(
               list_of(string("first"))(string("second"))(string("third")).
                 operator vector<string>())))))
  {}
};
}

namespace tut {
typedef test_group<schema_enumeration> tf;
typedef tf::object object;
}

namespace {
  tut::tf enumeration_test("schema::enumeration");
}

namespace tut {
template<> template<>
void object::test<1>(int) {
  run_test("first");
  C(S(Xany(0))).write_to(xp);
  ensure_equals("first", tree, xp);
}

template<> template<>
void object::test<2>(int) {
  run_test("second");
  C(S(Xany(1))).write_to(xp);
  ensure_equals("second", tree, xp);
}

template<> template<>
void object::test<3>(int) {
  run_test("third");
  C(S(Xany(2))).write_to(xp);
  ensure_equals("third", tree, xp);
}

template<> template<>
void object::test<4>(int) {
  try {
    run_test("d7");
    fail("out-of");
  } catch (tdl::tdl_error const &m) {
    ensure_equals(m.module(), "TDL Schema matcher");
  }
}

template<> template<>
void object::test<5>(int) {
  try {
    run_test("");
    fail("empty");
  } catch (tdl::tdl_error const &m) {
    ensure_equals(m.module(), "TDL Schema matcher");
  }
}

template<> template<>
void object::test<6>(int) {
  try {
    run_test("second bar");
    fail("overfilled #1");
  } catch (tdl::tdl_error const &m) {
    ensure_equals(m.module(), "TDL Schema matcher");
  }
}

template<> template<>
void object::test<7>(int) {
  try {
    run_test("foo\nbar");
    fail("overfilled #2");
  } catch (tdl::tdl_error const &m) {
    ensure_equals(m.module(), "TDL Schema matcher");
  }
}

template<> template<>
void object::test<8>(int) {
  no_test();
}

// further tests
}
