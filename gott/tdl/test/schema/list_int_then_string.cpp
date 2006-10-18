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
#include <gott/string/string.hpp>
#include <boost/assign/list_of.hpp>

using namespace boost::assign;
using namespace tdl::schema;

namespace stru = tdl::structure;
using gott::xany::Xany;

using stru::cf::S;
using stru::cf::C;
using stru::cf::M;

namespace {
stru::repatcher *int_r() {
  boost::scoped_ptr<stru::repatcher_getter> g(stru::repatcher_by_name());
  g->begin();
    g->data(Xany("integer"));
    g->begin(); g->end();
  g->end();
  return g->result_alloc();
}

struct schema_list_int_then_string : schema_basic {
  schema_list_int_then_string() 
  : schema_basic(
      rule_one("document", rule_attr(),
        rule("ordered", rule_attr("ord"), list_of
          (rule_one("list", rule_attr("list"),
            rule("node", 
              rule_attr("int",
                repatcher = int_r(),
                outer = slotcfg(slotcfg::list)))))
          (rule("node", rule_attr("string")))))) {}
};

typedef schema_list_int_then_string group_fixture_t;
}

GFTEST(int -> string) {
  group_fixture.run_test("4\nx");
  stru::cf::nd_list c;
  c.push_back(C(S(Xany(4), "int"), "list"));
  c.push_back(S(Xany("x"), "string"));
  C(M(c, "ord")).write_to(group_fixture.xp);
  Equals(group_fixture.tree, group_fixture.xp);
}

GFTEST(just string) {
  group_fixture.run_test("d7");
  stru::cf::nd_list c;
  c.push_back(S(Xany(), "list"));
  c.push_back(S(Xany("d7"), "string"));
  C(M(c, "ord")).write_to(group_fixture.xp);
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

GFTEST(follow) {
  try {
    group_fixture.run_test("foo bar");
    Check(false);
  } catch (tdl::tdl_error const &m) {
    Equals(m.module(), "TDL Schema matcher");
  }
}

GFTEST(just int) {
  try {
    group_fixture.run_test("4");
    Check(false);
  } catch (tdl::tdl_error const &m) {
    Equals(m.module(), "TDL Schema matcher");
  }
}

GFTEST(too many strings) {
  try {
    group_fixture.run_test("4,x,y");
    Check(false);
  } catch (tdl::tdl_error const &m) {
    Equals(m.module(), "TDL Schema matcher");
  }
}

GFTEST(many thingies) {
  group_fixture.run_test("4\n\n5\n6,7\n(zzz doink)");
  stru::cf::nd_list ll;
  for (int i = 4; i <= 7; ++i)
    ll.push_back(S(Xany(i), "int"));
  stru::cf::nd_list c;
  c.push_back(M(ll, "list"));
  c.push_back(S(Xany("(zzz doink)"), "string"));
  C(M(c, "ord")).write_to(group_fixture.xp);
  Equals(group_fixture.tree, group_fixture.xp);
}

