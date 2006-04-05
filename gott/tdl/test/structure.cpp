/* ***** BEGIN LICENSE BLOCK *****
 * Version: MPL 1.1/GPL 2.0/LGPL 2.1
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code is A C++ Library for Dealing with the TDL Language.
 *
 * The Initial Developer of the Original Code is
 * Aristid Breitkreuz (aribrei@arcor.de).
 * Portions created by the Initial Developer are Copyright (C) 2005-2006
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *  Aristid Breitkreuz (aribrei@arcor.de)
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 2 or later (the "GPL"), or
 * the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
 * in which case the provisions of the GPL or the LGPL are applicable instead
 * of those above. If you wish to allow use of your version of this file only
 * under the terms of either the GPL or the LGPL, and not to allow others to
 * use your version of this file under the terms of the MPL, indicate your
 * decision by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL or the LGPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of any one of the MPL, the GPL or the LGPL.
 *
 * ***** END LICENSE BLOCK ***** */

#include <gott/tdl/source_position.hpp>
#include <gott/tdl/structure/structure.hpp>
#include <gott/tdl/structure/container.hpp>
#include <gott/tdl/structure/revocable_adapter.hpp>
#include <gott/tdl/structure/comfort.hpp>
#include <gott/tdl/structure/print.hpp>
#include <gott/tut/tut.h>
#include <sstream>

namespace stru = tdl::structure;
namespace cf = stru::cf;
using gott::xany::Xany;
using tdl::source_position;

namespace tut {
struct structure_basic { };

typedef test_group<structure_basic> tf;
typedef tf::object object;
}

namespace {
tut::tf stru("structure");
}

namespace tut {
template<> template<>
void object::test<1>(int) {
  stru::container p1, p2;
  stru::writable_structure &w = p1;
  w.begin(source_position());
    w.data(Xany("hallo"));
  w.end();
  p1.copy_to(p2);
  ensure_equals("simple copy", p1, p2);
}

template<> template<>
void object::test<2>(int) {
  stru::container p1, p2;
  stru::writable_structure &w = p2;
  cf::S(Xany("hallo")).write_to(p1);
  w.begin(source_position());
    w.data(Xany("hallo"));
  w.end();
  ensure_equals("comfort vs simple", p1, p2);
}

template<> template<>
void object::test<3>(int) {
  stru::container p1, p2;
  stru::revocable_adapter a(p1);
  stru::revocable_structure &w = a;
  w.begin(source_position());
    w.begin(source_position());
      stru::revocable_structure::pth p = w.point();
      w.data(Xany(8899.2));
      w.begin(source_position());
        w.add_tag("--!");
      w.revert(p);
      w.get_rid_of(p);
      w.data(Xany("77"));
    w.end();
  w.end();
  cf::C(cf::S(Xany("77"))).write_to(p2);
  ensure_equals("reverting", p1, p2);
}

template<> template<>
void object::test<4>(int) {
  stru::container p1, p2;
  stru::writable_structure &w = p1;
  w.begin(source_position());
    w.begin(source_position());
      w.data(Xany(8899.2));
      w.begin(source_position());
        w.add_tag("--!");
        w.data(Xany("77"));
      w.end();
    w.end();
  w.end();
  cf::C(cf::S(Xany("77"))).write_to(p2);
  ensure("unequal", p1 != p2);
}

template<> template<>
void object::test<5>(int) {
  stru::container p1;
  stru::writable_structure &w = p1;
  w.begin(source_position());
    w.begin(source_position());
      w.data(Xany(8899.2));
      w.begin(source_position());
        w.add_tag("--!");
        w.data(Xany("77"));
      w.end();
    w.end();
  w.end();

  std::ostringstream strm;
  stru::direct_print<char> o(strm);
  p1.copy_to(o);
  ensure_equals("stringified tree", strm.str(),
      "-\n    8899.2\n        77 : --!");
}

template<> template<>
void object::test<6>(int) {
  no_test();
}

// further tests
}
