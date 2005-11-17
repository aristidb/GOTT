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

#include <gott/tdl/simple/parse/meta.hpp>
#include <gott/tut/tut.h>
#include <sstream>
#include <gott/string/string.hpp>
#include <gott/string/stl.hpp>

using std::pair;
using std::ostream;
using std::wistringstream;
using gott::tdl::simple::meta_parser;
using gott::string;

typedef pair<string, string> twostring;
NTL_MOVEABLE(twostring);

namespace tut {
struct meta_basic {
  meta_parser parser;
  string data, rest;
  Vector<pair<string, string> > xp, ev;
  bool operator() (string const &cmd, string const &param) {
    ev.push_back(pair<string,string>(cmd, param));
    return true;
  }
  void expect(string const &cmd, string const &param = L"") {
    xp.push_back(pair<string,string>(cmd, param));
  }
  void run_test() {
    wistringstream x(data);
    parser.parse(x);
    rest = x.str().c_str() + x.tellg();
  }
  meta_basic() { parser.set_default(boost::ref(*this)); }
};

typedef test_group<meta_basic> tf;
typedef tf::object object;
}

ostream &operator<<(ostream &o, pair<string,string> const &x) {
  return o << x.first << "::" << x.second << '\n';
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
  ensure_equals("simple meta declaration", range(ev), range(xp));
}

template<> template<>
void object::test<2>(int) {
  data = L"\n\n\n\n#?real     kluft\n\n\n\n\n   \n       \n\n#?delta_x yz";
  run_test();
  expect(L"real", L"kluft");
  ensure_equals("multi-line declaration", range(ev), range(xp));
  ensure_equals("multi-line declaration rest", rest, 
                string(L"   \n       \n\n#?delta_x yz"));
}

template<> template<>
void object::test<3>(int) {
  data = L"#?a\n#? b\n#\n#?c";
  run_test();
  expect(L"a");
  expect(L"", L"b");
  ensure_equals("multi-line #2", range(ev), range(xp));
  ensure_equals("multi-line #2 rest", rest, string(L"#\n#?c"));
}

template<> template<>
void object::test<4>(int) {
  no_test();
}

// further tests
}