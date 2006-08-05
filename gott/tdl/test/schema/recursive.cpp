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
#include <gott/tdl/structure/repatchers/enumeration.hpp>
#include <gott/tdl/structure/repatchers/integer.hpp>
#include <boost/assign/list_of.hpp>

using namespace boost::assign;

using namespace tdl::schema;
namespace stru = tdl::structure;
using gott::xany::Xany;
using gott::string;

using namespace stru::cf;

namespace {
struct recursive : tut::schema_basic {
  rule_t rec;
  
  recursive() {
    grammar = rule_one("tdl::schema::document", rule_t(&rec));
    rec = 
      rule("tdl::schema::ordered", rule_attr(coat = false),
          list_of
        (rule("tdl::schema::node",
              rule_attr(repatcher = new stru::repatch_integer())))
        (rule_one("tdl::schema::list", rule_attr(coat = false),
           rule_one("tdl::schema::ordered", 
                rule_attr(outer = slotcfg(slotcfg::optional), coat = false),
                rule_t(&rec)))));
  }
};
}

namespace tut {
typedef test_group<recursive> tf;
typedef tf::object object;
}

namespace {
  tut::tf recursive("schema::recursive");
}

namespace tut {
template<> template<>
void object::test<1>(int n) {
  std::ostringstream data;
  for (int i = 0; i < n; ++i)
    data << i << '\n';
  run_test(gott::string(data.str(), gott::ascii));
  nd_list ndl;
  for (int i = 0; i < n; ++i)
    ndl.push_back(S(Xany(i)));
  M(ndl).write_to(xp);
  ensure_equals(tree, xp);
}

template<> template<>
void object::test<8>(int) {
  no_test();
}

// further tests
}
