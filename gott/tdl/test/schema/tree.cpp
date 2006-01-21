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
#include <boost/assign/list_of.hpp>

namespace schema = gott::tdl::schema;
namespace stru = gott::tdl::structure;
namespace simple = gott::tdl::simple;
using gott::xany::Xany;

using namespace stru::cf;
using schema::rule_attr;
using schema::rule;

namespace {
struct schema_tree : tut::schema_basic {
  schema_tree() 
  : tut::schema_basic(rule_one("document", rule("tree")))
  {}
};
}

namespace tut {
typedef test_group<schema_tree> tf;
typedef tf::object object;
}

namespace {
  tut::tf integer_test("schema::tree");
}

namespace tut {
template<> template<>
void object::test<1>(int) {
  using namespace boost::assign;

  run_test("hallo du,sau du dummes schwein");
  C(MD(Xany("hallo"), list_of
        (S(Xany("du")))
        (MD(Xany("sau"), list_of
          (MD(Xany("du"), list_of
            (MD(Xany("dummes"), list_of
              (S(Xany("schwein"))))))))))).write_to(xp);
  ensure_equals(tree, xp);
}

template<> template<>
void object::test<2>(int) {
  no_test();
}

// further tests
}
