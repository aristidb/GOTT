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
#include <gott/tdl/structure/repatchers/enumeration.hpp>
#include <boost/assign/list_of.hpp>

using namespace boost::assign;

using namespace tdl::schema;
namespace stru = tdl::structure;
using gott::xany::Xany;
using gott::string;
using namespace stru::cf;
typedef stru::repatch_enumeration E;
typedef std::vector<gott::string> VN;
#define L(i) \
  rule_attr(#i, repatcher = new E(VN(1,#i)), outer = slotcfg(slotcfg::optional))

namespace {

struct schema_multi_follow : tut::schema_basic {
  schema_multi_follow() 
  : tut::schema_basic(
      rule_one("tdl::schema::document",
        rule("tdl::schema::follow", rule_attr("outer"),  list_of
          (rule("tdl::schema::follow", rule_attr("inner1"), list_of
            (rule("tdl::schema::node", L(1)))
            (rule("tdl::schema::node", L(2)))))
          (rule("tdl::schema::follow", rule_attr("inner2"), list_of
            (rule("tdl::schema::node", L(3)))
            (rule("tdl::schema::node", L(4)))))))) {}
};
}

namespace tut {
typedef test_group<schema_multi_follow> tf;
typedef tf::object object;
}

namespace {
  tut::tf multi_follow_test("schema::multi_follow");
}

namespace tut {
template<> template<>
void object::test<1>(int i) {
  gott::string out;
  unsigned mask = (i - 1) & 0xF;

  if (mask & 0x1)
    out = out + "1 ";
  if (mask & 0x2)
    out = out + "2 ";
  if (mask & 0x4)
    out = out + "3 ";
  if (mask & 0x8)
    out = out + "4 ";

  run_test(out);

  nd_list inner1;
  if (mask & 0x1)
    inner1.push_back(S(Xany(0), "1"));
  if (mask & 0x2)
    inner1.push_back(S(Xany(0), "2"));
  
  nd_list inner2;
  if (mask & 0x4)
    inner2.push_back(S(Xany(0), "3"));
  if (mask & 0x8)
    inner2.push_back(S(Xany(0), "4"));

  nd_list outer;
  outer.push_back(M(inner1, "inner1"));
  if (inner1.empty() || !inner2.empty())
    outer.push_back(M(inner2, "inner2"));

  C(M(outer, "outer")).write_to(xp);
  ensure_equals("masked", tree, xp);
}

template<> template<>
void object::test<17>(int) {
  no_test();
}
// further tests
}
