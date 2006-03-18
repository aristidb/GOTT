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
#include <boost/assign/list_of.hpp>

using namespace tdl::schema;
namespace stru = tdl::structure;
using gott::xany::Xany;
using gott::string;
using stru::cf::S;
using stru::cf::C;

namespace {
struct schema_multi_footype : tut::schema_basic {
  schema_multi_footype() 
  : tut::schema_basic(
    rule_one("document",
      rule("ordered", rule_attr_t(), boost::assign::list_of
        (rule_one("list", rule_attr_t("s"),
          rule_one("list", 
            rule_attr_t(std::vector<string>(1, "t"), true, Xany(), 0,
              slotcfg(), slotcfg(slotcfg::list)),
            rule("node",
              rule_attr_t(std::vector<string>(1, "ii"), true, Xany(), 
                new stru::repatch_integer(),
                slotcfg(), slotcfg(slotcfg::list))))))
        (rule("node", rule_attr_t("xx")))))) {}
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
  } catch (tdl::tdl_error const &m) {
    ensure_equals(m.module(), "TDL Schema matcher");
  }
}

template<> template<>
void object::test<5>(int) {
  try {
    run_test("44");
    fail("should be greedy");
  } catch (tdl::tdl_error const &m) {
    ensure_equals(m.module(), "TDL Schema matcher");
  }
}

template<> template<>
void object::test<6>(int) {
  no_test();
}

// further tests
}
