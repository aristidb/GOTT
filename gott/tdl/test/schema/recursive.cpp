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

using namespace boost::assign;

using namespace tdl::schema;
namespace stru = tdl::structure;
using gott::xany::Xany;
using gott::string;

using namespace stru::cf;

namespace {
stru::repatcher *int_r() {
  boost::scoped_ptr<stru::repatcher_getter> g(stru::repatcher_by_name());
  g->begin();
    g->data(Xany("integer"));
    g->begin(); g->end();
  g->end();
  return g->result_alloc();
}

struct recursive : schema_basic {
  rule_t rec;
  
  recursive() {
    grammar = rule_one("document", rule_t(&rec));
    rec = 
      rule("ordered", rule_attr(coat = false),
          list_of
        (rule("node",
              rule_attr(repatcher = int_r())))
        (rule_one("list", rule_attr(coat = false),
           rule_one("ordered", 
                rule_attr(outer = slotcfg(slotcfg::optional), coat = false),
                rule_t(&rec)))));
  }
};

typedef recursive group_fixture_t;
}

XTEST((gf, 1)(gen, (testsoon::range_generator<int>)(1)(8))) {
  std::ostringstream data;
  for (int i = 0; i < value; ++i)
    data << i << '\n';
  group_fixture.run_test(gott::string(data.str(), gott::ascii));
  nd_list ndl;
  for (int i = 0; i < value; ++i)
    ndl.push_back(S(Xany(i)));
  M(ndl).write_to(group_fixture.xp);
  Equals(group_fixture.tree, group_fixture.xp);
}

