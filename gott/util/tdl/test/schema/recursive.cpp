#if 0
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
#include <gott/util/tdl/schema/context_template.hpp>
#include <gott/util/tdl/schema/slot.hpp>
#include <gott/util/tdl/structure/types/enumeration.hpp>
#include <gott/util/tdl/structure/types/integer.hpp>

namespace schema = gott::tdl::schema;
namespace stru = gott::tdl::structure;
namespace simple = gott::tdl::simple;
using gott::xany::Xany;

using namespace stru::cf;
using schema::slotcfg;
using schema::rule_attr;

namespace {
struct recursive : tut::schema_basic {
  schema::context rec;
  
  recursive() {
    schema::context_template document, type;

    document.begin(L"document");
      document.param(0);
    document.end();

    type.begin(L"ordered", rule_attr(rule_attr::simple, false));
      type.begin(L"node", rule_attr(rule_attr::simple, true, 
            new stru::repatch_integer()));
      type.end();
      type.begin(L"ordered", rule_attr(rule_attr::simple, false), 
                 slotcfg(slotcfg::optional));
        type.param(0);
      type.end();
    type.end();

    Vector<schema::context*> cc;
    cc.Add(&rec);
    type.instantiate(cc, rec);
    document.instantiate(cc, context);
  }
};
}

namespace tut {
typedef test_group<recursive> tf;
typedef tf::object object;
}

namespace {
  tut::tf recursive("schema::recursive");
}

namespace tut {
template<> template<>
void object::test<1>(int n) {
  std::wostringstream data;
  for (int i = 0; i < n; ++i)
    data << i << L'\n';
  run_test(data.str());
  nd_list ndl;
  for (int i = 0; i < n; ++i)
    ndl.push_back(S(Xany(i)));
  M(ndl).write_to(xp);
  ensure_equals(tree, xp);
}

template<> template<>
void object::test<8>(int) {
  no_test();
}

// further tests
}
#endif
