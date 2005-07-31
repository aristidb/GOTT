// Copyright (C) 2004-2005 by Aristid Breitkreuz (aribrei@arcor.de)
// Content: TDL Testing
// Authors: Aristid Breitkreuz
//
// This File is part of the Gott Project (http://gott.sf.net)
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

#include <gott/util/tdl/structure/structure.hpp>
#include <gott/util/tdl/structure/tree.hpp>
#include <gott/util/tdl/structure/comfort.hpp>
#include <gott/util/tdl/structure/print.hpp>
#include <gott/util/tut/tut.h>

namespace util = gott::util;
namespace stru = util::tdl::structure;
namespace cf = stru::cf;
using gott::xany::Xany;

namespace tut {
struct structure_basic {

};

typedef test_group<structure_basic> tf;
typedef tf::object object;
}

namespace {
tut::tf stru("structure");
}

namespace tut {
template<> template<>
void object::test<1>(int) {
  stru::tree p1, p2;
  stru::writable_structure &w = p1;
  w.begin();
    w.data(Xany("hallo"));
  w.end();
  p1.copy_to(p2);
  ensure_equals("simple copy", p1, p2);
}

template<> template<>
void object::test<2>(int) {
  stru::tree p1, p2;
  stru::writable_structure &w = p2;
  cf::S(Xany("hallo")).write_to(p1);
  w.begin();
    w.data(Xany("hallo"));
  w.end();
  ensure_equals("comfort vs simple", p1, p2);
}

template<> template<>
void object::test<3>(int) {
  stru::tree p1, p2;
  stru::revocable_structure &w = p1;
  w.begin();
    w.begin();
      stru::revocable_structure::pth p = w.point();
      w.data(Xany(8899.2));
      w.begin();
        w.add_tag(L"--!");
      w.revert(p);
      w.data(Xany("77"));
    w.end();
  w.end();
  cf::C(cf::S(Xany("77"))).write_to(p2);
  ensure_equals("reverting", p1, p2);
}

template<> template<>
void object::test<4>(int) {
  stru::tree p1, p2;
  stru::writable_structure &w = p1;
  w.begin();
    w.begin();
      w.data(Xany(8899.2));
      w.begin();
        w.add_tag(L"--!");
        w.data(Xany("77"));
      w.end();
    w.end();
  w.end();
  cf::C(cf::S(Xany("77"))).write_to(p2);
  ensure("unequal", p1 != p2);
}

template<> template<>
void object::test<5>(int) {
  stru::tree p1;
  stru::writable_structure &w = p1;
  w.begin();
    w.begin();
      w.data(Xany(8899.2));
      w.begin();
        w.add_tag(L"--!");
        w.data(Xany("77"));
      w.end();
    w.end();
  w.end();

  ensure_equals("stringified tree", stru::to_string<wchar_t>(p1),
      L"-\n    8899.2\n        77 : --!");
}

template<> template<>
void object::test<6>(int) {
  no_test();
}

// further tests
}
