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
 * WITHOUT WARrule_attrNTY OF ANY KIND, either egroup_fixture.xpress or implied. See the License
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
#include <boost/assign/list_of.hpp>

using namespace tdl::schema;
namespace stru = tdl::structure;
using gott::xany::Xany;
using gott::string;

using stru::cf::S;
using stru::cf::C;

namespace {
stru::repatcher *int_r() {
  boost::scoped_ptr<stru::repatcher_getter> g(stru::repatcher_by_name());
  g->begin();
    g->data(Xany("integer"));
    g->begin(); g->end();
  g->end();
  return g->result_alloc();
}

struct schema_follow_then_list : schema_basic {
  schema_follow_then_list() 
  : schema_basic(
      rule_one("document", rule_attr("doc"),
        rule("follow", rule_attr("foll"), boost::assign::list_of
          (rule("node", rule_attr("s")))
          (rule("node", 
            rule_attr("i",
              repatcher = int_r(),
              outer = slotcfg(slotcfg::list))))))) {}
};

typedef schema_follow_then_list group_fixture_t;
}

GFTEST(single entity) {
  group_fixture.run_test("4\n 5");
  stru::cf::nd_list c;
  c.push_back(S(Xany("4"), "s"));
  c.push_back(S(Xany(5), "i"));
  C(M(c, "foll"), "doc").write_to(group_fixture.xp);
  Equals(group_fixture.tree, group_fixture.xp);
}

GFTEST(just string) {
  group_fixture.run_test("d7");
  C(C(S(Xany("d7"), "s"), "foll"), "doc").write_to(group_fixture.xp);
  Equals(group_fixture.tree, group_fixture.xp);
}

GFTEST(empty) {
  try {
    group_fixture.run_test("");
    Check(false);
  } catch (tdl::tdl_error const &m) {
    Equals(m.module(), "TDL Schema matcher");
  }
}

GFTEST(followed string) {
  group_fixture.run_test("foo -77");
  stru::cf::nd_list c;
  c.push_back(S(Xany("foo"), "s"));
  c.push_back(S(Xany(-77), "i"));
  C(M(c, "foll"), "doc").write_to(group_fixture.xp);
  Equals(group_fixture.tree, group_fixture.xp);
}

GFTEST() {
  try {
    group_fixture.run_test("4 99,y");
    Check(false);
  } catch (tdl::tdl_error const &m) {
    Equals(m.module(), "TDL Schema matcher");
  }
}

GFTEST(multiple) {
  group_fixture.run_test("x 1 2 3");
  stru::cf::nd_list c;
  c.push_back(S(Xany("x"), "s"));
  c.push_back(S(Xany(1), "i"));
  c.push_back(S(Xany(2), "i"));
  c.push_back(S(Xany(3), "i"));
  C(M(c, "foll"), "doc").write_to(group_fixture.xp);
  Equals(group_fixture.tree, group_fixture.xp);
}

