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
struct schema_max7string : tut::schema_basic {
  schema_max7string() 
  : tut::schema_basic(
      rule_one("document", 
        rule_one("ordered",
           rule("node",
             RA(std::vector<string>(1, "el"), true, Xany(), 0, 
                slotcfg(), slotcfg(slotcfg::maximum, 7)))))) {}
};
}

namespace tut {
typedef test_group<schema_max7string> tf;
typedef tf::object object;
}

namespace {
  tut::tf list_int_then_string_test("schema::max7string");
}

namespace tut {
template<> template<>
void object::test<1>(int t) {
  int n = t - 1; // minimum: 0
  
  std::ostringstream w;
  for (int i = 0; i < n; ++i) {
    w << i;
    if (i < n - 1)
      w << ',';
  }
  run_test(w.str());

  stru::cf::nd_list c;
  for (int i = 0; i < n; ++i) {
    wchar_t ch[2] = {'0'+i,0};
    c.push_back(S(Xany(ch), "el"));
  }
  C(M(c)).write_to(xp);

  ensure_equals("enough", tree, xp);
}

template<> template<>
void object::test<8>(int n) {
  try {
    std::ostringstream w;
    for (int i = 0; i < n; ++i) {
      w << i;
      if (i < n - 1)
        w << ',';
    }
    run_test(w.str());
    fail("too many");
  } catch(schema::mismatch const &m) {
    ensure_equals("correct error", m.what(), 
                  gott::string("1:15 : mismatch in document at token 7"));
  }
}

template<> template<>
void object::test<15>(int) {
  try {
    run_test("1 2 3");
    fail("going down");
  } catch (schema::mismatch const &m) {
    ensure_equals("correct error", gott::string(m.what()),
                  "1:1 : mismatch in document>ordered>list>node(el) after token 1");
  }
}

template<> template<>
void object::test<16>(int) {
  run_test("zzzz");
  C(C(S(Xany("zzzz"),"el"))).write_to(xp);
  ensure_equals("string", tree, xp);
}

template<> template<>
void object::test<17>(int) {
  no_test();
}

}
