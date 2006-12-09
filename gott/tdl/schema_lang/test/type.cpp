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
#include <gott/tdl/structure/container.hpp>
#include <gott/tdl/exceptions.hpp>
#include <boost/array.hpp>
#include <sstream>

using namespace tdl::schema;

namespace {

void match(std::istream &stream) {
  tdl::structure::container out;
  tdl::structure::repatchable_adapter2 helper3(out);
  tdl::structure::revocable_adapter helper2(helper3);
  tdl::structure::repatchable_adapter helper(helper2);
  tdl::schema::match m(
      rule_one("document", rule_attr(tag = "doc"),
        rule("tdl::schema_lang::type", rule_attr(tag = "T"))),
      helper);
  m.parse(stream);
}

TEST_GROUP(valid) {
  TEST_GROUP(simple) {
    char const *types[] = {
      "node",
      "tree"
    };
    char const **last_types = types + sizeof(types) / sizeof(types[0]);

    XTEST((generator, (std::vector<char const *>)(types)(last_types))) {
      std::stringstream stream;
      stream << value;
      Nothrows(match(stream), tdl::tdl_error&);
    }
  }
}

TEST_GROUP(invalid) {
  TEST(empty) {
    std::stringstream stream;
    Throws(match(stream), tdl::tdl_error,
      "TDL Schema matcher: failed to match schema at 0:1 ()");
  }

  TEST(shit) {
    std::stringstream stream;
    stream << "shit";
    Throws(match(stream), tdl::tdl_error,
      "TDL Schema matcher: failed to match schema at 1:1 (shit)");
  }
}

}
