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
 * WITHOUT WARrule_attrNTY OF ANY KIND, either express or implied. See the License
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
#include <boost/assign/list_of.hpp>

using namespace boost::assign;

using namespace tdl::schema;
namespace stru = tdl::structure;
using gott::xany::Xany;
using gott::string;
using namespace stru::cf;

namespace {
struct schema_multi_footype : tut::schema_basic {
  rule_t multi, footype;

  schema_multi_footype() 
  : tut::schema_basic(rule_one("document", rule_attr("--doc--"), 
        rule_t(&footype))) {
    footype = rule_one("named", rule_attr(tag = "a"),
        rule_t(&multi));

    multi =
      rule("unordered", rule_attr("--unordered--"), list_of
        (rule_one("named", rule_attr(tag = "plugin"), 
          rule_one("list", rule_attr(coat = false),
            rule("node", 
              rule_attr(
                tag = "plugin-data",
                outer = list())))))
        (rule_one("named", rule_attr(tag = "sum"),
          rule_one("list", rule_attr(coat = false),
            rule("node", 
              rule_attr(
                tag = "sum-data",
                repatcher = new stru::repatch_integer(),
                outer = some())))))
        (rule("node", 
          rule_attr("--other--", 
            repatcher = new stru::repatch_integer(),
            outer = slotcfg(slotcfg::some)))));
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
  } catch (tdl::tdl_error const &m) {
    ensure_equals(m.module(), "TDL Schema matcher");
  }
}

template<> template<>
void object::test<2>(int) {
  run_test("a\n plugin x\n sum 7\n 77");
  nd_list c;
  c.push_back(MD(Xany(), nd_list(1, S(Xany("x"), "plugin-data")), "plugin"));
  c.push_back(MD(Xany(), nd_list(1, S(Xany(7), "sum-data")), "sum"));
  c.push_back(S(Xany(77), "--other--"));
  C(MD(Xany(), nd_list(1, M(c, "--unordered--")), "a"), "--doc--")
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
  c.push_back(MD(Xany(), nd_list(1, S(Xany(-1220), "sum-data")), "sum"));
  c.push_back(MD(Xany(), p, "plugin"));
  C(MD(Xany(), nd_list(1, M(c, "--unordered--")), "a"), "--doc--")
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
  c.push_back(MD(Xany(), nd_list(1, S(Xany(-1220), "sum-data")), "sum"));
  c.push_back(MD(Xany(), p, "plugin"));
  C(MD(Xany(), nd_list(1, M(c, "--unordered--")), "a"), "--doc--")
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
  c.push_back(MD(Xany(), nd_list(1, S(Xany(-1220), "sum-data")), "sum"));
  c.push_back(S(Xany(22), "--other--"));
  c.push_back(MD(Xany(), p, "plugin"));
  c.push_back(S(Xany(-1234), "--other--"));
  C(MD(Xany(), nd_list(1, M(c, "--unordered--")), "a"), "--doc--")
    .write_to(xp);
  ensure_equals("reordered #1", tree, xp);
}

template<> template<>
void object::test<6>(int) {
  no_test();
}

// further tests
}
