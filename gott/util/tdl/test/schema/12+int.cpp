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
#include <gott/util/tdl/schema/slot.hpp>
#include <gott/util/tdl/structure/types/integer.hpp>

namespace schema = gott::tdl::schema;
namespace stru = gott::tdl::structure;
namespace simple = gott::tdl::simple;
using gott::xany::Xany;
using gott::string;

using stru::cf::S;
using stru::cf::C;
using stru::cf::M;
using schema::slotcfg;
typedef schema::rule_attr_t RA;
using schema::rule_t;
using schema::rule;

namespace {
struct schema_12plus_int : tut::schema_basic {
  schema_12plus_int() 
  : tut::schema_basic(
      rule("document", RA(),
        Vector<rule_t>() <<
         rule("list", RA(),
           Vector<rule_t>() <<
           rule("node",
             RA(Vector<string>() << "el", true, Xany(), 
                new stru::repatch_integer(), 
                slotcfg(), slotcfg(slotcfg::minimum, 12))))))
  {}
};
}

namespace tut {
typedef test_group<schema_12plus_int> tf;
typedef tf::object object;
}

namespace {
  tut::tf list_int_then_string_test("schema::12+int");
}

namespace tut {
template<> template<>
void object::test<1>(int n) {
  try {
    std::wostringstream w;
    for (int i = 1; i < n; ++i) {
      w << i;
      if (i < n - 1)
        w << L',';
    }
    run_test(w.str());
    fail("too few");
  } catch(schema::mismatch const &m) {
    std::stringstream err;
    if (n == 1)
      err << "0:1";
    else
      err << "1:" << ((n-1)*2-1);
    err << " : mismatch in document>list>node(el) after token ";
    if (n > 1)
      err << (n - 1);
    ensure_equals("correct error", m.what(), err.str());
  }
}

template<> template<>
void object::test<12>(int) {
  try {
    run_test(L"1 2 3");
    fail("going down");
  } catch (schema::mismatch const &m) {
    ensure_equals("correct error", std::string(m.what()),
                  "1:1 : mismatch in document>list>node(el) after token 1");
  }
}

template<> template<>
void object::test<13>(int) {
  try {
    run_test(L"zzzz");
    fail("string");
  } catch (schema::mismatch const &m) {
    ensure_equals("correct error", std::string(m.what()),
                  "1:1 : mismatch in document>list>node(el) at token zzzz");
  }
}

template<> template<>
void object::test<14>(int t) {
  int n = t - 2; // minimum: 12
  
  std::wostringstream w;
  for (int i = 0; i < n; ++i) {
    w << i;
    if (i < n - 1)
      w << L',';
  }
  run_test(w.str());

  stru::cf::nd_list c;
  for (int i = 0; i < n; ++i)
    c.push_back(S(Xany(i), L"el"));
  C(M(c)).write_to(xp);

  ensure_equals("enough", tree, xp);
}

template<> template<>
void object::test<22>(int) {
  no_test();
}

}
