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
#include <gott/tdl/schema/slot.hpp>
#include <gott/tdl/structure/repatchers/integer.hpp>

namespace schema = tdl::schema;
namespace stru = tdl::structure;
using gott::xany::Xany;
using gott::string;

using stru::cf::S;
using stru::cf::C;
using stru::cf::M;
using schema::slotcfg;
using schema::rule_t;

typedef schema::rule_attr_t RA;

namespace {
struct schema_5to8int : tut::schema_basic {
  schema_5to8int()
  : tut::schema_basic(
      rule_one("document", rule("node",
             RA(std::vector<string>(1, "el"), true, Xany(), 
                new stru::repatch_integer(), 
                slotcfg(), slotcfg(slotcfg::range, 5, 8)))))
  {}
};
}

namespace tut {
typedef test_group<schema_5to8int> tf;
typedef tf::object object;
}

namespace {
  tut::tf list_int_then_string_test("schema::5to8int");
}

namespace tut {
template<> template<>
void object::test<1>(int c) {
  try {
    std::ostringstream w;
    for (int i = 1; i < c; ++i) // 0-4
      w << (77 + i) << '\n';
    run_test(w.str());
    fail("too few");
  } catch (schema::mismatch const &m) {
    std::ostringstream err;
    err << (c-1) << ":1 : mismatch in document>list>node(el) after token ";
    if (c > 1) err << 76 + c;
    ensure_equals("correct error", m.what(), err.str());
  }
}

template<> template<>
void object::test<6>(int t) {
  std::ostringstream w;
  for (int i = 1; i < t; ++i) // 5-8
    w << '\n' << -i;
  run_test(w.str());
  stru::cf::nd_list c;
  for (int i = 1; i < t; ++i)
    c.push_back(S(Xany(-i), "el"));
  M(c).write_to(xp);
  ensure_equals("in range", tree, xp);
}

template<> template<>
void object::test<10>(int c) {
  try {
    std::ostringstream w;
    for (int i = 1; i < c; ++i) // 9-...
      w << (77 + i) << '\n';
    run_test(w.str());
    fail("too few");
  } catch (schema::mismatch const &m) {
    ensure_equals("correct error", m.what(),
                  std::string("9:1 : mismatch in document at token 86"));
  }
}

template<> template<>
void object::test<16>(int) {
  try {
    run_test("1 2 3");
    fail("going down");
  } catch (schema::mismatch const &m) {
    ensure_equals("correct error", std::string(m.what()), 
                  "1:1 : mismatch in document>list>node(el) after token 1");
  }
}

template<> template<>
void object::test<17>(int) {
  try {
    run_test("lala");
    fail("string");
  } catch (schema::mismatch const &m) {
    ensure_equals("correct error", std::string(m.what()), 
                  "1:1 : mismatch in document>list>node(el) at token lala");
  }
}

template<> template<>
void object::test<18>(int) {
  no_test();
}

// further tests
}
