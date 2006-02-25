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

#include <gott/tdl/write/writer.hpp>
#include <gott/tdl/structure/structure.hpp>
#include <gott/tdl/structure/container.hpp>
#include <gott/tdl/structure/print.hpp>
#include <gott/tut/tut.h>
#include <sstream>

namespace tut {
struct writer_test {
};

typedef test_group<writer_test> tf;
typedef tf::object object;
}

namespace {
tut::tf wri("writer");
}

namespace tut {
  void test(std::string input, std::string expected) {
    std::string new_string;
    tdl::structure::container old_interpretation;
    {
      std::istringstream i(input);
      tdl::structure::direct_struc(i, old_interpretation);
    }
    tdl::structure::container new_interpretation;
    {
      std::istringstream i(input);
      std::stringstream o;
      tdl::tdl_writer(o, 2).parse(i);
      new_string = o.str();
      tdl::structure::direct_struc(o, new_interpretation);
    }
    ensure_equals("interpretation", new_interpretation, old_interpretation);
    ensure_equals("string", new_string, expected);
  }

#define TEST(i, x, y) \
  template<> template<> void object::test<i>(int) { \
    tut::test(x, y); \
  }

TEST(1, "hallo du,sau", "hallo\n  du\n  sau")
TEST(2, "test #comment1\n  #comment2\n  foo",
      "test #comment1\n#comment2\n  foo")
TEST(3, "\"foo bar\"", "\"foo bar\"")
TEST(4, "x \"y\"\"z\"", "x\n  \"y\"\"z\"")
TEST(5, "\"x,y\"", "\"x,y\"")
TEST(6, "(x\n y,z)", "(x\n y,z)")
TEST(7, "\"(\"\n)", "\"(\"\n)")
TEST(8, "`\n  x\n  y", "`\n    x\n    y")
TEST(9, "a `\n  bar\n foo", "a\n  `\n      bar\n  foo")

// TODO: add impossible tests

template<> template<> void object::test
<10>(int) { no_test(); }


}
