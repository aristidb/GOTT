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

namespace schema = gott::tdl::schema;
namespace stru = gott::tdl::structure;
namespace simple = gott::tdl::simple;
using gott::xany::Xany;

using stru::cf::S;
using stru::cf::C;
using schema::rule_attr;
using schema::repatcher; // tag
using schema::tag; // another tag
typedef schema::rule_attr_t RA;

namespace {
struct schema_integer : tut::schema_basic {
  schema_integer() 
  : tut::schema_basic(
      rule_one("document", 
        rule("node", rule_attr(tag = "i", repatcher = 
            new stru::repatch_integer()))))
  {}
};
}

namespace tut {
typedef test_group<schema_integer> tf;
typedef tf::object object;
}

namespace {
  tut::tf integer_test("schema::integer");
}

namespace tut {
template<> template<>
void object::test<1>(int) {
  run_test("4");
  C(S(Xany(4), L"i")).write_to(xp);
  ensure_equals("single integer entity", tree, xp);
}

template<> template<>
void object::test<2>(int) {
  try {
    run_test("d7");
    fail("non-integral");
  } catch (schema::mismatch const &mm) {
    ensure_equals("correct error", 
      std::string(mm.what()), "1:1 : mismatch in document>node(i) at token d7");
  }
}

template<> template<>
void object::test<3>(int) {
  try {
    run_test("");
    fail("empty");
  } catch (schema::mismatch const &mm) {
    ensure_equals("correct error", 
     std::string(mm.what()), "0:1 : mismatch in document>node(i) after token ");
  }
}

template<> template<>
void object::test<4>(int) {
  try {
    run_test("4 bar");
    fail("overfilled #1");
  } catch (schema::mismatch const &mm) {
    ensure_equals("correct error", 
        std::string(mm.what()), "1:1 : mismatch in document after token 4");
  }
}

template<> template<>
void object::test<5>(int) {
  try {
    run_test("4\nbar");
    fail("overfilled #2");
  } catch (schema::mismatch const &mm) {
    ensure_equals("correct error", 
        std::string(mm.what()), "2:1 : mismatch in document at token bar");
  }
}

template<> template<>
void object::test<6>(int) {
  run_test("-777777");
  C(S(Xany(-777777), L"i")).write_to(xp);
  ensure_equals("negative", tree, xp);
}

template<> template<>
void object::test<7>(int) {
  try {
    run_test("foo bar");
    fail("overfilled #1");
  } catch (schema::mismatch const &mm) {
    ensure_equals("correct error", 
     std::string(mm.what()), "1:1 : mismatch in document>node(i) at token foo");
  }
}

template<> template<>
void object::test<8>(int) {
  no_test();
}

// further tests
}
