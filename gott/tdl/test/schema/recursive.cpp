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
#include <gott/tdl/schema/slot.hpp>
#include <gott/tdl/structure/repatchers/enumeration.hpp>
#include <gott/tdl/structure/repatchers/integer.hpp>

namespace schema = gott::tdl::schema;
namespace stru = gott::tdl::structure;
namespace simple = gott::tdl::simple;
using gott::xany::Xany;
using gott::string;

using namespace stru::cf;
using schema::slotcfg;
using schema::rule_t;
using schema::rule_attr_t;

namespace {
struct recursive : tut::schema_basic {
  rule_t rec;
  
  recursive() {
    grammar = rule("document", rule_attr_t(), Vector<rule_t>() << rule_t(&rec));
    rec = 
      rule("ordered", rule_attr_t(rule_attr_t::simple, false), Vector<rule_t>() <<
        rule("node", rule_attr_t(rule_attr_t::simple, true, 
            new stru::repatch_integer())) <<
        rule("list", rule_attr_t(rule_attr_t::simple, false), Vector<rule_t>() <<
          rule("ordered", 
                rule_attr_t(Vector<string>(), false, Xany(), 0,
                  slotcfg(), slotcfg(slotcfg::optional)),
                Vector<rule_t>() << rule_t(&rec))));
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
  std::ostringstream data;
  for (int i = 0; i < n; ++i)
    data << i << '\n';
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
