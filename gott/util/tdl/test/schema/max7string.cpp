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
#include <gott/util/tdl/schema/slot.hpp>

namespace schema = gott::tdl::schema;
namespace stru = gott::tdl::structure;
namespace simple = gott::tdl::simple;
using gott::xany::Xany;
using gott::string;

using stru::cf::S;
using stru::cf::C;
using stru::cf::M;
using schema::slotcfg;
using schema::rule_t;

typedef schema::rule_attr_t RA;

namespace {
struct schema_max7string : tut::schema_basic {
  schema_max7string() 
  : tut::schema_basic(
      rule("document", RA(),
        Vector<rule_t>() <<
         rule("list", RA(),
           Vector<rule_t>() <<
           rule("node",
             RA(Vector<string>() << "el", true, Xany(), 0, 
                slotcfg(), slotcfg(slotcfg::maximum, 7)))))) {}
};
}

namespace tut {
typedef test_group<schema_max7string> tf;
typedef tf::object object;
}

namespace {
  tut::tf list_int_then_string_test("schema::max7string");
}

namespace tut {
template<> template<>
void object::test<1>(int t) {
  int n = t - 1; // minimum: 0
  
  std::wostringstream w;
  for (int i = 0; i < n; ++i) {
    w << i;
    if (i < n - 1)
      w << L',';
  }
  run_test(w.str());

  stru::cf::nd_list c;
  for (int i = 0; i < n; ++i) {
    wchar_t ch[2] = {'0'+i,0};
    c.push_back(S(Xany(ch), L"el"));
  }
  C(M(c)).write_to(xp);

  ensure_equals("enough", tree, xp);
}

template<> template<>
void object::test<8>(int n) {
  try {
    std::wostringstream w;
    for (int i = 0; i < n; ++i) {
      w << i;
      if (i < n - 1)
        w << L',';
    }
    run_test(w.str());
    fail("too many");
  } catch(schema::mismatch const &m) {
    ensure_equals("correct error", m.what(), 
                  gott::string("1:15 : mismatch in document at token 7"));
  }
}

template<> template<>
void object::test<15>(int) {
  try {
    run_test(L"1 2 3");
    fail("going down");
  } catch (schema::mismatch const &m) {
    ensure_equals("correct error", gott::string(m.what()),
                  "1:1 : mismatch in document>list>node(el) after token 1");
  }
}

template<> template<>
void object::test<16>(int) {
  run_test(L"zzzz");
  C(C(S(Xany(L"zzzz"),L"el"))).write_to(xp);
  ensure_equals("string", tree, xp);
}

template<> template<>
void object::test<17>(int) {
  no_test();
}

}
