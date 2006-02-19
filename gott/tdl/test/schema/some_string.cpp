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

namespace schema = gott::tdl::schema;
namespace stru = gott::tdl::structure;
using gott::xany::Xany;
using gott::string;

using stru::cf::S;
using stru::cf::C;
using stru::cf::M;
using schema::slotcfg;
using schema::rule_t;
typedef schema::rule_attr_t RA;

namespace {
struct schema_some_string : tut::schema_basic {
  schema_some_string()
  : tut::schema_basic(
      rule_one("document", RA(),
        rule_one("list", RA(),
          rule("node", 
            RA(std::vector<string>(1, "el"), true, Xany(), 0, 
               slotcfg(), slotcfg(slotcfg::some)))))) {}
};
}

namespace tut {
typedef test_group<schema_some_string> tf;
typedef tf::object object;
}

namespace {
  tut::tf list_int_then_string_test("schema::some_string");
}

namespace tut {
template<> template<>
void object::test<1>(int) {
  run_test("aa\nbb\ncc");
  stru::cf::nd_list c;
  c.push_back(S(Xany("aa"), "el"));
  c.push_back(S(Xany("bb"), "el"));
  c.push_back(S(Xany("cc"), "el"));
  C(M(c)).write_to(xp);
  ensure_equals("three strings", tree, xp);
}

template<> template<>
void object::test<2>(int) {
  try {
    run_test("");
    fail("empty");
  } catch (schema::mismatch const &m) {
    ensure_equals("correct error", gott::string(m.what()), 
                  "0:1 : mismatch in document>list>node(el) after token ");
  }
}

template<> template<>
void object::test<3>(int) {
  try {
    run_test("1 2 3");
    fail("going down");
  } catch (schema::mismatch const &m) {
    ensure_equals("correct error", gott::string(m.what()),
                  "1:1 : mismatch in document>list>node(el) after token 1");
  }
}

template<> template<>
void object::test<4>(int) {
  run_test("zzzz");
  C(C(S(Xany("zzzz"), "el"))).write_to(xp);
  ensure_equals("one string", tree, xp);
}

template<> template<>
void object::test<5>(int t) {
  int n = t - 3; // minimum: 2 elements
  std::ostringstream w;
  for (int i = 0; i < n; ++i)
    w << char('A' + i) << '\n';
  run_test(w.str());
  stru::cf::nd_list c;
  for (int i = 0; i < n; ++i)
    c.push_back(S(Xany(std::string(1, 'A'+i)), "el"));
  C(M(c)).write_to(xp);
  ensure_equals("many", tree, xp);
}

template<> template<>
void object::test<15>(int) {
  no_test();
}

}
