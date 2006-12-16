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

stru::repatcher *foo_r() {
  boost::scoped_ptr<stru::repatcher_getter> g(stru::repatcher_by_name());
  g->begin();
    g->data(Xany("enumeration"));
    g->begin();
      g->begin(); g->data(Xany("foo")); g->end();
    g->end();
  g->end();
  return g->result_alloc();
}

struct schema_unordered_foo_integer_string : schema_basic {
  schema_unordered_foo_integer_string() 
  : schema_basic(
      rule_one("document",
        rule("unordered", rule_attr(), boost::assign::list_of
          (rule("node", 
                rule_attr("foo", repatcher = foo_r())))
          (rule("node",
                rule_attr(repatcher = int_r())))
          (rule("node", rule_attr()))))) {}
};

typedef schema_unordered_foo_integer_string group_fixture_t;
}

GFTEST(foo/int/string) {
  group_fixture.run_test("foo\n4\nx");
  stru::cf::nd_list c;
  c.push_back(S(Xany(0), "foo"));
  c.push_back(S(Xany(4)));
  c.push_back(S(Xany("x")));
  C(M(c)).write_to(group_fixture.xp);
  Equals(group_fixture.tree, group_fixture.xp);
}

GFTEST(just string) {
  try {
    group_fixture.run_test("d7");
    Check(false);
  } catch (tdl::tdl_error const &m) {
    Equals(m.module(), "TDL Schema matcher");
  }
}

GFTEST(empty) {
  try {
    group_fixture.run_test("");
    Check(false);
  } catch (tdl::tdl_error const &m) {
    Equals(m.module(), "TDL Schema matcher");
  }
}

GFTEST(string following string) {
  try {
    group_fixture.run_test("foo bar");
    Check(false);
  } catch (tdl::tdl_error const &m) {
    Equals(m.module(), "TDL Schema matcher");
  }
}

GFTEST(just foo) {
  try {
    group_fixture.run_test("foo");
    Check(false);
  } catch (tdl::tdl_error const &m) {
    Equals(m.module(), "TDL Schema matcher");
  }
}

GFTEST() {
  try {
    group_fixture.run_test("4,x,y");
    Check(false);
  } catch (tdl::tdl_error const &m) {
    Equals(m.module(), "TDL Schema matcher");
  }
}

GFTEST(string following integer) {
  try {
    group_fixture.run_test("732 bar");
    Check(false);
  } catch (tdl::tdl_error const &m) {
    Equals(m.module(), "TDL Schema matcher");
  }
}

GFTEST(reordered #1) {
  group_fixture.run_test("77,foo,foo");
  stru::cf::nd_list c;
  c.push_back(S(Xany(77)));
  c.push_back(S(Xany(0), "foo"));
  c.push_back(S(Xany("foo")));
  C(M(c)).write_to(group_fixture.xp);
  Equals(group_fixture.tree, group_fixture.xp);
}

GFTEST(reordered #2) {
  group_fixture.run_test("hallo\n-4,foo");
  stru::cf::nd_list c;
  c.push_back(S(Xany("hallo")));
  c.push_back(S(Xany(-4)));
  c.push_back(S(Xany(0), "foo"));
  C(M(c)).write_to(group_fixture.xp);
  Equals(group_fixture.tree, group_fixture.xp);
}

