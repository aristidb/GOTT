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

using namespace boost::assign;

namespace schema = gott::tdl::schema;
namespace stru = gott::tdl::structure;
namespace simple = gott::tdl::simple;
using gott::xany::Xany;
using gott::string;
using schema::slotcfg;

using namespace stru::cf;
using schema::rule;
using schema::rule_one;
typedef schema::rule_attr_t RA;
typedef stru::repatch_integer I;

namespace {
struct schema_ffxy : tut::schema_basic {
  schema_ffxy() 
  : tut::schema_basic(
      rule_one("document",
        rule_one("ordered",
          rule_one("follow",
            rule("ordered", RA(), list_of
              (rule_one("follow", RA("o"),
                rule("follow", 
                  RA(std::vector<string>(1, "xy"), true, Xany(), 0,
                     slotcfg(), slotcfg(slotcfg::list)),
                  list_of
                    (rule("node", RA("int1", true, new I())))
                    (rule("node", RA("int2", true, new I()))))))
              (rule("node"))))))) {}

  void run(string prefix, int n) {
    string test_data = prefix;
    for (int i = 0; i < n; ++i)
      test_data = test_data + "0 1 ";
    run_test(test_data + "\nA");
  }
};
}

namespace tut {
typedef test_group<schema_ffxy> tf;
typedef tf::object object;
}

namespace {
  tut::tf ffxy_test("schema::ffxy");
}

namespace tut {

template<> template<>
void object::test<1>(int n) { // test #1: no int, test #2: two int...
  run("", n - 1);
  nd_list ii;
  ii.push_back(S(Xany(0), "int1"));
  ii.push_back(S(Xany(1), "int2"));

  nd_list c;
  for (int i = 1; i < n; ++i)
    c.push_back(M(ii, "xy"));

  nd_list o;
  o.push_back(M(c, "o"));
  o.push_back(S(Xany("A")));

  C(C(C(M(o)))).write_to(xp);
  ensure_equals(tree, xp);
}

template<> template<>
void object::test<10>(int n) {
  try {
    run("55 ", n - 10);
    fail("odd");
  } catch (schema::mismatch const &mm) {
    return;
  }
}

template<> template<>
void object::test<20>(int) {
  no_test();
}

}
