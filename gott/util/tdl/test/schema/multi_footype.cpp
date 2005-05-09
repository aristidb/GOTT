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
#include <gott/util/tdl/schema/context_template.hpp>
#include <gott/util/tdl/schema/slot.hpp>

namespace u = gott::util;
namespace schema = u::tdl::schema;
namespace stru = u::tdl::structure;
namespace simple = u::tdl::simple;
using u::xany::Xany;
using std::wstring;
using stru::cf::S;
using stru::cf::C;
using schema::slotcfg;

typedef schema::rule::attributes RA;

namespace {
struct schema_multi_footype : tut::schema_basic {
  schema::context c[2];
  schema_multi_footype() {
    schema::context_template document, footype, multi;

    document.begin(schema::match_document::factory::index(), RA(wstring(L"--doc--")));
      document.param(0);
    document.end();

    footype.begin(schema::match_named::factory::index(), schema::match_named::attributes(L"a"));
      footype.param(1);
    footype.end();

    multi.begin(schema::match_unordered::factory::index(), RA(wstring(L"--unordered--")));
      multi.begin(schema::match_named::factory::index(), schema::match_named::attributes(L"plugin"));
        multi.begin(schema::match_string::factory::index(), RA(wstring(L"plugin-data")), slotcfg(slotcfg::list));
        multi.end();
      multi.end();
      multi.begin(schema::match_named::factory::index(), schema::match_named::attributes(L"sum"));
        multi.begin(schema::match_integer::factory::index(), RA(wstring(L"sum-data"))); // > 0
        multi.end();
      multi.end();
      multi.begin(schema::match_integer::factory::index(), RA(wstring(L"--other--")) );//, slotcfg(slotcfg::some));
      multi.end();
    multi.end();

    std::vector<schema::context*> v(2);
    for (size_t i = 0; i < 2; ++i)
      v[i] = &c[i];

    multi.instantiate(v, c[1]);
    footype.instantiate(v, c[0]);
    document.instantiate(v, context);
  }
};
}

namespace tut {
typedef test_group<schema_multi_footype> tf;
typedef tf::object object;
}

namespace {
  tut::tf multi_footype_test("schema::multi_footype");
}

namespace tut {
// ACTUALLY TEST (TODO)
}
