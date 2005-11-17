// Copyright (C) 2004-2005 by Aristid Breitkreuz (aribrei@arcor.de)
// Content: TDL Testing
// Authors: Aristid Breitkreuz
//
// This file is part of the Gott Project (http://gott.sf.net)
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

#include "common.hpp"
#include <gott/tdl/structure/types/integer.hpp>

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
      rule("document", rule_attr(), Vector<schema::rule_t>() <<
        rule("node", rule_attr(tag = "i", repatcher = new stru::repatch_integer()))))
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
  run_test(L"4");
  C(S(Xany(4), L"i")).write_to(xp);
  ensure_equals("single integer entity", tree, xp);
}

template<> template<>
void object::test<2>(int) {
  try {
    run_test(L"d7");
    fail("non-integral");
  } catch (schema::mismatch const &mm) {
    ensure_equals("correct error", 
      std::string(mm.what()), "1:1 : mismatch in document>node(i) at token d7");
  }
}

template<> template<>
void object::test<3>(int) {
  try {
    run_test(L"");
    fail("empty");
  } catch (schema::mismatch const &mm) {
    ensure_equals("correct error", 
     std::string(mm.what()), "0:1 : mismatch in document>node(i) after token ");
  }
}

template<> template<>
void object::test<4>(int) {
  try {
    run_test(L"4 bar");
    fail("overfilled #1");
  } catch (schema::mismatch const &mm) {
    ensure_equals("correct error", 
        std::string(mm.what()), "1:1 : mismatch in document after token 4");
  }
}

template<> template<>
void object::test<5>(int) {
  try {
    run_test(L"4\nbar");
    fail("overfilled #2");
  } catch (schema::mismatch const &mm) {
    ensure_equals("correct error", 
        std::string(mm.what()), "2:1 : mismatch in document at token bar");
  }
}

template<> template<>
void object::test<6>(int) {
  run_test(L"-777777");
  C(S(Xany(-777777), L"i")).write_to(xp);
  ensure_equals("negative", tree, xp);
}

template<> template<>
void object::test<7>(int) {
  try {
    run_test(L"foo bar");
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
