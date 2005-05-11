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

#include "common.hpp"

namespace u = gott::util;
using namespace u::tdl::schema;
namespace stru = u::tdl::structure;
namespace simple = u::tdl::simple;
using u::xany::Xany;
using std::wstring;
using stru::cf::S;
using stru::cf::C;

namespace {
struct schema_multi_footype : tut::schema_basic {
  schema_multi_footype() {
    context.begin(match_document::factory::index(), rule::attributes(wstring(L"d")));
      context.begin(match_ordered::factory::index(), rule::attributes(wstring(L"o")));
        context.begin(match_list::factory::index(), rule::attributes(wstring(L"s")));
          context.begin(match_list::factory::index(), rule::attributes(wstring(L"t")));
            context.begin(match_integer::factory::index(), rule::attributes(wstring(L"ii")));
            ;context.end();
          ;context.end();
        ;context.end();
        context.begin(match_string::factory::index(), rule::attributes(wstring(L"xx")));
      ;context.end();
    ;context.end();
  }
};
}

namespace tut {
typedef test_group<schema_multi_footype> tf;
typedef tf::object object;
}

namespace {
  tut::tf multi_footype_test("schema::list_list");
}

namespace tut {
template<> template<>
void object::test<1>() {
  std::cout << "\nFAKE" << std::flush;
}

// ACTUALLY TEST (TODO)
}
