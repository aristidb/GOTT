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
#include <gott/tdl/structure/repatchers/enumeration.hpp>

namespace schema = gott::tdl::schema;
namespace stru = gott::tdl::structure;
namespace simple = gott::tdl::simple;
using gott::xany::Xany;
using gott::string;
using namespace stru::cf;
using schema::rule_t;
using schema::slotcfg;
typedef schema::rule_attr_t RA;
typedef stru::repatch_enumeration E;
typedef std::vector<gott::string> VN;
#define L(i) \
  RA(Vector<string>() << #i, true, Xany(), new E(VN(1,#i)), \
     slotcfg(), slotcfg(slotcfg::optional))

namespace {

struct schema_multi_follow : tut::schema_basic {
  schema_multi_follow() 
  : tut::schema_basic(
      rule("document", RA(), Vector<rule_t>() <<
        rule("follow", RA("outer"), Vector<rule_t>() <<
          rule("follow", RA("inner1"), Vector<rule_t>() <<
            rule("node", L(1)) <<
            rule("node", L(2))) <<
          rule("follow", RA("inner2"), Vector<rule_t>() <<
            rule("node", L(3)) <<
            rule("node", L(4)))))) {}
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
  try {
  std::string out;
  unsigned mask = (i - 1) & 0xF;
  if (mask & 0x1)
    out += "1 ";
  if (mask & 0x2)
    out += "2 ";
  if (mask & 0x4)
    out += "3 ";
  if (mask & 0x8)
    out += "4 ";

  run_test(out);

  nd_list inner1;
  if (mask & 0x1)
    inner1.Add(S(Xany(0), "1"));
  if (mask & 0x2)
    inner1.Add(S(Xany(0), "2"));
  
  nd_list inner2;
  if (mask & 0x4)
    inner2.Add(S(Xany(0), "3"));
  if (mask & 0x8)
    inner2.Add(S(Xany(0), "4"));

  nd_list outer;
  outer.Add(M(inner1, "inner1"));
  outer.Add(M(inner2, "inner2"));

  C(M(outer, "outer")).write_to(xp);
  ensure_equals("masked", tree, xp);
  } catch(...) {
    std::cout << std::flush;
  }
}

template<> template<>
void object::test<17>(int) {
  no_test();
}
// further tests
}
