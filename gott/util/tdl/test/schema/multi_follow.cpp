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
#include <gott/util/tdl/structure/types/enumeration.hpp>

namespace schema = gott::tdl::schema;
namespace stru = gott::tdl::structure;
namespace simple = gott::tdl::simple;
using gott::xany::Xany;
using namespace stru::cf;

typedef schema::rule_attr RA;
typedef stru::repatch_enumeration E;
typedef Vector<gott::nstring> VN;

namespace {
struct schema_multi_follow : tut::schema_basic {
  schema_multi_follow() {
    schema::slotcfg o(schema::slotcfg::optional);
    context.begin("document");
      context.begin("follow", RA("outer"));
        context.begin("follow", RA("inner1"));
          context.begin("node", RA("1", true, new E(VN() << "1")), o);
          context.end();
          context.begin("node", RA("2", true, new E(VN() << "2")), o);
          context.end();
        context.end();
        context.begin("follow", RA("inner2"));
          context.begin("node", RA("3", true, new E(VN() << "3")), o);
          context.end();
          context.begin("node", RA("4", true, new E(VN() << "4")), o);
          context.end();
        context.end();
      context.end();
    context.end();
  }
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
void object::test<1>(int) {
  run_test(L"");
  C(S(Xany(), "outer")).write_to(xp);
  ensure_equals("empty", tree, xp);
}

template<> template<>
void object::test<2>(int) {
  no_test();
}

// further tests
}
