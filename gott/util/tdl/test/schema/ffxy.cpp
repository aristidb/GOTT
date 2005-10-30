// Copyright (C) 2005 by Aristid Breitkreuz (aribrei@arcor.de)
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
#include <gott/util/tdl/structure/types/integer.hpp>

namespace schema = gott::tdl::schema;
namespace stru = gott::tdl::structure;
namespace simple = gott::tdl::simple;
using gott::xany::Xany;
using gott::string;
using schema::slotcfg;

using namespace stru::cf;
using schema::rule_t;
typedef schema::rule_attr_t RA;
typedef stru::repatch_integer I;

namespace {
struct schema_ffxy : tut::schema_basic {
  schema_ffxy() 
  : tut::schema_basic(
      rule("document", RA(), Vector<rule_t>() <<
        rule("ordered", RA(), Vector<rule_t>() <<
          rule("follow", RA(), Vector<rule_t>() <<
            rule("ordered", RA(), Vector<rule_t>() <<
              rule("follow", RA("o"), Vector<rule_t>() <<
                rule("follow", 
                  RA(Vector<string>() << "xy", true, Xany(), 0,
                     slotcfg(), slotcfg(slotcfg::list)),
                  Vector<rule_t>() <<
                    rule("node", RA("int1", true, new I())) <<
                    rule("node", RA("int2", true, new I())))) <<
              rule("node", RA())))))) {}

  void run(string prefix, int n) {
    string test_data = prefix;
    for (int i = 0; i < n; ++i)
      test_data = test_data + "0 1 ";
    run_test((std::wstring(test_data) + L"\nA").c_str());
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
  nd_list c;
  for (int i = 1; i < n; ++i)
    c.Add(M(nd_list() << S(Xany(0), "int1") << S(Xany(1), "int2"), "xy"));
  C(C(C(M(nd_list() << M(c, "o") << S(Xany("A")))))).write_to(xp);
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
