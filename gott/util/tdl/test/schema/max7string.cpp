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
#include <gott/util/tdl/schema/slot.hpp>

namespace u = gott::util;
namespace schema = u::tdl::schema;
namespace stru = u::tdl::structure;
namespace simple = u::tdl::simple;
using u::xany::Xany;
using std::wstring;
using stru::cf::S;
using stru::cf::C;
using stru::cf::M;
using schema::slotcfg;

typedef schema::rule::attributes RA;

namespace {
struct schema_max7string : tut::schema_basic {
  schema_max7string() {
    context.begin(schema::match_document::factory::index());
      context.begin(schema::match_list::factory::index());
        context.begin(schema::match_integer::factory::index(), 
                      RA(wstring(L"el")), slotcfg(slotcfg::maximum, 7));
        context.end();
      context.end();
    context.end();
  }
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
  for (int i = 0; i < n; ++i)
    c.push_back(S(Xany(i), L"el"));
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
                  std::string("1:15 : mismatch at token 7"));
  }
}

template<> template<>
void object::test<15>(int) {
  try {
    run_test(L"1 2 3");
    fail("going down");
  } catch (schema::mismatch const &m) {
    ensure_equals("correct error", std::string(m.what()),
                  "1:1 : mismatch after token 1");
  }
}

template<> template<>
void object::test<16>(int) {
  try {
    run_test(L"zzzz");
    fail("string");
  } catch (schema::mismatch const &m) {
    ensure_equals("correct error", std::string(m.what()),
                  "1:1 : mismatch at token zzzz");
  }
}

template<> template<>
void object::test<17>(int) {
  no_test();
}

}
