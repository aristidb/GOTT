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
 * WITHOUT WARRANTY OF ANY KIND, either egroup_fixture.xpress or implied. See the License
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

namespace schema = tdl::schema;
namespace stru = tdl::structure;
using gott::xany::Xany;

using namespace stru::cf;

using schema::rule_attr;
using schema::rule_t;

namespace {
struct schema_named_string : schema_basic {
  schema_named_string() 
  : schema_basic(
      rule_one("document", rule_attr(schema:: tag = "doc"),
        rule_one("named", rule_attr(schema::tag = "ND"),
          rule("node", rule_attr(schema::tag = "S"))))) {}
};

typedef schema_named_string group_fixture_t;
}

GFTEST(single follow_integer_integer entity) {
  group_fixture.run_test("ND\n zz");
  C(MD(Xany(), nd_list(1, S(Xany("zz"), "S")), "ND"), "doc")
  .write_to(group_fixture.xp);
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

GFTEST(non-followed string) {
  try {
    group_fixture.run_test("ND,foo");
    Check(false);
  } catch (tdl::tdl_error const &m) {
    Equals(m.module(), "TDL Schema matcher");
  }
}

GFTEST(just one integer) {
  try {
    group_fixture.run_test("4");
    Check(false);
  } catch (tdl::tdl_error const &m) {
    Equals(m.module(), "TDL Schema matcher");
  }
}

GFTEST(nonsense) {
  try {
    group_fixture.run_test("4 99,y");
    Check(false);
  } catch (tdl::tdl_error const &m) {
    Equals(m.module(), "TDL Schema matcher");
  }
}

GFTEST(just label) {
  try {
    group_fixture.run_test("ND");
    Check(false);
  } catch (tdl::tdl_error const &m) {
    Equals(m.module(), "TDL Schema matcher");
  }
}

GFTEST(wrong label) {
  try {
    group_fixture.run_test("mD\n zz");
    Check(false);
  } catch (tdl::tdl_error const &m) {
    Equals(m.module(), "TDL Schema matcher");
  }
}

