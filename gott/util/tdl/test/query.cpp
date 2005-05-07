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

#include <gott/util/tdl/query/query.hpp>
#include <gott/util/tdl/structure/structure.hpp>
#include <gott/util/tdl/structure/tree.hpp>
#include <gott/util/tdl/structure/print.hpp>
#include <gott/util/tut/tut.h>

using std::wcout;
using std::flush;
using std::endl;
using std::wstring;

namespace tdl = gott::util::tdl;
using tdl::structure::tree;
using tdl::structure::writable_structure;
using tdl::query::selection;
using gott::util::xany::Xany;
using gott::util::range;

namespace tut {
struct query_basic {
  tree t, xp;

  query_basic() {
    writable_structure &tr = t;
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
void object::test<1>() {
  selection<tree::iterator> sel(t.get_root());
}

template<> template<>
void object::test<2>() {
  selection<tree::iterator> x1(t.get_root());
  selection<tree::iterator> sel(x1.find(1));
}

void object::test<3>() {
  selection<tree::iterator> x1(t.get_root());
  selection<tree::iterator> sel(x1.find(0));
}

void object::test<4>() {
  selection<tree::iterator> x1(t.get_root());
  selection<tree::iterator> x2(x1.find(0));
  selection<tree::iterator> sel(x2.find_tag(L"bar"));

}

//further tests
}
