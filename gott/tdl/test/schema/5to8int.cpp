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
#include <gott/tdl/schema/slot.hpp>

using namespace tdl::schema;
namespace stru = tdl::structure;
using gott::xany::Xany;
using gott::string;

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

struct schema_5to8int : schema_basic {
  schema_5to8int()
  : schema_basic(
      rule_one("document", rule("node",
          rule_attr("el",
                repatcher = int_r(), 
                outer = slotcfg(slotcfg::range, 5, 8)))))
  {}
};

typedef schema_5to8int group_fixture_t;
}

XTEST((n, "too few") (gf, 1) (gen, (testsoon::range_generator<int>)(0)(5))) {
  try {
    std::ostringstream w;
    for (int i = 0; i < value; ++i) // 0-4
      w << (77 + i) << '\n';
    group_fixture.run_test(gott::string(w.str(), gott::ascii));
    Check(false);
  } catch (tdl::tdl_error const &m) {
    Equals(m.module(), "TDL Schema matcher");
  }
}

XTEST((n, "in range") (gf, 1) (gen, (testsoon::range_generator<int>)(5)(9))) {
  std::ostringstream w;
  for (int i = 0; i < value; ++i) // 5-8
    w << '\n' << -i;
  group_fixture.run_test(gott::string(w.str(), gott::ascii));
  stru::cf::nd_list c;
  for (int i = 0; i < value; ++i)
    c.push_back(S(Xany(-i), "el"));
  M(c).write_to(group_fixture.xp);
  Equals(group_fixture.tree, group_fixture.xp);
}

XTEST((n, "too many") (gf, 1) (gen, (testsoon::range_generator<int>)(9)(16))) {
  try {
    std::ostringstream w;
    for (int i = 0; i < value; ++i) // 9-...
      w << (77 + i) << '\n';
    group_fixture.run_test(gott::string(w.str(), gott::ascii));
    Check(false);
  } catch (tdl::tdl_error const &m) {
    Equals(m.module(), "TDL Schema matcher");
  }
}

GFTEST(going down) {
  try {
    group_fixture.run_test("1 2 3");
    Check(false);
  } catch (tdl::tdl_error const &m) {
    Equals(m.module(), "TDL Schema matcher");
  }
}

GFTEST(string) {
  try {
    group_fixture.run_test("lala");
    Check(false);
  } catch (tdl::tdl_error const &m) {
    Equals(m.module(), "TDL Schema matcher");
  }
}
