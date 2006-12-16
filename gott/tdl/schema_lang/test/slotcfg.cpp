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
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code is A C++ Library for Dealing with the TDL Language.
 *
 * The Initial Developer of the Original Code is
 * Aristid Breitkreuz (aribrei@arcor.de).
 * Portions created by the Initial Developer are Copyright (C) 2006
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

#include <testsoon.hpp>
#include <gott/tdl/schema/match.hpp>
#include <gott/tdl/schema/rule.hpp>
#include <gott/tdl/schema/slot.hpp>
#include <gott/tdl/structure/structure.hpp>
#include <gott/tdl/structure/repatchable_adapter.hpp>
#include <gott/tdl/structure/revocable_adapter.hpp>
#include <gott/tdl/exceptions.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/assign/list_of.hpp>
#include <sstream>

using namespace tdl::schema;
using gott::string;
using boost::assign::list_of;
using namespace boost::lambda;

namespace {

struct acceptor : tdl::structure::writable_structure {
  void begin(tdl::source_position const &) {}
  void end() {}
  void add_tag(string const &) {}

  void data(gott::Xany const &x) {
    rep = gott::Xany_cast<slotcfg>(x);
  }

  slotcfg rep;
};

slotcfg get(std::istream &desc) {
  acceptor acc;
  tdl::structure::repatchable_adapter2 helper3(acc);
  tdl::structure::revocable_adapter helper2(helper3);
  tdl::structure::repatchable_adapter helper(helper2);
  match m(
      rule_one("document", rule_attr(tag = "doc"),
        rule("tdl::schema_lang::slotcfg", rule_attr(tag = "S"))),
      helper);
  m.parse(desc);
  return acc.rep;
}

TEST_GROUP(good) {
  void check(
      std::istream &desc,
      slotcfg::mode mode,
      std::vector<unsigned> const &good_count,
      std::vector<unsigned> const &bad_count) {
    slotcfg cfg;
    Nothrows(cfg = get(desc), tdl::tdl_error&);
    Equals(cfg.get_mode(), mode);
    for (std::vector<unsigned>::const_iterator it = good_count.begin();
        it != good_count.end();
        ++it) {
      slotcfg cfg2 = cfg;
      for (unsigned i = 0; i < *it; ++i)
        cfg2.add();
      Check1(_1 > 0, cfg2.expectation());
    }
    for (std::vector<unsigned>::const_iterator it = bad_count.begin();
        it != bad_count.end();
        ++it) {
      slotcfg cfg2 = cfg;
      for (unsigned i = 0; i < *it; ++i)
        cfg2.add();
      Check1(_1 < 0, cfg2.expectation());
    }
  }

  void check(
      string const &desc,
      slotcfg::mode mode,
      std::vector<unsigned> const &good_count,
      std::vector<unsigned> const &bad_count) {
    std::stringstream stream;
    stream << desc;
    check(stream, mode, good_count, bad_count);
  }

  TEST(one) {
    check(":one", slotcfg::one, list_of(1), list_of(0)(2)(3));
  }

  TEST(optional) {
    check(":optional", slotcfg::optional, list_of(0)(1), list_of(2)(3));
  }

  TEST(list) {
    check(":list", slotcfg::list, list_of(0)(1)(2)(3), std::vector<unsigned>());
  }

  TEST(some) {
    check(":some", slotcfg::some, list_of(1)(2)(3), list_of(0));
  }

  TEST(exactly 5) {
    check(":exactly 5", slotcfg::exactly, list_of(5), list_of(0)(4)(6)(7));
  }

  TEST(minimum 2) {
    check(":minimum 2", slotcfg::minimum, list_of(2)(3)(4), list_of(0)(1));
  }

  TEST(maximum 2) {
    check(":maximum 2", slotcfg::maximum, list_of(0)(1)(2), list_of(3)(4));
  }

  TEST(range 2 / 3) {
    check(":range 2,3", slotcfg::range, list_of(2)(3), list_of(0)(1)(4)(5));
  }
}

TEST_GROUP(bad) {
  TEST(muhkuh) {
    std::stringstream stream;
    stream << ":muhkuh";
    Throws(get(stream), tdl::tdl_error,
      "TDL Schema matcher: failed to match schema at 1:1 (:muhkuh)");
  }
}

}
