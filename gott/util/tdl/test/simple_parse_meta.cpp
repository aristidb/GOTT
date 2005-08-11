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

#include <gott/util/tdl/simple/parse/parser.hpp>
#include <gott/util/tut/tut.h>
#include <sstream>
#include <gott/util/nstring/nstring.hpp>
#include <gott/util/nstring/stl.hpp>


using std::vector;
using std::pair;
using std::ostream;
using std::wistringstream;
using gott::tdl::simple::meta_parser;
using gott::tdl::simple::parse_meta;
using gott::nstring;

namespace tut {
struct meta_basic {
  meta_parser parser;
  nstring data, rest;
  vector<pair<nstring, nstring> > xp, ev;
  bool operator() (nstring const &cmd, nstring const &param) {
    ev.push_back(pair<nstring,nstring>(cmd, param));
    return true;
  }
  void expect(nstring const &cmd, nstring const &param = L"") {
    xp.push_back(pair<nstring,nstring>(cmd, param));
  }
  void run_test() {
    wistringstream x(to_wstring(data));
    parse_meta(x, parser);
    rest = x.str().c_str() + x.tellg();
  }
  meta_basic() { parser.set_default(boost::ref(*this)); }
};

typedef test_group<meta_basic> tf;
typedef tf::object object;
}

ostream &operator<<(ostream &o, vector<pair<nstring,nstring> > const &v) {
  for (vector<pair<nstring,nstring> >::const_iterator it = v.begin();
       it != v.end(); ++it) {
    o << it->first << "::";
    o << it->second << '\n';
  }
  return o;
}

namespace {
tut::tf spmeta("simple::parse_meta");
}

namespace tut {

template<> template<>
void object::test<1>(int) {
  data = L"#?foobar qulux-dei zql";
  run_test();
  expect(L"foobar", L"qulux-dei zql");
  ensure_equals("simple meta declaration", ev, xp);
}

template<> template<>
void object::test<2>(int) {
  data = L"\n\n\n\n#?real     kluft\n\n\n\n\n   \n       \n\n#?delta_x yz";
  run_test();
  expect(L"real", L"kluft");
  ensure_equals("multi-line declaration", ev, xp);
  ensure_equals("multi-line declaration rest", rest, 
                nstring(L"   \n       \n\n#?delta_x yz"));
}

template<> template<>
void object::test<3>(int) {
  data = L"#?a\n#? b\n#\n#?c";
  run_test();
  expect(L"a");
  expect(L"", L"b");
  ensure_equals("multi-line #2", ev, xp);
  ensure_equals("multi-line #2 rest", rest, nstring(L"#\n#?c"));
}

template<> template<>
void object::test<4>(int) {
  no_test();
}

// further tests
}
