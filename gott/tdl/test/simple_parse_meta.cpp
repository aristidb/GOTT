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

#include <gott/tdl/parse/meta.hpp>
#include <gott/tut/tut.h>
#include <sstream>
#include <gott/string/string.hpp>
#include <vector>

using std::pair;
using std::ostream;
using std::istringstream;
using tdl::meta_parser;
using gott::string;

typedef pair<string, string> twostring;

namespace tut {
struct meta_basic {
  meta_parser parser;
  string data, rest;
  std::vector<twostring> xp, ev;
  bool operator() (string const &cmd, string const &param) {
    ev.push_back(pair<string,string>(cmd, param));
    return true;
  }
  void expect(string const &cmd, string const &param = L"") {
    xp.push_back(pair<string,string>(cmd, param));
  }
  void run_test() {
    istringstream x(data);
    parser.parse(x);
    rest = x.str().c_str() + x.tellg();
  }
  meta_basic() { parser.set_default(boost::ref(*this)); }
};

typedef test_group<meta_basic> tf;
typedef tf::object object;
}

namespace gott {
ostream &operator<<(ostream &o, pair<string,string> const &x) {
  return o << x.first << "::" << x.second << '\n';
}
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
