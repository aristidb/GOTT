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

#include <gott/tdl/query/query.hpp>
#include <gott/tdl/structure/structure.hpp>
#include <gott/tdl/structure/tree.hpp>
#include <gott/tdl/structure/print.hpp>
#include <gott/tdl/structure/comfort.hpp>
#include <gott/tut/tut.h>
#include <gott/range.hpp>

using std::flush;
using std::endl;

namespace tdl = gott::tdl;
using tdl::structure::tree;
using tdl::structure::writable_structure;
using tdl::query::selection;
using gott::xany::Xany;
using gott::range;
using gott::const_range;
namespace cf = tdl::structure::cf;

namespace tut {
struct query_basic {
  tree tree_obj, xp;

  query_basic() {
    writable_structure &tr = tree_obj;
    tr.begin();
      tr.data(Xany(L"root"));
      tr.begin();
        tr.data(Xany(L"first"));
        tr.begin();
          tr.data(Xany(L"foo"));
          tr.add_tag(L"foo");
        tr.end();
        tr.begin();
          tr.data(Xany(L"bar"));
          tr.add_tag(L"bar");
        tr.end();
      tr.end();
      tr.begin();
        tr.data(Xany(L"qoo"));
      tr.end();
    tr.end();
  }
};

typedef test_group<query_basic> tf;
typedef tf::object object;
}

namespace {
tut::tf stru("query");
}

namespace tut {
template<> template<>
void object::test<1>(int) {
  selection<tree::iterator> sel(tree_obj.get_root());
  tree_obj.copy_to(xp);
  ensure_equals("root", sel, selection<tree::iterator>(xp.get_root()));
}

template<> template<>
void object::test<2>(int) {
  selection<tree::iterator> x1(tree_obj.get_root());
  selection<tree::iterator> sel(x1.find(1));
  cf::S(Xany(L"qoo")).write_to(xp);
  ensure_equals("root[1]", sel, selection<tree::iterator>(xp.get_root()));
}

void object::test<3>(int) {
  selection<tree::iterator> x1(tree_obj.get_root());
  selection<tree::iterator> sel(x1.find(0));
  cf::nd_list v;
  v.push_back(cf::S(Xany(L"foo"), L"foo"));
  v.push_back(cf::S(Xany(L"bar"), L"bar"));
  cf::MD(Xany(L"first"), v).write_to(xp);
  ensure_equals("root[0]", sel, selection<tree::iterator>(xp.get_root()));
}

void object::test<4>(int) {
  selection<tree::iterator> x1(tree_obj.get_root());
  selection<tree::iterator> x2(x1.find(0));
  selection<tree::iterator> sel(x2.find_tag(L"bar"));
  cf::S(Xany(L"bar"), L"bar").write_to(xp);
  ensure_equals("root[0][bar]", sel, selection<tree::iterator>(xp.get_root()));
}

template<> template<>
void object::test<5>(int) {
  no_test();
}

//further tests
}
